/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator System Call Routines
 *
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


/*##################### Header and Constant Definitions ####################*/

#include <errno.h>
#include <fcntl.h>
#ifdef HPUX9
#include <ndir.h>	/* For getdirentries */
#endif
#include <signal.h>	/* Also #includes <sys/syscall.h> on PA */
#ifdef __ia64
#include <sys/syscall.h>
#endif
#include <sched.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <ulimit.h>
#include <unistd.h>	/* Also #includes <utime.h> */
#include <ustat.h>
#if !defined(__ia64) && !defined(HPUX1100)
#include <nfs/export.h>	/* For struct export because nfs.h references it! */
#endif
#include <nfs/nfs.h>	/* For getdomainname */
#ifndef HPUX9
#include <sys/fstyp.h>
#endif
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/mpctl.h>
#include <sys/param.h>
#include <sys/pstat.h>
#include <sys/resource.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/utsname.h>
#include <sys/vfs.h>
#include "std.h"
#include "bits.h"
#include "types.h"
#undef B0		/* from sys/???.h */
#include "fields.h"
#include "state.h"
#include "ssc.h"
#include "sim.h"
#include "libcore.h"
#include "os_support.h"
#include "simmem.h"
#include "lp64struct.h"
#include "load.h"
#include "syscall_api.h"
#include "ui.h"

/* Prototypes for *64 functions */
off64_t lseek64(int, off64_t, int);

typedef struct {
    WORD year;
    WORD mon;
    WORD mday;
    WORD hour;
    WORD min;
    WORD sec;
    WORD msec;
    WORD wday;
} SscTime;

typedef struct {
    WORD ssc_Sec;
    WORD ssc_Usec;
} SscTimeval;

typedef struct {
    WORD ssc_Minuteswest;
    WORD ssc_Dsttime;
} SscTimezone;

typedef struct {
    ADDR addr;
    WORD len;
} SscDiskReq;

typedef struct {
    unsigned fd;
    WORD count;
} SscDiskStat;

static struct {
    ADDR buff;
    size_t len, off;
    unsigned scale;
} prof[5];

/* XXX - move these (and following prototypes) to .h file? */
extern BOOL intrsim, extint, kybdint;

BOOL acceptIrpt(void);
void memMPMap(ADDR, char *, size_t);

extern int getdents (unsigned int, void*, unsigned int);
extern int getdomainname(char *, int);


#if 0
#define FAIL_FIXED_MMAPS
#endif
#ifndef MAP_FAILED	/* defined in mman.h on 11.0 */
#define MAP_FAILED	(void *)-1L
#endif
#ifndef MAP_NORESERVE	/* defined in mman.h on 11.0 */
#define MAP_NORESERVE	0x400
#endif

#define ADDPTR(addr)    (lp64 \
			   ? addr \
			   : (BitfX(addr,32,32) | BitfX(addr,32,2) << 61))

#define INITMALLOC	2048	/* size of initial malloc for malargbuf */
#define MAXIRPT		30	/* size of interrupts array */
#define FDMAX		1024	/* file descriptors array size */

#define SIZE_FD(n)	((((n)/32)+1)*4)

/* XXX - move to ssc.h */
#define SSC_STOP	0

#define SYS_SIGCLEANUP	139	/* Under #define _KERNEL in sys/syscall.h */
#ifdef HPUX9
#define SYS_GETDENTS	356
#ifdef NEW_MP
#define SYS_WAITID	382
#endif
#define MAP_SHLIB	0x40
#endif
#ifndef HPUX1020
#define SYS_FCNTL64		359
#define SYS_FTRUNCATE64		360
#define SYS_FSTAT64		361
#define SYS_GETDIRENTRIES64	362
#define SYS_GETRLIMIT64		363
#define SYS_LOCKF64		364
#define SYS_LSEEK64		365
#define SYS_LSTAT64		366
#define SYS_MMAP64		367
#define SYS_SETRLIMIT64		368
#define SYS_STAT64		369
#define SYS_TRUNCATE64		370
#define SYS_ULIMIT64		371
#endif
#ifdef HPUX1020
#undef SYS_LWP_GETPRIVATE
#undef SYS_LWP_SETPRIVATE
#endif
#ifndef HPUX1100
#define SYS_LWP_GETPRIVATE	67
#define SYS_LWP_SETPRIVATE	68
#define SYS_SIGINHIBIT	491
#define SYS_SIGENABLE	492
#endif
/* XXX - recent BEs define SYS_SPROFIL; older BEs may have used
         515 implicitly? */
#ifndef SYS_SPROFIL
#define SYS_SPROFIL	540
#endif
/* XXX - This is only if we want to support older executables before profil
	 got merged in with sprofil; current IA-64 headers don't have profil
 */
#ifndef SYS_PROFIL
#define SYS_PROFIL	44
#endif

/* Renumbered syscalls */
#undef SYS_LWP_SELF	/* 394 */
#define SYS_LWP_SELF	147

/* New syscalls */
#ifndef SYS_PSET_CTL
#define SYS_PSET_CTL	524
#endif
#ifndef SYS_LWP_MUTEX_INIT2
#define SYS_LWP_MUTEX_INIT2	253
#endif

#define PROF_USHORT	1
#define PROF_UINT	2

/* From SWDEV4's /usr/include/sys/unistd.h */
#ifndef CPU_HP_INTEL_EM_1_0
#define CPU_HP_INTEL_EM_1_0	0x300
#endif

/* Defines not in system headers */
#ifndef HPUX9
#define _SI_MACHINE_MODEL	5
#endif
#ifndef _SC_THREAD_THREADS_MAX
#define _SC_THREAD_KEYS_MAX	431
#define _SC_THREAD_STACK_MIN	432
#define _SC_THREAD_THREADS_MAX	433
#endif
#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN	4096
#define PTHREAD_KEYS_MAX	128
#endif
#ifndef MPC_GETLWP_BINDINGTYPE
#define MPC_GETLWP_BINDINGTYPE	12
#endif

/* Prototypes for syscalls not in system headers */
int _utssys(char *, int, int);
int getmount_cnt(time_t *);
int getmount_entry(ulong_t, BYTE *, BYTE *, struct mount_data *, char *);
#ifndef HPUX9
int lchmod(const char *, mode_t);	/* Belongs in sys/stat.h */
int sysinfo(int, BYTE *, size_t);
#endif

static struct {
    short type;
    short irrBit;
} irptTbl[MAXIRPT];
static unsigned irptTop = 0;

static unsigned clkTmr = 0;	/* for clock interrupts */
static unsigned profTmr = 0;	/* for profile interrupts */
static SscReqNode *sscNew = NULL;
static SscReqNode *sscDiskDone = NULL;
SscReqNode *sscPend = NULL;

ADDR sigHdlr = ~0ULL;
CTR alarm_insts = ~0ULL, alarm_interval;
CTR prof_insts = ~0ULL;
static CTR prof_interval;
static DWORD prof_mask, prof_maxcnt;
#define PRE_ALARM 16
#define PRE_PROFIL 64

static pid_t cons_pid = -1;
static int cfd;
static struct termios origttyIos;
BOOL noConsole = NO;
char *consLog = NULL;

#ifdef NEW_MP
extern int numpids;
static struct {
    int fdmap[FDMAX];
} fdMPstate[4];
#define fdmap	fdMPstate[curPid].fdmap
#else
static int fdmap[FDMAX] = {0, 1, 2};
#endif
static struct {
    char *name;
    int oflag;
    mode_t mode;
    off_t offset;
} fdInfo[FDMAX];

static BYTE *malargbuf;
static size_t malargbufsize = 0;

#ifdef OLD_WAY
static ADDR shm_addr  = 0x40000000B0000000ULL;
#endif

static void setStatReturn(REG *ret, REG *status)
{
    if ((int)*status == -1)
	*ret = errno;
    else {
	*ret = *status;
	*status = 0;
    }
}

static BYTE *stat32to64(struct stat ilp32stat)
{
    unsigned i;
    static struct stat64 lp64stat = {0};

    lp64stat.st_dev	= ilp32stat.st_dev;
    lp64stat.st_ino	= ilp32stat.st_ino;
    lp64stat.st_mode	= ilp32stat.st_mode;
    lp64stat.st_nlink	= ilp32stat.st_nlink;
    lp64stat.st_reserved1	= ilp32stat.st_reserved1;
    lp64stat.st_reserved2	= ilp32stat.st_reserved2;
    lp64stat.st_rdev	= ilp32stat.st_rdev;
    lp64stat.st_size	= ilp32stat.st_size;
    lp64stat.st_atime	= ilp32stat.st_atime;
    lp64stat.st_spare1	= ilp32stat.st_spare1;
    lp64stat.st_mtime	= ilp32stat.st_mtime;
    lp64stat.st_spare2	= ilp32stat.st_spare2;
    lp64stat.st_ctime	= ilp32stat.st_ctime;
    lp64stat.st_spare3	= ilp32stat.st_spare3;
    lp64stat.st_blksize	= ilp32stat.st_blksize;
    lp64stat.st_blocks	= ilp32stat.st_blocks;
    lp64stat.st_pad	= ilp32stat.st_pad;
    lp64stat.st_acl	= ilp32stat.st_acl;
    lp64stat.st_remote	= ilp32stat.st_remote;
    lp64stat.st_netdev	= ilp32stat.st_netdev;
    lp64stat.st_netino	= ilp32stat.st_netino;
    lp64stat.st_cnode	= ilp32stat.st_cnode;
    lp64stat.st_rcnode	= ilp32stat.st_rcnode;
    lp64stat.st_netsite	= ilp32stat.st_netsite;
    lp64stat.st_fstype	= ilp32stat.st_fstype;
    lp64stat.st_realdev	= ilp32stat.st_realdev;
    lp64stat.st_basemode	= ilp32stat.st_basemode;
    lp64stat.st_spareshort	= ilp32stat.st_spareshort;
    lp64stat.st_uid	= ilp32stat.st_uid;
    lp64stat.st_gid	= ilp32stat.st_gid;
    for (i = 0; i < _SPARE4_SIZE; i++)
	lp64stat.st_spare4[i] = ilp32stat.st_spare4[i];

    return (BYTE *)&lp64stat;
}

static BYTE *stat32to64x(struct stat ilp32stat)
{
    unsigned i;
    static struct stat64x lp64stat = {0};

    lp64stat.st_dev	= ilp32stat.st_dev;
    lp64stat.st_ino	= ilp32stat.st_ino;
    lp64stat.st_mode	= ilp32stat.st_mode;
    lp64stat.st_nlink	= ilp32stat.st_nlink;
    lp64stat.st_reserved1	= ilp32stat.st_reserved1;
    lp64stat.st_reserved2	= ilp32stat.st_reserved2;
    lp64stat.st_rdev	= ilp32stat.st_rdev;
    lp64stat.st_size	= ilp32stat.st_size;
    lp64stat.st_atime	= ilp32stat.st_atime;
    lp64stat.st_spare1	= ilp32stat.st_spare1;
    lp64stat.st_mtime	= ilp32stat.st_mtime;
    lp64stat.st_spare2	= ilp32stat.st_spare2;
    lp64stat.st_ctime	= ilp32stat.st_ctime;
    lp64stat.st_spare3	= ilp32stat.st_spare3;
    lp64stat.st_blksize	= ilp32stat.st_blksize;
    lp64stat.st_blocks	= ilp32stat.st_blocks;
    lp64stat.st_pad	= ilp32stat.st_pad;
    lp64stat.st_acl	= ilp32stat.st_acl;
    lp64stat.st_remote	= ilp32stat.st_remote;
    lp64stat.st_netdev	= ilp32stat.st_netdev;
    lp64stat.st_netino	= ilp32stat.st_netino;
    lp64stat.st_cnode	= ilp32stat.st_cnode;
    lp64stat.st_rcnode	= ilp32stat.st_rcnode;
    lp64stat.st_netsite	= ilp32stat.st_netsite;
    lp64stat.st_fstype	= ilp32stat.st_fstype;
    lp64stat.st_realdev	= ilp32stat.st_realdev;
    lp64stat.st_basemode	= ilp32stat.st_basemode;
    lp64stat.st_spareshort	= ilp32stat.st_spareshort;
    lp64stat.st_uid	= ilp32stat.st_uid;
    lp64stat.st_gid	= ilp32stat.st_gid;
    for (i = 0; i < _SPARE4_SIZE; i++)
	lp64stat.st_spare4[i] = ilp32stat.st_spare4[i];

    return (BYTE *)&lp64stat;
}

static BYTE *ustat32to64(BYTE *buf)
{
    struct ustat *ilp32ustat = (struct ustat *)buf;
    static struct ustat64 lp64ustat;

    lp64ustat.f_tfree	= ilp32ustat->f_tfree;
    lp64ustat.f_tinode	= ilp32ustat->f_tinode;
    (void)strcpy(lp64ustat.f_fname, ilp32ustat->f_fname);
    (void)strcpy(lp64ustat.f_fpack, ilp32ustat->f_fpack);
    lp64ustat.f_blksize	= ilp32ustat->f_blksize;

    return (BYTE *)&lp64ustat;
}

static struct flock *flock64to32(BYTE *buf)
{
    struct flock64 *lp64flock = (struct flock64 *)buf;
    static struct flock ilp32flock;

    ilp32flock.l_type	= lp64flock->l_type;
    ilp32flock.l_whence	= lp64flock->l_whence;
    ilp32flock.l_start	= lp64flock->l_start;
    ilp32flock.l_len	= lp64flock->l_len;
    ilp32flock.l_pid	= lp64flock->l_pid;

    return &ilp32flock;
}

static BYTE *flock32to64(BYTE *buf)
{
    struct flock *ilp32flock = (struct flock *)buf;
    static struct flock64 lp64flock;

    lp64flock.l_type	= ilp32flock->l_type;
    lp64flock.l_whence	= ilp32flock->l_whence;
    lp64flock.l_start	= ilp32flock->l_start;
    lp64flock.l_len	= ilp32flock->l_len;
    lp64flock.l_pid	= ilp32flock->l_pid;

    return (BYTE *)&lp64flock;
}

static struct timeval *timeval64to32(BYTE *buf)
{
    struct timeval64 *lp64tv = (struct timeval64 *)buf;
    static struct timeval ilp32tv;

    ilp32tv.tv_sec  = lp64tv->tv_sec;
    ilp32tv.tv_usec = lp64tv->tv_usec;
    return &ilp32tv;
}

#if 0
static BYTE *timeval32to64(struct timeval tv)
{
    static struct timeval64 tv64;

    tv64.tv_sec  = tv.tv_sec;
    tv64.tv_usec = tv.tv_usec;
    return (BYTE *)&tv64;
}
#endif

static BYTE *rlimit32to64(struct rlimit *rlp)
{
    static struct rlimit64 rlim64;

    rlim64.rlim_cur = rlp->rlim_cur;
    rlim64.rlim_max = rlp->rlim_max;
    return (BYTE *)&rlim64;
}

static struct rlimit rlimit64to32(struct rlimit64 *rlp64)
{
    struct rlimit rlim;

    rlim.rlim_cur = rlp64->rlim_cur;
    rlim.rlim_max = rlp64->rlim_max;
    return rlim;
}

static fd_set mapSelectFds(BYTE *buf, int num)
{
    int i;
    fd_set fds;

    FD_ZERO(&fds);
    for (i = 0; i < num; i++)
	if (FD_ISSET(i, (fd_set *)buf))
	    FD_SET(fdmap[i], &fds);
    return fds;
}

static int getShmSz(int shmid)
{
    struct shmid_ds buf;

    shmctl(shmid, IPC_STAT, &buf);
    return buf.shm_segsz;
}

#ifdef HPUX9
static int getDirEnts(int fd, BYTE *buf, int len)
{
    int i, n, slen;
    struct direct *directbuf;
    unsigned short reclen;
    BYTE *p, *q;
    off_t base;

    if (!(directbuf = malloc(len))) {
	printf("malloc failed\n");	/* XXX - what should we do? */
	return 0;
    }
    n = getdirentries(fd, directbuf, (size_t)len/2, &base);
    p = (BYTE *)directbuf;
    q = buf;
    for (i = 0; i < n;) {
	slen = ((struct direct *)p)->d_reclen - 8;
	(void)memset(q, 0, 8);	q += 8;
	(void)memcpy(q, p, 4);	q += 4; p += 4; i += 4;	/* fileno */

	reclen = (((unsigned)*p * 256) | (unsigned)*(p+1) & 0xFF) +
		 ((slen % 8 == 4) ? 12 : 8);
	*q = reclen >> 8;
	*(q+1) = reclen & 0xFF;
	q += 2; p += 2; i += 2;

	(void)memcpy(q, p, 2);		q += 2; p += 2; i += 2;	/* namlen */
	(void)memcpy(q, p, slen);	q += slen; p += slen; i += slen;
	if (slen % 8 == 4) {
	    (void)memset(q, 0, 4);	q += 4;
	}
    }
    free(directbuf);
    return q - buf;
}
#endif

static char *copyArg(REG arg)
{
    static BYTE buf[32768];

    if (arg) {
	memBBRd(ADDPTR(arg), buf, 0);
	return (char *)buf;
    } else
	return NULL;
}

static BOOL mmapOverlap(ADDR adr, REG size)
{
    ADDR start, end, a;
    DWORD junk;

    start = page_base(ADDPTR(adr));
    end = page_base(ADDPTR(adr)+size-1);
    for (a = start; a <= end; a += page_size)
	if (memMRd(a, 1, &junk))	/* page exists */
	    return YES;
    return NO;
}

void profCnt(void)
{
    ADDR baddr, offset, cntaddr;
    unsigned bucket;
    HWORD cnt2;
    DWORD cnt8;

    prof_insts += prof_interval;
    baddr = ip & ~0xFULL;
    if (prof[0].scale < 2 || baddr < prof[0].off)
	return;
    offset = baddr - prof[0].off;
    bucket = (((offset >> 4) * prof[0].scale) >> 12) & prof_mask;
    if (bucket >= prof[0].len)
	return;
    cntaddr = prof[0].buff + bucket;
    if (!memMRd(cntaddr, 2, &cnt8)) {
	prof[0].scale = 0;
	return;
    }
    cnt2 = cnt8;
    if (cnt2 < prof_maxcnt) {
	cnt2++;
	if (!memMWrt(cntaddr, 2, cnt2))
	    prof[0].scale = 0;
    }
}

static void chkMalargbuf(size_t len)
{
    /* make sure we have enough space in malargbuf */
    if (len > malargbufsize) {
	if (!(malargbuf = realloc(malargbuf, len)))
	    return;	/* XXX - set errno? */
	malargbufsize = len;
    }
}

static void execLoad(ADDR arg0, ADDR arg1, ADDR arg2)
{
    BYTE buf[128], buf3[1024];
    char *buf2[64], **argvp = buf2;
    BYTE *argp = buf3;
    unsigned argcnt = 0;
    static unsigned ptrSz = 0;
    ADDR argvAddr;

    if (!ptrSz)
	ptrSz = lp64 ? 8 : 4;
    memBBRd(ADDPTR(arg0), buf, 0);
    /* XXX - handle arg2 which points at environment */
    while (1) {
	if (lp64)
	    memBBRd(arg1, (BYTE *)&argvAddr, ptrSz);
	else {
	    argvAddr = 0;
	    memBBRd(ADDPTR(arg1), (BYTE *)&argvAddr+ptrSz, ptrSz);
	}
	*argvp = (char *)argp;
	if (!argvAddr) {
	    *argvp = 0;
	    break;
	}
	memBBRd(ADDPTR(argvAddr), argp, 0);
	arg1 += ptrSz;
	argvp++;
	argp += strlen((char *)argp) + 1;
	argcnt++;
    }
    memFreeAll();
#ifdef NEW_MP
    numpids--;
#endif
    resetState(0);
    elfLoad((char *)buf, argcnt, (char **)buf2);
#ifdef NEW_MP
    initAppState(numpids-1);
#endif
}

void saveOpenFiles(FILE *f)
{
    unsigned i, offset;

    for (i = 3; i < FDMAX; i++) {
	if (!fdInfo[i].name)
	    continue;
#if 0
	fsync(fdmap[i]);
#endif
	offset = lseek(fdmap[i], 0, SEEK_CUR);
	fprintf(f, "ski_file %s %o %o %x\n", fdInfo[i].name,
		fdInfo[i].oflag, fdInfo[i].mode, offset);
    }
}

void restoreOpenFile(char *name, unsigned oflag, unsigned mode, unsigned offset)
{
    int fd;

    fd = open(name, oflag, mode);
    lseek(fd, offset, SEEK_SET);
    fdmap[fd] = fd;
}

void doSyscall(HWORD num, REG arg0, REG arg1, REG arg2, REG arg3, REG arg4,
	       REG arg5, REG arg6, REG arg7, REG *ret, REG *status)
{
    BYTE buf[32768], buf2[32768];
    char msg[100];
    int size, pfd[2];
    size_t nbytes;
    struct stat statbuf;
    struct tms tmsbuf;
    struct utimbuf utimbuf;
    struct timeval *tmout;
#if 0
    struct timeval tvbuf;
    struct timezone tzbuf;
#endif
    struct rlimit rlimbuf;
    struct rusage rusgbuf;
    fd_set readfds, writefds, errorfds, *rfds, *wfds, *efds;
    struct mount_data mdata;
    static BOOL lgf;
#ifdef NEW_MP
    static BOOL firstsyscall[4] = {YES, YES, YES, YES};
    static ADDR anon_mmapMP[4];
#define anon_mmap	anon_mmapMP[curPid]
#else
    static ADDR anon_mmap;
#endif

#ifdef NEW_MP
    if (firstsyscall[curPid]) {
	int i;

	firstsyscall[curPid] = NO;
	fdmap[0] = 0;
	fdmap[1] = 1;
	fdmap[2] = 2;
	for (i = 3; i < FDMAX; i++)
	    fdmap[i] = -1;
	anon_mmap = lp64 ? 0x9FFFFFFF7F600000ULL : 0x200000006A7F0000ULL;
    }
#endif
    /* initialize malargbuf ptr if necessary */
    if (malargbufsize == 0) {
	int i;
	struct utsname name;
	char dummy;
	int ver, rev;

#ifndef NEW_MP
	for (i = 3; i < FDMAX; i++)
#ifdef HOST_MAP
	    fdmap[i] = -1;
#else
	    fdmap[i] = i;
#endif
#endif
	for (i = 3; i < FDMAX; i++)
	    fdInfo[i].name = NULL;
	if (!(malargbuf = malloc(INITMALLOC)))
	    return;	/* XXX - set errno? */
	malargbufsize = INITMALLOC;
	uname(&name);
	sscanf(name.release, "%c.%d.%d", &dummy, &ver, &rev);
	lgf = ver >= 11 || (ver == 10 && rev >= 20);
#ifndef NEW_MP
	anon_mmap = lp64 ? 0x9FFFFFFF7F600000ULL : 0x200000006A7F0000ULL;
#endif
	prof_interval = 10000 * mips;
    }

#if 0
    num &= ~0x400;
#endif
    switch (num) {
	case SYS_ACCEPT:
	    memBBRd(ADDPTR(arg2), (BYTE *)&size, sizeof (int));
	    if (arg1) {
		*status = accept(fdmap[arg0], buf, &size);
		memBBWrt(ADDPTR(arg1), buf, size);
	    } else
		*status = accept(fdmap[arg0], NULL, &size);
	    memBBWrt(ADDPTR(arg2), (BYTE *)&size, sizeof (int));
	    setStatReturn(ret, status);
	    break;
	case SYS_ACCESS:
	    *status = access(copyArg(arg0), arg1);
	    setStatReturn(ret, status);
	    break;
	case SYS_ALARM:
	    if (arg0 > MAX_ALARM)
		arg0 = MAX_ALARM;
	    if (arg0) {
		alarm_interval = arg0 * 1000000 * mips;
		alarm_insts = total_insts + alarm_interval;
		preInst |= PRE_ALARM;
	    } else {
		alarm_insts = ~0ULL;
		preInst &= ~PRE_ALARM;
	    }
	    /* XXX - need to return time left */
	    *ret = 0;
	    *status = 0;
	    break;
	case SYS_BIND:
	    memBBRd(ADDPTR(arg1), buf, arg2);
	    *status = bind(fdmap[arg0], buf, arg2);
	    setStatReturn(ret, status);
	    break;
	case SYS_BRK:
	    arg0 = ADDPTR(arg0);
	    if (arg0 > heap) {
		ADDR adr;

		for (adr = page_base(heap); adr < arg0; adr += page_size)
		    (void)pmemLookup_p(adr);
	    }
	    heap = arg0;
	    *status = 0;
	    *ret = 0;
	    break;
	case SYS_CHDIR:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = chdir((char *)buf);
	    setStatReturn(ret, status);
	    break;
	case SYS_CHMOD:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = chmod((char *)buf, arg1);
	    setStatReturn(ret, status);
	    break;
	case SYS_CHOWN:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = chown((char *)buf, arg1, arg2);
	    setStatReturn(ret, status);
	    break;
	case SYS_CHROOT:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = chroot((char *)buf);
	    setStatReturn(ret, status);
	    break;
	case SYS_CLOSE:
	    if (fdmap[arg0] > 2) {	/* don't close stdin, stdout, stderr */
		*status = close(fdmap[arg0]);
#if 0
fprintf(stderr, "closing file with fd %d (host fd %d)\n", (int)arg0, fdmap[arg0]);
#endif
		fdmap[arg0] = -1;
	    } else
		*status = 0;
	    setStatReturn(ret, status);
	    break;
	case SYS_CONNECT:
	    memBBRd(ADDPTR(arg1), buf, arg2);
	    *status = connect(fdmap[arg0], (char *)buf, arg2);
	    setStatReturn(ret, status);
	    break;
	case SYS_CREAT:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = creat((char *)buf, arg1);
	    setStatReturn(ret, status);
	    break;
	case SYS_DUP:
	    *status = dup(fdmap[arg0]);
	    setStatReturn(ret, status);
	    break;
	case SYS_DUP2:
	    *status = dup2(fdmap[arg0], fdmap[arg1]);
	    setStatReturn(ret, status);
	    break;
	case SYS_EXECVE:
	    execLoad(arg0, arg1, arg2);
	    /*symListFree();*/
	    progPause();
	    break;
	case SYS_EXIT:
	    ip += 4;
#ifdef NEW_MP
	    mpState[curPid].status_ = arg0 << 8;
	    progExit("pid %u exited with status %d\n", curPid+2, (int)arg0);
#else
	    GrWrtx(8, arg0, 0);
	    progExit("program exited with status %d\n", (int)arg0);
#endif
	    break;
	case SYS_FCHDIR:
	    *status = fchdir(fdmap[arg0]);
	    setStatReturn(ret, status);
	    break;
	case SYS_FCHMOD:
	    *status = fchmod(fdmap[arg0], arg1);
	    setStatReturn(ret, status);
	    break;
	case SYS_FCHOWN:
	    *status = fchown(fdmap[arg0], arg1, arg2);
	    setStatReturn(ret, status);
	    break;
	case SYS_FCNTL:
	    if (arg1 == F_GETLK || arg1 == F_SETLK || arg1 == F_SETLKW) {
		if (!arg2) {
		    DWORD junk;

		    errno = memMRd(0, 1, &junk) ? EINVAL : EFAULT;
		    *status = -1;
		} else if (lp64) {
		    memBBRd(arg2, buf, sizeof (struct flock64));
		    *status = fcntl(fdmap[arg0], arg1, flock64to32(buf));
		    memBBWrt(arg2, flock32to64(buf), sizeof (struct flock64));
		} else {
		    memBBRd(ADDPTR(arg2), buf, sizeof (struct flock));
		    *status = fcntl(fdmap[arg0], arg1, buf);
		    memBBWrt(ADDPTR(arg2), buf, sizeof (struct flock));
		}
	    } else
		*status = fcntl(fdmap[arg0], arg1, arg2);
	    setStatReturn(ret, status);
	    break;
#ifdef NEW_MP
	case SYS_FORK:
	    newPid();
	    memFork(curPid, numpids-1);
	    stateFork(curPid, numpids-1);
	    *ret = numpids+1;	/* new pid of child */
	    GrWrtx(9, 0, 0);	/* GR9 = 0 indicates parent */
	    *status = 0;
	    break;
#endif
	case SYS_FPATHCONF:
	    *status = fpathconf(fdmap[arg0], arg1);
	    setStatReturn(ret, status);
	    break;
	case SYS_FSTAT:
	case SYS_FSTAT64:
	    *status = fstat(fdmap[arg0], &statbuf);
#ifdef CONSTANT_TIME
statbuf.st_atime = statbuf.st_mtime;
#endif
	    if ((int)*status != -1) {
		if (lp64)
		    memBBWrt(arg1, stat32to64(statbuf), sizeof(struct stat64));
		else if (num == SYS_FSTAT64)
		    memBBWrt(ADDPTR(arg1), stat32to64x(statbuf), sizeof(struct stat64x));
		else
		    memBBWrt(ADDPTR(arg1), (BYTE *)&statbuf, sizeof(statbuf));
        }
	    setStatReturn(ret, status);
	    break;
	case SYS_FSYNC:
	    /* XXX - call __fsync_sys on 11.x? */
	    *status = fsync(fdmap[arg0]);
	    setStatReturn(ret, status);
	    break;
	case SYS_FTRUNCATE:
	    *status = ftruncate(fdmap[arg0], arg1);
	    setStatReturn(ret, status);
	    break;
	case SYS_FTRUNCATE64:
	    *status = ftruncate(fdmap[arg0], arg2);
	    setStatReturn(ret, status);
	    break;
	case SYS_GETDENTS:
	    chkMalargbuf(arg2);
#ifdef HPUX9
	    *status = getDirEnts(fdmap[arg0], malargbuf, arg2);
#else
	    *status = getdents(fdmap[arg0], malargbuf, arg2);
#endif
	    memBBWrt(ADDPTR(arg1), malargbuf, arg2);
	    setStatReturn(ret, status);
	    break;
	case SYS_GETDOMAINNAME:
	    chkMalargbuf(arg1);
	    *status = getdomainname((char *)malargbuf, arg1);
	    memBBWrt(ADDPTR(arg0), malargbuf, arg1);
	    setStatReturn(ret, status);
	    break;
	case SYS_GETGID:
	    *ret = getgid();
	    GrWrtx(9, getegid(), 0);
	    *status = 0;
	    break;
	case SYS_GETMOUNT_CNT:
	    if (arg0) {
		*status = getmount_cnt((time_t *)buf);
		memBBWrt(ADDPTR(arg0), buf, sizeof (time_t));
	    } else
		*status = getmount_cnt(NULL);
	    setStatReturn(ret, status);
	    break;
	case SYS_GETMOUNT_ENTRY:
	    *status = getmount_entry(arg0, buf, buf2, &mdata, copyArg(arg4));
	    memBBWrt(ADDPTR(arg1), buf, strlen((char *)buf)+1);
	    memBBWrt(ADDPTR(arg2), buf2, strlen((char *)buf2)+1);
	    /*memBBWrt(ADDPTR(arg3), (BYTE *)&mdata, sizeof (struct mdata));*/
	    setStatReturn(ret, status);
	    break;
	case SYS_GETPEERNAME:
	    memBBRd(ADDPTR(arg2), (BYTE *)&size, sizeof (int));
	    if (arg1) {
		*status = getpeername(fdmap[arg0], buf, &size);
		memBBWrt(ADDPTR(arg1), buf, size);
	    } else
		*status = getpeername(fdmap[arg0], NULL, &size);
	    memBBWrt(ADDPTR(arg2), (BYTE *)&size, sizeof (int));
	    setStatReturn(ret, status);
	    break;
	case SYS_GETPGRP2:
	    /* XXX - the first (& only) simulated process gets a pid of 2 */
	    *ret = 2;
	    *status = 0;
	    break;
	case SYS_GETPID:
#ifdef NEW_MP
	    *ret = curPid + 2;
	    GrWrtx(9, parentPid, 0);
#else
	    /* XXX - the first (& only) simulated process gets a pid of 2 */
	    *ret = 2;
	    /* XXX - for Shivaji, return Ski's pid */
	    *ret = getpid();
	    GrWrtx(9, 1, 0);
#endif
	    *status = 0;
	    break;
	case SYS_GETRLIMIT:
	    switch ((int)arg0) {
#ifndef HPUX9
		case RLIMIT_CORE:
		    break;
		case RLIMIT_CPU:
		    break;
		case RLIMIT_DATA:
		    break;
		case RLIMIT_FSIZE:
		    break;
#endif
		case RLIMIT_NOFILE:
		    *status = getrlimit(arg0, &rlimbuf);
		    if ((int)*status != -1) {
			if (lp64)
			    memBBWrt(arg1, rlimit32to64(&rlimbuf),
				     sizeof(struct rlimit64));
			else
			    memBBWrt(ADDPTR(arg1), (BYTE *)&rlimbuf,
				     sizeof(struct rlimit));
            }
		    break;
#ifndef HPUX9
		case RLIMIT_STACK:
		    break;
		case RLIMIT_RSS:
		    break;
		case RLIMIT_AS:
		    break;
#endif
		default:
		    errno = EINVAL;
		    *status = -1;
		    break;
	    }
	    setStatReturn(ret, status);
	    break;
	case SYS_GETRUSAGE:
	    switch ((int)arg0) {
		case RUSAGE_SELF:
		    *status = getrusage(arg0, &rusgbuf);
		    if ((int)*status != -1)
#if 0
			if (lp64)
			    memBBWrt(arg1, rusage32to64(&rusgbuf),
				     sizeof (struct rusage64));
			else
#endif
			    memBBWrt(ADDPTR(arg1), (BYTE *)&rlimbuf,
				     sizeof (struct rusage));
		    break;
		case RUSAGE_CHILDREN:
		default:
		    errno = EINVAL;
		    *status = -1;
		    break;
	    }
	    setStatReturn(ret, status);
	    break;
	case SYS_GETSOCKNAME:
	    memBBRd(ADDPTR(arg2), (BYTE *)&size, sizeof (int));
	    if (arg1) {
		*status = getsockname(fdmap[arg0], buf, &size);
		memBBWrt(ADDPTR(arg1), buf, size);
	    } else
		*status = getsockname(fdmap[arg0], NULL, &size);
	    memBBWrt(ADDPTR(arg2), (BYTE *)&size, sizeof (int));
	    setStatReturn(ret, status);
	    break;
	case SYS_GETSOCKOPT:
	    memBBRd(ADDPTR(arg4), (BYTE *)&size, sizeof (int));
	    *status = getsockopt(fdmap[arg0], arg1, arg2, buf, &size);
	    memBBWrt(ADDPTR(arg3), buf, size);
	    memBBWrt(ADDPTR(arg4), (BYTE *)&size, sizeof (int));
	    setStatReturn(ret, status);
	    break;
	case SYS_GETTIMEOFDAY:
#if 0
	    *status = gettimeofday(&tvbuf, &tzbuf);
	    if ((int)*status != -1) {
		if (lp64)
		    memBBWrt(arg0, timeval32to64(tvbuf),
			     sizeof (struct timeval64));
		else
		    memBBWrt(ADDPTR(arg0), (BYTE *)&tvbuf, sizeof tvbuf);
		if (arg1)
		    memBBWrt(ADDPTR(arg1), (BYTE *)&tzbuf, sizeof tzbuf);
	    }
#else
{
	    SimTime st;
	    getSimTime(&st);
	    memBBWrt(ADDPTR(arg0), (BYTE *)&st, 8);
	    if (arg1)
		memBBWrt(ADDPTR(arg1), (BYTE *)&st.simMinuteswest, 8);
}
	    *status = 0;
#endif
	    setStatReturn(ret, status);
	    break;
	case SYS_GETUID:
	    *ret = getuid();
	    GrWrtx(9, geteuid(), 0);
	    *status = 0;
	    break;
	case SYS_IOCTL:
	    size = (arg1 & IOCSIZE_MASK) >> 16;
	    /* check IOC_IN to see if I will write driver */
	    if ((arg1 & IOC_IN) && size)
		memBBRd(ADDPTR(arg2), buf, size);

	    if ((arg1 & IOC_OUT) || ((arg1 & IOC_IN) && size)) {
		*status = ioctl(fdmap[arg0], arg1, buf);
		if (((int)*status != -1) && (arg1 & IOC_OUT))
		    memBBWrt(ADDPTR(arg2), buf, size);
	    } else
		*status = ioctl(fdmap[arg0], arg1, arg2);
	    setStatReturn(ret, status);
	    break;
	case SYS_KILL:
	    if (!arg0 || arg0 == getpid())
		progExit("program killed with signal %d\n", (int)arg1);
	    *status = kill(arg0, arg1);
	    setStatReturn(ret, status);
	    break;
#ifndef HPUX9
	case SYS_LCHMOD:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = lchmod((char *)buf, arg1);
	    setStatReturn(ret, status);
	    break;
	case SYS_LCHOWN:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = lchown((char *)buf, arg1, arg2);
	    setStatReturn(ret, status);
	    break;
#endif
	case SYS_LINK:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    memBBRd(ADDPTR(arg1), buf2, 0);
	    *status = link((char *)buf, (char *)buf2);
	    setStatReturn(ret, status);
	    break;
	case SYS_LISTEN:
	    *status = listen(fdmap[arg0], arg1);
	    setStatReturn(ret, status);
	    break;
	case SYS_LOCKF:
	    *status = lockf(fdmap[arg0], arg1, arg2);
	    setStatReturn(ret, status);
	    break;
	case SYS_LSEEK:
	    *status = lseek(fdmap[arg0], arg1, arg2);
	    setStatReturn(ret, status);
	    break;
	case SYS_LSEEK64:
	    if (lgf)
#ifdef HPUX9	/* XXX - This doesn't seem to work! */
		*status = syscall(SYS_LSEEK64, fdmap[arg0], arg2, arg4);
#else
		*status = lseek64(fdmap[arg0], arg1, arg2);
#endif
	    else
		*status = lseek(fdmap[arg0], arg2, arg4);
	    setStatReturn(ret, status);
	    break;
	case SYS_LSTAT:
	case SYS_LSTAT64:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = lstat((char *)buf, &statbuf);
	    if ((int)*status != -1) {
		if (lp64)
		    memBBWrt(arg1, stat32to64(statbuf), sizeof(struct stat64));
		else if (num == SYS_LSTAT64)
		    memBBWrt(ADDPTR(arg1), stat32to64x(statbuf), sizeof(struct stat64x));
		else
		    memBBWrt(ADDPTR(arg1), (BYTE *)&statbuf, sizeof(statbuf));
        }
	    setStatReturn(ret, status);
	    break;
	case SYS_LWP_GETPRIVATE:
	    /* XXX - set errno if TPNAT? */
	    *status = TPNAT ? -1 : TP;
	    setStatReturn(ret, status);
	    break;
	case SYS_LWP_MUTEX_INIT2:
	    *status = 0;
	    setStatReturn(ret, status);
	    break;
	case SYS_LWP_SELF:
	    *status = 0;
	    setStatReturn(ret, status);
	    break;
	case SYS_LWP_SETPRIVATE:
	    TP = arg0;
	    TPNAT = 0;
	    *status = 0;
	    setStatReturn(ret, status);
	    break;
	case SYS_MADVISE:
	    *status = 0;
	    setStatReturn(ret, status);
	    break;
	case SYS_MKDIR:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = mkdir((char *)buf, arg1);
	    setStatReturn(ret, status);
	    break;
	case SYS_MKNOD:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = mknod((char *)buf, arg1, arg2);
	    setStatReturn(ret, status);
	    break;
	case SYS_MMAP:
	    if (!(arg3 & (MAP_FILE|MAP_ANONYMOUS)))
		arg3 |= MAP_FILE;
	    if (!(arg3 & (MAP_VARIABLE|MAP_FIXED)))
		arg3 |= MAP_VARIABLE;
#if 0
	    if (arg3 & MAP_SHLIB)
		arg3 &= ~(REG)MAP_SHLIB;
#endif
	    if (arg3 & MAP_NORESERVE)
		arg3 &= ~(REG)MAP_NORESERVE;
	    if (!(arg2 & PROT_WRITE)) {
		arg2 |= PROT_WRITE;
		if (arg3 & MAP_SHARED) {
		    arg3 &= ~(REG)MAP_SHARED;
		    arg3 |=  (REG)MAP_PRIVATE;
		}
	    }
	    if (arg3 & MAP_FIXED && mmapOverlap(arg0, arg1)) {
		errno = EINVAL;
		*status = -1;
		setStatReturn(ret, status);
		break;
	    }
if (!arg0 && arg3 & MAP_FIXED) {
arg3 &= ~(REG)MAP_FIXED;
arg3 |=  (REG)MAP_VARIABLE;
}
	    if (arg3 == (MAP_ANONYMOUS|MAP_FIXED|MAP_PRIVATE) ||
		arg3 == (MAP_ANONYMOUS|MAP_FIXED|MAP_PRIVATE|MAP_SHLIB) ||
		arg3 == (MAP_FILE|MAP_FIXED|MAP_PRIVATE) ||
		arg3 == (MAP_FILE|MAP_FIXED|MAP_PRIVATE|MAP_SHLIB) ||
		arg3 == (MAP_FILE|MAP_FIXED|MAP_SHARED) ||
		arg3 == (MAP_FILE|MAP_FIXED|MAP_SHARED|MAP_SHLIB)) {
		ADDR start;
		BYTE *hostadr;

#ifdef FAIL_FIXED_MMAPS
errno = EINVAL;
*status = -1;
setStatReturn(ret, status);
break;
#endif
		arg3 &= ~(REG)MAP_FIXED;
		arg3 |=  (REG)MAP_VARIABLE;
		hostadr = mmap((void *)0,arg1,arg2,arg3,fdmap[arg4],arg5);
		if (hostadr == MAP_FAILED) {
		    *status = (REG)(long)hostadr;
		    setStatReturn(ret, status);
		    break;
		}
		start = ADDPTR(arg0);	/* XXX - assume page aligned? */
		if (start < anon_mmap)
		    anon_mmap = start;
#if 0
fprintf(stderr, "fixed: %llx mapped to %x\n", start, hostadr);
#endif
		memMPMap(start, (char*)hostadr, arg1);
		if (arg3 & MAP_FILE)
		    mmapSyms(fdmap[arg4], start, arg1, arg5);
		else if (arg2 & PROT_EXEC)
		    dynBlock(start, arg1);
		*status = start;
		setStatReturn(ret, status);
	    } else if (arg3 == (MAP_ANONYMOUS|MAP_VARIABLE|MAP_PRIVATE) ||
		       arg3 == (MAP_ANONYMOUS|MAP_VARIABLE|MAP_PRIVATE|MAP_SHLIB) ||
		       arg3 == (MAP_FILE|MAP_VARIABLE|MAP_PRIVATE) ||
		       arg3 == (MAP_FILE|MAP_VARIABLE|MAP_PRIVATE|MAP_SHLIB) ||
		       arg3 == (MAP_FILE|MAP_VARIABLE|MAP_SHARED) ||
		       arg3 == (MAP_FILE|MAP_VARIABLE|MAP_SHARED|MAP_SHLIB)) {
		ADDR start, nbytes;
		BYTE *hostadr;

		hostadr = mmap((void *)(long)arg0,arg1,arg2,arg3,fdmap[arg4],arg5);
		if (hostadr == MAP_FAILED) {
		    *status = (REG)(long)hostadr;
		    setStatReturn(ret, status);
		    break;
		}
		nbytes = page_base(arg1) == arg1
			    ? arg1
			    : page_base(arg1) + page_size;
		anon_mmap -= nbytes;
		start = anon_mmap;
#if 0
fprintf(stderr, "var: %llx mapped to %x\n", start, hostadr);
#endif
		memMPMap(start, (char*)hostadr, arg1);
		if (arg3 & MAP_FILE)
		    mmapSyms(fdmap[arg4], start, arg1, arg5);
		else if (arg2 & PROT_EXEC)
		    dynBlock(start, arg1);
		*status = start;
		setStatReturn(ret, status);
	    } else {
		*status = ~(REG)0;
		*ret = EINVAL;
		progStop("unsupported mmap flags value: 0x%x\n", (int)arg3);
	    }
	    break;
	case SYS_MPCTL:
	    switch ((int)arg0) {
		case MPC_GETNUMSPUS:
		    *status = 1;
		    break;
		case MPC_GETFIRSTSPU:
		    *status = 1;
		    break;
		case MPC_GETNEXTSPU:
		    *status = -1;
		    break;
		case MPC_GETCURRENTSPU:
		    *status = 1;
		    break;
		case MPC_SETPROCESS:
		    *status = 0;
		    break;
		case MPC_SETPROCESS_FORCE:
		    *status = 0;
		    break;
		case MPC_SETLWP:
		    *status = 0;
		    break;
		case MPC_SETLWP_FORCE:
		    *status = 0;
		    break;
		case MPC_GETLWP_BINDINGTYPE:
		    *status = 0;
		    break;
		default:
		    errno = EINVAL;
		    *status = -1;
	    }
	    setStatReturn(ret, status);
	    break;
	case SYS_MPROTECT:
	    *ret = 0;
	    *status = 0;
	    break;
	case SYS_MUNMAP:
#if 0
fprintf(stderr, "unmapped %llx bytes from %llx (host %llx)\n",
arg1, ADDPTR(arg0), pmemLookup(page_base(ADDPTR(arg0))));
#endif
	    *status = munmap(pmemLookup(page_base(ADDPTR(arg0))),arg1);
	    {
		ADDR adr;

		for (adr = ADDPTR(arg0); adr < ADDPTR(arg0)+arg1;
		  adr += page_size)
		    memFree(adr);
	    }
	    munmapSyms(ADDPTR(arg0),ADDPTR(arg0)+arg1-1);
	    setStatReturn(ret, status);
	    break;
	case SYS_OPEN:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = open((char *)buf, arg1, arg2);
	    if ((int)*status != -1) {
#ifdef NEW_MP
		int simfd;

		for (simfd = 0; simfd < FDMAX; simfd++)
		    if (fdmap[simfd] == -1)
			break;
		fdmap[simfd] = *status;
		*status = simfd;
#if 0
fprintf(stderr, "opening file %s as fd %d (host fd %d)\n", buf, simfd, fdmap[simfd]);
#endif
#else
#ifdef HOST_MAP
		int simfd;

		for (simfd = 0; simfd < FDMAX; simfd++)
		    if (fdmap[simfd] == -1)
			break;
		fdInfo[simfd].name = strdup(buf);
		fdInfo[simfd].oflag = arg1;
		fdInfo[simfd].mode = arg2;
		fdmap[simfd] = *status;
		*status = simfd;
#else
		fdInfo[*status].name = strdup((char*)buf);
		fdInfo[*status].oflag = arg1 & ~01400;
		fdInfo[*status].mode = arg2;
		fdmap[*status] = *status;
#endif
#endif
	    }
	    setStatReturn(ret, status);
	    break;
	case SYS_PATHCONF:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = pathconf((char *)buf, arg1);
	    setStatReturn(ret, status);
	    break;
	case SYS_PIPE:
	    *status = pipe(pfd);
	    if ((int)*status != -1) {
		memBBWrt(ADDPTR(arg0), (BYTE *)pfd, sizeof pfd);
		*status = pfd[0];
		GrWrtx(9, pfd[1], 0);
	    }
	    setStatReturn(ret, status);
	    break;
	case SYS_PROFIL:
	    if (arg1 && arg3 > 1) {
		prof[0].buff  = ADDPTR(arg0);
		prof[0].len   = arg1;
		prof[0].off   = arg2;
		prof[0].scale = arg3;
		prof_insts = total_insts + prof_interval;
		prof_maxcnt = ONES(16);
		prof_mask = ~1;
		preInst |= PRE_PROFIL;
	    } else {
		prof_insts = ~0ULL;
		preInst &= ~PRE_PROFIL;
	    }
	    *status = 0;
	    setStatReturn(ret, status);
	    break;
	case SYS_PSET_CTL:
	    *status = 0;
	    setStatReturn(ret, status);
	    break;
	case SYS_PSTAT:
	    *status = 0;
	    switch ((int)arg0) {
		struct pst_static static_data;

		case PSTAT_PROC:
		    break;
		case PSTAT_STATIC:
		    *status = pstat_getstatic(&static_data, sizeof static_data, arg3, arg4);
		    if (!lp64)
			arg1 >>= 32;
		    memBBWrt(ADDPTR(arg1), &static_data, arg2);
		    break;
		case PSTAT_DYNAMIC:
		    break;
		case PSTAT_SETCMD:
		    break;
		case PSTAT_VMINFO:
		    break;
		case PSTAT_DISKINFO:
		    break;
		case PSTAT_SEMINFO:
		    break;
		case PSTAT_MSGINFO:
		    break;
		case PSTAT_SHMINFO:
		    break;
		case PSTAT_PROCESSOR:
		    break;
		case PSTAT_LVINFO:
		    break;
		case PSTAT_SWAPINFO:
		    break;
		case PSTAT_FILEINFO:
		    break;
		case PSTAT_IPCINFO:
		    break;
		case PSTAT_PROC_VM:
		    break;
		case PSTAT_STABLE:
		    break;
		default:
		    errno = EINVAL;
		    *status = -1;
	    }
	    setStatReturn(ret, status);
	    break;
	case SYS_READ:
	    chkMalargbuf(arg2);
	    nbytes = read(fdmap[arg0], malargbuf, arg2);
#if 0
fprintf(stderr, "reading from fd %d (host fd %d) into %llx\n", (int)arg0, fdmap[arg0], arg1);
#endif
	    *status = nbytes;
	    if ((int)nbytes > 0)
		memBBWrt(ADDPTR(arg1), malargbuf, nbytes);
	    setStatReturn(ret, status);
	    break;
	case SYS_READLINK:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    chkMalargbuf(arg2);
	    *status = readlink((char *)buf, (char *)malargbuf, arg2);
	    if ((int)*status != -1)
		memBBWrt(ADDPTR(arg1), malargbuf, (int)*status);
	    setStatReturn(ret, status);
	    break;
	case SYS_READV: {
	    int i;

	    /* XXX - check arg2 in 1..IOV_MAX? */
	    *status = 0;
	    for (i = 0; i < arg2; i++) {
		ADDR addr = 0;
		DWORD len = 0;

		if (lp64) {
		    memBBRd(arg1, (BYTE *)&addr, 8);
		    arg1 += 8;
		    memBBRd(arg1, (BYTE *)&len, 8);
		    arg1 += 8;
		} else {
		    memBBRd(ADDPTR(arg1), (BYTE *)&addr+4, 4);
		    arg1 += 4;
		    memBBRd(ADDPTR(arg1), (BYTE *)&len+4, 4);
		    arg1 += 4;
		}
		chkMalargbuf(len);
		nbytes = read(fdmap[arg0], malargbuf, len);
		if ((int)nbytes < 0) {
		    *status = nbytes;
		    break;
		}
		*status += nbytes;
		memBBWrt(ADDPTR(addr), malargbuf, nbytes);
		if ((int)nbytes < len)
		    break;
	    }
	    setStatReturn(ret, status);
	    break;
	}
	case SYS_RECV:
	    *status = recv(fdmap[arg0], buf, arg2, arg3);
	    memBBWrt(ADDPTR(arg1), buf, arg2);
	    setStatReturn(ret, status);
	    break;
	case SYS_RECVFROM:
	    memBBRd(ADDPTR(arg5), (BYTE *)&size, sizeof (int));
	    *status = recvfrom(fdmap[arg0], buf, arg2, arg3, buf2, &size);
	    memBBWrt(ADDPTR(arg1), buf, arg2);
	    memBBWrt(ADDPTR(arg4), buf2, size);
	    memBBWrt(ADDPTR(arg5), (BYTE *)&size, sizeof (int));
	    setStatReturn(ret, status);
	    break;
#if 0
	case SYS_RECVMSG:				/* fill in */
	    break;
#endif
	case SYS_RENAME:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    memBBRd(ADDPTR(arg1), buf2, 0);
	    *status = rename((char *)buf, (char *)buf2);
	    setStatReturn(ret, status);
	    break;
	case SYS_RMDIR:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = rmdir((char *)buf);
	    setStatReturn(ret, status);
	    break;
	case SYS_SCHED_GET_PRIORITY_MAX:
	    *status = sched_get_priority_max(arg0);
	    setStatReturn(ret, status);
	    break;
	case SYS_SCHED_GET_PRIORITY_MIN:
	    *status = sched_get_priority_min(arg0);
	    setStatReturn(ret, status);
	    break;
	case SYS_SELECT:
	    if (arg1) {
		memBBRd(ADDPTR(arg1), buf, SIZE_FD(arg0));
		readfds = mapSelectFds(buf, arg0);
		rfds = &readfds;
	    } else
		rfds = NULL;
	    if (arg2) {
		memBBRd(ADDPTR(arg2), buf, SIZE_FD(arg0));
		writefds = mapSelectFds(buf, arg0);
		wfds = &writefds;
	    } else
		wfds = NULL;
	    if (arg3) {
		memBBRd(ADDPTR(arg3), buf, SIZE_FD(arg0));
		errorfds = mapSelectFds(buf, arg0);
		efds = &errorfds;
	    } else
		efds = NULL;
	    if (arg4)
		if (lp64) {
		    memBBRd(arg4, buf, sizeof (struct timeval64));
		    tmout = timeval64to32(buf);
		} else {
		    memBBRd(ADDPTR(arg4), buf, sizeof (struct timeval));
		    tmout = (struct timeval *)buf;
		}
	    else
		tmout = NULL;
	    *status = select(arg0, (fd_set *)rfds, (fd_set *)wfds, (fd_set *)efds,
			     tmout);
	    if (rfds)
		memBBWrt(ADDPTR(arg1), (BYTE *)rfds, SIZE_FD(arg0));
	    if (wfds)
		memBBWrt(ADDPTR(arg2), (BYTE *)wfds, SIZE_FD(arg0));
	    if (efds)
		memBBWrt(ADDPTR(arg3), (BYTE *)efds, SIZE_FD(arg0));
	    setStatReturn(ret, status);
	    break;
	case SYS_SEND:
	    memBBRd(ADDPTR(arg1), buf, arg2);
	    *status = send(fdmap[arg0], buf, arg2, arg3);
	    setStatReturn(ret, status);
	    break;
#if 0
	case SYS_SENDMSG:				/* fill in */
	    break;
#endif
	case SYS_SENDTO:
	    memBBRd(ADDPTR(arg1), buf, arg2);
	    memBBRd(ADDPTR(arg4), buf2, arg5);
	    *status = sendto(fdmap[arg0], buf, arg2, arg3, buf2, arg5);
	    setStatReturn(ret, status);
	    break;
	case SYS_SETITIMER:
	    *ret = 0;
	    *status = 0;
	    if (arg0 == ITIMER_REAL) {
		CTR interval;

		memBBRd(ADDPTR(arg1), buf,
			lp64 ? 32 : sizeof (struct itimerval));
		interval = lp64 ? *(ADDR *)buf
				: ((struct itimerval *)buf)->it_interval.tv_sec;
		interval *= 1000000;
		interval += lp64 ? *(ADDR *)((char *)buf+8)
				 : ((struct itimerval *)buf)->it_interval.tv_usec;
		if (interval) {
		    alarm_interval = interval * mips;
		    alarm_insts = total_insts + alarm_interval;
		    preInst |= PRE_ALARM;
		} else {
		    alarm_insts = ~0ULL;
		    preInst &= ~PRE_ALARM;
		}
	    } else {
		(void)sprintf(msg, "ignoring system call %d\n", (int)num);
		(void)sysWrite(STDERR_FILENO, msg, strlen(msg));
	    }
	    break;
#if 0
	case SYS_SETPRGP3:
 getpgrp()  == setpgrp3(0)
 setpgrp3() == setpgrp3(1)
 setsid()   == setpgrp3(2)
	    break;
#endif
	case SYS_SETRLIMIT:
	    if (lp64) {
		memBBRd(arg1, buf, sizeof (struct rlimit64));
		rlimbuf = rlimit64to32((struct rlimit64 *)buf);
	    } else
		memBBRd(ADDPTR(arg1), (BYTE *)&rlimbuf, sizeof (struct rlimit));
	    switch ((int)arg0) {
#ifndef HPUX9
		case RLIMIT_CORE:
		    break;
		case RLIMIT_CPU:
		    break;
		case RLIMIT_DATA:
		    break;
		case RLIMIT_FSIZE:
		    break;
#endif
		case RLIMIT_NOFILE:
		    *status = setrlimit(arg0, &rlimbuf);
		    break;
#ifndef HPUX9
		case RLIMIT_STACK:
		    break;
		case RLIMIT_RSS:
		    break;
		case RLIMIT_AS:
		    break;
#endif
		default:
		    errno = EINVAL;
		    *status = -1;
	    }
	    setStatReturn(ret, status);
	    break;
	case SYS_SETSOCKOPT:
	    memBBRd(ADDPTR(arg3), buf, arg4);
	    *status = setsockopt(fdmap[arg0], arg1, arg2, buf, arg4);
	    setStatReturn(ret, status);
	    break;
	case SYS_SHMAT:
	    *status = (REG)(long)shmat(arg0, (void *)(long)ADDPTR(arg1), arg2);
	    {
#ifdef OLD_WAY
		ADDR adr;
#endif
        ADDR res = ADDPTR(*status);
		int sz;

		sz = getShmSz(arg0);
#ifdef OLD_WAY
		*status = shm_addr;
		/* XXX - If ever use this, change to use memMPMap call */
		for (adr = page_base(res); adr < res+sz; adr += page_size) {
		    (void)pmemMap(shm_addr, adr);
		    shm_addr += page_size;
		}
#else
		*status = res;
		memMPMap(res, (char*)(long)res, sz);
#endif
	    }
	    setStatReturn(ret, status);
	    break;
	case SYS_SHMCTL:
	    switch ((int)arg1) {
		case IPC_STAT:
		    break;
		case IPC_SET:
		    break;
		case IPC_RMID:
		    *status = shmctl(arg0, arg1, 0);
		    break;
		case SHM_LOCK:
		    break;
		case SHM_UNLOCK:
		    break;
		default:
		    errno = EINVAL;
		    *status = -1;
		    break;
	    }
	    setStatReturn(ret, status);
	    break;
	case SYS_SHMDT:
	    *status = shmdt((void *)(long)ADDPTR(arg0));
	    setStatReturn(ret, status);
	    break;
	case SYS_SHMGET:
	    *status = shmget(arg0, arg1, arg2);
	    setStatReturn(ret, status);
	    break;
	case SYS_SHUTDOWN:
	    *status = shutdown(fdmap[arg0], arg1);
	    setStatReturn(ret, status);
	    break;
	case SYS_SIGACTION:
	    *ret = 0;
	    *status = 0;
	    if (arg0 == SIGALRM) {
		memBBRd(ADDPTR(arg1), buf, sizeof (struct sigaction));
		if (lp64)
		    sigHdlr = *(ADDR *)buf;
		else
		    sigHdlr = (ADDR)(long)(((struct sigaction *)buf)->sa_handler)+8;
		if (!memMRd(ADDPTR(sigHdlr), 8, &sigHdlr)) {
		    errno = EFAULT;
		    *status = -1;
		    setStatReturn(ret, status);
		}
	    } else {
		(void)sprintf(msg, "ignoring system call %d\n", (int)num);
		(void)sysWrite(STDERR_FILENO, msg, strlen(msg));
	    }
	    break;
	case SYS_SIGBLOCK:
	case SYS_SIGCLEANUP:
	case SYS_SIGPAUSE:
	case SYS_SIGPENDING:
	case SYS_SIGPROCMASK:
	case SYS_SIGSETMASK:
	case SYS_SIGSTACK:
	case SYS_SIGSUSPEND:
	    (void)sprintf(msg, "ignoring system call %d\n", (int)num);
	    (void)sysWrite(STDERR_FILENO, msg, strlen(msg));
	    *ret = 0;
	    *status = 0;
	    break;
	case SYS_LWP_GETSCHEDULER:
	case SYS_SET_USERTHREADID:
	case SYS_SIGENABLE:
	case SYS_SIGINHIBIT:
	case SYS_SIGSETRETURN:
	    *ret = 0;
	    *status = 0;
	    break;
	case SYS_SIGSETSTATEMASK:
	    if (arg0 != 0x17) {
		errno = EINVAL;
		*status = -1;
		setStatReturn(ret, status);
		break;
	    }
	    if (arg2) {
		/* fill in *arg2 using memBBWrt */;
	    }
	    *ret = 0;
	    *status = 0;
	    break;
	case SYS_SIGVECTOR:
	    *ret = 0;
	    *status = 0;
	    if (arg0 == SIGFPE) {
		memBBRd(ADDPTR(arg1), buf, sizeof (struct sigvec));
		if (lp64)
		    sigHdlr = *(ADDR *)buf;
		else
		    sigHdlr = (ADDR)(long)(((struct sigvec *)buf)->sv_handler)+8;
		if (!memMRd(ADDPTR(sigHdlr), 8, &sigHdlr)) {
		    errno = EFAULT;
		    *status = -1;
		    setStatReturn(ret, status);
		}
	    } else {
		(void)sprintf(msg, "ignoring system call %d\n", (int)num);
		(void)sysWrite(STDERR_FILENO, msg, strlen(msg));
	    }
	    break;
	case SYS_SOCKET:
	    *status = socket(arg0, arg1, arg2);
	    fdmap[*status] = *status;
	    setStatReturn(ret, status);
	    break;
	case SYS_SOCKETPAIR:
	    *status = socketpair(arg0, arg1, arg2, (int *)buf);
	    memBBWrt(ADDPTR(arg3), buf, 8);	/* store 2 ints */
	    setStatReturn(ret, status);
	    break;
	case SYS_SPROFIL:
	    if (!arg0 || (arg3 != PROF_USHORT && arg3 != PROF_UINT)) {
		errno = EINVAL;
		*status = -1;
		setStatReturn(ret, status);
		break;
	    }
	    if (arg2)	/* tvp */
		/* fill in timeval */;
	    if (arg1) {
#if 0
		int i;

		for (i = 0; i < arg1; i++) {
		    memMRd(ADDPTR(arg0+32*i), &prof[i].base);
		}
#endif
		prof_insts = total_insts + prof_interval;
		prof_maxcnt = ONES(8*arg3);
		prof_mask = ~(arg3-1);
		preInst |= PRE_PROFIL;
	    } else {
		prof_insts = ~0ULL;
		preInst &= ~PRE_PROFIL;
	    }
	    *status = 0;
	    setStatReturn(ret, status);
	    break;
	case SYS_STAT:
	case SYS_STAT64:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = stat((char *)buf, &statbuf);
#ifdef CONSTANT_TIME
statbuf.st_atime = statbuf.st_mtime;
#endif
	    if ((int)*status != -1) {
		if (lp64)
		    memBBWrt(arg1, stat32to64(statbuf),
			     sizeof(struct stat64));
		else if (num == SYS_STAT64)
		    memBBWrt(ADDPTR(arg1), stat32to64x(statbuf), sizeof(struct stat64x));
		else
		    memBBWrt(ADDPTR(arg1), (BYTE *)&statbuf, sizeof(statbuf));
	    }
	    setStatReturn(ret, status);
	    break;
#if 0
	case SYS_STATFS:				/* fill in */
	    break;
#endif
	case SYS_SYMLINK:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    memBBRd(ADDPTR(arg1), buf2, 0);
	    *status = symlink((char *)buf, (char *)buf2);
	    setStatReturn(ret, status);
	    break;
	case SYS_SYSCONF:
	    switch ((int)arg0) {
		case _SC_CPU_VERSION:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
		case _SC_THREAD_THREADS_MAX:
		    *status = 1;
		    break;
		case _SC_THREAD_STACK_MIN:
		    *status = PTHREAD_STACK_MIN;
		    break;
		case _SC_THREAD_KEYS_MAX:
		    *status = PTHREAD_KEYS_MAX;
		    break;
		case _SC_CLK_TCK:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
		case _SC_CLOCKS_PER_SEC:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
#ifdef HPUX1100
		case _SC_XBS5_ILP32_OFF32:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
		case _SC_XBS5_ILP32_OFFBIG:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
		case _SC_XBS5_LP64_OFF64:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
		case _SC_XBS5_LPBIG_OFFBIG:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
#endif
		case _SC_IO_TYPE:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
		case _SC_MCAS_OFFSET:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
		case _SC_CPU_KEYBITS1:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
#ifdef HPUX1100
		case _SC_KERNEL_BITS:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
		case _SC_KERNEL_IS_BIGENDIAN:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
		case _SC_HW_32_64_CAPABLE:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
		case _SC_INT_MIN:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
		case _SC_INT_MAX:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
		case _SC_LONG_MIN:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
		case _SC_LONG_MAX:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
		case _SC_SSIZE_MAX:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
		case _SC_WORD_BIT:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
		case _SC_LONG_BIT:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
		case _SC_CPU_CHIP_TYPE:
		    *status = CPU_HP_INTEL_EM_1_0;
		    break;
#endif
		default:
		    *status = sysconf(arg0);	/* XXX - call __sysconf? */
		    break;
	    }
	    setStatReturn(ret, status);
	    break;
#ifndef HPUX9
	case SYS_SYSFS:
	    switch ((int)arg0) {
		case GETFSIND:
		    memBBRd(ADDPTR(arg1), buf, 0);
		    *status = sysfs(arg0, (char *)buf);
		    break;
		case GETFSTYP:
		    *status = sysfs(arg0, arg1, (char *)buf);
		    memBBWrt(ADDPTR(arg2), buf, strlen((char *)buf) + 1);
		    break;
		case GETNFSTYP:
		    *status = sysfs(arg0);
		    break;
		default:
		    errno = EINVAL;
		    *status = -1;
		    break;
	    }
	    setStatReturn(ret, status);
	    break;
	case SYS_SYSINFO:
	    switch ((int)arg0) {
		case _SI_MACHINE_MODEL:
		    /* XXX - if arg2 < sizeof "ia64", need to truncate */
		    *status = sizeof "ia64";
		    (void)strcpy((char *)buf, "ia64");
		    break;
		default:
		    /* XXX - what if buf not large enough, but arg1 is? */
		    *status = sysinfo(arg0, buf, sizeof buf);
		    break;
	    }
	    /* XXX - truncate if arg2 < *status? */
	    memBBWrt(ADDPTR(arg1), buf, *status);
	    setStatReturn(ret, status);
	    break;
#endif
	case SYS_TIME:
#if 0
	    if (arg0) {
		*status = time((time_t *)buf);
#ifdef CONSTANT_TIME
buf[0] = 0;
buf[1] = 0;
buf[2] = 0;
buf[3] = 0;
#endif
		memBBWrt(ADDPTR(arg0), buf, sizeof (time_t));
	    } else
		*status = time(NULL);
#ifdef CONSTANT_TIME
*status = 0;
#endif
#else
{
	    SimTime st;
	    getSimTime(&st);
	    if (arg0)
		memBBWrt(ADDPTR(arg0), (BYTE *)&st.simSec, 4);
	    *status = st.simSec;
}
#endif
	    setStatReturn(ret, status);
	    break;
	case SYS_TIMES:
#if 0
	    *status = times(&tmsbuf);
#ifdef CONSTANT_TIME
tmsbuf.tms_utime = 0;
tmsbuf.tms_stime = 0;
tmsbuf.tms_cutime = 0;
tmsbuf.tms_cstime = 0;
*status = 0;
#endif
	    if ((int)*status != -1)
		memBBWrt(ADDPTR(arg0), (BYTE *)&tmsbuf, sizeof tmsbuf);
#else
	    tmsbuf.tms_utime = total_insts / (mips * 10000ULL);
	    tmsbuf.tms_stime = 0;
	    tmsbuf.tms_cutime = 0;
	    tmsbuf.tms_cstime = 0;
	    memBBWrt(ADDPTR(arg0), (BYTE *)&tmsbuf, sizeof tmsbuf);
	    *status = 0;
#endif
	    setStatReturn(ret, status);
	    break;
	case SYS_TRUNCATE:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = truncate((char *)buf, arg1);
	    setStatReturn(ret, status);
	    break;
	case SYS_TRUNCATE64:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = truncate((char *)buf, arg2);
	    setStatReturn(ret, status);
	    break;
	case SYS_ULIMIT:
	    *status = ulimit(arg0, (long)arg1);
	    setStatReturn(ret, status);
	    break;
	case SYS_UMASK:
	    *ret = umask(arg0);
	    *status = 0;
	    break;
	case SYS_UMOUNT:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = umount((char *)buf);
	    setStatReturn(ret, status);
	    break;
	case SYS_UNLINK:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    *status = unlink((char *)buf);
	    setStatReturn(ret, status);
	    break;
	case SYS_UTIME:
	    memBBRd(ADDPTR(arg0), buf, 0);
	    memBBRd(ADDPTR(arg1), (BYTE *)&utimbuf, sizeof utimbuf);
	    *status = utime((char *)buf, &utimbuf);
	    setStatReturn(ret, status);
	    break;
	case SYS_UTSSYS:
	    switch ((unsigned)arg2) {
		case 0:		/* uname */
		    *status = _utssys((char *)buf, arg1, arg2);
		    memBBWrt(ADDPTR(arg0), buf, sizeof (struct utsname));
		    break;
		case 2:		/* ustat */
		    *status = _utssys((char *)buf, arg1, arg2);
		    if (lp64)
			memBBWrt(arg0, ustat32to64(buf),
				 sizeof (struct ustat64));
		    else
			memBBWrt(ADDPTR(arg0), buf, sizeof (struct ustat));
		    break;
		case 3:		/* setuname */
		case 4:		/* sethostname */
		    memBBRd(ADDPTR(arg0), buf, arg1);
		    *status = _utssys((char *)buf, arg1, arg2);
		    break;
		case 5:		/* gethostname */
		    *status = _utssys((char *)buf, arg1, arg2);
		    memBBWrt(ADDPTR(arg0), buf, arg1);
		    break;
		case 6:		/* set sysname field (called from setuname) */
		    break;
		case 7:		/* set uname release field (undocumented and
				   used by SD -- does IA-64 need it?) */
		    break;
		case 1:		/* was umask */
		default:
		    errno = EFAULT;
		    *status = -1;
		    break;
	    }
	    setStatReturn(ret, status);
	    break;
#ifdef NEW_MP
	case SYS_WAIT:
	    {
		int i;

		/* see if there are children */
		for (i = 0; i < numpids; i++)
		    if (mpState[i].parentPid_ == curPid+2)
			break;
		if (i == numpids) {	/* no children found */
		    errno = ECHILD;
		    *status = -1;
		} else {		/* children found */
		    /* see if any have already exited & not been waited for */
		    for (; i < numpids; i++)
			if (mpState[i].parentPid_ == curPid+2 &&
			    (mpState[i].nextPid_ == ~0U) &&
			    (mpState[i].status_ & 0x10000))
			    break;
		    if (i != numpids) {
			*status = i+2;
			mpState[i].status_ &= ~0x10000;
			if (arg0) {
			    int childstat = mpState[i].status_;

			    memBBWrt(ADDPTR(arg0),
				     (BYTE *)&childstat, sizeof (int));
			}
		    } else {
			/* see if any are still running */
			for (i = 0; i < numpids; i++)
			    if (mpState[i].parentPid_ == curPid+2 &&
				(mpState[i].nextPid_ != ~0U))
				break;
			if (i != numpids) {
			    ip += 4;
			    for (i = 0; i < numpids; i++)
			    if (mpState[i].nextPid_ == curPid) {
				unsigned newPid = nextPid;

				mpState[i].nextPid_ = newPid;
				curPid = newPid;
				break;
			    }
			    lastPid = ~0U;
			    for (i = 0; i < numpids; i++)
				if (mpState[i].nextPid_ != ~0U)
				    lastPid = i;
			    progPause();
			} else {
			    errno = ECHILD;
			    *status = -1;
			}
		    }
		}
	    }
	    setStatReturn(ret, status);
	    break;
	case SYS_WAIT3:
	    errno = ECHILD;
	    *status = -1;
	    setStatReturn(ret, status);
	    break;
	case SYS_WAITID:
	    errno = ECHILD;
	    *status = -1;
	    setStatReturn(ret, status);
	    break;
	case SYS_WAITPID:
	    errno = ECHILD;
	    *status = -1;
	    setStatReturn(ret, status);
	    break;
#endif
	case SYS_WRITE:
	    chkMalargbuf(arg2);
	    memBBRd(ADDPTR(arg1), malargbuf, arg2);
	    *status = sysWrite(fdmap[arg0], malargbuf, arg2);
	    setStatReturn(ret, status);
	    break;
	case SYS_WRITEV: {
	    int i;

	    /* XXX - check arg2 in 1..IOV_MAX? */
	    *status = 0;
	    for (i = 0; i < arg2; i++) {
		ADDR addr = 0;
		DWORD len = 0;

		if (lp64) {
		    memBBRd(arg1, (BYTE *)&addr, 8);
		    arg1 += 8;
		    memBBRd(arg1, (BYTE *)&len, 8);
		    arg1 += 8;
		} else {
		    memBBRd(ADDPTR(arg1), (BYTE *)&addr+4, 4);
		    arg1 += 4;
		    memBBRd(ADDPTR(arg1), (BYTE *)&len+4, 4);
		    arg1 += 4;
		}
		chkMalargbuf(len);
		memBBRd(ADDPTR(addr), malargbuf, len);
		nbytes = sysWrite(fdmap[arg0], malargbuf, len);
		if ((int)nbytes < 0) {
		    *status = nbytes;
		    break;
		}
		*status += nbytes;
	    }
	    setStatReturn(ret, status);
	    break;
	    }
	default:
	    *status = ~(REG)0;
	    *ret = EINVAL;
	    progStop("unsupported system call %d\n", (int)num);
	    break;
    }
}

#define TDELTA	100000

static SscReqNode *newSscReq(unsigned type, int cnt, unsigned fd, unsigned size)
{
    SscReqNode *p;

    if ((p = malloc(sizeof *p))) {
	p->type    = type;
	p->irptcnt = cnt;
	p->fd      = fd;
	p->count   = size;
	p->next    = NULL;
    }
    return p;
}

void addSscReq(SscReqNode *node)
{
    SscReqNode *p, *q;
    int t = 0;

#if 0
printf("(%llu) Adding SSC Req: type = %d, cnt = %d\n",
total_insts, (int)node->type, (int)node->irptcnt);
#endif
    for (p = sscPend; p; q = p, p = p->next) {
	if (node->irptcnt < t + p->irptcnt) {
	    node->irptcnt -= t;
	    node->next = p;
	    p->irptcnt -= node->irptcnt;
	    if (p == sscPend)
		sscPend = node;
	    else
		q->next = node;
	    return;
	}
	if (node->irptcnt == (t + p->irptcnt))
#if 0
	cmdWarn("Two interrupts at same time\n");
#endif
	t += p->irptcnt;
    }
    node->irptcnt -= t;
    node->next = p;
    if (sscPend)
	q->next = node;
    else
	sscPend = node;
}

static void delSscReq(SscReqNode *p)
{
#if 0
printf("(%llu) Deleting SSC Req: type = %d, cnt = %d\n",
total_insts, (int)p->type, (int)p->irptcnt);
#endif
    if (p == sscPend)
	sscPend = sscPend->next;
    else {
	SscReqNode *q;

	for (q = sscPend; q->next != p; q = q->next)
	    ;
	q->next = p->next;
    }
}

static int getPty(char master[], char slave[])
{
    int i, j, mfd, sfd;

    for (i = 0; i < 11; i++) {
	master[13] = slave[12] = "pqrstuvwxyz"[i];
	for (j = 0; j < 16; j++) {
	    master[14] = slave[13] = "0123456789abcdef"[j];
	    if ((mfd = open(master, O_RDWR | O_EXCL)) < 0)
		continue;
	    if ((sfd = open(slave, O_RDWR | O_EXCL)) < 0) {
		(void)close(mfd);
		continue;
	    }
	    (void)close(sfd);
	    return mfd;
	}
    }
    progExit("Unable to open console window\n");
    /*NOTREACHED*/
    return -1;
}

void closeConsole(void)
{
    if (cons_pid > 0)
	(void)kill(cons_pid, SIGTERM);
    if (noConsole && isatty(0))
	(void)tcsetattr(0, TCSANOW, &origttyIos);
}

static void initNoConsole(void)
{
    struct termios ttyIos;

    if (!isatty(0)) {
	cons_pid = 0;
	setvbuf(stdout, NULL, _IOLBF, 0);
	return;
    }
    if (tcgetattr(0, &origttyIos) == -1)
	progExit("Unable to get console attributes\n");

    ttyIos.c_iflag = (BRKINT | IGNPAR | ISTRIP | IXON);
    ttyIos.c_oflag = OPOST;
    ttyIos.c_cflag = (B38400 | CS8 | PARENB | HUPCL | CREAD);
			/* Change back to B9600? */
    ttyIos.c_lflag = (ISIG | IEXTEN);

    ttyIos.c_cc[VMIN] = 0;
    ttyIos.c_cc[VTIME] = 0;

    /* initialize special characters */
    ttyIos.c_cc[VINTR]	= CINTR;
    ttyIos.c_cc[VQUIT]	= CQUIT;
    ttyIos.c_cc[VERASE]	= CERASE;
    ttyIos.c_cc[VKILL]	= CKILL;
    ttyIos.c_cc[VEOF]	= CEOF;
    ttyIos.c_cc[VEOL]	= CNUL;
    ttyIos.c_cc[VEOL2]	= CNUL;		/* non-default */
    ttyIos.c_cc[VSTART]	= CSTART;
    ttyIos.c_cc[VSTOP]	= CSTOP;
    ttyIos.c_cc[VSWTCH]	= CSWTCH;
    ttyIos.c_cc[VWERASE]	= CERASE;
    ttyIos.c_cc[VLNEXT]	= CLNEXT;
    ttyIos.c_cc[VDSUSP]	= CDSUSP;
    ttyIos.c_cc[VSUSP]	= CSUSP;
    if (tcsetattr(0, TCSANOW, &ttyIos) == -1)
	progExit("Unable to set console attributes\n");
    (void)atexit(closeConsole);
    cons_pid = 0;
    setvbuf(stdout, NULL, _IOLBF, 0);
}

static int initConsole(void)
{
    char master[] = "/dev/ptym/ptyxx";
    char slave[]  = "/dev/pty/ttyxx";
    int mfd = getPty(master, slave);
    int pfd[2];

    if (pipe(pfd) == -1)
	return -1;
    if ((cons_pid = fork()) == 0) {	/* Child */
	struct termios ttyIos;
	char cmdBuf[80];

	ttyIos.c_iflag = (BRKINT | IGNPAR | ISTRIP | IXON);
	ttyIos.c_oflag = OPOST;
	ttyIos.c_cflag = (B38400 | CS8 | PARENB | HUPCL | CREAD);
			/* Change back to B9600? */
	ttyIos.c_lflag = (ISIG | IEXTEN);

	ttyIos.c_cc[VMIN] = 0;
	ttyIos.c_cc[VTIME] = 0;

	/* initialize special characters */
	ttyIos.c_cc[VINTR]	= CINTR;
	ttyIos.c_cc[VQUIT]	= CQUIT;
	ttyIos.c_cc[VERASE]	= CERASE;
	ttyIos.c_cc[VKILL]	= CKILL;
	ttyIos.c_cc[VEOF]	= CEOF;
	ttyIos.c_cc[VEOL]	= CNUL;
	ttyIos.c_cc[VEOL2]	= CNUL;		/* non-default */
	ttyIos.c_cc[VSTART]	= CSTART;
	ttyIos.c_cc[VSTOP]	= CSTOP;
	ttyIos.c_cc[VSWTCH]	= CSWTCH;
	ttyIos.c_cc[VWERASE]	= CERASE;
	ttyIos.c_cc[VLNEXT]	= CLNEXT;
	ttyIos.c_cc[VDSUSP]	= CDSUSP;
	ttyIos.c_cc[VSUSP]	= CSUSP;

#if 0
/* print current c_cc values */
	printf("c_cc[VINTR]  = \\x%02x\n", ttyIos.c_cc[VINTR]);
	printf("c_cc[VQUIT]  = \\x%02x\n", ttyIos.c_cc[VQUIT]);
	printf("c_cc[VERASE] = \\x%02x\n", ttyIos.c_cc[VERASE]);
	printf("c_cc[VKILL]  = \\x%02x\n", ttyIos.c_cc[VKILL]);
	printf("c_cc[VEOF]   = \\x%02x\n", ttyIos.c_cc[VEOF]);
	printf("c_cc[VEOL]   = \\x%02x\n", ttyIos.c_cc[VEOL]);
	printf("c_cc[VEOL2]  = \\x%02x\n", ttyIos.c_cc[VEOL2]);
	printf("c_cc[VSWTCH] = \\x%02x\n", ttyIos.c_cc[VSWTCH]);
	printf("c_cc[VWERASE]= \\x%02x\n", ttyIos.c_cc[VWERASE]);
	printf("c_cc[VLNEXT] = \\x%02x\n", ttyIos.c_cc[VLNEXT]);
	printf("c_cc[VDSUSP] = \\x%02x\n", ttyIos.c_cc[VDSUSP]);
	printf("c_cc[VMIN]   = \\x%02x\n", ttyIos.c_cc[VMIN]);
	printf("c_cc[VTIME]  = \\x%02x\n", ttyIos.c_cc[VTIME]);
	printf("c_cc[VSUSP]  = \\x%02x\n", ttyIos.c_cc[VSUSP]);
	printf("c_cc[VSTART] = \\x%02x\n", ttyIos.c_cc[VSTART]);
	printf("c_cc[VSTOP]  = \\x%02x\n", ttyIos.c_cc[VSTOP]);
#endif

	(void)close(2);
	(void)dup(pfd[1]);
	(void)close(1);
	(void)dup(pfd[1]);
	(void)close(pfd[0]);
	(void)close(0);
	(void)setpgrp();
	(void)umask(0);
	if (tcsetattr(mfd, TCSANOW, &ttyIos) == -1)
	    progExit("Unable to set console attributes\n");
	(void)sprintf(cmdBuf, "-S %s.%d", &master[13], mfd);
if (consLog) {
	if (execlp("hpterm", "hpterm", "-name", "Console", "-l", "-lf", consLog, cmdBuf, (char *)0)
	    == -1) {
	    cons_pid = -1;
	    return -1;
	}
} else {
	if (execlp("hpterm", "hpterm", "-name", "Console", cmdBuf, (char *)0)
	    == -1) {
	    cons_pid = -1;
	    return -1;
	}
}
	/*NOTREACHED*/
    } else if (cons_pid != -1) {	/* Parent */
	(void)close(pfd[1]);
	(void)close(mfd);
#if 0
	alarm(2);
	pause();
	alarm(0);
#endif
	(void)sleep(3);
	(void)atexit(closeConsole);
	return open(slave, O_RDWR | O_EXCL);
    }
    return -1;
}

/*ARGSUSED*/
static void asynckb(int sig)
{
#if 0
    cmdWarn("(%lld) Signal %d received.  Pending keyboard interrupt\n",
	    total_insts, sig);
#endif
    (void)signal(SIGIO, asynckb);
    intrsim = kybdint = YES;
}

static void readConsole(char *buf)
{
    if (noConsole)
	buf[0] = getchar();
    else if (!read(cfd, buf, 1))
	buf[0] = '\0';
}

static void writeConsole(char *buf)
{
#if 1
    if (cons_pid == -1) {
	if (noConsole)
	    initNoConsole();
	else if ((cfd = initConsole()) == -1)
	    progExit("Unable to open console window\n");
    }
#endif
    if (noConsole)
	(void)fprintf(stdout, "%s", buf);
    else
	(void)write(cfd, buf, strlen(buf));
}

static BOOL completeXfer(unsigned fd, WORD *count)
{
    SscReqNode *p;

    for (p = sscPend; p; p = p->next)
	if (p->type == DISK_IRPT && p->fd == fd) {
	    *count = p->count;
	    delSscReq(p);
	    free(p);
	    return YES;
	}
    return NO;
}

static BOOL getXfer(unsigned *fd, WORD *count)
{
    if (sscDiskDone) {
	SscReqNode *p = sscDiskDone;

	*fd = p->fd;
	*count = p->count;
	sscDiskDone = p->next;
	free(p);
	return YES;
    }
    return NO;
}

void doSSC(HWORD num, REG arg0, REG arg1, REG arg2, REG arg3, REG *ret)
{
    BYTE buf[100];
    char msg[100];
    unsigned	i;
    size_t	nbytes;
    static struct timeval	tp;
    static struct timezone	tzp;
    ADDR	adr;
#if 0
    struct tm	*uxTimeP;
    SscTime	sscTime;
#endif
    SscTimeval	sscTimeval;
    SscTimezone	sscTimezone;
    SscDiskReq	diskReq;
    SscDiskStat	diskStat;
    unsigned	diskXfer;

    /* initialize malargbuf ptr if necessary */
    if (malargbufsize == 0) {
	if (!(malargbuf = malloc(INITMALLOC)))
	    return;     /* XXX - set errno? */
	malargbufsize = INITMALLOC;
    }

#if 0
/* debug stuff */
if (num != SSC_PUTCHAR && num != SSC_GET_TOD)
	cmdWarn("(%llu) SSC call: %d.  arg0-3: %llx, %llx, %llx, %llx\n",
		total_insts, num, arg0, arg1, arg2, arg3);
#endif
    switch (num) {
	case SSC_CLOSE:
	    *ret = close(arg0);
#if 0
	    cmdWarn("Close file descriptor: %lld\n", arg0);
#endif
	    break;
	case SSC_CONNECT_INTERRUPT: {
	    BOOL found = NO;

#if 0
fprintf(stderr, "(%llu) Connecting SSC Irpt: type = %d, bit = %d\n",
total_insts, (int)arg0, (int)arg1);
#endif
	    for (i = 0; i < irptTop && !found; i++)
		if (irptTbl[i].type == arg0) {
		    irptTbl[i].irrBit = arg1;
		    found = YES;
		}
	    if (!found) {
		irptTbl[irptTop].type   = arg0;
		irptTbl[irptTop].irrBit = arg1;
		irptTop++;
	    }
	    if (arg0 == KEYBOARD_IRPT) {
		BOOL asyncio = YES;

#if 1
		(void)read(cfd, (BYTE *)buf, sizeof buf); /* clean buffer */
#endif
		/* enable async keyboard input */
		if (noConsole)
		    (void)ioctl(0, FIOSSAIOSTAT, &asyncio);
		else
		    (void)ioctl(cfd, FIOSSAIOSTAT, &asyncio);
		(void)signal(SIGIO, asynckb);
#if 0
progStop("Kybd interrupt connected to bit %d\n", (int)arg1);
#endif
	    }
	    /* no return value */
	    break;
	    }
	case SSC_CONSOLE_INIT:
	    if (!noConsole && (*ret = cfd = initConsole()) != -1)
#if 0
		(void)read(cfd, (BYTE *)buf, sizeof buf); /* clean buffer */
#else
		;
#endif
	    if (noConsole)
		initNoConsole();
	    if (!noConsole && cfd == -1)
		progExit("Unable to open console window\n");
	    break;
	case SSC_DUMP_CMD_SETUP:
	    (void)sprintf(msg, "ignoring unimplemented SSC: %d\n", (int)num);
	    (void)sysWrite(STDERR_FILENO, msg, strlen(msg));
	    *ret = -1;
	    break;
	case SSC_EXIT:
	    GrWrtx(8, arg0, 0);
#ifdef NEW_MP
	    progExit("processor %d exited with status %llx\n", curPid, arg0);
#else
	    progExit("processor exited with status %llx\n", arg0);
#endif
	    break;
	case SSC_GENERATE_INTERRUPT:
#if 0
	    if (arg0 == CLK_TIMER_IRPT || arg0 == PROF_TIMER_IRPT)
		cmdWarn("generating a clock or profile timer interrupt\n");
	    if (arg0 == DISK_IRPT)
		cmdWarn("generating a disk interrupt\n");
#endif
	    pendSscIrpt(arg0);
	    /* no return value */
	    break;
	case SSC_GET_COMPLETION:
	    if (getXfer(&diskStat.fd, &diskStat.count)) {
		memBBWrtP(arg0, (BYTE *)&diskStat, sizeof (SscDiskStat));
		*ret = 0;
	    } else
		*ret = -1;
	    break;
#if 0
	case SSC_GET_RTC:
	    (void)gettimeofday(&tp, &tzp); 
	    uxTimeP = localtime((const time_t *)&tp.tv_sec);
	    sscTime.year = uxTimeP->tm_year;
	    sscTime.mon  = uxTimeP->tm_mon;
	    sscTime.mday = uxTimeP->tm_mday;
	    sscTime.hour = uxTimeP->tm_hour;
	    sscTime.min  = uxTimeP->tm_min;
	    sscTime.sec  = uxTimeP->tm_sec;
	    sscTime.msec = tp.tv_usec/1000;
	    sscTime.wday = uxTimeP->tm_wday;
	    memBBWrtP(arg0, (BYTE *)&sscTime, sizeof (SscTime));
	    /* no return value */
	    break;
#endif
	case SSC_GET_TOD:
	    (void)gettimeofday(&tp, &tzp); 
	    sscTimeval.ssc_Sec  = tp.tv_sec;
	    sscTimeval.ssc_Usec = tp.tv_usec;
	    sscTimezone.ssc_Minuteswest = tzp.tz_minuteswest;
	    sscTimezone.ssc_Dsttime     = tzp.tz_dsttime;
	    memBBWrtP(arg0, (BYTE *)&sscTimeval, sizeof (SscTimeval));
	    memBBWrtP(arg1, (BYTE *)&sscTimezone, sizeof (SscTimezone));
	    /* no return value */
	    break;
	case SSC_GETCHAR:
	    readConsole((char *)buf);
	    *ret = buf[0];
	    GrWrtx(32, buf[0], 0);
	    break;
	case SSC_LOAD_SYMBOLS:
	    memBBRdP(arg1, buf, 0);
	    /* arg0, arg2, and arg3 are currently not used */
	    if (elfSymLoad((char *)buf))
		*ret = 0;
	    else
		*ret = -1;
	    break;
	case SSC_OPEN:
	    memBBRdP(arg0, buf, 0);
	    if (arg1 == SSC_READ_ACCESS)
		*ret = open((char *)buf, O_RDONLY);
	    else if (arg1 == SSC_WRITE_ACCESS)
		*ret = open((char *)buf, O_WRONLY);
	    else if (arg1 == (SSC_READ_ACCESS | SSC_WRITE_ACCESS))
		*ret = open((char *)buf, O_RDWR);
	    else
		progStop("SSC_OPEN: arg1 has an illegal access type\n");
#if 0
	    cmdWarn("File opened: %s.  Return status: %lld\n",
		    (char *)buf, *ret);
#endif
#if 0
	    fprintf(stderr, "File opened: %s, Mode: %s, Return status: %lld\n",
		    (char *)buf,
	arg1 == SSC_READ_ACCESS ? "R" : arg1 == SSC_WRITE_ACCESS ? "W" : "RW",
	*ret);
#endif
	    break;
	case SSC_PUTCHAR:
	    buf[0] = arg0;
	    buf[1] = '\0';
	    writeConsole((char *)buf);
	    /* no return value */
	    break;
	case SSC_READ:
	    /* XXX - should error return be a 32- or 64-bit -1? */
	    /* XXX - is the -1 right or should it be 0xFFFFFFFF? */
	    if ((*ret = (WORD)lseek(arg0, arg3, SEEK_SET)) == -1)
		return;
	    adr = arg2;
	    *ret = 0;
	    for (i = 0, diskXfer = 0; i < arg1; i++) {
		memBBRdP(adr, (BYTE *)&diskReq, sizeof (SscDiskReq));
		if (diskReq.len > malargbufsize) {
		    if (!(malargbuf = realloc(malargbuf, diskReq.len))) {
			*ret = -1;
			break;
		    }
		    malargbufsize = diskReq.len;
		}
#if 0
cmdWarn("SSC read: reading %d bytes\n", (int)diskReq.len);
#endif
#if 0
fprintf(stderr, "SSC read: attempting to read %d bytes from %llx\n",
(int)diskReq.len, diskReq.addr);
#endif
		nbytes = read(arg0, malargbuf, diskReq.len);
#if 0
fprintf(stderr, "SSC read: return value from read: %d, errno=%d\n", (int)nbytes, errno);
#endif
		if ((int)nbytes > 0) {
		    diskXfer += nbytes;
		    memBBWrtP(diskReq.addr, malargbuf, nbytes);
{
ADDR a, b;

b = (diskReq.addr + nbytes) & ~0xFULL;
for (a = diskReq.addr & ~0xF; a <= b; a += 16)
clearPdecode(a, 16);
}
		} else if (!nbytes)
		    break;
		else {
		    *ret = -1;
		    break;
		}
		adr += sizeof (SscDiskReq);
	    }
	    if (!(sscNew = newSscReq(DISK_IRPT, IO_IRPT_CNT, arg0, diskXfer)))
		*ret = -1;
	    addSscReq(sscNew);
	    break;
	case SSC_SET_PERIODIC_INTERRUPT:
	    if (arg0 == CLK_TIMER_IRPT)
		clkTmr = arg1;
	    else if (arg0 == PROF_TIMER_IRPT)
		profTmr = arg1;
	    else
		progStop("SSC_SET_PERIODIC:"
			 " arg0 has an illegal interrupt type\n");
	    if (!arg1) {
		SscReqNode *p;

		for (p = sscPend; p; p = p->next)
		    if (p->type == arg0) {
			delSscReq(p);
			free(p);
			break;
		    }
	    } else {
		SscReqNode *p;

		for (p = sscPend; p; p = p->next)
		    if (p->type == arg0) {
			delSscReq(p);
			p->irptcnt = arg1;
			addSscReq(p);
			break;
		    }
		if (!p) {
		    sscNew = newSscReq(arg0, arg1, 0, 0);
		    addSscReq(sscNew);
		}
	    }
#if 0
	    fprintf(stderr,
		    "(%llu) Periodic interrupt: type %d, interval %llu\n",
		    total_insts, (int)arg0, arg1);
#endif
	    /* no return value */
	    break;
	case SSC_STOP:
	    progStop("SSC breakpoint\n");
	    break;
	case SSC_WAIT_COMPLETION:
	    memBBRdP(arg0, (BYTE *)&diskStat, sizeof (SscDiskStat));
	    if (completeXfer(diskStat.fd, &diskStat.count)) {
		memBBWrtP(arg0, (BYTE *)&diskStat, sizeof (SscDiskStat));
		*ret = 0;
	    } else
		*ret = -1;
	    break;
	case SSC_WRITE:
	    if ((*ret = lseek(arg0, arg3, SEEK_SET)) == -1)
		return;
	    adr = arg2;
	    for (i = 0, diskXfer = 0; i < arg1; i++) {
		memBBRdP(adr, (BYTE *)&diskReq, sizeof (SscDiskReq));
		if (diskReq.len > malargbufsize) {
		    if (!(malargbuf = realloc(malargbuf, diskReq.len))) {
			*ret = -1;
			break;
		    }
		    malargbufsize = diskReq.len;
		}
		memBBRdP(diskReq.addr, malargbuf, diskReq.len);
		nbytes = write(arg0, malargbuf, diskReq.len);
		if ((int)nbytes > 0)
		    diskXfer += nbytes;
		else if (!nbytes)
		    break;
		else {
		    *ret = -1;
		    break;
		}
		adr += sizeof (SscDiskReq);
	    }
	    if (!(sscNew = newSscReq(DISK_IRPT, IO_IRPT_CNT, arg0, diskXfer)))
		*ret = -1;
	    addSscReq(sscNew);
	    break;
	default:
	    progStop("Unsupported SSC: %d\n", (int)num);
	    break;
    }
}

void pendSscIrpt(unsigned type)
{
    unsigned i;
    REG *p = &IRR0;		/* assume IRR0-3 are contiguous */

#if 0
fprintf(stderr, "(%llu) Pending SSC Irpt: type = %u\n", total_insts, type);
#endif
    for (i = 0; i < irptTop; i++)
	if (type == irptTbl[i].type) {
	    unsigned vector = irptTbl[i].irrBit, reg, bit;

#if 0
	    fprintf(stderr,
		    "(%llu) Pend interrupt vector %u for SSC interrupt %u\n",
		    total_insts, vector, type);
#endif
	    if (vector == 1 || (vector >= 3 && vector <= 15))
		continue;	/* ignore vectors 1 and 3 - 15 */

	    reg = vector >> 6 & 0x3;
	    bit = vector & 0x3F;
	    *(p + reg) |= (REG)1 << bit;
	    break;
	}
     if (i == irptTop)
	progStop("Could not pend SSC interrupt %u\n", type);

    if (type == CLK_TIMER_IRPT) {
	SscReqNode *q = sscPend;

	sscPend = q->next;
	q->irptcnt = clkTmr;
	addSscReq(q);
    }
    if (type == PROF_TIMER_IRPT) {
	SscReqNode *q = sscPend;

	sscPend = q->next;
	q->irptcnt = profTmr;
	addSscReq(q);
    }
    if (type == DISK_IRPT) {
	SscReqNode *q = sscPend;

	sscPend = q->next;
	if (sscDiskDone) {
	    SscReqNode *p;

	    for (p = sscDiskDone; p->next; p = p->next)
		;
	    p->next = q;
	} else
	    sscDiskDone = q;
	q->next = NULL;
    }

    if (acceptIrpt())
	intrsim = extint = YES;
}

void setFdmap(int fd1, int fd2)
{
    fdmap[fd1] = fd2;
}

static void doEFI_GetTime(REG arg0, REG *ret0)
{
    *ret0 = 0;
    (void)memMWrt(arg0, 8, 0);
    (void)memMWrt(arg0+8, 8, 0);
}

static void doPAL(REG *ret0, REG *ret1, REG *ret2, REG *ret3)
{
    REG arg0, arg1, arg2, arg3;
    BOOL junk;

    GrRd(28, arg0, junk, NO);
    GrRd(29, arg1, junk, NO);
    GrRd(30, arg2, junk, NO);
    GrRd(31, arg3, junk, NO);

    *ret0 = *ret1 = *ret2 = *ret3 = 0;
    if (arg0 == 0x4ULL) { /* PAL_CACHE_SUMMARY */
	*ret1 = 3;	/* CACHE_LEVELS_MIN */
    } else if (arg0 == 0x2ULL) { /* PAL_CACHE_INFO */
	/* XXX - are these cache parameters OK? */
	switch ((int)arg1) {
	    case 0:
		*ret1 = 4<<8;	/* L1I_INST_DAT_CACHE_ASSOC */
		*ret2 = 0x4000;	/* L1I_INST_DAT_CACHE_SZ */
		break;
	    case 1:
		*ret1 = arg2 == 1 ? 0 : 6<<8;	/* L2_UNIF_CACHE_ASSOC */
		*ret2 = arg2 == 1 ? 0 : 0x18000;/* L2_UNIF_CACHE_SZ */
		break;
	    case 2:
		/* L3 Unified Cache w/ line size = 32, assoc = 4 */
		*ret1 = arg2 == 1 ? 0 : 5<<16|4<<8;
		*ret2 = arg2 == 1 ? 0 : 0x200000;/* L3_UNIF_CACHE_SZ */
		break;
	}
    } else if (arg0 == 0x8ULL) { /* PAL_VM_SUMMARY */
	*ret1 = 7ULL<<40 | 7ULL<<32 | 15ULL<<16 | 18ULL<<8;
	*ret2 = 18ULL<<8 | 50;
    } else if (arg0 == 0x9ULL) { /* PAL_BUS_GET_FEATURES */
	*ret1 = 0;	/* avail */
	*ret2 = 0;	/* status */
	*ret3 = 0;	/* control */
    } else if (arg0 == 0xBULL) { /* PAL_DEBUG_INFO */
	*ret1 = 4;
	*ret2 = 4;
    } else if (arg0 == 0xEULL) { /* PAL_FREQ_RATIOS */
	*ret1 = 0x800000002ULL;
	*ret2 = 0x100000001ULL;
	*ret3 = 0x800000002ULL;
    } else if (arg0 == 0xFULL) { /* PAL_PERF_MON_INFO */
	*ret1 = 0ULL<<24 | 0ULL<<16 | 0ULL<<8 | 4;
    } else if (arg0 == 0x11ULL) { /* PAL_PROC_GET_FEATURES */
	*ret1 = 0;	/* avail */
	*ret2 = 0;	/* status */
	*ret3 = 0;	/* control */
    } else if (arg0 == 0x14ULL) { /* PAL_VERSION */
	*ret1 = 0;	/* min_pal_ver */
#if 0
	*ret2 = 0xFFULL<<24;	/* cur_pal_ver (vendor == INTEL) */
#else
	*ret2 = 0;	/* cur_pal_ver */
#endif
    } else if (arg0 == 0x22ULL) { /* PAL_VM_PAGE_SIZE */
	*ret1 = 15557ULL << 12;
    } else
	progStop("bad PAL call\n");
}

static HWORD pciCmd;
static BYTE pciCacheLineSz, pciLatTmr;
WORD pciBar[6] = {0xFFFFE001, 0xFFFFE000, 0xFFFFE000};

static REG pciConfigRd(REG arg1, REG arg2)
{
    BYTE segno, busno, devno, fnno, regno;

    segno = BitfR(arg1,32,8);
    busno = BitfR(arg1,40,8);
    devno = BitfR(arg1,48,5);
    fnno  = BitfR(arg1,53,3);
    regno = BitfR(arg1,56,8);
    if (arg1 == 0x7800)	/* Azusa platform */
	return arg2 == 2 ? 0x1000 : 0x000c1000;
    if (arg1 == 0x7804 && arg2 == 2)	/* command */
	return pciCmd;
    if (arg1 == 0x7804 && arg2 == 4)	/* command/status */
	return pciCmd;
    if (arg1 == 0x7806 && arg2 == 2)	/* status */
	return 0x0000;
    if (arg1 == 0x7808 && arg2 == 4)	/* revision ID and class code */
	return 0x0100000A;
    if (arg1 == 0x780A && arg2 == 2)	/* class */
	return 0x0100;	/* SCSI bus controller */
    if (arg1 == 0x780C && arg2 == 2)	/* cache line size & latency timer */
	return (HWORD)pciLatTmr << 8 | pciCacheLineSz;
    if (arg1 == 0x780E && arg2 == 1)	/* header type */
	return 0x00;
    if (arg1 >= 0x7810 && arg1 <= 0x7824 && arg2 == 4)	/* BARs */
	return pciBar[(arg1-0x7810)>>2];
    if (arg1 == 0x7830 && arg2 == 4)	/* Expansion ROM */
	return 0x00000000;
    if (arg1 == 0x783D && arg2 == 1)	/* Interrupt Pin */
	return 0x01;
    if (!segno && !busno && !fnno && !regno && arg2 == 4)
	return 0xFFFFFFFF;
    progStop("bad FW call\n");
    return 0;
}

static void pciConfigWr(REG arg1, REG arg2, REG arg3)
{
    BYTE segno, busno, devno, fnno, regno;

    segno = BitfR(arg1,32,8);
    busno = BitfR(arg1,40,8);
    devno = BitfR(arg1,48,5);
    fnno  = BitfR(arg1,53,3);
    regno = BitfR(arg1,56,8);
    if (arg1 == 0x7804 && arg2 == 2) {	/* command */
	pciCmd = arg3;
	return;
    }
/* XXX - this case seems to only be currently used in a situation that
	 probably should never occur, so perhaps move this to the end?
 */
    if (arg1 == 0x780C && arg2 == 1) {	/* cache line size */
	pciCacheLineSz = BitfR(arg3,56,8);
	return;
    }
    if (arg1 == 0x780C && arg2 == 2) {	/* cache line size & latency timer */
	pciCacheLineSz = BitfR(arg3,56,8);
	pciLatTmr = BitfR(arg3,48,8);
	return;
    }
    if (arg1 == 0x7810 && arg2 == 4) {	/* I/O BAR */
	pciBar[(arg1-0x7810)>>2] = (arg3 & ~0x1FFFULL) | 1;
	return;
    }
    if (arg1 >= 0x7814 && arg1 <= 0x7818 && arg2 == 4) { /* Memory BARs */
	pciBar[(arg1-0x7810)>>2] = arg3 & ~0x1FFFULL;
	return;
    }
    if (arg1 >= 0x781C && arg1 <= 0x7824 && arg2 == 4)	/* unimpl. BARs */
	return;
    if (arg1 == 0x7830 && arg2 == 4)	/* Expansion ROM */
	return;
    progStop("bad FW call\n");
}

REG doFW(REG imm, REG arg0, REG arg1, REG arg2, REG arg3)
{
    REG ret0 = 0, ret1 = 0, ret2 = 0, ret3 = 0;

    if (ip == 0x12FFFFB0ULL)
	doEFI_GetTime(arg0, &ret0);
    else if (arg0 < 0x100ULL)	/* XXX - hack that should detect PAL calls */
	doPAL(&ret0, &ret1, &ret2, &ret3);
    else if (arg0 == 0x1000000ULL)	/* SAL_SET_VECTORS */
	;
    else if (arg0 == 0x1000001ULL)	/* SAL_GET_STATE_INFO */
	ret0 = -5ULL;
    else if (arg0 == 0x1000002ULL)	/* SAL_GET_STATE_INFO_SIZE */
	ret1 = 1;
    else if (arg0 == 0x1000003ULL)	/* SAL_CLEAR_STATE_INFO */
	;
    else if (arg0 == 0x1000005ULL)	/* SAL_MC_SET_PARAMS */
	;
    else if (arg0 == 0x1000010ULL)	/* SAL_PCI_CONFIG_READ */
	ret1 = pciConfigRd(arg1, arg2);
    else if (arg0 == 0x1000011ULL)	/* SAL_PCI_CONFIG_WRITE */
	pciConfigWr(arg1, arg2, arg3);
    else if (arg0 == 0x1000012ULL)	/* SAL_FREQ_BASE */
	ret1 = 10000000ULL/*140000000ULL*/,
	ret2 = 100ULL;
    else
	progStop("bad FW call\n");
    GrWrtx(9,  ret1, 0);
    GrWrtx(10, ret2, 0);
    GrWrtx(11, ret3, 0);
    return ret0;
}
