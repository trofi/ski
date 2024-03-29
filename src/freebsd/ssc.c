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

#include <sys/param.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <assert.h>
#include <fcntl.h>
#include <libutil.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/* XXX - B0 is defined in termios.h to mean 0 baud. B0 is also
 *	 a macro defined in field.h. Undefine B0 here...
 */
#undef B0

#include "std.h"
#include "types.h"
#include "bits.h"
#include "fields.h"
#include "load.h"
#include "sim.h"
#include "state.h"
#include "simmem.h"
#include "ssc.h"
#include "ui.h"

#include "interruption.h"

#include "freebsd/machdep.h"

extern char commandline[];
extern unsigned int commandline_len;

#ifdef NEW_MP
extern REG os_boot_rendez_ip;
extern REG os_boot_rendez_gp;
#endif

static SscReqNode *sscHead = NULL;
SscReqNode *sscPend = NULL;

static struct {
	short	type;
	short	irrBit;
} irptTbl[30];
static unsigned int irptTop = 0;

static pid_t console;
static int fdin, fdout;
static struct termios saved_tios;

static void sigchld_handler(int sig, siginfo_t *si, void *ctx)
{
	tcsetattr(fdin, TCSANOW, &saved_tios);
	_exit(1);
}

static void restore_tios(void)
{
	int status;

	tcsetattr(fdin, TCSANOW, &saved_tios);
	if (console) {
		kill(console, SIGKILL);
		wait(&status);
	}
}

static void init_console(int fin, int fout)
{
	struct termios tios;

	tcgetattr(fin, &tios);
	saved_tios = tios;
	tios.c_lflag &= ~(ICANON|ECHO);
	tcsetattr(fin, TCSANOW, &tios);
	atexit(restore_tios);
	fdin = fin;
	fdout = fout;
}

static int xconsole()
{
	char name[PATH_MAX];
	struct sigaction act;
	char *tty;
	int namelen;
	int master, slave;

	if (openpty(&master, &slave, name, NULL, NULL) == -1)
		goto fail;

	console = fork();
	switch (console) {
	case -1:
		goto fail;
	case 0: /* child */
		close(slave);
		close(0);
		close(1);
		close(2);
		namelen = strlen(name);
		tty = name + namelen + 1;
		sprintf(tty, "-S%s%d", name + namelen - 2, master);
		execlp("xterm", "xterm", tty, "-geo", "80x24", "-name",
		    "xski console", 0);
		abort();
	default: /* parent */
		close(master);
		init_console(slave, slave);
		act.sa_sigaction = sigchld_handler;
		act.sa_flags = SA_NOCLDSTOP|SA_SIGINFO;
		sigaction(SIGCHLD, &act, NULL);

		/* Read the window ID. */
		if (read(slave, name, 128) == -1)
			goto fail;

		fcntl(slave, F_SETFL, O_NONBLOCK);
		break;
	}

	return 0;

 fail:
	progExit("Unable to create console window\n");
	return -1;
}

static BOOL addSscReq(int fd, size_t size)
{
	SscReqNode *np, *p;
	int prevcnt = 0;

	if (!(np = malloc(sizeof *np)))
		return NO;
	for (p = sscPend; p; p = p->next)
		prevcnt += p->irptcnt;
	np->fd = fd;
	np->count = size;
	np->irptcnt = IO_IRPT_CNT - prevcnt;
	np->next = 0;
	if (sscHead) {
		for (p = sscHead; p->next; p = p->next) ;
		p->next = np;
	} else
		sscHead = np;

	if (!sscPend)
		sscPend = np;

	return YES;
}

static void delSscReq(SscReqNode *p)
{
	SscReqNode *q = sscHead;

	if (p != sscHead) {
		while (q->next != p)
			q = q->next;
		q->next = p->next;
	} else
		sscHead = sscHead->next;

	if (p == sscPend)
		sscPend = sscPend->next;
	free(p);
}

void pendSscIrpt(unsigned int type)
{
	unsigned i, reg, bit, vector;
	REG *p = &IRR0;			/* assume IRR0-3 are contiguous */

	for (i = 0; i < irptTop; i++) {
		if (type == irptTbl[i].type) {
			vector = irptTbl[i].irrBit;
			if (vector == 1 || (vector >= 3 && vector <= 15))
				continue;	/* ignore vectors 1 and 3-15 */

			reg = vector >> 6 & 0x3;
			bit = vector & 0x3F;
			*(p + reg) |= (REG)1 << bit;
		}
	}

	if (acceptIrpt())
		intrsim = extint = YES;
}

void doSSC(HWORD num, REG arg0, REG arg1, REG arg2, REG arg3, REG *ret)
{
	BOOL olddt = PSR_DT;

	PSR_DT = 0;

	switch (num) {
	case SSC_CLOSE:
		*ret = close(arg0);
		break;

	case SSC_CONSOLE_INIT: {
		if (interface == BATCH)
			init_console(0, 1);
		else
			xconsole();
		break;
	}

	case SSC_EXIT:
		GrWrtx (8, arg0, 0);
		progExit("kernel exited with status %d\n", (int)arg0);
		break;

	case SSC_GENERATE_INTERRUPT:
		pendSscIrpt(arg0);
		break;

	case SSC_GET_ARGS:
		memBBWrt(arg0, commandline, commandline_len);
		*ret = commandline_len;
		break;

	case SSC_GET_RTC: {
		struct timeval tp;
		struct timezone tzp;
		struct tm *tm;
		SscTime sscTime;

		gettimeofday (&tp, &tzp);
		tm = localtime((const time_t *)&tp.tv_sec);
		sscTime.year = tm->tm_year;
		sscTime.mon  = tm->tm_mon;
		sscTime.mday = tm->tm_mday;
		sscTime.hour = tm->tm_hour;
		sscTime.min  = tm->tm_min;
		sscTime.sec  = tm->tm_sec;
		sscTime.msec = tp.tv_usec/1000;
		sscTime.wday = tm->tm_wday;
		memBBWrtP(arg0, (BYTE *)&sscTime, sizeof(SscTime));
		/* no return value */
		break;
	}

	case SSC_GET_TOD: {
		struct timeval tp;
		struct timezone tzp;
 		SscTimeval tv;
		SscTimezone tz;

		gettimeofday (&tp, &tzp);
 		tv.ssc_Sec = tp.tv_sec;
		tv.ssc_Usec = tp.tv_usec;
		tz.ssc_Minuteswest = tzp.tz_minuteswest;
		tz.ssc_Dsttime = tzp.tz_dsttime;
		memBBWrtP(arg0, (BYTE *)&tv, sizeof(tv));
		if (arg1 != 0)
			memBBWrtP(arg1, (BYTE *)&tz, sizeof(tz));
		break;
	}

	case SSC_GETCHAR: {
		int sz;
		char buf[1];

		sz = read(fdin, buf, 1);
		*ret = (sz == 1) ? buf[0] : 0;
		break;
	}

	case SSC_LOAD_SYMBOLS: {
		char fn[PATH_MAX];
		memBBRd(arg1, fn, 0);
		*ret = (elfSymLoad(fn)) ? 0 : -1;
		break;
	}

	case SSC_OPEN: {
		int flags;
		char fn[PATH_MAX];

		memBBRd(arg0, fn, 0);
		if (arg1 == SSC_READ_ACCESS)
			flags = O_RDONLY;
		else if (arg1 == SSC_WRITE_ACCESS)
			flags = O_WRONLY;
		else if (arg1 == (SSC_READ_ACCESS | SSC_WRITE_ACCESS))
			flags = O_RDWR;
		else
			flags = -1;

		*ret = (flags != -1) ? open(fn, flags) : -1;
		break;
	}

	case SSC_PUTCHAR: {
		char buf[1];

		buf[0] = arg0;
		write(fdout, buf, 1);
		break;
	}

	case SSC_READ: {
		SscDiskReq iov;
		off_t filesize;
		size_t size, ofs;
		ADDR adr;
		char *map;
		int i;

		filesize = lseek(arg0, 0LL, SEEK_END);
		if (filesize == -1 || arg3 > filesize) {
			*ret = -1;
			break;
		}

		size = 0;
		adr = arg2;
		for (i = 0; i < arg1; i++) {
			memBBRd(adr, (BYTE *)&iov, sizeof(iov));
			size += iov.len;
			adr += sizeof(iov);
		}

		if (arg3 + size > filesize)
			size = filesize - arg3;

		map = mmap(0, size, PROT_READ, MAP_SHARED, arg0, arg3);
		if (map == MAP_FAILED) {
			*ret = -1;
			break;
		}

		adr = arg2;
		ofs = 0;
		for (i = 0; i < arg1; i++) {
			memBBRd(adr, (BYTE *)&iov, sizeof(iov));
			if (ofs + iov.len >= size)
				iov.len = size - ofs;
			memBBWrt_opt(iov.addr, map + ofs, iov.len);
			ofs += iov.len;
			adr += sizeof(iov);
		}

		munmap(map, size);

		if (!addSscReq(arg0, size))
			*ret = -1;
		break;
	}

	case SSC_WAIT_COMPLETION: {
		SscDiskStat stat;
		SscReqNode *p;

		memBBRd(arg0, (BYTE *)&stat, sizeof(stat));
		*ret = -1;
		for (p = sscPend; p; p = p->next) {
			if (p->fd == stat.fd) {
				stat.count = p->count;
				delSscReq(p);
				memBBWrt(arg0, (BYTE *)&stat, sizeof(stat));
				*ret = 0;
				break;
			}
		}
		break;
	}

	case SSC_WRITE: {
		SscDiskReq iov;
		off_t filesize;
		size_t size, ofs;
		ADDR adr;
		char *map;
		int i;

		filesize = lseek(arg0, 0LL, SEEK_END);
		if (filesize == -1 || arg3 > filesize) {
			*ret = -1;
			break;
		}

		size = 0;
		adr = arg2;
		for (i = 0; i < arg1; i++) {
			memBBRd(adr, (BYTE *)&iov, sizeof(iov));
			size += iov.len;
			adr += sizeof(iov);
		}

		if (arg3 + size > filesize)
			size = filesize - arg3;

		map = mmap(0, size, PROT_WRITE, MAP_SHARED, arg0, arg3);
		if (map == MAP_FAILED) {
			*ret = -1;
			break;
		}

		adr = arg2;
		ofs = 0;
		for (i = 0; i < arg1; i++) {
			memBBRd(adr, (BYTE *)&iov, sizeof(iov));
			if (ofs + iov.len >= size)
				iov.len = size - ofs;
			memBBRd(iov.addr, map, iov.len);
			ofs += iov.len;
			adr += sizeof(iov);
		}

		munmap(map, size);

		if (!addSscReq(arg0, size))
			*ret = -1;
		break;
	}

#ifdef NEW_MP
	case SSC_SAL_SET_VECTORS:
		if (arg0 == 2) {
			os_boot_rendez_ip=arg1;
			os_boot_rendez_gp=arg2;
		}
		break;
#endif

	default:
		progExit("SSC #%d: unimplemented call\n", num);
		break;
	}

	PSR_DT = olddt;
}
