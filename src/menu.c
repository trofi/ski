/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Menu Routines
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

#include <string.h>
#include "std.h"
#include "types.h"
#include "ssDCmd.h"
#include "exportui.h"
#include "coreui.h"
#include "libsym.h"
#include "libsrs.h"
#include "libcore.h"	/* for setDerivedState, and get*TlbInfo */
#include "ski.h"
#include "tlb.h"
#include "traceui.h"

#define MENUTBLSIZ	200

/*##################### Local Variables ####################################*/

static unsigned topmenu;

static BOOL cmdHelp(unsigned argc, char *argv[]);
#if 0
static BOOL dummyCmd(unsigned argc, char *argv[]);
#endif
static BOOL cmdSLst(unsigned argc, char *argv[]);
static BOOL showInstTlb(unsigned argc, char *argv[]);
static BOOL showDataTlb(unsigned argc, char *argv[]);
static BOOL showPageTable(unsigned argc, char *argv[]);
static BOOL showGrMap(unsigned argc, char *argv[]);
static BOOL stateSave(unsigned argc, char *argv[]);
static BOOL cmdEscape(unsigned argc, char *argv[]);

/*##################### Globals - Imports ##################################*/

extern	int	scrn_lines;

#if 0
/* io.c */
BOOL cmdJnl(unsigned argc, char *argv[]);
#endif

#if 0
/* ssCch.c */
BOOL dcchShow(unsigned argc, char *argv[]);
BOOL icchShow(unsigned argc, char *argv[]);
#endif

/* ssDBT.c */
BOOL bptDisplay(unsigned argc, char *argv[]);
BOOL bptSet(unsigned argc, char *argv[]);
BOOL bptDelete(unsigned argc, char *argv[]);
BOOL bptDAll(unsigned argc, char *argv[]);
BOOL iAbptSet(unsigned argc, char *argv[]);
BOOL dbptDisplay(unsigned argc, char *argv[]);
BOOL dbptSet(unsigned argc, char *argv[]);
BOOL dbptDelete(unsigned argc, char *argv[]);
BOOL dbptDAll(unsigned argc, char *argv[]);
BOOL tbDisplay(unsigned argc, char *argv[]);

#if 0
/* ssDBrk.c */
BOOL ainfAssign(unsigned argc, char *argv[]);
BOOL aoutfAssign(unsigned argc, char *argv[]);
BOOL aerrfAssign(unsigned argc, char *argv[]);

/* ssDCMP.c */
BOOL cmdMacDef(unsigned argc, char *argv[]);
BOOL cmdMDel(unsigned argc, char *argv[]);
BOOL cmdMLst(unsigned argc, char *argv[]);
#endif

/* ssDCmd.c */
BOOL cmdSysEsc(unsigned argc, char *argv[]);
BOOL cmdComment(unsigned argc, char *argv[]);
BOOL cmdEval(unsigned argc, char *argv[]);

BOOL cmdExit(unsigned argc, char *argv[]);
BOOL cmdSource(unsigned argc, char *argv[]);
BOOL cmdGoto(unsigned argc, char *argv[]);

#if 0
BOOL cmdRedirect(unsigned argc, char *argv[]);
#endif
BOOL cmdLoad(unsigned argc, char *argv[]);
BOOL cmdIALoad(unsigned argc, char *argv[]);
BOOL cmdROMLoad(unsigned argc, char *argv[]);
BOOL cmdFWLoad(unsigned argc, char *argv[]);
#if 0
BOOL cmdDirSet(unsigned argc, char *argv[]);
BOOL cmdAdopt(unsigned argc, char *argv[]);
#endif
BOOL cmdIf(unsigned argc, char *argv[]);

/* ssDDM.c */
BOOL valAssign(unsigned argc, char *argv[]);
BOOL byteAssign(unsigned argc, char *argv[]);
BOOL hwAssign(unsigned argc, char *argv[]);
BOOL wordAssign(unsigned argc, char *argv[]);
BOOL dwordAssign(unsigned argc, char *argv[]);
BOOL stringAssign(unsigned argc, char *argv[]);
BOOL instrAssign(unsigned argc, char *argv[]);
BOOL templAssign(unsigned argc, char *argv[]);
BOOL dasmDump(unsigned argc, char *argv[]);
BOOL dataDump(unsigned argc, char *argv[]);

#if 0
BOOL grClear(unsigned argc, char *argv[]);
BOOL memDump(unsigned argc, char *argv[]);
BOOL spcGBds(unsigned argc, char *argv[]);
BOOL pagGProt(unsigned argc, char *argv[]);

/* ssDPE.c */
BOOL cmdInit(unsigned argc, char *argv[]);
BOOL runProg(unsigned argc, char *argv[]);
BOOL runallProg(unsigned argc, char *argv[]);
BOOL contProg(unsigned argc, char *argv[]);
BOOL contallProg(unsigned argc, char *argv[]);
BOOL stopProg(unsigned argc, char *argv[]);
BOOL stepallProg(unsigned argc, char *argv[]);
BOOL pstDump(unsigned argc, char *argv[]);
#endif

/* ssDSM.c */
BOOL prgwFwd(unsigned argc, char *argv[]);
BOOL prgwBkwd(unsigned argc, char *argv[]);
BOOL prgwJump(unsigned argc, char *argv[]);
BOOL prgwAsm(unsigned argc, char *argv[]);
BOOL prgwMixed(unsigned argc, char *argv[]);
BOOL datwExpr(unsigned argc, char *argv[]);
BOOL datwFwd(unsigned argc, char *argv[]);
BOOL datwBkwd(unsigned argc, char *argv[]);
BOOL datwJump(unsigned argc, char *argv[]);
BOOL datwHex(unsigned argc, char *argv[]);
BOOL datwBndl(unsigned argc, char *argv[]);
BOOL datwST(unsigned argc, char *argv[]);
BOOL datwAT(unsigned argc, char *argv[]);

/* ui.c */
BOOL regwFwd(unsigned argc, char *argv[]);
BOOL regwBkwd(unsigned argc, char *argv[]);
BOOL regwDump(unsigned argc, char *argv[]);
BOOL showUrs(unsigned argc, char *argv[]);
BOOL showSrs(unsigned argc, char *argv[]);
BOOL showGrs(unsigned argc, char *argv[]);
BOOL showFrs(unsigned argc, char *argv[]);
BOOL showIArs(unsigned argc, char *argv[]);

/* ssDSym.c */
BOOL cmdISymLst(unsigned argc, char *argv[]);
#if 0
BOOL cmdSymDef(unsigned argc, char *argv[]);
#endif
static BOOL cmdSDel(unsigned argc, char *argv[]);

#if 0
/* ssIO.c */
BOOL cmdIOLoadSom(unsigned argc, char *argv[]);
BOOL cmdIOLoadBinary(unsigned argc, char *argv[]);

/* ssReg.c */
BOOL cmdRegLst(unsigned argc, char *argv[]);

/* ssVAT.c */
BOOL dtlbShow(unsigned argc, char *argv[]);
BOOL itlbShow(unsigned argc, char *argv[]);

/* stack.c */
BOOL cmdStkTrc(unsigned argc, char *argv[]);
#endif

BOOL cstkDisplay(unsigned argc, char *argv[]);
BOOL showLoadModules(unsigned argc, char *argv[]);

static BOOL cmdItr_d(unsigned argc, char *argv[]);
static BOOL cmdItr_i(unsigned argc, char *argv[]);

/*##################### Globals - Exports ##################################*/

MENU cmdmenu[MENUTBLSIZ] = {
#if 1
/* comment out commands not yet implemented */
    {".",	1, 1, "source a file of simulator commands", cmdSource,
			"\t. <file>"},
    {"!",	1, MAXARGS, "execute external command", cmdEscape,
			"\t! <cmd>"},
    {"=",	2, 2, "assign value to register/internal symbol", valAssign,
			"\t= <register|isymbol> <value>"},
    {"=1",	2, MAXARGS, "assign byte(s) to memory location(s)", byteAssign,
			"\t=1 <address> {<value>}+"},
    {"=2",	2, MAXARGS, "assign halfword(s) to memory location(s)",
			hwAssign, "\t=2 <address> {<value>}+"},
    {"=4",	2, MAXARGS, "assign word(s) to memory location(s)", wordAssign,
			"\t=4 <address> {<value>}+"},
    {"=8",	2, MAXARGS, "assign dbl-word(s) to memory location(s)",
			dwordAssign, "\t=8 <address> {<value>}+"},
    {"=i",	2, MAXARGS, "assign instruction(s) to slot location(s)",
			instrAssign, "\t=i <address> {<assembly>}+"},
    {"=s",	2, MAXARGS, "assign a character string to memory location(s)",
			stringAssign, "\t=s <address> <string>"},
    {"=t",	2, MAXARGS, "assign template(s) to bundle location(s)",
			templAssign, "\t=t <address> {<template>}+"},
#if 0
    {"bs",	0, MAXARGS, "set IA-64 breakpoint", bptSet,
			"\tbs [ <address> [ <count> [ <cmd seq> ] ] ]"},
#else
    {"bs",	0, 1, "set IA-64 breakpoint", bptSet,
			"\tbs [ <address> ]"},
#endif
    {"bD",	0, 0, "delete all breakpoints", bptDAll,
			"\tbD"},
    {"bd",	1, 1, "delete breakpoint n", bptDelete,
			"\tbd <bpt num>"},
    {"bl",	0, 0, "list breakpoints", bptDisplay,
			"\tbl"},
    {"cont",	0, 0, "continue program", runProg, /* contProg, */
			"\tcont"},
    {"cstack",	0, 1, "display call stack", cstkDisplay,
			"\tcstack [ <file> ]"},
    {"dj",	0, 1, "jump to specified address in data window", datwJump,
			"\tdj [ <address> ]"},
    {"db",	0, 1, "move data window backward n lines", datwBkwd,
			"\tdb [ <count> ]"},
    {"dbndl",	0, 0, "display data window as instruction bundles", datwBndl,
			"\tdbndl"},
#if 0
    {"dbs",	2, MAXARGS, "set data breakpoint", dbptSet,
			"\tdbs <address> <length> [ { wo | rw }"
			" [ <count> [ <cmd seq> ] ] ]"},
#else
    {"dbs",	2, 3, "set data breakpoint", dbptSet,
			"\tdbs <address> <length> [ r | w | rw ]"},
#endif
    {"dbD",	0, 0, "delete all data breakpoints", dbptDAll,
			"\tdbD"},
    {"dbd",	1, 1, "delete data breakpoint n", dbptDelete,
			"\tdbd <bpt num>"},
    {"dbl",	0, 0, "list data breakpoints", dbptDisplay,
			"\tdbl"},
    {"dd",	2, 3, "dump memory contents", dataDump,
			"\tdd <start adr> <end adr> [ <file> ]"},
    {"de",	0, 1, "set expression for data window", datwExpr,
			/*"\tde [ <expr> ]"*/""},
    {"df",	0, 1, "move data window forward n lines", datwFwd,
			"\tdf [ <count> ]"},
    {"dh",	0, 0, "display data window in hex format", datwHex,
			"\tdh"},
    {"eval",	1, MAXARGS, "evaluate expression", cmdEval,
			"\teval <hex exp>+"},
    {"fr",	0, 0, "show FRs in register window", showFrs,
			"\tfr"},
    {"fwload",	1, 2, "load firmware image file", cmdFWLoad,
			"\tfwload <file> [ <mapfile> ]"},
    {"goto",	1, 1, "goto label", cmdGoto, "\tgoto <label>"},
    {"gr",	0, 0, "show GRs in register window", showGrs,
			"\tgr"},
#if 0
    {"grclr",	0, 0, "clear all general registers", dummyCmd, /* grClear, */
			"\tgrclr"},
#endif
    {"grmap",	0, 0, "display GR map", showGrMap, /*"\tgrmap"*/""},
    {"help",	0, 1, "display this command menu", cmdHelp,
			"\thelp [ <cmd> ]"},
    {"iar",	0, 0, "show IA regs in register window", showIArs,
			"\tiar"},
#if 0
    {"iabs",	0, MAXARGS, "set IA-32 breakpoint", iAbptSet,
			"\tiabs [ <address> [ <count> [ <cmd seq> ] ] ]"},
#else
    {"iabs",	0, 1, "set IA-32 breakpoint", iAbptSet,
			"\tiabs [ <address> ]"},
#endif
    {"iaload",	2, MAXARGS, "load IA executable file", cmdIALoad,
			"\tiaload <file> <address> [ <mapfile> [ <args> ] ]"},
    {"if",	2, MAXARGS, "conditionally execute rest of line", cmdIf,
			"\tif <hex exp> true-cmd [: false-cmd]"},
#if 0
    {"init",	0, 0, "initialize simulator state", dummyCmd, /* cmdInit, */
			"\tinit"},
#endif
    {"isyms",	0, 1, "list internal symbols", cmdISymLst,
			"\tisyms [ <file> ]"},
    {"itr.d",	4, 4, "", cmdItr_d,
			"\titr.d slot ifa tpa itir"},
    {"itr.i",	4, 4, "", cmdItr_i,
			"\titr.i slot ifa tpa itir"},
    {"load",	1, MAXARGS, "load executable file", cmdLoad,
			"\tload <file> [ <args> ]"},
    {"pj",	0, 1, "jump to specified address in program window", prgwJump,
			"\tpj [ <address> ]"},
    {"pa",	0, 1, "display program window in assembly only", prgwAsm,
			"\tpa"},
    {"pb",	0, 1, "move program window backward n bundles", prgwBkwd,
			"\tpb [ <count> ]"},
    {"pd",	2, 3, "dump memory disassembled as instructions", dasmDump,
			"\tpd <start adr> <end adr> [ <file> ]"},
    {"pf",	0, 1, "move program window forward n bundles", prgwFwd,
			"\tpf [ <count> ]"},
    {"pm",	0, 1, "display program window in both source and assembly",
			prgwMixed, "\tpm"},
    {"quit",	0, 1, "quit (?!)", cmdExit,
			"\tquit [ <ret value>"},
#if 0
    {"run",	0, MAXARGS, "run program on current processor", runProg,
			"\trun [ <arglist> ]"},
#else
    {"run",	0, 0, "run program", runProg,
			"\trun"},
#endif
    {"rb",	0, 1, "move register window backward n lines", regwBkwd,
			"\trb [ <count> ]"},
    {"rd",	0, 1, "dump register contents", regwDump,
			"\trd [ <file> ]"},
    {"rest",	1, 1, "restore simulator state from a file", stateRest,
			"\trest <file>"},
    {"rf",	0, 1, "move register window forward n lines", regwFwd,
			"\trf [ <count> ]"},
    {"romload",	2, 3, "load ROM image file", cmdROMLoad,
			"\tromload <file> <address> [ <mapfile> ]"},
#if 0
    {"step",	0, MAXARGS, "execute single (or n) instruction(s)", stepProg,
			"\tstep [ <count> [ <cmd seq> ] ]"},
#else
    {"step",	0, 2, "execute <count> instruction(s) or until <expr> nonzero",
			stepProg,
			"\tstep [ <count> | until <expr> ]"},
#endif
    {"save",	1, 1, "save simulator state in a file", stateSave,
			"\tsave <file>"},
    {"sdt",	0, 1, "show data TLB", showDataTlb, "\tsdt [ <file> ]"},
    {"sit",	0, 1, "show instruction TLB", showInstTlb, "\tsit [ <file> ]"},
    {"slm",	0, 1, "show load modules", showLoadModules, "\tslm [ <file> ]"},
    {"spt",	0, 1, "show page table", showPageTable,
			/*"\tspt [ <file> ]"*/""},
    {"sr",	0, 0, "show System Registers (CRs/RRs/PKRs/DBRs/IBRs/PMCs/PMDs)"
			" in reg window", showSrs, "\tsr"},
#if 0
    {"stop",	0, 0, "stop execution of program", dummyCmd, /* stopProg, */
			"\tstop"},
#endif
    {"symlist",	0, 1, "list symbols", cmdSLst,
			"\tsymlist [ <file> ]"},
    {"symdel",	2, 2, "delete symbols in address range", cmdSDel,
			"\tsymdel <low> <high>"},
    {"trace",	1, 1, "enable or disable tracing", traceGen,
			"\ttrace <bool expr>\n"},
    {"trfile",	1, 2, "specify trace file", traceFile,
			"\ttrfile <file> [w|a]\n"},
    {"ur",	0, 0, "show User Registers (PRs/BRs/ARs) in register window",
			showUrs, "\tur"},
    {"", 0, 0, "", 0, ""}
#else
    {"=i",	"assign symbolic instruction to address", cmdInst},
    {"adopt",	"prepare to be adopted by a debugger", cmdAdopt},
    {"contall",	"continue all programs", contallProg},
    {"cd",	"get indirect files from given directory", cmdDirSet},
    {"initall",	"initialize all processors state", NULL},
    {"ioldbin",	"load binary file to IO address", cmdIOLoadBinary},
    {"ioldsom",	"load object file to IO address", cmdIOLoadSom},
    {"jnl",	"open journal file", cmdJnl},
    {"macro",	"define macro", cmdMacDef},
    {"macdel",	"delete (pop) macro definition", cmdMDel},
    {"maclist",	"list macros", cmdMLst},
    {"page",	"create/change access protection for page", pagGProt},
    {"runall",	"run program on all processors", runallProg},
    {"reglist",	"list registers to file", cmdRegLst},
    {"stepall",	"exec. one (or n) inst(s) on all processors", stepallProg},
    {"sdc",	"show data cache", dcchShow},
    {"sic",	"show instruction cache", icchShow},
    {"space",	"create/change bounds/protect of specified space", spcGBds},
    {"stats",	"print statistics for most recent run", pstDump},
    {"symbol",	"define symbol", cmdSymDef},
    {"symdel",	"delete (pop) symbol definition", cmdSDel},
    {"tblist",	"list last few taken branches", tbDisplay},
    {"!",	"pass command string to system", cmdSysEsc},
    {"<",	"take application input from file", ainfAssign},
    {">",	"write application output to file", aoutfAssign},
    {"2>",	"write application error output to file", aerrfAssign},
    {0, 0, 0}
#endif
};


/*##################### Functions and Subroutines ##########################*/

/*--------------------------------------------------------------------------
 * menu$Initialize - Set the value of topmenu, the current size of the
 *  command menu table.  This value will be modified by the implementation-
 *  dependent routines that adds new commands to the table.
 *--------------------------------------------------------------------------*/
void menuInit(void)
{
    MENU *menu;

    topmenu = 0;
    for (menu = cmdmenu; strcmp(menu->keywd, ""); menu++)
	topmenu++;
}

/*---------------------------------------------------------------------------
 * menu$Insert - Insert a command to the command menu.
 *---------------------------------------------------------------------------*/
BOOL menuIns(const char *keywd, int minargs, int maxargs, const char *descrip,
	     PFV fcn, const char *format)
{
    unsigned i;

    if (strlen(keywd) >= KEYWDSIZ || strlen(descrip) >= DESCRSIZ  ||
	strlen(format) >= DESCRSIZ) {
	cmdWarn("Command name and/or description too long: %s.  Ignored\n",
		keywd);
	return NO;
    }
    if (topmenu == MENUTBLSIZ) {
	cmdWarn("Command table overflow.  Commands beginning from %s"
		" are ignored\n", keywd);
	return NO;
    }
    for (i = 0; i < topmenu; i++)
	if (!strcmp(keywd, cmdmenu[i].keywd)) {
	    cmdWarn("Command (%s) already in table.  Ignored\n", keywd);
	    return NO;
	}
    (void)strcpy(cmdmenu[topmenu].keywd,   keywd);
    (void)strcpy(cmdmenu[topmenu].descrip, descrip);
    (void)strcpy(cmdmenu[topmenu].format,  format);
    cmdmenu[topmenu].minargs = minargs;
    cmdmenu[topmenu].maxargs = maxargs;
    cmdmenu[topmenu].fcn     = fcn;
    topmenu++;
    return YES;
}

/*--------------------------------------------------------------------------
 * menu$Lookup - Looks up a command string among the keywords of a menu.
 *  Takes a string to lookup and finds the first entry for which the passed
 *  string is a prefix of the keyword and returns a pointer to its dispatch
 *  function.  Returns NULL if no match is found.
 *--------------------------------------------------------------------------*/
MENU *menuLkp(char *s)
{
    MENU *menu;

    for (menu = cmdmenu; menu < &cmdmenu[topmenu]; menu++)
	if ((void *)menu->keywd == (void *)strstr(menu->keywd, s))
	    return menu;
    return NULL;
}

/*--------------------------------------------------------------------------
 * command$Help - Displays a list of menu entry keywords and descriptions
 *  for the passed menu.
 *--------------------------------------------------------------------------*/
static BOOL cmdHelp(unsigned argc, char *argv[])
{
    MENU *menu;

    if (!argc) {
	unsigned i;
	static char helpbuf[6000];
	char *p = helpbuf;

	for (i = 0; i < topmenu; i++) {
	    menu = &cmdmenu[i];
	    if (!menu->format[0])
		continue;
	    p += sprintf(p, "%-8s %s\n", menu->keywd, menu->descrip);
	}
	cmdOut("cmdhelp", "Command  Description\n", helpbuf, NULL);
    } else {
	if ((menu = menuLkp(argv[0])) && menu->format[0]) {
	    msgwPrint("%s\t%s\n", menu->keywd, menu->descrip);
	    msgwPrint("format:%s\n", menu->format);
	} else
	    msgwPrint("help: Unknown command: %s\n", argv[0]);
    }
    return YES;
}

#if 0
static BOOL dummyCmd(unsigned argc, char *argv[])
{
    cmdwPrint("%s: Command not yet implemented\n", argv[0]);
    return YES;
}
#endif

/*--------------------------------------------------------------------------
 * command$Symbol$List - Displays the list of the simulator internal symbols
 *  (e.g., registers and bit fields).
 *--------------------------------------------------------------------------*/
static BOOL cmdSLst(unsigned argc, char *argv[])
{
    Symbol sym;
    size_t bufsz = 0;
    FILE *f = NULL;
    char *p, *symbuf;

    symNextSymInit();
    while (symNextSym(&sym))
	bufsz += 16 + 1 + strlen(sym.name) + 1;
    if (!bufsz) {
	cmdWarn("No symbols to list\n");
	return YES;
    }
    if (!(p = symbuf = malloc(bufsz+1))) {
	cmdErr("Could not malloc space for symbol list\n");
	return NO;
    }
    *p = '\0';
    symNextSymInit();
    while (symNextSym(&sym))
	p += sprintf(p, "%"RFT" %s\n", sym.addr, sym.name);
    if (argc == 1 && !(f = cmdOpenFile(argv[0], "w"))) {
	free(symbuf);
	return NO;
    }
    cmdOut("symlist", "Value            Name\n", symbuf, f);
    free(symbuf);
    return YES;
}

static BOOL cmdSDel(unsigned argc, char *argv[])
{
    ADDR sa, ea;

    if (!evalExpr(argv[0], HEXEXP, &sa))
	return NO;
    if (!evalExpr(argv[1], HEXEXP, &ea))
	return NO;
    symDeleteAddr(sa, ea);
    return YES;
}

/*--------------------------------------------------------------------------
 * showInstTlb - Display the contents of the ITLB.
 *--------------------------------------------------------------------------*/
static BOOL showInstTlb(unsigned argc, char *argv[])
{
    size_t bufsz = instTlbDisplaySize();
    FILE *f = NULL;
    char *buf, hdr[81];

    if (!bufsz) {
	cmdWarn("No I-TLB to list\n");
	return YES;
    }
    if (!(buf = malloc(bufsz+1))) {
	cmdErr("Could not malloc space for itlb list\n");
	return NO;
    }
    getInstTlbInfo(hdr, sizeof (hdr), buf, bufsz+1);
    if (argc == 1 && !(f = cmdOpenFile(argv[0], "w"))) {
	free(buf);
	return NO;
    }
    cmdOut("itlblist", hdr, buf, f);
    free(buf);
    return YES;
}

/*--------------------------------------------------------------------------
 * showDataTlb - Display the contents of the DTLB.
 *--------------------------------------------------------------------------*/
static BOOL showDataTlb(unsigned argc, char *argv[])
{
    size_t bufsz = dataTlbDisplaySize();
    FILE *f = NULL;
    char *buf, hdr[81];

    if (!bufsz) {
	cmdWarn("No D-TLB to list\n");
	return YES;
    }
    if (!(buf = malloc(bufsz+1))) {
	cmdErr("Could not malloc space for dtlb list\n");
	return NO;
    }
    getDataTlbInfo(hdr, sizeof (hdr), buf, bufsz+1);
    if (argc == 1 && !(f = cmdOpenFile(argv[0], "w"))) {
	free(buf);
	return NO;
    }
    cmdOut("dtlblist", hdr, buf, f);
    free(buf);
    return YES;
}

/*--------------------------------------------------------------------------
 * showPageTable - Display the contents of the page table.
 *--------------------------------------------------------------------------*/
static BOOL showPageTable(unsigned argc, char *argv[])
{
    size_t bufsz = pageTableDisplaySize();
    FILE *f = NULL;
    char *buf;

    if (!bufsz) {
	cmdWarn("No page table entries\n");
	return YES;
    }
    if (!(buf = malloc(bufsz+1))) {
	cmdErr("Could not malloc space for page table list\n");
	return NO;
    }
    getPageTableInfo(buf);
    if (argc == 1 && !(f = cmdOpenFile(argv[0], "w"))) {
	free(buf);
	return NO;
    }
    cmdOut("pagetable", "Page Table Addr\n", buf, f);
    free(buf);
    return YES;
}

/*--------------------------------------------------------------------------
 * showGrMap - Display the contents of the GR mapping table.
 *--------------------------------------------------------------------------*/
static BOOL showGrMap(unsigned argc, char *argv[])
{
    static char buf[8192];

    getGrMapInfo(buf);
    cmdOut("grmap", "  V   P\n", buf, NULL);
    return YES;
}

/*--------------------------------------------------------------------------
 * stateSave - Save the current simulator state into a file.
 *--------------------------------------------------------------------------*/
static BOOL stateSave(unsigned argc, char *argv[])
{
    BOOL ret;

    /* XXX - tilde-expand argv[0] here */

    cmdwSetStatus("Saving State...");
    if (!(ret = srs_saveState(argv[0])))
	cmdErr("Save state error: %s\n", srs_errmsgGet());
    cmdwSetStatus("");
    return ret;
}

/*--------------------------------------------------------------------------
 * state$Restore - Restore the simulator state from a file.
 *--------------------------------------------------------------------------*/
BOOL stateRest(unsigned argc, char *argv[])
{
    BOOL ret;

    /* XXX - tilde-expand argv[0] here */

    cmdwSetStatus("Restoring State...");
    if ((ret = srs_restoreState(argv[0]))) {
	/* XXX - for MP, need to do this on each processor */
	setDerivedState(0);
	scrnUpdate();
    } else
	cmdErr("Restore state error: %s\n", srs_errmsgGet());
    cmdwSetStatus("");
    return ret;
}

static BOOL cmdEscape(unsigned argc, char *argv[])
{
    FILE *pd;
    char line[200], buf[32768], cmd[100];

    catArgs(0, argc-1, argv, cmd);
    pd = popen(cmd+1, "r");
    buf[0] = '\0';
    while (fgets(line, sizeof line, pd))
	(void)strcat(buf, line);
    cmdOut("escape", "", buf, NULL);
    (void)pclose(pd);
    return YES;
}

static BOOL cmdItr_d(unsigned argc, char *argv[])
{
    DWORD slot;
    ADDR tva, tpa, tar;

    if (!evalExpr(argv[0], DECEXP, &slot))
	return NO;
    if (!evalExpr(argv[1], HEXEXP, &tva))
	return NO;
    if (!evalExpr(argv[2], HEXEXP, &tpa))
	return NO;
    if (!evalExpr(argv[3], HEXEXP, &tar))
	return NO;
#if 0
    if (slot >= NDTRS)
	return NO;
#endif
    uiDtrInsert(slot, tva, tpa, tar);
    return YES;
}

static BOOL cmdItr_i(unsigned argc, char *argv[])
{
    DWORD slot;
    ADDR tva, tpa, tar;

    if (!evalExpr(argv[0], DECEXP, &slot))
	return NO;
    if (!evalExpr(argv[1], HEXEXP, &tva))
	return NO;
    if (!evalExpr(argv[2], HEXEXP, &tpa))
	return NO;
    if (!evalExpr(argv[3], HEXEXP, &tar))
	return NO;
#if 0
    if (slot >= NITRS)
	return NO;
#endif
    uiItrInsert(slot, tva, tpa, tar);
    return YES;
}
