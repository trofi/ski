/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Instruction Write Routines
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
#include "trace.h"

void nullWrt(INSTINFO *info)
{
    /* NULL */
}

void oneGrWrt(INSTINFO *info)
{
    if (OUT_OF_FRAME(r1)) {
	illegalOpFault();
	return;
    }
    GrWrt(r1, DST1, DSTNAT1, info->pgrtgt ? &grs[info->pgrtgt - 1] : NULL);
}

void ld16Wrt(INSTINFO *info)
{
    if (OUT_OF_FRAME(r1)) {
	illegalOpFault();
	return;
    }
    GrWrt(r1, DST1, DSTNAT1, info->pgrtgt ? &grs[info->pgrtgt - 1] : NULL);
    ArWrt(CSD_ID, DST2);
}

void lfetchWrt(INSTINFO *info)
{
    GrWrt(r3, DST2, DSTNAT2, NO);
}

void twoGrWrt(INSTINFO *info)
{
    GrWrt(r3, DST2, DSTNAT2, NO);
    GrWrt(r1, DST1, DSTNAT1, info->pgrtgt ? &grs[info->pgrtgt - 1] : NULL);
}

void twoPrWrt(INSTINFO *info)
{
    if (DSTNAT1) {
	PrWrt(p1, 0);
	PrWrt(p2, 0);
    } else {
	PrWrt(p1, CMPRES1);
	PrWrt(p2, CMPRES2);
    }
}

void twoPrAndWrt(INSTINFO *info)
{
    if (!CMPRES1 || DSTNAT1) {
	PrWrt(p1, 0);
	PrWrt(p2, 0);
    }
}

void twoPrOrWrt(INSTINFO *info)
{
    if (CMPRES1 && !DSTNAT1) {
	PrWrt(p1, 1);
	PrWrt(p2, 1);
    }
}

void twoPrOACWrt(INSTINFO *info)
{
    if (CMPRES1 && !DSTNAT1) {
	PrWrt(p1, 1);
	PrWrt(p2, 0);
    }
}

void allPrWrt(INSTINFO *info)
{
    int i;
    BYTE tmp_rrbp = rrbp;

    rrbp = 0;
    for (i = 1; i < NPRS; i++) {
	DST1 >>= 1;
	DST2 >>= 1;
	if (DST2 & 0x1ULL)
	    PrWrt(i, DST1 & 0x1ULL);
    }
    rrbp = tmp_rrbp;
}

void rotPrWrt(INSTINFO *info)
{
    int i;
    BYTE tmp_rrbp = rrbp;

    rrbp = 0;
    DST1 >>= 16;
    for (i = 16; i < NPRS; i++) {
	PrWrt(i, DST1 & 0x1ULL);
	DST1 >>= 1;
    }
    rrbp = tmp_rrbp;
}

void oneArWrt(INSTINFO *info)
{
    traceArTgt(DST1);
    ArWrt(ar3, DST1);
}

void oneFrWrt(INSTINFO *info)
{
    FrWrt(f1, FDST1);
    if (FPHI(f1))
	PSR_MFH = YES;
    else if (FPLO(f1))
	PSR_MFL = YES;
}

void twoFrWrt(INSTINFO *info)
{
    FrWrt(f1, FDST1);
    FrWrt(f2, FDST2);
    if (FPHI(f1) || FPHI(f2))
	PSR_MFH = YES;
    if (FPLO(f1) || FPLO(f2))
	PSR_MFL = YES;
}

void twoFrPiWrt(INSTINFO *info)
{
    FrWrt(f1, FDST1);
    FrWrt(f2, FDST2);
    if (FPHI(f1) || FPHI(f2))
	PSR_MFH = YES;
    if (FPLO(f1) || FPLO(f2))
	PSR_MFL = YES;
    GrWrt(r3, DST1, DSTNAT1, NO);
}

void frs0Wrt(INSTINFO *info)
{
    FrWrt(f1, FDST1);
    FPSR_SF0_FLAGS |= DST1 & FP_FLAGS;
    if (FPHI(f1))
	PSR_MFH = YES;
    else if (FPLO(f1))
	PSR_MFL = YES;
}

void frs1Wrt(INSTINFO *info)
{
    FrWrt(f1, FDST1);
    FPSR_SF1_FLAGS |= DST1 & FP_FLAGS;
    if (FPHI(f1))
	PSR_MFH = YES;
    else if (FPLO(f1))
	PSR_MFL = YES;
}

void frs2Wrt(INSTINFO *info)
{
    FrWrt(f1, FDST1);
    FPSR_SF2_FLAGS |= DST1 & FP_FLAGS;
    if (FPHI(f1))
	PSR_MFH = YES;
    else if (FPLO(f1))
	PSR_MFL = YES;
}

void frs3Wrt(INSTINFO *info)
{
    FrWrt(f1, FDST1);
    FPSR_SF3_FLAGS |= DST1 & FP_FLAGS;
    if (FPHI(f1))
	PSR_MFH = YES;
    else if (FPLO(f1))
	PSR_MFL = YES;
}

void frts0Wrt(INSTINFO *info)
{
    FrWrt(f1, FDST1);
    FPSR_SF0_FLAGS |= DST1 & FP_FLAGS;
    if (FPHI(f1))
	PSR_MFH = YES;
    else if (FPLO(f1))
	PSR_MFL = YES;
    if (DST1 & FP_TRAP) {
	fpExceptionTrap(info->delta, (DST1 >> 11) << 4);
	return StTrap;
    }
}

void frts1Wrt(INSTINFO *info)
{
    FrWrt(f1, FDST1);
    FPSR_SF1_FLAGS |= DST1 & FP_FLAGS;
    if (FPHI(f1))
	PSR_MFH = YES;
    else if (FPLO(f1))
	PSR_MFL = YES;
    if (DST1 & FP_TRAP) {
	fpExceptionTrap(info->delta, (DST1 >> 11) << 4);
	return StTrap;
    }
}

void frts2Wrt(INSTINFO *info)
{
    FrWrt(f1, FDST1);
    FPSR_SF2_FLAGS |= DST1 & FP_FLAGS;
    if (FPHI(f1))
	PSR_MFH = YES;
    else if (FPLO(f1))
	PSR_MFL = YES;
    if (DST1 & FP_TRAP) {
	fpExceptionTrap(info->delta, (DST1 >> 11) << 4);
	return StTrap;
    }
}

void frts3Wrt(INSTINFO *info)
{
    FrWrt(f1, FDST1);
    FPSR_SF3_FLAGS |= DST1 & FP_FLAGS;
    if (FPHI(f1))
	PSR_MFH = YES;
    else if (FPLO(f1))
	PSR_MFL = YES;
    if (DST1 & FP_TRAP) {
	fpExceptionTrap(info->delta, (DST1 >> 11) << 4);
	return StTrap;
    }
}

void frpts0Wrt(INSTINFO *info)
{
    FrWrt(f1, FDST1);
    FPSR_SF0_FLAGS |= (DST1 | DST2) & FP_FLAGS;
    if (FPHI(f1))
	PSR_MFH = YES;
    else if (FPLO(f1))
	PSR_MFL = YES;
    if ((DST1 | DST2) & FP_TRAP) {
	fpSimdTrap(info->delta, DST1, DST2);
	return StTrap;
    }
}

void frpts1Wrt(INSTINFO *info)
{
    FrWrt(f1, FDST1);
    FPSR_SF1_FLAGS |= (DST1 | DST2) & FP_FLAGS;
    if (FPHI(f1))
	PSR_MFH = YES;
    else if (FPLO(f1))
	PSR_MFL = YES;
    if ((DST1 | DST2) & FP_TRAP) {
	fpSimdTrap(info->delta, DST1, DST2);
	return StTrap;
    }
}

void frpts2Wrt(INSTINFO *info)
{
    FrWrt(f1, FDST1);
    FPSR_SF2_FLAGS |= (DST1 | DST2) & FP_FLAGS;
    if (FPHI(f1))
	PSR_MFH = YES;
    else if (FPLO(f1))
	PSR_MFL = YES;
    if ((DST1 | DST2) & FP_TRAP) {
	fpSimdTrap(info->delta, DST1, DST2);
	return StTrap;
    }
}

void frpts3Wrt(INSTINFO *info)
{
    FrWrt(f1, FDST1);
    FPSR_SF3_FLAGS |= (DST1 | DST2) & FP_FLAGS;
    if (FPHI(f1))
	PSR_MFH = YES;
    else if (FPLO(f1))
	PSR_MFL = YES;
    if ((DST1 | DST2) & FP_TRAP) {
	fpSimdTrap(info->delta, DST1, DST2);
	return StTrap;
    }
}

void ldFrWrt(INSTINFO *info)
{
    FrWrt(f1, FDST1);
    GrWrt(r3, DST2, DSTNAT2, NO);
    if (FPHI(f1))
	PSR_MFH = YES;
    else if (FPLO(f1))
	PSR_MFL = YES;
}

void fcmps0Wrt(INSTINFO *info)
{
    PrWrt(p1, CMPRES1);
    PrWrt(p2, CMPRES2);
    FPSR_SF0_FLAGS |= DST1;
}

void fcmps1Wrt(INSTINFO *info)
{
    PrWrt(p1, CMPRES1);
    PrWrt(p2, CMPRES2);
    FPSR_SF1_FLAGS |= DST1;
}

void fcmps2Wrt(INSTINFO *info)
{
    PrWrt(p1, CMPRES1);
    PrWrt(p2, CMPRES2);
    FPSR_SF2_FLAGS |= DST1;
}

void fcmps3Wrt(INSTINFO *info)
{
    PrWrt(p1, CMPRES1);
    PrWrt(p2, CMPRES2);
    FPSR_SF3_FLAGS |= DST1;
}

void frPrs0Wrt(INSTINFO *info)
{
    if (QUAL) {
	FrWrt(f1, FDST1);
	FPSR_SF0_FLAGS |= DST1;
	if (FPHI(f1))
	    PSR_MFH = YES;
	else if (FPLO(f1))
	    PSR_MFL = YES;
    }
    PrWrt(p2, PDST1);
}

void frPrs1Wrt(INSTINFO *info)
{
    if (QUAL) {
	FrWrt(f1, FDST1);
	FPSR_SF1_FLAGS |= DST1;
	if (FPHI(f1))
	    PSR_MFH = YES;
	else if (FPLO(f1))
	    PSR_MFL = YES;
    }
    PrWrt(p2, PDST1);
}

void frPrs2Wrt(INSTINFO *info)
{
    if (QUAL) {
	FrWrt(f1, FDST1);
	FPSR_SF2_FLAGS |= DST1;
	if (FPHI(f1))
	    PSR_MFH = YES;
	else if (FPLO(f1))
	    PSR_MFL = YES;
    }
    PrWrt(p2, PDST1);
}

void frPrs3Wrt(INSTINFO *info)
{
    if (QUAL) {
	FrWrt(f1, FDST1);
	FPSR_SF3_FLAGS |= DST1;
	if (FPHI(f1))
	    PSR_MFH = YES;
	else if (FPLO(f1))
	    PSR_MFL = YES;
    }
    PrWrt(p2, PDST1);
}

void fpsrWrt(INSTINFO *info)
{
    FPSR = DST1;
}

void oneBrWrt(INSTINFO *info)
{
    traceBrPred(DST1);
    BrWrt(b1, DST1);
}

void ipWrt(INSTINFO *info)
{
    if (!unixABI && PSR_IC)
	IIPA = ip;
    ip = DST1;
    /* XXX - this is a hack so the "return" gets saved in combfns.c */
    if (1)
	return StTakenBr | st;
}

void brIAWrt(INSTINFO *info)
{
    Status ret = StTakenBr;

    if (!unixABI && PSR_IC)
	IIPA = ip;
    ip = DST1;
    PSR_IS = YES;
    PSR_DD = PSR_DA = PSR_IA = PSR_ED = NO;
    sof = soil = sor = rrbg = rrbf = rrbp = 0;
    rse_invalidate_non_current_regs();
    arithFlagsFromEflags();
    setSegGRsFromARs();
    setIAmode();
    setEIPfromIP();
    if (!PSR_IC)	/* Impl-specific. Matches itanium1 and itanium2 */
	progStop("Machine check at IP = %016llx.  Branching to IA-32 when "
		 "PSR.ic is 0\n", ip);
    if (executionMode == RUN_EM)
	/* Before going into the IA loop, check for traps: tknbr, ss, unimpl */
	while (PSR_IS) {
	    total_cycles++;
	    if (ret & ST_INST_COUNT)
		total_insts++;
	    ret = iAiCycle();
	}
    /* XXX - this is a hack so the "return" gets saved in combfns.c */
    if (1)
	return ret | ST_STOP;
}

void condIpWrt(INSTINFO *info)
{
    if (DST2) {
	if (!unixABI && PSR_IC)
	    IIPA = ip;
	ip = DST1;
	return StTakenBr;
    }
}

void qualIpWrt(INSTINFO *info)
{
    if (QUAL) {
	if (!unixABI && PSR_IC)
	    IIPA = ip;
	ip = DST1;
	return StTakenBr;
    }
}

void callWrt(INSTINFO *info)
{
    if (!unixABI && PSR_IC)
	IIPA = ip;
    ip = DST1;
    BrWrt(b1, DST2);
    soil = sor = rrbg = rrbf = rrbp = 0;
    updateGrMap(0);
    /* XXX - this is a hack so the "return" gets saved in combfns.c */
    if (1)
	return StTakenBr;
}

void clrrrbWrt(INSTINFO *info)
{
    rrbg = rrbf = rrbp = 0;
}

void clrrrbpWrt(INSTINFO *info)
{
    rrbp = 0;
}

void cfmWrt(INSTINFO *info)
{
    int growth = (int)(SOF - sof);
    int old = sof;

    sof = SOF;
    if (OUT_OF_FRAME(r1)) {
	sof = old;
	illegalOpFault();
	return;
    }
    if (SOF > N_STACK_VIRT || SOIL > SOF || SOR > SOF) {
	sof = old;
	illegalOpFault();
	return;
    }
    if (SOR != sor && (rrbg || rrbf || rrbp)) {
	sof = old;
	reservedRegFieldFault(0);
	return;
    }
    sof = old; /* rse_new_frame() may longjmp() and RSE must be consistent
		  when doing so */
    if (!rse_new_frame(growth)) {
	/* To keep RSE consistent */
	sof = n_stack_phys - (clean + dirty + invalid);
	return;
    }
    sof = SOF;
    soil = SOIL;
    sor  = SOR;
    updateGrMap(old);
    GrWrt(r1, DST1, 0, info->pgrtgt ? &grs[info->pgrtgt - 1] : NULL);
}

#if 0
void cfmBolWrt(INSTINFO *info)
{
    sof  = SOF;
    soil = SOIL;
    bol  = info->bol;
    updateGrMap();
}
#endif

void breakWrt(INSTINFO *info)
{
    GrWrtx(8, SC_RET, 0);
    if (unixABI && !SRC2 && SRC1 == 0xE000000000000000ULL) {
	PrWrt(14, !!SC_STAT);
	PrWrt(15, !SC_STAT);
    }
#if defined __linux__
    else if (unixABI)
	GrWrtx(10, SC_STAT, 0);
#endif /* !defined __linux__ */
    /* XXX - this is a hack so the "return" gets saved in combfns.c */
}

void st1Wrt(INSTINFO *info)
{
    FM_mem_cache_update(SRC2, info);
    if (!memWrt1(SRC2, DST1))
	return;
}

void st2Wrt(INSTINFO *info)
{
    FM_mem_cache_update(SRC2, info);
    if (!memWrt2(SRC2, DST1))
	return;
}

void st4Wrt(INSTINFO *info)
{
    FM_mem_cache_update(SRC2, info);
    if (!memWrt4(SRC2, DST1))
	return;
}

void st8Wrt(INSTINFO *info)
{
    FM_mem_cache_update(SRC2, info);
    if (!memWrt8(SRC2, DST1))
	return;
}

void st16Wrt(INSTINFO *info)
{
    FM_mem_cache_update(SRC2, info);
    if (!memWrt16(SRC2, DST1, DST2))
	return;
}

void stSpillWrt(INSTINFO *info)
{
    FM_mem_cache_update(SRC2, info);
    if (!memWrt8(SRC2, DST1))
	return;
    UNAT = DST2;
}

void stfeWrt(INSTINFO *info)
{
    QWORD qval = freg2ext(FSRC1);

    FM_mem_cache_update(SRC2, info);
    if (!memWrt10(SRC2, qval.left, qval.right))
	return;
}

void st1PiWrt(INSTINFO *info)
{
    FM_mem_cache_update(SRC2, info);
    if (!memWrt1(SRC2, DST1))
	return;
    GrWrt(r3, SRC2+imm64, 0, NO);
}

void st2PiWrt(INSTINFO *info)
{
    FM_mem_cache_update(SRC2, info);
    if (!memWrt2(SRC2, DST1))
	return;
    GrWrt(r3, SRC2+imm64, 0, NO);
}

void st4PiWrt(INSTINFO *info)
{
    FM_mem_cache_update(SRC2, info);
    if (!memWrt4(SRC2, DST1))
	return;
    GrWrt(r3, SRC2+imm64, 0, NO);
}

void st8PiWrt(INSTINFO *info)
{
    FM_mem_cache_update(SRC2, info);
    if (!memWrt8(SRC2, DST1))
	return;
    GrWrt(r3, SRC2+imm64, 0, NO);
}

void st16PiWrt(INSTINFO *info)
{
    FM_mem_cache_update(SRC2, info);
    if (!memWrt16(SRC2, DST1, DST2))
	return;
    GrWrt(r3, SRC2+imm64, 0, NO);
}

void stSpillPiWrt(INSTINFO *info)
{
    FM_mem_cache_update(SRC2, info);
    if (!memWrt8(SRC2, DST1))
	return;
    UNAT = DST2;
    GrWrt(r3, SRC2+imm64, 0, NO);
}

void stfePiWrt(INSTINFO *info)
{
    QWORD qval = freg2ext(FSRC1);

    FM_mem_cache_update(SRC2, info);
    if (!memWrt10(SRC2, qval.left, qval.right))
	return;
    GrWrt(r3, SRC2+imm64, 0, NO);
}

void xchg1Wrt(INSTINFO *info)
{
    if (!memXchg(SRC2, 1, SRC1, &DST1))
	return;
    GrWrt(r1, DST1, 0, info->pgrtgt ? &grs[info->pgrtgt - 1] : NULL);
}

void xchg2Wrt(INSTINFO *info)
{
    if (!memXchg(SRC2, 2, SRC1, &DST1))
	return;
    GrWrt(r1, DST1, 0, info->pgrtgt ? &grs[info->pgrtgt - 1] : NULL);
}

void xchg4Wrt(INSTINFO *info)
{
    if (!memXchg(SRC2, 4, SRC1, &DST1))
	return;
    GrWrt(r1, DST1, 0, info->pgrtgt ? &grs[info->pgrtgt - 1] : NULL);
}

void xchg8Wrt(INSTINFO *info)
{
    if (!memXchg(SRC2, 8, SRC1, &DST1))
	return;
    GrWrt(r1, DST1, 0, info->pgrtgt ? &grs[info->pgrtgt - 1] : NULL);
}

void cmpxchg1Wrt(INSTINFO *info)
{
    if (!memXchgCond(SRC2, 1, SRC1, &DST1))
	return;
    GrWrt(r1, DST1, 0, info->pgrtgt ? &grs[info->pgrtgt - 1] : NULL);
}

void cmpxchg2Wrt(INSTINFO *info)
{
    if (!memXchgCond(SRC2, 2, SRC1, &DST1))
	return;
    GrWrt(r1, DST1, 0, info->pgrtgt ? &grs[info->pgrtgt - 1] : NULL);
}

void cmpxchg4Wrt(INSTINFO *info)
{
    if (!memXchgCond(SRC2, 4, SRC1, &DST1))
	return;
    GrWrt(r1, DST1, 0, info->pgrtgt ? &grs[info->pgrtgt - 1] : NULL);
}

void cmpxchg8Wrt(INSTINFO *info)
{
    if (!memXchgCond(SRC2, 8, SRC1, &DST1))
	return;
    GrWrt(r1, DST1, 0, info->pgrtgt ? &grs[info->pgrtgt - 1] : NULL);
}

void cmp8xchg16Wrt(INSTINFO *info)
{
    if (!memXchg16Cond(SRC2, SRC1, SRC3, &DST1))
	return;
    GrWrt(r1, DST1, 0, info->pgrtgt ? &grs[info->pgrtgt - 1] : NULL);
}

void fetchadd4Wrt(INSTINFO *info)
{
    if (!memXchgAdd(SRC2, 4, SRC1, &DST1))
	return;
    GrWrt(r1, DST1, 0, info->pgrtgt ? &grs[info->pgrtgt - 1] : NULL);
}

void fetchadd8Wrt(INSTINFO *info)
{
    if (!memXchgAdd(SRC2, 8, SRC1, &DST1))
	return;
    GrWrt(r1, DST1, 0, info->pgrtgt ? &grs[info->pgrtgt - 1] : NULL);
}

void psrWrt(INSTINFO *info)
{
    psr = DST1;
    if (acceptIrpt()) {
	intrsim = extint = YES;
	st |= ST_CHECK;
    }
}

void cplWrt(INSTINFO *info)
{
    PSR_CPL = CPL;
}

void rfiWrt(INSTINFO *info)
{
    Status ret = st | StRFI;

    psr = DST1;
    ip  = DST2;
    icp = NULL;
    if (acceptIrpt()) {		/* PSR.i could have been updated */
	intrsim = extint = YES;
	ret |= ST_CHECK;
    }

    if (PSR_IS && !PSR_IC)   /* Impl-specific. Matches itanium1 and itanium2 */
	progStop("Machine check at IP = %016llx.  Branching to IA-32 when "
		 "PSR.ic is 0\n", ip);

    if (PSR_IS && executionMode == RUN_EM) {
	while (PSR_IS) {
	    total_cycles++;
	    if (st & ST_INST_COUNT)
		total_insts++;
	    ret = iAiCycle();
	}
    }

    /* XXX - this is a hack so the "return" gets saved in combfns.c */
    if (1)
	return ret | ST_STOP;
}

void oneCrWrt(INSTINFO *info)
{
    CrWrt(cr3, DST1);
    if (cr3 == TPR_ID && acceptIrpt())
	intrsim = extint = YES;
    if (cr3 == EOI_ID)
	completeIrpt();
    if (intrsim)
	st |= ST_CHECK;
}

void oneDbrWrt(INSTINFO *info)
{
    DbrWrt(B7(SRC2), DST1);
}

void oneIbrWrt(INSTINFO *info)
{
    IbrWrt(B7(SRC2), DST1);
}

void oneMsrWrt(INSTINFO *info)
{
    /* XXX - new */
}

void onePkrWrt(INSTINFO *info)
{
    PkrWrt(B7(SRC2), DST1);
}

void onePmcWrt(INSTINFO *info)
{
    PmcWrt(B7(SRC2), DST1);
}

void onePmdWrt(INSTINFO *info)
{
    PmdWrt(B7(SRC2), DST1);
}

void oneRrWrt(INSTINFO *info)
{
    RrWrt(BitfX(SRC2,0,3), SRC1);
}
