/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Debugger Internal Symbol Table Routines
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
#include "bits.h"
#include "types.h"
#include "fields.h"
#include "ssDCmd.h"
#include "exportui.h"
#include "coreui.h"
#include "libsym.h"	/* for symGet, symSet, symAddrtoDefFunc */
#include "libcore.h"
#include "state.h"

#define ISYMTBLSIZ	2500	/* max number of entries in internal */
				/*  symbol table */

/*##################### Datatypes ##########################################*/

/*##################### Globals - Imports ##################################*/

extern	REG	priv;
extern	BOOL	E_bit, defEndian;
extern	BOOL	defWidth;
extern	BOOL	cchEnable, takbrTrap;
extern	BOOL	pseudoOps;
extern	BOOL	symPsw;

/*##################### Local Variables ####################################*/

static unsigned topisym = 0;	/* highest isymtbl entry in use */

static struct isym isymtbl[ISYMTBLSIZ] = {{{0}}};	/* internal symbol table */

/*##################### Functions and Subroutines ##########################*/

#ifdef NEW_MP
CTR getCurPid(void);
#endif

/*---------------------------------------------------------------------------
 * compareInternalSymbols - This routine is used by isymInit to sort the
 *  internal symbols.  In particular, it is used by the qsort in isymInit.
 *---------------------------------------------------------------------------*/
static int cmpisym(const void *s1, const void *s2)
{
    const struct isym *sp1 = s1;
    const struct isym *sp2 = s2;

    return strcmp(sp1->name, sp2->name);
}

/*---------------------------------------------------------------------------
 * command$Internal$Symbol$List - Routine to implement the "isyms" command
 *  functionality.  The routine relies on a sorted internal symbol table.
 *---------------------------------------------------------------------------*/
BOOL cmdISymLst(unsigned argc, char *argv[])
{
    FILE *f = NULL;
    static char symbuf[7000];
    char *p;
    struct isym *ent;

    if (argc == 1 && !(f = cmdOpenFile(argv[0], "w")))
	return NO;
    p = symbuf;
    for (ent = isymtbl; ent < &isymtbl[topisym]; ent++)
	if (ent->type == REGSET_SYM && ent->start) {
	    char buf[30];

	    (void)snprintf(buf, sizeof (buf), "%s - %s\n", ent->name, ent->name);
	    p += sprintf(p, buf, 0, ent->ndx-1);
	} else
	    p += sprintf(p, "%s\n", ent->name);
    cmdOut("isymlist", "Name\n", symbuf, f);
    return YES;
}

static void isymChk(const char *sname)
{
    unsigned i;

    if (topisym == ISYMTBLSIZ) {
	cmdErr("Internal symbol table overflow\n");
	exitSim(EXIT_FAILURE);
    }
    if (strlen(sname) >= NAMLEN) {
	cmdErr("Internal symbol name too long: %s\n", sname);
	exitSim(EXIT_FAILURE);
    }
    for (i = 0; i < topisym; i++)
	if (!strcmp(sname, isymtbl[i].name)) {
	    cmdWarn("Internal symbol (%s) already in table.  Ignored\n", sname);
	    return;
	}
    (void)strcpy(isymtbl[topisym].name, sname);
}

/*---------------------------------------------------------------------------
 * internalSymbol$Insert - Place an internal symbol (register name, etc.)
 *  into the internal symbol table.
 *---------------------------------------------------------------------------*/

static void isymIns1(const char *sname, PGetFn gf, PSetFn sf, int dbase)
{
    isymChk(sname);
    isymtbl[topisym].getFn = gf;
    isymtbl[topisym].setFn = sf;
    isymtbl[topisym].type  = SINGLE_SYM;
    isymtbl[topisym].dbase = dbase;
    topisym++;
}

static void isymIns2(const char *sname, PGetFn gf, PSetFn sf, int dbase, int ndx)
{
    isymChk(sname);
    isymtbl[topisym].getFn = gf;
    isymtbl[topisym].setFn = sf;
    isymtbl[topisym].type  = REGSET_SYM;
    if (ndx >= 0) {	/* specific register in a register set */
	isymtbl[topisym].ndx = ndx;
	isymtbl[topisym].start = 0;
    } else {
	isymtbl[topisym].ndx = -ndx;
	isymtbl[topisym].start = 1;
    }
    isymtbl[topisym].len = 0;
    isymtbl[topisym].dbase = dbase;
    topisym++;
}

/* len is bit-field length, start is the little-endian bit number of the left
   edge of the field */
static void isymIns3(const char *sname, PGetFn gf, PSetFn sf, int dbase,
	      unsigned start, unsigned len)
{
    isymChk(sname);
    isymtbl[topisym].getFn = gf;
    isymtbl[topisym].setFn = sf;
    isymtbl[topisym].type  = BITF_SYM;
    isymtbl[topisym].len   = len;
    isymtbl[topisym].start = start;
    isymtbl[topisym].dbase = dbase;
    topisym++;
}

static void isymIns4(const char *sname, PGetFn gf, PSetFn sf, int dbase, unsigned ndx,
	      unsigned start, unsigned len)
{
    isymChk(sname);
    isymtbl[topisym].getFn = gf;
    isymtbl[topisym].setFn = sf;
    isymtbl[topisym].type  = RS_BITF_SYM;
    isymtbl[topisym].ndx   = ndx;
    isymtbl[topisym].len   = len;
    isymtbl[topisym].start = start;
    isymtbl[topisym].dbase = dbase;
    topisym++;
}

/*---------------------------------------------------------------------------
 * internalSymbol$ValueLookup - Find the passed internal symbol name in the
 *  internal symbol table.  If found, return its pointer value field, else
 *  return NULL.
 *---------------------------------------------------------------------------*/
struct isym *isymVLkp(const char *sname)
{
    struct isym *ent;
    static struct isym ret;

    for (ent = isymtbl; ent < &isymtbl[topisym]; ent++)
	if (ent->type == REGSET_SYM && ent->start) {
	    unsigned ndx;
	    char buf[30];

	    if (sscanf(sname, ent->name, &ndx) != 1)
		continue;
	    (void)sprintf(buf, ent->name, ndx);
	    if (!strcmp(sname, buf) && ndx < ent->ndx) {
		ret = *ent;
		ret.ndx = ndx;
		return &ret;
	    }
	} else {
	    if (!strcmp(sname, ent->name)) {
		ret = *ent;
		return &ret;
	    }
	}
    return NULL;
}

/*---------------------------------------------------------------------------
 * internalSymbol$Init - Initialize the internal symbol table.
 *---------------------------------------------------------------------------*/
void isymInit(void)
{
    /* initialize internal symbol names for registers */
    isymIns1("ip", ipGet, ipSet, HEXEXP);

    isymIns1("psr", psrGet, psrSet, HEXEXP);
    isymIns3("psr.um",  psrGet, psrSet, HEXEXP,  5, 6);
    isymIns3("psr.be",  psrGet, psrSet, HEXEXP,  1, 1);
    isymIns3("psr.up",  psrGet, psrSet, HEXEXP,  2, 1);
    isymIns3("psr.ac",  psrGet, psrSet, HEXEXP,  3, 1);
    isymIns3("psr.mfl", psrGet, psrSet, HEXEXP,  4, 1);
    isymIns3("psr.mfh", psrGet, psrSet, HEXEXP,  5, 1);
    isymIns3("psr.ic",  psrGet, psrSet, HEXEXP, 13, 1);
    isymIns3("psr.i",   psrGet, psrSet, HEXEXP, 14, 1);
    isymIns3("psr.pk",  psrGet, psrSet, HEXEXP, 15, 1);
    isymIns3("psr.dt",  psrGet, psrSet, HEXEXP, 17, 1);
    isymIns3("psr.dfl", psrGet, psrSet, HEXEXP, 18, 1);
    isymIns3("psr.dfh", psrGet, psrSet, HEXEXP, 19, 1);
    isymIns3("psr.sp",  psrGet, psrSet, HEXEXP, 20, 1);
    isymIns3("psr.pp",  psrGet, psrSet, HEXEXP, 21, 1);
    isymIns3("psr.di",  psrGet, psrSet, HEXEXP, 22, 1);
    isymIns3("psr.si",  psrGet, psrSet, HEXEXP, 23, 1);
    isymIns3("psr.db",  psrGet, psrSet, HEXEXP, 24, 1);
    isymIns3("psr.lp",  psrGet, psrSet, HEXEXP, 25, 1);
    isymIns3("psr.tb",  psrGet, psrSet, HEXEXP, 26, 1);
    isymIns3("psr.rt",  psrGet, psrSet, HEXEXP, 27, 1);
    isymIns3("psr.cpl", psrGet, psrSet, HEXEXP, 33, 2);
    isymIns3("psr.is",  psrGet, psrSet, HEXEXP, 34, 1);
    isymIns3("psr.mc",  psrGet, psrSet, HEXEXP, 35, 1);
    isymIns3("psr.it",  psrGet, psrSet, HEXEXP, 36, 1);
    isymIns3("psr.id",  psrGet, psrSet, HEXEXP, 37, 1);
    isymIns3("psr.da",  psrGet, psrSet, HEXEXP, 38, 1);
    isymIns3("psr.dd",  psrGet, psrSet, HEXEXP, 39, 1);
    isymIns3("psr.ss",  psrGet, psrSet, HEXEXP, 40, 1);
    isymIns3("psr.ri",  psrGet, psrSet, HEXEXP, 42, 2);
    isymIns3("psr.ed",  psrGet, psrSet, HEXEXP, 43, 1);
    isymIns3("psr.bn",  psrGet, psrSet, HEXEXP, 44, 1);
    isymIns3("psr.ia",  psrGet, psrSet, HEXEXP, 45, 1);

    isymIns1("cfm", cfmGet, NULL, HEXEXP);
    isymIns1("sof", sofGet, NULL, DECEXP);
    isymIns1("sol", solGet, NULL, DECEXP);
    isymIns1("sor", sorGet, NULL, DECEXP);
    isymIns1("rrbg", rrbgGet, rrbgSet, DECEXP);
    isymIns1("rrbf", rrbfGet, rrbfSet, DECEXP);
    isymIns1("rrbp", rrbpGet, rrbpSet, DECEXP);
    isymIns1("dirty", dirtyGet, NULL, DECEXP);
    isymIns1("dirtyNat", dirtyNatGet, NULL, DECEXP);
    isymIns1("clean", cleanGet, NULL, DECEXP);
    isymIns1("cleanNat", cleanNatGet, NULL, DECEXP);
    isymIns1("invalid", invalidGet, NULL, DECEXP);
    isymIns1("bol", bolGet, NULL, DECEXP);

    isymIns2("r%u", grGet, grSet, HEXEXP, -/*GR_STK_BASE+N_STACK_VIRT*/128);
    isymIns2("r%u.nat", grNatGet, grNatSet, HEXEXP, -/*GR_STK_BASE+N_STACK_VIRT*/128);
    isymIns2("gp", grGet, grSet, HEXEXP, GP_ID);
    isymIns2("sp", grGet, grSet, HEXEXP, SP_ID);
    isymIns2("tp", grGet, grSet, HEXEXP, TP_ID);
    isymIns2("gp.nat", grNatGet, grNatSet, HEXEXP, GP_ID);
    isymIns2("sp.nat", grNatGet, grNatSet, HEXEXP, SP_ID);
    isymIns2("tp.nat", grNatGet, grNatSet, HEXEXP, TP_ID);
    isymIns2("pgr%u", phyGrGet, phyGrSet, HEXEXP,
	     -/*GR_STK_BASE+N_STACK_PHYS*/128);
    isymIns2("pgr%u.nat", phyGrNatGet, phyGrNatSet, HEXEXP,
	     -/*GR_STK_BASE+N_STACK_PHYS*/128);
    isymIns2("bkr%u", bkrGet, bkrSet, HEXEXP, -/*NBGRS*/16);
    isymIns2("bkr%u.nat", bkrNatGet, bkrNatSet, HEXEXP, -/*NBGRS*/16);
    isymIns2("f%u.s", frSignGet, frSignSet, HEXEXP, -/*NFRS*/128);
    isymIns2("f%u.e", frExpGet, frExpSet, HEXEXP, -/*NFRS*/128);
    isymIns2("f%u.m", frMantGet, frMantSet, HEXEXP, -/*NFRS*/128);
    isymIns2("p%u", prGet, prSet, HEXEXP, -/*NPRS*/64);
    isymIns2("b%u", brGet, brSet, HEXEXP, -/*NBRS*/8);
    isymIns2("ar%u", arGet, arSet, HEXEXP, -/*NARS*/128);
    isymIns2("rp", brGet, brSet, HEXEXP, RP_ID);
    isymIns2("fpsr",  arGet, arSet, HEXEXP, FPSR_ID);
    isymIns4("fpsr.traps",arGet, arSet, HEXEXP, FPSR_ID, 0, 6);
    isymIns4("fpsr.sf0",  arGet, arSet, HEXEXP, FPSR_ID, 6, 13);
    isymIns4("fpsr.sf0.ftz", arGet, arSet, HEXEXP, FPSR_ID, 6, 1);
    isymIns4("fpsr.sf0.wre", arGet, arSet, HEXEXP, FPSR_ID, 7, 1);
    isymIns4("fpsr.sf0.pc", arGet, arSet, HEXEXP, FPSR_ID, 9, 2);
    isymIns4("fpsr.sf0.rc", arGet, arSet, HEXEXP, FPSR_ID, 11, 2);
    isymIns4("fpsr.sf0.v", arGet, arSet, HEXEXP, FPSR_ID, 13, 1);
    isymIns4("fpsr.sf0.d", arGet, arSet, HEXEXP, FPSR_ID, 14, 1);
    isymIns4("fpsr.sf0.z", arGet, arSet, HEXEXP, FPSR_ID, 15, 1);
    isymIns4("fpsr.sf0.o", arGet, arSet, HEXEXP, FPSR_ID, 16, 1);
    isymIns4("fpsr.sf0.u", arGet, arSet, HEXEXP, FPSR_ID, 17, 1);
    isymIns4("fpsr.sf0.i", arGet, arSet, HEXEXP, FPSR_ID, 18, 1);
    isymIns4("fpsr.sf1",  arGet, arSet, HEXEXP, FPSR_ID, 19, 13);
    isymIns4("fpsr.sf2",  arGet, arSet, HEXEXP, FPSR_ID, 32, 13);
    isymIns4("fpsr.sf2.pc", arGet, arSet, HEXEXP, FPSR_ID, 35, 2);
    isymIns4("fpsr.sf2.rc", arGet, arSet, HEXEXP, FPSR_ID, 37, 2);
    isymIns4("fpsr.sf2.v", arGet, arSet, HEXEXP, FPSR_ID, 39, 1);
    isymIns4("fpsr.sf2.d", arGet, arSet, HEXEXP, FPSR_ID, 40, 1);
    isymIns4("fpsr.sf2.z", arGet, arSet, HEXEXP, FPSR_ID, 41, 1);
    isymIns4("fpsr.sf2.o", arGet, arSet, HEXEXP, FPSR_ID, 42, 1);
    isymIns4("fpsr.sf2.u", arGet, arSet, HEXEXP, FPSR_ID, 43, 1);
    isymIns4("fpsr.sf2.i", arGet, arSet, HEXEXP, FPSR_ID, 44, 1);
    isymIns4("fpsr.sf3",  arGet, arSet, HEXEXP, FPSR_ID, 45, 13);
    isymIns2("itc",   arGet, arSet, HEXEXP, ITC_ID);
    isymIns2("rsc",   arGet, arSet, HEXEXP, RSC_ID);
    isymIns4("rsc.mode", arGet, arSet, HEXEXP, RSC_ID, 0, 2);
    isymIns4("rsc.pl",   arGet, arSet, HEXEXP, RSC_ID, 2, 2);
    isymIns4("rsc.be",   arGet, arSet, HEXEXP, RSC_ID, 4, 1);
    isymIns4("rsc.loadrs", arGet, arSet, HEXEXP, RSC_ID, 16, 14);
    isymIns2("bsp",   arGet, arSet, HEXEXP, BSP_ID);
    isymIns2("bspst", arGet, arSet, HEXEXP, BSPST_ID);
    isymIns2("rnat",  arGet, arSet, HEXEXP, RNAT_ID);
    isymIns2("csd",   arGet, arSet, HEXEXP, CSD_ID);
    isymIns2("unat",  arGet, arSet, HEXEXP, UNAT_ID);
    isymIns2("ccv",   arGet, arSet, HEXEXP, CCV_ID);
    isymIns2("k%u",   arGet, arSet, HEXEXP, -/*NKRS?*/8);
    isymIns2("lc",    arGet, arSet, DECEXP, LC_ID);
    isymIns2("ec",    arGet, arSet, DECEXP, EC_ID);
    isymIns2("pfs",   arGet, arSet, HEXEXP, PFS_ID);
    isymIns4("pfs.pfm", arGet, arSet, HEXEXP, PFS_ID, 37, 38);
    isymIns4("pfs.pfm.sof", arGet, arSet, DECEXP, PFS_ID, 6, 7);
    isymIns4("pfs.pfm.sol", arGet, arSet, DECEXP, PFS_ID, 13, 7);
    isymIns4("pfs.pfm.sor", arGet, arSet, DECEXP, PFS_ID, 17, 4);
    isymIns4("pfs.pec", arGet, arSet, DECEXP, PFS_ID, 57, 6);
    isymIns4("pfs.ppl", arGet, arSet, HEXEXP, PFS_ID, 63, 2);
    isymIns2("cr%u", crGet, crSet, HEXEXP, -/*NCRS*/128);
    isymIns2("dcr",  crGet, crSet, HEXEXP, DCR_ID);
    isymIns4("dcr.pp", crGet, crSet, HEXEXP, DCR_ID, 0, 1);
    isymIns4("dcr.be", crGet, crSet, HEXEXP, DCR_ID, 1, 1);
    isymIns4("dcr.lc", crGet, crSet, HEXEXP, DCR_ID, 2, 1);
    isymIns4("dcr.dm", crGet, crSet, HEXEXP, DCR_ID, 8, 1);
    isymIns4("dcr.dp", crGet, crSet, HEXEXP, DCR_ID, 9, 1);
    isymIns4("dcr.dk", crGet, crSet, HEXEXP, DCR_ID, 10, 1);
    isymIns4("dcr.dx", crGet, crSet, HEXEXP, DCR_ID, 11, 1);
    isymIns4("dcr.dr", crGet, crSet, HEXEXP, DCR_ID, 12, 1);
    isymIns4("dcr.da", crGet, crSet, HEXEXP, DCR_ID, 13, 1);
    isymIns4("dcr.dd", crGet, crSet, HEXEXP, DCR_ID, 14, 1);
    isymIns2("itm",  crGet, crSet, HEXEXP, ITM_ID);
    isymIns2("iva",  crGet, crSet, HEXEXP, IVA_ID);
    isymIns2("pta",  crGet, crSet, HEXEXP, PTA_ID);
    isymIns2("gpta", crGet, crSet, HEXEXP, GPTA_ID);
    isymIns2("ipsr", crGet, crSet, HEXEXP, IPSR_ID);
    isymIns4("ipsr.um",  crGet, crSet, HEXEXP, IPSR_ID,  5, 6);
    isymIns4("ipsr.be",  crGet, crSet, HEXEXP, IPSR_ID,  1, 1);
    isymIns4("ipsr.up",  crGet, crSet, HEXEXP, IPSR_ID,  2, 1);
    isymIns4("ipsr.ac",  crGet, crSet, HEXEXP, IPSR_ID,  3, 1);
    isymIns4("ipsr.mfl", crGet, crSet, HEXEXP, IPSR_ID,  4, 1);
    isymIns4("ipsr.mfh", crGet, crSet, HEXEXP, IPSR_ID,  5, 1);
    isymIns4("ipsr.ic",  crGet, crSet, HEXEXP, IPSR_ID, 13, 1);
    isymIns4("ipsr.i",   crGet, crSet, HEXEXP, IPSR_ID, 14, 1);
    isymIns4("ipsr.pk",  crGet, crSet, HEXEXP, IPSR_ID, 15, 1);
    isymIns4("ipsr.dt",  crGet, crSet, HEXEXP, IPSR_ID, 17, 1);
    isymIns4("ipsr.dfl", crGet, crSet, HEXEXP, IPSR_ID, 18, 1);
    isymIns4("ipsr.dfh", crGet, crSet, HEXEXP, IPSR_ID, 19, 1);
    isymIns4("ipsr.sp",  crGet, crSet, HEXEXP, IPSR_ID, 20, 1);
    isymIns4("ipsr.pp",  crGet, crSet, HEXEXP, IPSR_ID, 21, 1);
    isymIns4("ipsr.di",  crGet, crSet, HEXEXP, IPSR_ID, 22, 1);
    isymIns4("ipsr.si",  crGet, crSet, HEXEXP, IPSR_ID, 23, 1);
    isymIns4("ipsr.db",  crGet, crSet, HEXEXP, IPSR_ID, 24, 1);
    isymIns4("ipsr.lp",  crGet, crSet, HEXEXP, IPSR_ID, 25, 1);
    isymIns4("ipsr.tb",  crGet, crSet, HEXEXP, IPSR_ID, 26, 1);
    isymIns4("ipsr.rt",  crGet, crSet, HEXEXP, IPSR_ID, 27, 1);
    isymIns4("ipsr.cpl", crGet, crSet, HEXEXP, IPSR_ID, 33, 2);
    isymIns4("ipsr.is",  crGet, crSet, HEXEXP, IPSR_ID, 34, 1);
    isymIns4("ipsr.mc",  crGet, crSet, HEXEXP, IPSR_ID, 35, 1);
    isymIns4("ipsr.it",  crGet, crSet, HEXEXP, IPSR_ID, 36, 1);
    isymIns4("ipsr.id",  crGet, crSet, HEXEXP, IPSR_ID, 37, 1);
    isymIns4("ipsr.da",  crGet, crSet, HEXEXP, IPSR_ID, 38, 1);
    isymIns4("ipsr.dd",  crGet, crSet, HEXEXP, IPSR_ID, 39, 1);
    isymIns4("ipsr.ss",  crGet, crSet, HEXEXP, IPSR_ID, 40, 1);
    isymIns4("ipsr.ri",  crGet, crSet, HEXEXP, IPSR_ID, 42, 2);
    isymIns4("ipsr.ed",  crGet, crSet, HEXEXP, IPSR_ID, 43, 1);
    isymIns4("ipsr.bn",  crGet, crSet, HEXEXP, IPSR_ID, 44, 1);
    isymIns4("ipsr.ia",  crGet, crSet, HEXEXP, IPSR_ID, 45, 1);
    isymIns2("isr",  crGet, crSet, HEXEXP, ISR_ID);
    isymIns4("isr.x", crGet, crSet, HEXEXP, ISR_ID, 32, 1);
    isymIns4("isr.w", crGet, crSet, HEXEXP, ISR_ID, 33, 1);
    isymIns4("isr.r", crGet, crSet, HEXEXP, ISR_ID, 34, 1);
    isymIns4("isr.na", crGet, crSet, HEXEXP, ISR_ID, 35, 1);
    isymIns4("isr.sp", crGet, crSet, HEXEXP, ISR_ID, 36, 1);
    isymIns4("isr.rs", crGet, crSet, HEXEXP, ISR_ID, 37, 1);
    isymIns4("isr.ir", crGet, crSet, HEXEXP, ISR_ID, 38, 1);
    isymIns4("isr.ni", crGet, crSet, HEXEXP, ISR_ID, 39, 1);
    isymIns4("isr.ei", crGet, crSet, HEXEXP, ISR_ID, 42, 2);
    isymIns4("isr.ed", crGet, crSet, HEXEXP, ISR_ID, 43, 1);
    isymIns2("iip",  crGet, crSet, HEXEXP, IIP_ID);
    isymIns2("ifa",  crGet, crSet, HEXEXP, IFA_ID);
    isymIns2("itir", crGet, crSet, HEXEXP, ITIR_ID);
    isymIns2("iipa", crGet, crSet, HEXEXP, IIPA_ID);
    isymIns2("ifs",  crGet, crSet, HEXEXP, IFS_ID);
    isymIns2("iim",  crGet, crSet, HEXEXP, IIM_ID);
    isymIns2("iha",  crGet, crSet, HEXEXP, IHA_ID);
    isymIns2("lid",  crGet, crSet, HEXEXP, LID_ID);
    isymIns2("ivr",  crGet, crSet, HEXEXP, IVR_ID);
    isymIns2("tpr",  crGet, crSet, HEXEXP, TPR_ID);
    isymIns2("eoi",  crGet, crSet, HEXEXP, EOI_ID);
    isymIns2("irr0", crGet, crSet, HEXEXP, IRR0_ID);
    isymIns2("irr1", crGet, crSet, HEXEXP, IRR1_ID);
    isymIns2("irr2", crGet, crSet, HEXEXP, IRR2_ID);
    isymIns2("irr3", crGet, crSet, HEXEXP, IRR3_ID);
    isymIns2("itv",  crGet, crSet, HEXEXP, ITV_ID);
    isymIns2("pmv",  crGet, crSet, HEXEXP, PMV_ID);
    isymIns2("cmcv", crGet, crSet, HEXEXP, CMCV_ID);
    isymIns2("lrr0", crGet, crSet, HEXEXP, LRR0_ID);
    isymIns2("lrr1", crGet, crSet, HEXEXP, LRR1_ID);
    isymIns2("rr%u", rrGet, rrSet, HEXEXP, -/*NRRS*/8);
    isymIns2("pkr%u", pkrGet, pkrSet, HEXEXP, -/*NPKRS*/16);
    isymIns2("dbr%u", dbrGet, dbrSet, HEXEXP, -/*NDBRS*/16);
    isymIns2("ibr%u", ibrGet, ibrSet, HEXEXP, -/*NIBRS*/16);
    isymIns2("pmc%u", pmcGet, pmcSet, HEXEXP, -NPMS);
    isymIns2("pmd%u", pmdGet, pmdSet, HEXEXP, -NPMS);
    isymIns2("cpuid%u", cpuidGet, NULL, HEXEXP, -/*NCPUIDS*/5);

    /* iA Registers */
    isymIns4("eax", grGet, grSet, HEXEXP, EAX_ID, 31, 32);
    isymIns4("ebx", grGet, grSet, HEXEXP, EBX_ID, 31, 32);
    isymIns4("ecx", grGet, grSet, HEXEXP, ECX_ID, 31, 32);
    isymIns4("edx", grGet, grSet, HEXEXP, EDX_ID, 31, 32);
    isymIns4("ax",  grGet, grSet, HEXEXP, EAX_ID, 15, 16);
    isymIns4("bx",  grGet, grSet, HEXEXP, EBX_ID, 15, 16);
    isymIns4("cx",  grGet, grSet, HEXEXP, ECX_ID, 15, 16);
    isymIns4("dx",  grGet, grSet, HEXEXP, EDX_ID, 15, 16);
    isymIns4("ah",  grGet, grSet, HEXEXP, EAX_ID, 15,  8);
    isymIns4("bh",  grGet, grSet, HEXEXP, EBX_ID, 15,  8);
    isymIns4("ch",  grGet, grSet, HEXEXP, ECX_ID, 15,  8);
    isymIns4("dh",  grGet, grSet, HEXEXP, EDX_ID, 15,  8);
    isymIns4("al",  grGet, grSet, HEXEXP, EAX_ID,  7,  8);
    isymIns4("bl",  grGet, grSet, HEXEXP, EBX_ID,  7,  8);
    isymIns4("cl",  grGet, grSet, HEXEXP, ECX_ID,  7,  8);
    isymIns4("dl",  grGet, grSet, HEXEXP, EDX_ID,  7,  8);
    isymIns4("esp", grGet, grSet, HEXEXP, ESP_ID, 31, 32);
    isymIns4("ebp", grGet, grSet, HEXEXP, EBP_ID, 31, 32);
    isymIns4("esi", grGet, grSet, HEXEXP, ESI_ID, 31, 32);
    isymIns4("edi", grGet, grSet, HEXEXP, EDI_ID, 31, 32);
    isymIns4("iasp",grGet, grSet, HEXEXP, ESP_ID, 15, 16);
    isymIns4("bp",  grGet, grSet, HEXEXP, EBP_ID, 15, 16);
    isymIns4("si",  grGet, grSet, HEXEXP, ESI_ID, 15, 16);
    isymIns4("di",  grGet, grSet, HEXEXP, EDI_ID, 15, 16);
    isymIns4("es",  grGet, grSet, HEXEXP, ES_ID,  15, 16);
    isymIns4("cs",  grGet, grSet, HEXEXP, CS_ID,  15, 16);
    isymIns4("ss",  grGet, grSet, HEXEXP, SS_ID,  15, 16);
    isymIns4("ds",  grGet, grSet, HEXEXP, DS_ID,  15, 16);
    isymIns4("fs",  grGet, grSet, HEXEXP, FS_ID,  15, 16);
    isymIns4("gs",  grGet, grSet, HEXEXP, GS_ID,  15, 16);
    isymIns4("ldt", grGet, grSet, HEXEXP, LDT_ID, 15, 16);
    isymIns2("esd", grGet, grSet, HEXEXP, ESD_ID);
    isymIns2("ssd", grGet, grSet, HEXEXP, SSD_ID);
    isymIns2("dsd", grGet, grSet, HEXEXP, DSD_ID);
    isymIns2("fsd", grGet, grSet, HEXEXP, FSD_ID);
    isymIns2("gsd", grGet, grSet, HEXEXP, GSD_ID);
    isymIns2("ldtd",grGet, grSet, HEXEXP, LDTD_ID);
    isymIns2("gdtd",grGet, grSet, HEXEXP, GDTD_ID);

    isymIns2("iobase",arGet, arSet, HEXEXP, IOBASE_ID);
    isymIns4("tss", arGet, arSet, HEXEXP, TSS_ID, 15, 16);
    isymIns2("tssd",arGet, arSet, HEXEXP, TSSD_ID);
    isymIns2("idtd",arGet, arSet, HEXEXP, IDTD_ID);
    isymIns2("cflg",arGet, arSet, HEXEXP, CFLG_ID);
    isymIns4("iacr0",arGet, arSet, HEXEXP, CFLG_ID, 31, 32);
    isymIns4("iacr2",arGet, arSet, HEXEXP, CR3_CR2_ID, 63, 32);
    isymIns4("iacr3",arGet, arSet, HEXEXP, CR3_CR2_ID, 31, 32);
    isymIns4("iacr4",arGet, arSet, HEXEXP, CFLG_ID, 63, 32);
    isymIns4("dr6", arGet, arSet, HEXEXP, DR6_DR7_ID, 31, 32);
    isymIns4("dr7", arGet, arSet, HEXEXP, DR6_DR7_ID, 63, 32);
    isymIns2("fcr", arGet, arSet, HEXEXP, FCR_ID);
    isymIns2("fsr", arGet, arSet, HEXEXP, FSR_ID);
    isymIns2("fir", arGet, arSet, HEXEXP, FIR_ID);
    isymIns2("fdr", arGet, arSet, HEXEXP, FDR_ID);
    isymIns4("eflags",arGet, arSet, HEXEXP, EFLAGS_ID, 31, 32);

    isymIns4("eflags.le",  arGet, arSet, HEXEXP, EFLAGS_ID, 25, 1);
    isymIns4("eflags.be",  arGet, arSet, HEXEXP, EFLAGS_ID, 24, 1);
    isymIns4("eflags.lt",  arGet, arSet, HEXEXP, EFLAGS_ID, 23, 1);
    isymIns4("eflags.id",  arGet, arSet, HEXEXP, EFLAGS_ID, 21, 1);
    isymIns4("eflags.vip", arGet, arSet, HEXEXP, EFLAGS_ID, 20, 1);
    isymIns4("eflags.vif", arGet, arSet, HEXEXP, EFLAGS_ID, 19, 1);
    isymIns4("eflags.ac",  arGet, arSet, HEXEXP, EFLAGS_ID, 18, 1);
    isymIns4("eflags.vm",  arGet, arSet, HEXEXP, EFLAGS_ID, 17, 1);
    isymIns4("eflags.rf",  arGet, arSet, HEXEXP, EFLAGS_ID, 16, 1);
    isymIns4("eflags.nt",  arGet, arSet, HEXEXP, EFLAGS_ID, 14, 1);
    isymIns4("eflags.iopl",arGet, arSet, HEXEXP, EFLAGS_ID, 13, 2);
    isymIns4("eflags.of",  arGet, arSet, HEXEXP, EFLAGS_ID, 11, 1);
    isymIns4("eflags.df",  arGet, arSet, HEXEXP, EFLAGS_ID, 10, 1);
    isymIns4("eflags.if",  arGet, arSet, HEXEXP, EFLAGS_ID,  9, 1);
    isymIns4("eflags.tf",  arGet, arSet, HEXEXP, EFLAGS_ID,  8, 1);
    isymIns4("eflags.sf",  arGet, arSet, HEXEXP, EFLAGS_ID,  7, 1);
    isymIns4("eflags.zf",  arGet, arSet, HEXEXP, EFLAGS_ID,  6, 1);
    isymIns4("eflags.af",  arGet, arSet, HEXEXP, EFLAGS_ID,  4, 1);
    isymIns4("eflags.pf",  arGet, arSet, HEXEXP, EFLAGS_ID,  2, 1);
    isymIns4("eflags.cf",  arGet, arSet, HEXEXP, EFLAGS_ID,  0, 1);

    isymIns1("$insts$", getTotalInsts, NULL, DECEXP);
    isymIns1("$cycles$", getTotalCycles, NULL, DECEXP);
    isymIns1("$faults$", getTotalFaults, NULL, DECEXP);
#ifdef NEW_MP
    isymIns1("$pid$", getCurPid, NULL, DECEXP);
#endif
    isymIns1("$heap$", heapGet, NULL, HEXEXP);
    isymIns1("$exited$", getExited, NULL, HEXEXP);

    qsort((void *)isymtbl, topisym, sizeof (struct isym), cmpisym);
#if 0
    unsigned save = cproc;

    for (cproc = 0; cproc < nproc; cproc++) {
	isymIns("cproc", &cproc, NO);	/* symbol for cproc */
#endif
#if 0
	for (i = 0; i < NURS; i++) {
	    (void)sprintf(s, "$t%d", i);
	    isymIns(s, &urs[i], YES);
	}
	isymIns("$inrtyp$", &inrtype, NO); isymIns("$cache$", &cchEnable, NO);
	isymIns("$skip$", &stepLvl, NO); isymIns("$takbr$", &takbrTrap, NO);
	isymIns("$page$", &nextavail, NO); isymIns("$pseudo$", &pseudoOps, NO);
	isymIns("$sympsw$", &symPsw, NO);
    }
    cproc = save;
#endif
}

/*---------------------------------------------------------------------------
 * Functions needed by the save/restore functionality
 *---------------------------------------------------------------------------*/
void symGetInit(void)
{
    symNextSymInit();
}

BOOL symGet(char **symnam, ADDR *adr)
{
    Symbol sym;
    BOOL status = symNextSym(&sym);

    if (status) {
	*symnam = sym.name;
	*adr    = sym.addr;
    }
    return status;
}

void symSet(char *strval, ADDR adr)
{
    symInsert(strval, adr, 0);
}

void symAddrtoDefFunc(ADDR adr, char sname[], int rmdr, int add0x, int width)
{
    symAddrtoName(adr, sname, rmdr, add0x, width);
}
