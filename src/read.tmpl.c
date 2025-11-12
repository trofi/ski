/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Instruction Read Routines
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
#include "itc.h"

Status illRd(INSTINFO *info)
{
    if (!IR->opcode) {
	fprintf(stderr, "Stopping at %016llx due to opcode 0\n", ip);
	running = NO;
    } else
	(void)fprintf(stderr,
		"Illegal instruction - ip = %016llx, opcode %04x\n",
		ip, IR->opcode);
    return ILL_INST;
}

Status nopRd(INSTINFO *info)
{
    return 0;
}

Status unpredRd(INSTINFO *info)
{
    return 1;
}

Status nullRd(INSTINFO *info)
{
    return PrRd(qp);
}

Status twoGrRd(INSTINFO *info)
{
    GrRd(r2, SRC1, SRCNAT1, info->pgrr2 ? &grs[info->pgrr2 - 1] : NULL);
    GrRd(r3, SRC2, SRCNAT2, info->pgrr3 ? &grs[info->pgrr3 - 1] : NULL);
    return PrRd(qp);
}

Status gr0Gr3Rd(INSTINFO *info)
{
    SRC1 = SRCNAT1 = 0;
    GrRd(r3, SRC2, SRCNAT2, info->pgrr3 ? &grs[info->pgrr3 - 1] : NULL);
    QUAL = YES;
    return PrRd(qp);
}

Status tbitGrImmRd(INSTINFO *info)
{
    GrRd(r3, SRC1, SRCNAT1, NO);
    POS  = pos6;
    QUAL = YES;
    return PrRd(qp);
}

Status tbitGIuncRd(INSTINFO *info)
{
    GrRd(r3, SRC1, SRCNAT1, NO);
    POS  = pos6;
    QUAL = PrRd(qp);
    return 1;
}

Status shrpRd(INSTINFO *info)
{
    GrRd(r2, SRC1, SRCNAT1, NO);
    GrRd(r3, SRC2, SRCNAT2, NO);
    CNT = count6;
    return PrRd(qp);
}

Status shrTwoGrsRd(INSTINFO *info)
{
    GrRd(r3, SRC1, SRCNAT1, NO);
    GrRd(r2, CNT, SRCNAT2, NO);
    return PrRd(qp);
}

Status shrGrImmRd(INSTINFO *info)
{
    GrRd(r3, SRC1, SRCNAT1, NO);
    CNT = count5;
    SRCNAT2 = 0;
    return PrRd(qp);
}

Status shlTwoGrsRd(INSTINFO *info)
{
    GrRd(r2, SRC1, SRCNAT1, NO);
    GrRd(r3, CNT, SRCNAT2, NO);
    return PrRd(qp);
}

Status shlGrImmRd(INSTINFO *info)
{
    GrRd(r2, SRC1, SRCNAT1, NO);
    CNT = count5;
    SRCNAT2 = 0;
    return PrRd(qp);
}

Status mux1Rd(INSTINFO *info)
{
    GrRd(r2, SRC1, SRCNAT1, NO);
    if ((MHT = mbtype4) == reserved) {
	illegalOpFault();
	return StFault;
    }
    return PrRd(qp);
}

Status mux2Rd(INSTINFO *info)
{
    GrRd(r2, SRC1, SRCNAT1, NO);
    MHT = ~mhtype8;	/* reverse endianness */
    return PrRd(qp);
}

Status shaddRd(INSTINFO *info)
{
    GrRd(r2, SRC1, SRCNAT1, NO);
    /* XXX - needed for pshladd2 and pshradd2 only -> use a separate fn? */
    if ((CNT = count2) == reserved) {
	illegalOpFault();
	return StFault;
    }
    GrRd(r3, SRC2, SRCNAT2, NO);
    return PrRd(qp);
}

Status immGrRd(INSTINFO *info)
{
    SRC1 = imm64;
    SRCNAT1 = 0;
    GrRd(r3, SRC2, SRCNAT2, info->pgrr3 ? &grs[info->pgrr3 - 1] : NULL);
    return PrRd(qp);
}

Status cmpGrRd(INSTINFO *info)
{
    GrRd(r2, SRC1, SRCNAT1, info->pgrr2 ? &grs[info->pgrr2 - 1] : NULL);
    GrRd(r3, SRC2, SRCNAT2, info->pgrr3 ? &grs[info->pgrr3 - 1] : NULL);
    QUAL = YES;
    return PrRd(qp);
}

Status cmpiRd(INSTINFO *info)
{
    SRC1 = imm64;
    SRCNAT1 = 0;
    GrRd(r3, SRC2, SRCNAT2, info->pgrr3 ? &grs[info->pgrr3 - 1] : NULL);
    QUAL = YES;
    return PrRd(qp);
}

Status cmpuncRd(INSTINFO *info)
{
    GrRd(r2, SRC1, SRCNAT1, info->pgrr2 ? &grs[info->pgrr2 - 1] : NULL);
    GrRd(r3, SRC2, SRCNAT2, info->pgrr3 ? &grs[info->pgrr3 - 1] : NULL);
    QUAL = PrRd(qp);
    return 1;
}

Status cmpiuncRd(INSTINFO *info)
{
    SRC1 = imm64;
    SRCNAT1 = 0;
    GrRd(r3, SRC2, SRCNAT2, info->pgrr3 ? &grs[info->pgrr3 - 1] : NULL);
    QUAL = PrRd(qp);
    return 1;
}

Status chkarRd(INSTINFO *info)
{
    SRC1 = info->ct->ipn + target25;
    return PrRd(qp);
}

Status chkafRd(INSTINFO *info)
{
    SRC1 = info->ct->ipn + target25;
    return PrRd(qp);
}

Status chksrRd(INSTINFO *info)
{
    SRC1 = info->ct->ipn + target25;
    GrRd(r2, SRC2, QUAL, NO);	/* SRC2 is unused */
    return PrRd(qp);
}

Status chksfRd(INSTINFO *info)
{
    FREG fr = FrRd(f2);
    BOOL fphi = FPHI(f2);
    BOOL fplo = FPLO(f2);

    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    SRC1 = info->ct->ipn + target25;
    QUAL = fr.special && fr.class == CLASS_NAT;
    return PrRd(qp);
}

Status tnatRd(INSTINFO *info)
{
    GrRd(r3, SRC1, SRCNAT1, NO);	/* SRC1 is unused */
    QUAL = YES;
    return PrRd(qp);
}

Status tnatUncRd(INSTINFO *info)
{
    GrRd(r3, SRC1, SRCNAT1, NO);	/* SRC1 is unused */
    QUAL = PrRd(qp);
    return 1;
}

Status grImmRd(INSTINFO *info)
{
    GrRd(r3, SRC1, SRCNAT1, info->pgrr3 ? &grs[info->pgrr3 - 1] : NULL);
    SRC2 = imm64;
    SRCNAT2 = 0;
    return PrRd(qp);
}

Status oneGrRd(INSTINFO *info)
{
    GrRd(r2, SRC1, SRCNAT1, info->pgrr2 ? &grs[info->pgrr2 - 1] : NULL);
    return PrRd(qp);
}

Status gr3Rd(INSTINFO *info)
{
    GrRd(r3, SRC1, SRCNAT1, info->pgrr3 ? &grs[info->pgrr3 - 1] : NULL);
    return PrRd(qp);
}

Status ldfpRd(INSTINFO *info)
{
    BYTE f1Bank = f1 < 32 ? BitfX8(f1,7,1) : BitfX8(f1,7,1) ^ BitfX8(rrbf,7,1);
    BYTE f2Bank = f2 < 32 ? BitfX8(f2,7,1) : BitfX8(f2,7,1) ^ BitfX8(rrbf,7,1);
    BOOL fphi, fplo;

    if (f1Bank == f2Bank || f1 <= 1 || f2 <= 1) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2);
    fplo = FPLO(f1) || FPLO(f2);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, MEM_LD_ACCESS);
	return StFault;
    }
    GrRd(r3, SRC1, SRCNAT1, NO);
    if (SRCNAT1) {
	regNatConsumptionFault(MEM_LD_ACCESS);
	return StFault;
    }
    return PrRd(qp);
}

Status ldfpsRd(INSTINFO *info)
{
    BYTE f1Bank = f1 < 32 ? BitfX8(f1,7,1) : BitfX8(f1,7,1) ^ BitfX8(rrbf,7,1);
    BYTE f2Bank = f2 < 32 ? BitfX8(f2,7,1) : BitfX8(f2,7,1) ^ BitfX8(rrbf,7,1);
    BOOL fphi, fplo;

    if (f1Bank == f2Bank || f1 <= 1 || f2 <= 1) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2);
    fplo = FPLO(f1) || FPLO(f2);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, SPEC_LD_ACCESS);
	return StFault;
    }
    GrRd(r3, SRC1, SRCNAT1, NO);
    return PrRd(qp);
}

Status ldfpPiRd(INSTINFO *info)
{
    BYTE f1Bank = f1 < 32 ? BitfX8(f1,7,1) : BitfX8(f1,7,1) ^ BitfX8(rrbf,7,1);
    BYTE f2Bank = f2 < 32 ? BitfX8(f2,7,1) : BitfX8(f2,7,1) ^ BitfX8(rrbf,7,1);
    BOOL fphi, fplo;

    if (f1Bank == f2Bank || f1 <= 1 || f2 <= 1 || OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2);
    fplo = FPLO(f1) || FPLO(f2);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, MEM_LD_ACCESS);
	return StFault;
    }
    GrRd(r3, SRC1, SRCNAT1, NO);
    if (SRCNAT1) {
	regNatConsumptionFault(MEM_LD_ACCESS);
	return StFault;
    }
    return PrRd(qp);
}

Status ldfpsPiRd(INSTINFO *info)
{
    BYTE f1Bank = f1 < 32 ? BitfX8(f1,7,1) : BitfX8(f1,7,1) ^ BitfX8(rrbf,7,1);
    BYTE f2Bank = f2 < 32 ? BitfX8(f2,7,1) : BitfX8(f2,7,1) ^ BitfX8(rrbf,7,1);
    BOOL fphi, fplo;

    if (f1Bank == f2Bank || f1 <= 1 || f2 <= 1 || OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2);
    fplo = FPLO(f1) || FPLO(f2);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, SPEC_LD_ACCESS);
	return StFault;
    }
    GrRd(r3, SRC1, SRCNAT1, NO);
    return PrRd(qp);
}

Status ldPiRd(INSTINFO *info)
{
    GrRd(r3, SRC1, SRCNAT1, NO);
    GrRd(r2, SRC2, SRCNAT2, NO);
    return PrRd(qp);
}

Status oneImmRd(INSTINFO *info)
{
    SRC1 = imm64;
    SRCNAT1 = 0;
    return PrRd(qp);
}

Status allPrRd(INSTINFO *info)
{
    int i;
    BYTE tmp_rrbp = rrbp;

    rrbp = 0;
    SRC1 = 0;
    for (i = 63; i >= 0; i--)
	SRC1 = SRC1 << 1 | (PrRd(i) & 0x1);
    rrbp = tmp_rrbp;
    return PrRd(qp);
}

Status oneFrRd(INSTINFO *info)
{
    BOOL fphi, fplo;

    if (f1 <= 1) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2);
    fplo = FPLO(f1) || FPLO(f2);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    return PrRd(qp);
}

Status getfRd(INSTINFO *info)
{
    if (OUT_OF_FRAME(r1)) {
	illegalOpFault();
	return StFault;
    }
    if ((PSR_DFH && FPHI(f2)) || (PSR_DFL && FPLO(f2))) {
	disabledFpregFault(FPHI(f2) << 1 | FPLO(f2), 0);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    return PrRd(qp);
}

Status setfRd(INSTINFO *info)
{
    if (f1 <= 1) {
	illegalOpFault();
	return StFault;
    }
    if ((PSR_DFH && FPHI(f1)) || (PSR_DFL && FPLO(f1))) {
	disabledFpregFault(FPHI(f1) << 1 | FPLO(f1), 0);
	return StFault;
    }
    GrRd(r2, SRC1, SRCNAT1, NO);
    return PrRd(qp);
}

Status fclassRd(INSTINFO *info)
{
    if ((PSR_DFH && FPHI(f2)) || (PSR_DFL && FPLO(f2))) {
	disabledFpregFault(FPHI(f2) << 1 | FPLO(f2), 0);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    QUAL = YES;
    return PrRd(qp);
}

Status fclassUncRd(INSTINFO *info)
{
    FSRC1 = FrRd(f2);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    QUAL = PrRd(qp);
    if (QUAL && ((PSR_DFH && FPHI(f2)) || (PSR_DFL && FPLO(f2)))) {
	disabledFpregFault(FPHI(f2) << 1 | FPLO(f2), 0);
	return StFault;
    }
    return 1;
}

Status fr2s0Rd(INSTINFO *info)
{
    BOOL fphi, fplo;

    if (f1 <= 1) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2);
    fplo = FPLO(f1) || FPLO(f2);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    CTRLS = FPSR_SF0_CTRLS;
    TRAPS = FPSR_TRAPS;
    return PrRd(qp);
}

Status fr2s1Rd(INSTINFO *info)
{
    BOOL fphi, fplo;

    if (f1 <= 1) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2);
    fplo = FPLO(f1) || FPLO(f2);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    CTRLS = FPSR_SF1_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    return PrRd(qp);
}

Status fr2s2Rd(INSTINFO *info)
{
    BOOL fphi, fplo;

    if (f1 <= 1) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2);
    fplo = FPLO(f1) || FPLO(f2);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    CTRLS = FPSR_SF2_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    return PrRd(qp);
}

Status fr2s3Rd(INSTINFO *info)
{
    BOOL fphi, fplo;

    if (f1 <= 1) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2);
    fplo = FPLO(f1) || FPLO(f2);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    CTRLS = FPSR_SF3_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    return PrRd(qp);
}

Status fr3s0Rd(INSTINFO *info)
{
    FSRC1 = FrRd(f3);
    TRAPS = FPSR_TRAPS;
    QUAL = PrRd(qp);
    if (QUAL) {
	BOOL fphi, fplo;

	if (f1 <= 1) {
	    illegalOpFault();
	    return StFault;
	}
	fphi = FPHI(f1) || FPHI(f3);
	fplo = FPLO(f1) || FPLO(f3);
	if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	    disabledFpregFault(fphi << 1 | fplo, 0);
	    return StFault;
	}
    }
    return 1;
}

Status fr3s1Rd(INSTINFO *info)
{
    FSRC1 = FrRd(f3);
    CTRLS = FPSR_SF1_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    QUAL = PrRd(qp);
    if (QUAL) {
	BOOL fphi, fplo;

	if (f1 <= 1) {
	    illegalOpFault();
	    return StFault;
	}
	fphi = FPHI(f1) || FPHI(f3);
	fplo = FPLO(f1) || FPLO(f3);
	if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	    disabledFpregFault(fphi << 1 | fplo, 0);
	    return StFault;
	}
    }
    return 1;
}

Status fr3s2Rd(INSTINFO *info)
{
    FSRC1 = FrRd(f3);
    CTRLS = FPSR_SF2_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    QUAL = PrRd(qp);
    if (QUAL) {
	BOOL fphi, fplo;

	if (f1 <= 1) {
	    illegalOpFault();
	    return StFault;
	}
	fphi = FPHI(f1) || FPHI(f3);
	fplo = FPLO(f1) || FPLO(f3);
	if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	    disabledFpregFault(fphi << 1 | fplo, 0);
	    return StFault;
	}
    }
    return 1;
}

Status fr3s3Rd(INSTINFO *info)
{
    FSRC1 = FrRd(f3);
    CTRLS = FPSR_SF3_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    QUAL = PrRd(qp);
    if (QUAL) {
	BOOL fphi, fplo;

	if (f1 <= 1) {
	    illegalOpFault();
	    return StFault;
	}
	fphi = FPHI(f1) || FPHI(f3);
	fplo = FPLO(f1) || FPLO(f3);
	if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	    disabledFpregFault(fphi << 1 | fplo, 0);
	    return StFault;
	}
    }
    return 1;
}

Status stfRd(INSTINFO *info)
{
    BOOL fphi, fplo;

    fphi = FPHI(f2);
    fplo = FPLO(f2);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, MEM_ST_ACCESS);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    GrRd(r3, SRC2, SRCNAT2, NO);
    return PrRd(qp);
}

Status stfPiRd(INSTINFO *info)
{
    BOOL fphi, fplo;

    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f2);
    fplo = FPLO(f2);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, MEM_ST_ACCESS);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    GrRd(r3, SRC2, SRCNAT2, NO);
    return PrRd(qp);
}

Status twoFrRd(INSTINFO *info)
{
    BOOL fphi, fplo;

    if (f1 <= 1) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2) || FPHI(f3);
    fplo = FPLO(f1) || FPLO(f2) || FPLO(f3);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    FSRC2 = FrRd(f3);
    SRCNAT2 = FSRC2.special && FSRC2.class == CLASS_NAT;
    return PrRd(qp);
}

Status twoFrs0Rd(INSTINFO *info)
{
    BOOL fphi, fplo;

    if (f1 <= 1) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2) || FPHI(f3);
    fplo = FPLO(f1) || FPLO(f2) || FPLO(f3);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    FSRC2 = FrRd(f3);
    SRCNAT2 = FSRC2.special && FSRC2.class == CLASS_NAT;
    TRAPS = FPSR_TRAPS;
    QUAL  = YES;
    return PrRd(qp);
}

Status twoFrs1Rd(INSTINFO *info)
{
    BOOL fphi, fplo;

    if (f1 <= 1) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2) || FPHI(f3);
    fplo = FPLO(f1) || FPLO(f2) || FPLO(f3);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    FSRC2 = FrRd(f3);
    SRCNAT2 = FSRC2.special && FSRC2.class == CLASS_NAT;
    CTRLS = FPSR_SF1_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    QUAL  = YES;
    return PrRd(qp);
}

Status twoFrs2Rd(INSTINFO *info)
{
    BOOL fphi, fplo;

    if (f1 <= 1) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2) || FPHI(f3);
    fplo = FPLO(f1) || FPLO(f2) || FPLO(f3);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    FSRC2 = FrRd(f3);
    SRCNAT2 = FSRC2.special && FSRC2.class == CLASS_NAT;
    CTRLS = FPSR_SF2_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    QUAL  = YES;
    return PrRd(qp);
}

Status twoFrs3Rd(INSTINFO *info)
{
    BOOL fphi, fplo;

    if (f1 <= 1) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2) || FPHI(f3);
    fplo = FPLO(f1) || FPLO(f2) || FPLO(f3);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    FSRC2 = FrRd(f3);
    SRCNAT2 = FSRC2.special && FSRC2.class == CLASS_NAT;
    CTRLS = FPSR_SF3_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    QUAL  = YES;
    return PrRd(qp);
}

Status fcmps0Rd(INSTINFO *info)
{
    BOOL fphi, fplo;

    fphi = FPHI(f2) || FPHI(f3);
    fplo = FPLO(f2) || FPLO(f3);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    FSRC2 = FrRd(f3);
    SRCNAT2 = FSRC2.special && FSRC2.class == CLASS_NAT;
    TRAPS = FPSR_TRAPS;
    QUAL  = YES;
    return PrRd(qp);
}

Status fcmps1Rd(INSTINFO *info)
{
    BOOL fphi, fplo;

    fphi = FPHI(f2) || FPHI(f3);
    fplo = FPLO(f2) || FPLO(f3);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    FSRC2 = FrRd(f3);
    SRCNAT2 = FSRC2.special && FSRC2.class == CLASS_NAT;
    CTRLS = FPSR_SF1_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    QUAL  = YES;
    return PrRd(qp);
}

Status fcmps2Rd(INSTINFO *info)
{
    BOOL fphi, fplo;

    fphi = FPHI(f2) || FPHI(f3);
    fplo = FPLO(f2) || FPLO(f3);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    FSRC2 = FrRd(f3);
    SRCNAT2 = FSRC2.special && FSRC2.class == CLASS_NAT;
    CTRLS = FPSR_SF2_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    QUAL  = YES;
    return PrRd(qp);
}

Status fcmps3Rd(INSTINFO *info)
{
    BOOL fphi, fplo;

    fphi = FPHI(f2) || FPHI(f3);
    fplo = FPLO(f2) || FPLO(f3);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f2);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    FSRC2 = FrRd(f3);
    SRCNAT2 = FSRC2.special && FSRC2.class == CLASS_NAT;
    CTRLS = FPSR_SF3_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    QUAL  = YES;
    return PrRd(qp);
}

Status fcmpUncs0Rd(INSTINFO *info)
{
    FSRC1 = FrRd(f2);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    FSRC2 = FrRd(f3);
    SRCNAT2 = FSRC2.special && FSRC2.class == CLASS_NAT;
    TRAPS = FPSR_TRAPS;
    QUAL  = PrRd(qp);
    if (QUAL) {
	BOOL fphi, fplo;

	fphi = FPHI(f2) || FPHI(f3);
	fplo = FPLO(f2) || FPLO(f3);
	if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	    disabledFpregFault(fphi << 1 | fplo, 0);
	    return StFault;
	}
    }
    return 1;
}

Status fcmpUncs1Rd(INSTINFO *info)
{
    FSRC1 = FrRd(f2);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    FSRC2 = FrRd(f3);
    SRCNAT2 = FSRC2.special && FSRC2.class == CLASS_NAT;
    CTRLS = FPSR_SF1_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    QUAL  = PrRd(qp);
    if (QUAL) {
	BOOL fphi, fplo;

	fphi = FPHI(f2) || FPHI(f3);
	fplo = FPLO(f2) || FPLO(f3);
	if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	    disabledFpregFault(fphi << 1 | fplo, 0);
	    return StFault;
	}
    }
    return 1;
}

Status fcmpUncs2Rd(INSTINFO *info)
{
    FSRC1 = FrRd(f2);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    FSRC2 = FrRd(f3);
    SRCNAT2 = FSRC2.special && FSRC2.class == CLASS_NAT;
    CTRLS = FPSR_SF2_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    QUAL  = PrRd(qp);
    if (QUAL) {
	BOOL fphi, fplo;

	fphi = FPHI(f2) || FPHI(f3);
	fplo = FPLO(f2) || FPLO(f3);
	if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	    disabledFpregFault(fphi << 1 | fplo, 0);
	    return StFault;
	}
    }
    return 1;
}

Status fcmpUncs3Rd(INSTINFO *info)
{
    FSRC1 = FrRd(f2);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    FSRC2 = FrRd(f3);
    SRCNAT2 = FSRC2.special && FSRC2.class == CLASS_NAT;
    CTRLS = FPSR_SF3_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    QUAL  = PrRd(qp);
    if (QUAL) {
	BOOL fphi, fplo;

	fphi = FPHI(f2) || FPHI(f3);
	fplo = FPLO(f2) || FPLO(f3);
	if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	    disabledFpregFault(fphi << 1 | fplo, 0);
	    return StFault;
	}
    }
    return 1;
}

Status frcpas0Rd(INSTINFO *info)
{
    FSRC1 = FrRd(f2);
    FSRC2 = FrRd(f3);
    TRAPS = FPSR_TRAPS;
    QUAL  = PrRd(qp);
    if (QUAL) {
	BOOL fphi, fplo;

	if (f1 <= 1) {
	    illegalOpFault();
	    return StFault;
	}
	fphi = FPHI(f1) || FPHI(f2) || FPHI(f3);
	fplo = FPLO(f1) || FPLO(f2) || FPLO(f3);
	if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	    disabledFpregFault(fphi << 1 | fplo, 0);
	    return StFault;
	}
    }
    return 1;
}

Status frcpas1Rd(INSTINFO *info)
{
    FSRC1 = FrRd(f2);
    FSRC2 = FrRd(f3);
    CTRLS = FPSR_SF1_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    QUAL  = PrRd(qp);
    if (QUAL) {
	BOOL fphi, fplo;

	if (f1 <= 1) {
	    illegalOpFault();
	    return StFault;
	}
	fphi = FPHI(f1) || FPHI(f2) || FPHI(f3);
	fplo = FPLO(f1) || FPLO(f2) || FPLO(f3);
	if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	    disabledFpregFault(fphi << 1 | fplo, 0);
	    return StFault;
	}
    }
    return 1;
}

Status frcpas2Rd(INSTINFO *info)
{
    FSRC1 = FrRd(f2);
    FSRC2 = FrRd(f3);
    CTRLS = FPSR_SF2_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    QUAL  = PrRd(qp);
    if (QUAL) {
	BOOL fphi, fplo;

	if (f1 <= 1) {
	    illegalOpFault();
	    return StFault;
	}
	fphi = FPHI(f1) || FPHI(f2) || FPHI(f3);
	fplo = FPLO(f1) || FPLO(f2) || FPLO(f3);
	if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	    disabledFpregFault(fphi << 1 | fplo, 0);
	    return StFault;
	}
    }
    return 1;
}

Status frcpas3Rd(INSTINFO *info)
{
    FSRC1 = FrRd(f2);
    FSRC2 = FrRd(f3);
    CTRLS = FPSR_SF3_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    QUAL  = PrRd(qp);
    if (QUAL) {
	BOOL fphi, fplo;

	if (f1 <= 1) {
	    illegalOpFault();
	    return StFault;
	}
	fphi = FPHI(f1) || FPHI(f2) || FPHI(f3);
	fplo = FPLO(f1) || FPLO(f2) || FPLO(f3);
	if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	    disabledFpregFault(fphi << 1 | fplo, 0);
	    return StFault;
	}
    }
    return 1;
}

Status fmas0Rd(INSTINFO *info)
{
    BOOL fphi, fplo;

    if (f1 <= 1) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2) || FPHI(f3) || FPHI(f4);
    fplo = FPLO(f1) || FPLO(f2) || FPLO(f3) || FPLO(f4);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f3);
    FSRC2 = FrRd(f4);
    FSRC3 = FrRd(f2);
    CTRLS = FPSR_SF0_CTRLS;
    TRAPS = FPSR_TRAPS;
    return PrRd(qp);
}

Status fmas1Rd(INSTINFO *info)
{
    BOOL fphi, fplo;

    if (f1 <= 1) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2) || FPHI(f3) || FPHI(f4);
    fplo = FPLO(f1) || FPLO(f2) || FPLO(f3) || FPLO(f4);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f3);
    FSRC2 = FrRd(f4);
    FSRC3 = FrRd(f2);
    CTRLS = FPSR_SF1_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    return PrRd(qp);
}

Status fmas2Rd(INSTINFO *info)
{
    BOOL fphi, fplo;

    if (f1 <= 1) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2) || FPHI(f3) || FPHI(f4);
    fplo = FPLO(f1) || FPLO(f2) || FPLO(f3) || FPLO(f4);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f3);
    FSRC2 = FrRd(f4);
    FSRC3 = FrRd(f2);
    CTRLS = FPSR_SF2_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    return PrRd(qp);
}

Status fmas3Rd(INSTINFO *info)
{
    BOOL fphi, fplo;

    if (f1 <= 1) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2) || FPHI(f3) || FPHI(f4);
    fplo = FPLO(f1) || FPLO(f2) || FPLO(f3) || FPLO(f4);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f3);
    FSRC2 = FrRd(f4);
    FSRC3 = FrRd(f2);
    CTRLS = FPSR_SF3_CTRLS;
    TRAPS = TD(CTRLS) ? 0x3F : FPSR_TRAPS;
    return PrRd(qp);
}

Status threeFrRd(INSTINFO *info)
{
    BOOL fphi, fplo;

    if (f1 <= 1) {
	illegalOpFault();
	return StFault;
    }
    fphi = FPHI(f1) || FPHI(f2) || FPHI(f3) || FPHI(f4);
    fplo = FPLO(f1) || FPLO(f2) || FPLO(f3) || FPLO(f4);
    if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {
	disabledFpregFault(fphi << 1 | fplo, 0);
	return StFault;
    }
    FSRC1 = FrRd(f3);
    SRCNAT1 = FSRC1.special && FSRC1.class == CLASS_NAT;
    FSRC2 = FrRd(f4);
    SRCNAT2 = FSRC2.special && FSRC2.class == CLASS_NAT;
    FSRC3 = FrRd(f2);
    SRCNAT3 = FSRC3.special && FSRC3.class == CLASS_NAT;
    return PrRd(qp);
}

Status fpsrRd(INSTINFO *info)
{
    SRC1 = FPSR;
    return PrRd(qp);
}

Status oneBrRd(INSTINFO *info)
{
    SRC1 = BrRd(b2);
    return PrRd(qp);
}

Status ipRd(INSTINFO *info)
{
    SRC1 = info->ct->ipn + imm64;
    return PrRd(qp);
}

Status tgtRd(INSTINFO *info)
{
    SRC1 = info->ct->ipn + target25;
    return PrRd(qp);
}

Status brpRd(INSTINFO *info)
{
    SRC1 = info->ct->ipn + target25;
    return traceEnb;
}

Status brpiRd(INSTINFO *info)
{
    SRC1 = BrRd(b2);
    return traceEnb;
}

Status unpredTgtRd(INSTINFO *info)
{
    SRC1 = info->ct->ipn + target25;
    return 1;
}

Status tgtPrRd(INSTINFO *info)
{
    SRC1 = info->ct->ipn + target25;
    SRC2 = PrRd(qp);
    return 1;
}

Status callRd(INSTINFO *info)
{
    SRC1 = (IPTR(info) & ~(ADDR)0xF) + 16;
    SRC2 = info->ct->ipn + target25;
    return PrRd(qp);
}

Status calliRd(INSTINFO *info)
{
    SRC1 = (IPTR(info) & ~(ADDR)0xF) + 16;
    SRC2 = BrRd(b2) & ~(ADDR)0xF;
    return PrRd(qp);
}

Status brIARd(INSTINFO *info)
{
    SRC1 = BrRd(b2);
    SRC2 = qp;
    return 1;
}

Status oneArRd(INSTINFO *info)
{
    if (ar3 == ITC_ID)
	SRC1 = itc_read();
    else
	SRC1 = ArRd(ar3);
    return PrRd(qp);
}

Status breakRd(INSTINFO *info)
{
    /* XXX - check SRCNATs? */
#if defined __linux__
    int base = 32;

    /* Note: we read out0-out5 here.  This may look counter-intuitive since the
       called function expects the arguments in in0-in5.  However, the normal
       syscall sequence is:

	 br.call syscall_stub -> break 0x100000 -> br.call -> syscall -> br.ret

       The first call renames out0-outN to in0-inN, the second call
       acts as a no-op as far as renaming is concerned because "size
       of frame" (sof) is set to zero by the first br.call.

       However, consider the case of an inlined system call.  In this case, the
       syscall sequence is:

         break 0x100000 -> br.call -> syscall -> br.ret

       This time, the syscall args are in out0-outN when the break
       instruction is executed and (the formerly second) br.call is
       what is causing the renaming from out0-outN to in0-inN.  In
       other words, to make the user-level simulation work both with
       syscall stubs and with inlined stubs, we need to read the
       _output_ registers.  */
    if (unixABI)
        base += soil;
    GrRd(base + 0, ARG0, SRCNAT1, NO);
    GrRd(base + 1, ARG1, SRCNAT1, NO);
    GrRd(base + 2, ARG2, SRCNAT1, NO);
    GrRd(base + 3, ARG3, SRCNAT1, NO);
    GrRd(base + 4, ARG4, SRCNAT1, NO);
    GrRd(base + 5, ARG5, SRCNAT1, NO);
    GrRd(base + 6, ARG6, SRCNAT1, NO);
    GrRd(base + 7, ARG7, SRCNAT1, NO);
    GrRd(15, SC_NUM, SRCNAT1, NO);
#else /* !defined __linux__ */
    GrRd(8, SC_NUM, SRCNAT1, NO);
    GrRd(32, ARG0, SRCNAT1, NO);
    GrRd(33, ARG1, SRCNAT1, NO);
    GrRd(34, ARG2, SRCNAT1, NO);
    GrRd(35, ARG3, SRCNAT1, NO);
    GrRd(36, ARG4, SRCNAT1, NO);
    GrRd(37, ARG5, SRCNAT1, NO);
    GrRd(38, ARG6, SRCNAT1, NO);
    GrRd(39, ARG7, SRCNAT1, NO);
#endif /* !defined __linux */
    SRC1 = ip;
    SRC2 = imm64;
    return PrRd(qp);
}

Status breakbRd(INSTINFO *info)
{
    SRC1 = ip;
    SRC2 = 0;
    return PrRd(qp);
}

Status breakxRd(INSTINFO *info)
{
    SRC1 = ip;
    SRC2 = imm64 & 0x1fffff;    /* only the bottom 21 bits */
    return PrRd(qp);
}

Status allocRd(INSTINFO *info)
{
    SRC1 = PFS;
    SOIL = il;
    SOF  = il + o;
    SOR  = r;
    return 1;
}

Status stRd(INSTINFO *info)
{
    GrRd(r2, SRC1, SRCNAT1, NO);
    GrRd(r3, SRC2, SRCNAT2, NO);
    return PrRd(qp);
}

Status st16Rd(INSTINFO *info)
{
    GrRd(r2, SRC1, SRCNAT1, NO);
    GrRd(r3, SRC2, SRCNAT2, NO);
    SRC3 = ArRd(CSD_ID);
    return PrRd(qp);
}

Status stPiRd(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    GrRd(r2, SRC1, SRCNAT1, NO);
    GrRd(r3, SRC2, SRCNAT2, NO);
    return PrRd(qp);
}

Status deprRd(INSTINFO *info)
{
    GrRd(r2, SRC1, SRCNAT1, NO);
    GrRd(r3, SRC2, SRCNAT2, NO);
    POS = pos6;
    LEN = len4;
    return PrRd(qp);
}

Status depiRd(INSTINFO *info)
{
    SRC1 = imm64;
    SRCNAT1 = 0;
    GrRd(r3, SRC2, SRCNAT2, NO);
    POS = pos6;
    LEN = len6;
    return PrRd(qp);
}

Status depzrRd(INSTINFO *info)
{
    GrRd(r2, SRC1, SRCNAT1, NO);
    POS = pos6;
    return PrRd(qp);
}

Status depziRd(INSTINFO *info)
{
    SRC1 = imm64;
    SRCNAT1 = 0;
    POS = pos6;
    return PrRd(qp);
}

Status extrRd(INSTINFO *info)
{
    GrRd(r3, SRC1, SRCNAT1, NO);
    POS = pos6;
    LEN = len6;
    return PrRd(qp);
}

Status psrRd(INSTINFO *info)
{
    SRC1 = psr;
    CPL = PSR_CPL;
    return PrRd(qp);
}

Status unpredPsrRd(INSTINFO *info)
{
    SRC1 = psr;
    CPL = PSR_CPL;
    return 1;
}

Status psrGrRd(INSTINFO *info)
{
    GrRd(r2, SRC1, SRCNAT1, NO);
    SRC2 = psr;
    CPL = PSR_CPL;
    return PrRd(qp);
}

Status cplGrRd(INSTINFO *info)
{
    GrRd(r2, SRC1, SRCNAT1, NO);
    CPL = PSR_CPL;
    return PrRd(qp);
}

Status cplGr3Rd(INSTINFO *info)
{
    if (OUT_OF_FRAME(r1)) {	/* remove redundant check in write phase? */
	illegalOpFault();
	return StFault;
    }
    GrRd(r3, SRC1, SRCNAT1, NO);
    SRC2 = PSR_SP;
    CPL = PSR_CPL;
    return PrRd(qp);
}

Status ptceRd(INSTINFO *info)
{
    GrRd(r3, SRC1, SRCNAT1, NO);
    CPL = PSR_CPL;
    return PrRd(qp);
}

Status epcRd(INSTINFO *info)
{
    SRC1 = PFS_PPL;
    SRC2 = PSR_IT;
    CPL = PSR_CPL;
    return 1;
}

Status indRegRd(INSTINFO *info)
{
    GrRd(r2, SRC1, SRCNAT1, NO);
    GrRd(r3, SRC2, SRCNAT2, NO);
    CPL = PSR_CPL;
    return PrRd(qp);
}

Status oneCrRd(INSTINFO *info)
{
    SRC1 = CrRd(cr3);
    CPL  = PSR_CPL;
    return PrRd(qp);
}
