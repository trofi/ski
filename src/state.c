/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Architected State Routines
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

#include <math.h>
#include <stdio.h>

#include "std.h"
#include "bits.h"
#include "exec.h"
#include "types.h"
#include "fields.h"
#include "state.h"

unsigned nproc = 1;
unsigned cproc = 0;

#ifdef NEW_MP
mpstate mpState[4], zeroState = {0};

unsigned curPid = 0, lastPid = ~0U;
unsigned n_stack_phys;
BOOL abi, unixABI = NO;
BOOL running = YES, exited = NO;
ADDR ibBase;
ADDR max_sp;
#else
GREG grs[MAX_NGRS];
unsigned grmap[GR_STK_BASE+N_STACK_VIRT];
GREG bankedGrs[NBGRS];
BOOL prs[NPRS];
FREG frs[NFRS];
unsigned frmap[NFRS + FR_ROT_SIZE];
REG brs[NBRS];   /* Branch Registers */
REG ars[NARS];   /* Application Registers */

REG psr = 0;     /* Processor Status Register */

REG crs[NCRS];   /* Control Registers */
REG rrs[NRRS];   /* Region Register ID */
REG pkrs[NPKRS]; /* Protection Key Register ID */
REG dbrs[NDBRS]; /* Data Breakpoint Register */
REG ibrs[NIBRS]; /* Instruction Breakpoint Register */
REG pmcs[NPMS];  /* Performance Monitor Configuration */
REG pmds[NPMS];  /* Performance Monitor Data */
#if 0
const REG cpuids[NCPUIDS] = { 0x2d7474656c776548ULL, 0x006472616b636150ULL,
#else
const REG cpuids[NCPUIDS] = { 0x5320696b53205048ULL, 0x726f74616c756d69ULL,
#endif
			      0,                     0x000000001f000004ULL,
			      0x5ULL };

unsigned n_stack_phys /* = 96 */;	/* initialized in InitPlatform() */
BYTE sof = N_STACK_VIRT, soil = 0, sor = 0;
BYTE rrbg = 0, rrbf = 0, rrbp = 0;
unsigned bol = 0;
int dirty = 0, dirtyNat = 0, clean = 0, cleanNat = 0;
int invalid;				/* initialized in InitPlatform() */
BOOL cfle = NO;

ADDR ip, heap;
ADDR ibBase;

BOOL running = YES, exited = NO;
BOOL abi, unixABI = NO, lp64;
ADDR max_sp;
#endif

const FREG ZERO_FREG = {NO, 0, 64, 0, 0, 0};

/*
 * Routines needed by Save/Restore
 */

REG ipGet(int proc)
{
#ifdef NEW_MP
    return mpState[proc].ip_;
#else
    return ip;
#endif
}

REG psrGet(int proc)
{
#ifdef NEW_MP
    return mpState[proc].psr_;
#else
    return psr;
#endif
}

REG grGet(int proc, int i)
{
    REG val;
    BOOL nat;
#ifdef NEW_MP
    int save = curPid;

    curPid = proc;
#endif
    GrRd(i, val, nat, NO);
    (void)nat;
#ifdef NEW_MP
    curPid = save;
#endif
    return val;
}

REG grNatGet(int proc, int i)
{
    REG val;
    BOOL nat;
#ifdef NEW_MP
    int save = curPid;

    curPid = proc;
#endif
    GrRd(i, val, nat, NO);
    (void)val;
#ifdef NEW_MP
    curPid = save;
#endif
    return nat;
}

REG phyGrGet(int proc, int i)
{
#ifdef NEW_MP
    return mpState[proc].grs_[i].val;
#else
    return grs[i].val;
#endif
}

REG phyGrNatGet(int proc, int i)
{
#ifdef NEW_MP
    return mpState[proc].grs_[i].nat;
#else
    return grs[i].nat;
#endif
}

REG frSignGet(int proc, int i)
{
    FREG fr;
    BYTE sign;
    WORD exp;
    DWORD mant;
#ifdef NEW_MP
    int save = curPid;

    curPid = proc;
#endif
    fr = FrRd(i);
#ifdef NEW_MP
    curPid = save;
#endif
    freg2spill(fr, &sign, &exp, &mant);
    return sign;
}

REG frExpGet(int proc, int i)
{
    FREG fr;
    BYTE sign;
    WORD exp;
    DWORD mant;
#ifdef NEW_MP
    int save = curPid;

    curPid = proc;
#endif
    fr = FrRd(i);
#ifdef NEW_MP
    curPid = save;
#endif
    freg2spill(fr, &sign, &exp, &mant);
    return exp;
}

REG frMantGet(int proc, int i)
{
    FREG fr;
    BYTE sign;
    WORD exp;
    DWORD mant;
#ifdef NEW_MP
    int save = curPid;

    curPid = proc;
#endif
    fr = FrRd(i);
#ifdef NEW_MP
    curPid = save;
#endif
    freg2spill(fr, &sign, &exp, &mant);
    return mant;
}

REG phyFrSignGet(int proc, int i)
{
    BYTE sign;
    WORD exp;
    DWORD mant;

#ifdef NEW_MP
    freg2spill(mpState[proc].frs_[i], &sign, &exp, &mant);
#else
    freg2spill(frs[i], &sign, &exp, &mant);
#endif
    return sign;
}

REG phyFrExpGet(int proc, int i)
{
    BYTE sign;
    WORD exp;
    DWORD mant;

#ifdef NEW_MP
    freg2spill(mpState[proc].frs_[i], &sign, &exp, &mant);
#else
    freg2spill(frs[i], &sign, &exp, &mant);
#endif
    return exp;
}

REG phyFrMantGet(int proc, int i)
{
    BYTE sign;
    WORD exp;
    DWORD mant;

#ifdef NEW_MP
    freg2spill(mpState[proc].frs_[i], &sign, &exp, &mant);
#else
    freg2spill(frs[i], &sign, &exp, &mant);
#endif
    return mant;
}

REG prGet(int proc, int i)
{
#ifdef NEW_MP
    int save = curPid;
    REG ret;

    curPid = proc;
    ret = PrRd(i);
    curPid = save;
    return ret;
#else
    return PrRd(i);
#endif
}

BOOL phyPrGet(int proc, int i)
{
#ifdef NEW_MP
    return mpState[proc].prs_[i];
#else
    return prs[i];
#endif
}

REG brGet(int proc, int i)
{
#ifdef NEW_MP
    return mpState[proc].brs_[i];
#else
    return brs[i];
#endif
}

REG arGet(int proc, int i)
{
#ifdef NEW_MP
    return mpState[proc].ars_[i];
#else
    return ars[i];
#endif
}

REG cfmGet(int proc)
{
#ifdef NEW_MP
    REG cfm;
    int save = curPid;

    curPid = proc;
    cfm = (REG)rrbp << 32 | (unsigned)rrbf << 25 | rrbg << 18 |
	  (sor >> 3) << 14 | soil << 7 | sof;
    curPid = save;
#else
    REG cfm = (REG)rrbp << 32 | (unsigned)rrbf << 25 | rrbg << 18 |
	      (sor >> 3) << 14 | soil << 7 | sof;
#endif
    return cfm;
}

REG sofGet(int proc)
{
#ifdef NEW_MP
    return mpState[proc].sof_;
#else
    return sof;
#endif
}

REG solGet(int proc)
{
#ifdef NEW_MP
    return mpState[proc].soil_;
#else
    return soil;
#endif
}

REG sorGet(int proc)
{
#ifdef NEW_MP
    return mpState[proc].sor_;
#else
    return sor;
#endif
}

REG rrbgGet(int proc)
{
#ifdef NEW_MP
    return mpState[proc].rrbg_;
#else
    return rrbg;
#endif
}

REG rrbfGet(int proc)
{
#ifdef NEW_MP
    return mpState[proc].rrbf_;
#else
    return rrbf;
#endif
}

REG rrbpGet(int proc)
{
#ifdef NEW_MP
    return mpState[proc].rrbp_;
#else
    return rrbp;
#endif
}

REG bolGet(int proc)
{
#ifdef NEW_MP
    return mpState[proc].bol_;
#else
    return bol;
#endif
}

REG dirtyGet(int proc)
{
#ifdef NEW_MP
    return mpState[proc].dirty_;
#else
    return dirty;
#endif
}

REG dirtyNatGet(int proc)
{
#ifdef NEW_MP
    return mpState[proc].dirtyNat_;
#else
    return dirtyNat;
#endif
}

REG cleanGet(int proc)
{
#ifdef NEW_MP
    return mpState[proc].clean_;
#else
    return clean;
#endif
}

REG cleanNatGet(int proc)
{
#ifdef NEW_MP
    return mpState[proc].cleanNat_;
#else
    return cleanNat;
#endif
}

REG invalidGet(int proc)
{
#ifdef NEW_MP
    return mpState[proc].invalid_;
#else
    return invalid;
#endif
}

BOOL cfleGet(int proc)
{
#ifdef NEW_MP
    return mpState[proc].cfle_;
#else
    return cfle;
#endif
}

REG crGet(int proc, int i)
{
#ifdef NEW_MP
    return mpState[proc].crs_[i];
#else
    return crs[i];
#endif
}

REG bkrGet(int proc, int i)
{
#ifdef NEW_MP
    return mpState[proc].bankedGrs_[i].val;
#else
    return bankedGrs[i].val;
#endif
}

REG bkrNatGet(int proc, int i)
{
#ifdef NEW_MP
    return mpState[proc].bankedGrs_[i].nat;
#else
    return bankedGrs[i].nat;
#endif
}

REG rrGet(int proc, int i)
{
#ifdef NEW_MP
    return mpState[proc].rrs_[i];
#else
    return rrs[i];
#endif
}

REG pkrGet(int proc, int i)
{
#ifdef NEW_MP
    return mpState[proc].pkrs_[i];
#else
    return pkrs[i];
#endif
}

REG dbrGet(int proc, int i)
{
#ifdef NEW_MP
    return mpState[proc].dbrs_[i];
#else
    return dbrs[i];
#endif
}

REG ibrGet(int proc, int i)
{
#ifdef NEW_MP
    return mpState[proc].ibrs_[i];
#else
    return ibrs[i];
#endif
}

REG pmcGet(int proc, int i)
{
#ifdef NEW_MP
    return mpState[proc].pmcs_[i];
#else
    return pmcs[i];
#endif
}

REG pmdGet(int proc, int i)
{
#ifdef NEW_MP
    return mpState[proc].pmds_[i];
#else
    return pmds[i];
#endif
}

REG cpuidGet(int proc, int i)
{
#ifdef NEW_MP
    return mpState[proc].cpuids_[i];
#else
    return cpuids[i];
#endif
}

BOOL ipSet(int proc, REG val)
{
#ifdef NEW_MP
    int save = curPid;

    curPid = proc;
    if (!PSR_IS && ((val & 0x3) || SLOT(val) > 2)) {
	curPid = save;
	return NO;
    }
    ip = val;
    icp = abi ? setIcp() : NULL;
    curPid = save;
#else
    if (!PSR_IS && ((val & 0x3) || SLOT(val) > 2))
	return NO;
    ip = val;
    icp = abi ? setIcp() : NULL;
#endif
    return YES;
}

BOOL psrSet(int proc, REG val)
{
#ifdef NEW_MP
    BOOL psr_bn;
    int save = curPid;

    curPid = proc;
    psr_bn = PSR_BN;
    if (reservedIpsrField(val)) {
	curPid = save;
	return NO;
    }
    psr = val;
    if (PSR_BN != psr_bn)
	switchBanks();
    curPid = save;
#else
    BOOL psr_bn = PSR_BN;

    if (reservedIpsrField(val))
	return NO;
    psr = val;
    if (PSR_BN != psr_bn)
	switchBanks();
#endif
    return YES;
}

BOOL grSet(int proc, int i, REG val)
{
    REG old;
    BOOL nat;
#ifdef NEW_MP
    int save = curPid;

    curPid = proc;
    if (!i || i >= GR_STK_BASE+sof) {
	curPid = save;
	return NO;
    }
    GrRd(i, old, nat, NO);
    (void)old;
    GrWrtx(i, val, nat);
    curPid = save;
#else
    if (!i || i >= GR_STK_BASE+sof)
	return NO;
    GrRd(i, old, nat, NO);
    (void)old;
    GrWrtx(i, val, nat);
#endif
    return YES;
}

BOOL grNatSet(int proc, int i, BOOL nat)
{
    REG val;
    BOOL old;
#ifdef NEW_MP
    int save = curPid;

    curPid = proc;
    if (!i || i >= GR_STK_BASE+sof) {
	curPid = save;
	return NO;
    }
    GrRd(i, val, old, NO);
    (void)old;
    GrWrtx(i, val, nat & 1);
    curPid = save;
#else
    if (!i || i >= GR_STK_BASE+sof)
	return NO;
    GrRd(i, val, old, NO);
    (void)old;
    GrWrtx(i, val, nat & 1);
#endif
    return YES;
}

BOOL phyGrSet(int proc, int i, REG val)
{
    if (!i && val)
	return NO;
#ifdef NEW_MP
    mpState[proc].grs_[i].val = val;
#else
    grs[i].val = val;
#endif
    return YES;
}

BOOL phyGrNatSet(int proc, int i, BOOL nat)
{
    if (!i && nat)
	return NO;
#ifdef NEW_MP
    mpState[proc].grs_[i].nat = nat & 1;
#else
    grs[i].nat = nat & 1;
#endif
    return YES;
}

/* XXX - why isn't 3rd arg of type BYTE or BOOL? */
BOOL frSignSet(int proc, int i, REG val)
{
    FREG fr;
    BYTE sign;
    WORD exp;
    DWORD mant;
#ifdef NEW_MP
    int save = curPid;
#endif

    if (i < 2 || val > 1)
	return NO;
#ifdef NEW_MP
    curPid = proc;
#endif
    fr = FrRd(i);
    freg2spill(fr, &sign, &exp, &mant);
    fr = fill2freg(val, exp, mant);
    FrWrt(i, fr);
#ifdef NEW_MP
    curPid = save;
#endif
    return YES;
}

BOOL frExpSet(int proc, int i, WORD exp)
{
    FREG fr;
    BYTE sign;
    WORD oldexp;
    DWORD mant;

    if (i < 2 || exp > 0x1FFFF)
	return NO;
    fr = FrRd(i);
    freg2spill(fr, &sign, &oldexp, &mant);
    fr = fill2freg(sign, exp, mant);
    FrWrt(i, fr);
    return YES;
}

BOOL frMantSet(int proc, int i, DWORD mant)
{
    FREG fr;
    BYTE sign;
    WORD exp;
    DWORD oldmant;

    if (i < 2)
	return NO;
    fr = FrRd(i);
    freg2spill(fr, &sign, &exp, &oldmant);
    fr = fill2freg(sign, exp, mant);
    FrWrt(i, fr);
    return YES;
}

BOOL phyFrSignSet(int proc, int i, BYTE sign)
{
    BYTE oldsign;
    WORD exp;
    DWORD mant;

    if (i < 2)
	return sign ? NO : YES;
#ifdef NEW_MP
    freg2spill(mpState[proc].frs_[i], &oldsign, &exp, &mant);
    mpState[proc].frs_[i] = fill2freg(sign, exp, mant);
#else
    freg2spill(frs[i], &oldsign, &exp, &mant);
    frs[i] = fill2freg(sign, exp, mant);
#endif
    return YES;
}

BOOL phyFrExpSet(int proc, int i, WORD exp)
{
    BYTE sign;
    WORD oldexp;
    DWORD mant;

    if (!i)
	return exp == 0 ? YES : NO;
    if (i == 1)
	return exp == 0xFFFF ? YES : NO;
#ifdef NEW_MP
    freg2spill(mpState[proc].frs_[i], &sign, &oldexp, &mant);
    mpState[proc].frs_[i] = fill2freg(sign, exp, mant);
#else
    freg2spill(frs[i], &sign, &oldexp, &mant);
    frs[i] = fill2freg(sign, exp, mant);
#endif
    return YES;
}

BOOL phyFrMantSet(int proc, int i, DWORD mant)
{
    BYTE sign;
    WORD exp;
    DWORD oldmant;

    if (!i)
	return mant == 0 ? YES : NO;
    if (i == 1)
	return mant == 0x8000000000000000ULL ? YES : NO;
#ifdef NEW_MP
    freg2spill(mpState[proc].frs_[i], &sign, &exp, &oldmant);
    mpState[proc].frs_[i] = fill2freg(sign, exp, mant);
#else
    freg2spill(frs[i], &sign, &exp, &oldmant);
    frs[i] = fill2freg(sign, exp, mant);
#endif
    return YES;
}

BOOL prSet(int proc, int i, BOOL val)
{
    if (!i && !val)
	return NO;
    PrWrt(i, val & 1);
    return YES;
}

BOOL phyPrSet(int proc, int i, BOOL val)
{
    if (!i && !val)
	return NO;
#ifdef NEW_MP
    mpState[proc].prs_[i] = val & 1;
#else
    prs[i] = val & 1;
#endif
    return YES;
}

BOOL brSet(int proc, int i, REG val)
{
#ifdef NEW_MP
    mpState[proc].brs_[i] = val;
#else
    brs[i] = val;
#endif
    return YES;
}

BOOL arSet(int proc, int i, REG val)
{
    if (reservedAR(i) && val/* || reservedArField(i, &val)*/)
	return NO;
#ifdef NEW_MP
    mpState[proc].ars_[i] = val;
#else
    ars[i] = val;
#endif
    return YES;
}

void cfmSet(int proc, REG val)
{
    sof  = XFM_SOF(val);
    soil = XFM_SOIL(val);
    sor  = XFM_SOR(val) << 3;
    rrbg = XFM_RRBG(val);
    rrbf = XFM_RRBF(val);
    rrbp = XFM_RRBP(val);
}

BOOL rrbgSet(int proc, REG val)
{
   if (val >= GR_ROT_SIZE)
	return NO;
#ifdef NEW_MP
    mpState[proc].rrbg_ = val;
#else
   rrbg = val;
#endif
   return YES;
}

BOOL rrbfSet(int proc, REG val)
{
   if (val >= FR_ROT_SIZE)
	return NO;
#ifdef NEW_MP
    mpState[proc].rrbf_ = val;
#else
   rrbf = val;
#endif
   return YES;
}

BOOL rrbpSet(int proc, REG val)
{
   if (val >= PR_ROT_SIZE)
	return NO;
#ifdef NEW_MP
    mpState[proc].rrbp_ = val;
#else
   rrbp = val;
#endif
   return YES;
}

/* XXX - Add bounds checking */
void bolSet(int proc, unsigned val)
{
#ifdef NEW_MP
    mpState[proc].bol_ = val;
#else
    bol = val;
#endif
}

/* XXX - Add bounds checking */
void dirtySet(int proc, int val)
{
#ifdef NEW_MP
    mpState[proc].dirty_ = val;
#else
    dirty = val;
#endif
}

/* XXX - Add bounds checking (plus check dirty/dirtyNat combo later) */
void dirtyNatSet(int proc, int val)
{
#ifdef NEW_MP
    mpState[proc].dirtyNat_ = val;
#else
    dirtyNat = val;
#endif
}

/* XXX - Add bounds checking */
void cleanSet(int proc, int val)
{
#ifdef NEW_MP
    mpState[proc].clean_ = val;
#else
    clean = val;
#endif
}

/* XXX - Add bounds checking */
void cleanNatSet(int proc, int val)
{
#ifdef NEW_MP
    mpState[proc].cleanNat_ = val;
#else
    cleanNat = val;
#endif
}

/* XXX - Add bounds checking */
void invalidSet(int proc, int val)
{
#ifdef NEW_MP
    mpState[proc].invalid_ = val;
#else
    invalid = val;
#endif
}

void cfleSet(int proc, BOOL val)
{
#ifdef NEW_MP
    mpState[proc].cfle_ = val & 1;
#else
    cfle = val & 1;
#endif
}

BOOL crSet(int proc, int i, REG val)
{
    if ((reservedCR(i) && val) || reservedCrField(i, &val))
	return NO;
#ifdef NEW_MP
    mpState[proc].crs_[i] = val;
#else
    crs[i] = val;
#endif
    return YES;
}

BOOL bkrSet(int proc, int i, REG val)
{
#ifdef NEW_MP
    mpState[proc].bankedGrs_[i].val = val;
#else
    bankedGrs[i].val = val;
#endif
    return YES;
}

BOOL bkrNatSet(int proc, int i, BOOL nat)
{
#ifdef NEW_MP
    mpState[proc].bankedGrs_[i].nat = nat & 1;
#else
    bankedGrs[i].nat = nat & 1;
#endif
    return YES;
}

BOOL rrSet(int proc, int i, REG val)
{
#ifdef NEW_MP
    mpState[proc].rrs_[i] = val;
#else
    rrs[i] = val;
#endif
    return YES;
}

BOOL pkrSet(int proc, int i, REG val)
{
#ifdef NEW_MP
    mpState[proc].pkrs_[i] = val;
#else
    pkrs[i] = val;
#endif
    return YES;
}

BOOL dbrSet(int proc, int i, REG val)
{
#ifdef NEW_MP
    mpState[proc].dbrs_[i] = val;
#else
    dbrs[i] = val;
#endif
    return YES;
}

BOOL ibrSet(int proc, int i, REG val)
{
#ifdef NEW_MP
    mpState[proc].ibrs_[i] = val;
#else
    ibrs[i] = val;
#endif
    return YES;
}

BOOL pmdSet(int proc, int i, REG val)
{
#ifdef NEW_MP
    mpState[proc].pmds_[i] = val;
#else
    pmds[i] = val;
#endif
    return YES;
}

BOOL pmcSet(int proc, int i, REG val)
{
#ifdef NEW_MP
    mpState[proc].pmcs_[i] = val;
#else
    pmcs[i] = val;
#endif
    return YES;
}

BOOL getLp64(int proc)
{
#ifdef NEW_MP
    return mpState[proc].lp64_;
#else
    return lp64;
#endif
}

void setLp64(int proc, BOOL val)
{
#ifdef NEW_MP
    mpState[proc].lp64_ = val;
#else
    lp64 = val;
#endif
}

ADDR heapGet(int proc)
{
#ifdef NEW_MP
    return mpState[proc].heap_;
#else
    return heap;
#endif
}

void heapSet(int proc, ADDR adr)
{
#ifdef NEW_MP
    mpState[proc].heap_ = adr;
#else
    heap = adr;
#endif
}

void getGrMapInfo(char buf[])
{
    int i;
    char *p = buf;

    for (i = GR_STK_BASE; i < NGRS; i++)
	p += sprintf(p, "%3d %3d %3d\n", i, grmap[i], PHYS_GR(i));
}

REG getExited(void)
{
    return exited;
}

void setExited(BOOL val)
{
    exited = val;
}

#ifdef NEW_MP
/*
 * copy the state of the current process to the new process
 */
void stateFork(unsigned old, unsigned new)
{
    unsigned i;

    mpState[new].parentPid_ = old+2;
    for (i = 0; i < MAX_NGRS; i++)
	mpState[new].grs_[i] = mpState[old].grs_[i];
    for (i = 0; i < NFRS; i++)
	mpState[new].frs_[i] = mpState[old].frs_[i];
    for (i = 0; i < NPRS; i++)
	mpState[new].prs_[i] = mpState[old].prs_[i];
    for (i = 0; i < NBRS; i++)
	mpState[new].brs_[i] = mpState[old].brs_[i];
    for (i = 0; i < NARS; i++)
	mpState[new].ars_[i] = mpState[old].ars_[i];
    for (i = 0; i < NFRS+FR_ROT_SIZE; i++)
	mpState[new].frmap_[i] = mpState[old].frmap_[i];
    for (i = 0; i < GR_STK_BASE+N_STACK_VIRT; i++)
	mpState[new].grmap_[i] = mpState[old].grmap_[i];
    mpState[new].grs_[8].val = 0;
    mpState[new].grs_[8].nat = NO;
    mpState[new].grs_[9].val = 1;	/* indicates child */
    mpState[new].grs_[9].nat = NO;
    mpState[new].prs_[14] = 0;
    mpState[new].prs_[15] = 1;
    mpState[new].psr_ = mpState[old].psr_;
    mpState[new].lp64_ = mpState[old].lp64_;
    mpState[new].heap_ = mpState[old].heap_;
    mpState[new].sof_ = mpState[old].sof_;
    mpState[new].soil_ = mpState[old].soil_;
    mpState[new].sor_ = mpState[old].sor_;
    mpState[new].rrbg_ = mpState[old].rrbg_;
    mpState[new].rrbf_ = mpState[old].rrbf_;
    mpState[new].rrbp_ = mpState[old].rrbp_;
    mpState[new].bol_ = mpState[old].bol_;
    mpState[new].dirty_ = mpState[old].dirty_;
    mpState[new].dirtyNat_ = mpState[old].dirtyNat_;
    mpState[new].clean_ = mpState[old].clean_;
    mpState[new].cleanNat_ = mpState[old].cleanNat_;
    mpState[new].invalid_ = mpState[old].invalid_;
    mpState[new].cfle_ = mpState[old].cfle_;
    mpState[new].ip_ = mpState[old].ip_ + 4;
    mpState[new].icp_ = setIcp();
}

#endif
