/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Execution Routines
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
#include "bits.h"
#include "types.h"
#include "fields.h"
#include "state.h"
#include "sim.h"
#include "simmem.h"

Status addEx(INSTINFO *info)
{
    ALU_2OP_EX(SRC1 + SRC2);
    return 0;
}

Status add1Ex(INSTINFO *info)
{
    ALU_2OP_EX(SRC1 + SRC2 + 1);
    return 0;
}

Status addp4Ex(INSTINFO *info)
{
    ALU_2OP_EX(BitfX(SRC2,32,2) << 61 | (WORD)(SRC1 + SRC2));
    return 0;
}

Status andEx(INSTINFO *info)
{
    ALU_2OP_EX(SRC1 & SRC2);
    return 0;
}

Status andcEx(INSTINFO *info)
{
    ALU_2OP_EX(SRC1 & ~SRC2);
    return 0;
}

Status allocEx(INSTINFO *info)
{
    DST1 = SRC1;
    return 0;
}

Status brCallEx(INSTINFO *info)
{
    int callNat;

    DST1 = SRC2;
    DST2 = SRC1;
    /* XXX - can ORing of PSR_CPL be avoided in application mode? */
    /* XXX - the following might need to be moved to the write stage */
    PFS_PPL  = PSR_CPL;
    PFS_PEC  = EC_CNT;
    PFM_RRBP = rrbp;
    PFM_RRBF = rrbf;
    PFM_RRBG = rrbg;
    PFM_SOR  = sor >> 3;
    PFM_SOIL = soil;
    PFM_SOF  = sof;

    /* equivalent to SDM' call to rse_preserve_frame(soil) from here... */
    if (bol + soil > (N_STACK_PHYS - 1)) {
	bol -= N_STACK_PHYS - soil;
    } else
	bol += soil;
    callNat = NATS_GROW(BSP, soil);
    BSP += (soil + callNat) * 8;
    dirty += soil;
    dirtyNat += callNat;
    /* ---------------------------------------------------- ... to here */

    sof -= soil;
    return 0;
}

Status brCexitEx(INSTINFO *info)
{
    DST1 = SRC1;
    DST2 = !(LC > 0 || EC_CNT > 1);
    if (LC > 0) {
	LC--;
	PrWrt(63, 1);
	rotate_regs();
    } else if (EC_CNT > 0) {
	EC_CNT--;
	PrWrt(63, 0);
	rotate_regs();
    } else
	PrWrt(63, 0);
    return 0;
}

Status brCloopEx(INSTINFO *info)
{
    DST1 = SRC1;
    DST2 = LC > 0;
    if (LC > 0)
	LC--;
    return 0;
}

Status brCondiEx(INSTINFO *info)
{
    DST1 = SRC1 & ~(ADDR)0xF;
    return 0;
}

Status brpEx(INSTINFO *info)
{
    DST1 = SRC1;
    traceBrPred(DST1);
    return 0;
}

Status brpiEx(INSTINFO *info)
{
    DST1 = SRC1 & ~(ADDR)0xF;
    traceBrPred(DST1);
    return 0;
}

Status brCtopEx(INSTINFO *info)
{
    DST1 = SRC1;
    DST2 = LC > 0 || EC_CNT > 1;
    if (LC > 0) {
	LC--;
	PrWrt(63, 1);
	rotate_regs();
    } else if (EC_CNT > 0) {
	EC_CNT--;
	PrWrt(63, 0);
	rotate_regs();
    } else
	PrWrt(63, 0);
    return 0;
}

Status brIAEx(INSTINFO *info)
{
    if (SRC2 || BSPST != BSP) {
	illegalOpFault();
	return StFault;
    }
    if (PSR_DI) {
	disabledInstSetTransitionFault();
	return StFault;
    }
    DST1 = SRC1;
    return 0;
}

Status brRetEx(INSTINFO *info)
{
    int growth, old_frame = sof;

    DST1 = SRC1 & ~(ADDR)0xF;
    growth = (int)PFM_SOF - (int)PFM_SOIL - (int)sof;
    /* XXX - the following might need to be moved to the write stage */
    EC_CNT = PFS_PEC;
    rrbp   = PFM_RRBP;
    rrbf   = PFM_RRBF;
    rrbg   = PFM_RRBG;
    sor    = PFM_SOR << 3;
    soil   = PFM_SOIL;
    sof    = PFM_SOF;
    if (bol < soil) {
	bol += N_STACK_PHYS - soil;
    } else
	bol -= soil;
    /* XXX */
    updateGrMap(0);
    rse_restore_frame(soil, growth, old_frame);
    if (cfle) st |= ST_CHECK;
    /* Demote privilege */
    /* XXX - can this be avoided in application mode? */
    if (PSR_CPL < PFS_PPL) {
	PSR_CPL = PFS_PPL;
	if (PSR_LP) {
	    unsigned slot = SLOT(ip);

	    if (!unixABI && PSR_IC)
		IIPA = ip;
	    ip = DST1;
	    if (unimplAddr(ip))
		unimplInstructionAddressTrap(slot, StTrap);
	    else
		lowerPrivilegeTransferTrap(slot);
	    return StTrap;
	}
    }
    tracePrevPFS(PFS);
    return 0;
}

Status brWexitEx(INSTINFO *info)
{
    DST1 = SRC1;
    DST2 = !(SRC2 || EC_CNT > 1);
    PrWrt(63, 0);
    if (SRC2)
	rotate_regs();
    else if (EC_CNT > 0) {
	EC_CNT--;
	rotate_regs();
    }
    return 0;
}

Status brWtopEx(INSTINFO *info)
{
    DST1 = SRC1;
    DST2 = SRC2 || EC_CNT > 1;
    PrWrt(63, 0);
    if (SRC2)
	rotate_regs();
    else if (EC_CNT > 0) {
	EC_CNT--;
	rotate_regs();
    }
    return 0;
}

Status breakEx(INSTINFO *info)
{
    if (!SRC2 && isbpt(SRC1) != -1) {
	static char sym[40];

	symAddrtoName(SRC1, sym, 4, NO, -32);
	progStop("Breakpoint (IA-64) at %s\n", sym);
	/* counts and commands in breakpoints are not implemented yet. */
    }
/* XXX - move these or get them from a header <machine/break.h>? */
#define BREAK_DE_SVC_LOADED	1
#define BREAK_DE_LIB_LOADED	2
#define BREAK_DE_LIB_UNLOADED	3
#define BREAK_DE_LOAD_COMPLETE	4
#define BREAK_DE_BOR		5
#define BREAK_DEBUG_EVENT	0xF0327ULL
    /* XXX - use #defines for the constants here */
    if (unixABI &&
#if defined __linux__
	(imm64 == 0x100000)
#else /* !defined __linux__ */
	!SRC2 && SRC1 == 0xE000000000000000ULL
#endif /* !defined __linux__ */
	)
	doSyscall(SC_NUM, ARG0, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,
		  &SC_RET, &SC_STAT);
    else if (unixABI && (imm64 == BREAK_DEBUG_EVENT || imm64 == 0x70327ULL))
	switch ((int)ARG0) {
	    case BREAK_DE_SVC_LOADED:
	    case BREAK_DE_LIB_LOADED:
		addLM(ARG1, ARG2);
		break;
	    case BREAK_DE_LIB_UNLOADED:
		break;
	    case BREAK_DE_LOAD_COMPLETE:
		break;
	    case BREAK_DE_BOR:
		break;
	    default:
		breakInstFault(SRC2);
		return StFault;
	}
    else if (!unixABI && (imm64 == 0x80000ULL || imm64 == 0x80001ULL)) {
	GrRd(15, SC_NUM, SRCNAT1, NO);	/* XXX - check SRCNAT1? */
	doSSC(SC_NUM, ARG0, ARG1, ARG2, ARG3, &SC_RET);
        st |= ST_CHECK;
    } else if (!unixABI && (SRC1 >= 0x12FFF000ULL && SRC1 < 0x13000000ULL)) {
	SC_RET = doFW(imm64, ARG0, ARG1, ARG2, ARG3);
    /* XXX - for CSY Tests, halt on "break 0" */
    } else if (!unixABI && !imm64)
	progExit("Halting Simulation\n");
    else {
	breakInstFault(SRC2);
	return StFault;
    }
    return 0;
}

Status chkarNcEx(INSTINFO *info)
{
    DST1 = SRC1;
    if (ALAT_WRITE_CHECK)
	DST2 = alat_cmp(NO, r1, NO);
    else
	DST2 = 1;
    return 0;
}

Status chkarClrEx(INSTINFO *info)
{
    DST1 = SRC1;
    if (ALAT_WRITE_CHECK)
	DST2 = alat_cmp(NO, r1, YES);
    else
	DST2 = 1;
    return 0;
}

Status chkafNcEx(INSTINFO *info)
{
    DST1 = SRC1;
    if (ALAT_WRITE_CHECK)
	DST2 = alat_cmp(YES, f1, NO);
    else
	DST2 = 1;
    return 0;
}

Status chkafClrEx(INSTINFO *info)
{
    DST1 = SRC1;
    if (ALAT_WRITE_CHECK)
	DST2 = alat_cmp(YES, f1, YES);
    else
	DST2 = 1;
    return 0;
}

Status cmp4eqEx(INSTINFO *info)
{
    CMP_EX((WORD)SRC1 == (WORD)SRC2);
    return 0;
}

Status cmp4neEx(INSTINFO *info)
{
    CMP_EX((WORD)SRC1 != (WORD)SRC2);
    return 0;
}

Status cmp4gtEx(INSTINFO *info)
{
    CMP_EX((int)SRC1 > (int)SRC2);
    return 0;
}

Status cmp4geEx(INSTINFO *info)
{
    CMP_EX((int)SRC1 >= (int)SRC2);
    return 0;
}

Status cmp4ltEx(INSTINFO *info)
{
    CMP_EX((int)SRC1 < (int)SRC2);
    return 0;
}

Status cmp4ltuEx(INSTINFO *info)
{
    CMP_EX((WORD)SRC1 < (WORD)SRC2);
    return 0;
}

Status cmp4leEx(INSTINFO *info)
{
    CMP_EX((int)SRC1 <= (int)SRC2);
    return 0;
}

Status cmpeqEx(INSTINFO *info)
{
    CMP_EX(SRC1 == SRC2);
    return 0;
}

Status cmpneEx(INSTINFO *info)
{
    CMP_EX(SRC1 != SRC2);
    return 0;
}

Status cmpgtEx(INSTINFO *info)
{
    CMP_EX((long long)SRC1 > (long long)SRC2);
    return 0;
}

Status cmpgeEx(INSTINFO *info)
{
    CMP_EX((long long)SRC1 >= (long long)SRC2);
    return 0;
}

Status cmpltEx(INSTINFO *info)
{
    CMP_EX((long long)SRC1 < (long long)SRC2);
    return 0;
}

Status cmpltuEx(INSTINFO *info)
{
    CMP_EX(SRC1 < SRC2);
    return 0;
}

Status cmpleEx(INSTINFO *info)
{
    CMP_EX((long long)SRC1 <= (long long)SRC2);
    return 0;
}

Status copyfromgrEx(INSTINFO *info)
{
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    DST1 = SRC1;
    return 0;
}

Status copytogrEx(INSTINFO *info)
{
    DST1 = SRC1;
    DSTNAT1 = 0;
    return 0;
}

Status copyEx(INSTINFO *info)
{
    DST1 = SRC1;
    return 0;
}

Status coverEx(INSTINFO *info)
{
    int coverNat;

    /* equivalent to SDM' call to rse_preserve_frame(sof) from here... */
    if (bol + sof > (N_STACK_PHYS - 1)) {
	bol -= N_STACK_PHYS - sof;
    } else
	bol += sof;
    coverNat = NATS_GROW(BSP, sof);
    BSP += (sof + coverNat) * 8;
    dirty += sof;
    dirtyNat += coverNat;
    /* --------------------------------------------------- ... to here */

    if (!PSR_IC) {
	IFM_RRBP = rrbp;
	IFM_RRBF = rrbf;
	IFM_RRBG = rrbg;
	IFM_SOR  = sor >> 3;
	IFM_SOIL = soil;
	IFM_SOF  = sof;
	IFS_V    = 1;
    }
    sof = soil = sor = rrbg = rrbf = rrbp = 0;
    updateGrMap(0);
    return 0;
}

Status czx1lEx(INSTINFO *info)
{
    REG mask = 0xff00000000000000ULL, inx;

    for (inx = 0; inx < 8; inx++) {
	if (!(SRC1 & mask))
	    break;
	mask >>= 8;
    }
    ALU_1OP_EX(inx);
    return 0;
}

Status czx1rEx(INSTINFO *info)
{
    REG mask = 0xff, inx;

    for (inx = 0; inx < 8; inx++) {
	if (!(SRC1 & mask))
	    break;
	mask <<= 8;
    }
    ALU_1OP_EX(inx);
    return 0;
}

Status czx2lEx(INSTINFO *info)
{
    REG mask = 0xffff000000000000ULL, inx;

    for (inx = 0; inx < 4; inx++) {
	if (!(SRC1 & mask))
	    break;
	mask >>= 16;
    }
    ALU_1OP_EX(inx);
    return 0;
}

Status czx2rEx(INSTINFO *info)
{
    REG mask = 0xffff, inx;

    for (inx = 0; inx < 4; inx++) {
	if (!(SRC1 & mask))
	    break;
	mask <<= 16;
    }
    ALU_1OP_EX(inx);
    return 0;
}

Status depEx(INSTINFO *info)
{
    ALU_2OP_EX((SRC2 & ~(ONES(LEN) << POS)) | ((SRC1 & ONES(LEN)) << POS));
    return 0;
}

Status depzEx(INSTINFO *info)
{
    ALU_1OP_EX((SRC1 & ONES(len6)) << POS);
    return 0;
}

Status extrEx(INSTINFO *info)
{
    if (POS + LEN > 64)
	LEN = 64 - POS;
    ALU_1OP_EX(((long long)(SRC1 << (64-POS-LEN))) >> (64-LEN));
    return 0;
}

Status extruEx(INSTINFO *info)
{
    if (POS + LEN > 64)
	LEN = 64 - POS;
    ALU_1OP_EX((SRC1 >> POS) & ONES(LEN));
    return 0;
}

Status flushrsEx(INSTINFO *info)
{
    while (dirty + dirtyNat > 0) {
	if (rse_store() == -1)
	    return StFault;
	/* The SDM includes call to... */
	/* deliver_unmasked_pending_external_interrupt(); */
    }
    return 0;
}

Status hintEx(INSTINFO *info)
{
    /* execute_hint() */
    return 0;
}

Status invalaEx(INSTINFO *info)
{
    if (ALAT_WRITE_CHECK)
	alat_inval_all_entries();
    return 0;
}

Status invalar1Ex(INSTINFO *info)
{
    if (ALAT_WRITE_CHECK)
	alat_inval_single_entry(NO, r1);
    return 0;
}

Status invalaf1Ex(INSTINFO *info)
{
    if (ALAT_WRITE_CHECK)
	alat_inval_single_entry(YES, f1);
    return 0;
}

Status loadrsEx(INSTINFO *info)
{
    ADDR tmp_ptr;
    ADDR BspLoad = BSPST - (clean + cleanNat) * 8;
    int words_to_load;

    if (RSC_MODE || (RSC_LOADRS && sof)) {
	illegalOpFault();
	return StFault;
    }

    /* equivalent to SDM' call to rse_ensure_regs_loaded(RSC_LOADRS) up to...*/
    words_to_load = (RSC_LOADRS >> 3) - (clean + cleanNat + dirty + dirtyNat) ;
    if (words_to_load >= 0) {
	dirtyNat += cleanNat;
	dirty    += clean;
	BSPST     = BSP - (dirty + dirtyNat) * 8;
	cleanNat  = clean = 0;
	while (words_to_load > 0) {
	    int regs_loaded;

	    if (dirty == N_STACK_PHYS && NAT_COLLECT_BIT(BspLoad - 8) != 63) {
		illegalOpFault();
		return StFault;
	    }
	    if ((regs_loaded = rse_load()) == -1)
		return StFault;
	    if (regs_loaded) {
		dirty++;
		clean--;
	    } else {
		dirtyNat++;
		cleanNat--;
	    }
	    BSPST    = BSP - (dirty + dirtyNat) * 8;
	    BspLoad -= 8;
	    words_to_load--;
	}
    } else {
	tmp_ptr  = BSP - (RSC_LOADRS & ~0x7);
	dirtyNat = ((long long)BSP >> 9) - ((long long)tmp_ptr >> 9);
	dirty    = (RSC_LOADRS >> 3) - dirtyNat;
	BSPST    = BSP - (dirty + dirtyNat) * 8;
	clean    = cleanNat = 0;
	invalid  = N_STACK_PHYS - (sof + dirty + clean);
    }
    /* ------------------------------------------------------------ ... here */

    /* SDM also makes RNAT undefined */

    return 0;
}

Status movImmEx(INSTINFO *info)
{
    DST1 = imm64;
    return 0;
}

Status movlEx(INSTINFO *info)
{
    DST1 = imm64;
    DSTNAT1 = 0;
    return 0;
}

Status movprgrEx(INSTINFO *info)
{
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    DST1 = SRC1;
    DST2 = sign_ext(imm64, 17);
}

Status movtopsrumEx(INSTINFO *info)
{
    /* SRC2 is psr */
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (reservedUserMaskField(SRC1)) {
	reservedRegFieldFault(0);
	return StFault;
    }
    DST1 = SRC2;
    X_PSR_BE(DST1) = X_PSR_BE(SRC1);
    if (!X_PSR_SP(SRC2))
	X_PSR_UP(DST1) = X_PSR_UP(SRC1);
    BitfR(DST1,58,3) = BitfX(SRC1,58,3);
    return 0;
}

Status movfrompsrumEx(INSTINFO *info)
{
    DST1 = X_PSR_UM(SRC1);
    DSTNAT1 = 0;
    return 0;
}

Status movfromarmEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r1) || reservedARm(ar3)) {
	illegalOpFault();
	return StFault;
    }
    if ((ar3 == RNAT_ID || ar3 == BSPST_ID) && RSC_MODE) {
	illegalOpFault();
	return StFault;
    }
    /* XXX - move PSR_SI and PSR_CPL reads to read function? */
    if (ar3 == ITC_ID && PSR_SI && PSR_CPL) {
	privRegFault();
	return StFault;
    }
    DST1 = SRC1;
    DSTNAT1 = 0;
    return 0;
}

Status movfromariEx(INSTINFO *info)
{
    if (reservedARi(ar3)) {
	illegalOpFault();
	return StFault;
    }
    DST1 = SRC1;
    DSTNAT1 = 0;
    return 0;
}

Status movtoarmEx(INSTINFO *info)
{
    if (reservedARm(ar3) || ar3 == BSP_ID) {
	illegalOpFault();
	return StFault;
    }
    if ((ar3 == RNAT_ID || ar3 == BSPST_ID) && RSC_MODE) {
	illegalOpFault();
	return StFault;
    }
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (ar3 == RSC_ID && reservedRscField(SRC1)) {
	reservedRegFieldFault(0);
	return StFault;
    }
    if (ar3 == FPSR_ID && reservedFpsrField(SRC1)) {
	reservedRegFieldFault(0);
	return StFault;
    }
    /* Assumes K0_ID is 0 */
    if ((ar3 == ITC_ID || ar3 <= K7_ID) && PSR_CPL) {
	privRegFault();
	return StFault;
    }
    if (ignoredAR(ar3))
	return StSuccess;
    if (ar3 == BSPST_ID) {
	DST1 = SRC1 & ~7;
	/* equivalent to SDM' call to rse_update_internal_stack_pointers() */
	/* from here... */
	dirtyNat = NATS_GROW(DST1, dirty);
	BSP = DST1 + (dirty + dirtyNat) * 8;
	invalid += clean;
	clean = cleanNat = 0;
	/* --------------------------------------------------- ... to here */
	/* SDM also makes RNAT undefined */
    } else if (ar3 == RNAT_ID)
	DST1 = BitfX(SRC1,1,63);
    else if (ar3 == RSC_ID && XRSC_PL(SRC1) < PSR_CPL) {
	DST1 = SRC1;
	XRSC_PL(DST1) = PSR_CPL;
    } else
	DST1 = SRC1;
    return 0;
}

Status movtoariEx(INSTINFO *info)
{
    if (reservedARi(ar3)) {
	illegalOpFault();
	return StFault;
    }
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (ar3 == PFS_ID && reservedPfsField(SRC1)) {
	reservedRegFieldFault(0);
	return StFault;
    }
    if (ignoredAR(ar3))
	return StSuccess;
    if (ar3 == EC_ID)
	DST1 = BitfX(SRC1,58,6);
    else
	DST1 = SRC1;
    return 0;
}

Status movfromcpuidEx(INSTINFO *info)
{
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (B7(SRC1) >= NCPUIDS) {
	reservedRegFieldFault(0);
	return StFault;
    }
    DST1 = CpuidRd(B7(SRC1));
    DSTNAT1 = 0;
    return 0;
}

Status nopEx(INSTINFO *info)
{
    return 0;
}

Status orEx(INSTINFO *info)
{
    DST1 = SRC1 | SRC2;
    DSTNAT1 = SRCNAT1 | SRCNAT2;
    return 0;
}

Status proberEx(INSTINFO *info)
{
    unsigned tmp_pl;
    int ret;

    if (OUT_OF_FRAME(r1)) {	/* remove redundant check in write phase? */
	illegalOpFault();
	return StFault;
    }
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(PROBER_ACCESS);
	return StFault;
    }
    tmp_pl = BitfX(SRC1,62,2);
    /* XXX - Should PSR_CPL be read in read function? */
    if (tmp_pl < PSR_CPL)
	tmp_pl = PSR_CPL;
    if ((ret = probeLookup(SRC2, PROBER_ACCESS, tmp_pl)) == -1)
	return StFault;
    DST1 = ret;
    DSTNAT1 = 0;
    return 0;
}

Status probewEx(INSTINFO *info)
{
    unsigned tmp_pl;
    int ret;

    if (OUT_OF_FRAME(r1)) {	/* remove redundant check in write phase? */
	illegalOpFault();
	return StFault;
    }
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(PROBEW_ACCESS);
	return StFault;
    }
    tmp_pl = BitfX(SRC1,62,2);
    /* XXX - Should PSR_CPL be read in read function? */
    if (tmp_pl < PSR_CPL)
	tmp_pl = PSR_CPL;
    if ((ret = probeLookup(SRC2, PROBEW_ACCESS, tmp_pl)) == -1)
	return StFault;
    DST1 = ret;
    DSTNAT1 = 0;
    return 0;
}

Status proberwFEx(INSTINFO *info)
{
    unsigned tmp_pl;
    ADDR pa;

    if (SRCNAT2) {
	regNatConsumptionFault(PROBERW_ACCESS);
	return StFault;
    }
    tmp_pl = BitfX(SRC1,62,2);
    /* XXX - Should PSR_CPL be read in read function? */
    if (tmp_pl < PSR_CPL)
	tmp_pl = PSR_CPL;
    if (dtlbLookup(SRC2, 1, PROBERW_ACCESS, tmp_pl, PSR_DT, &pa) == -1)
	return StFault;
    return 0;
}

Status proberFEx(INSTINFO *info)
{
    unsigned tmp_pl;
    ADDR pa;

    if (SRCNAT2) {
	regNatConsumptionFault(PROBERF_ACCESS);
	return StFault;
    }
    tmp_pl = BitfX(SRC1,62,2);
    /* XXX - Should PSR_CPL be read in read function? */
    if (tmp_pl < PSR_CPL)
	tmp_pl = PSR_CPL;
    if (dtlbLookup(SRC2, 1, PROBERF_ACCESS, tmp_pl, PSR_DT, &pa) == -1)
	return StFault;
    return 0;
}

Status probewFEx(INSTINFO *info)
{
    unsigned tmp_pl;
    ADDR pa;

    if (SRCNAT2) {
	regNatConsumptionFault(PROBEWF_ACCESS);
	return StFault;
    }
    tmp_pl = BitfX(SRC1,62,2);
    /* XXX - Should PSR_CPL be read in read function? */
    if (tmp_pl < PSR_CPL)
	tmp_pl = PSR_CPL;
    if (dtlbLookup(SRC2, 1, PROBEWF_ACCESS, tmp_pl, PSR_DT, &pa) == -1)
	return StFault;
    return 0;
}

Status rumEx(INSTINFO *info)
{
    if (reservedUserMaskField(imm64)) {
	reservedRegFieldFault(0);
	return StFault;
    }
    DST1 = SRC1;
    X_PSR_BE(DST1) &= ~X_PSR_BE(imm64);
    if (!X_PSR_SP(SRC1))
	X_PSR_UP(DST1) &= ~X_PSR_UP(imm64);
    BitfR(DST1,58,3) &= ~BitfX(imm64,58,3);
    return 0;
}

Status shladdEx(INSTINFO *info)
{
    DST1 = (SRC1 << CNT) + SRC2;
    DSTNAT1 = SRCNAT1 | SRCNAT2;
    return 0;
}

Status shladdp4Ex(INSTINFO *info)
{
    DST1 = (WORD)((SRC1 << CNT) + SRC2);
    DST1 |= BitfX(SRC2,32,2) << 61;
    DSTNAT1 = SRCNAT1 | SRCNAT2;
    return 0;
}

Status shrpEx(INSTINFO *info)
{
    DST1 = CNT ? (SRC1 << (64-CNT)) | (SRC2 >> CNT) : SRC2;
    DSTNAT1 = SRCNAT1 | SRCNAT2;
    return 0;
}

Status subEx(INSTINFO *info)
{
    DST1 = SRC1 - SRC2;
    DSTNAT1 = SRCNAT1 | SRCNAT2;
    return 0;
}

Status sub1Ex(INSTINFO *info)
{
    DST1 = SRC1 - SRC2 - 1;
    DSTNAT1 = SRCNAT1 | SRCNAT2;
    return 0;
}

Status sumEx(INSTINFO *info)
{
    if (reservedUserMaskField(imm64)) {
	reservedRegFieldFault(0);
	return StFault;
    }
    DST1 = SRC1;
    X_PSR_BE(DST1) |= X_PSR_BE(imm64);
    if (!X_PSR_SP(SRC1))
	X_PSR_UP(DST1) |= X_PSR_UP(imm64);
    BitfR(DST1,58,3) |= BitfX(imm64,58,3);
    return 0;
}

Status sxt1Ex(INSTINFO *info)
{
    DST1 = (long long)(SRC1 << 56) >> 56;
    DSTNAT1 = SRCNAT1;
    return 0;
}

Status sxt2Ex(INSTINFO *info)
{
    DST1 = (long long)(SRC1 << 48) >> 48;
    DSTNAT1 = SRCNAT1;
    return 0;
}

Status sxt4Ex(INSTINFO *info)
{
    DST1 = (long long)(SRC1 << 32) >> 32;
    DSTNAT1 = SRCNAT1;
    return 0;
}

Status tbitNzEx(INSTINFO *info)
{
    CMPRES1 = BitfX(SRC1,63-POS,1);
    CMPRES2 = !CMPRES1;
    DSTNAT1 = SRCNAT1;
    return 0;
}

Status tbitZEx(INSTINFO *info)
{
    if (QUAL) {
	CMPRES1 = !BitfX(SRC1,63-POS,1);
	CMPRES2 = !CMPRES1;
    } else {
	CMPRES1 = 0;
	CMPRES2 = 0;
    }
    DSTNAT1 = SRCNAT1;
    return 0;
}

Status tnatNzEx(INSTINFO *info)
{
    CMPRES1 = SRCNAT1;
    CMPRES2 = !CMPRES1;
    DSTNAT1 = 0;
    return 0;
}

Status tnatZEx(INSTINFO *info)
{
    if (QUAL) {
	CMPRES1 = !SRCNAT1;
	CMPRES2 = !CMPRES1;
    } else {
	CMPRES1 = 0;
	CMPRES2 = 0;
    }
    DSTNAT1 = 0;
    return 0;
}

Status xorEx(INSTINFO *info)
{
    DST1 = SRC1 ^ SRC2;
    DSTNAT1 = SRCNAT1 | SRCNAT2;
    return 0;
}

Status zxt1Ex(INSTINFO *info)
{
    DST1 = (BYTE)SRC1;
    DSTNAT1 = SRCNAT1;
    return 0;
}

Status zxt2Ex(INSTINFO *info)
{
    DST1 = (HWORD)SRC1;
    DSTNAT1 = SRCNAT1;
    return 0;
}

Status zxt4Ex(INSTINFO *info)
{
    DST1 = (WORD)SRC1;
    DSTNAT1 = SRCNAT1;
    return 0;
}
