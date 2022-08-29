/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 *
 * Simulator Main Routines
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

#if defined __linux__
# include <sys/time.h>
#else /* !defined __linux__ */
# include <time.h>
#endif /* !defined __linux__ */
#include <string.h>
#include "std.h"
#include "bits.h"
#include "types.h"
#include "fields.h"
#include "ski.h"
#include "sim.h"	/* for Interface */
#include "ssDCmd.h"
#include "exportui.h"
#include "coreui.h"
#include "libsrs.h"
#include "libcore.h"
#include "icnt_ui.gen.h"

#define NELEM(arr)	(sizeof arr/sizeof arr[0])

/*##################### Globals - Exports ##################################*/

BOOL fileLoaded = NO;
char *icntfile = NULL;
#ifdef NEW_MP
unsigned viewPid = 0;
#endif

Interface interface;

#if defined __linux__
char commandline[1024];  /* XXX S.Eranian added to capture the command line */
REG  commandline_count;
unsigned commandline_len;
#endif /* defined __linux__ */

/*##################### Globals - Imports ##################################*/

#ifdef CALCINFO
extern unsigned calcInfo;
#endif
#ifndef CORE_STATE
extern BOOL dosABI, unixABI;
#endif

void bptLoad(void);
void bptUnload(void);

extern void saveOpenFiles(FILE *);
extern void restoreOpenFile(char *, unsigned, unsigned, unsigned);

/*##################### Local Variables ####################################*/

static CTR start_insts, start_cycles, start_faults;
static BOOL loadedbpt;
static struct timeval starttime, endtime;
static struct timezone tzp;

/*##################### Functions and Subroutines ##########################*/

/* qsort comparison function to reverse sort based on total execution count */
static int cntCompare(const void *a, const void *b)
{
    CTR aCnt = ((ICntPtr)a)->cnt.cnt, bCnt = ((ICntPtr)b)->cnt.cnt;

    return (aCnt < bCnt) - (aCnt > bCnt);
}

ICNTS getICnts(unsigned);

static void dumpIcnts(void)
{
    unsigned i;
    FILE *fp;

    if (!icntfile)
	return;
    if (!(fp = fopen(icntfile, "w"))) {
	msgwPrint("Couldn't open instruction count file\n");
	exitSim(EXIT_FAILURE);
    }
    for (i = 0; i < NELEM(icnt); i++)
	icnt[i].cnt = getICnts(i);
    qsort(icnt, NELEM(icnt), sizeof icnt[0], cntCompare);
    for (i = 0; i < NELEM(icnt); i++)
	if (icnt[i].cnt.cnt)
	    (void)fprintf(fp, "%-39s %9llu %9llu %9llu %9llu\n",
			  icnt[i].name,
			  (unsigned long long) icnt[i].cnt.cnt,
			  (unsigned long long) icnt[i].cnt.ptcnt,
			  (unsigned long long) icnt[i].cnt.pfcnt,
			  (unsigned long long) icnt[i].cnt.p0cnt);
	else
	    break;
    (void)fclose(fp);
}

static char *iRate(struct timeval t1, struct timeval t2, CTR insts, CTR cycles,
		   CTR faults)
{
    double secs;
    static char buf[80];
    char *p;

    if (t1.tv_usec > t2.tv_usec) {
	t2.tv_usec += 1000000;
	t2.tv_sec--;
    }
    secs = t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec)/1000000.0;
    p = buf;
    p += sprintf(p, "%llu insts", insts);
    if (faults != ~(CTR)0)
	p += sprintf(p, " (%llu fault%s)", faults, faults != 1 ? "s" : "");
    if (secs)
	p += sprintf(p, ", %.2f sec, %.0f i/s", secs, insts/secs);
    else
	p += sprintf(p, ", 0 sec");
    if (cycles)
	p += sprintf(p, ", %llu cycles, %.2f ipc\n",
		     cycles, (double)insts/cycles);
    else
	p += sprintf(p, "\n");
    return buf;
}

static void setup_execLoop(void)
{
    start_insts = getTotalInsts();
    start_cycles = getTotalCycles();
    start_faults = getTotalFaults();
    loadedbpt = NO;
    traceDisc();
    (void)gettimeofday(&starttime, &tzp);
}

#ifdef NEW_MP
extern unsigned curPid;
#endif
void cleanup_execLoop(BOOL showIrate)
{
#if 0
    REG psrval = psrGet(0);
#endif
    char *msg;

#ifdef CALCINFO
printf("calcInfo = %u\n", calcInfo);
#endif
    (void)gettimeofday(&endtime, &tzp);
    if (loadedbpt)
	bptUnload();

    if ((interface != BATCH || showIrate) && (msg = getExecMsg()))
	msgwPrint("%s", msg);
    if (showIrate) {
	CTR tf = getTotalFaults();

	msgwPrint(iRate(starttime, endtime,
			getTotalInsts()-start_insts,
			getTotalCycles()-start_cycles,
			tf == ~(CTR)0 ? tf : tf-start_faults));
    }
    dumpIcnts();
    traceVcyEnb();
#ifdef NEW_MP
    viewPid = curPid;
#endif
    scrnUpdate();
}

static BOOL stepUntil_firstInst(const char *expr, REG *val)
{
    if (stepIt_loop(1)) {
	(void)evalExpr(expr, HEXEXP, val);
	if (!*val) {
	    bptLoad();
	    loadedbpt = YES;
	    return YES;
	}
    }
    return NO;
}

BOOL stepUntil_loopX(CTR delay, REG val, const char *expr)
{
    REG psrval = psrGet(0);
    BOOL psr_is = X_PSR_IS(psrval);

    while (delay && !val) {
	while (psr_is && delay && !val) {	/* iA */
	    delay--;
	    if (!stepIt_loop(1))
		return NO;
	    (void)evalExpr(expr, HEXEXP, &val);
	    psrval = psrGet(0);
	    psr_is = X_PSR_IS(psrval);
	}
	while (!psr_is && delay && !val) {	/* EM */
	    delay--;
	    if (!stepIt_loop(1))
		return NO;
	    (void)evalExpr(expr, HEXEXP, &val);
	    psrval = psrGet(0);
	    psr_is = X_PSR_IS(psrval);
	}
    }
    return !delay;	/* if delay == 0, call this function again */
}

static void stepUntil_loop(REG val, const char *expr)
{
    REG psrval = psrGet(0);
    BOOL psr_is = X_PSR_IS(psrval);

    while (!val) {
	while (psr_is && !val) {	/* iA */
	    if (!stepIt_loop(1))
		return;
	    (void)evalExpr(expr, HEXEXP, &val);
	    psrval = psrGet(0);
	    psr_is = X_PSR_IS(psrval);
	}
	while (!psr_is && !val) {	/* EM */
	    if (!stepIt_loop(1))
		return;
	    (void)evalExpr(expr, HEXEXP, &val);
	    psrval = psrGet(0);
	    psr_is = X_PSR_IS(psrval);
	}
    }
}

BOOL brCall(ADDR a);
static void stepCall_loop(void)
{
    REG psrval = psrGet(0);
    BOOL psr_is = X_PSR_IS(psrval);

    for (;;) {
	while (psr_is) {	/* iA */
	    if (brCall(ipGet(0)))
		return;
	    if (!stepIt_loop(1))
		return;
	    psrval = psrGet(0);
	    psr_is = X_PSR_IS(psrval);
	}
	while (!psr_is) {	/* EM */
	    if (brCall(ipGet(0)))
		return;
	    if (!stepIt_loop(1))
		return;
	    psrval = psrGet(0);
	    psr_is = X_PSR_IS(psrval);
	}
    }
}

static void stepUntil(char *expr)
{
    REG val;

    setup_execLoop();
    if (!stepUntil_firstInst(expr, &val)) {
	cleanup_execLoop(NO);
	return;
    }
    cmdwSetStatus("Running...");
    switch (interface) {
	case X_INTERFACE:
#ifdef HAVE_MOTIF
	    if (!cmdFile) {
		stepUntil_setupX(expr);
		break;
	    }
#else
	    fprintf (stderr,
		     "Sorry, MOTIF support has not been compiled in.\n");
	    exit (-1);
#endif
	case CURSES_INTERFACE:
	case BATCH:
	    stepUntil_loop(val, expr);
	    cleanup_execLoop(getTotalInsts()-start_insts > 1);
	    break;
    }
    cmdwSetStatus("");
}

static BOOL stepIt_firstInst(CTR *cnt)
{
    if (stepIt_loop(1)) {
	if (*cnt) {
	    (*cnt)--;
	    if (*cnt) {
		bptLoad();
		loadedbpt = YES;
	    }
	}
	return YES;
    }
    return NO;
}

static void stepCall(void)
{
    CTR cnt = 2;

    setup_execLoop();
    if (!stepIt_firstInst(&cnt)) {
	cleanup_execLoop(NO);
	return;
    }
    cmdwSetStatus("Running...");
    switch (interface) {
	case X_INTERFACE:
            break;
	case CURSES_INTERFACE:
	case BATCH:
	    stepCall_loop();
	    cleanup_execLoop(getTotalInsts()-start_insts > 1);
	    break;
    }
    cmdwSetStatus("");
}

static void stepIt(CTR cnt)
{
    setup_execLoop();
    if (!stepIt_firstInst(&cnt)) {
	cleanup_execLoop(NO);
	return;
    }
    cmdwSetStatus("Running...");
    switch (interface) {
	case X_INTERFACE:
#ifdef HAVE_MOTIF
	    if (!cmdFile) {
		stepIt_setupX(cnt);
		break;
	    }
#else
	    fprintf (stderr,
		     "Sorry, MOTIF support has not been compiled in.\n");
	    exit (-1);
#endif
        case GTK_INTERFACE:
#ifdef HAVE_GTK
	    stepIt_setupGtk(cnt);
#else
	    fprintf (stderr,
		     "Sorry, GTK support has not been compiled in.\n");
#endif
	    break;
	case CURSES_INTERFACE:
	case BATCH:
	    (void)stepIt_loop(cnt);
	    cleanup_execLoop(getTotalInsts()-start_insts > 1);
	    break;
    }
    cmdwSetStatus("");
}

BOOL stepProg(unsigned argc, char *argv[])
{
    CTR cnt = 1;
    REG tmp;
    BOOL until = NO;

    if (!fileLoaded || getExited()) {
	msgwPrint("Nothing to run\n");
	return NO;
    }
    if (argc >= 1) {
	if (!strcmp(argv[0], "until")) {
	    if (argc == 1)
		return NO;
	    if (!evalExpr(argv[1], HEXEXP, &tmp))	/* check exp validity */
		return NO;
	    until = YES;
	} else if (!strcmp(argv[0], "call")) {
	    stepCall();
	    return YES;
	} else
	    if (!evalExpr(argv[0], DECEXP, &cnt))
		return NO;
    }
    if (until)
	stepUntil(argv[1]);
    else
	stepIt(cnt);
    return YES;
}

static BOOL runIt_firstInst(void)
{
    if (stepIt_loop(1)) {
	bptLoad();
	loadedbpt = YES;
	return YES;
    }
    return NO;
}

void runIt(BOOL showIrate)
{
    setup_execLoop();
    if (!runIt_firstInst()) {
	cleanup_execLoop(showIrate);
	return;
    }
    cmdwSetStatus("Running...");
    switch (interface) {
	case X_INTERFACE:
#ifdef HAVE_MOTIF
	    if (!cmdFile) {
		runIt_setupX();
		break;
	    }
#else
	    fprintf (stderr,
		     "Sorry, MOTIF support has not been compiled in.\n");
	    exit (-1);
#endif
        case GTK_INTERFACE:
#ifdef HAVE_GTK
	    runIt_setupGtk();
	    break;
#else
	    fprintf (stderr,
		     "Sorry, GTK support has not been compiled in.\n");
	    exit(-1);
#endif
	case CURSES_INTERFACE:
	case BATCH:
	    (void)runIt_loop();
	    cleanup_execLoop(showIrate);
	    break;
    }
    cmdwSetStatus("");
}

BOOL runProg(unsigned argc, char *argv[])
{
    if (!fileLoaded || getExited()) {
	msgwPrint("Nothing to run\n");
	return NO;
    }
    runIt(YES);
    return YES;
}


/*
 * Save/Restore Routines
 */

BOOL saveSimState(FILE *f)
{
    fprintf(f, "ski_initfd 0\n");
#ifndef CORE_STATE
    if (unixABI)
	fprintf(f, "ski_unixABI 0\n");
    if (dosABI)
	fprintf(f, "ski_dosABI 0\n");
#endif
    if (getLp64(0))
	fprintf(f, "ski_lp64 0\n");
    if (getExited())
	fprintf(f, "ski_$exited$ 0\n");
    fprintf(f, "ski_$insts$ 1 %llx\n", getTotalInsts());
    fprintf(f, "ski_$cycles$ 1 %llx\n", getTotalCycles());
    fprintf(f, "ski_$faults$ 1 %llx\n", getTotalFaults());
    fprintf(f, "ski_$heap$ 1 %llx\n", heapGet(0));
    fprintf(f, "ski_max_sp 1 %llx\n", getMaxSP());
    saveOpenFiles(f);
    return YES;
}

RstStat restoreSimState(FILE *f, char *name)
{
    char symval[80];
    REG val;

    if (!strcmp("ski_initfd", name)) {
	if (!srs_nextRstVal(f, "%s", symval))		/* Ignore count */
	    return ERROR;
	initAppState(0);
	fileLoaded = YES;	/* XXX - Need a sim variable for this? */
	return FOUND;
    }
#ifndef CORE_STATE
    if (!strcmp("ski_unixABI", name)) {
	if (!srs_nextRstVal(f, "%s", symval))		/* Ignore count */
	    return ERROR;
	setABI(YES);
	(void)ipSet(0, ipGet(0));	/* to initialize icp */
	return FOUND;
    }
    if (!strcmp("ski_dosABI", name)) {
	if (!srs_nextRstVal(f, "%s", symval))		/* Ignore count */
	    return ERROR;
	setABI(YES);
	(void)ipSet(0, ipGet(0));	/* to initialize icp */
	return FOUND;
    }
#endif
    if (!strcmp("ski_lp64", name)) {
	if (!srs_nextRstVal(f, "%s", symval))		/* Ignore count */
	    return ERROR;
	setLp64(0, YES);
	return FOUND;
    }
    if (!strcmp("ski_$exited$", name)) {
	if (!srs_nextRstVal(f, "%s", symval))		/* Ignore count */
	    return ERROR;
	setExited(YES);
	return FOUND;
    }
    if (!strcmp("ski_$insts$", name)) {
	if (!srs_nextRstVal(f, "%s", symval))
	    return ERROR;
	if (!srs_nextRstVal(f, "%llx", &val))
	    return ERROR;
	setTotalInsts(val);
	return FOUND;
    }
    if (!strcmp("ski_$cycles$", name)) {
	if (!srs_nextRstVal(f, "%s", symval))
	    return ERROR;
	if (!srs_nextRstVal(f, "%llx", &val))
	    return ERROR;
	setTotalCycles(val);
	return FOUND;
    }
    if (!strcmp("ski_$faults$", name)) {
	if (!srs_nextRstVal(f, "%s", symval))
	    return ERROR;
	if (!srs_nextRstVal(f, "%llx", &val))
	    return ERROR;
	setTotalFaults(val);
	return FOUND;
    }
    if (!strcmp("ski_$heap$", name)) {
	if (!srs_nextRstVal(f, "%s", symval))
	    return ERROR;
	if (!srs_nextRstVal(f, "%llx", &val))
	    return ERROR;
	heapSet(0, val);
	return FOUND;
    }
    if (!strcmp("ski_max_sp", name)) {
	if (!srs_nextRstVal(f, "%s", symval))
	    return ERROR;
	if (!srs_nextRstVal(f, "%llx", &val))
	    return ERROR;
	setMaxSP(val);
	return FOUND;
    }
    if (!strcmp("ski_file", name)) {
	unsigned oflag, mode, offset;

	if (!srs_nextRstVal(f, "%s", symval))
	    return ERROR;
	if (!srs_nextRstVal(f, "%o", &oflag))
	    return ERROR;
	if (!srs_nextRstVal(f, "%o", &mode))
	    return ERROR;
	if (!srs_nextRstVal(f, "%x", &offset))
	    return ERROR;
	restoreOpenFile(symval, oflag, mode, offset);
	return FOUND;
    }
    if (!strncmp("ski_", name, strlen("ski_"))) {
	srs_errmsgSet("Unrecognized Ski symbol: %s", name);
	return ERROR;
    } else
	return NOT_FOUND;
}

/*************************************
 *  Command line option processing   *
 *************************************/

static unsigned topargs;

typedef struct {
    char name[NAMLEN];
    void *var;
    ARG kind;
    char interface[IFACELEN];
    char descr[TITLESIZ];
} Args;

static Args args[ARGSIZ];

BOOL argIns(char *name, void *var, ARG kind, char *iface, char *descr)
{
    unsigned i;

    if (topargs == ARGSIZ) {
	(void)fprintf(stderr, "Arguments table overflow - %s ignored\n", name);
	return NO;
    }
    if (strlen(name) >= NAMLEN || strlen(iface) >= IFACELEN) {
	(void)fprintf(stderr,
		"Argument name and/or interface descriptor too long."
		"  %s argument ignored.\n", name);
	return NO;
    }
    for (i = 0; i < strlen(iface); i++)
	if (strchr("bcx", iface[i]) == NULL) {
	    (void)fprintf(stderr,
		    "Illegal interface descriptor: %c.  %s argument ignored\n",
		    iface[i], name);
	    return NO;
	}
    for (i = 0; i < topargs; i++)
	if (!strcmp(name, args[i].name)) {
	    (void)fprintf(stderr, "Argument (%s) already in table.  Ignored\n",
			  name);
	    return NO;
	}
    (void)strcpy(args[topargs].name, name);
    (void)strcpy(args[topargs].interface, iface);
    (void)strcpy(args[topargs].descr, descr);
    args[topargs].var  = var;
    args[topargs].kind = kind;
    topargs++;
    return YES;
}

/* return -1 for unrecognized option, 0 for BOOL option, 1 for the rest */
static int lookupOption(char *argname, char *argval)
{
    char ch;
    unsigned i;

    switch (interface) {
	case BATCH:
	    ch = 'b';
	    break;
	case CURSES_INTERFACE:
	    ch = 'c';
	    break;
	case X_INTERFACE:
	    ch = 'x';
	    break;
    }
    for (i = 0; i < topargs; i++)
	if (!strcmp(argname, args[i].name)) {
	    if (!strchr(args[i].interface, ch))
		break;
	    if (args[i].kind == ARG_BOOL) {
		*(BOOL *)args[i].var = YES;
		return 0;
	    }
	    if (!argval) {
		(void)fprintf(stderr, "missing value for option %s\n", argname);
		return -1;
	    }
	    switch (args[i].kind) {
		case ARG_INT4:
		    *(WORD *)args[i].var = atoi(argval);
		    return 1;
		case ARG_INT8:
		    (void)sscanf(argval, "%llx", (REG *)args[i].var);
		    return 1;
		case ARG_STRING:
		    *(char **)args[i].var = argval;
		    return 1;
		default:
		    break;
	    }
	}
    (void)fprintf(stderr, "unrecognized option %s\n", argname);
    return -1;
}

/* return index of first argument after options or -1 on failure */
int parseOptions(int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc; i++) {
	if (argv[i][0] == '-') {
	    int j;

	    if ((j = lookupOption(argv[i], argv[i+1])) == -1)
		return -1;
	    i += j;
	} else
	    break;
    }
    return i;
}

void displayOptions(void)
{
    char ch;
    unsigned i;

    switch (interface) {
	case BATCH:
	    ch = 'b';
	    break;
	case CURSES_INTERFACE:
	    ch = 'c';
	    break;
	case X_INTERFACE:
	    ch = 'x';
	    break;
    }
    (void)fprintf(stderr, "Options:\n");
    for (i = 0; i < topargs; i++) {
	if (!strcmp(args[i].descr, "") || !strchr(args[i].interface, ch))
	    continue;
	(void)fprintf(stderr, "    %s\n", args[i].descr);
    }
}
