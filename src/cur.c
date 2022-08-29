/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Curses Routines
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

#include "std.h"

#include <curses.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>

#include "types.h"
#include "ski.h"
#include "ssDCmd.h"
#include "exportui.h"
#include "coreui.h"
#include "program.h"
#include "data.h"
#include "libcore.h"

#define MIN_LINES 24
#define MIN_COLS 80

#define REGSIZE 64	/* Max size */
#define CMDHISTSIZE  20
#define CMDWINSIZE  4
#define TLBSIZE  40

#define MAXCMDLEN 100
#if 0
#define CUR_DEBUG
#endif

static WINDOW *regh, *regw, *prgw, *datw, *cmdh, *cmdw;
static unsigned regwSize, prgwSize, datwSize;
static char erasech, killch;


BOOL userint = NO;

extern ADDR dataStart;

ADDR iAinstAddr(ADDR baseAddr, BYTE iAmode, int delta);
char *skiID(void);
#if !defined __linux__
static void sigwinchHandler(int sig);
static void (*stdSigwinchHandler)(int sig);
#endif /* !defined __linux__ */

static char *ski_id;

/****************************/
/* Register Window Routines */
/****************************/

void regwUpdateCur(void)
{
    REG size;
    unsigned i;

    wstandout(regh);
    mvwprintw(regh, 0, 0, "%-*s", COLS, actRegw->title);
    wstandend(regh);
    wrefresh(regh);
    wclear(regw);
    (void)evalExpr(actRegw->size, DECEXP, &size);
    if (actRegw->curln > size-regwSize)
	actRegw->curln = size-regwSize;
    for (i = 0; i < size; i++)
	mvwprintw(regw, i, 0, "%-*s", COLS, actRegw->fcn(i));
    prefresh(regw, actRegw->curln, 0, 1, 0, regwSize, COLS-1);
}

void prgwUpdateCur(void)
{
    if (fileLoaded) {
	ADDR adr;
	unsigned i, j, srcrows, asmrows, nbytes;
	char *p, buf[80], *text;

	switch (prgCmd) {
	    case PrgUseTop:
		adr = prgwTop;
		break;
	    case PrgUseBot:
		adr = prgwBot;
		break;
	    default:
		adr = prgwCVA;
		if (adr >= prgwTop && adr <= prgwBot)	/* adr is in view */
		    adr = prgwTop;
		else if (prgwIS)	/* iA */
		    adr = iAinstAddr(adr, prgwIAmode, -prgOverlap);
		else			/* EM */
		    adr -= prgOverlap*16;
	    break;
	}
	wmove(prgw, 1, 0);
	wclrtobot(prgw);
	if (prgwIS) {		/* iA */
	    for (prgwTop = adr, i = 0; i < prgRows; i++, adr += nbytes) {
		wmove(prgw, i+1, 0);
		text = prgwLineIA(adr, prgwIAmode, YES, &nbytes);
		wprintw(prgw, "%s", text);
	    }
	    prgBndl = prgRows;
	    prgwBot = adr - nbytes;
	} else {		/* EM */
	    if (prgCmd != PrgUseBot) {
		for (prgwTop = adr, i = prgRows, prgBndl = 0; i > 0; ) {
		    wmove(prgw, prgRows-i+1, 0);
		    text = prgwLine(adr, &srcrows, &asmrows);
		    if (i >= (srcrows + asmrows)) {
			i -= srcrows + asmrows;
			wprintw(prgw, "%s", text);
			adr += 16;
			prgBndl++;
		    } else {	/* fill in remaining lines with source */
			for (p = text, j = 0; j < i && j < srcrows; j++)
			    p = strchr(p, '\n') + 1;
			*p = '\0';
			wprintw(prgw, "%s", text);
			i = 0;
		    }
		}
		prgwBot = adr - 16;
	    } else {		/* PrgUseBot */
		for (i = prgRows, prgBndl = 0; i > 0; ) {
		    text = prgwLine(adr, &srcrows, &asmrows);
		    wmove(prgw, i-(srcrows + asmrows)+1, 0);
		    if (i >= (srcrows + asmrows)) {
			i -= srcrows + asmrows;
			wprintw(prgw, "%s", text);
			adr -= 16;
			prgBndl++;
		    } else {
			wmove(prgw, 1, 0);
			if (i >= asmrows) {
			    for (p = text, j = 0; j < (srcrows+asmrows) - i;
				 j++)
				p = strchr(p, '\n') + 1;
			    wprintw(prgw, "%s", p);
			    i = 0;
			} else
			    for (; i > 0; i--)
				wdeleteln(prgw);
		    }
		}
		prgwTop = adr + 16;
	    }
	}
	wstandout(prgw);
	if (strlen(p = getSrcName(prgwTop)))
	    (void)sprintf(buf, "Program           (file: %s)", p);
	else
	    (void)strcpy(buf, "Program");
	mvwprintw(prgw, 0, 0, "%-*s", COLS, buf);
	wstandend(prgw);
	prgCmd = PrgUseTop;
    } else {
	int n = 0;
	unsigned lines = 0, len = 0, maxlen = 0, spaces;
	char *p, *buf = productInfo(), line[MIN_COLS];

	for (p = buf; *p; p++)
	    if (*p != '\n')
		len++;
	    else {
		if (len > maxlen)
		    maxlen = len;
		len = 0;
		lines++;
	    }
	spaces = (COLS - maxlen)/2;
	wmove(prgw, (prgRows - lines)/2 + 1, 0);
	for (p = buf; *p; p++) {
	    line[n++] = *p;
	    if (*p == '\n') {
		line[n] = '\0';
		wprintw(prgw, "%*s%s", spaces, "", line);
		n = 0;
	    }
	}
    }
    wrefresh(prgw);
}


void datwUpdateCur(void)
{
    wstandout(datw);
    if (datExpr[0] && !strcmp(actDatw->tag, "Data")) {
	char buf[200];

	(void)snprintf(buf, sizeof (buf), "%s      (Tracking: %s)", actDatw->title, datExpr);
	mvwprintw(datw, 0, 0, "%-*s", COLS, buf);
    } else
	mvwprintw(datw, 0, 0, "%-*s", COLS, actDatw->title);
    wstandend(datw);
    wmove(datw, 1, 0);
    wclrtobot(datw);
    mvwaddstr(datw, 1, 0, actDatw->bdfcn(datwSize));
    wrefresh(datw);
#if 0
    cmdwUpdateCur();
#endif
}

void cmdwUpdateCur(void)
{
    wmove(cmdw, LINES-1, 3);
    wrefresh(cmdh);
    prefresh(cmdw, CMDHISTSIZE-CMDWINSIZE, 0,
		LINES-CMDWINSIZE, 0, LINES-1, COLS-1);
}

BOOL regwFwdCur(unsigned argc, char *argv[])
{
    REG size, cnt;
    unsigned rwline = actRegw->curln;

    (void)evalExpr(actRegw->size, DECEXP, &size);
    if (argc) {
	if (!evalExpr(argv[0], DECEXP, &cnt))
	    return NO;
	rwline += cnt;
    } else
	rwline += regwSize-1;
    if (size > regwSize && rwline > (size-regwSize))
	rwline = size-regwSize;
    actRegw->curln = rwline;
    regwUpdateCur();
    return YES;
}

BOOL regwBkwdCur(unsigned argc, char *argv[])
{
    REG size, cnt;
    int rwline = actRegw->curln;

    (void)evalExpr(actRegw->size, DECEXP, &size);
    if (argc) {
	if (!evalExpr(argv[0], DECEXP, &cnt))
	    return NO;
	rwline -= cnt;
    } else
	rwline -= regwSize-1;
    if (rwline < 0)
	rwline = 0;
    actRegw->curln = rwline;
    regwUpdateCur();
    return YES;
}

#ifdef TO_BE_USED
tlbCur()
{
    WINDOW *tlbh, *tlbd;
    int i;

    tlbh = newwin(2, COLS, 0, 0);
    tlbd = newpad(TLBSIZE, COLS);

    mvwprintw(tlbh, 0, 0, "%-*s", COLS, "TLB Header Line 1");
    mvwprintw(tlbh, 1, 0, "%-*s", COLS, "TLB Header Line 2");
    for (i = 0; i < TLBSIZE; i++)
	mvwprintw(tlbd, i, 0, "TLB Data Line %2d", i);
    wrefresh(tlbh);
    prefresh(tlbd, 7, 0, 2, 0, LINES-1, COLS-1);
}
#endif

#ifdef CUR_DEBUG
static FILE *dbg;
#endif

static void createWindows(void)
{
    if (LINES >= MIN_LINES+4) {
	regwSize = 9;
	prgwSize = 10 + (LINES - MIN_LINES-4)/2;
	datwSize = 3 + (LINES - MIN_LINES-3)/2;
    } else {
	regwSize = 5;
	prgwSize = 10 + (LINES - MIN_LINES)/2;
	datwSize = 3 + (LINES - MIN_LINES+1)/2;
    }

    /* register window */
    regh = newwin(1, COLS, 0, 0);
    regw = newpad(REGSIZE, COLS);
    leaveok(regw, TRUE);

    /* program window */
    prgw = newwin(prgwSize, COLS, regwSize+1, 0);
#if defined CUR_DEBUG && !defined __linux__
fprintf(dbg, "init: %d %d %d %d %d %d %d %d\n",
prgw->_maxx, prgw->_maxy,
prgw->_begx, prgw->_begy,
prgw->_allocx, prgw->_allocy,
prgw->_bmarg, prgw->_flags);
fflush(dbg);
#endif /* defined CUR_DEBUG && !defined __linux__ */
    wstandout(prgw);
    mvwprintw(prgw, 0, 0, "%-*s", COLS, "Program");
    wstandend(prgw);
    leaveok(prgw, TRUE);
    prgRows = prgwSize - 1;
    prgColumns = COLS - 1;	/* XXX - "-1" to avoid using last char? */

    /* data window */
    datw = newwin(datwSize, COLS, regwSize+prgwSize+1, 0);
    wstandout(datw);
    mvwprintw(datw, 0, 0, "%-*s", COLS, "Data");
    wstandend(datw);
    leaveok(datw, TRUE);
    datRows = datwSize - 1;
    datInfo.numDataWds = COLS < 122 ? 4 : 8;
    datInfo.expr[0] = '\0';

    /* command header window */
    cmdh = newwin(1, COLS, regwSize+prgwSize+datwSize+1, 0);
#if 0
    cmdwSetStatusCur("");
#else
    wstandout(cmdh);
    mvwprintw(cmdh, 0, 0, "%s%*s", "Command", (int)(COLS-strlen("Command")), ski_id);
    wstandend(cmdh);
#endif
}

/*ARGSUSED*/
static void userintHandler(int sig)
{
    (void)signal(SIGINT, userintHandler);
    userint = YES;
    setIntrsim(YES);
}

#if !defined __linux__
/*ARGSUSED*/
static void sigwinchHandler(int sig)
{
#ifdef CUR_DEBUG
    fprintf(dbg, "got sigwinch, COLS = %d, COLUMNS = %s\n",
	COLS, getenv("COLUMNS"));
    fflush(dbg);
#endif
    (void)signal(SIGWINCH, stdSigwinchHandler);
    (void)raise(SIGWINCH);
    (void)signal(SIGWINCH, sigwinchHandler);
#ifdef CUR_DEBUG
    fprintf(dbg, "got sigwinch2, COLS = %d, COLUMNS = %s\n",
	COLS, getenv("COLUMNS"));
    fflush(dbg);
#endif
    if (LINES < MIN_LINES || COLS < MIN_COLS) {
	/* XXX - should stay in simulator and wait until user makes screen
		 bigger or quits
	 */
	endwin();
	(void)fprintf(stderr, "screen size is %dx%d -- minimum is %dx%d\n",
		      LINES, COLS, MIN_LINES, MIN_COLS);
	exit(EXIT_FAILURE);
    }
    delwin(regh);
    delwin(regw);
    delwin(prgw);
    delwin(datw);
    delwin(cmdh);
    createWindows();
    scrnUpdate();
}
#endif /* !defined __linux__ */

void scrnInitCur(void)
{
    unsigned i;

#ifdef CUR_DEBUG
dbg = fopen("cur.dbg", "w");
# if defined __linux__
    trace(0xffffffff);
#endif /* defined __linux__ */
#endif
    if (!isatty(STDOUT_FILENO)) {
	int fd;
	FILE *fp;

	fd = dup(STDOUT_FILENO);
	fp = fopen("/dev/tty", "w");
	(void)dup2(fileno(fp), STDOUT_FILENO);
	setFdmap(STDOUT_FILENO, fd);
    }
    if (!isatty(STDIN_FILENO)) {
	int fd;
	FILE *fp;

	fd = dup(STDIN_FILENO);
	fp = fopen("/dev/tty", "r");
	(void)dup2(fileno(fp), STDIN_FILENO);
	setFdmap(STDIN_FILENO, fd);
    }
    if (!initscr()) {
	(void)fprintf(stderr, "screen initialization failed\n");
	exit(EXIT_FAILURE);
    }
    if (LINES < MIN_LINES || COLS < MIN_COLS) {
	endwin();
	(void)fprintf(stderr, "screen size is %dx%d -- minimum is %dx%d\n",
		      LINES, COLS, MIN_LINES, MIN_COLS);
	exit(EXIT_FAILURE);
    }
    cbreak();
    noecho();
    erasech = erasechar();
    killch  = killchar();
    createWindows();
    cmdw = newpad(CMDHISTSIZE, COLS);
    scrollok(cmdw, TRUE);
    keypad(stdscr, TRUE);
    (void)signal(SIGINT, userintHandler);
#if !defined __linux__
    stdSigwinchHandler = signal(SIGWINCH, sigwinchHandler);
#endif /* !defined __linux__ */
    (void)signal(SIGPIPE, SIG_IGN);
    /*putp("meml");*/

    /* register window */
    /* actRegw set to GRs at program startup */

    /* data window */
    datwCVA = dataStart;
    for (i = 0; i < topdatw; i++)
	datwtbl[i].size = datwSize;

    /* command window */
    ski_id = skiID();
    cmdwSetStatusCur("");
    for (i = 0; i < CMDHISTSIZE; i++)
	mvwprintw(cmdw, i, 0, "%s", "");
    mvwprintw(cmdw, CMDHISTSIZE-1, 0, "* ");

    scrnUpdate();
}

void scrnEndCur(void)
{
    prefresh(cmdw, CMDHISTSIZE-CMDWINSIZE, 0,
	     LINES-CMDWINSIZE, 0, LINES-1, COLS-1);
    endwin();
}

void prgwDrawCur(void)
{
    prgwUpdateCur();
}

void datwDrawCur(void)
{
    datwUpdateCur();
}

void cmdwPrintCur(const char *s)
{
    static char news[200], *np;

    (void)strcpy(news, s);
    /* XXX - strtok doesn't handle \n at beginning or multiple consecutive
	     newlines within the string
     */
    /* XXX - seems to be OK if strlen(np) > COLS -- just gets truncated? */
    for (np = strtok(news, "\n"); np; np = strtok(NULL, "\n")) {
	mvwaddstr(cmdw, CMDHISTSIZE-1, 0, np);
	scroll(cmdw);
    }
    prefresh(cmdw, CMDHISTSIZE-CMDWINSIZE, 0,
	     LINES-CMDWINSIZE, 0, LINES-1, COLS-1);
}

void cmdwSetStatusCur(const char *msg)
{
    char buf[80];

    wstandout(cmdh);
    (void)sprintf(buf, "Command %2d/%-2d  %s", viewPid+2, 5, msg);
    mvwaddstr(cmdh, 0, 0, buf);
    mvwprintw(cmdh, 0, strlen(buf), "%*s", (int)(COLS-strlen(buf)), ski_id);
    wstandend(cmdh);
    wrefresh(cmdh);
}

static void scrnRedrawCur(void)
{
    regwUpdateCur();
    prgwUpdateCur();
    datwUpdateCur();
    cmdwUpdateCur();
}

void cmdOutCur(const char *hdr, const char *buf)
{
    FILE *pd;
    static char *pager = NULL;
    int pgst;

#if !defined PAGER
#  define PAGER "less"
#endif /* !defined PAGER */

    if (!pager && !(pager = getenv("PAGER")))
	pager = PAGER;
    if (!(pd = popen(pager, "w"))) {
	cmdErr("popen failed\n");
	return;
    }
    clear();
    refresh();
    (void)fputs(hdr, pd);
    (void)fputs(buf, pd);
    pgst = pclose(pd);

#if 0
    touchwin(regw);
    touchwin(prgw);
    touchwin(datw);
    touchwin(cmdw);
#endif
#if 0
    werase(regh);
    werase(regw);
    werase(prgw);
    werase(datw);
    werase(cmdh);
    werase(cmdw);
#endif
    clear();
    refresh();
    touchwin(prgw);
    touchwin(datw);
    touchwin(cmdh);
    scrnRedrawCur();
    /*
     * Put the terminal back in keypad mode; savetty()/restty()
     * doesn't do this and reset_shell_mode()/reset_prog_mode()
     * doesn't work either because those routines look at the keypad
     * flag of stdscr only.  Nobody said ncurses is a nice API...
     */
    keypad(cmdw, TRUE);
    if (pgst != -1 && pgst > 0)
	cmdErr("Pager %s not found (status=%x)\n", pager, pgst);
}

void cmdLoopCur(void)
{
    int cmdln;
    char cmdline[MAXCMDLEN] = "", prevcmd[MAXCMDLEN] = "";

    cmdln = 0;

    for (;;) {
	int in, pos;

	pos = 0;
	cmdln = 0;
	do {
	    /*
	     * This used to call mvgetch() but since ncurses does not
	     * deal with overlapping windows, we must not try to read
	     * from stdscr... --davidm 03/04/00
	     */
	    in = mvwgetch(cmdw, CMDHISTSIZE-1, pos+2);
#ifdef CUR_DEBUG
	    fprintf(dbg, "pos=%d, in=%x\n", pos, in);
#endif
#if defined __linux__
	    if (in == 010) {
#ifdef CUR_DEBUG
		fprintf(dbg, "got KEY_RESIZE: COLS=%d, LINES=%d\n",
			COLS, LINES);
#endif
		delwin(regh);
		delwin(regw);
		delwin(prgw);
		delwin(datw);
		delwin(cmdh);
		while (LINES < MIN_LINES || COLS < MIN_COLS) {
		    cmdwPrint("screen size is %dx%d -- minimum is %dx%d:"
			      " please make screen bigger",
			      LINES, COLS, MIN_LINES, MIN_COLS);
		    mvwgetch(cmdw, CMDHISTSIZE-1, pos+2);
		    if (LINES >= MIN_LINES && COLS >= MIN_COLS)
			cmdwPrint("\n");
		}
		cmdwPrint("new screen size is %dx%d\n", LINES, COLS);
		createWindows();
		scrnUpdate();
		mvwaddstr(cmdw, CMDHISTSIZE-1, 0, "* ");
		mvwaddstr(cmdw, CMDHISTSIZE-1, 2, cmdline);
		prefresh(cmdw, CMDHISTSIZE-CMDWINSIZE, 0,
			 LINES-CMDWINSIZE, 0, LINES-1, COLS-1);
		continue;
	    } else
#endif /* defined __linux__ */
        if (in == erasech || in == KEY_BACKSPACE || in == '\010' ) {
		/* backspace/delete */
		if (!pos)
		    continue;
		pos--;
		cmdline[pos] = '\0';
#if defined __linux__
		mvwdelch(cmdw, CMDHISTSIZE-1, pos+2);
#else /* !defined __linux__ */
        mvwaddch(cmdw, CMDHISTSIZE-1, pos+2, ' ');
        wmove(cmdw, CMDHISTSIZE-1, pos+2);
#endif /* !defined __linux__ */
		prefresh(cmdw, CMDHISTSIZE-1, 0, LINES-1, 0, LINES-1, COLS-1);
		continue;
	    } else if (in == killch) {
		/* erase to beginning (^U) */
		pos = 0;
		cmdline[pos] = '\0';
		wmove(cmdw, CMDHISTSIZE-1, 2);
		wclrtoeol(cmdw);
		prefresh(cmdw, CMDHISTSIZE-1, 0, LINES-1, 0, LINES-1, COLS-1);
		continue;
	    } else if (in == '\027') {
		/* erase previous word (^W) */
		if (!pos)
		    continue;
		pos--;
		while (cmdline[pos] == ' ')
		    pos--;
		while (pos && cmdline[pos] != ' ')
		    pos--;
		if (pos)
		    pos++;
		cmdline[pos] = '\0';
		wmove(cmdw, CMDHISTSIZE-1, pos+2);
		wclrtoeol(cmdw);
		prefresh(cmdw, CMDHISTSIZE-1, 0, LINES-1, 0, LINES-1, COLS-1);
		continue;
	    } else if (in == KEY_SF || in == KEY_DOWN || in == '\016') {
		/* ^N */
		if (cmdln == 0)
		    continue;
		cmdln--;
		prefresh(cmdw, CMDHISTSIZE-CMDWINSIZE-cmdln, 0,
			LINES-CMDWINSIZE, 0, LINES-1, COLS-1);
		continue;
	    } else if (in == KEY_SR || in == KEY_UP || in == '\020') {
		/* ^P */
		if (CMDHISTSIZE-CMDWINSIZE-cmdln == 0)
		    continue;
		cmdln++;
		prefresh(cmdw, CMDHISTSIZE-CMDWINSIZE-cmdln, 0,
			LINES-CMDWINSIZE, 0, LINES-1, COLS-1);
		continue;
	    } else if (in >= KEY_BREAK) {
#ifdef CUR_DEBUG
		fprintf(dbg, "got unknown KEY %d\n", in);
#endif
		continue;
	    }
	    if ((in & 0377) == '\n') {
		cmdline[pos] = '\0';
		break;
	    }
#if defined __linux__
	    cmdline[pos++] = in;
	    pechochar(cmdw, in);
	    if (cmdln) {
		cmdln = 0;
		prefresh(cmdw, CMDHISTSIZE-CMDWINSIZE, 0,
			 LINES-CMDWINSIZE, 0, LINES-1, COLS-1);
	    }
#else
	    cmdline[pos] = in;
	    mvwaddch(cmdw, CMDHISTSIZE-1, pos+2, in);
	    prefresh(cmdw, CMDHISTSIZE-1, 0, LINES-1, 0, LINES-1, COLS-1);
	    pos++;
#endif
	} while (1);
	if (cmdline[0] == '\0' && prevcmd[0]) {	/* empty command */
	    (void)strcpy(cmdline, prevcmd);
	    wmove(cmdw, CMDHISTSIZE-1, 2);
	    wclrtoeol(cmdw);
	    waddstr(cmdw, cmdline);
	    prefresh(cmdw, CMDHISTSIZE-1, 0, LINES-1, 0, LINES-1, COLS-1);
	}
	(void)strcpy(prevcmd, cmdline);
	scroll(cmdw);

	if (cmdline[0])
	    (void)cmdExLin(cmdline);
	else {
	    mvwaddstr(cmdw, CMDHISTSIZE-1, 0, "No previous command");
	    prefresh(cmdw, CMDHISTSIZE-CMDWINSIZE, 0,
		     LINES-CMDWINSIZE, 0, LINES-1, COLS-1);
	    scroll(cmdw);
	}
	mvwaddstr(cmdw, CMDHISTSIZE-1, 0, "* ");
	prefresh(cmdw, CMDHISTSIZE-CMDWINSIZE, 0,
		LINES-CMDWINSIZE, 0, LINES-1, COLS-1);
    }
}
