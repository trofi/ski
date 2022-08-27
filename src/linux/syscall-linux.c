/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- 
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


#define _FILE_OFFSET_BITS	64
#define _LARGEFILE_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <sched.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#if defined(__hpux)
#include <ustat.h>
#endif

#undef B0	/* termio.h on Linux defines this */

#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_arp.h>

#include <linux/posix_types.h>
#include <linux/personality.h>
#include <linux/sockios.h>
#include <sys/io.h>

#include <sys/file.h>
#include <sys/fsuid.h>
#include <sys/ipc.h>
#include <sys/klog.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/msg.h>
#include <sys/poll.h>
#include <sys/reboot.h>
#include <sys/resource.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/swap.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/utsname.h>
#include <sys/vfs.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/uio.h>

#include <linux/serial.h>
#include <asm/unistd.h>

#include "std.h"
#include "bits.h"
#include "types.h"
#include "fields.h"
#include "state.h"
#include "ssc.h"
#include "sim.h"
#include "simmem.h"
#include "syscall-print.h"
#include "libui.h"
#include "float.h"
#include "hpux/syscall-hpux.h"
#include "syscall-linux.h"
#include "exportui.h"
#include "coreui.h"
#include "libsrs.h"
#include "libcore.h"
#include "combfns.h"
#include "interruption.h"
#include "netdev.h"
#include "os_support.h"

#if defined NEW_MP
# error "not supported"
#endif /* defined NEW_MP */

#define MAXIRPT		30	/* size of interrupts array */
#define FDMAX		1024	/* file descriptors array size */

#define ADDPTR(addr)    (lp64 \
			   ? addr \
			   : (BitfX(addr,32,32) | BitfX(addr,32,2) << 61))

#define LIA64_IOC_NONE		0U
#define LIA64_IOC_READ		1U
#define LIA64_IOC_WRITE		2U
#define LIA64_IOC_NRBITS	8
#define LIA64_IOC_TYPEBITS	8
#define LIA64_IOC_SIZEBITS	14
#define LIA64_IOC_DIRBITS	2
#define LIA64_IOC_NRMASK	((1 << LIA64_IOC_NRBITS)-1)
#define LIA64_IOC_TYPEMASK	((1 << LIA64_IOC_TYPEBITS)-1)
#define LIA64_IOC_SIZEMASK	((1 << LIA64_IOC_SIZEBITS)-1)
#define LIA64_IOC_DIRMASK	((1 << LIA64_IOC_DIRBITS)-1)
#define LIA64_IOC_NRSHIFT	0
#define LIA64_IOC_TYPESHIFT	(LIA64_IOC_NRSHIFT+LIA64_IOC_NRBITS)
#define LIA64_IOC_SIZESHIFT	(LIA64_IOC_TYPESHIFT+LIA64_IOC_TYPEBITS)
#define LIA64_IOC_DIRSHIFT	(LIA64_IOC_SIZESHIFT+LIA64_IOC_SIZEBITS)
/* used to decode them.. */
#define LIA64_IOC_DIR(nr)	\
     (((nr) >> LIA64_IOC_DIRSHIFT) & LIA64_IOC_DIRMASK)
#define LIA64_IOC_TYPE(nr)	\
     (((nr) >> LIA64_IOC_TYPESHIFT) & LIA64_IOC_TYPEMASK)
#define LIA64_IOC_NR(nr)	\
     (((nr) >> LIA64_IOC_NRSHIFT) & LIA64_IOC_NRMASK)
#define LIA64_IOC_SIZE(nr)	\
     (((nr) >> LIA64_IOC_SIZESHIFT) & LIA64_IOC_SIZEMASK)
#define LIA64_IOC_IN		(LIA64_IOC_WRITE << LIA64_IOC_DIRSHIFT)
#define LIA64_IOC_OUT		(LIA64_IOC_READ << LIA64_IOC_DIRSHIFT)
#define LIA64_IOC_INOUT		\
     ((LIA64_IOC_WRITE|LIA64_IOC_READ) << LIA64_IOC_DIRSHIFT)
#define LIA64_IOCSIZE_MASK	(LIA64_IOC_SIZEMASK << LIA64_IOC_SIZESHIFT)
#define LIA64_IOCSIZE_SHIFT	(LIA64_IOC_SIZESHIFT)

#define LIA64_F_GETLK         5
#define LIA64_F_SETLK         6
#define LIA64_F_SETLKW        7

#define LIA64_RLIMIT_CPU      0               /* CPU time in ms */
#define LIA64_RLIMIT_FSIZE    1               /* Maximum filesize */
#define LIA64_RLIMIT_DATA     2               /* max data size */
#define LIA64_RLIMIT_STACK    3               /* max stack size */
#define LIA64_RLIMIT_CORE     4               /* max core file size */
#define LIA64_RLIMIT_RSS      5               /* max resident set size */
#define LIA64_RLIMIT_NPROC    6               /* max number of processes */
#define LIA64_RLIMIT_NOFILE   7               /* max number of open files */
#define LIA64_RLIMIT_MEMLOCK  8               /* max locked-in memory */
#define LIA64_RLIMIT_AS       9               /* address space limit */
#define LINUX_RLIM_NLIMITS    10

#define SA_ONSTACK	0x08000000

#define LIA64_FPSR_TRAP_VD	(1 << 0)	/* invalid op trap disabled */
#define LIA64_FPSR_TRAP_DD	(1 << 1)	/* denormal trap disabled */
#define LIA64_FPSR_TRAP_ZD	(1 << 2)	/* zero-divide trap disabled */
#define LIA64_FPSR_TRAP_OD	(1 << 3)	/* overflow trap disabled */
#define LIA64_FPSR_TRAP_UD	(1 << 4)	/* underflow trap disabled */
#define LIA64_FPSR_TRAP_ID	(1 << 5)	/* inexact trap disabled */
#define LIA64_FPSR_S0(x)	((x) <<  6)
#define LIA64_FPSR_S1(x)	((DWORD) (x) << 19)
#define LIA64_FPSR_S2(x)	((DWORD) (x) << 32)
#define LIA64_FPSR_S3(x)	((DWORD) (x) << 45)

/* floating-point status field: */
#define FPSF_FTZ	(1 << 0)		/* flush-to-zero */
#define FPSF_WRE	(1 << 1)		/* widest-range exponent */
#define FPSF_PC(x)	(((x) & 0x3) << 2)	/* precision control */
#define FPSF_RC(x)	(((x) & 0x3) << 4)	/* rounding control */
#define FPSF_TD		(1 << 6)		/* trap disabled */

/* floating-pint rounding control: */
#define FPRC_NEAREST	0x0
#define FPRC_NEGINF	0x1
#define FPRC_POSINF	0x2
#define FPRC_TRUNC	0x3

#define FPSF_DEFAULT	(FPSF_PC (0x3) | FPSF_RC (FPRC_NEAREST))

/* This default value is the same as HP-UX uses.  Don't change it
   without a very good reason.  */
#define LIA64_FPSR_DEFAULT						\
	(  LIA64_FPSR_TRAP_VD | LIA64_FPSR_TRAP_DD | LIA64_FPSR_TRAP_ZD	\
	 | LIA64_FPSR_TRAP_OD | LIA64_FPSR_TRAP_UD | LIA64_FPSR_TRAP_ID	\
	 | LIA64_FPSR_S0 (FPSF_DEFAULT)					\
	 | LIA64_FPSR_S1 (FPSF_DEFAULT | FPSF_TD | FPSF_WRE)		\
	 | LIA64_FPSR_S2 (FPSF_DEFAULT | FPSF_TD)			\
	 | LIA64_FPSR_S3 (FPSF_DEFAULT | FPSF_TD))

/* Format of dirent as returned by the _host_ getdents(): */

struct dirent {
	long		d_ino;
	long		d_off;
	unsigned short	d_reclen;
	char		d_name[256]; /* We must not include limits.h! */
};

struct dirent64 {
	unsigned long long d_ino;
	long long	d_off;
	unsigned short	d_reclen;
	unsigned char	d_type;
	char		d_name[256];
};

#if !defined offsetof
# define offsetof(t,f)	(((char *) ((t *) 0)->f) - (char *) 0)
#endif /* !defined offsetof */
#define dirent_name_off		offsetof(struct dirent, d_name)
#define dirent64_name_off	offsetof(struct dirent64, d_name)

typedef long long lia64_clock_t;	/* Linux/ia64 uses 64 bit clock_t */
typedef long long lia64_off_t;
typedef int lia64_pid_t;
typedef unsigned int lia64_uid_t;
typedef unsigned int lia64_gid_t;
typedef long long lia64_time_t;
typedef int lia64_suseconds_t;
typedef unsigned long long lia64_size_t;

struct lia64_tms
  {
    lia64_clock_t tms_utime;
    lia64_clock_t tms_stime;
    lia64_clock_t tms_cutime;
    lia64_clock_t tms_cstime;
  };

struct lia64_flock
  {
    short l_type;
    short l_whence;
    lia64_off_t l_start;
    lia64_off_t l_len;
    lia64_pid_t l_pid;
    int pad;
  };

struct lia64_rlimit
  {
    long long rlim_cur;
    long long rlim_max;
  };

struct lia64_timeval
  {
    lia64_time_t tv_sec;
    lia64_suseconds_t tv_usec;
    int pad;
  };

struct lia64_itimerval
  {
    struct lia64_timeval it_interval;
    struct lia64_timeval it_value;
  };

struct lia64_timespec
  {
    lia64_time_t tv_sec;
    long long tv_nsec;
  };

struct lia64_rusage
  {
    struct lia64_timeval ru_utime;	/* user time used */
    struct lia64_timeval ru_stime;	/* system time used */
    long long ru_maxrss;		/* maximum resident set size */
    long long ru_ixrss;			/* integral shared memory size */
    long long ru_idrss;			/* integral unshared data size */
    long long ru_isrss;			/* integral unshared stack size */
    long long ru_minflt;		/* page reclaims */
    long long ru_majflt;		/* page faults */
    long long ru_nswap;			/* swaps */
    long long ru_inblock;		/* block input operations */
    long long ru_oublock;		/* block output operations */
    long long ru_msgsnd;		/* messages sent */
    long long ru_msgrcv;		/* messages received */
    long long ru_nsignals;		/* signals received */
    long long ru_nvcsw;			/* voluntary context switches */
    long long ru_nivcsw;		/* involuntary " */
  };

struct lia64_fsid_t
  {
    int val[2];
  };

struct lia64_statfs
  {
    long long f_type;
    long long f_bsize;
    long long f_blocks;
    long long f_bfree;
    long long f_bavail;
    long long f_files;
    long long f_ffree;
    struct lia64_fsid_t f_fsid;
    long long f_namelen;
    long long f_spare[6];
  };

struct lia64_msgbuf
  {
    long mtype;
    char mtext[1];
  };

struct lia64_stat
  {
    unsigned long long St_dev;
    unsigned long long St_ino;
    unsigned long long St_nlink;
    unsigned int St_mode;
    unsigned int St_uid;
    unsigned int St_gid;
    unsigned int __pad0;
    unsigned long long St_rdev;
    unsigned long long St_size;
    lia64_time_t St_atime;
    unsigned long long __reserved0;
    lia64_time_t St_mtime;
    unsigned long long __reserved1;
    lia64_time_t St_ctime;
    unsigned long long __reserved2;
    unsigned long long St_blksize;
    long long St_blocks;
    unsigned long long __unused[3];
  };

struct lia64_old_stat
  {
    unsigned int St_dev;
    unsigned int St_ino;
    unsigned int St_mode;
    unsigned int St_nlink;
    unsigned int St_uid;
    unsigned int St_gid;
    unsigned int St_rdev;
    unsigned int __pad1;
    unsigned long long St_size;
    lia64_time_t St_atime;
    lia64_time_t St_mtime;
    lia64_time_t St_ctime;
    unsigned int St_blksize;
    int St_blocks;
    unsigned int __unused1;
    unsigned int __unused2;
  };

struct lia64_iovec
  {
    long long iov_base;
    lia64_size_t iov_len;
  };

typedef struct
  {
    unsigned long long sig[1];
  }
lia64_sigset_t;

struct lia64_sigaction
  {
    ADDR lia_handler;			/* can't use sa_handler... */
    unsigned long long sa_flags;
    lia64_sigset_t sa_mask;
  };

static struct shmseg
  {
    struct shmseg *next;		/* must be first! */
    int id;
    REG size;
    ADDR addr;
    BYTE *hostaddr;
  }
*shmseg_list;

typedef struct
  {
    WORD year;
    WORD mon;
    WORD mday;
    WORD hour;
    WORD min;
    WORD sec;
    WORD msec;
    WORD wday;
  }
SscTime;

typedef struct
  {
    WORD ssc_Sec;
    WORD ssc_Usec;
  }
SscTimeval;

typedef struct
  {
    WORD ssc_Minuteswest;
    WORD ssc_Dsttime;
  }
SscTimezone;

typedef struct
  {
    ADDR addr;
    WORD len;
  }
SscDiskReq;

typedef struct
  {
    unsigned fd;
    WORD count;
  }
SscDiskStat;

static struct {
    ADDR buff;
    size_t len, off;
    unsigned scale;
} prof[5];

static struct
  {
    short type;
    short irrBit;
  }
irptTbl[MAXIRPT];

struct lia64_stack {
	DWORD ss_sp;
	WORD ss_flags;
	WORD ss_pad0;
	DWORD ss_size;
};

struct lia64_siginfo {
	WORD si_signo;
	WORD si_errno;
	WORD si_code;
};

struct lia64_sigcontext {
	/* this should match with the Linux/ia64 kernel: */
	DWORD		sc_flags;	/* see manifest constants above */
	DWORD		sc_nat;		/* bit i == 1 iff gr[i] is a NaT */
	struct lia64_stack sc_stack;	/* previously active stack */
	DWORD		sc_ip;		/* instruction pointer */
	DWORD		sc_cfm;		/* current frame marker */
	DWORD		sc_psr;		/* processor status reg. */
	DWORD		sc_ar_rsc;	/* register stack configuration reg */
	DWORD		sc_ar_bsp;	/* backing store pointer */
	DWORD		sc_ar_rnat;	/* RSE NaT collection register */
	DWORD		sc_ar_ccv;	/* compare & xchge compare value */
	DWORD		sc_ar_unat;	/* ar.unat of interrupted context */
	DWORD		sc_ar_fpsr;	/* floating-point status register */
	DWORD		sc_ar_pfs;	/* previous function state */
	DWORD		sc_ar_ec;	/* epilog count register */
	DWORD		sc_ar_lc;	/* loop count register */
	DWORD		sc_pr;		/* predicate registers */
	DWORD		sc_br[8];	/* branch registers */
	DWORD		sc_gr[32];	/* (static) general registers */
	QWORD		sc_fr[128];	/* floating-point registers */
	DWORD		sc_rbs_base;	/* NULL or new base of sighandler's rbs */
	DWORD		sc_loadrs;
	DWORD		sc_ar25;	/* rsvd for scratch use */
	DWORD		sc_ar26;	/* rsvd for scratch use */
	DWORD		sc_rsvd[12];	/* reserved for future use */
	lia64_sigset_t	sc_mask;	/* signal mask to restore */
};

struct lia64_msghdr {
  DWORD	msg_name;
  WORD	msg_namelen;
  WORD	pad0;
  DWORD	msg_iov;
  DWORD	msg_iovlen;
  DWORD	msg_control;
  DWORD	msg_controllen;
  WORD	msg_flags;
};

struct lia64_cmsghdr {
  DWORD	cmsg_len;
  WORD	cmsg_level;
  WORD	cmsg_type;
};

#define LIA64_CMSG_ALIGN(l)	(((l) + 7) & ~7ULL)
#define LIA64_CMSG_SPACE(l)						    \
  (LIA64_CMSG_ALIGN (l) + LIA64_CMSG_ALIGN (sizeof (struct lia64_cmsghdr)))
#define LIA64_CMSG_DATA(c)	((struct lia64_cmsghdr *)		      \
  ((unsigned char *) (c) + LIA64_CMSG_ALIGN (sizeof (struct lia64_cmsghdr))))

unsigned clkTmr = 0, clkTmrSve;		/* for clock interrupts */
unsigned profTmr = 0, profTmrSve;	/* for profile interrupts */
SscReqNode *sscHead = 0;
SscReqNode *sscPend = 0;

ADDR sigHdlr = ~0ULL;
CTR alarm_insts = ~0ULL, alarm_interval;
CTR prof_insts = ~0ULL;
static CTR prof_interval;
static DWORD prof_mask, prof_maxcnt;

DWORD pending_signals;
DWORD masked_signals;
static int num_inservice;

static struct lia64_sigaction sig_act[64];
static ADDR fault_ip[64];
static unsigned irptTop = 0;
static int fdmap[FDMAX];
static int backmap[FDMAX];	/* valid during select only!! */
struct {
    char *name;
    int oflag;
    mode_t mode;
    off_t offset;
} fdInfo[FDMAX];

static BYTE *malargbuf;
static int malargbufsize = -1;
static BYTE *malargbuf2;
static int malargbufsize2 = -1;
static BYTE *malargbuf3;
static int malargbufsize3 = -1;

pid_t cons_pid = -1;
static int cfd = -1;
BOOL trace_syscalls;
char *consLog = NULL;
BOOL noConsole;
extern BOOL userint;
static struct termios sane_ttyIos;

static char *slave;
#ifdef __linux__
# define MASTER_ID	8
#else
  static char master[] = "/dev/ptym/ptyxx";
# define MASTER_ID	13
#endif

extern void mmapSyms (int, ADDR, ADDR, ADDR);
extern void munmapSyms (ADDR, ADDR);
extern void dynBlock (ADDR, ADDR);
extern void memFree (ADDR);
#include <sys/syscall.h>
#define uselib(libname) syscall(__NR_uselib, libname)

extern int setresuid (uid_t, uid_t, uid_t);
extern int getresuid (uid_t *, uid_t *, uid_t *);
extern int setresgid (gid_t, gid_t, gid_t);
extern int getresgid (gid_t *, gid_t *, gid_t *);
extern int getsid (pid_t);

/* FIXME_laurentm: was removed from memory.c */
BOOL
memIsMapped(ADDR adr)
{
    if (!abi && !dtlbMLookup(adr, 0, PSR_CPL, PSR_DT, &adr))
		return NO;
	if (pmemLookup(adr))
		return YES;

	return NO;
}

static int
getdents (unsigned int fd, struct dirent *dirp, unsigned int count)
{
	return syscall (__NR_getdents, fd, dirp, count);
}

#if 0
static int
getdents64 (unsigned int fd, struct dirent64 *dirp, unsigned int count)
{
	return syscall (__NR_getdents64, fd, dirp, count);
}
#endif

extern BOOL intrsim, extint;

#define DEBUG_SIGNALS	0

int signal_pending(void)
{
	return (pending_signals & ~masked_signals) ? 1 : 0;
}

void
signal_queue (int signal)
{
#if DEBUG_SIGNALS
  fprintf(stderr, "signal_queue(sig=%d)\n", signal);
#endif

  pending_signals |= (1ULL << (signal - 1));
  preInst |= PRE_SIGNAL;

  if (signal == SIGINT && interface != BATCH)
    {
      userint = YES;
      setIntrsim (YES);
    }
}

void
signal_queue_info (int signal, ADDR ip)
{
  signal_queue (signal);
  fault_ip[signal] = ip;
}

void
signal_invoke_handler (int in_syscall)
{
  struct lia64_sigcontext sc;
  struct lia64_siginfo si;
  REG sp, nat, gp, entry;
  static struct CT_t ct;
  static INSTINFO info;
  DWORD active;
  int signal;
  int i;

  active = pending_signals & ~masked_signals;
  signal = ffs ((unsigned int) active);
  if (signal == 0)
    {
      signal = ffs ((unsigned ) (active >> 32));
      if (!signal)
	return;

      signal += 32;
    }
  pending_signals &= ~(1ULL << (signal - 1));

#if DEBUG_SIGNALS
  fprintf(stderr, "signal_invoke_handler(sig=%d)\n", signal);
#endif

  if (sig_act[signal].lia_handler == (ADDR) SIG_DFL)
    {
      switch (signal)
	{
	case SIGABRT: case SIGFPE: case SIGSEGV: case SIGTRAP:
	  GrWrtx (8, signal | 0x80, 0);	/* set exit status */
	  progExit ("Caught signal %d @ 0x%llx, dumping core...\n",
		    signal, fault_ip[signal]);
	  break;

	case SIGCHLD: case SIGURG: case SIGWINCH:
	  return;		/* ignore signal */

	default:
	  GrWrtx (8, signal, 0);	/* set exit status */
	  progExit ("Caught signal %d @ 0x%llx exiting\n",
		    signal, fault_ip[signal]);
	}
    }
  else if (sig_act[signal].lia_handler == (ADDR) SIG_IGN)
    {
      switch (signal)
	{
	case SIGKILL: case SIGSTOP:
	  GrWrtx (8, signal | 0x80, 0);	/* set exit status */
	  progExit ("Caught uncatchable signal %d @ 0x%llx; "
		    "shouldn't happen...\n", signal, fault_ip[signal]);
	  break;

	default:
	  return;
	}
    }

  if (sig_act[signal].sa_flags & SA_ONSTACK)
    cmdWarn ("Ignoring SA_ONSTACK for signal %d\n",
	     sig_act[signal].sa_flags, signal);

  memset (&sc, 0, sizeof (sc));

  sc.sc_cfm = (1ULL << 63)	/* valid bit */
    | (((REG) rrbp << 32) | ((REG) rrbf << 25) | ((REG) rrbg << 18)
       | ((REG) (sor >> 3) << 14) | ((REG) soil << 7) | sof);
  sc.sc_ar_bsp = BSP;

  /* simulate the register stack flushing and the cover that are done
     in sigtramp: */
  coverComb(0);
  flushrsComb(0);

  if (fault_ip[signal])
    ip = fault_ip[signal];

  sc.sc_flags = (1 << 2);		/* always set FPH_VALID flag */
  if (in_syscall)
    {
      sc.sc_flags |= (1 << 1);
      ip += 4;				/* skip over break instruction */
      if ((ip & 0xf) == 0xc)
	ip += 0x4;
      GrWrtx ( 8,  4, 0);		/* return r8==EINTR */
      GrWrtx (10, -1, 0);		/* set error indication */
    }
  sc.sc_ip  = (ip & ~(ADDR)0xf) | SLOT (ip);
  sc.sc_psr = psr;
  sc.sc_ar_rsc = RSC;
  sc.sc_ar_rnat = RNAT;
  sc.sc_ar_ccv = CCV;
  sc.sc_ar_unat = UNAT;
  sc.sc_ar_fpsr = FPSR;
  sc.sc_ar_pfs = PFS;
  sc.sc_ar_ec = EC;
  sc.sc_ar_lc = LC;
  for (i = 1; i < 64; ++i)
    sc.sc_pr |= (DWORD) PrRd (i) << i;
  for (i = 0; i < 8; ++i)
    sc.sc_br[i] = BrRd(i);
  for (i = 1; i < 32; ++i) {
    GrRd (i, sc.sc_gr[i], nat, NO);
    sc.sc_nat |= nat << i;
  }
  for (i = 0; i < 128; ++i)
    {
      BYTE sign;
      WORD exp;
      QWORD val;

      freg2spill (FrRd(i), &sign, &exp, &val.right);
      val.left = SEXP (sign, exp);
      memcpy (&sc.sc_fr[i], &val, sizeof (val));
    }
  sc.sc_mask.sig[0] = masked_signals;

  memset (&info, 0, sizeof (info));
  info.extrainfo[1] = 3;	/* destination reg for ar.pfs */
  info.extrainfo[2] = 0;	/* number of inputs & locals */
  info.extrainfo[3] = 3;	/* number of outputs */
  info.extrainfo[4] = 0;	/* number of rotating regs */
  alloc_r1_ar_pfs_i_l_o_rComb(&info);

  si.si_signo = signal;
  si.si_errno = 0;
  si.si_code = 0;

  GrWrtx(32, signal, 0);		/* pass signal number in out0 */

  GrRd(12, sp, nat, NO);
  sp &= ~ (REG) 0xf;
  sp -= (sizeof (sc) + 0xf) & ~0xf;
  pmemLookup_p(page_base(sp));
  pmemLookup_p(page_base(sp + sizeof (sc) - 1));
  memWrt8(sp + 0x000, sc.sc_flags);
  memWrt8(sp + 0x008, sc.sc_nat);
  memWrt8(sp + 0x010, sc.sc_stack.ss_sp);
  memWrt4(sp + 0x018, sc.sc_stack.ss_flags);
  memWrt8(sp + 0x020, sc.sc_stack.ss_size);
  memWrt8(sp + 0x028, sc.sc_ip);
  memWrt8(sp + 0x030, sc.sc_cfm);
  memWrt8(sp + 0x038, sc.sc_psr);
  memWrt8(sp + 0x040, sc.sc_ar_rsc);
  memWrt8(sp + 0x048, sc.sc_ar_bsp);
  memWrt8(sp + 0x050, sc.sc_ar_rnat);
  memWrt8(sp + 0x058, sc.sc_ar_ccv);
  memWrt8(sp + 0x060, sc.sc_ar_unat);
  memWrt8(sp + 0x068, sc.sc_ar_fpsr);
  memWrt8(sp + 0x070, sc.sc_ar_pfs);
  memWrt8(sp + 0x078, sc.sc_ar_ec);
  memWrt8(sp + 0x080, sc.sc_ar_lc);
  memWrt8(sp + 0x088, sc.sc_pr);
  for (i = 0; i < 8; ++i)
    memWrt8(sp + 0x090 + 8*i, sc.sc_br[i]);
  for (i = 0; i < 32; ++i)
    memWrt8(sp + 0x0d0 + 8*i, sc.sc_gr[i]);
  for (i = 0; i < 128; ++i)
    memWrt16(sp + 0x1d0 + 16*i, sc.sc_fr[i].left, sc.sc_fr[i].right);
  memWrt8(sp + 0x9d0, sc.sc_mask.sig[0]);
  GrWrtx(34, sp, 0);		/* pass pointer to sigcontext in out2 */

  sp -= (sizeof (si) + 0xf) & ~0xf;
  pmemLookup_p(page_base(sp - 16));
  memWrt4(sp + 0x00, si.si_signo);
  memWrt4(sp + 0x04, si.si_errno);
  memWrt4(sp + 0x08, si.si_code);
  GrWrtx(33, sp, 0);		/* pass pointer to siginfo in out1 */

  GrWrtx(12, sp - 16, 0);	/* allocate scratch space and set new sp */

  masked_signals |= sig_act[signal].sa_mask.sig[0];
  if (memRd8(sig_act[signal].lia_handler, MEM_LD_ACCESS, &entry) < 0
      || memRd8(sig_act[signal].lia_handler + 8, MEM_LD_ACCESS, &gp) < 0)
    {
      fprintf (stderr,
	       "Failed to read function descriptor for handler of signal %d",
	       signal);
      exit (1);
    }

  GrWrtx(1, gp, 0);			/* set handler's global pointer */
  FPSR = LIA64_FPSR_DEFAULT;

  BrWrt(6, entry);
  ct.ipn = 0xa000000000002000ULL;

  memset (&info, 0, sizeof (info));
  info.ct = &ct;
  info.samepg = 0;
  info.extrainfo[0] = 0; /* qp */
  info.extrainfo[1] = 0;	/* b1 */
  info.extrainfo[2] = 6;	/* b2 */
  br_call_spnt_few_b1_b2Comb(&info);

  icp = NULL;		/* force update of cached instruction pointer */

  ++num_inservice;
}

static inline ADDR
ia64_rse_skip_regs (ADDR addr, int num_regs)
{
	int delta = ((addr >> 3) & 0x3f) + num_regs;

	if (num_regs < 0)
		delta -= 0x3e;
	return addr + 8*(num_regs + delta/0x3f);
}

void
signal_return (void)
{
  struct lia64_sigcontext sc;
  REG sp, bsp, nat;
  INSTINFO info;
  int i;

  GrRd(12, sp, nat, NO);
  sp = sp + 16 + ((sizeof (struct lia64_siginfo) + 0xf) & ~0xf);
  memRd8(sp + 0x000, MEM_LD_ACCESS, &sc.sc_flags);
  memRd8(sp + 0x008, MEM_LD_ACCESS, &sc.sc_nat);
  memRd8(sp + 0x010, MEM_LD_ACCESS, &sc.sc_stack.ss_sp);
  memRd4(sp + 0x018, MEM_LD_ACCESS, &sc.sc_stack.ss_flags);
  memRd8(sp + 0x020, MEM_LD_ACCESS, &sc.sc_stack.ss_size);
  memRd8(sp + 0x028, MEM_LD_ACCESS, &sc.sc_ip);
  memRd8(sp + 0x030, MEM_LD_ACCESS, &sc.sc_cfm);
  memRd8(sp + 0x038, MEM_LD_ACCESS, &sc.sc_psr);
  memRd8(sp + 0x040, MEM_LD_ACCESS, &sc.sc_ar_rsc);
  memRd8(sp + 0x048, MEM_LD_ACCESS, &sc.sc_ar_bsp);
  memRd8(sp + 0x050, MEM_LD_ACCESS, &sc.sc_ar_rnat);
  memRd8(sp + 0x058, MEM_LD_ACCESS, &sc.sc_ar_ccv);
  memRd8(sp + 0x060, MEM_LD_ACCESS, &sc.sc_ar_unat);
  memRd8(sp + 0x068, MEM_LD_ACCESS, &sc.sc_ar_fpsr);
  memRd8(sp + 0x070, MEM_LD_ACCESS, &sc.sc_ar_pfs);
  memRd8(sp + 0x078, MEM_LD_ACCESS, &sc.sc_ar_ec);
  memRd8(sp + 0x080, MEM_LD_ACCESS, &sc.sc_ar_lc);
  memRd8(sp + 0x088, MEM_LD_ACCESS, &sc.sc_pr);
  for (i = 0; i < 8; ++i)
    memRd8(sp + 0x090 + 8*i, MEM_LD_ACCESS, &sc.sc_br[i]);
  for (i = 0; i < 32; ++i)
    memRd8(sp + 0x0d0 + 8*i, MEM_LD_ACCESS, &sc.sc_gr[i]);
  for (i = 0; i < 128; ++i)
    memRd16(sp + 0x1d0 + 16*i, MEM_LD_ACCESS, &sc.sc_fr[i]);
  memRd8(sp + 0x9d0, MEM_LD_ACCESS, &sc.sc_mask.sig[0]);

  RSC = 0;

  /* simulate write to bspstore: */
  bsp = ia64_rse_skip_regs(sc.sc_ar_bsp, sc.sc_cfm & 0x7f);
  GrWrtx (2, bsp, 0);

  memset (&info, 0, sizeof (info));
  info.extrainfo[2] = 2;
  info.extrainfo[3] = BSPST_ID;
  mov_m_ar3_r2Comb (&info);

  RNAT = sc.sc_ar_rnat;
  CCV = sc.sc_ar_ccv;
  UNAT = sc.sc_ar_unat;
  FPSR = sc.sc_ar_fpsr;
  PFS = sc.sc_ar_pfs;
  EC = sc.sc_ar_ec;
  LC = sc.sc_ar_lc;
  for (i = 1; i < 64; ++i)
    PrWrt(i, (sc.sc_pr >> i) & 0x1);
  for (i = 0; i < 8; ++i)
    BrWrt(i, sc.sc_br[i]);
  for (i = 1; i < 32; ++i)
    GrWrtx (i, sc.sc_gr[i], (sc.sc_nat >> i) & 0x1);
  for (i = 0; i < 128; ++i)
    {
      extern FREG qword2freg (QWORD val);

      FrWrt(i, qword2freg(sc.sc_fr[i]));
    }

  /* simulate loadrs 0: */
  loadrsComb (0);

  RSC = sc.sc_ar_rsc;

  /* simulate rfi from sigtramp to interrupted context: */
  PSR_CPL = 0;
  IIP = sc.sc_ip & ~(ADDR)0xf;
  IPSR = sc.sc_psr;
  X_PSR_RI (IPSR) = (sc.sc_ip & 0x3);
  IFS = sc.sc_cfm | (1ULL << 63);
  rfiComb (0);

  masked_signals = sc.sc_mask.sig[0];

  --num_inservice;
  if (pending_signals == 0 && num_inservice == 0)
    preInst &= ~PRE_SIGNAL;

#if DEBUG_SIGNALS
  fprintf(stderr, "signal_return: pending_signals=%llx\n", pending_signals);
#endif
}


static void
rusage_host2lia64 (const struct rusage *h, struct lia64_rusage *l)
{
  l->ru_utime.tv_sec = h->ru_utime.tv_sec;
  l->ru_utime.tv_usec = h->ru_utime.tv_usec;
  l->ru_stime.tv_sec = h->ru_stime.tv_sec;
  l->ru_stime.tv_usec = h->ru_stime.tv_usec;
  l->ru_maxrss = h->ru_maxrss;
  l->ru_ixrss = h->ru_ixrss;
  l->ru_idrss = h->ru_idrss;
  l->ru_isrss = h->ru_isrss;
  l->ru_minflt = h->ru_minflt;
  l->ru_majflt = h->ru_majflt;
  l->ru_nswap = h->ru_nswap;
  l->ru_inblock = h->ru_inblock;
  l->ru_oublock = h->ru_oublock;
  l->ru_msgsnd = h->ru_msgsnd;
  l->ru_msgrcv = h->ru_msgrcv;
  l->ru_nsignals = h->ru_nsignals;
  l->ru_nvcsw = h->ru_nvcsw;
  l->ru_nivcsw = h->ru_nivcsw;
}

static void
stat_host2lia64 (const struct stat *h, struct lia64_stat *l)
{
  l->St_dev = h->st_dev;
  l->St_ino = h->st_ino;
  l->St_mode = h->st_mode;
  l->St_nlink = h->st_nlink;
  l->St_uid = h->st_uid;
  l->St_gid = h->st_gid;
  l->St_rdev = h->st_rdev;
  l->St_size = h->st_size;
  l->St_atime = h->st_atime;
  l->St_mtime = h->st_mtime;
  l->St_ctime = h->st_ctime;
  l->St_blksize = h->st_blksize;
  l->St_blocks = h->st_blocks;
}

static void
old_stat_host2lia64 (const struct stat *h, struct lia64_old_stat *l)
{
  l->St_dev = h->st_dev;
  l->St_ino = h->st_ino;
  l->St_mode = h->st_mode;
  l->St_nlink = h->st_nlink;
  l->St_uid = h->st_uid;
  l->St_gid = h->st_gid;
  l->St_rdev = h->st_rdev;
  l->St_size = h->st_size;
  l->St_atime = h->st_atime;
  l->St_mtime = h->st_mtime;
  l->St_ctime = h->st_ctime;
  l->St_blksize = h->st_blksize;
  l->St_blocks = h->st_blocks;
}

void
setFdmap (int fd1, int fd2)
{
  fdmap[fd1] = fd2;
}

/*
 * Allocate the lowest available file desciptor and make it equivalent
 * with host file descriptor HOST_FD.  */
static int
fdmap_alloc (int host_fd)
{
  int fd;

  if (interface == BATCH)
    {
      /* in batch mode, preserve identity mapping */
      fdmap[host_fd] = host_fd;
      return host_fd;
    }

  /*
   * In non-batch mode, we need to ensure that we allocate the lowest
   * available simulated fd (necessary to preserve the semantics of
   * dup() for example).
   */
  for (fd = 0; fd < FDMAX; ++fd)
    if (fdmap[fd] < 0)
      {
	fdmap[fd] = host_fd;
	return fd;
      }
  errno = EMFILE;
  return -1;
}

/*
 * Remap filedescriptors such that fdmap[] is an identity map.  This
 * may end up closing file descriptors that are used for the debug
 * window, for example.
 */
static void
fdmap_make_identity_map (void)
{
  int i, j;

  /*
   * The invariant here is that for for all k in 0..(i-1),
   * fdmap[k]==k or fdmap[k] == -1.
   *
   * If fdmap[i] != i, the above invariant guarantees that there is no
   * k<i such that fdmap[k]==i.  However, there could be a j>i such
   * that fdmap[j]==i.  If so, we need to remap fdmap[j] first because
   * otherwise dup2(fdmap[i], i) would destroy the filedescriptor for
   * fdmap[j].  We do this with a simple fdmap[k]=dup(i).
   */
  for (i = 0; i < FDMAX; ++i)
    if (fdmap[i] >= 0 && fdmap[i] != i)
      {
	for (j = i + 1; j < FDMAX; ++j)
	  if (fdmap[j] == i)
	    {
	      setFdmap (j, dup (i));
	      break;
	    }
	dup2 (fdmap[i], i);
	setFdmap (i, i);
      }
  for (i = 0; i < FDMAX; ++i)
    if (fdmap[i] < 0)
      close (i);
}

static void
setStatReturn (REG *ret, REG *status)
{
  if ((int)*status == -1)
    *ret = errno;
  else
    {
      *ret = *status;
      *status = 0;
    }
}

void
cmsg_lia642host (struct lia64_msghdr *lia64_msghdr, struct msghdr *msghdr)
{
  size_t nbytes, len = lia64_msghdr->msg_controllen, total = 0;
  struct cmsghdr *cmsghdr = msghdr->msg_control;
  struct lia64_cmsghdr *lia64_cmsghdr;

  lia64_cmsghdr = alloca (len);
  memBBRd (ADDPTR (lia64_msghdr->msg_control), (BYTE *) lia64_cmsghdr, len);

  while (len >= sizeof (struct lia64_cmsghdr))
    {
      nbytes = (((char *) lia64_cmsghdr + lia64_cmsghdr->cmsg_len)
		- (char *) LIA64_CMSG_DATA (lia64_cmsghdr));
      if (lia64_cmsghdr->cmsg_level == SOL_SOCKET
	  && lia64_cmsghdr->cmsg_type == SCM_RIGHTS)
	{
	  /* let's hope this is the only control message needing
             explicit conversion... */
	  int *rdp, *wrp, *end, lia64_fd;

	  rdp = (int *) LIA64_CMSG_DATA (lia64_cmsghdr);
	  end = (int *) ((char *) lia64_cmsghdr + lia64_cmsghdr->cmsg_len);
	  wrp = (int *) CMSG_DATA (cmsghdr);

	  while (rdp < end)
	    {
	      lia64_fd = *rdp++;
	      if (lia64_fd < FDMAX)
		*wrp++ = fdmap[lia64_fd];
	      else
		*wrp++ = -1;
	    }
	}
      else
	memcpy (CMSG_DATA (cmsghdr), LIA64_CMSG_DATA (lia64_cmsghdr), nbytes);
      cmsghdr->cmsg_level = lia64_cmsghdr->cmsg_level;
      cmsghdr->cmsg_type = lia64_cmsghdr->cmsg_type;
      cmsghdr->cmsg_len = CMSG_SPACE (nbytes);

      len -= LIA64_CMSG_ALIGN (lia64_cmsghdr->cmsg_len);
      lia64_cmsghdr = (lia64_cmsghdr +
		       LIA64_CMSG_ALIGN (lia64_cmsghdr->cmsg_len));
      total += CMSG_ALIGN (cmsghdr->cmsg_len);
      cmsghdr = (struct cmsghdr *)
	((char *) cmsghdr + CMSG_ALIGN (cmsghdr->cmsg_len));
    }
  msghdr->msg_controllen = total;
}

void
cmsg_host2lia64 (struct lia64_msghdr *lia64_msghdr, struct msghdr *msghdr)
{
  size_t nbytes, len = msghdr->msg_controllen, total = 0;
  struct cmsghdr *cmsghdr = msghdr->msg_control;
  struct lia64_cmsghdr *lia64_cmsghdr;
  BYTE *buf;

  buf = alloca (lia64_msghdr->msg_controllen);
  lia64_cmsghdr = (struct lia64_cmsghdr *) buf;

  while (len >= sizeof (struct cmsghdr))
    {
      nbytes = (((char *) cmsghdr + cmsghdr->cmsg_len)
		- (char *) CMSG_DATA (cmsghdr));
      if (total + LIA64_CMSG_SPACE (nbytes) >= lia64_msghdr->msg_controllen)
	{
	  lia64_msghdr->msg_flags |= MSG_CTRUNC;
	  break;
	}

      if (cmsghdr->cmsg_level == SOL_SOCKET
	  && cmsghdr->cmsg_type == SCM_RIGHTS)
	{
	  /* let's hope this is the only control message needing
             explicit conversion... */
	  int *rdp, *wrp, *end;

	  rdp = (int *) CMSG_DATA (cmsghdr);
	  end = (int *) ((char *) cmsghdr + cmsghdr->cmsg_len);
	  wrp = (int *) LIA64_CMSG_DATA (lia64_cmsghdr);

	  while (rdp < end)
	    *wrp++ = fdmap_alloc (*rdp++);
	}
      else
	memcpy (LIA64_CMSG_DATA (lia64_cmsghdr), CMSG_DATA (cmsghdr), nbytes);
      lia64_cmsghdr->cmsg_level = cmsghdr->cmsg_level;
      lia64_cmsghdr->cmsg_type = cmsghdr->cmsg_type;
      lia64_cmsghdr->cmsg_len = LIA64_CMSG_SPACE (nbytes);

      total += LIA64_CMSG_ALIGN (lia64_cmsghdr->cmsg_len);
      lia64_cmsghdr = (lia64_cmsghdr +
		       LIA64_CMSG_ALIGN (lia64_cmsghdr->cmsg_len));
      len -= CMSG_ALIGN (cmsghdr->cmsg_len);
      cmsghdr = (struct cmsghdr *)
	((char *) cmsghdr + CMSG_ALIGN (cmsghdr->cmsg_len));
    }
  memBBWrt_alloc (ADDPTR (lia64_msghdr->msg_control), buf, total);
  lia64_msghdr->msg_controllen = total;
}

static fd_set
mapSelectFds (BYTE *buf, int num, int *new_num)
{
  int i, mapped_fd;
  fd_set fds;

  FD_ZERO (&fds);
  for (i = 0; i < num; i++)
    if (FD_ISSET (i, (fd_set *)buf))
      {
	mapped_fd = fdmap[i];
	backmap[mapped_fd] = i;
	FD_SET (mapped_fd, &fds);
	if (mapped_fd >= *new_num)
	  *new_num = mapped_fd + 1;
      }
  return fds;
}

static fd_set
mapBackSelectFds (fd_set *buf, int num)
{
  int i, mapped_fd;
  fd_set fds;

  FD_ZERO (&fds);
  for (i = 0; i < num; i++)
    if (FD_ISSET (i, buf))
      {
	mapped_fd = backmap[i];
	FD_SET (mapped_fd, &fds);
      }
  return fds;
}

static int
getPty (void)
{
  int pfd;

  pfd = getpt();
  if (pfd >= 0)
    {
      /* change slave permissions & unlock slave */
      if (grantpt (pfd) == 0
	 && unlockpt (pfd) == 0
	 && (slave = ptsname (pfd)) != NULL)
       return pfd;
      close (pfd);
    }
  GrWrtx (8, -1 << 8, 0);	/* set exit status */
  progExit ("Unable to open console window\n");
  return -1;
}

static void
restore_tty (void)
{
  tcsetattr (0, TCSANOW, &sane_ttyIos);
}

/*
 * New version of initConsole.
 * It does not use a pipe but the slave/master pty instead
 *
 * The master is actually opened by the simulator and passed via
 * cmdline option to xterm. Then the slave is safely opened.
 *
 * This way we can get the SIGIO going on input from xterm. The prior
 * version did not accept any input from in xterm.
 * Also take a look at the CONNECT_INTERRUPT code for keyboard in this file
 * for more bug fixes.
 *
 * 01/20/1999 S.Eranian
 *
 * 02/22/1999 S.Eranian switched to termios clean up bug (ttyIos not completely
 * 			initialized).
 * 05/24/1999 S.Eranian fixed the extra windowid problem from xterm
 */

static int
initConsole (int raw)
{
  struct termios tty_saved, tmp;
  char cmdBuf[80];
  int sfd, mfd, i, ret;

  /* open master and slave: */
  mfd = getPty ();
  sfd = open (slave, O_RDWR | O_EXCL | O_NOCTTY);
  if (sfd < -1)
    {
      GrWrtx (8, -1 << 8, 0);	/* set exit status */
      progExit ("Unable to open slave pty\n");
    }

  /*
   * When used with the -Sxxx option, xterm prints out the windowid of
   * the pseudo-tty for the 'master' to use.  We don't need that window
   * id, but we need to discard it.  This involves two steps: first,
   * we need to turn of tty ECHO while starting up xterm to ensure
   * the window id doesn't show up on the xterm.  Second, we need
   * to read the window id off the input stream and discard it.
   * This is done in the read below.
   */

  /* get current state */
  tcgetattr (sfd,  &tty_saved);
  tmp = tty_saved;
  tmp.c_lflag &= ~(ECHO | ECHONL);	/* turn off local echo */
  tcsetattr (sfd, TCSADRAIN, &tmp);

  if ((cons_pid = fork ()) == 0)
    {
      close (sfd);			/* child doesn't need slave */

      for (i = 0; i < FDMAX; ++i)
	if (i != mfd)
	  fcntl (i, F_SETFD, 1);	/* set close-on-exec flag */

      /*
       * Go into a separate process group; we don't want to receive
       * signals sent to the simulator
       */
      setpgrp ();

#ifdef __linux__
      /* From what I can tell, xterm all but ignores the two-character
        pty "name", so it doesn't really matter what we're passing
        along here as long as it's just two characters.  */
      snprintf (cmdBuf, sizeof (cmdBuf), "-S%s/%d",
               strrchr (slave, '/') + 1, mfd);
#else
      /* this ought to work for HP-UX */
      sprintf (cmdBuf, "-S %s.%d", master + MASTER_ID, mfd);
#endif
      if (getenv("SKI_USE_FAKE_XTERM"))
        {
	  char tmppat[80];
	  int fd;

	  strcpy(tmppat, "ski.XXXXXX");
	  fd = mkstemp(tmppat);
	  close(fd);
          ret = execlp ("ski-fake-xterm", "ski-fake-xterm", 
	                "-o", tmppat, cmdBuf, NULL);
	}
      else if (consLog)
          ret = execlp ("ski-fake-xterm", "ski-fake-xterm", 
	                "-o", consLog, cmdBuf, NULL);
      else
        ret = execlp ("xterm", "xterm", "-geo", "80x24", "-name", 
                      "\"Linux/ia64 Console\"", cmdBuf, NULL);

      _exit (-1);

    }

  /* parent doesn't need master */
  close (mfd);

  /* read first byte of the window id (and discard it): */
  if (read (sfd, cmdBuf, 1) < 1)
    {
      GrWrtx (8, -1 << 8, 0);	/* set exit status */
      progExit ("Unable to create console window\n");
    }
  /*
   * Now once we've seen the first character(s), a tcflush() should be
   * enough supposing that data had been sent all at once (which is
   * the case).  This might also flush input characters that a user
   * typed early on, but that's probably OK.
   *
   * This solution works for both raw and non-raw mode.
   */
  tcflush (sfd, TCIFLUSH);

  if (raw)
    {
      cfmakeraw (&tmp);
      tmp.c_cc[VMIN] = 0;
      tmp.c_cc[VTIME] = 0;
      tcsetattr (sfd, TCSADRAIN, &tmp);
    }
  else
    /* restore the original (cooked) mode: */
    tcsetattr (sfd, TCSADRAIN, &tty_saved);
  return sfd;
}

static void
readConsole (char *buf)
{
  /*
   * we always use the raw system call to avoid
   * some stupid buffering problem
   * Also make sure that is something went wrong nothing shows up
   * in the buffer
   * 05/07/99 S.Eranian
   */
  if (read (noConsole ? 0 : cfd, buf, 1) != 1) buf[0] = '\0';
}

static void
init (void)
{
  struct sigaction act;

  memset (&act, 0, sizeof (act));
  act.sa_handler = signal_queue;
  sigaction (SIGSEGV, &act, 0);

  memset (fdmap, 0xff, sizeof (fdmap));

  if (!noConsole)
    {
      cfd = initConsole (0);
      if (cfd < 0)
	noConsole = YES;
    }

  if (interface == BATCH)
    {
      /* in batch mode we want fdmap[] to be an identity map */
      if (!noConsole)
	{
	  /* redirect stdio to console: */
	  dup2 (cfd, 0);
	  dup2 (cfd, 1);
	  dup2 (cfd, 2);
	  close (cfd);
	  cfd = 0;
	}
      setFdmap (0, 0);
      setFdmap (1, 1);
      setFdmap (2, 2);
    }
  else
    {
      if (noConsole)
	{
	  setFdmap (0, dup (0));
	  setFdmap (1, dup (1));
	  setFdmap (2, dup (2));
	}
      else
	{
	  setFdmap (0, dup (cfd));
	  setFdmap (1, dup (cfd));
	  setFdmap (2, dup (cfd));
	}
    }
}

void
profCnt (void)
{
  ADDR baddr, offset, cntaddr;
  unsigned bucket;
  HWORD cnt2;

  prof_insts += prof_interval;
  baddr = ip & ~0xFULL;
  if (prof[0].scale < 2 || baddr < prof[0].off)
    return;
  offset = baddr - prof[0].off;
  bucket = (((offset >> 4) * prof[0].scale) >> 12) & prof_mask;
  if (bucket >= prof[0].len)
    return;
  cntaddr = prof[0].buff + bucket;
  if (!memMRd(cntaddr, 2, (DWORD*) &cnt2))
    {
      prof[0].scale = 0;
      return;
    }
  if (cnt2 < prof_maxcnt)
    {
      cnt2++;
      if (!memMWrt(cntaddr, 2, cnt2))
	prof[0].scale = 0;
    }
}

static ADDR
get_unmapped_area (DWORD size)
{
  static ADDR base = 0x2000000000000000ULL;
  ADDR addr, start, end;

  while (1)
    {
      end = page_align(base + size);
      for (addr = base; 1; addr += page_size)
	{
	  if (addr > end)
	    {
	      start = base;
	      base = end;
	      return start;
	    }
	  if (memIsMapped (addr))
	    {
	      /* the page at ADDR exists already */
	      base = page_base (addr) + page_size;
	      break;
	    }
	}
    }
}

static struct shmseg *
shmseg_new (int id, long size)
{
  struct shmseg *shmseg;

  shmseg = malloc (sizeof (*shmseg));
  if (!shmseg)
    return NULL;

  memset (shmseg, 0, sizeof (*shmseg));
  shmseg->id = id;
  shmseg->size = size;
  shmseg->next = shmseg_list;
  shmseg_list = shmseg;
  return shmseg;
}

static struct shmseg *
shmseg_lookup_id (int id)
{
  struct shmseg *shmseg;

  for (shmseg = shmseg_list; shmseg; shmseg = shmseg->next)
    if (shmseg->id == id)
      return shmseg;
  return 0;
}

static struct shmseg *
shmseg_lookup_addr (ADDR addr)
{
  struct shmseg *shmseg;

  for (shmseg = shmseg_list; shmseg; shmseg = shmseg->next)
    if (shmseg->addr == addr)
      return shmseg;
  return 0;
}

static void
shmseg_free (struct shmseg *shmseg)
{
  struct shmseg *prev;

  prev = (struct shmseg *) &shmseg_list;
  while (prev->next != shmseg)
    prev = prev->next;

  prev->next = shmseg->next;
  free (shmseg);
}

#define ENSURE_SIZE(newsize)						\
  if ((int) (newsize) > malargbufsize)					\
    {									\
      if (!(malargbuf = realloc (malargbuf, (newsize) + 1024)))		\
	{								\
	  *status = -1;							\
	  *ret = ENOMEM;						\
	  setStatReturn (ret, status);					\
	  break;							\
	}								\
      malargbufsize = (newsize) + 1024;					\
    }

#define ENSURE_SIZE2(newsize)						\
  if ((int) (newsize) > malargbufsize2)					\
    {									\
      if (!(malargbuf2 = realloc (malargbuf2, (newsize) + 1024)))	\
	{								\
	  *status = -1;							\
	  *ret = ENOMEM;						\
	  setStatReturn (ret, status);					\
	  break;							\
	}								\
      malargbufsize2 = (newsize) + 1024;				\
    }

#define ENSURE_SIZE3(newsize)						\
  if ((int) (newsize) > malargbufsize3)					\
    {									\
      if (!(malargbuf3 = realloc (malargbuf3, (newsize) + 1024)))	\
	{								\
	  *status = -1;							\
	  *ret = ENOMEM;						\
	  setStatReturn (ret, status);					\
	  break;							\
	}								\
      malargbufsize3 = (newsize) + 1024;				\
    }

#define CHECK_FD(fd)							\
  if ((unsigned) (fd) >= FDMAX)						\
    {									\
      *status = -1;							\
      errno = EBADF;							\
      setStatReturn (ret, status);					\
      break;								\
    }

#define CHECK_SIGNUM(sig)						\
  if ((unsigned) (sig) >= 64)						\
    {									\
      *status = -1;							\
      errno = EINVAL;							\
      setStatReturn (ret, status);					\
      break;								\
    }

void
doSyscall (HWORD num, REG arg0, REG arg1, REG arg2, REG arg3, REG arg4,
	   REG arg5, REG arg6, REG arg7, REG *ret, REG *status)
{
  BYTE msg[500], buf[32768], buf1[32768], buf2[32768], *bp, elfhdr[20];
  REG tmp, new_base, adr;
  static int need_init = 1;
  ssize_t nbytes;
  int max, i, fd, nfds, resource, pfd[2], bytes_in, bytes_out;
  struct tms host_tms;
  struct lia64_tms lia64_tms;
  struct flock host_flock;
  struct lia64_flock lia64_flock;
  struct rlimit host_rlimit;
  struct rlimit host_rlimit_old;
  struct lia64_rlimit lia64_rlimit;
  struct lia64_rlimit lia64_rlimit_old;
  struct rusage host_rusage;
  struct lia64_rusage lia64_rusage;
  struct lia64_timeval lia64_timeval;
  struct timeval host_timeval, *tmout;
  struct lia64_timespec lia64_timespec;
  struct timespec host_timespec;
  struct timezone tzbuf;
  lia64_gid_t lia64_gid;
  fd_set readfds, writefds, errorfds;
  fd_set *rfds, *wfds, *efds;
  struct lia64_statfs lia64_statfs;
  struct statfs host_statfs;
  struct lia64_itimerval lia64_itval;
  struct itimerval host_itval;
  struct lia64_stat lia64_stat;
  struct lia64_old_stat lia64_old_stat;
  struct stat host_stat;
  struct dirent *host_dirent;
  struct lia64_iovec lia64_iovec;
  struct sigaction act;
  struct lia64_sigaction lia64_sigact, *sa;
  struct lia64_timeval lia64_utimes[2];
  struct timeval host_utimes[2];
  sigset_t sigset, saved_sigset;
  int done, waitstatus;
  socklen_t addrlen;
  struct sockaddr *sockaddr;
  struct shmseg *shmseg;
  DWORD lia64_sigset;
  BOOL must_move;
  BYTE *hostadr;
  ADDR end;
  REG curr;
  char *cp;
  int sig;

  if (need_init)
    {
      need_init = 0;
      init ();
    }

  if (trace_syscalls)
    traceSyscallEnter (num, arg0, arg1, arg2, arg3, arg4, arg5);

  *status = -1;
  *ret = ENOSYS;
  switch (num)
    {
    case SYS_EXIT:
    case LIA64_exit:
    case LIA64_exit_group:
      GrWrtx (8, arg0, 0);
      progExit("program exited with status %d\n", (int)arg0);
      break;

    case SYS_READ:
    case LIA64_read:
      /* make sure we have enough space in malargbuf */
      ENSURE_SIZE (arg2);
      CHECK_FD (arg0);
      nbytes = read (fdmap[arg0], malargbuf, arg2);
      *status = nbytes;
      if ((ssize_t)nbytes > 0)
	memBBWrt_alloc (ADDPTR (arg1), malargbuf, nbytes);
      setStatReturn (ret, status);
      break;

    case SYS_WRITE:
    case LIA64_write:
      /* make sure we have enough space in malargbuf */
      ENSURE_SIZE (arg2);
      CHECK_FD (arg0);
      if (arg2 > 0)
	memBBRd(ADDPTR(arg1), malargbuf, arg2);
      *status = sysWrite (fdmap[arg0], malargbuf, arg2);
      setStatReturn (ret, status);
      break;

    case SYS_OPEN:
    case LIA64_open:
      simroot(ADDPTR(arg0), buf, arg1 & O_CREAT);
      *status = open ((char *)buf, arg1, arg2);
      if ((int)*status != -1) {
#ifdef HOST_MAP
	int simfd;

	for (simfd = 0; simfd < FDMAX; simfd++)
	  if (fdmap[simfd] == -1)
	    break;
	fdInfo[simfd].name = strdup (buf);
	fdInfo[simfd].oflag = arg1;
	fdInfo[simfd].mode = arg2;
	fdmap[simfd] = *status;
	*status = simfd;
#else
	*status = fdmap_alloc (*status);
#endif
      }
      setStatReturn (ret, status);
      break;

    case LIA64_openat:
      simroot(ADDPTR(arg1), buf, arg2 & O_CREAT);
      *status = openat (arg0, (char *)buf, arg2, arg3);
      if ((int)*status != -1) {
#ifdef HOST_MAP
	int simfd;

	for (simfd = 0; simfd < FDMAX; simfd++)
	  if (fdmap[simfd] == -1)
	    break;
	fdInfo[simfd].name = strdup (buf);
	fdInfo[simfd].oflag = arg2;
	fdInfo[simfd].mode = arg3;
	fdmap[simfd] = *status;
	*status = simfd;
#else
	*status = fdmap_alloc (*status);
#endif
      }
      setStatReturn (ret, status);
      break;

    case SYS_CLOSE:
    case LIA64_close:
      CHECK_FD (arg0);
      if (fdmap[arg0] >= 0)
	*status = close (fdmap[arg0]);
      setFdmap (arg0, -1);
      setStatReturn (ret, status);
      break;

    case SYS_LINK:
    case LIA64_link:
      memBBRd (ADDPTR (arg0), buf, 0);
      memBBRd (ADDPTR (arg1), buf2, 0);
      *status = link ((char *)buf, (char *)buf2);
      setStatReturn (ret, status);
      break;

    case SYS_UNLINK:
    case LIA64_unlink:
      simroot(ADDPTR(arg0), buf, 0);
      *status = unlink ((char *)buf);
      setStatReturn (ret, status);
      break;

    case LIA64_execve:
      {
	char **wrp = (char **) (((long) buf + 7) & ~7);
	size_t len, off, total = 32768;
	char *mem = malloc (total), **argv, **envp;
	REG argp;

	argv = wrp;

	memBBRd (arg0, (BYTE *) mem, 0);
	len = strlen (mem) + 1;
	*wrp++ = "bskinc";
	*wrp++ = mem;
	off = len;

	arg1 += 8;	/* skip over argv[0] (we use arg1 instead) */

	/* convert argv from lia64 to host format: */
	while (1)
	  {
	    memBBRd (ADDPTR (arg1), (BYTE *) &argp, 8);
	    arg1 += 8;
	    if (!argp)
	      break;

	    memBBRd (ADDPTR (argp), buf2, 0);
	    len = strlen ((char*) buf2) + 1;
	    if (off + len >= total)
	      {
		total += 32768;
		mem = realloc (mem, total);
		if (!mem)
		  {
		    fprintf (stderr, "Simulator out of memory!\n");
		    exit (-1);
		  }
	      }
	    memcpy (mem + off, buf2, len);
	    *wrp++ = mem + off;
	    off += len;
	  }
	*wrp++ = 0;

	/* ditto for envp: */
	envp = wrp;
	while (1)
	  {
	    memBBRd (arg2, (BYTE *) &argp, 8);
	    arg2 += 8;
	    if (!argp)
	      break;

	    memBBRd (argp, buf2, 0);
	    len = strlen ((char *)buf2) + 1;
	    if (off + len >= total)
	      {
		total += 32768;
		mem = realloc (mem, total);
		if (!mem)
		  {
		    fprintf (stderr, "Simulator out of memory!\n");
		    exit (-1);
		  }
	      }
	    memcpy (mem + off, buf2, len);
	    *wrp++ = mem + off;
	    off += len;
	  }
	*wrp++ = 0;

	if (stat (argv[1], &host_stat) < 0)
	  {
	    /*
	     * If file doesn't exist at all, return ENOENT.  Ugly, but
	     * tcsh uses this to locate the executable.
	     */
	    *status = -1;
	    errno = ENOENT;
	  }
	else
	  {
	    int r;
	    fdmap_make_identity_map ();
	    elfhdr[18] = 0;
	    elfhdr[19] = 0;
	    fd = open (argv[1], O_RDONLY);
	    r = read (fd, elfhdr, sizeof (elfhdr));
	    (void)r; /* TODO */
	    close (fd);
	    if (elfhdr[18] == 50 && elfhdr[19] == 0)
	      /* looks like an IA-64 binary: */
	      *status = execve ("/usr/bin/bskinc", argv, envp);
	    else
	      /* try it as a host binary: */
	      *status = execve (argv[1], argv + 1, envp);

	  }
	setStatReturn (ret, status);
	if (mem)
	  free (mem);
      }
      break;

    case SYS_CHDIR:
    case LIA64_chdir:
      memBBRd (ADDPTR (arg0), buf, 0);
      *status = chdir ((char *)buf);
      setStatReturn (ret, status);
      break;

    case SYS_FCHDIR:
    case LIA64_fchdir:
      CHECK_FD (arg0);
      *status = fchdir (fdmap[arg0]);
      setStatReturn (ret, status);
      break;

    case LIA64_utimes:
      memBBRd (ADDPTR (arg0), buf, 0);
      memBBRd (ADDPTR (arg1), (BYTE *) &lia64_utimes[0],
	       sizeof (lia64_utimes));
      host_utimes[0].tv_sec  = lia64_utimes[0].tv_sec;
      host_utimes[0].tv_usec = lia64_utimes[0].tv_usec;
      host_utimes[1].tv_sec  = lia64_utimes[1].tv_sec;
      host_utimes[1].tv_usec = lia64_utimes[1].tv_usec;
      *status = utimes ((char *) buf, &host_utimes[0]);
      setStatReturn (ret, status);
      break;

    case SYS_MKNOD:
    case LIA64_mknod:
      memBBRd (ADDPTR (arg0), buf, 0);
      *status = mknod ((char *)buf, arg1, arg2);
      setStatReturn (ret, status);
      break;

    case SYS_CHMOD:
    case LIA64_chmod:
      memBBRd (ADDPTR (arg0), buf, 0);
      *status = chmod ((char *)buf, arg1);
      setStatReturn (ret, status);
      break;

    case SYS_CHOWN:
    case LIA64_chown:
      memBBRd (ADDPTR (arg0), buf, 0);
      *status = chown ((char *)buf, arg1, arg2);
      setStatReturn (ret, status);
      break;

    case SYS_lseek:
    case LIA64_lseek:
      CHECK_FD (arg0);
      *status = lseek (fdmap[arg0], arg1, arg2);
      setStatReturn (ret, status);
      break;

    case SYS_GETPID:
    case LIA64_getpid:
      *status = getpid ();
      setStatReturn (ret, status);
      break;

    case LIA64_getppid:
      *status = getppid ();
      setStatReturn (ret, status);
      break;

    case LIA64_gettid:
      *status = syscall (SYS_gettid);
      setStatReturn (ret, status);
      break;

    case LIA64_mount:
      memBBRd (ADDPTR (arg0), buf, 0);
      memBBRd (ADDPTR (arg1), buf1, 0);
      memBBRd (ADDPTR (arg2), buf2, 0);
      *status = mount ((char *)buf, (char *)buf1, (char *)buf2, arg3, 0);
      setStatReturn (ret, status);
      break;

    case SYS_UMOUNT:
    case LIA64_umount:
      memBBRd (ADDPTR (arg0), buf, 0);
      *status = umount ((char *)buf);
      setStatReturn (ret, status);
      break;

    case LIA64_setuid:
      *status = setuid (arg0);
      setStatReturn (ret, status);
      break;

    case LIA64_getuid:
      *status = getuid ();
      setStatReturn (ret, status);
      break;

    case LIA64_geteuid:
      *status = geteuid ();
      setStatReturn (ret, status);
      break;

    case LIA64_ptrace:
      sprintf ((char *)msg, "Simulator doesn't ptrace()!!\n");
      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      errno = EOPNOTSUPP;
      setStatReturn (ret, status);
      break;

    case SYS_ACCESS:
    case LIA64_access:
      simroot(ADDPTR(arg0), buf, 0);
      *status = access (buf, arg1);
      setStatReturn (ret, status);
      break;

    case LIA64_faccessat:
      simroot(ADDPTR(arg1), buf, 0);
      *status = faccessat (fdmap[arg0], buf, arg2, arg3);
      setStatReturn (ret, status);
      break;

    case LIA64_sync:
      sync ();
      break;

    case LIA64_fsync:
      CHECK_FD (arg0);
      *status = fsync (fdmap[arg0]);
      setStatReturn (ret, status);
      break;

    case LIA64_fdatasync:
      CHECK_FD (arg0);
      *status = fdatasync (fdmap[arg0]);
      setStatReturn (ret, status);
      break;

    case SYS_KILL:
    case LIA64_kill:
      *status = kill (arg0, arg1);
      setStatReturn (ret, status);
      break;

    case LIA64_tgkill:
      *status = syscall (SYS_tgkill, arg0, arg1, arg2);
      setStatReturn (ret, status);
      break;

    case SYS_RENAME:
    case LIA64_rename:
      memBBRd (ADDPTR (arg0), buf, 0);
      memBBRd (ADDPTR (arg1), buf2, 0);
      *status = rename ((char *)buf, (char *)buf2);
      setStatReturn (ret, status);
      break;

    case SYS_MKDIR:
    case LIA64_mkdir:
      memBBRd (ADDPTR (arg0), buf, 0);
      *status = mkdir ((char *)buf, arg1);
      setStatReturn (ret, status);
      break;

    case SYS_RMDIR:
    case LIA64_rmdir:
      memBBRd (ADDPTR (arg0), buf, 0);
      *status = rmdir ((char *)buf);
      setStatReturn (ret, status);
      break;

    case SYS_DUP:
    case LIA64_dup:
      CHECK_FD (arg0);
      *status = dup (fdmap[arg0]);
      if ((int) *status >= 0)
	*status = fdmap_alloc (*status);
      setStatReturn (ret, status);
      break;

    // A bit unusual syscall API:
    // struct fd_pair { long fd[2]; };
    // struct fd_pair pipe(void);
    case LIA64_pipe:
      *status = pipe (pfd);
      if ((int)*status != -1)
	{
	  *status = fdmap_alloc (pfd[0]);
	  GrWrtx (9, fdmap_alloc (pfd[1]), 0);
	}
      setStatReturn (ret, status);
      break;

    // int pipe2(int pipefd[2], int flags);
    case LIA64_pipe2:
      *status = pipe2 (pfd, arg1);
      if ((int)*status != -1)
	{
	  int ia64_pfd[2] = {
	    fdmap_alloc (pfd[0]),
	    fdmap_alloc (pfd[1]),
	  };
	  memBBWrt_alloc (ADDPTR (arg0), (BYTE *) ia64_pfd,
			  sizeof (ia64_pfd));
	}
      setStatReturn (ret, status);
      break;

    case SYS_TIMES:
    case LIA64_times:
#ifdef CONSTANT_TIME
      memset (&lia64_tms, 0, sizeof (lia64_tms));
      *status = 0;
#else
      *status = times (&host_tms);
      if ((int)*status != -1)
	{
	  lia64_tms.tms_utime = host_tms.tms_utime;
	  lia64_tms.tms_stime = host_tms.tms_stime;
	  lia64_tms.tms_cutime = host_tms.tms_cutime;
	  lia64_tms.tms_cstime = host_tms.tms_cstime;
	  memBBWrt_alloc (ADDPTR (arg0), (BYTE *) &lia64_tms,
			  sizeof (lia64_tms));
	}
#endif
      setStatReturn (ret, status);
      break;

    case SYS_BRK:
    case LIA64_brk:
      arg0 = ADDPTR (arg0);
      if (arg0 == 0)
	arg0 = heapGet(curPid);	/* return current heap addr */
      else if (arg0 > heapGet(curPid))
	{
	  for (adr = page_base(heapGet(curPid)); adr < arg0; adr += page_size)
	    pmemLookup_p (adr);
	  heapSet (curPid, arg0);
	}
      *ret = arg0;
      *status = 0;
      break;

    case SYS_SETGID:
    case LIA64_setgid:
      *status = setgid (arg0);
      setStatReturn (ret, status);
      break;

    case SYS_GETGID:
    case LIA64_getgid:
      *status = getgid ();
      setStatReturn (ret, status);
      break;

    case LIA64_getegid:
      *status = getegid ();
      setStatReturn (ret, status);
      break;

    case SYS_ACCT:
    case LIA64_acct:
      memBBRd (ADDPTR (arg0), buf, 0);
      *status = acct ((char *) buf);
      setStatReturn (ret, status);
      break;

    case LIA64_ioctl:
      CHECK_FD (arg0);

      bytes_in = 0;
      bytes_out = 0;
      done = 0;
      switch (arg1)
	{
	case TCSBRK:
	case TCSBRKP:
	case TCXONC:
	case TCFLSH:
	case TIOCEXCL:
	case TIOCNXCL:
	case TIOCSCTTY:
	case TIOCCONS:
	case TIOCNOTTY:
	case TIOCSBRK:
	case TIOCCBRK:
	case FIONCLEX:
	case FIOCLEX:
	  break;

	case TCGETS:
	case TIOCGLCKTRMIOS:
	  /* assumes host OS matches Linux/ia64 */
	  bytes_out = sizeof (struct termios);
	  break;

	case TCSETS:
	case TCSETSW:
	case TCSETSF:
	case TIOCSLCKTRMIOS:
	  /* assumes host OS matches Linux/ia64 */
	  bytes_in = sizeof (struct termios);
	  break;

	case TCGETA:
	  /* assumes host OS matches Linux/ia64 */
	  bytes_out = sizeof (struct termio);
	  break;

	case TCSETA:
	case TCSETAW:
	case TCSETAF:
	  /* assumes host OS matches Linux/ia64 */
	  bytes_in = sizeof (struct termio);
	  break;

	case TIOCGPGRP:
	case TIOCOUTQ:
	case TIOCMGET:
	case TIOCGSOFTCAR:
	case TIOCINQ:
	case TIOCGETD:
	case TIOCGSID:
	case FIOASYNC:
	  bytes_out = 4;
	  break;

	case TIOCSPGRP:
	case TIOCMBIS:
	case TIOCMBIC:
	case TIOCMSET:
	case TIOCSSOFTCAR:
	case TIOCPKT:
	case FIONBIO:
	case TIOCSETD:
	  bytes_in = 4;
	  break;

	case TIOCSTI:
	  bytes_in = 1;
	  break;

	case TIOCGWINSZ:
	  /* assumes host OS matches Linux/ia64 */
	  bytes_out = sizeof (struct winsize);
	  break;

	case TIOCSWINSZ:
	  /* assumes host OS matches Linux/ia64 */
	  bytes_in =  sizeof (struct winsize);
	  break;

	case TIOCLINUX:
	  strcpy ((char *)msg, "syscall-linux.c: TIOCLINUX unimplemented\n");
	  sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
	  break;

	case TIOCGSERIAL:
	  bytes_out = sizeof (struct serial_struct);
	  break;

	case TIOCSSERIAL:
	  bytes_in = sizeof (struct serial_struct);
	  break;

	case FIOSETOWN:
	case SIOCSPGRP:
	case SIOCSIFENCAP:
	  bytes_in = 4;
	  break;

	case FIOGETOWN:
	case SIOCGPGRP:
	case SIOCATMARK:
	case SIOCGIFENCAP:	/* get encapsulations */
	  bytes_out = 4;
	  break;

	case SIOCGSTAMP:	/* get stamp */
	  *status = ioctl (fdmap[arg0], SIOCGSTAMP, &host_timeval);
	  lia64_timeval.tv_sec = host_timeval.tv_sec;
	  lia64_timeval.tv_usec = host_timeval.tv_usec;
	  if ((int)*status != -1)
	    memBBWrt_alloc (ADDPTR (arg2), (BYTE*) &lia64_timeval, bytes_out);
	  done = 1;
	  break;

	case SIOCGARP:		/* get ARP table entry		*/
	case SIOCGRARP:		/* get RARP table entry		*/
	  bytes_in = bytes_out = sizeof (struct arpreq);
	  break;

	case SIOCDARP:		/* delete ARP table entry	*/
	case SIOCDRARP:		/* delete RARP table entry	*/
	case SIOCSARP:		/* set ARP table entry		*/
	case SIOCSRARP:		/* set RARP table entry		*/
	  bytes_in = sizeof (struct arpreq);
	  break;

	case SIOCGIFNAME:	/* get iface name		*/
	case SIOCGIFFLAGS:	/* get flags			*/
	case SIOCGIFMETRIC:	/* get metric			*/
	case SIOCGIFMTU:	/* get MTU size			*/
	case SIOCGIFHWADDR:	/* Get hardware address		*/
	case SIOCGIFSLAVE:	/* Driver slaving support	*/
	case SIOCGIFMAP:	/* Get device parameters	*/
	case SIOCGIFINDEX:	/* name -> if_index mapping	*/
	case SIOCGIFTXQLEN:	/* Get the tx queue length	*/
	case SIOCGIFADDR:	/* get PA address		*/
	case SIOCGIFBRDADDR:	/* get broadcast PA address	*/
	case SIOCGIFDSTADDR:	/* get remote PA address	*/
	case SIOCGIFNETMASK:	/* get network PA mask		*/
	case SIOCGIFPFLAGS:
	case SIOCGIFCOUNT:	/* get number of devices */
	  bytes_in = bytes_out = sizeof (struct ifreq);
	  break;

	case SIOCSIFFLAGS:	/* set flags			*/
	case SIOCSIFMETRIC:	/* set metric			*/
	case SIOCSIFMTU:	/* set MTU size			*/
	case SIOCSIFMAP:	/* set device parameters	*/
	case SIOCSIFHWADDR:	/* set hardware address 	*/
	case SIOCSIFSLAVE:
	case SIOCADDMULTI:	/* Multicast address lists	*/
	case SIOCDELMULTI:
	case SIOCSIFHWBROADCAST:/* set hardware broadcast addr	*/
	case SIOCSIFTXQLEN:	/* set the tx queue length 	*/
	case SIOCSIFNAME:	/* set interface name		*/
	case SIOCSIFADDR:	/* set PA address		*/
	case SIOCSIFBRDADDR:	/* set broadcast PA address	*/
	case SIOCSIFDSTADDR:	/* set remote PA address	*/
	case SIOCSIFNETMASK:	/* set network PA mask		*/
	case SIOCSIFPFLAGS:	/* set/get extended flags set	*/
	  bytes_in = sizeof (struct ifreq);
	  break;

	case SIOCADDRT:	/* add routing table entry	*/
	case SIOCDELRT:	/* delete routing table entry	*/
#if 0
	  bytes_in = sizeof (struct lia64_rtentry);
	  break;
#endif

	case TIOCSERCONFIG:
	case TIOCSERGWILD:
	case TIOCSERSWILD:
	case TIOCSERGSTRUCT:	/* For debugging only */
	case TIOCSERGETLSR:	/* Get line status register */
	case TIOCSERGETMULTI:	/* Get multiport config  */
	case TIOCSERSETMULTI:	/* Set multiport config */
	case TIOCMIWAIT:	/* wait for a change on serial input line(s) */
	case TIOCGICOUNT:	/* read serial port inline interrupt counts */
	case SIOCRTMSG:	/* call to routing system	*/
	case SIOCSIFLINK:	/* set iface channel		*/
	case SIOCGIFMEM:	/* get memory address (BSD)	*/
	case SIOCSIFMEM:	/* set memory address (BSD)	*/
	case SIOCDIFADDR:	/* delete PA address		*/
	case SIOCADDDLCI:	/* Create new DLCI device	*/
	case SIOCDELDLCI:	/* Delete DLCI device		*/
	case SIOCGIFBR:		/* Bridging support		*/
	case SIOCSIFBR:		/* Set bridging options 	*/
	case SIOCGIFCONF:	/* get iface list		*/
	  sprintf ((char *)msg, "syscall-linux.c: ioctl() 0x%llx unimplemented\n",
		   arg1);
	  sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
	  break;

	default:
	  if (arg1 < 0x10000)
	    {
	      sprintf ((char *)msg, "syscall-linux.c: suspect ioctl() 0x%llx\n",
                   arg1);
	      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
	    }

	  /* This is broken for ioctl() calls where the Linux/ia64
	     arguments differ from that of the host OS.  We'll worry
	     about it when we get to this point... */
	  if (arg1 & LIA64_IOC_IN)
	    bytes_in = LIA64_IOC_SIZE (arg1);
	  if (arg1 & LIA64_IOC_OUT)
	    bytes_out = LIA64_IOC_SIZE (arg1);
	}

      if (!done)
	{
	  if (bytes_in > 0 || bytes_out > 0)
	    {
	      if (bytes_in > 0)
		memBBRd (ADDPTR (arg2), buf, bytes_in);
	      *status = ioctl (fdmap[arg0], arg1, buf);
	      if (((int)*status != -1) && bytes_out > 0)
		memBBWrt_alloc (ADDPTR (arg2), buf, bytes_out);
	    }
	  else
	    *status = ioctl (fdmap[arg0], arg1, arg2);
	}
      setStatReturn (ret, status);
      break;

    case SYS_FCNTL:
    case LIA64_fcntl:
      CHECK_FD (arg0);
      if (arg1 == F_GETLK || arg1 == F_SETLK || arg1 == F_SETLKW)
	{
	  if (!arg2)
	    errno = memIsMapped (0) ? EINVAL : EFAULT;
	  else
	    {
	      memBBRd (arg2, (BYTE *) &lia64_flock,
		       sizeof (struct lia64_flock));
	      host_flock.l_type = lia64_flock.l_type;
	      host_flock.l_whence = lia64_flock.l_whence;
	      host_flock.l_start = lia64_flock.l_start;
	      host_flock.l_len = lia64_flock.l_len;
	      host_flock.l_pid = lia64_flock.l_pid;
	      *status = fcntl (fdmap[arg0], arg1, &host_flock);
	      lia64_flock.l_type = host_flock.l_type;
	      lia64_flock.l_whence = host_flock.l_whence;
	      lia64_flock.l_start = host_flock.l_start;
	      lia64_flock.l_len = host_flock.l_len;
	      lia64_flock.l_pid = host_flock.l_pid;
	      memBBWrt_alloc (arg2, (BYTE *) &lia64_flock,
			sizeof (struct lia64_flock));
	    }
	}
      else
	*status = fcntl (fdmap[arg0], arg1, arg2);
      setStatReturn (ret, status);
      break;

    case SYS_UMASK:
    case LIA64_umask:
      *status = umask (arg0);
      setStatReturn (ret, status);
      break;

    case SYS_CHROOT:
    case LIA64_chroot:
      memBBRd (ADDPTR (arg0), buf, 0);
      *status = chroot ((char *) buf);
      setStatReturn (ret, status);
      break;

#if defined(__hpux)
    case LIA64_ustat:
      *status = ustat (arg0, (struct ustat *) buf);
      memBBWrt_alloc (ADDPTR (arg1), buf, sizeof (struct ustat));
      setStatReturn (ret, status);
      break;
#endif

    case SYS_DUP2:
    case LIA64_dup2:
      CHECK_FD (arg0);
      CHECK_FD (arg1);

      if (interface == BATCH)
	{
	  /* in batch mode, fdmap[] is an identity map... */
	  *status = dup2 (arg0, arg1);
	  if ((int) *status != -1)
	    {
	      setFdmap (arg0, arg0);
	      setFdmap (arg1, arg1);
	    }
	}
      else
	{
	  if (fdmap[arg1] >= 0)
	    close (fdmap[arg1]);
	  *status = dup (fdmap[arg0]);
	  setFdmap (arg1, *status);
	}
      setStatReturn (ret, status);
      break;

    case LIA64_setreuid:
      *status = setreuid (arg0, arg1);
      setStatReturn (ret, status);
      break;

    case LIA64_setregid:
      *status = setregid (arg0, arg1);
      setStatReturn (ret, status);
      break;

    case LIA64_getresuid:
      {
	lia64_uid_t lia64_uid, lia64_euid, lia64_suid;
	uid_t host_uid, host_euid, host_suid;

	*status = getresuid (&host_uid, &host_euid, &host_suid);
	lia64_uid = host_uid;
	lia64_euid = host_euid;
	lia64_suid = host_suid;
	if ((int) *status != -1)
	  {
	    memBBWrt_alloc (ADDPTR (arg0), (BYTE *) &lia64_uid,
			    sizeof (lia64_uid));
	    memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &lia64_euid,
			    sizeof (lia64_uid));
	    memBBWrt_alloc (ADDPTR (arg2), (BYTE *) &lia64_suid,
			    sizeof (lia64_uid));
	  }
      }
      setStatReturn (ret, status);
      break;

    case LIA64_setresuid:
      *status = setresuid (arg0, arg1, arg2);
      setStatReturn (ret, status);
      break;

    case LIA64_getresgid:
      {
	lia64_gid_t lia64_gid, lia64_egid, lia64_sgid;
	gid_t host_gid, host_egid, host_sgid;

	*status = getresgid (&host_gid, &host_egid, &host_sgid);
	lia64_gid = host_gid;
	lia64_egid = host_egid;
	lia64_sgid = host_sgid;
	if ((int) *status != -1)
	  {
	    memBBWrt_alloc (ADDPTR (arg0), (BYTE *) &lia64_gid,
			    sizeof (lia64_gid));
	    memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &lia64_egid,
			    sizeof (lia64_gid));
	    memBBWrt_alloc (ADDPTR (arg2), (BYTE *) &lia64_sgid,
			    sizeof (lia64_gid));
	  }
      }
      setStatReturn (ret, status);
      break;

    case LIA64_setresgid:
      *status = setresgid (arg0, arg1, arg2);
      setStatReturn (ret, status);
      break;

    case LIA64_getgroups:
      *status = getgroups (sizeof (buf) / sizeof (lia64_gid_t),
			   (gid_t *) buf);
      if ((int) *status != -1)
	for (i = 0, bp = buf; i < MIN (*status, arg0); ++i)
	  {
	    lia64_gid = *(gid_t *) bp;
	    memBBWrt_alloc (arg1, (BYTE *) &lia64_gid, sizeof (lia64_gid));
	    bp += sizeof (gid_t);
	    arg1 += sizeof (lia64_gid);
	  }
      setStatReturn (ret, status);
      break;

    case LIA64_setgroups:
      for (i = 0, bp = buf, tmp = arg1; i < arg0; ++i)
	{
	  memBBRd (tmp, (BYTE *) &lia64_gid, sizeof (lia64_gid));
	  *(gid_t *) bp = lia64_gid;
	  bp += sizeof (gid_t);
	  tmp += sizeof (lia64_gid);
	}
      *status = setgroups (arg0, (const gid_t *) buf);
      setStatReturn (ret, status);
      break;

    case LIA64_getpgid:
      *status = __getpgid (arg0);
      setStatReturn (ret, status);
      break;

    case LIA64_setpgid:
      *status = setpgid (arg0, arg1);
      setStatReturn (ret, status);
      break;

    case LIA64_setsid:
      *status = setsid ();
      setStatReturn (ret, status);
      break;

    case LIA64_getsid:
      *status = getsid (arg0);
      setStatReturn (ret, status);
      break;

    case LIA64_sethostname:
      if (arg1 > 0)
	memBBRd(ADDPTR(arg0), buf, arg1);
      *status = sethostname ((char *) buf, arg1);
      setStatReturn (ret, status);
      break;

    case LIA64_setrlimit:
      memBBRd (arg1, (BYTE *)&lia64_rlimit, sizeof (struct lia64_rlimit));
      host_rlimit.rlim_cur = lia64_rlimit.rlim_cur;
      host_rlimit.rlim_max = lia64_rlimit.rlim_max;
      switch (arg0)
	{
	case LIA64_RLIMIT_CPU:		resource = RLIMIT_CPU; break;
	case LIA64_RLIMIT_FSIZE: 	resource = RLIMIT_FSIZE; break;
	case LIA64_RLIMIT_DATA:		resource = RLIMIT_DATA; break;
	case LIA64_RLIMIT_STACK: 	resource = RLIMIT_STACK; break;
	case LIA64_RLIMIT_CORE:		resource = RLIMIT_CORE; break;
	case LIA64_RLIMIT_RSS:		resource = RLIMIT_RSS; break;
	case LIA64_RLIMIT_NPROC: 	resource = RLIMIT_NPROC; break;
	case LIA64_RLIMIT_NOFILE: 	resource = RLIMIT_NOFILE; break;
	case LIA64_RLIMIT_MEMLOCK: 	resource = RLIMIT_MEMLOCK; break;
	case LIA64_RLIMIT_AS:		resource = RLIMIT_AS; break;
	default:			resource = -1; break;
	}
      *status = setrlimit (resource, &host_rlimit);
      setStatReturn (ret, status);
      break;

    case LIA64_getrlimit:
      switch (arg0)
	{
	case LIA64_RLIMIT_CPU:		resource = RLIMIT_CPU; break;
	case LIA64_RLIMIT_FSIZE: 	resource = RLIMIT_FSIZE; break;
	case LIA64_RLIMIT_DATA:		resource = RLIMIT_DATA; break;
	case LIA64_RLIMIT_STACK: 	resource = RLIMIT_STACK; break;
	case LIA64_RLIMIT_CORE:		resource = RLIMIT_CORE; break;
	case LIA64_RLIMIT_RSS:		resource = RLIMIT_RSS; break;
	case LIA64_RLIMIT_NPROC: 	resource = RLIMIT_NPROC; break;
	case LIA64_RLIMIT_NOFILE: 	resource = RLIMIT_NOFILE; break;
	case LIA64_RLIMIT_MEMLOCK: 	resource = RLIMIT_MEMLOCK; break;
	case LIA64_RLIMIT_AS:		resource = RLIMIT_AS; break;
	default:			resource = -1; break;
	}
      *status = getrlimit (resource, &host_rlimit);
      if ((int) *status != -1)
	{
	  lia64_rlimit.rlim_cur = host_rlimit.rlim_cur;
	  lia64_rlimit.rlim_max = host_rlimit.rlim_max;
	  memBBWrt_alloc (arg1, (BYTE *)&lia64_rlimit,
			  sizeof (struct lia64_rlimit));
	}
      setStatReturn (ret, status);
      break;

    // int prlimit64(pid_t pid, int resource,
    // 		     const struct rlimit64 *new, struct rlimit64 *old);
    case LIA64_prlimit64:

      if (arg2)
      {
	memBBRd (arg2, (BYTE *)&lia64_rlimit, sizeof (struct lia64_rlimit));
        host_rlimit.rlim_cur = lia64_rlimit.rlim_cur;
        host_rlimit.rlim_max = lia64_rlimit.rlim_max;
      }

      switch (arg1)
	{
	case LIA64_RLIMIT_CPU:		resource = RLIMIT_CPU; break;
	case LIA64_RLIMIT_FSIZE: 	resource = RLIMIT_FSIZE; break;
	case LIA64_RLIMIT_DATA:		resource = RLIMIT_DATA; break;
	case LIA64_RLIMIT_STACK: 	resource = RLIMIT_STACK; break;
	case LIA64_RLIMIT_CORE:		resource = RLIMIT_CORE; break;
	case LIA64_RLIMIT_RSS:		resource = RLIMIT_RSS; break;
	case LIA64_RLIMIT_NPROC: 	resource = RLIMIT_NPROC; break;
	case LIA64_RLIMIT_NOFILE: 	resource = RLIMIT_NOFILE; break;
	case LIA64_RLIMIT_MEMLOCK: 	resource = RLIMIT_MEMLOCK; break;
	case LIA64_RLIMIT_AS:		resource = RLIMIT_AS; break;
	default:			resource = -1; break;
	}

      *status = prlimit (arg0, resource, arg2 ? &host_rlimit : 0, arg3 ? &host_rlimit_old : 0);

      if ((int) *status != -1 && arg3)
	{
	  lia64_rlimit_old.rlim_cur = host_rlimit_old.rlim_cur;
	  lia64_rlimit_old.rlim_max = host_rlimit_old.rlim_max;
	  memBBWrt_alloc (arg3, (BYTE *)&lia64_rlimit_old,
			  sizeof (struct lia64_rlimit));
	}

      setStatReturn (ret, status);
      break;

    case LIA64_getrusage:
      *status = getrusage (arg0, &host_rusage);
      if ((int) *status != -1)
	rusage_host2lia64 (&host_rusage, &lia64_rusage);
      setStatReturn (ret, status);
      break;

    case SYS_GETTIMEOFDAY:
    case LIA64_gettimeofday:
      *status = gettimeofday (&host_timeval, &tzbuf);
      if ((int)*status != -1)
	{
	  lia64_timeval.tv_sec = host_timeval.tv_sec;
	  lia64_timeval.tv_usec = host_timeval.tv_usec;
	  memBBWrt_alloc (arg0, (BYTE *) &lia64_timeval,
			  sizeof (lia64_timeval));
	  memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &tzbuf,
			  sizeof (tzbuf));
	}
      setStatReturn (ret, status);
      break;

    case LIA64_settimeofday:
      memBBRd (arg0, (BYTE *) &lia64_timeval, sizeof (lia64_timeval));
      memBBRd (ADDPTR (arg1), (BYTE *) &tzbuf, sizeof (tzbuf));
      host_timeval.tv_sec = lia64_timeval.tv_sec;
      host_timeval.tv_usec = lia64_timeval.tv_usec;
      *status = settimeofday (&host_timeval, &tzbuf);
      setStatReturn (ret, status);
      break;

    // int clock_gettime(clockid_t clockid, struct timespec *tp);
    case LIA64_clock_gettime:
      *status = clock_gettime (arg0, &host_timespec);
      if ((int)*status != -1)
	{
	  lia64_timespec.tv_sec = host_timespec.tv_sec;
	  lia64_timespec.tv_nsec = host_timespec.tv_nsec;
	  memBBWrt_alloc (arg1, (BYTE *) &lia64_timespec,
			  sizeof (struct lia64_timespec));
	}
      setStatReturn (ret, status);
      break;

    case LIA64_select:
      nfds = 0;
      if (arg1)
	{
	  memBBRd (ADDPTR (arg1), buf, FD_SETSIZE/8);
	  readfds = mapSelectFds (buf, arg0, &nfds);
	  rfds = &readfds;
	}
      else
	rfds = NULL;
      if (arg2)
	{
	  memBBRd (ADDPTR (arg2), buf, FD_SETSIZE/8);
	  writefds = mapSelectFds (buf, arg0, &nfds);
	  wfds = &writefds;
	}
      else
	wfds = NULL;
      if (arg3)
	{
	  memBBRd (ADDPTR (arg3), buf, FD_SETSIZE/8);
	  errorfds = mapSelectFds (buf, arg0, &nfds);
	  efds = &errorfds;
	}
      else
	efds = NULL;
      tmout = NULL;
      if (arg4)
	{
	  memBBRd (arg4, (BYTE *) &lia64_timeval, sizeof (lia64_timeval));
	  host_timeval.tv_sec = lia64_timeval.tv_sec;
	  host_timeval.tv_usec = lia64_timeval.tv_usec;
	  tmout = &host_timeval;
	}
      *status = select (nfds, rfds, wfds, efds, tmout);
      if (rfds)
	{
	  readfds = mapBackSelectFds (rfds, nfds);
	  memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &readfds, FD_SETSIZE/8);
	}
      if (wfds)
	{
	  writefds = mapBackSelectFds (wfds, nfds);
	  memBBWrt_alloc (ADDPTR (arg2), (BYTE *) &writefds, FD_SETSIZE/8);
	}
      if (efds)
	{
	  errorfds = mapBackSelectFds (efds, nfds);
	  memBBWrt_alloc (ADDPTR (arg3), (BYTE *) &errorfds, FD_SETSIZE/8);
	}
      setStatReturn (ret, status);
      break;

    case LIA64_poll:
      {
	struct pollfd *host_ufds;

	ENSURE_SIZE (arg1*sizeof (struct pollfd));
	host_ufds = (struct pollfd *) malargbuf;
	if (arg1 > 0)
	  {
	    memBBRd (ADDPTR (arg0), malargbuf, arg1*sizeof (struct pollfd));
	    for (i = 0; i < arg1; ++i)
	      {
		backmap[i] = host_ufds[i].fd;	/* save the original fd */
		host_ufds[i].fd = fdmap[host_ufds[i].fd];
	      }
	  }
	*status = poll ((struct pollfd *) malargbuf, arg1, arg2);
	if ((int) *status != -1)
	  {
	    for (i = 0; i < arg1; ++i)
	      host_ufds[i].fd = backmap[i];	/* restore original fd */
	    memBBWrt_alloc (ADDPTR (arg0), malargbuf,
			    arg1*sizeof (struct pollfd));
	  }
      }
      setStatReturn (ret, status);
      break;

    case SYS_SYMLINK:
    case LIA64_symlink:
      memBBRd (ADDPTR (arg0), buf, 0);
      memBBRd (ADDPTR (arg1), buf2, 0);
      *status = symlink ((char *) buf, (char *) buf2);
      setStatReturn (ret, status);
      break;

    case SYS_READLINK:
    case LIA64_readlink:
      simroot(ADDPTR(arg0), buf, 0);
      /* make sure we have enough space in malargbuf */
      ENSURE_SIZE (arg2);
      *status = readlink ((char *) buf, (char *) malargbuf, arg2);
      if ((int) *status != -1)
	memBBWrt_alloc (ADDPTR (arg1), malargbuf, (int) *status);
      setStatReturn (ret, status);
      break;

    case LIA64_uselib:
      memBBRd (ADDPTR (arg0), buf, 0);
      *status = uselib ((char *) buf);
      setStatReturn (ret, status);
      break;

    case LIA64_swapon:
      memBBRd (ADDPTR (arg0), buf, 0);
      *status = swapon ((char *)buf, arg1);
      setStatReturn (ret, status);
      break;

    case LIA64_swapoff:
      memBBRd (ADDPTR (arg0), buf, 0);
      *status = swapoff ((char *)buf);
      setStatReturn (ret, status);
      break;

    case LIA64_reboot:
      *status = reboot (arg2);
      setStatReturn (ret, status);
      break;

    case SYS_TRUNCATE64:
    case LIA64_truncate:
      memBBRd (ADDPTR (arg0), buf, 0);
      *status = truncate ((char *) buf, arg1);
      setStatReturn (ret, status);
      break;

    case SYS_FTRUNCATE64:
    case LIA64_ftruncate:
      CHECK_FD (arg0);
      *status = ftruncate (fdmap[arg0], arg1);
      setStatReturn (ret, status);
      break;

    case SYS_FCHMOD:
    case LIA64_fchmod:
      CHECK_FD (arg0);
      *status = fchmod (fdmap[arg0], arg1);
      setStatReturn (ret, status);
      break;

    case SYS_FCHOWN:
    case LIA64_fchown:
      CHECK_FD (arg0);
      *status = fchown (fdmap[arg0], arg1, arg2);
      setStatReturn (ret, status);
      break;

    case LIA64_getpriority:
      errno = 0;
      *status = getpriority (arg0, arg1);
      setStatReturn (ret, status);
      break;

    case LIA64_setpriority:
      *status = setpriority (arg0, arg1, arg2);
      setStatReturn (ret, status);
      break;

    case LIA64_statfs:
    case LIA64_fstatfs:
      if (num == LIA64_statfs)
	{
	  memBBRd (ADDPTR (arg0), buf, 0);
	  *status = statfs ((char *)buf, &host_statfs);
	}
      else
	{
	  CHECK_FD (arg0);
	  *status = fstatfs (fdmap[arg0], &host_statfs);
	}
      if ((int) *status != -1)
	{
	  lia64_statfs.f_type = host_statfs.f_type;
	  lia64_statfs.f_bsize = host_statfs.f_bsize;
	  lia64_statfs.f_blocks = host_statfs.f_blocks;
	  lia64_statfs.f_bfree = host_statfs.f_bfree;
	  lia64_statfs.f_bavail = host_statfs.f_bavail;
	  lia64_statfs.f_files = host_statfs.f_files;
	  lia64_statfs.f_ffree = host_statfs.f_ffree;
	  lia64_statfs.f_fsid.val[0] = host_statfs.f_fsid.__val[0];
	  lia64_statfs.f_fsid.val[1] = host_statfs.f_fsid.__val[1];
	  lia64_statfs.f_namelen = host_statfs.f_namelen;
	  memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &lia64_statfs,
			  sizeof (lia64_statfs));
	}
      setStatReturn (ret, status);
      break;

    case LIA64_semget:
      *status = semget (arg0, arg1, arg2);
      setStatReturn (ret, status);
      break;

    case LIA64_semop:
      if (arg2 > 0)
	memBBRd (ADDPTR (arg1), buf, arg2*sizeof (struct sembuf));
      *status = semop (arg0, (struct sembuf *) buf, arg2);
      setStatReturn (ret, status);
      break;

    case LIA64_semctl:
      switch (arg2)
	{
	case IPC_RMID:
	case GETALL:
	case SETALL:
	case GETVAL:
	case GETPID:
	case GETNCNT:
	case SETVAL:
	  *status = semctl (arg0, arg1, arg2, arg3);
	  setStatReturn (ret, status);
	  break;

	default:
	  sprintf ((char *)msg, "Ignoring unimplemented semctl() system call\n");
	  sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
	}
      break;

    case LIA64_msgget:
      *status = semget (arg0, arg1, arg2);
      setStatReturn (ret, status);
      break;

    case LIA64_msgsnd:
    case LIA64_msgrcv:
      memBBRd (ADDPTR (arg1), buf, sizeof (REG) + arg2);
      bp = buf;
      if (sizeof (long) == 4)
	{
	  buf[7] = buf[3]; buf[6] = buf[2]; buf[5] = buf[1]; buf[4] = buf[0];
	  bp = buf + 4;
	}
      if (num == LIA64_msgsnd)
	*status = msgsnd (arg0, bp, arg2, arg3);
      else
	*status = msgrcv (arg0, bp, arg2, arg3, arg4);
      setStatReturn (ret, status);
      break;

    case LIA64_msgctl:
      if (arg1 == IPC_RMID)
	{
	  *status = msgctl (arg0, arg1, NULL);
	  setStatReturn (ret, status);
	}
      else
	{
	  sprintf ((char *)msg, "Ignoring unimplemented msgctl() system call\n");
	  sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
	}
      break;

    case LIA64_shmget:
      arg1 = page_align(arg1);
      *status = shmget (arg0, arg1, arg2);

      if ((int) *status != -1)
	{
	  if (arg0 != IPC_PRIVATE && !(arg2 & IPC_CREAT))
	    {
	      struct shmid_ds shmid_ds;

	      /* get segment size from existing shm segment: */
	      if (shmctl ((int) *status, IPC_STAT, &shmid_ds) != -1)
		arg1 = shmid_ds.shm_segsz;
	    }
	  shmseg_new ((int) *status, arg1);
	}
      setStatReturn (ret, status);
      break;

    case LIA64_set_tid_address:
      arg0 = ADDPTR (arg0);
      *status = syscall (__NR_set_tid_address, arg0);
      setStatReturn (ret, status);
      break;

    case LIA64_shmat:
      *ret = EINVAL;
      shmseg = shmseg_lookup_id (arg0);
      if (!shmseg)
	{
	  struct shmid_ds shmid_ds;

	  /* get segment size from existing shm segment: */
	  if (shmctl (arg0, IPC_STAT, &shmid_ds) != -1)
	    shmseg = shmseg_new (arg0, shmid_ds.shm_segsz);
	}
      if (shmseg)
	{
	  *status = (REG) (long) shmat (arg0, NULL, arg2);
	  if ((long) *status != -1)
	    {
	      hostadr = (BYTE *) (long) *status;

	      if (arg1 == 0)
		arg1 = get_unmapped_area (shmseg->size);

	      shmseg->addr = arg1;
	      shmseg->hostaddr = hostadr;

	      end = page_align(arg1 + shmseg->size);
	      for (adr = arg1; adr < end; adr += page_size)
		{
		  memMPMap (adr, (char *)hostadr, 1);
		  hostadr += page_size;
		}
	      *status = ADDPTR (arg1);
	    }
	}
      setStatReturn (ret, status);
      break;

    case LIA64_shmdt:
      *ret = EINVAL;
      shmseg = shmseg_lookup_addr (arg0);
      if (shmseg)
	{
	  if (shmseg->addr)
	    {
	      *status = shmdt (shmseg->hostaddr);
	      end = page_align(shmseg->addr + shmseg->size);

	      for (adr = shmseg->addr; adr < end; adr += page_size)
		memFree (adr);
	      setStatReturn (ret, status);
	    }
	  shmseg_free (shmseg);
	}
      break;

    case LIA64_shmctl:
      switch (arg1)
	{
	case IPC_RMID:
	case SHM_LOCK:
	case SHM_UNLOCK:
	  *status = shmctl (arg0, arg1, NULL);
	  setStatReturn (ret, status);
	  break;

	default:
	  sprintf ((char *)msg, "Ignoring unimplemented shmctl() system call\n");
	  sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
	  break;
	}
      break;

    case LIA64_syslog:
      if (arg2 > 0)
	memBBRd (ADDPTR (arg1), buf, arg2);
      *status = klogctl (arg0, (char *)buf, arg2);
      setStatReturn (ret, status);
      break;

    case LIA64_setitimer:
      /* note: we do real time timers here---the simulator is much
	 slower than real hw which could cause premature/unwarranted
	 firing of timers.  */
      memBBRd (ADDPTR (arg1), (BYTE *) &lia64_itval, sizeof (lia64_itval));
      if (lia64_itval.it_interval.tv_sec == 0
	  && lia64_itval.it_interval.tv_usec == 1)
	{
	  /* some code, in particular the glibc profil() code, use
	     this to determine the timer granularity; to avoid dying
	     due to a continuous stream of SIGPROF events, we fake an
	     artificially low granularity... */
	  lia64_itval.it_interval.tv_usec = 50000;
	}
      host_itval.it_interval.tv_sec = lia64_itval.it_interval.tv_sec;
      host_itval.it_interval.tv_usec = lia64_itval.it_interval.tv_usec;
      host_itval.it_value.tv_sec = lia64_itval.it_value.tv_sec;
      host_itval.it_value.tv_usec = lia64_itval.it_value.tv_usec;
      *status = setitimer (arg0, &host_itval, &host_itval);
      if ((int) *status != -1 && arg2 != 0)
	{
	  lia64_itval.it_interval.tv_sec = host_itval.it_interval.tv_sec;
	  lia64_itval.it_interval.tv_usec = host_itval.it_interval.tv_usec;
	  lia64_itval.it_value.tv_sec = host_itval.it_value.tv_sec;
	  lia64_itval.it_value.tv_usec = host_itval.it_value.tv_usec;
	  memBBWrt_alloc (ADDPTR (arg2), (BYTE *) &lia64_itval,
			  sizeof (lia64_itval));
	}
      setStatReturn (ret, status);
      break;

    case LIA64_getitimer:
      *status = getitimer (arg0, &host_itval);
      if ((int) *status != -1 && arg2 != 0)
	{
	  lia64_itval.it_interval.tv_sec = host_itval.it_interval.tv_sec;
	  lia64_itval.it_interval.tv_usec = host_itval.it_interval.tv_usec;
	  lia64_itval.it_value.tv_sec = host_itval.it_value.tv_sec;
	  lia64_itval.it_value.tv_usec = host_itval.it_value.tv_usec;
	  memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &lia64_itval,
			  sizeof (lia64_itval));
	}
      setStatReturn (ret, status);
      break;

    case LIA64_old_stat:
      simroot(ADDPTR(arg0), buf, 0);
      *status = stat ((char *) buf, &host_stat);
      if ((int) *status != -1)
	{
	  old_stat_host2lia64 (&host_stat, &lia64_old_stat);
	  memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &lia64_old_stat,
			  sizeof (lia64_old_stat));
	}
      setStatReturn (ret, status);
      break;

    case LIA64_stat:
      simroot(ADDPTR(arg0), buf, 0);
      *status = stat ((char *) buf, &host_stat);
      if ((int) *status != -1)
	{
	  stat_host2lia64 (&host_stat, &lia64_stat);
	  memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &lia64_stat,
			  sizeof (lia64_stat));
	}
      setStatReturn (ret, status);
      break;

    case LIA64_old_lstat:
      simroot(ADDPTR(arg0), buf, 0);
      *status = lstat ((char *) buf, &host_stat);
      if ((int) *status != -1)
	{
	  old_stat_host2lia64 (&host_stat, &lia64_old_stat);
	  memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &lia64_old_stat,
			  sizeof (lia64_old_stat));
	}
      setStatReturn (ret, status);
      break;

    case LIA64_lstat:
      simroot(ADDPTR(arg0), buf, 0);
      *status = lstat ((char *) buf, &host_stat);
      if ((int) *status != -1)
	{
	  stat_host2lia64 (&host_stat, &lia64_stat);
	  memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &lia64_stat,
			  sizeof (lia64_stat));
	}
      setStatReturn (ret, status);
      break;

    case LIA64_old_fstat:
      CHECK_FD (arg0);
      *status = fstat (fdmap[arg0], &host_stat);
      if ((int) *status != -1)
	{
	  old_stat_host2lia64 (&host_stat, &lia64_old_stat);
	  memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &lia64_old_stat,
			  sizeof (lia64_old_stat));
	}
      setStatReturn (ret, status);
      break;

    case LIA64_fstat:
      CHECK_FD (arg0);
      *status = fstat (fdmap[arg0], &host_stat);
      if ((int) *status != -1)
	{
	  stat_host2lia64 (&host_stat, &lia64_stat);
	  memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &lia64_stat,
			  sizeof (lia64_stat));
	}
      setStatReturn (ret, status);
      break;

    case LIA64_newfstatat:
      /* arg0: int fd
         arg1: const char * filename
         arg2: stat * statbuf
         arg3: int flags
      */
      CHECK_FD (arg0);
      memBBRd (ADDPTR (arg1), buf, 0);
      *status = fstatat (fdmap[arg0], buf, &host_stat, arg3);
      if ((int) *status != -1)
	{
	  stat_host2lia64 (&host_stat, &lia64_stat);
	  memBBWrt_alloc (ADDPTR (arg2), (BYTE *) &lia64_stat,
			  sizeof (lia64_stat));
	}
      setStatReturn (ret, status);
      break;

    case LIA64_vhangup:
      *status = vhangup ();
      setStatReturn (ret, status);
      break;

    case LIA64_lchown:
      memBBRd (ADDPTR (arg0), buf, 0);
      *status = lchown ((char *)buf, arg1, arg2);
      setStatReturn (ret, status);
      break;

    case LIA64_vm86:
      sprintf ((char *)msg, "Ignoring unimplemented vm86() system call\n");
      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      break;

    case LIA64_wait4:
      *status = wait4 (arg0, &waitstatus, arg2, &host_rusage);
      if ((int) *status != -1)
	{
	  if (arg1)
	    memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &waitstatus, 4);
	  if (arg3)
	    {
	      rusage_host2lia64 (&host_rusage, &lia64_rusage);
	      memBBWrt_alloc (ADDPTR (arg3), (BYTE *) &lia64_rusage,
			sizeof (lia64_rusage));
	    }
	}
      setStatReturn (ret, status);
      break;

    case LIA64_sysinfo:
      sprintf ((char *)msg, "Ignoring unimplemented sysinfo() system call\n");
      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      break;

    case LIA64_clone: /* (flags, newsp, parent_tid, tls, child_tid) */
    case LIA64_clone2: /* (flags, newsp, stack_size, parent_tid, child_tid, tls) */
      sig = arg0 & 0xff;
      tmp = arg0 & ~sig;
      if (   sig == SIGCHLD
	  && arg1 == 0
	  && ((tmp & ~(CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID | CLONE_CHILD_SETTID) == 0) /* fork()-ish */
	      || (tmp == CLONE_VFORK | CLONE_VM)) /* vfork()-ish */
	  )
	{
	  /* we're doing a plain-ol' fork or vfork */
	  *status = fork ();
	  setStatReturn (ret, status);

	  if ((int) *status == 0 && interface != BATCH)
	    {
	      interface = BATCH;
	      fdmap_make_identity_map ();
	    }
	}
      else
	{
	  sprintf ((char *)msg, "Ignoring unimplemented clone(flags=%#llx, newsp=%#llx) system call sig=%d tmp=%#llx\n", (long long)arg0, (long long)arg1, sig, (long long)tmp);
	  sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
	}
      break;

    case LIA64_setdomainname:
      if (arg1 > 0)
	memBBRd (ADDPTR (arg0), buf, arg1);
      *status = setdomainname ((char *)buf, arg1);
      setStatReturn (ret, status);
      break;

    case LIA64_uname:
      *status = uname ((struct utsname *) buf);
      if ((int) *status == 0)
	{
	  strcpy (&((struct utsname *)buf)->machine[0], "ia64");
	  /* New glibc's check for minimum kernel. Defined in glibc as:
	   * sysdeps/unix/sysv/linux/ia64/configure.ac:arch_minimum_kernel=3.2.18
	   */
	  strcpy (&((struct utsname *)buf)->release[0], "3.2.18");
	  memBBWrt_alloc (ADDPTR (arg0), buf, sizeof (struct utsname));
	}
      setStatReturn (ret, status);
      break;

    case LIA64_adjtimex:
      sprintf ((char *)msg, "Ignoring unimplemented adjtimex() system call\n");
      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      break;

    case LIA64_create_module:
    case LIA64_init_module:
    case LIA64_delete_module:
    case LIA64_get_kernel_syms:
    case LIA64_query_module:
      sprintf ((char *)msg, "Ignoring unimplemented module system call %d\n",
               num);
      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      break;

    case LIA64_quotactl:
      sprintf ((char *)msg, "Ignoring unimplemented quotactl() system call\n");
      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      break;

    case LIA64_bdflush:
      sprintf ((char *)msg, "Ignoring unimplemented bdflush() system call\n");
      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      break;

    case LIA64_sysfs:
      sprintf ((char *)msg, "Ignoring unimplemented sysfs() system call\n");
      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      break;

    case LIA64_personality:
      if ((arg0 & PER_MASK) == PER_LINUX)
	{
	  *status = PER_LINUX;
	  setStatReturn (ret, status);
	}
      else
	{
	  sprintf ((char *)msg, "Ignoring unimplemented personality(0x%x) call\n",
		   (int) arg0);
	  sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
	}
      break;

    case LIA64_afs_syscall:
      sprintf ((char *)msg, "Ignoring unimplemented afs_syscall()\n");
      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      break;

    case LIA64_setfsuid:
      *status = setfsuid (arg0);
      setStatReturn (ret, status);
      break;

    case LIA64_setfsgid:
      *status = setfsgid (arg0);
      setStatReturn (ret, status);
      break;

    case LIA64_getdents:
      CHECK_FD (arg0);
      max = arg2 / (sizeof (*host_dirent) - sizeof (host_dirent->d_name) + 1);
      *status = getdents (fdmap[arg0], (struct dirent *) buf,
			  arg2 - max*(18 - dirent_name_off));
      if ((int) *status != -1)
	{
	  REG end, total = 0;
	  long long lia64_ino;
	  lia64_off_t lia64_off;
	  unsigned short lia64_reclen;

	  end = arg1 + arg2;
	  for (bp = buf; bp < buf + *status; bp += host_dirent->d_reclen)
	    {
	      host_dirent = (struct dirent *) bp;

	      lia64_ino = host_dirent->d_ino;
	      lia64_off = host_dirent->d_off;
	      lia64_reclen = (host_dirent->d_reclen + (18 - dirent_name_off)
			      + 7) & ~7;

	      memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &lia64_ino,
			      sizeof (lia64_ino));
	      memBBWrt_alloc (ADDPTR (arg1 + 8), (BYTE *) &lia64_off,
			      sizeof (lia64_off));
	      memBBWrt_alloc (ADDPTR (arg1 + 16), (BYTE *) &lia64_reclen, 2);
	      memBBWrt_alloc (ADDPTR (arg1 + 18), (BYTE *)host_dirent->d_name,
			      strlen (host_dirent->d_name) + 1);

	      arg1 += lia64_reclen;
	      total += lia64_reclen;
	    }
	  *status = total;
	}
      setStatReturn (ret, status);
      break;

    case LIA64_getdents64:
      CHECK_FD (arg0);
      max = arg2 / (sizeof (*host_dirent) - sizeof (host_dirent->d_name) + 1);
      *status = nbytes = getdents (fdmap[arg0], (struct dirent *) buf,
				   arg2 - max*(19 - dirent_name_off));
      if ((int) *status != -1)
	{
	  REG end, total = 0;
	  long long lia64_ino;
	  lia64_off_t lia64_off;
	  unsigned char lia64_type;
	  unsigned short lia64_reclen;
	  int old_wd;
	  int r;

	  *status = old_wd = open (".", 0);
	  if ((int) *status == -1)
	    {
	      setStatReturn (ret, status);
	      break;
	    }

	  *status = fchdir (fdmap[arg0]);
	  if ((int) *status == -1)
	    {
	      int r;
	      r = fchdir (old_wd);
	      (void)r; /* TODO */
	      close (old_wd);
	      setStatReturn (ret, status);
	      break;
	    }

	  end = arg1 + arg2;
	  for (bp = buf; bp < buf + nbytes; bp += host_dirent->d_reclen)
	    {
	      host_dirent = (struct dirent *) bp;

	      lia64_ino = host_dirent->d_ino;
	      lia64_off = host_dirent->d_off;
	      lia64_reclen = (host_dirent->d_reclen + (19 - dirent_name_off)
			      + 7) & ~7;
	      lia64_type = 0;	/* DT_UNKNOWN */

	      *status = stat (host_dirent->d_name, &host_stat);
	      if ((int) *status != -1)
		{
		  if (S_ISLNK (host_stat.st_mode))
		    lia64_type = 10;		/* DT_LNK */
		  else if (S_ISREG (host_stat.st_mode))
		    lia64_type = 8;		/* DT_REG */
		  else if (S_ISDIR (host_stat.st_mode))
		    lia64_type = 4;		/* DT_DIR */
		  else if (S_ISCHR (host_stat.st_mode))
		    lia64_type = 2;		/* DT_CHR */
		  else if (S_ISBLK (host_stat.st_mode))
		    lia64_type = 6;		/* DT_BLK */
		  else if (S_ISFIFO (host_stat.st_mode))
		    lia64_type = 1;		/* DT_FIFO */
		  else if (S_ISSOCK (host_stat.st_mode))
		    lia64_type = 12;		/* DT_SOCK */
		}
	      memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &lia64_ino,
			      sizeof (lia64_ino));
	      memBBWrt_alloc (ADDPTR (arg1 + 8), (BYTE *) &lia64_off,
			      sizeof (lia64_off));
	      memBBWrt_alloc (ADDPTR (arg1 + 16), (BYTE *) &lia64_reclen, 2);
	      memBBWrt_alloc (ADDPTR (arg1 + 18), (BYTE *) &lia64_type, 1);
	      memBBWrt_alloc (ADDPTR (arg1 + 19), (BYTE *) host_dirent->d_name,
			      strlen (host_dirent->d_name) + 1);

	      arg1 += lia64_reclen;
	      total += lia64_reclen;
	    }

	  r = fchdir (old_wd);
	  (void)r; /* TODO */
	  close (old_wd);

	  *status = total;
	}
      setStatReturn (ret, status);
      break;

    case LIA64_flock:
      CHECK_FD (arg0);
      *status = flock (fdmap[arg0], arg1);
      setStatReturn (ret, status);
      break;

    case LIA64_readv:
      CHECK_FD (arg0);
      *status = 0;
      for (i = 0; i < arg2; ++i)
	{
	  memBBRd (ADDPTR (arg1), (BYTE *) &lia64_iovec, sizeof (lia64_iovec));
	  ENSURE_SIZE (lia64_iovec.iov_len);
	  tmp = read (fdmap[arg0], malargbuf, lia64_iovec.iov_len);
	  if ((ssize_t) tmp == -1)
	    {
	      *status = -1;
	      break;
	    }
	  memBBWrt_alloc (ADDPTR (lia64_iovec.iov_base), malargbuf, tmp);
	  *status += tmp;
	  if (tmp < lia64_iovec.iov_len)
	    break;
	  arg1 += sizeof (lia64_iovec);
	}
      setStatReturn (ret, status);
      break;

    case LIA64_writev:
      CHECK_FD (arg0);
      *status = 0;
      for (i = 0; i < arg2; ++i)
	{
	  memBBRd (ADDPTR (arg1), (BYTE *) &lia64_iovec, sizeof (lia64_iovec));
	  ENSURE_SIZE (lia64_iovec.iov_len);
	  memBBRd (ADDPTR (lia64_iovec.iov_base), malargbuf,
		   lia64_iovec.iov_len);
	  tmp = write (fdmap[arg0], malargbuf, lia64_iovec.iov_len);
	  if ((ssize_t) tmp == -1)
	    {
	      *status = -1;
	      break;
	    }
	  *status += tmp;
	  if (tmp < lia64_iovec.iov_len)
	    break;
	  arg1 += sizeof (lia64_iovec);
	}
      setStatReturn (ret, status);
      break;

    case LIA64_pread:
      CHECK_FD (arg0);
      ENSURE_SIZE (arg2);
#if (__GLIBC__ > 2) || (__GLIBC__ == 2 && __GLIBC_MINOR__ > 0)
      *status = pread (fdmap[arg0], malargbuf, arg2, arg3);
#else
      *status = lseek (fdmap[arg0], arg3, SEEK_SET);
      if ((off_t) *status != -1)
	*status = read (fdmap[arg0], malargbuf, arg2);
#endif
      if ((ssize_t) *status != -1)
	memBBWrt_alloc (ADDPTR (arg1), malargbuf, *status);
      setStatReturn (ret, status);
      break;

    case LIA64_pwrite:
      CHECK_FD (arg0);
      ENSURE_SIZE (arg2);
      if (arg2 > 0)
	memBBRd (ADDPTR (arg1), malargbuf, arg2);
#if (__GLIBC__ > 2) || (__GLIBC__ == 2 && __GLIBC_MINOR__ > 0)
      *status = pwrite (fdmap[arg0], malargbuf, arg2, arg3);
#else
      *status = lseek (fdmap[arg0], arg3, SEEK_SET);
      if ((off_t) *status != -1)
	*status = write (fdmap[arg0], malargbuf, arg2);
#endif
      setStatReturn (ret, status);
      break;

    case LIA64__sysctl:
      break;

    case LIA64_mmap:
    case LIA64_mmap2:
      {
	int fd = -1, flags;

	if (num == LIA64_mmap2)
	  arg5 *= page_size;	/* convert page offset into byte offset */

	if (!(arg2 & PROT_WRITE))
	  {
	    arg2 |= PROT_WRITE;
	    if (arg3 & MAP_SHARED)
	      {
		arg3 &= ~(REG)MAP_SHARED;
		arg3 |=  (REG)MAP_PRIVATE;
	      }
	  }

	if (!(arg3 & MAP_ANONYMOUS))
	  {
	    CHECK_FD (arg4);
	    fd = fdmap[arg4];
	  }

	end = page_align(arg0 + arg1);

	if (arg3 & MAP_FIXED)
	  {
	    for (adr = arg0; adr < end; adr += page_size)
	      if (memIsMapped (adr))
		{
		  /* page exists: unmap it */
		  if (page_offset (adr) == 0)
		    {
		      *status = munmap ((void *) pmemLookup_p (adr), page_size);
		      if ((long) *status != 0)
			goto done;
		    }
		  memFree (adr);
		}
	  }
	else
	  {
	    arg0 = get_unmapped_area (arg1);
	    end = page_align(arg0 + arg1);
	  }

	flags = arg3 & ~MAP_FIXED;

	hostadr = mmap (0, arg1, arg2, flags, fd, arg5);
	if ((long) hostadr == -1)
	  {
	    *status = -1;
	    setStatReturn (ret, status);
	    goto done;
	  }

	for (adr = arg0; adr < end; adr += page_size)
	  {
		memMPMap (adr, (char *)hostadr, 1);
	    hostadr += page_size;
	  }
	*status = ADDPTR (arg0);

	if (!(arg3 & MAP_ANONYMOUS))
	  {
	    off_t save = lseek (fd, 0, SEEK_CUR);
	    mmapSyms (fd, *status, arg1, arg5);
	    lseek (fd, save, SEEK_SET);
	  }
	else if ((arg3 & MAP_PRIVATE) && (arg2 & PROT_EXEC))
	  dynBlock (*status, arg1);

	setStatReturn (ret, status);
      }
      break;

    case SYS_MUNMAP:
    case LIA64_munmap:
	  end = page_align(arg0 + arg1);
      for (adr = arg0; adr < end; adr += page_size)
	{
	  if (memIsMapped (adr))
	    {
	      if (page_offset (adr) == 0)
		{
		  *status = munmap ((void *) pmemLookup_p (adr), page_size);
		  if ((long) *status != 0)
		    break;
		}
	      memFree (adr);
	    }
	}
      munmapSyms (ADDPTR (arg0), ADDPTR (arg0) + arg1 - 1);
      setStatReturn (ret, status);
      break;

    case LIA64_mlock:
    case LIA64_mlockall:
    case LIA64_munlock:
    case LIA64_munlockall:
      /* ignore these calls */
      *status = 0;
      setStatReturn (ret, status);
      break;

    case LIA64_mremap:
      must_move = NO;
      for (adr = page_align(arg0 + arg1); adr < page_align(arg0 + arg2);
		   adr += page_size)
	if (memIsMapped (adr))
	  {
	    /* page exists already - move if we can */
	    if (!(arg3 & MREMAP_MAYMOVE))
	      {
		*status = ~(REG)0;
		*ret = ENOMEM;
		goto done;
	      }
	    must_move = YES;
	    break;
	  }

      new_base = page_base (arg0);
      if (must_move)
	{
	  /* find a suitably large free range of memory: */
	  new_base = get_unmapped_area (arg2);

	  /* remap old pages to new_base: */
	  tmp = new_base;
	  for (adr = page_base (arg0); adr < page_align(arg0 + arg1);
	       adr += page_size)
	    {
		  memMPMap (tmp, (char*) pmemLookup_p (adr), 1);
	      tmp += page_size;
	    }
	}

      /* expand mapping: */
      for (adr = page_align(new_base+arg1); adr < page_align(new_base+arg2);
		   adr += page_size)
	pmemLookup_p (adr);

      *status = new_base;
      setStatReturn (ret, status);
      break;

    case LIA64_mprotect:
      if (!arg2)
	{
	  /* unmap range if no access permissions: */
	  end = page_align(arg0 + arg1);
	  for (adr = arg0; adr < end; adr += page_size)
	    {
	      if (memIsMapped (adr))
		{
		  if (page_offset (adr) == 0)
		    {
		      *status = munmap ((void *) pmemLookup_p (adr), page_size);
		      if ((long) *status != 0)
			break;
		    }
		  memFree (adr);
		}
	    }
	  munmapSyms (ADDPTR (arg0), ADDPTR (arg0) + arg1 - 1);
	}
      else
	{
	  *status = 0;
	  while (arg1 > 0)
	    {
	      tmp = arg1;
	      if (tmp > page_size)
		tmp = page_size;

	      *status = mprotect ((void *) pmemLookup_p (arg0), tmp, arg2);
	      if ((long) *status == -1)
		break;

	      arg1 -= tmp;
	      arg0 += tmp;
	    }
	}
      setStatReturn (ret, status);
      break;

    case LIA64_msync:
      for (curr = arg0; curr < arg0 + arg1; curr += page_size)
	{
	  *status = msync ((void *) pmemLookup_p (ADDPTR (curr)), page_size, arg2);
	  if ((int) *status == -1)
	    break;
	}
      setStatReturn (ret, status);
      break;

    case LIA64_sched_getparam:
      *status = sched_setparam (arg0, (struct sched_param *) buf);
      if ((int) *status != -1)
	memBBWrt_alloc (ADDPTR (arg1), buf, sizeof (struct sched_param));
      setStatReturn (ret, status);
      break;

    case LIA64_sched_setparam:
      memBBRd (ADDPTR (arg1), buf, sizeof (struct sched_param));
      *status = sched_setparam (arg0, (struct sched_param *) buf);
      setStatReturn (ret, status);
      break;

    case LIA64_sched_getscheduler:
      *status = sched_getscheduler (arg0);
      setStatReturn (ret, status);
      break;

    case LIA64_sched_setscheduler:
      memBBRd (ADDPTR (arg2), buf, sizeof (struct sched_param));
      *status = sched_setscheduler (arg0, arg1, (struct sched_param *) buf);
      setStatReturn (ret, status);
      break;

    case LIA64_sched_yield:
      *status = sched_yield ();
      setStatReturn (ret, status);
      break;

    case LIA64_sched_get_priority_max:
      *status = sched_get_priority_max (arg0);
      setStatReturn (ret, status);
      break;

    case LIA64_sched_get_priority_min:
      *status = sched_get_priority_min (arg0);
      setStatReturn (ret, status);
      break;

    case LIA64_sched_rr_get_interval:
      *status = sched_rr_get_interval (arg0, &host_timespec);
      if ((int) *status != -1)
	{
	  lia64_timespec.tv_sec = host_timespec.tv_sec;
	  lia64_timespec.tv_nsec = host_timespec.tv_nsec;
	  memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &lia64_timespec,
		    sizeof (lia64_timespec));
	}
      setStatReturn (ret, status);
      break;

    case LIA64_nanosleep:
      memBBRd (ADDPTR (arg0), (BYTE *) &lia64_timespec,
	       sizeof (lia64_timespec));
      host_timespec.tv_sec = lia64_timespec.tv_sec;
      host_timespec.tv_nsec = lia64_timespec.tv_nsec;
      *status = nanosleep (&host_timespec, &host_timespec);
      if (arg1 != 0)
	{
	  if ((int) *status == 0)
	    memset (&lia64_timespec, 0, sizeof (lia64_timespec));
	  else
	    {
	      lia64_timespec.tv_sec = host_timespec.tv_sec;
	      lia64_timespec.tv_nsec = host_timespec.tv_nsec;
	    }
	  memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &lia64_timespec,
			  sizeof (lia64_timespec));
	}
      setStatReturn (ret, status);
      break;

    case LIA64_nfsservctl:
      sprintf ((char *)msg, "Ignoring unimplemented call to nfsservctl()\n");
      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      break;

    case LIA64_prctl:
      sprintf ((char *)msg, "Ignoring unimplemented call to prctl()\n");
      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      break;

    case LIA64_rt_sigaction:
      CHECK_SIGNUM (arg0);

      sa = sig_act + arg0;
      lia64_sigact = *sa;

      if (arg1)
	memBBRd (ADDPTR (arg1), (BYTE *) sa, sizeof (lia64_sigact));

      if (arg2)
	memBBWrt_alloc (ADDPTR (arg2), (BYTE *) &lia64_sigact,
			sizeof (lia64_sigact));

      memset (&act, 0, sizeof (act));
      act.sa_flags = sa->sa_flags & ~SA_ONSTACK;
      if (sa->lia_handler == (ADDR) SIG_DFL
	  || sa->lia_handler == (ADDR) SIG_IGN)
	act.sa_handler = (void (*)()) (long) sa->lia_handler;
      else
	act.sa_handler = signal_queue;
      *status = sigaction (arg0, &act, 0);
      setStatReturn (ret, status);
      break;

    case LIA64_rt_sigprocmask:
      *status = 0;
      *ret = 0;
      if (arg2)
	memBBWrt_alloc (arg2, (BYTE *) &masked_signals,
			sizeof (masked_signals));
      if (arg1)
	{
	  memBBRd (arg1, (BYTE *) &lia64_sigset, sizeof (lia64_sigset));
	  switch (arg0)
	    {
	    case SIG_BLOCK:
	      lia64_sigset |= masked_signals;
	      break;

	    case SIG_UNBLOCK:
	      lia64_sigset = masked_signals & ~lia64_sigset;
	      break;

	    case SIG_SETMASK:
	      break;

	    default:
	      *status = -1; *ret = EINVAL;
	      break;
	    }
	  if (status >= 0)
	    masked_signals = lia64_sigset;
	}
      break;

    case LIA64_old_getpagesize: /* was getpagesize(); we keep this for backwards compat. */
      *status = page_size;
      setStatReturn (ret, status);
      break;

    case LIA64_sigaltstack:
      /* Don't print a warning here---we'll print a warning when we
	 attempt to deliver a signal on an alternate stack instead.  */
      break;

    case LIA64_rt_sigsuspend:
      lia64_sigset = masked_signals;		/* save masked_signals */
      memBBRd (arg0, (BYTE *) &masked_signals, sizeof (masked_signals));
      /*
       * Block the host OS from delivering signals to avoid a
       * race condition between checking pending_signals and calling
       * sigsuspend:
       */
      sigfillset (&sigset);
      sigprocmask (SIG_BLOCK, &sigset, &saved_sigset);

      /*
       * Expose all signals to simulator; we filter out/defer signals
       * ourselves and always want to get Ctrl-C when in non-batch
       * mode...
       */
      sigemptyset (&sigset);

      *status = -1;
      errno = EINTR;

	  /* If no signals pending, call host's sigsuspend. */
      while (!signal_pending() && !userint)
		  *status = sigsuspend (&sigset);

      if (signal_pending())
		  signal_invoke_handler (1);
      else if (userint) {
		  *status = -1;
		  errno = EINTR;
	  }
      masked_signals = lia64_sigset;		/* restore masked signals */
      sigprocmask (SIG_SETMASK, &saved_sigset, 0);
      setStatReturn (ret, status);
      break;

    case LIA64_rt_sigpending:
      memBBWrt_alloc (arg0, (BYTE *) &pending_signals,
		      sizeof (pending_signals));
      *status = *ret = 0;
      break;

    case LIA64_rt_sigqueueinfo:
    case LIA64_rt_sigtimedwait:
      sprintf ((char *)msg, "Ignoring system call %d\n", num);
      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      *status = 0;
      setStatReturn (ret, status);
      break;

    case LIA64_getcwd:
      ENSURE_SIZE (arg1);
      cp = getcwd ((char *)malargbuf, arg1);
      if (cp)
	{
	  *status = strlen ((char *)malargbuf) + 1;
	  memBBWrt_alloc (ADDPTR (arg0), malargbuf, *status);
	}
      setStatReturn (ret, status);
      break;

    case LIA64_capget:
      /* GNU libc 2.0 doesn't seem to provide access to the capget()
         syscall */
      sprintf ((char *)msg, "Ignoring unimplemented call to capget()\n");
      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      break;

    case LIA64_capset:
      /* GNU libc 2.0 doesn't seem to provide access to the capset()
         syscall */
      sprintf ((char *)msg, "Ignoring unimplemented call to capset()\n");
      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      break;

    case LIA64_sendfile:
      /* GNU libc 2.0 doesn't seem to provide access to the sendfile()
         syscall */
      sprintf ((char *)msg, "Ignoring unimplemented call to sendfile()\n");
      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      break;

    case LIA64_getpmsg:
      /* GNU libc 2.0 doesn't seem to provide access to the getpmsg()
         syscall */
      sprintf ((char *)msg, "Ignoring unimplemented call to getpmsg()\n");
      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      break;

    case LIA64_putpmsg:
      /* GNU libc 2.0 doesn't seem to provide access to the putpmsg()
         syscall */
      sprintf ((char *)msg, "Ignoring unimplemented call to putpmsg()\n");
      sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      break;

    case LIA64_socket:
      *status = socket (arg0, arg1, arg2);
      if ((int)*status > -1)
	*status = fdmap_alloc (*status);
      setStatReturn (ret, status);
      break;

    case LIA64_bind:
      CHECK_FD (arg0);
      addrlen = arg2;
      if (addrlen > 0)
	{
	  ENSURE_SIZE (addrlen);
	  memBBRd (ADDPTR(arg1), malargbuf, addrlen);
	}
      *status = bind (fdmap[arg0], (struct sockaddr *) malargbuf, addrlen);
      setStatReturn (ret, status);
      break;

    case LIA64_connect:
      CHECK_FD (arg0);
      addrlen = arg2;
      if (addrlen > 0)
	{
	  ENSURE_SIZE (addrlen);
	  memBBRd (ADDPTR(arg1), malargbuf, addrlen);
	}
      *status = connect (fdmap[arg0], (struct sockaddr *) malargbuf, addrlen);
      setStatReturn (ret, status);
      break;

    case LIA64_listen:
      CHECK_FD (arg0);
      *status = listen (fdmap[arg0], arg1);
      setStatReturn (ret, status);
      break;

    case LIA64_accept:
      CHECK_FD (arg0);
      addrlen = 0;
      sockaddr = NULL;
      if (arg2)
	memBBRd (ADDPTR (arg2), (BYTE *) &addrlen, 4);
      if (addrlen > 0)
	{
	  ENSURE_SIZE (addrlen);
	  sockaddr = (struct sockaddr *) malargbuf;
	}
      *status = accept (fdmap[arg0], sockaddr, &addrlen);
      if ((int) *status != -1)
	{
	  if (arg1)
	    memBBWrt_alloc (ADDPTR (arg1), (BYTE *) sockaddr, addrlen);
	  if (arg2)
	    memBBWrt_alloc (ADDPTR (arg2), (BYTE *) &addrlen, 4);
	  *status = fdmap_alloc (*status);
	}
      setStatReturn (ret, status);
      break;

    case LIA64_getsockname:
      CHECK_FD (arg0);
      memBBRd (ADDPTR(arg2), (BYTE *) &addrlen, 4);
      ENSURE_SIZE (addrlen);
      *status = getsockname (fdmap[arg0],
			     (struct sockaddr *) malargbuf, &addrlen);
      if ((int) *status != -1)
	{
	  memBBWrt_alloc(ADDPTR(arg1), malargbuf, addrlen);
	  memBBWrt_alloc(ADDPTR(arg2), (BYTE *) &addrlen, 4);
	}
      setStatReturn (ret, status);
      break;

    case LIA64_getpeername:
      CHECK_FD (arg0);
      memBBRd (ADDPTR(arg2), (BYTE *) &addrlen, 4);
      ENSURE_SIZE (addrlen);
      *status = getpeername (fdmap[arg0],
			     (struct sockaddr *) malargbuf, &addrlen);
      if ((int) *status != -1)
	{
	  memBBWrt_alloc(ADDPTR(arg1), malargbuf, addrlen);
	  memBBWrt_alloc(ADDPTR(arg2), (BYTE *) &addrlen, 4);
	}
      setStatReturn (ret, status);
      break;

    case LIA64_socketpair:
      {
	int sv[2];
        int *svp = NULL;

	if (arg3)
	  svp = sv;
	*status = socketpair (arg0, arg1, arg2, svp);
	if ((int)*status > -1)
	  {
	    sv[0] = fdmap_alloc (sv[0]);
	    sv[1] = fdmap_alloc (sv[1]);
	    memBBWrt_alloc (ADDPTR(arg3), (BYTE *)svp, 2*sizeof(int));
	  }
      }
      setStatReturn (ret, status);
      break;

    case LIA64_send:
      {
	CHECK_FD (arg0);

        /* make sure we have enough space in malargbuf */
	ENSURE_SIZE (arg2);
	if (arg2 > 0)
	  memBBRd (ADDPTR(arg1), malargbuf, arg2);

        *status = send (fdmap[arg0], malargbuf, arg2,  arg3);
        setStatReturn (ret, status);
      }
      break;

    case LIA64_sendto:
      CHECK_FD (arg0);
      ENSURE_SIZE (arg2);
      if (arg2 > 0)
	memBBRd (ADDPTR (arg1), malargbuf, arg2);
      addrlen = (int) arg5;
      ENSURE_SIZE2 (addrlen);
      if (arg4 && addrlen > 0)
	memBBRd (ADDPTR (arg4), malargbuf2, addrlen);
      *status = sendto (fdmap[arg0], malargbuf, arg2,  arg3,
			(struct sockaddr *) malargbuf2, addrlen);
      setStatReturn (ret, status);
      break;

    case LIA64_recv:
      {
	CHECK_FD (arg0);

        /* make sure we have enough space in malargbuf */
	ENSURE_SIZE (arg2);
	*status = recv (fdmap[arg0], malargbuf, arg2, arg3);
	if ((int)*status > -1)
	  memBBWrt_alloc (ADDPTR (arg1), malargbuf, *status);
        setStatReturn (ret, status);
      }
      break;

    case LIA64_recvfrom:
      CHECK_FD (arg0);
      ENSURE_SIZE (arg2);	/* message buffer size */
      addrlen = 0;
      if (arg5)
	memBBRd (ADDPTR(arg5), (BYTE *) &addrlen, 4);
      ENSURE_SIZE2 (addrlen);
      *status = recvfrom (fdmap[arg0], malargbuf, arg2, arg3,
			  (struct sockaddr *) malargbuf2, &addrlen);
      if ((int) *status != -1)
	{
	  memBBWrt_alloc (ADDPTR (arg1), malargbuf, (int) *status);
	  if (arg4)
	    memBBWrt_alloc (ADDPTR (arg4), (BYTE *) malargbuf2, addrlen);
	  if (arg5)
	    memBBWrt_alloc (ADDPTR (arg5), (BYTE *) &addrlen, 4);
	}
      setStatReturn (ret, status);
      break;

    case LIA64_shutdown:
      CHECK_FD (arg0);
      *status = shutdown (fdmap[arg0], arg1);
      setStatReturn (ret, status);
      break;

    case LIA64_setsockopt:
      {
        /*
	 * We support all integer type paramter + struct linger
	 * (because it's explicitly using int. All other options I am
	 * aware of are single ints so we should be fine.
	 */
	CHECK_FD (arg0);

	if (arg4 != sizeof (int) && arg4 != sizeof (struct linger))
	  {
	    GrWrtx (8, -1 << 8, 0);	/* set exit status */
	    progExit("NR_setsockopt(%d): non-supported parameter *len=%d not "
		     "supported\n", (int)arg2, (int)arg4);
	  }
	ENSURE_SIZE(arg4);

	if (arg4 > 0)
	  memBBRd (ADDPTR (arg3), malargbuf, (unsigned)arg4);
	*status = setsockopt (fdmap[arg0], arg1, arg2, malargbuf, (int)arg4);
	setStatReturn (ret, status);
      }
      break;

    case LIA64_getsockopt:
      {
	socklen_t len;

	/*
	 * We support all integer type parameters + struct linger
	 * (because it's explicitly using int. All other options I am
	 * aware of are single ints so we should be fine.
	 */
	CHECK_FD (arg0);
	memBBRd (ADDPTR (arg4), (BYTE *) &len, 4);

	if (len != sizeof (int) && len != sizeof (struct linger))
	  {
	    GrWrtx (8, -1 << 8, 0);	/* set exit status */
	    progExit ("NR_getsockopt(%d): parameter *len=%d not supported\n",
		      (int)arg2, (int)len);
	  }
	ENSURE_SIZE(len);

	*status = getsockopt (fdmap[arg0], arg1, arg2, malargbuf, &len);
	if ((int)*status == 0)
	  {
	    memBBWrt_alloc (ADDPTR (arg3), (BYTE *)malargbuf, len);
	    memBBWrt_alloc (ADDPTR (arg4), (BYTE *)&len, 4);
	  }
	setStatReturn (ret, status);
      }
      break;

    case LIA64_sendmsg:
      {
	struct lia64_msghdr lia64_msghdr;
	struct lia64_iovec *lia64_iovec;
	struct msghdr msghdr;
	struct iovec iovec;
	size_t len, size;

	memBBRd (ADDPTR (arg1), (BYTE *) &lia64_msghdr, sizeof (lia64_msghdr));
	memset (&msghdr, 0, sizeof (msghdr));
	if (lia64_msghdr.msg_name && lia64_msghdr.msg_namelen > 0)
	  {
	    ENSURE_SIZE(lia64_msghdr.msg_namelen);
	    memBBRd (ADDPTR (lia64_msghdr.msg_name), (BYTE *) malargbuf,
		     lia64_msghdr.msg_namelen);
	    msghdr.msg_name = malargbuf;
	    msghdr.msg_namelen = lia64_msghdr.msg_namelen;
	  }

	size = lia64_msghdr.msg_iovlen * sizeof (struct lia64_iovec);
	msghdr.msg_iovlen = 0;
	if (size > 0)
	  {
	    /* translate iovec into a single contiguous buffer: */
	    lia64_iovec = alloca (size);
	    memBBRd (ADDPTR (lia64_msghdr.msg_iov),
		     (BYTE *) lia64_iovec, size);
	    for (size = 0, i = 0; i < lia64_msghdr.msg_iovlen; ++i)
	      size += lia64_iovec[i].iov_len;
	    ENSURE_SIZE2 (size);
	    bp = malargbuf2;
	    for (i = 0; i < lia64_msghdr.msg_iovlen; ++i)
	      {
		len = lia64_iovec[i].iov_len;
		if (len > 0)
		  memBBRd (ADDPTR (lia64_iovec[i].iov_base), bp, len);
		bp += len;
	      }
	    iovec.iov_base = malargbuf2;
	    iovec.iov_len = size;
	    msghdr.msg_iovlen = 1;
	    msghdr.msg_iov = &iovec;
	  }

	if (lia64_msghdr.msg_control && lia64_msghdr.msg_controllen > 0)
	  {
	    ENSURE_SIZE3 (lia64_msghdr.msg_controllen);
	    msghdr.msg_control = malargbuf3;
	    cmsg_lia642host (&lia64_msghdr, &msghdr);
	  }

	msghdr.msg_flags = lia64_msghdr.msg_flags;

	*status = sendmsg (fdmap[arg0], &msghdr, arg2);

	setStatReturn (ret, status);
      }
      break;

    case LIA64_recvmsg:
      {
	struct lia64_msghdr lia64_msghdr;
	struct lia64_iovec *lia64_iovec = 0;
	struct msghdr msghdr;
	struct iovec iovec;
	size_t len, size;

	memBBRd (ADDPTR (arg1), (BYTE *) &lia64_msghdr, sizeof (lia64_msghdr));
	memset (&msghdr, 0, sizeof (msghdr));
	if (lia64_msghdr.msg_name && lia64_msghdr.msg_namelen > 0)
	  {
	    ENSURE_SIZE(lia64_msghdr.msg_namelen);
	    msghdr.msg_name = malargbuf;
	    msghdr.msg_namelen = lia64_msghdr.msg_namelen;
	  }

	size = lia64_msghdr.msg_iovlen * sizeof (struct lia64_iovec);
	msghdr.msg_iovlen = 0;
	if (size > 0)
	  {
	    /* translate iovec into a single contiguous buffer: */
	    lia64_iovec = alloca (size);
	    memBBRd (ADDPTR (lia64_msghdr.msg_iov),
		     (BYTE *) lia64_iovec, size);
	    for (size = 0, i = 0; i < lia64_msghdr.msg_iovlen; ++i)
	      size += lia64_iovec[i].iov_len;
	    ENSURE_SIZE2 (size);
	    iovec.iov_base = malargbuf2;
	    iovec.iov_len = size;
	    msghdr.msg_iovlen = 1;
	    msghdr.msg_iov = &iovec;
	  }

	if (lia64_msghdr.msg_control && lia64_msghdr.msg_controllen > 0)
	  {
	    ENSURE_SIZE3 (lia64_msghdr.msg_controllen);
	    msghdr.msg_control = malargbuf3;
	  }

	msghdr.msg_flags = lia64_msghdr.msg_flags;

	*status = recvmsg (fdmap[arg0], &msghdr, arg2);

	if (*status != -1)
	  {
	    lia64_msghdr.msg_namelen = msghdr.msg_namelen;
	    lia64_msghdr.msg_flags = msghdr.msg_flags;;

	    /* copy peer address, if necessary: */

	    if (lia64_msghdr.msg_name && lia64_msghdr.msg_namelen > 0)
	      memBBWrt_alloc (ADDPTR (lia64_msghdr.msg_name),
			      (BYTE *) malargbuf, msghdr.msg_namelen);

	    /* copy message back into iovec: */

	    bp = malargbuf2;
	    nbytes = *status;
	    for (i = 0; nbytes > 0 && i < lia64_msghdr.msg_iovlen; ++i)
	      {
		len = lia64_iovec[i].iov_len;
		if (nbytes < len)
		  len = nbytes;
		if (len > 0)
		  memBBWrt_alloc (ADDPTR (lia64_iovec[i].iov_base), bp, len);
		nbytes -= len;
	      }

	    cmsg_host2lia64 (&lia64_msghdr, &msghdr);

	    /* copy back message header: */
	    memBBWrt_alloc (ADDPTR (arg1), (BYTE *) &lia64_msghdr,
			    sizeof (lia64_msghdr));
	  }
	setStatReturn (ret, status);
      }
      break;

    // long futex(unsigned int * uaddr, int op, unsigned int val,
    // 		  const struct timespec * utime, unsigned int * uaddr2,
    // 		  unsigned int val3);
    case LIA64_futex:
      // Have to mimic part of kernel's state machine?
      // Or can run it as is?
      *status = syscall (SYS_futex, arg0, arg1, arg2, arg3, arg4, arg5);
      setStatReturn (ret, status);
      break;

    case LIA64_pivot_root:
    case LIA64_mincore:
    case LIA64_madvise:
    default:
      {
        const char * sn = syscallName (num);
        sprintf ((char *)msg, "doSyscall: unimplemented system call %s(%u)\n", sn ? sn : "???", num);
        sysWrite (STDERR_FILENO, msg, strlen ((char *)msg));
      }
    }
 done:
  if (trace_syscalls)
    traceSyscallExit (*ret, *status);
}

static inline int
iskbdio (void)
{
  int r, nb = 0;

  /*
   * we read from console in 2 steps to avoid breaking too much code
   * (ioctl, then read)
   */
  r = ioctl (noConsole ? 0 : cfd, FIONREAD, &nb);
  if (r == -1)
    {
      GrWrtx (8, -1 << 8, 0);	/* set exit status */
      progExit("isbkbdio: ioctl=%d\n", errno);
    }
  return nb > 0 ? 1 : 0;
}

static void
writeConsole (char *buf, size_t len)
{
    ssize_t r;
    /*
     * always use raw mode so we get a clean output
     * 05/07/99 S.Eranian
     */
    r = write (noConsole ? 1 : cfd, buf, len);
    (void)r; /* TODO */
}

static void
allocMem (ADDR start, REG size)
{
  ADDR adr;

  for (adr = start; adr <= page_base(start + size - 1); adr += page_size)
    pmemLookup_p (adr);
}

static BOOL
addSscReq (unsigned fd, unsigned size)
{
  SscReqNode *np, *p;
  int prevcnt = 0;

  if (!(np = malloc (sizeof *np)))
    return NO;
  for (p = sscPend; p; p = p->next)
    prevcnt += p->irptcnt;
  np->fd      = fd;
  np->count   = size;
  np->irptcnt = IO_IRPT_CNT - prevcnt;
  np->next    = 0;
  if (!sscHead)
    sscHead = np;
  else
    {
      for (p = sscHead; p->next; p = p->next)
	;
      p->next = np;
    }
  if (!sscPend)
    sscPend = np;
  return YES;
}

static void
delSscReq (SscReqNode *p)
{
  SscReqNode *q = sscHead;

  if (p == sscHead)
    sscHead = sscHead->next;
  else
    {
      while (q->next != p)
	q = q->next;
      q->next = p->next;
    }
  if (p == sscPend)
    sscPend = sscPend->next;
  free (p);
}

static BOOL
completeXfer (unsigned fd, WORD *count)
{
  SscReqNode *p;

  for (p = sscPend; p; p = p->next)
    if (p->fd == fd)
      {
	*count = p->count;
	delSscReq (p);
	return YES;
      }
  return NO;
}

static BOOL
getXfer (unsigned *fd, WORD *count)
{
  SscReqNode *p = sscHead;

  if (p == sscPend)
    return NO;
  else
    {
      *fd = p->fd;
      *count = p->count;
      delSscReq (p);
      return YES;
    }
}

void
pendSscIrpt (unsigned type)
{
  unsigned i, reg, bit, vector;
  REG *p = &IRR0;		/* assume IRR0-3 are contiguous */

  for (i = 0; i < irptTop; i++)
    if (type == irptTbl[i].type)
      {
	vector = irptTbl[i].irrBit;
	if (vector == 1 || (vector >= 3 && vector <= 15))
	  continue;	/* ignore vectors 1 and 3 - 15 */

	reg = vector >> 6 & 0x3;
	bit = vector & 0x3F;
	*(p + reg) |= (REG)1 << bit;
      }

  if (acceptIrpt())
    intrsim = extint = YES;
}


#ifdef SKINET_ENABLE

static void
async_handler (int sig)
{
  /*signal (SIGIO, async_handler);*/

  /*
   * We check on both possible events just in case
   * they're both ready
   */
  if (iskbdio()) pendSscIrpt (KEYBOARD_IRPT);
  if (isnetio()) pendSscIrpt (NETWORK_IRPT);
}

#else

static void
asynckb (int sig)
{
  signal (SIGIO, asynckb);
  pendSscIrpt (KEYBOARD_IRPT);
}

#endif

void
doSSC (HWORD num, REG arg0, REG arg1, REG arg2, REG arg3, REG *ret)
{
  static struct timezone tzp1, tzp2;
  static struct timeval tp1, tp2;
#ifdef SKINET_ENABLE
  static unsigned char ether[ETH_ALEN];
  /* frame: try good alignment but does not really matter because of MemRRD */
  static unsigned long frame[512];
#endif
  SscTimezone sscTimezone;
  SscTimeval sscTimeval;
  SscDiskStat diskStat;
  SscDiskReq diskReq;
  struct tm *uxTimeP;
  unsigned diskXfer;
  SscTime sscTime;
  ssize_t nbytes;
  BYTE buf[100];
  unsigned i;
  BOOL olddt;
  ADDR adr;

  switch (num)
    {
    case SSC_STOP:
      progStop ("SSC breakpoint\n");
      break;

    case SSC_CTL_TRACE:
#ifdef TRACE_DAVIDM
      {
	extern int davidm_trace;

	davidm_trace = arg0;
      }
#endif
      break;

    case SSC_GET_ARGS:
      {
	extern char commandline[];
	extern unsigned commandline_len;

	olddt = PSR_DT;
	PSR_DT = 0;

	memBBWrt(arg0, (BYTE *)commandline, commandline_len);
	*ret = commandline_len;

	PSR_DT = olddt;
        break;
      }

    case SSC_OPEN:
      olddt = PSR_DT;
      PSR_DT = 0;
      memBBRd (arg0, buf, 0);
      PSR_DT = olddt;
      if (arg1 == SSC_READ_ACCESS)
	*ret = open ((char *)buf, O_RDONLY);
      else if (arg1 == SSC_WRITE_ACCESS)
	*ret = open ((char *)buf, O_WRONLY);
      else if (arg1 == (SSC_READ_ACCESS | SSC_WRITE_ACCESS))
	*ret = open ((char *)buf, O_RDWR);
      else
	progExit ("doSSC (SSC_OPEN): arg1 has an illegal access type\n");
      break;

    case SSC_READ:
      if ((*ret = lseek (arg0, arg3, SEEK_SET)) == -1)
	return;
      olddt = PSR_DT;
      PSR_DT = 0;
      adr = arg2;
      *ret = 0;
      for (i = 0, diskXfer = 0; i < arg1; i++)
	{
	  memBBRd (adr, (BYTE *)&diskReq, sizeof (SscDiskReq));
	  if ((int) diskReq.len > malargbufsize)
	    {
	      if (!(malargbuf = realloc (malargbuf, diskReq.len)))
		{
		  *ret = -1;
		  break;
		}
	      malargbufsize = diskReq.len;
	    }
	  nbytes = read (arg0, malargbuf, diskReq.len);
	  if (nbytes > 0)
	    {
	      diskXfer += nbytes;
	      allocMem (diskReq.addr, nbytes);
	      memBBWrt_opt (diskReq.addr, malargbuf, nbytes);
	    }
	  else if (!nbytes)
	    break;
	  else
	    {
	      *ret = -1;
	      break;
	    }
	  adr += sizeof (SscDiskReq);
	}
      if (!addSscReq (arg0, diskXfer))
	*ret = -1;
      PSR_DT = olddt;
#if 0
      pendSscIrpt(SIMSCSI_IRPT);
#endif
      break;

    case SSC_WRITE:
      if ((*ret = lseek (arg0, arg3, SEEK_SET)) == -1)
	return;
      olddt = PSR_DT;
      PSR_DT = 0;
      adr = arg2;
      for (i = 0, diskXfer = 0; i < arg1; i++)
	{
	  memBBRd (adr, (BYTE *)&diskReq, sizeof (SscDiskReq));
	  if ((int) diskReq.len > malargbufsize)
	    {
	      if (!(malargbuf = realloc (malargbuf, diskReq.len)))
		{
		  *ret = -1;
		  break;
		}
	      malargbufsize = diskReq.len;
	    }
	  if (diskReq.len > 0)
	    memBBRd (diskReq.addr, malargbuf, diskReq.len);
	  nbytes = write (arg0, malargbuf, diskReq.len);
	  if (nbytes > 0)
	    diskXfer += nbytes;
	  else if (!nbytes)
	    break;
	  else
	    {
	      *ret = -1;
	      break;
	    }
	  adr += sizeof (SscDiskReq);
	}
      if (!addSscReq (arg0, diskXfer))
	*ret = -1;
      PSR_DT = olddt;
#if 0
      pendSscIrpt(SIMSCSI_IRPT);
#endif
      break;

    case SSC_CLOSE:
      *ret = close (arg0);
      break;

    case SSC_WAIT_COMPLETION:
      olddt = PSR_DT;
      PSR_DT = 0;
      memBBRd (arg0, (BYTE *)&diskStat, sizeof (SscDiskStat));
      if (completeXfer (diskStat.fd, &diskStat.count))
	{
		memBBWrt(arg0, (BYTE *)&diskStat, sizeof (SscDiskStat));
	  *ret = 0;
	}
      else
	*ret = -1;
      PSR_DT = olddt;
      break;

    case SSC_CONNECT_INTERRUPT:
      {
	BOOL found = NO;

	for (i = 0; i < irptTop && !found; i++)
	  if (irptTbl[i].type == arg0)
	    {
	      irptTbl[i].irrBit = arg1;
	      found = YES;
	    }
	if (!found)
	  {
	    irptTbl[irptTop].type   = arg0;
	    irptTbl[irptTop].irrBit = arg1;
	    irptTop++;
	  }
	if (arg0 == KEYBOARD_IRPT)
	  {
	    /* enable async keyboard input */
#if defined SKINET_ENABLE
	    signal (SIGIO, async_handler);
#else /* !defined SKINET_ENABLE */
        /* FIXME_laurentm: is this correct? */
	    signal (SIGIO, asynckb);
#endif /* !defined SKINET_ENABLE */
	    /*
	     * S.Eranian 01/20/1999
	     *
	     * Fixed the missing F_SETOWN to actually enable
	     * the SIGIO delivery on the resource.
	     */
	    if (noConsole) {
	      fcntl (0, F_SETOWN, getpid());
	      fcntl (0, F_SETFL, fcntl(0,F_GETFL,0)|O_ASYNC|O_NONBLOCK);
	    } else {
	      fcntl (cfd, F_SETOWN, getpid());
	      fcntl (cfd, F_SETFL, fcntl(cfd,F_GETFL,0)|O_ASYNC|O_NONBLOCK);
	    }
	  }
	/* no return value */
	break;
      }

    case SSC_GENERATE_INTERRUPT:
      pendSscIrpt (arg0);
      /* no return value */
      break;

    case SSC_GET_COMPLETION:
      olddt = PSR_DT;
      PSR_DT = 0;
      if (getXfer (&diskStat.fd, &diskStat.count))
	{
		memBBWrt (arg0, (BYTE *)&diskStat, sizeof (SscDiskStat));
	  *ret = 0;
	}
      else
	*ret = -1;
      PSR_DT = olddt;
      break;

    case SSC_GET_RTC:
      olddt = PSR_DT;
      PSR_DT = 0;
      gettimeofday (&tp1, &tzp1);

      uxTimeP = localtime ((const time_t *)&tp1.tv_sec);
      sscTime.year = uxTimeP->tm_year;
      sscTime.mon  = uxTimeP->tm_mon;
      sscTime.mday = uxTimeP->tm_mday;
      sscTime.hour = uxTimeP->tm_hour;
      sscTime.min  = uxTimeP->tm_min;
      sscTime.sec  = uxTimeP->tm_sec;
      sscTime.msec = tp1.tv_usec/1000;
      sscTime.wday = uxTimeP->tm_wday;
      olddt = PSR_DT;
      PSR_DT = 0;
      memBBWrt (arg0, (BYTE *)&sscTime, sizeof (SscTime));
      PSR_DT = olddt;
      /* no return value */
      break;

    case SSC_GET_TOD:
      olddt = PSR_DT;
      PSR_DT = 0;

      gettimeofday (&tp2, &tzp2);
      sscTimeval.ssc_Sec  = tp2.tv_sec;
      sscTimeval.ssc_Usec = tp2.tv_usec;
      sscTimezone.ssc_Minuteswest = tzp2.tz_minuteswest;
      sscTimezone.ssc_Dsttime     = tzp2.tz_dsttime;
      memBBWrt (arg0, (BYTE *)&sscTimeval, sizeof (SscTimeval));
      if ( arg1 ) memBBWrt (arg1, (BYTE *)&sscTimezone,
                            sizeof (SscTimezone));
      PSR_DT = olddt;
      /* no return value */
      break;

    case SSC_LOAD_SYMBOLS:
      olddt = PSR_DT;
      PSR_DT = 0;
      memBBRd (arg1, buf, 0);
      /* arg0, arg2, and arg3 are currently not used */
      if (elfSymLoad((char *)buf))
	*ret = 0;
      else
	*ret = -1;
      PSR_DT = olddt;
      break;

    case SSC_SET_PERIODIC_INTERRUPT:
      if (arg0 == CLK_TIMER_IRPT)
	clkTmr = clkTmrSve = arg1;
      else if (arg0 == PROF_TIMER_IRPT)
	profTmr = profTmrSve = arg1;
      else
	progExit("SSC_SET_PERIODIC: arg0 has an illegal interrupt type\n");
      /* no return value */
      break;

    case SSC_CONSOLE_INIT:
      if (noConsole)
	{
	  struct termios ttyIos;

	  /*
	   * this code is just to make sure that we get the same behavior
	   * from the real tty as we would with the xterm
	   * 05/07/99 S.Eranian 
	   */
      	   tcgetattr (0,  &ttyIos);
	   sane_ttyIos = ttyIos;

	   atexit (restore_tty);

	   /* termios makes it easy to go raw... */
	   cfmakeraw (&ttyIos);

	   ttyIos.c_cc[VMIN] = 0;
	   ttyIos.c_cc[VTIME] = 0;

	   tcsetattr (0, TCSANOW, &ttyIos);
	}
      else
	*ret = cfd = initConsole (1);
      break;

    case SSC_EXIT:
      GrWrtx (8, arg0, 0);
#ifdef SKINET_ENABLE
      netdev_cleanup();
#endif
      progExit ("kernel exited with status %d\n", (int)arg0);
      break;

    case SSC_PUTCHAR:
      buf[0] = arg0;
      buf[1] = '\0';
      writeConsole ((char *)buf, 1);
      /* no return value */
      break;

    case SSC_GETCHAR:
      {
	char c;
	readConsole (&c);

	*ret = c;
	GrWrtx (8, c, 0);
      }
      break;

#ifdef SKINET_ENABLE

    case SSC_NETDEV_PROBE:
      { 
	olddt = PSR_DT;
	PSR_DT = 0;
	memBBRd (arg0, buf, 0);

	/* buf holds the device name to open on the host */
        *ret = (long)netdev_open ((char *)buf, ether);

	/* copy back the link level addr */
		memBBWrt (arg1, (BYTE *)ether, sizeof (ether));

	PSR_DT = olddt;
      }
      break;

    case SSC_NETDEV_SEND:
      {
	extern long netdev_send (int, char *, int);
	olddt = PSR_DT;
        PSR_DT = 0;
	if (arg2 > 0)
	  memBBRd (arg1, (BYTE *)frame, arg2);
        PSR_DT = olddt;
	*ret = netdev_send (arg0, (char *)frame, arg2);
      }
      break;

    case SSC_NETDEV_RECV:
      {
	extern long netdev_recv (int, char *, int);

	*ret = netdev_recv (arg0, (char *)frame, arg2);
	olddt = PSR_DT;
        PSR_DT = 0;
        memBBWrt (arg1, (BYTE *)frame, *ret);
        PSR_DT = olddt;
      }
      break;

    case SSC_NETDEV_ATTACH:
      *ret = netdev_attach ((int)arg0, (unsigned int)arg1);
      signal (SIGIO, async_handler);
      break;

    case SSC_NETDEV_DETACH:
      *ret = netdev_detach ((int)arg0);
      break;
#endif /* SKINET_ENABLE */
    }
}

REG
sys_get_cycle_count (void)
{
  if (!unixABI)
    return ITC;
  else
    {
#     define CYCLES_PER_SEC	700
      struct timeval now;

      gettimeofday (&now, 0);

      return (1000000 * (REG) now.tv_sec + now.tv_usec) * CYCLES_PER_SEC;
    }
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

