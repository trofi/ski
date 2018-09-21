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
#ifndef _SKI_SYSCALL_H
#define _SKI_SYSCALL_H

#define	STAT64_USE_TIMESPEC	1

typedef long long	long64 __attribute__((aligned(8)));
typedef long64		time64;

struct timeval64 {
	long64	tv_sec;
	long64	tv_usec;
};

struct itimerval64 {
	struct timeval64 it_interval;
	struct timeval64 it_value;
};

struct rusage64 {
	struct timeval64 ru_utime;
	struct timeval64 ru_stime;
	long64	ru_maxrss;
	long64	ru_ixrss;
	long64	ru_idrss;
	long64	ru_isrss;
	long64	ru_minflt;
	long64	ru_majflt;
	long64	ru_nswap;
	long64	ru_inblock;
	long64	ru_oublock;
	long64	ru_msgsnd;
	long64	ru_msgrcv;
	long64	ru_nsignals;
	long64	ru_nvcsw;
	long64	ru_nivcsw;
};

struct timespec64 {
	time64	tv_sec;
	long64	tv_nsec;
};

struct stat64 {
	dev_t	st_dev;
	ino_t	st_ino;
	mode_t	st_mode;
	nlink_t	st_nlink;
	uid_t	st_uid;
	gid_t	st_gid;
	dev_t	st_rdev;
#if STAT64_USE_TIMESPEC
	struct timespec64 st_atimespec;
	struct timespec64 st_mtimespec;
	struct timespec64 st_ctimespec;
#else
	time64	st_atimesec;
	long64	st_atimensec;
	time64	st_mtimesec;
	long64	st_mtimensec;
	time64	st_ctimesec;
	long64	st_ctimensec;
#endif
	off_t	st_size;
	int64_t	st_blocks;
	uint32_t st_blksize;
	uint32_t st_flags;
	uint32_t st_gen;
};

struct statfs64 {
	long64	f_bsize;
	long64	f_iosize;
	long64	f_blocks;
	long64	f_bfree;
	long64	f_bavail;
	long64	f_files;
	long64	f_ffree;
	fsid_t	f_fsid;
	uid_t	f_owner;
	int	f_type;
	int	f_flags;
	long64	f_syncwrites;
	long64	f_asyncwrites;
	char	f_fstypename[MFSNAMELEN];
	char	f_mntonname[MNAMELEN];
	long64	f_syncreads;
	long64	f_asyncreads;
	char	f_mntfromname[MNAMELEN];
};

void ntos_itimerval(const struct itimerval *, struct itimerval64 *);
void ntos_rusage(const struct rusage *, struct rusage64 *);
void ntos_stat(const struct stat *, struct stat64 *);
void ntos_statfs(const struct statfs *, struct statfs64 *);
void ntos_timespec(const struct timespec *, struct timespec64 *);
void ntos_timeval(const struct timeval *, struct timeval64 *);

void ston_itimerval(const struct itimerval64 *, struct itimerval *);
void ston_timeval(const struct timeval64 *, struct timeval *);

#endif	/* _SKI_SYSCALL_H */
