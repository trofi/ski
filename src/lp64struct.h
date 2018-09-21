/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Datatypes For LP64 System Calls
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
#ifndef _SKI_LP64STRUCT_H
#define _SKI_LP64STRUCT_H

#define _SPARE4_SIZE 3

/*
 * The following are LP64 typedef and struct definitions as of Feb/96
 * for selected structs needed for LP64 system call implementation.
 */

#if 0
/* The following typedefs for LP64 are equivalent to ILP32's */

typedef uint16_t	__cnode_t;	/* see stat.h */
typedef uint16_t	__site_t;	/* see stat.h */
typedef uint16_t	mode_t;		/* For file types and modes */
typedef uint16_t	nlink_t;	/* For link counts */

typedef int32_t		daddr_t;
typedef int32_t		dev_t;		/* For device numbers */
typedef int32_t		gid_t;		/* For group IDs */
typedef int32_t		pid_t;		/* For process and session IDs */
typedef int32_t		time_t;		/* For times in seconds */
typedef int32_t		uid_t;		/* For user IDs */

typedef uint32_t	clock_t;	/* For clock ticks */

#endif

typedef int64_t		ad_long_t;	/* For block size */
#ifdef HPUX1020	/* XXX - needed on earlier OSes? */
typedef uint64_t	ino64_t;	/* Just API goes wide for now... */
#endif

typedef int64_t	time64_t;


/*
 * File record locking structure used by fcntl()
 */
struct flock64 {
    short	l_type;		/* F_RDLCK, F_WRLCK, or F_UNLCK */
    short	l_whence;	/* SEEK_SET, SEEK_CUR, SEEK_END */
    off64_t	l_start;	/* starting offset relative to l_whence */
    off64_t	l_len;		/* len == 0 means "until end of file" */
    pid_t	l_pid;		/* Process ID of the process holding the
				   lock, returned with F_GETLK */
};


/*
 * The stat structure filled in by stat(), lstat(), and fstat()
 */
/* For LP64 applications */
struct stat64 {
    dev_t	st_dev;
    ino64_t	st_ino;
    mode_t	st_mode;
    nlink_t	st_nlink;
    unsigned short	st_reserved1;	/* old st_uid,replaced spare positions*/
    unsigned short	st_reserved2;	/* old st_gid,replaced spare positions*/
    dev_t	st_rdev;
    off64_t	st_size;
    time64_t	st_atime;
    int		st_spare1;
    time64_t	st_mtime;
    int		st_spare2;
    time64_t	st_ctime;
    int		st_spare3;
    ad_long_t	st_blksize;
    blkcnt_t	st_blocks;
    unsigned int	st_pad:30;
    unsigned int	st_acl:1;	/* set if there are optional */
					/* ACL entries */
    unsigned int	st_remote:1;	/* Set if file is remote */
    dev_t	st_netdev;	/* ID of device containing */
				/* network special file */
    ino64_t	st_netino;	/* Inode number of network special file */
    __cnode_t	st_cnode;
    __cnode_t	st_rcnode;
    __site_t	st_netsite;	/* The site where the network device lives */
    short	st_fstype;
    dev_t	st_realdev;	/* Real device number of device containing */
				/* the inode for this file */
    /* Steal three spare for the device site number */
    unsigned short	st_basemode;
    unsigned short	st_spareshort;
    uid_t	st_uid;
    gid_t	st_gid;
    int32_t	st_spare4[_SPARE4_SIZE];
};

/* For ILP32 applications with _FILE_OFFSET_BITS=64 */
struct stat64x {
    dev_t	st_dev;
    ino_t	st_ino;
    mode_t	st_mode;
    nlink_t	st_nlink;
    unsigned short	st_reserved1;	/* old st_uid,replaced spare positions*/
    unsigned short	st_reserved2;	/* old st_gid,replaced spare positions*/
    dev_t	st_rdev;
    off64_t	st_size;
    time_t	st_atime;
    int		st_spare1;
    time_t	st_mtime;
    int		st_spare2;
    time_t	st_ctime;
    int		st_spare3;
    long	st_blksize;
    blkcnt_t	st_blocks;
    unsigned int	st_pad:30;
    unsigned int	st_acl:1;	/* set if there are optional */
					/* ACL entries */
    unsigned int	st_remote:1;	/* Set if file is remote */
    dev_t	st_netdev;	/* ID of device containing */
				/* network special file */
    ino_t	st_netino;	/* Inode number of network special file */
    __cnode_t	st_cnode;
    __cnode_t	st_rcnode;
    __site_t	st_netsite;	/* The site where the network device lives */
    short	st_fstype;
    dev_t	st_realdev;	/* Real device number of device containing */
				/* the inode for this file */
    /* Steal three spare for the device site number */
    unsigned short	st_basemode;
    unsigned short	st_spareshort;
    uid_t	st_uid;
    gid_t	st_gid;
    int32_t	st_spare4[_SPARE4_SIZE];
};


/*
 * Structure used by ustat()
 * bh: "Our subset doesn't have this file, but sys/types.h lists daddr_t as
 *     being an int32_t and ino_t as a uint64_t."
 */
struct ustat64 {
    daddr_t	f_tfree;	/* total free */
    ino64_t	f_tinode;	/* total inodes free */
    char	f_fname[6];	/* filsys name */
    char	f_fpack[6];	/* filsys pack name */
    int		f_blksize;	/* filsys block size */
};

struct timeval64 {
    int64_t	tv_sec;
    int64_t	tv_usec;
};

struct rlimit64 {
    int64_t	rlim_cur;
    int64_t	rlim_max;
};

#endif	/* _SKI_LP64STRUCT_H */
