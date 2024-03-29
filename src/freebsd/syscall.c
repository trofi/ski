/*
 * Copyright (C) 1995-2007, Hewlett-Packard Development Company, L.P.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/sysctl.h>
#include <sys/syslimits.h>
#include <sys/time.h>
#include <sys/mount.h>
#include <sys/resource.h>
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "std.h"
#include "types.h"
#include "sim.h"
#include "state.h"
#include "simmem.h"
#include "libsrs.h"
#include "os_support.h"

#include "freebsd/machdep.h"
#include "freebsd/signal.h"
#include "freebsd/syscall.h"

#define	FBSD_PAGESIZE	8192
#define	FBSD_PAGEMASK	(FBSD_PAGESIZE-1)

/* Arbitrary... */
#define	FBSD_MMAP_START	0x6000000100000000ULL
#define	FBSD_MMAP_END	0x7FFFFFFFFFFFFFFFULL

#define	fbsd_trunc_page(x)	((ADDR)(x) & ~FBSD_PAGEMASK)
#define	fbsd_round_page(x)	fbsd_trunc_page((x) + FBSD_PAGEMASK)

BOOL pmemMap(ADDR adr, void *hostadr);

extern int fdlimit;

int _close(int);
int _open(const char *, int, int);

BOOL trace_syscalls = NO;
char *consLog = NULL;

static ADDR mmap_base = FBSD_MMAP_START;

static void *
unmap_mapped_pages(ADDR base, REG size)
{
	ADDR addr, end;
	void *res;

	/* We expect page aligned addresses here. */
	assert((base & FBSD_PAGEMASK) == 0);

	end = fbsd_round_page(base + size);
	res = pmemLookup(base);
	for (addr = base; addr < end; addr += page_size) {
		if (pmemLookup(addr) == NULL)
			continue;
		memFree(addr);
	}
	return res;
}

static ADDR
verify_unmapped_pages(ADDR base, REG size)
{
	ADDR addr, end;

	/* We expect page aligned addresses here. */
	assert((base & FBSD_PAGEMASK) == 0);

	end = fbsd_round_page(base + size);
	for (addr = base; addr < end; addr += page_size) {
		if (pmemLookup(addr) != NULL)
			return 0;
	}
	return base;
}

static ADDR
get_unmapped_pages(REG size)
{
	ADDR start;

	while (mmap_base <= FBSD_MMAP_END) {
		start = verify_unmapped_pages(mmap_base, size);
		if (start != 0) {
			mmap_base = fbsd_round_page(mmap_base + size);
			return start;
		}
		mmap_base += FBSD_PAGESIZE;
	}
	return 0ULL;
}

static int
fbsd_ioctl(int fd, unsigned long com, REG data)
{
	void *buf;
	size_t len;
	int res;

	len = IOCPARM_LEN(com);
	if (len > FBSD_PAGESIZE) {
		errno = ENOTTY;
		return -1;
	}
	if (len)
		buf = alloca(len);
	else
		buf = (void*)(intptr_t)data;
	if ((com & IOC_IN) && len)
		memBBRd(data, buf, len);
	res = ioctl(fd, com, buf);
	if ((com & IOC_OUT) && len)
		memBBWrt_alloc(data, buf, len);
	return res;
}

void doSyscall(HWORD num, REG arg0, REG arg1, REG arg2, REG arg3, REG arg4,
    REG arg5, REG arg6, REG arg7, REG *ret, REG *status)
{
	int res = -1;

	*status = 0;

	if (num == SYS___syscall) {
		num = arg0;
		arg0 = arg1;
		arg1 = arg2;
		arg2 = arg3;
		arg3 = arg4;
		arg4 = arg5;
		arg5 = arg6;
		arg6 = arg7;
	}

#if 0
	printf("syscall %d", num);
#endif

	switch (num) {
	case SYS_exit: /* 1 */
		/* void sys_exit(int rval); */
		GrWrtx(8, arg0, 0);
		progExit("program exited with status %d\n", (int)arg0);
		goto out;
	case SYS_read: { /* 3 */
		/* ssize_t read(int fd, void *buf, size_t nbyte); */
		char *buf = alloca(arg2);
		res = read(arg0, buf, arg2);
		if (res > 0)
			memBBWrt_alloc(arg1, buf, res);
		break;
	}
	case SYS_write: { /* 4 */
		/* ssize_t write(int fd, const void *buf, size_t nbyte); */
		char *buf = alloca(arg2);
		memBBRd(arg1, buf, arg2);
		res = write(arg0, buf, arg2);
		break;
	}
	case SYS_open: { /* 5 */
		/* int open(char *path, int flags, int mode); */
		char path[PATH_MAX];
		simroot(arg0, path, arg1 & O_CREAT);
		res = _open(path, arg1, arg2);
		if (res >= fdlimit) {
			_close(res);
			errno = EMFILE;
			res = -1;
		}
		break;
	}
	case SYS_close: /* 6 */
		/* int close(int fd); */
		if (arg0 >= fdlimit) {
			errno = EBADF;
			res = -1;
		} else
			res = _close(arg0);
		break;
	case SYS_unlink: { /* 10 */
		/* int unlink(char *path); */
		char path[PATH_MAX];
		simroot(arg0, path, 0);
		res = unlink(path);
		break;
	}
	case SYS_fchdir: /* 13 */
		/* int fchdir(int fd); */
		res = fchdir(arg0);
		break;
	case SYS_break: { /* 17 */
		/* int obreak(char *nsize); */
		REG brk = heapGet(0);
		if (brk <= FBSD_MMAP_END) {
			if (arg0 > brk) {
				for (brk = page_base(brk); brk < arg0;
				     brk += page_size)
					pmemLookup_p(brk);
				heapSet(0, brk);
			} else if (arg0 == 0)
				arg0 = brk;
			res = 0;
		} else {
			errno = EINVAL;
			res = -1;
		}
		break;
	}
	case SYS_getpid: /* 20 */
		/* pid_t getpid(void); */
		*ret = getpid();
		goto out;
	case SYS_getuid: /* 24 */
		/* uid_t getuid(void); */
		*ret = getuid();
		goto out;
	case SYS_geteuid: /* 25 */
		/* uid_t geteuid(void); */
		*ret = geteuid();
		goto out;
	case SYS_access: { /* 33 */
		/* int access(char *path, int flags); */
		char path[PATH_MAX];
		simroot(arg0, path, 0);
		res = access(path, arg1);
		break;
	}
	case SYS_chflags: { /* 34 */
		/* int chflags(char *path, int flags); */
		char path[PATH_MAX];
		simroot(arg0, path, 0);
		res = chflags(path, arg1);
		break;
	}
	case SYS_kill: /* 37 */
		/* int kill(int pid, int signum); */
		res = kill(arg0, arg1);
		break;
	case SYS_pipe: { /* 42 */
		/* (int,int) pipe(void); */
		int fildes[2];
		res = pipe(fildes);
		if (res == 0) {
			GrWrtx(9, (REG)fildes[1], 0);
			res = (REG)fildes[0];
		}
		break;
	}
	case SYS_getegid: /* 43 */
		/* gid_t getegid(void); */
		*ret = getegid();
		goto out;
	case SYS_getgid: /* 47 */
		/* gid_t getgid(void); */
		*ret = getgid();
		goto out;
	case SYS_ioctl: /* 54 */
		/* int ioctl(int fd, u_long com, caddr_t data); */
		res = fbsd_ioctl(arg0, arg1, arg2);
		break;
	case SYS_readlink: { /* 58 */
		/* int readlink(char *path, char *buf, int count); */
		char *buf, path[PATH_MAX];
		size_t count;
		simroot(arg0, path, 0);
		count = arg2;
		buf = alloca(count);
		res = readlink(path, buf, count);
		memBBWrt_alloc(arg1, buf, count);
		break;
	}
	case SYS_munmap: { /* 73 */
		/* int munmap(void *addr, size_t len); */
		caddr_t addr;
		ADDR reloc;
		reloc = arg0;
		arg0 = fbsd_trunc_page(arg0);
		reloc -= arg0;
		arg1 += reloc;
		addr = unmap_mapped_pages(arg0, arg1);
		res = munmap(addr, arg1);
		break;
	}
	case SYS_mprotect: { /* 74 */
		/* int mprotect(const void *addr, size_t len, int prot); */
		ADDR reloc;
		void *addr;
		reloc = arg0;
		arg0 = fbsd_trunc_page(arg0);
		arg1 += reloc - arg0;
		arg1 = fbsd_round_page(arg1);
		addr = pmemLookup(arg0);
		res = mprotect(addr, arg1, arg2);
		break;
	}
	case SYS_setitimer: { /* 83 */
		/* int setitimer(u_int which, struct itimerval *itv,
		 *     struct itimerval *oitv);
		 */
		struct itimerval itv, oitv, *itvp;
		struct itimerval64 itv64;
		itvp = (arg1 != 0) ? &itv : NULL;
		if (arg1 != 0) {
			memBBRd(arg1, (void*)&itv64, sizeof(itv64));
			ston_itimerval(&itv64, itvp);
		}
		res = setitimer((int)arg0, itvp, &oitv);
		if (arg2 != 0) {
			ntos_itimerval(&oitv, &itv64);
			memBBWrt_alloc(arg2, (void*)&itv64, sizeof(itv64));
		}
		break;
	}
	case SYS_getdtablesize: /* 89 */
		/* int getdtablesize(void); */
		res = getdtablesize() - SKI_NFDS;
		break;
	case SYS_dup2: /* 90 */
		/* int dup2(u_int from, u_int to); */
		if (arg1 >= fdlimit) {
			errno = EBADF;
			res = -1;
		} else
			res = dup2(arg0, arg1);
		break;
	case SYS_fcntl: { /* 92 */
		/* int fcntl(int fd, int cmd, long arg); */
		struct flock l;
		switch (arg1) {
		case F_GETLK:
		case F_SETLK:
		case F_SETLKW:
			memBBRd(arg2, (void*)&l, sizeof(l));
			res = fcntl(arg0, arg1, &l);
			memBBWrt_alloc(arg2, (void*)&l, sizeof(l));
			break;
		default:
			res = fcntl(arg0, arg1, arg2);
			break;
		}
		break;
	}
	case SYS_socket: /* 97 */
		/* int socket(int domain, int type, int protocol); */
		res = socket(arg0, arg1, arg2);
		break;
	case SYS_connect: { /* 98 */
		/* int connect(int s, caddr_t name, int namelen); */
		void *name;
		name = alloca(arg2);
		memBBRd(arg1, name, arg2);
		res = connect(arg0, name, arg2);
		break;
	}
	case SYS_gettimeofday: { /* 116 */
		/* int gettimeofday(struct timeval *tp,
		 *     struct timezone *tzp);
		 */
		struct timeval64 tp64;
		struct timeval tp;
		struct timezone tzp;
		res = gettimeofday(&tp, &tzp);
		ntos_timeval(&tp, &tp64);
		if (arg0 != 0)
			memBBWrt_alloc(arg0, (void*)&tp64, sizeof(tp64));
		if (arg1 != 0)
			memBBWrt_alloc(arg1, (void*)&tzp, sizeof(tzp));
		break;
	}
	case SYS_getrusage: { /* 117 */
		/* int getrusage(int who, struct rusage *rusage); */
		struct rusage64 ru64;
		struct rusage ru;
		res = getrusage(arg0, &ru);
		ntos_rusage(&ru, &ru64);
		memBBWrt_alloc(arg1, (void*)&ru64, sizeof(ru64));
		break;
	}
	case SYS_sendto: { /* 133 */
		/* int sendto(int s, caddr_t buf, size_t len, int flags,
		 *     caddr_t to, int tolen);
		 */
		void *msg, *to;
		msg = alloca(arg2);
		memBBRd(arg1, msg, arg2);
		to = alloca(arg5);
		memBBRd(arg4, to, arg5);
		res = sendto(arg0, msg, arg2, arg3, to, arg5);
		break;
	}
	case SYS_fstatfs: { /* 158 */
		/* int fstatfs(int fd, struct statfs *buf); */
		struct statfs64 buf64;
		struct statfs buf;
		res = fstatfs(arg0, &buf);
		ntos_statfs(&buf, &buf64);
		memBBWrt_alloc(arg1, (void*)&buf64, sizeof(buf64));
		break;
	}
	case SYS_stat: { /* 188 */
		/* int stat(char *path, struct stat *ub); */
		char path[PATH_MAX];
		struct stat64 sb64;
		struct stat sb;
		simroot(arg0, path, 0);
		res = stat(path, &sb);
		ntos_stat(&sb, &sb64);
		memBBWrt_alloc(arg1, (void*)&sb64, sizeof(sb64));
		break;
	}
	case SYS_fstat: { /* 189 */
		/* int fstat(int fd, struct stat *sb); */
		struct stat64 sb64;
		struct stat sb;
		res = fstat(arg0, &sb);
		ntos_stat(&sb, &sb64);
		memBBWrt_alloc(arg1, (void*)&sb64, sizeof(sb64));
		break;
	}
	case SYS_lstat: { /* 190 */
		/* int lstat(char *path, struct stat *ub); */
		char path[PATH_MAX];
		struct stat64 sb64;
		struct stat sb;
		simroot(arg0, path, 0);
		res = stat(path, &sb);
		ntos_stat(&sb, &sb64);
		memBBWrt_alloc(arg1, (void*)&sb64, sizeof(sb64));
		break;
	}
	case SYS_getrlimit: { /* 194 */
		/* int getrlimit(u_int which, struct rlimit *rlp); */
		struct rlimit rlp;
		res = getrlimit(arg0, &rlp);
		memBBWrt_alloc(arg1, (void*)&rlp, sizeof(rlp));
		break;
	}
	case SYS_getdirentries: { /* 196 */
		/* int getdirentries(int fd, char *buf, u_int count,
		 *     long *basep);
		 */
		long64 basep64;
		char *buf;
		long basep;
		buf = alloca(arg2);
		res = getdirentries(arg0, buf, arg2, &basep);
		memBBWrt_alloc(arg1, (void*)buf, arg2);
		basep64 = basep;
		memBBWrt_alloc(arg3, (void*)&basep64, sizeof(basep64));
		break;
	}
	case SYS_mmap: { /* 197 */
		/* caddr_t mmap(caddr_t addr, size_t len, int prot, int flags,
		 *	int fd, int pad, off_t pos);
		 */
		caddr_t addr;
		ADDR arg, reloc;
		addr = NULL;
		reloc = arg0;
		arg0 = fbsd_trunc_page(arg0);
		reloc -= arg0;
		arg1 += reloc;
		arg6 -= reloc;
		arg1 = fbsd_round_page(arg1);
		/* Try to map where the process wants it. */
		arg = (arg0 != 0) ? verify_unmapped_pages(arg0, arg1) : 0;
		if (arg == 0) {
			if (arg3 & MAP_FIXED)
				addr = unmap_mapped_pages(arg0, arg1);
			else
				arg0 = get_unmapped_pages(arg1);
		} else
			arg0 = arg;
		if (arg0 == 0) {
			errno = EINVAL;
			res = -1;
			break;
		}
		if (arg3 == MAP_STACK)
			arg3 = MAP_ANON;
		addr = mmap(addr, arg1, arg2, arg3, arg4, arg6);
		if (addr != (void*)-1) {
			for (arg = arg0; arg1 > 0; arg1 -= page_size) {
				pmemMap(arg, addr);
				arg += page_size;
				addr += page_size;
			}
			*ret = arg0 + reloc;
			goto out;
		}
		res = -1;
		break;
	}
	case SYS_lseek: /* 199 */
		/* off_t lseek(int fd, int pad, off_t offset, int whence); */
		res = lseek(arg0, arg2, arg3);
		break;
	case SYS___sysctl: { /* 202 */
		/* int __sysctl(int *name, u_int namelen, void *old,
		 *	size_t *oldlenp, void *new, size_t newlen);
		 */
		char *buf;
		int *name;
		size_t namelen, newlen, oldlen, *oldlenp;
		void *new, *old;
		namelen = arg1 << 2;
		newlen = (arg4) ? arg5 : 0;
		if (arg3 != 0L) {
			memBBRd(arg3, (void*)&oldlen, sizeof(size_t));
			oldlenp = &oldlen;
		} else {
			oldlen = 0;
			oldlenp = NULL;
		}
		buf = alloca(namelen + newlen + oldlen);
		name = (void*)buf;
		new = (newlen) ? buf + namelen : NULL;
		old = (oldlen) ? buf + namelen + newlen : NULL;
		memBBRd(arg0, (void*)name, namelen);
		if (newlen)
			memBBRd(arg4, new, newlen);
		if (name[0] == CTL_HW && name[1] == HW_PAGESIZE && arg1 == 2) {
			*oldlenp = sizeof(int);
			*(int*)old = FBSD_PAGESIZE;
			res = 0;
		} else if (name[0] == CTL_KERN && name[1] == KERN_USRSTACK &&
		    arg1 == 2) {
			*oldlenp = sizeof(uint64_t);
			*(uint64_t*)old = 0xA000000000000000ULL;
			res = 0;
		} else
			res = sysctl(name, namelen >> 2, old, oldlenp, new,
			    newlen);
		if (old != NULL) {
			memBBWrt_alloc(arg3, (void*)oldlenp, sizeof(size_t));
			memBBWrt_alloc(arg2, old, oldlen);
		}
		break;
	}
	case SYS_poll: { /* 209 */
		/* int poll(struct pollfd *fds, u_int nfds, int timeout); */
		struct pollfd *fds;
		size_t fdssz;
		fdssz = arg1 * sizeof(struct pollfd);
		fds = alloca(fdssz);
		memBBRd(arg0, (void*)fds, fdssz);
		res = poll(fds, arg1, arg2);
		memBBWrt(arg0, (void*)fds, fdssz);
		break;
	}
	case SYS_clock_gettime: { /* 232 */
		/* int clock_gettime(clockid_t clock_id, struct timespec *tp);
		 */
		struct timespec ts;
		struct timespec64 ts64;
		res = clock_gettime(arg0, &ts);
		ntos_timespec(&ts, &ts64);
		memBBWrt_alloc(arg1, (void*)&ts64, sizeof(ts64));
		break;
	}
	case SYS_issetugid: /* 253 */
		/* int issetugid(void); */
		*ret = issetugid();
		goto out;
	case SYS_sigprocmask: { /* 340 */
		/* int sigprocmask(int how, const sigset_t *set,
		 *         sigset_t *oset);
		 */
		sigset_t set, oset;
		sigset_t *setp, *osetp;
		if (arg1 != 0) {
			setp = &set;
			memBBRd(arg1, (void*)setp, sizeof(*setp));
		} else
			setp = NULL;
		osetp = (arg2 != 0) ? &oset : NULL;
		res = sigprocmask(arg0, setp, osetp);
		if (arg2 != 0)
			memBBWrt_alloc(arg2, (void*)osetp, sizeof(*osetp));
		break;
	}
	case 416: { /* SYS_sigaction */
		/* int sigaction(int sig, const struct sigaction *act,
		 *     struct sigaction *oact);
		 */
		struct sigaction64 sa;
		if (arg2 != 0) {
			res = signal_get_handler((int)arg0, &sa);
			memBBWrt_alloc(arg2, (void*)&sa, sizeof(sa));
		}
		if (arg1 != 0) {
			memBBRd(arg1, (void*)&sa, sizeof(sa));
			res = signal_set_handler((int)arg0, &sa);
		}
		break;
	}
	default:
		printf("syscall %d\n", num);
		/* We'll give it a shot */
		res = __syscall(num, arg0, arg1, arg2, arg3, arg4, arg5);
		break;
	}

	if (res == -1) {
		*ret = errno;
		*status = ~0ULL;
	} else
		*ret = res;

 out:	;
#if 0
	printf(" = %llx,%llx\n", *status, *ret);
#endif
}
