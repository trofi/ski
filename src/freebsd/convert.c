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
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mount.h>
#include <sys/resource.h>
#include <string.h>
#include "freebsd/syscall.h"

void ntos_itimerval(const struct itimerval *it, struct itimerval64 *it64)
{
	ntos_timeval(&it->it_interval, &it64->it_interval);
	ntos_timeval(&it->it_value, &it64->it_value);
}

void ntos_rusage(const struct rusage *ru, struct rusage64 *ru64)
{
	ntos_timeval(&ru->ru_utime, &ru64->ru_utime);
	ntos_timeval(&ru->ru_stime, &ru64->ru_stime);
	ru64->ru_maxrss = ru->ru_maxrss;
	ru64->ru_ixrss = ru->ru_ixrss;
	ru64->ru_idrss = ru->ru_idrss;
	ru64->ru_isrss = ru->ru_isrss;
	ru64->ru_minflt = ru->ru_minflt;
	ru64->ru_majflt = ru->ru_majflt;
	ru64->ru_nswap = ru->ru_nswap;
	ru64->ru_inblock = ru->ru_inblock;
	ru64->ru_oublock = ru->ru_oublock;
	ru64->ru_msgsnd = ru->ru_msgsnd;
	ru64->ru_msgrcv = ru->ru_msgrcv;
	ru64->ru_nsignals = ru->ru_nsignals;
	ru64->ru_nvcsw = ru->ru_nvcsw;
	ru64->ru_nivcsw = ru->ru_nivcsw;
}

void ntos_stat(const struct stat *st, struct stat64 *st64)
{
	st64->st_dev = st->st_dev;
	st64->st_ino = st->st_ino;
	st64->st_mode = st->st_mode;
	st64->st_nlink = st->st_nlink;
	st64->st_uid = st->st_uid;
	st64->st_gid = st->st_gid;
	st64->st_rdev = st->st_rdev;
#if STAT64_USE_TIMESPEC
	ntos_timespec(&st->st_atimespec, &st64->st_atimespec);
	ntos_timespec(&st->st_mtimespec, &st64->st_mtimespec);
	ntos_timespec(&st->st_ctimespec, &st64->st_ctimespec);
#else
	st64->st_atimesec = st->st_atime;
	st64->st_atimensec = 0;
	st64->st_mtimesec = st->st_mtime;
	st64->st_mtimensec = 0;
	st64->st_ctimesec = st->st_ctime;
	st64->st_ctimensec = 0;
#endif
	st64->st_size = st->st_size;
	st64->st_blocks = st->st_blocks;
	st64->st_blksize = st->st_blksize;
	st64->st_flags = st->st_flags;
	st64->st_gen = st->st_gen;
}

void ntos_statfs(const struct statfs *f, struct statfs64 *f64)
{
	f64->f_bsize = f->f_bsize;
	f64->f_iosize = f->f_iosize;
	f64->f_blocks = f->f_blocks;
	f64->f_bfree = f->f_bfree;
	f64->f_bavail = f->f_bavail;
	f64->f_files = f->f_files;
	f64->f_ffree = f->f_ffree;
	f64->f_fsid = f->f_fsid;
	f64->f_owner = f->f_owner;
	f64->f_type = f->f_type;
	f64->f_flags = f->f_flags;
	f64->f_syncwrites = f->f_syncwrites;
	f64->f_asyncwrites = f->f_asyncwrites;
	bcopy(f->f_fstypename, f64->f_fstypename, MFSNAMELEN);
	bcopy(f->f_mntonname, f64->f_mntonname, MNAMELEN);
	f64->f_syncreads = f->f_syncreads;
	f64->f_asyncreads = f->f_asyncreads;
	bcopy(f->f_mntfromname, f64->f_mntfromname, MNAMELEN);
}

void ntos_timespec(const struct timespec *tv, struct timespec64 *tv64)
{
	tv64->tv_sec = tv->tv_sec;
	tv64->tv_nsec = tv->tv_nsec;
}

void ntos_timeval(const struct timeval *tv, struct timeval64 *tv64)
{
	tv64->tv_sec = tv->tv_sec;
	tv64->tv_usec = tv->tv_usec;
}

void ston_itimerval(const struct itimerval64 *it64, struct itimerval *it)
{
	ston_timeval(&it64->it_interval, &it->it_interval);
	ston_timeval(&it64->it_value, &it->it_value);
}

void ston_timeval(const struct timeval64 *tv64, struct timeval *tv)
{
	tv->tv_sec = tv64->tv_sec;
	tv->tv_usec = tv64->tv_usec;
}
