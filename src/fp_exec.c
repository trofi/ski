/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Floating-Point Execution Routines
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
#include "float.h"

Status famaxEx(INSTINFO *info)
{
    DST1 = famax(&FSRC1, &FSRC2, &FDST1, TRAPS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status faminEx(INSTINFO *info)
{
    DST1 = famin(&FSRC1, &FSRC2, &FDST1, TRAPS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fchkf0Ex(INSTINFO *info)
{
    QUAL = (FPSR_SF0_FLAGS & ~FPSR_TRAPS) ||
	   (FPSR_SF0_FLAGS & ~FPSR_SF0_FLAGS);
    DST1 = info->ct->ipn + target25;
    return 0;
}

Status fchkf1Ex(INSTINFO *info)
{
    QUAL = (FPSR_SF1_FLAGS & ~FPSR_TRAPS) ||
	   (FPSR_SF1_FLAGS & ~FPSR_SF0_FLAGS);
    DST1 = info->ct->ipn + target25;
    return 0;
}

Status fchkf2Ex(INSTINFO *info)
{
    QUAL = (FPSR_SF2_FLAGS & ~FPSR_TRAPS) ||
	   (FPSR_SF2_FLAGS & ~FPSR_SF0_FLAGS);
    DST1 = info->ct->ipn + target25;
    return 0;
}

Status fchkf3Ex(INSTINFO *info)
{
    QUAL = (FPSR_SF3_FLAGS & ~FPSR_TRAPS) ||
	   (FPSR_SF3_FLAGS & ~FPSR_SF0_FLAGS);
    DST1 = info->ct->ipn + target25;
    return 0;
}

Status fclassEx(INSTINFO *info)
{
    if (QUAL) {
	BOOL res = fclass(&FSRC1, imm64);

	CMPRES1 = res;
	CMPRES2 = !res;
    } else
	CMPRES1 = CMPRES2 = 0;
    DSTNAT1 = SRCNAT1 && !BitfX(imm64,55,1);
    return 0;
}

Status fclrf0Ex(INSTINFO *info)
{
    DST1 = SRC1;
    X_FPSR_SF0_FLAGS(DST1) = 0;
    return 0;
}

Status fclrf1Ex(INSTINFO *info)
{
    DST1 = SRC1;
    X_FPSR_SF1_FLAGS(DST1) = 0;
    return 0;
}

Status fclrf2Ex(INSTINFO *info)
{
    DST1 = SRC1;
    X_FPSR_SF2_FLAGS(DST1) = 0;
    return 0;
}

Status fclrf3Ex(INSTINFO *info)
{
    DST1 = SRC1;
    X_FPSR_SF3_FLAGS(DST1) = 0;
    return 0;
}

Status fcmpEqEx(INSTINFO *info)
{
    DST1 = fcmpEq(&FSRC1, &FSRC2, QUAL, &CMPRES1, &CMPRES2, TRAPS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fcmpLtEx(INSTINFO *info)
{
    DST1 = fcmpLt(&FSRC1, &FSRC2, QUAL, &CMPRES1, &CMPRES2, TRAPS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fcmpLeEx(INSTINFO *info)
{
    DST1 = fcmpLe(&FSRC1, &FSRC2, QUAL, &CMPRES1, &CMPRES2, TRAPS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fcmpUnordEx(INSTINFO *info)
{
    DST1 = fcmpUnord(&FSRC1, &FSRC2, QUAL, &CMPRES1, &CMPRES2, TRAPS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fcvtfxEx(INSTINFO *info)
{
    DST1 = fcvtfx(&FSRC1, &FDST1, TRAPS, CTRLS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fcvtfxtEx(INSTINFO *info)
{
    BitfR8(CTRLS,2,2) = 3;	/* set rc to RZ */
    DST1 = fcvtfx(&FSRC1, &FDST1, TRAPS, CTRLS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fcvtfxuEx(INSTINFO *info)
{
    DST1 = fcvtfu(&FSRC1, &FDST1, TRAPS, CTRLS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fcvtfxutEx(INSTINFO *info)
{
    BitfR8(CTRLS,2,2) = 3;	/* set rc to RZ */
    DST1 = fcvtfu(&FSRC1, &FDST1, TRAPS, CTRLS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fcvtxfEx(INSTINFO *info)
{
    if (FSRC1.special && FSRC1.class == CLASS_NAT) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD val;

	val = freg2dword(FSRC1);
	if (SIGN(val)) {
	    FDST1.sign = 1;
	    val = -val;
	} else
	    FDST1.sign = 0;
	if (val) {
	    FDST1.mant = val;
	    FDST1.exp = INTEGER_EXP - numLzeroes(&FDST1.mant);
	    FDST1.unorm = 0;
	} else {
	    FDST1.unorm = 64;
	    FDST1.exp = 0;
	}
	FDST1.special = NO;
	FDST1.class = CLASS_NONE;
    }
    return 0;
}

Status fmergensEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	BYTE sign1, sign2;
	WORD exp1, exp2;
	DWORD mant1, mant2;

	freg2spill(FSRC1, &sign1, &exp1, &mant1);
	freg2spill(FSRC2, &sign2, &exp2, &mant2);
	FDST1 = fill2freg(1-sign1, exp2, mant2);
    }
    return 0;
}

Status fmergesEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	BYTE sign1, sign2;
	WORD exp1, exp2;
	DWORD mant1, mant2;

	freg2spill(FSRC1, &sign1, &exp1, &mant1);
	freg2spill(FSRC2, &sign2, &exp2, &mant2);
	FDST1 = fill2freg(sign1, exp2, mant2);
    }
    return 0;
}

Status fmergeseEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	BYTE sign1, sign2;
	WORD exp1, exp2;
	DWORD mant1, mant2;

	freg2spill(FSRC1, &sign1, &exp1, &mant1);
	freg2spill(FSRC2, &sign2, &exp2, &mant2);
	FDST1 = fill2freg(sign1, exp1, mant2);
    }
    return 0;
}

Status fmaEx(INSTINFO *info)
{
    DST1 = fma(&FSRC1, &FSRC2, &FSRC3, &FDST1, PC_DYN, TRAPS, CTRLS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fmasEx(INSTINFO *info)
{
    DST1 = fma(&FSRC1, &FSRC2, &FSRC3, &FDST1, PC_SGL, TRAPS, CTRLS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fmadEx(INSTINFO *info)
{
    DST1 = fma(&FSRC1, &FSRC2, &FSRC3, &FDST1, PC_DBL, TRAPS, CTRLS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fmsEx(INSTINFO *info)
{
    DST1 = fms(&FSRC1, &FSRC2, &FSRC3, &FDST1, PC_DYN, TRAPS, CTRLS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fmssEx(INSTINFO *info)
{
    DST1 = fms(&FSRC1, &FSRC2, &FSRC3, &FDST1, PC_SGL, TRAPS, CTRLS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fmsdEx(INSTINFO *info)
{
    DST1 = fms(&FSRC1, &FSRC2, &FSRC3, &FDST1, PC_DBL, TRAPS, CTRLS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fnmaEx(INSTINFO *info)
{
    DST1 = fnma(&FSRC1, &FSRC2, &FSRC3, &FDST1, PC_DYN, TRAPS, CTRLS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fnmasEx(INSTINFO *info)
{
    DST1 = fnma(&FSRC1, &FSRC2, &FSRC3, &FDST1, PC_SGL, TRAPS, CTRLS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fnmadEx(INSTINFO *info)
{
    DST1 = fnma(&FSRC1, &FSRC2, &FSRC3, &FDST1, PC_DBL, TRAPS, CTRLS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fmaxEx(INSTINFO *info)
{
    DST1 = fmax(&FSRC1, &FSRC2, &FDST1, TRAPS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fminEx(INSTINFO *info)
{
    DST1 = fmin(&FSRC1, &FSRC2, &FDST1, TRAPS);
    if (DST1 & FP_FAULT) {
	fpExceptionFault(DST1);
	return StFault;
    }
    return 0;
}

Status fpamaxEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = 0;
    } else {
	DWORD val1, val2, result;
	unsigned ret0, ret1;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);

	FSRC1 = sgl2freg(WD0(val1));
	FSRC2 = sgl2freg(WD0(val2));
	ret0 = famax(&FSRC1, &FSRC2, &FDST1, TRAPS);
	WD0(result) = freg2sgl(FDST1);

	FSRC1 = sgl2freg(WD1(val1));
	FSRC2 = sgl2freg(WD1(val2));
	ret1 = famax(&FSRC1, &FSRC2, &FDST1, TRAPS);
	WD1(result) = freg2sgl(FDST1);

	FDST1 = dword2freg(result);
	DST1 = ret0 | ret1;
	if (DST1 & FP_FAULT) {
	    fpSimdFault(ret0, ret1);
	    return StFault;
	}
    }
    return 0;
}

Status fpaminEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = 0;
    } else {
	DWORD val1, val2, result;
	unsigned ret0, ret1;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);

	FSRC1 = sgl2freg(WD0(val1));
	FSRC2 = sgl2freg(WD0(val2));
	ret0 = famin(&FSRC1, &FSRC2, &FDST1, TRAPS);
	WD0(result) = freg2sgl(FDST1);

	FSRC1 = sgl2freg(WD1(val1));
	FSRC2 = sgl2freg(WD1(val2));
	ret1 = famin(&FSRC1, &FSRC2, &FDST1, TRAPS);
	WD1(result) = freg2sgl(FDST1);

	FDST1 = dword2freg(result);
	DST1 = ret0 | ret1;
	if (DST1 & FP_FAULT) {
	    fpSimdFault(ret0, ret1);
	    return StFault;
	}
    }
    return 0;
}

Status fpcmpEqEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = 0;
    } else {
	DWORD val1, val2, result;
	unsigned ret0, ret1;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);

	FSRC1 = sgl2freg(WD0(val1));
	FSRC2 = sgl2freg(WD0(val2));
	ret0 = fcmpEq(&FSRC1, &FSRC2, YES, &CMPRES1, &CMPRES2, TRAPS);
	WD0(result) = CMPRES1 ? 0xFFFFFFFFU : 0;

	FSRC1 = sgl2freg(WD1(val1));
	FSRC2 = sgl2freg(WD1(val2));
	ret1 = fcmpEq(&FSRC1, &FSRC2, YES, &CMPRES1, &CMPRES2, TRAPS);
	WD1(result) = CMPRES1 ? 0xFFFFFFFFU : 0;

	FDST1 = dword2freg(result);
	DST1 = ret0 | ret1;
	if (DST1 & FP_FAULT) {
	    fpSimdFault(ret0, ret1);
	    return StFault;
	}
    }
    return 0;
}

Status fpcmpLtEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = 0;
    } else {
	DWORD val1, val2, result;
	unsigned ret0, ret1;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);

	FSRC1 = sgl2freg(WD0(val1));
	FSRC2 = sgl2freg(WD0(val2));
	ret0 = fcmpLt(&FSRC1, &FSRC2, YES, &CMPRES1, &CMPRES2, TRAPS);
	WD0(result) = CMPRES1 ? 0xFFFFFFFFU : 0;

	FSRC1 = sgl2freg(WD1(val1));
	FSRC2 = sgl2freg(WD1(val2));
	ret1 = fcmpLt(&FSRC1, &FSRC2, YES, &CMPRES1, &CMPRES2, TRAPS);
	WD1(result) = CMPRES1 ? 0xFFFFFFFFU : 0;

	FDST1 = dword2freg(result);
	DST1 = ret0 | ret1;
	if (DST1 & FP_FAULT) {
	    fpSimdFault(ret0, ret1);
	    return StFault;
	}
    }
    return 0;
}

Status fpcmpLeEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = 0;
    } else {
	DWORD val1, val2, result;
	unsigned ret0, ret1;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);

	FSRC1 = sgl2freg(WD0(val1));
	FSRC2 = sgl2freg(WD0(val2));
	ret0 = fcmpLe(&FSRC1, &FSRC2, YES, &CMPRES1, &CMPRES2, TRAPS);
	WD0(result) = CMPRES1 ? 0xFFFFFFFFU : 0;

	FSRC1 = sgl2freg(WD1(val1));
	FSRC2 = sgl2freg(WD1(val2));
	ret1 = fcmpLe(&FSRC1, &FSRC2, YES, &CMPRES1, &CMPRES2, TRAPS);
	WD1(result) = CMPRES1 ? 0xFFFFFFFFU : 0;

	FDST1 = dword2freg(result);
	DST1 = ret0 | ret1;
	if (DST1 & FP_FAULT) {
	    fpSimdFault(ret0, ret1);
	    return StFault;
	}
    }
    return 0;
}

Status fpcmpNeqEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = 0;
    } else {
	DWORD val1, val2, result;
	unsigned ret0, ret1;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);

	FSRC1 = sgl2freg(WD0(val1));
	FSRC2 = sgl2freg(WD0(val2));
	ret0 = fcmpEq(&FSRC1, &FSRC2, YES, &CMPRES1, &CMPRES2, TRAPS);
	WD0(result) = CMPRES2 ? 0xFFFFFFFFU : 0;

	FSRC1 = sgl2freg(WD1(val1));
	FSRC2 = sgl2freg(WD1(val2));
	ret1 = fcmpEq(&FSRC1, &FSRC2, YES, &CMPRES1, &CMPRES2, TRAPS);
	WD1(result) = CMPRES2 ? 0xFFFFFFFFU : 0;

	FDST1 = dword2freg(result);
	DST1 = ret0 | ret1;
	if (DST1 & FP_FAULT) {
	    fpSimdFault(ret0, ret1);
	    return StFault;
	}
    }
    return 0;
}

Status fpcmpNltEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = 0;
    } else {
	DWORD val1, val2, result;
	unsigned ret0, ret1;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);

	FSRC1 = sgl2freg(WD0(val1));
	FSRC2 = sgl2freg(WD0(val2));
	ret0 = fcmpLt(&FSRC1, &FSRC2, YES, &CMPRES1, &CMPRES2, TRAPS);
	WD0(result) = CMPRES2 ? 0xFFFFFFFFU : 0;

	FSRC1 = sgl2freg(WD1(val1));
	FSRC2 = sgl2freg(WD1(val2));
	ret1 = fcmpLt(&FSRC1, &FSRC2, YES, &CMPRES1, &CMPRES2, TRAPS);
	WD1(result) = CMPRES2 ? 0xFFFFFFFFU : 0;

	FDST1 = dword2freg(result);
	DST1 = ret0 | ret1;
	if (DST1 & FP_FAULT) {
	    fpSimdFault(ret0, ret1);
	    return StFault;
	}
    }
    return 0;
}

Status fpcmpNleEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = 0;
    } else {
	DWORD val1, val2, result;
	unsigned ret0, ret1;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);

	FSRC1 = sgl2freg(WD0(val1));
	FSRC2 = sgl2freg(WD0(val2));
	ret0 = fcmpLe(&FSRC1, &FSRC2, YES, &CMPRES1, &CMPRES2, TRAPS);
	WD0(result) = CMPRES2 ? 0xFFFFFFFFU : 0;

	FSRC1 = sgl2freg(WD1(val1));
	FSRC2 = sgl2freg(WD1(val2));
	ret1 = fcmpLe(&FSRC1, &FSRC2, YES, &CMPRES1, &CMPRES2, TRAPS);
	WD1(result) = CMPRES2 ? 0xFFFFFFFFU : 0;

	FDST1 = dword2freg(result);
	DST1 = ret0 | ret1;
	if (DST1 & FP_FAULT) {
	    fpSimdFault(ret0, ret1);
	    return StFault;
	}
    }
    return 0;
}

Status fpcmpOrdEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = 0;
    } else {
	DWORD val1, val2, result;
	unsigned ret0, ret1;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);

	FSRC1 = sgl2freg(WD0(val1));
	FSRC2 = sgl2freg(WD0(val2));
	ret0 = fcmpUnord(&FSRC1, &FSRC2, YES, &CMPRES1, &CMPRES2, TRAPS);
	WD0(result) = CMPRES2 ? 0xFFFFFFFFU : 0;

	FSRC1 = sgl2freg(WD1(val1));
	FSRC2 = sgl2freg(WD1(val2));
	ret1 = fcmpUnord(&FSRC1, &FSRC2, YES, &CMPRES1, &CMPRES2, TRAPS);
	WD1(result) = CMPRES2 ? 0xFFFFFFFFU : 0;

	FDST1 = dword2freg(result);
	DST1 = ret0 | ret1;
	if (DST1 & FP_FAULT) {
	    fpSimdFault(ret0, ret1);
	    return StFault;
	}
    }
    return 0;
}

Status fpcmpUnordEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = 0;
    } else {
	DWORD val1, val2, result;
	unsigned ret0, ret1;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);

	FSRC1 = sgl2freg(WD0(val1));
	FSRC2 = sgl2freg(WD0(val2));
	ret0 = fcmpUnord(&FSRC1, &FSRC2, YES, &CMPRES1, &CMPRES2, TRAPS);
	WD0(result) = CMPRES1 ? 0xFFFFFFFFU : 0;

	FSRC1 = sgl2freg(WD1(val1));
	FSRC2 = sgl2freg(WD1(val2));
	ret1 = fcmpUnord(&FSRC1, &FSRC2, YES, &CMPRES1, &CMPRES2, TRAPS);
	WD1(result) = CMPRES1 ? 0xFFFFFFFFU : 0;

	FDST1 = dword2freg(result);
	DST1 = ret0 | ret1;
	if (DST1 & FP_FAULT) {
	    fpSimdFault(ret0, ret1);
	    return StFault;
	}
    }
    return 0;
}

Status fpcvtfxEx(INSTINFO *info)
{
    if (FSRC1.special && FSRC1.class == CLASS_NAT) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = DST2 = 0;
    } else {
	DWORD mant, val, result;

	val = freg2dword(FSRC1);

	FSRC1 = sgl2freg(WD0(val));
	DST1 = fpcvtfx(&FSRC1, &FDST1, TRAPS, CTRLS);
	mant = freg2dword(FDST1);
	WD0(result) = WD0(mant);

	FSRC1 = sgl2freg(WD1(val));
	DST2 = fpcvtfx(&FSRC1, &FDST1, TRAPS, CTRLS);
	mant = freg2dword(FDST1);
	WD1(result) = WD0(mant);

	FDST1 = dword2freg(result);
	if ((DST1 | DST2) & FP_FAULT) {
	    fpSimdFault(DST1, DST2);
	    return StFault;
	}
    }
    return 0;
}

Status fpcvtfxtEx(INSTINFO *info)
{
    if (FSRC1.special && FSRC1.class == CLASS_NAT) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = DST2 = 0;
    } else {
	DWORD mant, val, result;

	BitfR8(CTRLS,2,2) = 3;	/* set rc to RZ */
	val = freg2dword(FSRC1);

	FSRC1 = sgl2freg(WD0(val));
	DST1 = fpcvtfx(&FSRC1, &FDST1, TRAPS, CTRLS);
	mant = freg2dword(FDST1);
	WD0(result) = WD0(mant);

	FSRC1 = sgl2freg(WD1(val));
	DST2 = fpcvtfx(&FSRC1, &FDST1, TRAPS, CTRLS);
	mant = freg2dword(FDST1);
	WD1(result) = WD0(mant);

	FDST1 = dword2freg(result);
	if ((DST1 | DST2) & FP_FAULT) {
	    fpSimdFault(DST1, DST2);
	    return StFault;
	}
    }
    return 0;
}

Status fpcvtfxuEx(INSTINFO *info)
{
    if (FSRC1.special && FSRC1.class == CLASS_NAT) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = DST2 = 0;
    } else {
	DWORD mant, val, result;

	val = freg2dword(FSRC1);

	FSRC1 = sgl2freg(WD0(val));
	DST1 = fpcvtfu(&FSRC1, &FDST1, TRAPS, CTRLS);
	mant = freg2dword(FDST1);
	WD0(result) = WD0(mant);

	FSRC1 = sgl2freg(WD1(val));
	DST2 = fpcvtfu(&FSRC1, &FDST1, TRAPS, CTRLS);
	mant = freg2dword(FDST1);
	WD1(result) = WD0(mant);

	FDST1 = dword2freg(result);
	if ((DST1 | DST2) & FP_FAULT) {
	    fpSimdFault(DST1, DST2);
	    return StFault;
	}
    }
    return 0;
}

Status fpcvtfxutEx(INSTINFO *info)
{
    if (FSRC1.special && FSRC1.class == CLASS_NAT) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = DST2 = 0;
    } else {
	DWORD mant, val, result;

	BitfR8(CTRLS,2,2) = 3;	/* set rc to RZ */
	val = freg2dword(FSRC1);

	FSRC1 = sgl2freg(WD0(val));
	DST1 = fpcvtfu(&FSRC1, &FDST1, TRAPS, CTRLS);
	mant = freg2dword(FDST1);
	WD0(result) = WD0(mant);

	FSRC1 = sgl2freg(WD1(val));
	DST2 = fpcvtfu(&FSRC1, &FDST1, TRAPS, CTRLS);
	mant = freg2dword(FDST1);
	WD1(result) = WD0(mant);

	FDST1 = dword2freg(result);
	if ((DST1 | DST2) & FP_FAULT) {
	    fpSimdFault(DST1, DST2);
	    return StFault;
	}
    }
    return 0;
}

Status fpmaEx(INSTINFO *info)
{
    if ((FSRC1.special && FSRC1.class == CLASS_NAT) ||
	(FSRC2.special && FSRC2.class == CLASS_NAT) ||
	(FSRC3.special && FSRC3.class == CLASS_NAT)) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = DST2 = 0;
    } else {
	DWORD val1, val2, val3, result;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);
	val3 = freg2dword(FSRC3);

	FSRC1 = sgl2freg(WD0(val1));
	FSRC2 = sgl2freg(WD0(val2));
	FSRC3 = f2 ? sgl2freg(WD0(val3)) : FrRd(0);
	DST1 = fma(&FSRC1, &FSRC2, &FSRC3, &FDST1, PC_SIMD, TRAPS, CTRLS);
	WD0(result) = freg2sgl(FDST1);

	FSRC1 = sgl2freg(WD1(val1));
	FSRC2 = sgl2freg(WD1(val2));
	FSRC3 = f2 ? sgl2freg(WD1(val3)) : FrRd(0);
	DST2 = fma(&FSRC1, &FSRC2, &FSRC3, &FDST1, PC_SIMD, TRAPS, CTRLS);
	WD1(result) = freg2sgl(FDST1);

	FDST1 = dword2freg(result);
	if ((DST1 | DST2) & FP_FAULT) {
	    fpSimdFault(DST1, DST2);
	    return StFault;
	}
    }
    return 0;
}

Status fpmsEx(INSTINFO *info)
{
    if ((FSRC1.special && FSRC1.class == CLASS_NAT) ||
	(FSRC2.special && FSRC2.class == CLASS_NAT) ||
	(FSRC3.special && FSRC3.class == CLASS_NAT)) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = DST2 = 0;
    } else {
	DWORD val1, val2, val3, result;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);
	val3 = freg2dword(FSRC3);

	FSRC1 = sgl2freg(WD0(val1));
	FSRC2 = sgl2freg(WD0(val2));
	FSRC3 = f2 ? sgl2freg(WD0(val3)) : FrRd(0);
	DST1 = fms(&FSRC1, &FSRC2, &FSRC3, &FDST1, PC_SIMD, TRAPS, CTRLS);
	WD0(result) = freg2sgl(FDST1);

	FSRC1 = sgl2freg(WD1(val1));
	FSRC2 = sgl2freg(WD1(val2));
	FSRC3 = f2 ? sgl2freg(WD1(val3)) : FrRd(0);
	DST2 = fms(&FSRC1, &FSRC2, &FSRC3, &FDST1, PC_SIMD, TRAPS, CTRLS);
	WD1(result) = freg2sgl(FDST1);

	FDST1 = dword2freg(result);
	if ((DST1 | DST2) & FP_FAULT) {
	    fpSimdFault(DST1, DST2);
	    return StFault;
	}
    }
    return 0;
}

Status fpnmaEx(INSTINFO *info)
{
    if ((FSRC1.special && FSRC1.class == CLASS_NAT) ||
	(FSRC2.special && FSRC2.class == CLASS_NAT) ||
	(FSRC3.special && FSRC3.class == CLASS_NAT)) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = DST2 = 0;
    } else {
	DWORD val1, val2, val3, result;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);
	val3 = freg2dword(FSRC3);

	FSRC1 = sgl2freg(WD0(val1));
	FSRC2 = sgl2freg(WD0(val2));
	FSRC3 = f2 ? sgl2freg(WD0(val3)) : FrRd(0);
	DST1 = fnma(&FSRC1, &FSRC2, &FSRC3, &FDST1, PC_SIMD, TRAPS, CTRLS);
	WD0(result) = freg2sgl(FDST1);

	FSRC1 = sgl2freg(WD1(val1));
	FSRC2 = sgl2freg(WD1(val2));
	FSRC3 = f2 ? sgl2freg(WD1(val3)) : FrRd(0);
	DST2 = fnma(&FSRC1, &FSRC2, &FSRC3, &FDST1, PC_SIMD, TRAPS, CTRLS);
	WD1(result) = freg2sgl(FDST1);

	FDST1 = dword2freg(result);
	if ((DST1 | DST2) & FP_FAULT) {
	    fpSimdFault(DST1, DST2);
	    return StFault;
	}
    }
    return 0;
}

Status fpmaxEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = 0;
    } else {
	DWORD val1, val2, result;
	unsigned ret0, ret1;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);

	FSRC1 = sgl2freg(WD0(val1));
	FSRC2 = sgl2freg(WD0(val2));
	ret0 = fmax(&FSRC1, &FSRC2, &FDST1, TRAPS);
	WD0(result) = freg2sgl(FDST1);

	FSRC1 = sgl2freg(WD1(val1));
	FSRC2 = sgl2freg(WD1(val2));
	ret1 = fmax(&FSRC1, &FSRC2, &FDST1, TRAPS);
	WD1(result) = freg2sgl(FDST1);

	FDST1 = dword2freg(result);
	DST1 = ret0 | ret1;
	if (DST1 & FP_FAULT) {
	    fpSimdFault(ret0, ret1);
	    return StFault;
	}
    }
    return 0;
}

Status fpminEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
	DST1 = 0;
    } else {
	DWORD val1, val2, result;
	unsigned ret0, ret1;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);

	FSRC1 = sgl2freg(WD0(val1));
	FSRC2 = sgl2freg(WD0(val2));
	ret0 = fmin(&FSRC1, &FSRC2, &FDST1, TRAPS);
	WD0(result) = freg2sgl(FDST1);

	FSRC1 = sgl2freg(WD1(val1));
	FSRC2 = sgl2freg(WD1(val2));
	ret1 = fmin(&FSRC1, &FSRC2, &FDST1, TRAPS);
	WD1(result) = freg2sgl(FDST1);

	FDST1 = dword2freg(result);
	DST1 = ret0 | ret1;
	if (DST1 & FP_FAULT) {
	    fpSimdFault(ret0, ret1);
	    return StFault;
	}
    }
    return 0;
}

Status fpmergensEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD val1, val2, result;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);

	BitfR(result,0,1)   = 1 - BitfX(val1,0,1);
	BitfR(result,1,31)  = BitfX(val2,1,31);
	BitfR(result,32,1)  = 1 - BitfX(val1,32,1);
	BitfR(result,33,31) = BitfX(val2,33,31);

	FDST1 = dword2freg(result);
    }
    return 0;
}

Status fpmergesEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD val1, val2, result;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);

	BitfR(result,0,1)   = BitfX(val1,0,1);
	BitfR(result,1,31)  = BitfX(val2,1,31);
	BitfR(result,32,1)  = BitfX(val1,32,1);
	BitfR(result,33,31) = BitfX(val2,33,31);

	FDST1 = dword2freg(result);
    }
    return 0;
}

Status fpmergeseEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD val1, val2, result;

	val1 = freg2dword(FSRC1);
	val2 = freg2dword(FSRC2);

	BitfR(result,0,9)   = BitfX(val1,0,9);
	BitfR(result,9,23)  = BitfX(val2,9,23);
	BitfR(result,32,9)  = BitfX(val1,32,9);
	BitfR(result,41,23) = BitfX(val2,41,23);

	FDST1 = dword2freg(result);
    }
    return 0;
}

Status fprcpaEx(INSTINFO *info)
{
    if (QUAL) {
	if ((FSRC1.special && FSRC1.class == CLASS_NAT) ||
	    (FSRC2.special && FSRC2.class == CLASS_NAT)) {
	    FDST1.special = YES;
	    FDST1.class = CLASS_NAT;
	    PDST1 = NO;
	    DST1 = 0;
	} else {
	    DWORD val1, val2, result;
	    BOOL pret0, pret1;
	    unsigned ret0, ret1;

	    val1 = freg2dword(FSRC1);
	    val2 = freg2dword(FSRC2);

	    FSRC1 = sgl2freg(WD0(val1));
	    FSRC2 = sgl2freg(WD0(val2));
	    ret0 = fprcpa(&FSRC1, &FSRC2, &FDST1, &pret0, TRAPS);
	    WD0(result) = freg2sgl(FDST1);

	    FSRC1 = sgl2freg(WD1(val1));
	    FSRC2 = sgl2freg(WD1(val2));
	    ret1 = fprcpa(&FSRC1, &FSRC2, &FDST1, &pret1, TRAPS);
	    WD1(result) = freg2sgl(FDST1);

	    FDST1 = dword2freg(result);
	    DST1 = ret0 | ret1;
	    PDST1 = pret0 & pret1;
	    if (DST1 & FP_FAULT) {
		fpSimdFault(ret0, ret1);
		return StFault;
	    }
	}
    } else
	PDST1 = NO;
    return 0;
}

Status fprsqrtaEx(INSTINFO *info)
{
    if (QUAL) {
	if (FSRC1.special && FSRC1.class == CLASS_NAT) {
	    FDST1.special = YES;
	    FDST1.class = CLASS_NAT;
	    PDST1 = NO;
	    DST1 = 0;
	} else {
	    DWORD val, result;
	    BOOL pret0, pret1;
	    unsigned ret0, ret1;

	    val = freg2dword(FSRC1);

	    FSRC1 = sgl2freg(WD0(val));
	    ret0 = fprsqrta(&FSRC1, &FDST1, &pret0, TRAPS);
	    WD0(result) = freg2sgl(FDST1);

	    FSRC1 = sgl2freg(WD1(val));
	    ret1 = fprsqrta(&FSRC1, &FDST1, &pret1, TRAPS);
	    WD1(result) = freg2sgl(FDST1);

	    FDST1 = dword2freg(result);
	    DST1 = ret0 | ret1;
	    PDST1 = pret0 & pret1;
	    if (DST1 & FP_FAULT) {
		fpSimdFault(ret0, ret1);
		return StFault;
	    }
	}
    } else
	PDST1 = NO;
    return 0;
}

Status frcpaEx(INSTINFO *info)
{
    if (QUAL) {
	if ((FSRC1.special && FSRC1.class == CLASS_NAT) ||
	    (FSRC2.special && FSRC2.class == CLASS_NAT)) {
	    FDST1.special = YES;
	    FDST1.class = CLASS_NAT;
	    PDST1 = NO;
	    DST1 = 0;
	} else {
	    DST1 = frcpa(&FSRC1, &FSRC2, &FDST1, &PDST1, TRAPS);
	    if (DST1 & FP_FAULT) {
		fpExceptionFault(DST1);
		return StFault;
	    }
	}
    } else
	PDST1 = NO;
    return 0;
}

Status frsqrtaEx(INSTINFO *info)
{
    if (QUAL) {
	if (FSRC1.special && FSRC1.class == CLASS_NAT) {
	    FDST1.special = YES;
	    FDST1.class = CLASS_NAT;
	    PDST1 = NO;
	    DST1 = 0;
	} else {
	    DST1 = frsqrta(&FSRC1, &FDST1, &PDST1, TRAPS);
	    if (DST1 & FP_FAULT) {
		fpExceptionFault(DST1);
		return StFault;
	    }
	}
    } else
	PDST1 = NO;
    return 0;
}

Status fandEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD val;

	val = freg2dword(FSRC1) & freg2dword(FSRC2);
	FDST1 = dword2freg(val);
    }
    return 0;
}

Status fandcmEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD val;

	val = freg2dword(FSRC1) & ~freg2dword(FSRC2);
	FDST1 = dword2freg(val);
    }
    return 0;
}

Status fmixlEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD mant1, mant2, val;

	mant1 = freg2dword(FSRC1);
	mant2 = freg2dword(FSRC2);
	WD0(val) = WD0(mant1);
	WD1(val) = WD0(mant2);
	FDST1 = dword2freg(val);
    }
    return 0;
}

Status fmixlrEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD mant1, mant2, val;

	mant1 = freg2dword(FSRC1);
	mant2 = freg2dword(FSRC2);
	WD0(val) = WD0(mant1);
	WD1(val) = WD1(mant2);
	FDST1 = dword2freg(val);
    }
    return 0;
}

Status fmixrEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD mant1, mant2, val;

	mant1 = freg2dword(FSRC1);
	mant2 = freg2dword(FSRC2);
	WD0(val) = WD1(mant1);
	WD1(val) = WD1(mant2);
	FDST1 = dword2freg(val);
    }
    return 0;
}

Status forEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD val;

	val = freg2dword(FSRC1) | freg2dword(FSRC2);
	FDST1 = dword2freg(val);
    }
    return 0;
}

Status fpackEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD val;

	WD0(val) = freg2sgl(FSRC1);
	WD1(val) = freg2sgl(FSRC2);
	FDST1 = dword2freg(val);
    }
    return 0;
}

Status fselEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2 || SRCNAT3) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD mant1, mant2, mant3, val;

	mant1 = freg2dword(FSRC1);
	mant2 = freg2dword(FSRC2);
	mant3 = freg2dword(FSRC3);
	val = (mant1 & mant3) | (mant2 & ~mant3);
	FDST1 = dword2freg(val);
    }
    return 0;
}

Status fsetc0Ex(INSTINFO *info)
{
    DST1 = SRC1;
    X_FPSR_SF0_CTRLS(DST1) = (X_FPSR_SF0_CTRLS(DST1) & amask7) | omask7;
    if (X_FPSR_SF0_TD(DST1) || X_FPSR_SF0_PC(DST1) == 1) {
	reservedRegFieldFault(0);
	return StFault;
    }
    return 0;
}

Status fsetc1Ex(INSTINFO *info)
{
    DST1 = SRC1;
    X_FPSR_SF1_CTRLS(DST1) = (X_FPSR_SF0_CTRLS(DST1) & amask7) | omask7;
    if (X_FPSR_SF1_PC(DST1) == 1) {
	reservedRegFieldFault(0);
	return StFault;
    }
    return 0;
}

Status fsetc2Ex(INSTINFO *info)
{
    DST1 = SRC1;
    X_FPSR_SF2_CTRLS(DST1) = (X_FPSR_SF0_CTRLS(DST1) & amask7) | omask7;
    if (X_FPSR_SF2_PC(DST1) == 1) {
	reservedRegFieldFault(0);
	return StFault;
    }
    return 0;
}

Status fsetc3Ex(INSTINFO *info)
{
    DST1 = SRC1;
    X_FPSR_SF3_CTRLS(DST1) = (X_FPSR_SF0_CTRLS(DST1) & amask7) | omask7;
    if (X_FPSR_SF3_PC(DST1) == 1) {
	reservedRegFieldFault(0);
	return StFault;
    }
    return 0;
}

Status fswapEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD mant1, mant2, val;

	mant1 = freg2dword(FSRC1);
	mant2 = freg2dword(FSRC2);
	WD0(val) = WD1(mant2);
	WD1(val) = WD0(mant1);
	FDST1 = dword2freg(val);
    }
    return 0;
}

Status fswapnlEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD mant1, mant2, val;

	mant1 = freg2dword(FSRC1);
	mant2 = freg2dword(FSRC2);
	WD0(val) = WD1(mant2);
	WD1(val) = WD0(mant1);
	SIGN(val) = !BitfX(mant2,32,1);
	FDST1 = dword2freg(val);
    }
    return 0;
}

Status fswapnrEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD mant1, mant2, val;

	mant1 = freg2dword(FSRC1);
	mant2 = freg2dword(FSRC2);
	WD0(val) = WD1(mant2);
	WD1(val) = WD0(mant1);
	BitfR(val,32,1) = !SIGN(mant1);
	FDST1 = dword2freg(val);
    }
    return 0;
}

Status fsxtlEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD mant1, mant2, val;

	mant1 = freg2dword(FSRC1);
	mant2 = freg2dword(FSRC2);
	WD0(val) = SIGN(mant1) ? ~0U : 0U;
	WD1(val) = WD0(mant2);
	FDST1 = dword2freg(val);
    }
    return 0;
}

Status fsxtrEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD mant1, mant2, val;

	mant1 = freg2dword(FSRC1);
	mant2 = freg2dword(FSRC2);
	WD0(val) = BitfX(mant1,32,1) ? ~0U : 0U;
	WD1(val) = WD1(mant2);
	FDST1 = dword2freg(val);
    }
    return 0;
}

Status fxorEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD val;

	val = freg2dword(FSRC1) ^ freg2dword(FSRC2);
	FDST1 = dword2freg(val);
    }
    return 0;
}

Status getfsEx(INSTINFO *info)
{
    DST1 = freg2sgl(FSRC1);
    DSTNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    return 0;
}

Status getfdEx(INSTINFO *info)
{
    DST1 = freg2dbl(FSRC1);
    DSTNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    return 0;
}

Status getfexpEx(INSTINFO *info)
{
    BYTE sign;
    WORD exp;
    DWORD mant;

    freg2spill(FSRC1, &sign, &exp, &mant);
    DST1 = SEXP(sign, exp);
    DSTNAT1 = NATVAL(sign, exp, mant);
    return 0;
}

Status getfsigEx(INSTINFO *info)
{
    BYTE sign;
    WORD exp;

    freg2spill(FSRC1, &sign, &exp, &DST1);
    DSTNAT1 = NATVAL(sign, exp, DST1);
    return 0;
}

Status setfsEx(INSTINFO *info)
{
    if (SRCNAT1) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else
	FDST1 = sgl2freg(SRC1);
    return 0;
}

Status setfdEx(INSTINFO *info)
{
    if (SRCNAT1) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else
	FDST1 = dbl2freg(SRC1);
    return 0;
}

Status setfexpEx(INSTINFO *info)
{
    if (SRCNAT1) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else
	FDST1 = fill2freg(BitfX(SRC1,46,1), BitfX(SRC1,47,17),
			  0x8000000000000000ULL);
    return 0;
}

Status setfsigEx(INSTINFO *info)
{
    if (SRCNAT1) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else
	FDST1 = dword2freg(SRC1);
    return 0;
}

Status xmahEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2 || SRCNAT3) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD m1, m2, m3, val;

	m1 = freg2dword(FSRC1);
	m2 = freg2dword(FSRC2);
	m3 = freg2dword(FSRC3);
	val = xmah(m1, m2, m3);
	FDST1 = dword2freg(val);
    }
    return 0;
}

Status xmahuEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2 || SRCNAT3) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD m1, m2, m3, val;

	m1 = freg2dword(FSRC1);
	m2 = freg2dword(FSRC2);
	m3 = freg2dword(FSRC3);
	val = xmahu(m1, m2, m3);
	FDST1 = dword2freg(val);
    }
    return 0;
}

Status xmalEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2 || SRCNAT3) {
	FDST1.special = YES;
	FDST1.class = CLASS_NAT;
    } else {
	DWORD m1, m2, m3, val;

	m1 = freg2dword(FSRC1);
	m2 = freg2dword(FSRC2);
	m3 = freg2dword(FSRC3);
	val = m1 * m2 + m3;
	FDST1 = dword2freg(val);
    }
    return 0;
}
