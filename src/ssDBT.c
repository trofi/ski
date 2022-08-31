/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Debugger Breakpoint, Instruction Trace, and Call Stack
 * Routines
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

#include "ssDBT.h"

#include "bits.h"
#include "types.h"
#include "fields.h"
#include "state.h"
#include "libcore.h"
#include "sim.h"
#include "ssDCmd.h"
#include "libsym.h"
#include "memui.h"
#include "simmem.h"
#include "ssDPrs.h"
#include "tlb.h"
#include "ui.h"

#define NDBPTS		10
#define NBPTS		10
#define TBTBLSZ		20	/* taken branch table size */

#define IAINT3		0xcc

/*##################### Datatypes ##########################################*/

typedef enum { EMbpt, IAbpt } BptType;

/* Breakpoint structure */
struct bpt {
    BOOL loaded;	/* has the breakpoint been loaded? */
    BOOL inuse;
    BOOL realmode; /* if break was set while in real mode */
    ADDR adr;
    ADDR padr;		/* phys address at which bpt has been installed */
    BptType type;
    union {
	struct {
	    DWORD slot;
	} em;
	struct {
	    BYTE op;
	} ia;
    } u;
    WORD count;
    char cmd[134];
};

/* Data breakpoint structure */
struct dbpt {
    BOOL inuse;
    BOOL realmode; /* if break was set while in real mode */
    BOOL loaded;
    ADDR start;
    ADDR padr;
    REG len, access, count;
    char cmd[134];
};

#ifdef TAKEN_BRANCH
/* taken branch table structure */
struct tb {
    ADDR tgtofs;
};
#endif

/*##################### Globals - Exports ##################################*/

static BOOL	dbptsSet = NO;	/* Are any data breakpoints currently set? */

#ifdef TAKEN_BRANCH
BOOL	takbrTrap = NO;	/* enable branch tabling */
#endif
int     numBptsToLoad;

/*##################### Local Variables ####################################*/

static	struct bpt bpts[NBPTS] = {{0}};
static	struct dbpt dbpts[NDBPTS] = {{0}};
static	BOOL	bptsSet = NO;	/* Are any breakpoints currently set? */

#ifdef TAKEN_BRANCH
static	struct tb	tbTbl[TBTBLSZ] = {0};
static	unsigned	tbCount = 0;
static	struct tb	*tbPtr = tbTbl;
#endif

/*#####################  Macros  ###########################################*/

/*##################### Functions and Subroutines ##########################*/

/***********************/
/* Breakpoint Routines */
/***********************/

/*--------------------------------------------------------------------------
 * breakpoint Display - Display list of breakpoints in use.
 *--------------------------------------------------------------------------*/
BOOL bptDisplay(unsigned argc, char *argv[])
{
    int i;
    char sym[20];
    static char bptlist[NBPTS * 80];
    char *p = bptlist;

    if (!bptsSet) {
	cmdWarn("No breakpoints set\n");
	return YES;
    }
    for (i = 0; i < NBPTS; i++) {
	if (!bpts[i].inuse)
	    continue;
	p += sprintf(p, "   %d  %c ", i, bpts[i].realmode ? 'P' : 'V');
	symAddrtoName(bpts[i].adr, sym, 4, NO, 16);
	p += sprintf(p, "%s", sym);
	p += sprintf(p, "  %s", bpts[i].type == IAbpt ? "IA-32" : "IA-64");
	p += sprintf(p, "  %-.14s\n", bpts[i].cmd);
    }
    cmdOut("breakpoints", "   #    Address                   Command\n",
	   bptlist, NULL);
    return YES;
}

/*--------------------------------------------------------------------------
 * is Breakpoint - If a breakpoint is set at the passed address
 *  return the number of the first such breakpoint, else return -1.
 *--------------------------------------------------------------------------*/

int isbpt(ADDR adr)
{
    struct bpt *p;
    unsigned i;
    REG psrval = psrGet(0);
    BOOL psrit_sve = X_PSR_IT(psrval);

    if (!bptsSet)
	return -1;
    if (!instTranslate(adr, &adr))
	return -1;
    for (p = bpts, i = 0; i < NBPTS; p++, i++) {
	ADDR bptadr;

	if (!p->inuse)
	    continue;
	X_PSR_IT(psrval) = !p->realmode;
	(void)psrSet(0, psrval);
	if (!instTranslate(p->adr, &bptadr))
	   continue;
	if (bptadr == (p->type == EMbpt ? adr & ~3 : adr))
	    break;
    }
    X_PSR_IT(psrval) = psrit_sve;
    (void)psrSet(0, psrval);
    return i == NBPTS ? -1 : i;
}

/*--------------------------------------------------------------------------
 * breakpoint$GetNextAvailable - Return number of lowest numbered breakpoint
 *  not currently in use after marking it as in use or -1 if none are
 *  currently available.
 *--------------------------------------------------------------------------*/
static int bptGNA(void)
{
    int i;

    for (i = 0; i < NBPTS; i++)
	if (bpts[i].inuse == NO) {
	    bpts[i].inuse = YES;
	    return i;
	}
    return -1;
}

static BOOL setBreakpoint(BptType type, unsigned argc, char *argv[])
{
    int b;
    ADDR adr = ipGet(0);	/* default address is current ip */
    BOOL otherinfo = NO;
    REG count = 0, psrval;
    char buf[140];

    if (argc >= 1)
	if (!evalExpr(argv[0], HEXEXP, &adr))
	    return NO;
    if (argc >= 2)
	if (!evalExpr(argv[1], DECEXP, &count))
	    return NO;
    buf[0] = '\0';
    if (type == EMbpt) {
	adr &= ~(ADDR)0x3;
	if (SLOT(adr) > 2) {
	    cmdErr("Illegal slot field in breakpoint address\n");
	    return NO;
	}
    }
    if ((b = isbpt(adr)) != -1)  {
	if (!otherinfo)
	    cmdWarn("Breakpoint already set at that location\n");
    } else {
	if ((b = bptGNA()) == -1) {
	    cmdErr("All breakpoints in use\n");
	    return NO;
	}
	bpts[b].adr = adr;
    }
    /* XXX - need to add check for unallocated memory */
    if (argc >= 3) {
	catArgs(2, argc-1, argv, buf);
	(void)strcpy(bpts[b].cmd, buf);
    }
    bpts[b].count = count;
    bpts[b].type = type;
    psrval = psrGet(0);
    bpts[b].realmode = !X_PSR_IT(psrval);
    bptsSet = YES;
    prgwUpdate();
    return YES;
}

/*--------------------------------------------------------------------------
 * breakpoint Set - Set an EM breakpoint, at a given virtual address.
 *  Default is current ip.
 *--------------------------------------------------------------------------*/
BOOL bptSet(unsigned argc, char *argv[])
{
    return setBreakpoint(EMbpt, argc, argv);
}

/*--------------------------------------------------------------------------
 * IA breakpoint Set - Set an IA breakpoint, at a given virtual address.
 *  Default is current ip.
 *--------------------------------------------------------------------------*/
BOOL iAbptSet(unsigned argc, char *argv[])
{
    return setBreakpoint(IAbpt, argc, argv);
}

/*--------------------------------------------------------------------------
 * breakpoint$Delete - Prompt for a breakpoint number, then clear that
 *  breakpoint.
 *--------------------------------------------------------------------------*/
BOOL bptDelete(unsigned argc, char *argv[])
{
    REG b;
    char tmp1[256];

    if (!bptsSet) {
	cmdWarn("No breakpoints set\n");
	return YES;
    }
    if (argc == 1) {
	if (!evalExpr(argv[0], DECEXP, &b))
	    return NO;
	if (b >= NBPTS) {
	    cmdErr("Bad breakpoint number. (Use 0-9)\n");
	    return NO;
	} else if (!bpts[b].inuse) {
	    cmdWarn("Breakpoint #%d wasn't set\n", (int)b);
	    return NO;
	} else {
	    bpts[b].inuse = bptsSet = NO;
	    symAddrtoName(bpts[b].adr, tmp1, 4, NO, -16);
	    msgwPrint("Breakpoint #%d at %s (%s) deleted\n",
		      (int)b, tmp1, bpts[b].type == IAbpt ? "IA-32" : "IA-64");
	    for (b = 0; b < NBPTS; b++)
		if (bpts[b].inuse)
		    bptsSet = YES;
	    prgwUpdate();
	    return YES;
	}
    }
    return YES;
}

/*--------------------------------------------------------------------------
 * breakpoint$DeleteAll - Delete all breakpoints.
 *--------------------------------------------------------------------------*/
BOOL bptDAll(unsigned argc, char *argv[])
{
    int b;

    if (!bptsSet) {
	cmdWarn("No breakpoints set\n");
	return YES;
    }
    for (b = 0; b < NBPTS; b++)
	bpts[b].inuse = NO;
    bptsSet = NO;
    prgwUpdate();
    msgwPrint("All breakpoints deleted\n");
    return YES;
}

/*--------------------------------------------------------------------------
 * breakpoint Load - Replace the instruction at each of the "inuse"
 *  breakpoints with a BREAK instruction and update predecode cache.
 *--------------------------------------------------------------------------*/
static int ibptLoad(void)
{
    struct bpt *p;
    REG psrval = psrGet(0);
    BOOL psrit_sve = X_PSR_IT(psrval);
    int to_load = 0;

    for (p = bpts; p < &bpts[NBPTS]; p++) {
	if (!p->inuse || p->loaded)
	    continue;
	X_PSR_IT(psrval) = !p->realmode;
	(void)psrSet(0, psrval);
	if (p->type == EMbpt) {
	    Bundle bndl;
	    ADDR badr = p->adr & ~(ADDR)0xf;
 	    if (!memMIRd(badr, (DWORD *)&bndl))	{ /* try to read bundle */
 		++to_load;
		continue;
	    }
	    bptReplace(&bndl, &p->adr, &p->u.em.slot);
	    (void)memMIWrt(badr, (DWORD *)&bndl);	/* write back */
	    itlbMLookup(p->adr, X_PSR_IT(psrval), &p->padr);
	} else {	/* IAbpt */
	    static BYTE int3 = IAINT3;

	    if (!memMIAIRd(p->adr, &p->u.ia.op, 1))
		continue;
	    (void)memMIAIWrt(p->adr, &int3, 1);
	    iabptReplace(p->adr);
	}
	p->loaded = YES;
    }
    X_PSR_IT(psrval) = psrit_sve;
    (void)psrSet(0, psrval);
    return to_load;
}

/*--------------------------------------------------------------------------
 * breakpoint Unload - Replace all breakpoints with the instructions that
 *  originally were there and update predecode cache.
 *--------------------------------------------------------------------------*/
static void ibptUnload(void)
{
    struct bpt *p;
    REG psrval = psrGet(0);
    BOOL psrit_sve = X_PSR_IT(psrval);

    if (!bptsSet)
	return;

    X_PSR_IT(psrval) = 0;
    (void)psrSet(0, psrval);
    for (p = bpts; p < &bpts[NBPTS]; p++) {
	if (!p->inuse || !p->loaded)
	    continue;
	p->loaded = 0;
	if (p->type == EMbpt) {
	    Bundle bndl;
	    ADDR badr = p->padr & ~(ADDR)0xf;

	    if (!memMIRd(badr, (DWORD *)&bndl))
		continue;
	    bptRestore(&bndl, p->padr, p->u.em.slot);
	    (void)memMIWrt(badr, (DWORD *)&bndl);
	} else {	/* IAbpt */
	    (void)memMIAIWrt(p->adr, &p->u.ia.op, 1);	/* restore opcode */
	    iabptRestore(p->adr);
	}
    }
    X_PSR_IT(psrval) = psrit_sve;
    (void)psrSet(0, psrval);
}

/****************************/
/* Data Breakpoint Routines */
/****************************/

/*--------------------------------------------------------------------------
 * dataBreakpoint$Display - Display list of breakpoints in use.
 *--------------------------------------------------------------------------*/
BOOL dbptDisplay(unsigned argc, char *argv[])
{
    int i;
    char sym[20];
    static char dbptlist[NDBPTS * 80];
    static const char accstr[][3] = { "??", "R ", "W ", "RW" };
    char *p = dbptlist;

    if (!dbptsSet) {
	cmdWarn("No data breakpoints set\n");
	return YES;
    }
    for (i = 0; i < NDBPTS; i++) {
	if (!dbpts[i].inuse)
	    continue;
	p += sprintf(p, "   %d   %s  ", i, accstr[dbpts[i].access]);
	symAddrtoName(dbpts[i].start, sym, 4, NO, 16);
	p += sprintf(p, "%c ", dbpts[i].realmode ? 'P' : 'V');
	p += sprintf(p, "%s  %llx\n", sym, dbpts[i].len);
    }
    cmdOut("databpts", "   #  Type       Address      Length\n", dbptlist,
	   NULL);
    return YES;
}


/*--------------------------------------------------------------------------
 * dataBreakpoint$GetNextAvailable - Return number of lowest numbered data
 *  break not currently in use after marking it as in use or -1 if none
 *  are currently available.
 *--------------------------------------------------------------------------*/
static int dbptGNA(void)
{
    int i;

    for (i = 0; i < NDBPTS; i++)
	if (!dbpts[i].inuse) {
	    dbpts[i].inuse = YES;
	    return i;
	}
    return -1;
}


/*--------------------------------------------------------------------------
 * dataBreakpoint$Set - Set a data breakpoint.
 *--------------------------------------------------------------------------*/
BOOL dbptSet(unsigned argc, char *argv[])
{
    REG psrval = psrGet(0);
    int b;

    if ((b = dbptGNA()) == -1) {
	cmdErr("All data breakpoints in use\n");
	return YES;
    }
    if (!evalExpr(argv[0], HEXEXP, &dbpts[b].start))
	goto fail;
    if (!evalExpr(argv[1], DECEXP, &dbpts[b].len))
	goto fail;
    if (page_base(dbpts[b].start)
	!= page_base(dbpts[b].start + dbpts[b].len - 1))
    {
	cmdErr("Data breakpoint may not cross %lu-byte page boundary\n",
	       page_size);
	goto fail;
    }
    if (argc > 2) {
	if (!strcmp(argv[2], "r"))
	    dbpts[b].access = 1;
	else if (!strcmp(argv[2], "rw"))
	    dbpts[b].access = 3;
	else if (!strcmp(argv[2], "w"))
	    dbpts[b].access = 2;
	else {
	    cmdErr("Bad access type\n");
	    goto fail;
	}
    } else
	dbpts[b].access = 3;
    dbpts[b].realmode = !X_PSR_DT(psrval);
    dbpts[b].loaded = NO;
    dbptsSet = YES;
    return YES;

  fail:
    dbpts[b].inuse = NO;
    return NO;
}


/*--------------------------------------------------------------------------
 * dataBreakpoint$Delete - Prompt for a data breakpoint number, then clear
 *  that data breakpoint.
 *--------------------------------------------------------------------------*/
BOOL dbptDelete(unsigned argc, char *argv[])
{
    REG b;

    if (!dbptsSet) {
	cmdWarn("No data breakpoints set\n");
	return YES;
    }
    if (argc == 1) {
	if (!evalExpr(argv[0], DECEXP, &b))
	    return NO;
	if (b >= NDBPTS)
	    cmdErr("Bad data breakpoint number. (Use 0-%d)\n", NDBPTS-1);
	else if (!dbpts[b].inuse)
	    cmdWarn("Data breakpoint #%d wasn't set\n", (int)b);
	else
	    dbpts[b].inuse = dbpts[b].loaded = NO;
    }
    dbptsSet = NO;
    for (b = 0; b < NDBPTS; b++)
	if (dbpts[b].inuse)
	    dbptsSet = YES;
    return YES;
}


/*--------------------------------------------------------------------------
 * dataBreakpoint$DeleteAll - Delete all data breakpoints.
 *--------------------------------------------------------------------------*/
BOOL dbptDAll(unsigned argc, char *argv[])
{
    int b;

    if (!dbptsSet) {
	cmdWarn("No data breakpoints set\n");
	return YES;
    }
    for (b = 0; b < NDBPTS; b++)
	dbpts[b].inuse = dbpts[b].loaded = NO;
    dbptsSet = NO;
    msgwPrint("All data breakpoints deleted\n");
    return YES;
}

static int dbptLoad(void)
{
    int to_load = 0;
    struct dbpt *p;

    for (p = dbpts; p < &dbpts[NDBPTS]; p++) {
	if (!p->inuse || p->loaded)
	    continue;

	if (abi || p->realmode)
	    p->padr = p->start;
	else if (dbptLookup(p->start, &p->padr) < 0) {
	    ++to_load;	/* no translation yet; load it later */
	    continue;
	}
	p->loaded = YES;
	memSetDbptBit(p->padr);
    }
    return to_load;
}

static void dbptUnload(void)
{
    int b;

    for (b = 0; b < NDBPTS; b++) {
      if (!dbpts[b].loaded)
	continue;
      memClrDbptBit(dbpts[b].padr);
      dbpts[b].loaded = NO;
    }
}

/*--------------------------------------------------------------------------
 * dataBreakpoint$Check - Find the data breakpoint with the passed address,
 *  access type (1 == read, 2 == write, 3 == read/write), and data length.
 *  If any part of the passed datatype lies within the range of one of the
 *  data breakpoints, has the correct matching access type, and the associated
 *  count is zero or decrements to zero, return YES.  Else return NO after
 *  executing the associated command if a match occurs but the counter is
 *  nonzero.  (count down is not yet implemented).
 *--------------------------------------------------------------------------*/
BOOL dbptCheck(ADDR adr, unsigned acctyp, unsigned dlen)
{
    struct dbpt *p;
    ADDR start, end, adrstart, adrend;

    for (p = dbpts; p < &dbpts[NDBPTS]; p++) {
	if (p->loaded && (p->access & acctyp)) {
	    start = p->padr; end = start + p->len - 1;
	    adrstart = adr;
	    adrend = adrstart + dlen - 1;
	    /* XXX - can adr be assumed to be aligned ? */
	    if (end < adrstart || start > adrend)
		continue;
	    /* found it */
	    return YES;
	}
    }
    return NO;
}

#ifdef TAKEN_BRANCH
/*************************/
/* Taken branch routines */
/*************************/

/*---------------------------------------------------------------------------
 * takenBranch$Record - Record in a table the occurrence of the branch target.
 *---------------------------------------------------------------------------*/
void tbRecord(void)
{
    if (tbCount < TBTBLSZ)
	tbCount++;
    tbPtr->tgtofs = ipGet(0);
    if (++tbPtr == &tbTbl[TBTBLSZ])
	tbPtr = tbTbl;
}


/*---------------------------------------------------------------------------
 * takenBranch$Display - Display the last tbCount taken branches on the
 *  screen.
 *---------------------------------------------------------------------------*/
BOOL tbDisplay(unsigned argc, char *argv[])
{
    int i;
    struct tb *tbp;
    static char tblist[BUFSIZ];
    char *p = tblist;

    if (!PswT() && !takbrTrap) {
	cmdErr("Branch tabling not enabled\n");
	return YES;
    } else if (!tbCount) {
	cmdErr("No branches recorded\n");
	return YES;
    }
    p += sprintf(p, "\t--- Last %d taken branches ---\n", tbCount);
    p += sprintf(p, "   Branch target address\n");
    p += sprintf(p, "   ---------------------\n");
    tbp = tbCount < TBTBLSZ ? tbTbl : tbPtr;
    for (i = 0; i < tbCount; i++) {
	p += sprintf(p, "   %"RFT"\n", tbp->tgtofs);
	if (++tbp == &tbTbl[TBTBLSZ])
	    tbp = tbTbl;
    }
    cmdOut(tblist);
    return YES;
}
#endif

void bptLoad(void)
{
    int to_load = 0;

    /*
     * For the case where bptLoad() gets called from tlbInsert(), we
     * need to clear numBptsToLoad first to avoid the infinite
     * recursion that would arise if memMIRd() in ibptLoad() caused a
     * TLB translation to be inserted from the VHPT.
     */
    numBptsToLoad = 0;

    if (bptsSet)
	to_load += ibptLoad();
    if (dbptsSet)
	to_load += dbptLoad();

    numBptsToLoad = to_load;
}

void bptUnload(void)
{
    if (bptsSet)
	ibptUnload();
    if (dbptsSet)
	dbptUnload();
}
