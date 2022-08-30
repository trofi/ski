/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator DOS Emulation Routines
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
#include <sys/ioctl.h>
#if !defined(__FreeBSD__)
#include <syscall.h>
#endif
#include <sys/times.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#if defined(__FreeBSD__)
#include <termios.h>
#define termio  termios
#define TCGETA  TIOCGETA
#define TCSETA  TIOCSETA
#else
#include <termio.h>
#endif
#include <time.h>
#if defined __linux__
# include <sys/time.h>
#else /* !defined __linux */
extern int localtime_r(const __time_t *, __struct_tm *);
# include <ulimit.h>
#endif /* !defined __linux */
#include <unistd.h>	/* Also #includes <utime.h> */
#include <sys/utsname.h>
#include <ctype.h>

#include "dos.h"

#include "std.h"
#include "bits.h"
#include "types.h"
#undef B0		/* from sys/???.h */
#include "fields.h"
#include "state.h"
#include "sim.h"	/* just for Status & progExit */
#include "simmem.h"
#include "ia_types.h"
#include "ia_state.h"
#include "ia_exec.h"
#include "ui.h"

extern ArithFlags arithFlags;
IAWORD pspSeg;

/* from Appendix C of Microsoft MS-DOS Programmer's Reference */
#define ERROR_INVALID_FUNCTION		0x01
#define ERROR_FILE_NOT_FOUND		0x02
#define ERROR_PATH_NOT_FOUND		0x03
#define ERROR_TOO_MANY_OPEN_FILES	0x04
#define ERROR_ACCESS_DENIED		0x05
#define ERROR_INVALID_HANDLE		0x06
#define ERROR_ARENA_TRASHED		0x07
#define ERROR_NOT_ENOUGH_MEMORY		0x08
#define ERROR_INVALID_BLOCK		0x09
#define ERROR_BAD_ENVIRONMENT		0x0A
#define ERROR_BAD_FORMAT		0x0B
#define ERROR_INVALID_ACCESS		0x0C
/* XXX - finish these */

#define OPEN_ACCESS_MASK		0x03
#define OPEN_ACCESS_READONLY		0x00
#define OPEN_ACCESS_WRITEONLY		0x01
#define OPEN_ACCESS_READWRITE		0x02

#define ATTR_NORMAL			0x00
#define ATTR_READONLY			0x01

#define INITMALLOC	2048	/* size of initial malloc for malargbuf */

/* XXX - should fhmap be shared with fdmap[] in syscall.c? */
#define MAX_FHMAP	100
static int fhmap[MAX_FHMAP] = {0, 1, 2};

Status dosInt21(BYTE func, BYTE subFunc)
{
    /* XXX - copied from syscall.c - many of these aren't needed here */
    char buf[65536], *p;
    DWORD val;
    int oflag, status;
    mode_t mode;
    ssize_t ret;
    size_t nbytes;
    struct timeval now;
    struct timezone zone;
    struct tm locTime;
    ADDR adr;
    static char *malargbuf;
    static unsigned malargbufsize = 0;
    ssize_t r;

    /* initialize malargbuf ptr & fhmap if necessary */
    if (malargbufsize == 0) {
	int i;

	for (i = 3; i < 100; i++)
	    fhmap[i] = i;
	if (!(malargbuf = malloc(INITMALLOC)))
	    return StSuccess;		/* XXX - report error? */
	malargbufsize = INITMALLOC;
    }

    switch (func) {
    case 0x00:				/* terminate program */
	progExit("IA-32 program terminated\n");
	break;
    case 0x02:				/* display character */
	buf[0] = DL;
	r = write(fhmap[1], buf, 1);
	(void)r;
	break;
    case 0x08:				/* read keyboard without echo */
	if (isatty(fhmap[0])) {
	    struct termio origTermio, newTermio;

	    (void)ioctl(fhmap[0], TCGETA, &origTermio);
	    newTermio = origTermio;
	    newTermio.c_iflag &= ~ICRNL;
	    newTermio.c_lflag &= ~(ECHO | ICANON);
	    newTermio.c_cc[VMIN] = 1;
	    newTermio.c_cc[VTIME] = 0;
	    (void)ioctl(fhmap[0], TCSETA, &newTermio);
	    if (read(fhmap[0], buf, 1) == 1)
		AL = buf[0];
	    (void)ioctl(fhmap[0], TCSETA, &origTermio);
	} else {
	    if (read(fhmap[0], buf, 1) == 1)
		AL = buf[0];
	}
	break;
    case 0x09:				/* display string */
	/* XXX - do we need segment limit checks, etc. ??? */
	adr = DSD.base + DX;
	p = buf;
	do {
	    (void)memMRd(adr++, 1, &val);
	    *p++ = val;
	} while (*(p-1) != '$' && p < buf + sizeof buf);
	nbytes = p - buf - 1;
	(void)sysWrite(fhmap[1], buf, nbytes);
	break;
    case 0x2a:				/* get date */
	(void)gettimeofday(&now, &zone);
	(void)localtime_r(&now.tv_sec, &locTime);
	AL = locTime.tm_wday;
	CX = locTime.tm_year + 1900;
	DH = locTime.tm_mon + 1;
	DL = locTime.tm_mday;
	break;
    case 0x2c:				/* get time */
	(void)gettimeofday(&now, &zone);
	(void)localtime_r(&now.tv_sec, &locTime);
	CH = locTime.tm_hour;
	CL = locTime.tm_min;
	DH = locTime.tm_sec;
	DL = now.tv_usec / 10000;
	break;
    case 0x30:				/* get version number */
	AX = 0x0006;	/* DOS 6.0 */
	BX = 0;
	CX = 0;
	break;
    case 0x3c:				/* create file with handle */
	adr = DSD.base + DX;
	memBBRd(adr, (BYTE *)buf, 0);
	for (p = buf; *p; p++)
	    *p = (char)tolower((int)*p);
	/* we ignore all attributes except ATTR_READONLY */
	if (CX & ATTR_READONLY)
	    mode = S_IRUSR;
	else
	    mode = S_IRUSR | S_IWUSR;
	if ((status = creat(buf, mode)) == -1) {
	    arithFlags.cf = 1;
	    switch (errno) {
	    case ENOENT:
	    case ENOTDIR:
		AX = ERROR_PATH_NOT_FOUND;
		break;
	    case EMFILE:
	    case ENFILE:
		AX = ERROR_TOO_MANY_OPEN_FILES;
		break;
	    default:
		AX = ERROR_ACCESS_DENIED;
		break;
	    }
	} else {
	    arithFlags.cf = 0;
	    fhmap[status] = status;
	    AX = status;
	}
	break;
    case 0x3d:				/* open file with handle */
	adr = DSD.base + DX;
	memBBRd(adr, (BYTE *)buf, 0);
	for (p = buf; *p; p++)
	    *p = (char)tolower((int)*p);
	switch ((BYTE)AL & OPEN_ACCESS_MASK) {
	case OPEN_ACCESS_READONLY:
	    oflag = O_RDONLY;
	    break;
	case OPEN_ACCESS_WRITEONLY:
	    oflag = O_WRONLY;
	    break;
	case OPEN_ACCESS_READWRITE:
	    oflag = O_RDWR;
	    break;
	default:
	    arithFlags.cf = 1;
	    AX = ERROR_INVALID_ACCESS;
	    return StSuccess;
	}
	/* we ignore the OPEN_SHARE_* & OPEN_FLAGS_NOINHERIT flags */
	if ((status = open(buf, oflag)) == -1) {
	    arithFlags.cf = 1;
	    switch (errno) {
	    case ENOENT:
		AX = ERROR_FILE_NOT_FOUND;
		break;
	    case ENOTDIR:
		AX = ERROR_PATH_NOT_FOUND;
		break;
	    case EMFILE:
	    case ENFILE:
		AX = ERROR_TOO_MANY_OPEN_FILES;
		break;
	    case EACCES:
	    case EISDIR:
		AX = ERROR_ACCESS_DENIED;
		break;
	    default:
		AX = ERROR_INVALID_ACCESS;	/* XXX - right error ? */
		break;
	    }
	} else {
	    arithFlags.cf = 0;
	    fhmap[status] = status;
	    AX = status;
	}
	break;
    case 0x3e:				/* close file with handle */
	if (BX <= 2)		/* don't close stdin, stdout, stderr */
	    arithFlags.cf = 0;
	else if (BX > MAX_FHMAP || close(fhmap[BX]) == -1) {
	    arithFlags.cf = 1;
	    AX = ERROR_INVALID_HANDLE;
	} else
	    arithFlags.cf = 0;
	break;
    case 0x3f:				/* read file or device */
	/* make sure we have enough space in malargbuf */
	if (CX > malargbufsize) {
	    if (!(malargbuf = realloc(malargbuf, CX)))
		return StSuccess;	/* XXX - report error? */
	    malargbufsize = CX;
	}
	if (BX > MAX_FHMAP) {
	    arithFlags.cf = 1;
	    AX = ERROR_INVALID_HANDLE;
	} else if ((ret = read(fhmap[BX], malargbuf, CX)) == -1) {
	    arithFlags.cf = 1;
	    if (errno == EBADF)
		AX = ERROR_INVALID_HANDLE;
	    else
		AX = ERROR_ACCESS_DENIED;
	} else {	/* read successful  */
	    arithFlags.cf = 0;
	    AX = ret;
	    /* XXX - do we need segment limit checks, etc. ??? */
	    adr = DSD.base + DX;
	    memBBWrt(adr, (BYTE *)malargbuf, ret);
	}
	break;
    case 0x40:				/* write file or device */
	/* make sure we have enough space in malargbuf */
	if (CX > malargbufsize) {
	    if (!(malargbuf = realloc(malargbuf, CX)))
		return StSuccess;	/* XXX - report error? */
	    malargbufsize = CX;
	}
	/* XXX - do we need segment limit checks, etc. ??? */
	adr = DSD.base + DX;
	memBBRd(adr, (BYTE *)malargbuf, CX);
	if (BX > MAX_FHMAP) {
	    arithFlags.cf = 1;
	    AX = ERROR_INVALID_HANDLE;
	} else if ((ret = sysWrite(fhmap[BX], malargbuf, CX)) == -1) {
	    arithFlags.cf = 1;
	    if (errno == EBADF)
		AX = ERROR_INVALID_HANDLE;
	    else
		AX = ERROR_ACCESS_DENIED;
	} else {	/* write successful */
	    arithFlags.cf = 0;
	    AX = ret;
	}
	break;
    case 0x44:				/* device status/control */
	switch (subFunc) {
	case 0x0:			/* get device data */
	    /* XXX - minimal support to get stdlib working */
	    if (BX > MAX_FHMAP) {
		arithFlags.cf = 1;
		AX = ERROR_INVALID_HANDLE;
	    } else if (isatty(fhmap[BX])) {
		arithFlags.cf = 0;
		DX = 0xc3;
	    } else {
		arithFlags.cf = 0;
		DX = 0x02;
	    }
	    break;
	default:
	    progExit("unsupported DOS int 21 function %02x%02x\n",
		     (int)func, (int)subFunc);
	    break;
	}
	break;
    case 0x4c:				/* end program */
	progExit("IA-32 program terminated with status %d\n", (int)AL);
	break;
    case 0x51:				/* get PSP address */
    case 0x62:
	BX = pspSeg;
	break;
    default:
	progExit("unsupported DOS int 21 function %02x\n", (int)func);
	break;
    }
    return StSuccess;
}
