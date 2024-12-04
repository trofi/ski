/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Main Function and Command-Line Parsing
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

#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>

#include "std.h"
#include "bits.h"
#include "types.h"
#include "fields.h"
#include "simmem.h"
#include "ski.h"
#include "ssDCmd.h"
#include "asm.h"
#include "libsym.h"
#include "load.h"
#include "platform.h"
#include "ui.h"

extern Interface interface;

static char *initfile = NULL;
static char *restfile = NULL;
char *initramfsFile = NULL;
extern BOOL bstats, alwaysNat, natStats;
extern WORD maxNats;
static BOOL dispargs = NO;
extern char *icntfile;
static WORD xcnt = 0;
static REG rcnt = 0;
#ifdef NEW_MP
/* XXX - make this static and pass to libcore when ready. */
/*static*/ WORD numcpus = 1;
#endif

#ifdef SKINET_ENABLE
extern BOOL ski_nonet;
#endif
#ifdef __linux__
extern char *src_root;
extern BOOL force_user;
extern BOOL force_system;
extern BOOL trace_syscalls;
#endif

#if defined(__FreeBSD__)
#define	DEFAULT_SIM_ROOT	"/compat/ski"
#else
#define	DEFAULT_SIM_ROOT	""
#endif

extern char *sim_root;
extern size_t sim_root_len;

static char *progname;

#if defined __linux__
/* XXX S.Eranian added to capture the command line */
extern char commandline[1024];
extern REG  commandline_count;
extern unsigned commandline_len;
#endif /* defined __linux__ */

/*************************************
 *  Command line option processing   *
 *************************************/

static void argsInit(void)
{
    argIns("-help",      &dispargs,  ARG_BOOL,   "bcx",
      "-help        Display command-line options");
    argIns("-i",         &initfile,  ARG_STRING, "bcx",
      "-i <file>    Process initialization file at startup");
    argIns("-icnt",      &icntfile,  ARG_STRING, "b",
      "-icnt <file> Store instruction counts in <file>");
    argIns("-nat",       &alwaysNat, ARG_BOOL,   "bcx",	"");
    argIns("-natcount",  &maxNats,   ARG_INT4,   "bcx",	"");
    argIns("-natstats",  &natStats,  ARG_BOOL,   "bcx",	"");
    argIns("-r",         &rcnt,      ARG_INT8,   "c",	"");
    argIns("-rest",      &restfile,  ARG_STRING, "bcx",
      "-rest <file> Restore simulation state from <file>");
#ifdef NEW_MP
    argIns("-cpus",      &numcpus,   ARG_INT4,   "bcx",
      "-cpus <num>  Number of simulated cpus");
#endif
    argIns("-stats",     &bstats,    ARG_BOOL,   "b",
      "-stats       Display execution run-time and instruction rate");
    argIns("-x",         &xcnt,      ARG_INT4,   "x",	"");

#ifdef SKINET_ENABLE
    argIns("-nonet",	    &ski_nonet, ARG_BOOL,   "bcx",
      "-nonet       Disable networking feature");
#endif

#ifdef __linux__
    argIns("-srcroot",   &src_root, ARG_STRING,  "cx",
      "-srcroot     Source Root Directory");

    argIns("-forceuser", &force_user, ARG_BOOL,  "bcx",
      "-forceuser   Force user-level simulation");

    argIns("-forcesystem", &force_system, ARG_BOOL,  "bcx",
      "-forcesystem Force system-level simulation");

    argIns("-strace",    &trace_syscalls, ARG_BOOL, "bcx",
      "-strace      Trace system call execution");

    argIns("-initramfs", &initramfsFile, ARG_STRING,  "bcx",
      "-initramfs   Initramfs file for linux kernel");
#endif

    argIns("-simroot", &sim_root, ARG_STRING, "bcx",
	"-simroot     Simulated root directory");
}

static void Usage(void)
{
    fprintf(stderr, "Usage: %s [options] [file [args]]\n", progname);
}

int main(int argc, char *argv[])
{
    char *p;
    int ndx;
    char *fileName = NULL;


    /*
     * For system-mode, it's sometimes useful to increase the
     * granularity of the physical memory allocations.  For example,
     * data-breakpoints cannot cross a (host) page-size boundary and
     * if you need to watch a large data area for modifications, it
     * can be achieved by setting SKI_PAGE_SIZE to a large
     * integer-power-of-2 value.  Nobody said it's pretty, but it
     * seems to work quite well and keeps the code impact to a
     * minimum.
     */
    p = getenv ("SKI_PAGE_SIZE");
    page_size = 0;
    if (p) {
	page_size = atol (p);
	if ((page_size & (page_size - 1)) != 0 || page_size < getpagesize())
	    page_size = 0;
    }
    if (!page_size)
      page_size = getpagesize();

    /* we are assuming that page_size is a power of 2 */
    if ((page_size & (page_size - 1)) != 0) {
	(void)fprintf(stderr, "%s: Invalid page size (%ld)\n",
		      progname, page_size);
	return EXIT_FAILURE;
    }

    /* compute log2_page_size */
    page_mask = page_size;
    log2_page_size = 0;
    while (page_mask >>= 1) {
	++log2_page_size;
    }

    page_mask = ~((DWORD) page_size - 1);

/* XXX - use basename(3) in 10.X? */
    progname = (p = strrchr(argv[0], '/')) ? p + 1 : argv[0];

    switch (progname[0]) {
	case 'b':
	    interface = BATCH;
	    break;
	case 's':
	    interface = CURSES_INTERFACE;
 	    break;
	default:
	    interface = BATCH;
	    break;
    }

#if defined __linux__
    if (strcmp (progname, "bskinc") == 0) {
	extern BOOL noConsole;
	force_user = 1;
	noConsole = 1;
    }
#endif /* defined __linux */

    argsInit();
    implArgs();

    if ((ndx = parseOptions(argc, argv)) == -1) {
	Usage();
	displayOptions();
	return EXIT_FAILURE;
    } else if (dispargs == YES) {
	displayOptions();
	return EXIT_FAILURE;
    }

    if (icntfile)
	setIcntEnb();
    initSysState();
#ifdef NEW_MP
{
    int i;
    for (i = 0; i < 4; i++)
	initState(i);
}
#else
    initState(0);
#endif
    InitPlatform();
    symInit();
    if (asm_init() != AsmOK) {
	(void)fprintf(stderr, "%s: Out of memory during asm_init\n", progname);
	return EXIT_FAILURE;
    }

    if (ndx < argc) {
	fileName = argv[ndx];

#ifdef __linux__
	/*
	 * S.Eranian added for kernel command line arguments
	 * each argument is added to a single string which is zero terminated
	 * a \0 is inserted between each argument
	 */
	{
	    int i;

	    p = commandline;
	    for (i = ndx + 1; i < argc ; i++ ) {
		if (p + strlen (argv[i]) + 1
		    >= commandline + sizeof(commandline))
		{
		    break;
		}
		p += sprintf(p, "%s", argv[i]);
		*p++ = ' ';
		commandline_count++;
	    }
	    commandline_len = p - commandline;
	}
#endif /* __linux__ */

	if (sim_root == NULL) {
	    sim_root = DEFAULT_SIM_ROOT;
	    if ((p = getenv("SKI_SIMROOT")) != NULL)
                sim_root = p;
	}
	sim_root_len = strlen(sim_root);

	if (!elfLoad(fileName, argc-ndx, &argv[ndx])) {
	    (void)fprintf(stderr, "%s: Could not open %s for reading\n",
			  progname, argv[ndx]);
	    return EXIT_FAILURE;
	}
#ifdef GET_INFO
	printf("delta is %d\n", getInfo()->delta);
#endif
	initAppState(0);
    }

    uiInit();
    implInit();
    scrnInit();
    if (restfile) {
	/* XXX - this calls a menu fn directly which isn't very elegant */
	if (!stateRest(1, &restfile))
	    exitSim(EXIT_FAILURE);
    } else if (initfile) {
	if (!readCmdFile(initfile) && interface == BATCH)
	    exitSim(EXIT_FAILURE);
	scrnUpdate();
    }
    cmdLoop();
    /*NOTREACHED*/
    return EXIT_SUCCESS;
}
