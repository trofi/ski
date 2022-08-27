/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Debugger Command Interpreter Routines
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

#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/param.h>	/* for MIN */
#include <string.h>
#include <signal.h>
#include "std.h"
#include "bits.h"
#include "types.h"
#include "fields.h"
#include "ski.h"
#include "ssDCmd.h"
#include "exportui.h"
#include "coreui.h"
#include "data.h"
#include "libsym.h"
#include "libcore.h"
#include "dos.h"
#include "libtrace.h"

#define MAXFILEDEPTH	16
#define MAXCMDLEN	4000

#if 0
#include "ssDefs.h"
#include "mp.h"
#include "ssReg.h"
#include "program.h"
#endif

/*##################### Globals - Imports ##################################*/

#if 0
extern	BOOL	statsOnly, noscreen;
extern	ADDR	progStart, dataStart;
#else
extern	ADDR	dataStart;
#endif
extern	BOOL	userint;

/*##################### Globals - Exports ##################################*/

#if 0
char	loadname[132] = "";	/* name of load module */
#endif

FILE *cmdFile = NULL;
static IAWORD pspSeg = 0;		/* segment address of PSP */

/*##################### Local Variables   ##################################*/

#if 0
static char	dir[132] = "";	/* directory name for file redirection */
static BOOL	locsyms = NO;	/* local symbols in load file?? */
#endif
static	BOOL	dosABI;

/*##################### Functions and Subroutines ##########################*/

/*--------------------------------------------------------------------------
 * preprocess$Line - Preprocess command line by removing comments and
 *  replacing macros.
 *--------------------------------------------------------------------------*/
static void preppLin(char *cmdbuf)
{
    char *comment;

    if ((comment = strchr(cmdbuf, '#'))) {
	*comment = '\n';
	*(comment+1) = '\0';
    }
}

/* this works like strtok(str, " \t") but also supports quoted arguments */
static char *nextArg(char *str)
{
    char *p, *tok;
    static char *last;

    if (str)
	last = str;

    /* skip over leading whitespace */
    if ((tok = last))
	while (*tok == ' ' || *tok == '\t')
	    tok++;

    if (!tok || *tok == '\0')
	return 0;

    if ((p = strpbrk(tok, " \t\""))) {		/* find whitespace or quote */
	if (*p == '"' && p == tok) {		/* opening quote */
	    tok++;				/* skip it */
	    if ((p = strchr(tok, '"'))) {		/* find closing quote */
		*p++ = '\0';
	    }
	} else {				/* whitespace */
	    *p++ = '\0';
	}
    }

    last = p;
    return tok;
}

/*--------------------------------------------------------------------------
 * command$Execute - Executes the commands in the passed string (and NULL)
 *  terminated string.
 *--------------------------------------------------------------------------*/
static BOOL cmdEx(char *cmd)
{
    char *cmdnam;
    MENU *menu;
    unsigned i, argc;
    char *argv[MAXARGS];

    cmdnam = nextArg(cmd);

    if (!cmdnam) {
	msgwPrint("No command given\n");
	return NO;
    }

    if ((menu = menuLkp(cmdnam))) {
	for (i = 0; i < MAXARGS && (argv[i] = nextArg(NULL)); i++)
	    ;
	if (i >= MAXARGS) {
	    cmdErr("%s: Too many arguments (> %d)\n", cmdnam, MAXARGS);
	    return NO;
	}
	argc = i;
	if (argc < menu->minargs || argc > menu->maxargs) {
	    cmdErr("%s: Illegal number of arguments < %d >\n", cmdnam, argc);
	    return NO;
	}
#if 0
/* debugging code */
	printf("  cmdnam: >>%s<<    argc: %d\n", cmdnam, argc);
	for (i = 0; i < argc; i++)
	    printf("    arg[%d]: >>%s<<\n", i, argv[i]);
#endif
	return menu->fcn(argc, argv);
    } else {
	msgwPrint("%s: No such command\n", cmdnam);
	return NO;
    }
}


/*--------------------------------------------------------------------------
 * command$ExecuteLine - Executes all commands in the passed newline
 *  (and NULL) terminated string.
 *--------------------------------------------------------------------------*/
BOOL cmdExLin(const char *cmdlin)
{
    char cmdbuf[MAXCMDLEN], *cmd[MAXCMDS];
    unsigned i, ccnt;

    (void)strcpy(cmdbuf, cmdlin);
    preppLin(cmdbuf);
    if (cmdbuf[0] == '\n')
	return YES;
    cmd[0] = strtok(cmdbuf, ";\n");
    for (i = 1; i < MAXCMDS && (cmd[i] = strtok(NULL, ";\n")) != NULL; i++)
	;
    if (i >= MAXCMDS) {
	cmdErr("Too many commands in a line (> %d)\n", MAXCMDS);
	return NO;
    }
    ccnt = i;
    for (i = 0; i < ccnt; i++)
	if (!cmdEx(cmd[i]))
	    return NO;
    return YES;
}

#if 0
/*--------------------------------------------------------------------------
 * command$SystemEscape - Get and pass a command string to the system for
 *  execution.
 *--------------------------------------------------------------------------*/
BOOL cmdSysEsc(unsigned argc, char *argv[])
{
    scrnClear();
    (void)system(argv[0]);
    nlWait("\nRETURN to continue...");
    scrnRedraw();
}
#endif

/*--------------------------------------------------------------------------
 * command$Evaluate - Evaluate the expression and display its value
 *--------------------------------------------------------------------------*/
BOOL cmdEval(unsigned argc, char *argv[])
{
    unsigned i;
    REG expr;
    struct isym *psym;
    ADDR adr;

#if 0
if (!strcmp(argv[0], "cols")) {
extern int COLS;
msgwPrint("%s = %d %s\n", argv[0], COLS, getenv("COLUMNS"));
return YES;
}
if (!strcmp(argv[0], "prgw")) {
showPrgDim();
return YES;
}
#endif
    for (i = 0; i < argc; i++) {
	if (!evalExpr(argv[i], HEXEXP, &expr))
	    return NO;
	psym = isymVLkp(argv[i]);
	if (psym && psym->type == IP_SYM) {
	    char tgtbuf[80];

	    symAddrtoName(expr, tgtbuf, 4, NO, 60);
	    msgwPrint("%s = %s\n", argv[i], tgtbuf);
	} else if ((psym && psym->dbase == 16) || symNametoAddr(argv[i], &adr))
	    msgwPrint("%s = %llx\n", argv[i], expr);
	else if (psym)
	    msgwPrint("%s = %llu\n", argv[i], expr);
	else
	    msgwPrint("Hex: %llx   Decimal: %llu\n", expr, expr);
    }
    return YES;
}

/*--------------------------------------------------------------------------
 * command$If - Get a number. If it evaluates to zero, ignore rest of
 *  input line.
 *--------------------------------------------------------------------------*/
BOOL cmdIf(unsigned argc, char *argv[])
{
    unsigned i, colon, first, last;
    REG val;
    MENU *menu;

    for (i = 1; i < argc; i++)
	if (!strcmp(argv[i], ":"))
	    break;
    colon = i;
    if (!evalExpr(argv[0], HEXEXP, &val))
	return NO;
    if (val) {
	first = 1;
	last  = colon;
    } else {
	first = colon+1;
	last  = argc;
	if (first > last)
	    return YES;
    }
    if (first == last) {
	cmdErr("missing command\n");
	return NO;
    }
    if ((menu = menuLkp(argv[first])) != NULL) {
	unsigned nargs = last - first - 1;

	if (nargs < menu->minargs || nargs > menu->maxargs) {
	    msgwPrint("%s: Illegal number of arguments < %d >\n",
		      argv[first], nargs);
	    return NO;
	}
	return (*menu->fcn)(nargs, &argv[first+1]);
    } else {
	msgwPrint("%s: No such command\n", argv[first]);
	return NO;
    }
}

/*--------------------------------------------------------------------------
 * command$Exit - Exit the simulator.
 *--------------------------------------------------------------------------*/
BOOL cmdExit(unsigned argc, char *argv[])
{
    REG exitval = 0;

    if (argc == 1 && !evalExpr(argv[0], DECEXP, &exitval))
	return NO;
    exitSim(exitval);
    /*NOTREACHED*/
    return NO;
}

BOOL readCmdFile(char *fname)
{
    FILE *fp;
    char buf[MAXCMDLEN];
    BOOL status = YES;

    if (!strcmp(fname, "-"))
	fp = stdin;
    else if (!(fp = cmdOpenFile(fname, "r")))
	return NO;

    cmdFile = fp;
    userint = NO;
    while (fgets(buf, MAXCMDLEN, fp)) {
	if (strlen(buf) > 1 && buf[strlen(buf)-2] == ':')
	    continue;
	if (!cmdExLin(buf)) {
	    status = NO;
	    break;
	}
	if (userint) {
	    cmdErr("Interrupting command file\n");
	    userint = NO;
	    setIntrsim(NO);
	    status = NO;
	    break;
	}
    }
    cmdFile = NULL;
    (void)fclose(fp);
    return status;
}

BOOL cmdSource(unsigned argc, char *argv[])
{
    static int fdepth = 0;
    BOOL ret;

    if (fdepth++ == MAXFILEDEPTH) {
	cmdErr("Nesting overflow\n");
	ret = NO;
    } else
	ret = readCmdFile(argv[0]);
    fdepth--;
    return ret;
}

BOOL cmdGoto(unsigned argc, char *argv[])
{
    char buf[MAXCMDLEN], lbl[32];

    if (!cmdFile) {
	cmdErr("goto only allowed inside a command file\n");
	return NO;
    }
    rewind(cmdFile);
    (void)sprintf(lbl, "%s:\n", argv[0]);
    while (fgets(buf, MAXCMDLEN, cmdFile))
	if (!strcmp(buf, lbl))
	    return YES;
    cmdErr("couldn't find label %s\n", argv[0]);
    return NO;
}

static void cmdExpandFilename(char filename[], const char *name)
{
    if (name[0] == '~') {
	if (name[1] == '/' || name[1] == '\0')
	    (void)sprintf(filename, "%s%s", getenv("HOME"), &name[1]);
	else {
	    char *t;
	    struct passwd *pwd;

	    t = strchr(name, '/');
	    if (t)
		*t = '\0';
	    if ((pwd = getpwnam(&name[1])))
		(void)strcpy(filename, pwd->pw_dir);
	    else
		cmdErr("No such user %s\n", &name[1]);
	    if (t) {
		*t = '/';
		(void)strcat(filename, t);
	    }
	}
    } else
	(void)strcpy(filename, name);
}

/*--------------------------------------------------------------------------
 * command$OpenFile - Open a filename with the specified mode.
 *--------------------------------------------------------------------------*/
FILE *cmdOpenFile(char *name, char *mode)
{
    char filename[256];
    FILE *strm;

    cmdExpandFilename(filename, name);
    if (!(strm = fopen(filename, mode)))
	cmdErr("Cannot open file %s (%s) for %s\n", name, filename,
	       mode[0] == 'r' ? "reading" :
	       mode[0] == 'w' ? "writing" :
	       "appending");
    return strm;
}


/*--------------------------------------------------------------------------
 * command$Load - Get a filename, then load the user specified file.
 *--------------------------------------------------------------------------*/
BOOL cmdLoad(unsigned argc, char *argv[])
{
#if 0
    char fn[132], temp[256];
    REG misc;
    unsigned save = cproc;

    (void)strcpy(fn, "a.out");
    getStrToken("Load file: ", fn, 132);
    getNumToken("Misc. info: ", &misc, 16);
    locsyms = (BOOL)misc;
    (void)strcpy(temp, dir); (void)strcat(temp, fn);
    if (cmdLFile(temp, 0, YES)) {
	(void)strcpy(loadname, fn);
	regwUpdate();
	for (cproc = 0; cproc < nproc; cproc++) {
	    prgwCVA = progStart;
	    ia = progStart;
	    datwCVA = dataStart;
	}
	cproc = save;
	prgwDraw();
	datwDraw();
    }
#endif
    if (!elfLoad(argv[0], argc, argv)) {
	fprintf(stderr, "Could not load ELF file: %s\n", argv[0]);
	return NO;
    }
    initAppState(0);
    datwCVA = dataStart;
    scrnUpdate();
    return YES;
}

/*--------------------------------------------------------------------------
 * Load a binary image file at a given address and optionally read symbols
 * from a map file.  The program is loaded at the given offset.
 *--------------------------------------------------------------------------*/
static BOOL bin_load(unsigned argc, char *argv[], ADDR offset)
{
    ADDR start, symOffset;
    FILE *fp;
    struct stat statBuf;
    BYTE *buf, *segAdr;
    EXEheader eh = { 0 };
    EXErelocItemPtr reloc;
    int i;
    off_t codeSize, codeFileOffset;
    IAWORD startSeg, relocSeg;
    Psp psp = { 0xCD20, 0xFF9F };

    if (!(fp = cmdOpenFile(argv[0], "r")))
	return NO;

    if (!evalExpr(argv[1], HEXEXP, &start))
	return NO;

    /* XXX - add code to read OBJ files */

    if (fstat(fileno(fp), &statBuf) == -1) {
	cmdErr("%s\n", strerror(errno));
	return NO;
    }

    /* XXX - should we worry about really big files? */
    if (!(buf = malloc(statBuf.st_size))) {
	cmdErr("%s\n", strerror(errno));
	return NO;
    }

    if (fread(buf, 1, statBuf.st_size, fp) != statBuf.st_size) {
	cmdErr("%s\n", strerror(errno));
	free(buf);
	return NO;
    }

    pspSeg = start >> 4;
    startSeg = (start + offset) >> 4;

    /* XXX - this works on a BIG-ENDIAN host only */
    if (statBuf.st_size >= sizeof (EXEheader) &&
	buf[0] == 0x4d && buf[1] == 0x5a) {	/* EXE signature */
	swab(buf, &eh, sizeof (EXEheader));
	codeFileOffset = eh.exHeaderSize * 16;
	codeSize = statBuf.st_size - codeFileOffset;
	symOffset = offset;
	for (i = 0, reloc = (EXErelocItemPtr)(buf + eh.exRelocTable);
	     i < eh.exRelocItems; i++, reloc++) {
	    swab(reloc, reloc, sizeof (EXErelocItem));
	    segAdr = buf + codeFileOffset + (reloc->seg << 4) + reloc->ofs;
	    relocSeg = (*(segAdr+1) << 8 | *segAdr) + startSeg;
	    *(segAdr+1) = relocSeg >> 8;
	    *segAdr = relocSeg & 0xff;
	}
    } else {
	startSeg = pspSeg;
	eh.exInitIP = 0x100;
	eh.exMaxAlloc = (640*1024UL)/16;
	codeFileOffset = 0;
	codeSize = statBuf.st_size;
	symOffset = 0;
    }

    if (!memMIAIWrt(start + offset, buf + codeFileOffset, codeSize)) {
	cmdErr("write to memory failed\n");
	free(buf);
	(void)fclose(fp);
	return NO;
    }
    free(buf);
    (void)fclose(fp);

    /* XXX - need to build a complete PSP */
    /* XXX - need to set AH & AL (and init FCBs?) */
    if (dosABI) {		/* init DOS environment */
	ADDR memAdr, endAdr;
	unsigned argn;
	BYTE pspCTLen;
	IASegDesc csd, dsd, ssd, esd;
	REG cs, ds, es, ss, tmpreg;
	IAeflags efl;

	cs = startSeg + eh.exInitCS;
	(void)grSet(0, CS_ID, cs);
	tmpreg = grGet(0, CSD_ID);
	csd = *(IASegDescPtr)&tmpreg;
	csd.base = cs << 4;
	csd.lim = 0xffff;
	(void)grSet(0, CSD_ID, *(REG *)&csd);

	ds = pspSeg;
	(void)grSet(0, DS_ID, ds);
	tmpreg = grGet(0, DSD_ID);
	dsd = *(IASegDescPtr)&tmpreg;
	dsd.base = ds << 4;
	dsd.lim = 0xffff;
	(void)grSet(0, DSD_ID, *(REG *)&dsd);

	es = pspSeg;
	(void)grSet(0, ES_ID, es);
	tmpreg = grGet(0, ESD_ID);
	esd = *(IASegDescPtr)&tmpreg;
	esd.base = es << 4;
	esd.lim = 0xffff;
	(void)grSet(0, ESD_ID, *(REG *)&esd);

	ss = startSeg + eh.exInitSS;
	(void)grSet(0, SS_ID, ss);
	tmpreg = grGet(0, SSD_ID);
	ssd = *(IASegDescPtr)&tmpreg;
	ssd.base = ss << 4;
	ssd.lim = 0xffff;
	(void)grSet(0, SSD_ID, *(REG *)&ssd);

	(void)grSet(0, SP_ID, eh.exInitSP);

	tmpreg = arGet(0, EFLAGS_ID);
	efl = *(IAeflagsPtr)&tmpreg;
	efl.iopl = 3;
	efl.if_ = 1;
	efl.rv1 = 1;		/* XXX - this should be somewhere else */
	(void)arSet(0, EFLAGS_ID, *(REG *)&efl);
	(void)ipSet(0, csd.base + eh.exInitIP);
	setDerivedState(1);
	for (pspCTLen = 0, argn = 3; argn < argc; argn++) {
	    psp.pspCommandTail[pspCTLen + 1] = ' ';
	    (void)strncpy((char *)&psp.pspCommandTail[pspCTLen + 2],
			  argv[argn], 125 - pspCTLen);
	    pspCTLen += MIN(strlen(argv[argn]) + 1, 125 - pspCTLen);
	}
	psp.pspCommandTail[0] = pspCTLen;
	psp.pspCommandTail[pspCTLen + 1] = '\r';
	if (!memMIAIWrt(start, (BYTE *)&psp, sizeof psp)) {
	    cmdErr("write to memory failed\n");
	    return NO;
	}
	endAdr = start + offset + codeSize +
	    MIN(eh.exMaxAlloc*16UL, 640*1024UL - codeSize - offset);
	for (memAdr = start + offset + codeSize; memAdr < endAdr;
	     memAdr += page_size)
	    memZerosPageSet(page_base(memAdr));
    }

    if (argc >= 3) {	/* map file specified */
	char line[100], *p;
	ADDR seg, ofs, adr;
	char * rl;

	if (!(fp = cmdOpenFile(argv[2], "r")))
	    return NO;
	/* search for Address section */
	while (fgets(line, 100, fp)) {
	    if (strncmp(line, "  Address", 9) == 0)
		break;
	}
	/* skip blank line */
	rl = fgets(line, 100, fp);
	(void)rl;
	/* put addresses into symbol table */
	while (fgets(line, 100, fp)) {
	    if (strlen(line) < 8)	/* "blank" line marks end of section */
		break;
	    /* XXX - add support for Absolute symbols */
	    p = strtok(line, ":");
	    seg = p ? (ADDR)strtoul(p, 0, 16) : 0;
	    p = strtok(NULL, " \t");
	    ofs = p ? (ADDR)strtoul(p, 0, 16) : 0;
	    adr = (seg << 4) + ofs + start + symOffset;
        if ((p = strtok(NULL, " \t\r\n")))
		symInsert(p, adr, 0);
	}
	(void)fclose(fp);
    }
    fileLoaded = YES;
    scrnUpdate();
    return YES;
}

/*--------------------------------------------------------------------------
 * Load an IA executable (COM/EXE-only for now) at a given address and
 * optionally read symbols from a map file.  The program is loaded at an
 * offset of 0x100 (and someday, a real PSP will be built).
 *--------------------------------------------------------------------------*/
BOOL cmdIALoad(unsigned argc, char *argv[])
{
    REG psrval = psrGet(0);

    X_PSR_IS(psrval) = YES;
    (void)psrSet(0, psrval);
    dosABI = YES;
    setABI(YES);
    return bin_load(argc, argv, (ADDR)0x100);
}

/*--------------------------------------------------------------------------
 * Load a ROM image file at a given address and optionally read symbols
 * from a map file.
 *--------------------------------------------------------------------------*/
BOOL cmdROMLoad(unsigned argc, char *argv[])
{
    dosABI = NO;
    setABI(NO);
    return bin_load(argc, argv, (ADDR)0);
}

/*--------------------------------------------------------------------------
 * Load a Firmware image file to end at 0xFFFFFFFF and optionally read
 * symbols from a map file.
 *--------------------------------------------------------------------------*/
BOOL cmdFWLoad(unsigned argc, char *argv[])
{
    FILE *fp;
    struct stat statBuf;
    BYTE *buf;
    ADDR start;
    off_t codeSize;

    if (!(fp = cmdOpenFile(argv[0], "r")))
	return NO;

    if (fstat(fileno(fp), &statBuf) == -1) {
	cmdErr("%s\n", strerror(errno));
	return NO;
    }

    if (!(buf = malloc(statBuf.st_size))) {
	cmdErr("%s\n", strerror(errno));
	return NO;
    }

    if (fread(buf, 1, statBuf.st_size, fp) != statBuf.st_size) {
	cmdErr("%s\n", strerror(errno));
	free(buf);
	return NO;
    }

    start = 0x100000000ULL - statBuf.st_size;
    codeSize = statBuf.st_size;

    if (!memMIAIWrt(start, buf, codeSize)) {
	cmdErr("write to memory failed\n");
	free(buf);
	(void)fclose(fp);
	return NO;
    }
    free(buf);
    (void)fclose(fp);

    if (argc >= 2) {	/* map file specified */
	char line[100], *p;
	ADDR seg, ofs, adr;
	char * rl;

	if (!(fp = cmdOpenFile(argv[1], "r")))
	    return NO;
	/* search for Address section */
	while (fgets(line, 100, fp)) {
	    if (strncmp(line, "  Address", 9) == 0)
		break;
	}
	/* skip blank line */
	rl = fgets(line, 100, fp);
	(void)rl;
	/* put addresses into symbol table */
	while (fgets(line, 100, fp)) {
	    if (strlen(line) < 8)	/* "blank" line marks end of section */
		break;
	    /* XXX - add support for Absolute symbols */
	    p = strtok(line, ":");
	    seg = p ? (ADDR)strtoul(p, 0, 16) : 0;
	    p = strtok(NULL, " \t");
	    ofs = p ? (ADDR)strtoul(p, 0, 16) : 0;
	    adr = (seg << 4) + ofs + start;
	    if ((p = strtok(NULL, " \t\r\n")))
		symInsert(p, adr, 0);
	}
	(void)fclose(fp);
    }
    fileLoaded = YES;
    scrnUpdate();
    return YES;
}

void *cmdMalloc(size_t amt)
{
    void *p;

    if (!(p = malloc(amt)))
	cmdErr("malloc failed\n");
    return p;
}

#if 0
/*--------------------------------------------------------------------------
 * command$LoadFile - Load the specified file (not necessarily specified
 *   by immediate user input).
 *--------------------------------------------------------------------------*/
BOOL cmdLFile(char *fn, char *cmdmsg, BOOL errrprt)
{
    BOOL loadOK;
    FILE *strm;

#if 0
    if (!(strm = fopen(fn, "r"))) {
	if (errrprt) {
	    char buf[256]; /* XXX - should be max length of fn + extra */

	    (void)sprintf(buf, "Can't open load file - %s\n", fn);
	    cmdErr(buf);
	}
	return NO;
    }

    if (cmdmsg && !statsOnly)
	cmdwPrint(cmdmsg);
    loadOK = loadFile(strm, locsyms);
    (void)fclose(strm);
#else
    loadOK = loadFile(fn, locsyms);
#endif
    runStatus = UNINIT;
    if (!loadOK && noscreen)
	exit(EXIT_FAILURE);
	/*NOTREACHED*/
    else
	return loadOK;
}


/*--------------------------------------------------------------------------
 * command$DirectorySet - Set the directory name for which all command
 *  stream files are to be taken from.
 *--------------------------------------------------------------------------*/
BOOL cmdDirSet(unsigned argc, char *argv[])
{
    char buf[132], *s = buf, *t;

    if (!getStr("Directory name: ", s, 132)) {
	*dir = NULL;
	return;
    }
    if (s[strlen(s)-1] != '/')
	(void)strcat(s, "/");
    if (!strncmp(s, "../", 3) && (t = strrchr(dir, '/'))) {
	*t = NULL;
	if (t = strrchr(dir, '/'))
	    *(t+1) = NULL;
	s += 3;
    }
    if (*s == '/')
	(void)strcpy(dir, s);
    else
	(void)strcat(dir, s);
}


/*--------------------------------------------------------------------------
 * command$XDB - Enter an infinite loop so xdb can adopt this process.
 *--------------------------------------------------------------------------*/
BOOL cmdAdopt(unsigned argc, char *argv[])
{
    for (;;) {
	;	/* do nothing */
    }
    return YES;
}
#endif

void exitSim(int exitval)
{
    if (traceOutClose() == -1)
	cmdErr("Error when closing trace file\n");
    scrnEnd();
    exit(exitval);
}
