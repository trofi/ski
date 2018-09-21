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
#include "memory.h"

Status fetchaddEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r1)) {
	illegalOpFault();
	return StFault;
    }
    if (SRCNAT2) {
	regNatConsumptionFault(MEM_SEMA_ACCESS);
	return StFault;
    }
    FM_mem_cache_update(SRC2, info);
    return 0;
}

Status ld1Ex(INSTINFO *info)
{
    LD_EX(BYTE,1,NO);
    return 0;
}

Status ld2Ex(INSTINFO *info)
{
    LD_EX(HWORD,2,NO);
    return 0;
}

Status ld4Ex(INSTINFO *info)
{
    LD_EX(WORD,4,NO);
    return 0;
}

Status ld8Ex(INSTINFO *info)
{
    LD_EX(DWORD,8,NO);
    return 0;
}

Status ld16Ex(INSTINFO *info)
{
    LD16_EX(QWORD,16,NO);
    return 0;
}

Status ldFillEx(INSTINFO *info)
{
    BOOL nat1 = BitfX(UNAT,63-BitfX(SRC1,55,6),1);

    LD_EX(DWORD,8,nat1);
    return 0;
}

Status ld1PiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_EX(BYTE,1,NO);
    POST_INC();
    return 0;
}

Status ld2PiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_EX(HWORD,2,NO);
    POST_INC();
    return 0;
}

Status ld4PiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_EX(WORD,4,NO);
    POST_INC();
    return 0;
}

Status ld8PiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_EX(DWORD,8,NO);
    POST_INC();
    return 0;
}

Status ldFillPiEx(INSTINFO *info)
{
    BOOL nat1 = BitfX(UNAT,63-BitfX(SRC1,55,6),1);

    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_EX(DWORD,8,nat1);
    POST_INC();
    return 0;
}

Status ld1aEx(INSTINFO *info)
{
    LD_A_EX(BYTE,1);
    return 0;
}

Status ld2aEx(INSTINFO *info)
{
    LD_A_EX(HWORD,2);
    return 0;
}

Status ld4aEx(INSTINFO *info)
{
    LD_A_EX(WORD,4);
    return 0;
}

Status ld8aEx(INSTINFO *info)
{
    LD_A_EX(DWORD,8);
    return 0;
}

Status ld1aPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_A_EX(BYTE,1);
    POST_INC();
    return 0;
}

Status ld2aPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_A_EX(HWORD,2);
    POST_INC();
    return 0;
}

Status ld4aPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_A_EX(WORD,4);
    POST_INC();
    return 0;
}

Status ld8aPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_A_EX(DWORD,8);
    POST_INC();
    return 0;
}

Status ld1sEx(INSTINFO *info)
{
    LD_S_EX(BYTE,1);
    return 0;
}

Status ld2sEx(INSTINFO *info)
{
    LD_S_EX(HWORD,2);
    return 0;
}

Status ld4sEx(INSTINFO *info)
{
    LD_S_EX(WORD,4);
    return 0;
}

Status ld8sEx(INSTINFO *info)
{
    LD_S_EX(DWORD,8);
    return 0;
}

Status ld1sPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_S_EX(BYTE,1);
    POST_INC_SPEC();
    return 0;
}

Status ld2sPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_S_EX(HWORD,2);
    POST_INC_SPEC();
    return 0;
}

Status ld4sPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_S_EX(WORD,4);
    POST_INC_SPEC();
    return 0;
}

Status ld8sPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_S_EX(DWORD,8);
    POST_INC_SPEC();
    return 0;
}

Status ld1saEx(INSTINFO *info)
{
    LD_SA_EX(BYTE,1);
    return 0;
}

Status ld2saEx(INSTINFO *info)
{
    LD_SA_EX(HWORD,2);
    return 0;
}

Status ld4saEx(INSTINFO *info)
{
    LD_SA_EX(WORD,4);
    return 0;
}

Status ld8saEx(INSTINFO *info)
{
    LD_SA_EX(DWORD,8);
    return 0;
}

Status ld1saPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_SA_EX(BYTE,1);
    POST_INC_SPEC();
    return 0;
}

Status ld2saPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_SA_EX(HWORD,2);
    POST_INC_SPEC();
    return 0;
}

Status ld4saPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_SA_EX(WORD,4);
    POST_INC_SPEC();
    return 0;
}

Status ld8saPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_SA_EX(DWORD,8);
    POST_INC_SPEC();
    return 0;
}

Status ld1cncEx(INSTINFO *info)
{
    LD_C_NC_EX(BYTE,1);
    return 0;
}

Status ld2cncEx(INSTINFO *info)
{
    LD_C_NC_EX(HWORD,2);
    return 0;
}

Status ld4cncEx(INSTINFO *info)
{
    LD_C_NC_EX(WORD,4);
    return 0;
}

Status ld8cncEx(INSTINFO *info)
{
    LD_C_NC_EX(DWORD,8);
    return 0;
}

Status ld1cncPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_C_NC_EX(BYTE,1);
    POST_INC();
    return 0;
}

Status ld2cncPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_C_NC_EX(HWORD,2);
    POST_INC();
    return 0;
}

Status ld4cncPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_C_NC_EX(WORD,4);
    POST_INC();
    return 0;
}

Status ld8cncPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_C_NC_EX(DWORD,8);
    POST_INC();
    return 0;
}

Status ld1cclrEx(INSTINFO *info)
{
    LD_C_CLR_EX(BYTE,1);
    return 0;
}

Status ld2cclrEx(INSTINFO *info)
{
    LD_C_CLR_EX(HWORD,2);
    return 0;
}

Status ld4cclrEx(INSTINFO *info)
{
    LD_C_CLR_EX(WORD,4);
    return 0;
}

Status ld8cclrEx(INSTINFO *info)
{
    LD_C_CLR_EX(DWORD,8);
    return 0;
}

Status ld1cclrPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_C_CLR_EX(BYTE,1);
    POST_INC();
    return 0;
}

Status ld2cclrPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_C_CLR_EX(HWORD,2);
    POST_INC();
    return 0;
}

Status ld4cclrPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_C_CLR_EX(WORD,4);
    POST_INC();
    return 0;
}

Status ld8cclrPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LD_C_CLR_EX(DWORD,8);
    POST_INC();
    return 0;
}

Status ldfsEx(INSTINFO *info)
{
    LDF_EX(WORD,4,sgl);
    return 0;
}

Status ldfdEx(INSTINFO *info)
{
    LDF_EX(DWORD,8,dbl);
    return 0;
}

Status ldf8Ex(INSTINFO *info)
{
    LDF_EX(DWORD,8,dword);
    return 0;
}

Status ldfeEx(INSTINFO *info)
{
    LDF_EX(QWORD,16,ext);
    return 0;
}

Status ldfFillEx(INSTINFO *info)
{
    LDF_EX(QWORD,16,qword);
    return 0;
}

Status ldfpsEx(INSTINFO *info)
{
    DWORD dval;

    LDFP_MEM(8,&dval);
    LDFP_DST_S();
    LDFP_POST_INC(8);
    return 0;
}

Status ldfpdEx(INSTINFO *info)
{
    QWORD val;

    LDFP_MEM(16,&val);
    LDFP_DST_D();
    LDFP_POST_INC(16);
    return 0;
}

Status ldfp8Ex(INSTINFO *info)
{
    QWORD mant;

    LDFP_MEM(16,&mant);
    LDFP_DST_8();
    LDFP_POST_INC(16);
    return 0;
}

Status ldfsPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_EX(WORD,4,sgl);
    POST_INC();
    return 0;
}

Status ldfdPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_EX(DWORD,8,dbl);
    POST_INC();
    return 0;
}

Status ldf8PiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_EX(DWORD,8,dword);
    POST_INC();
    return 0;
}

Status ldfePiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_EX(QWORD,16,ext);
    POST_INC();
    return 0;
}

Status ldfFillPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_EX(QWORD,16,qword);
    POST_INC();
    return 0;
}

Status ldfsaEx(INSTINFO *info)
{
    LDF_A_EX(WORD,4,sgl,val);
    return 0;
}

Status ldfdaEx(INSTINFO *info)
{
    LDF_A_EX(DWORD,8,dbl,val);
    return 0;
}

Status ldf8aEx(INSTINFO *info)
{
    LDF_A_EX(DWORD,8,dword,val);
    return 0;
}

Status ldfeaEx(INSTINFO *info)
{
    LDF_A_EX(QWORD,16,ext,val.left = val.right);
    return 0;
}

Status ldfpsaEx(INSTINFO *info)
{
    DWORD dval;
    int ret;

    LDFP_ADV(8,&dval);
    if (ret == 1)
	dval = 0;
    LDFP_DST_S();
    LDFP_POST_INC(8);
    return 0;
}

Status ldfpdaEx(INSTINFO *info)
{
    QWORD val;
    int ret;

    LDFP_ADV(16,&val);
    if (ret == 1)
	val.left = val.right = 0;
    LDFP_DST_D();
    LDFP_POST_INC(16);
    return 0;
}

Status ldfp8aEx(INSTINFO *info)
{
    QWORD mant;
    int ret;

    LDFP_ADV(16,&mant);
    if (ret == 1)
	mant.left = mant.right = 0;
    LDFP_DST_8();
    LDFP_POST_INC(16);
    return 0;
}

Status ldfsaPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_A_EX(WORD,4,sgl,val);
    POST_INC();
    return 0;
}

Status ldfdaPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_A_EX(DWORD,8,dbl,val);
    POST_INC();
    return 0;
}

Status ldf8aPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_A_EX(DWORD,8,dword,val);
    POST_INC();
    return 0;
}

Status ldfeaPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_A_EX(QWORD,16,ext,val.left = val.right);
    POST_INC();
    return 0;
}

Status ldfssEx(INSTINFO *info)
{
    LDF_S_EX(WORD,4,sgl);
    return 0;
}

Status ldfdsEx(INSTINFO *info)
{
    LDF_S_EX(DWORD,8,dbl);
    return 0;
}

Status ldf8sEx(INSTINFO *info)
{
    LDF_S_EX(DWORD,8,dword);
    return 0;
}

Status ldfesEx(INSTINFO *info)
{
    LDF_S_EX(QWORD,16,ext);
    return 0;
}

Status ldfpssEx(INSTINFO *info)
{
    DWORD dval;
    int ret;

    LDFP_SPEC(8,SPEC_LD_ACCESS,&dval,S);
    LDFP_POST_INC_SPEC(8);
    return 0;
}

Status ldfpdsEx(INSTINFO *info)
{
    QWORD val;
    int ret;

    LDFP_SPEC(16,SPEC_LD_ACCESS,&val,D);
    LDFP_POST_INC_SPEC(16);
    return 0;
}

Status ldfp8sEx(INSTINFO *info)
{
    QWORD mant;
    int ret;

    LDFP_SPEC(16,SPEC_LD_ACCESS,&mant,8);
    LDFP_POST_INC_SPEC(16);
    return 0;
}

Status ldfssPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_S_EX(WORD,4,sgl);
    POST_INC_SPEC();
    return 0;
}

Status ldfdsPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_S_EX(DWORD,8,dbl);
    POST_INC_SPEC();
    return 0;
}

Status ldf8sPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_S_EX(DWORD,8,dword);
    POST_INC_SPEC();
    return 0;
}

Status ldfesPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_S_EX(QWORD,16,ext);
    POST_INC_SPEC();
    return 0;
}

Status ldfssaEx(INSTINFO *info)
{
    LDF_SA_EX(WORD,4,sgl);
    return 0;
}

Status ldfdsaEx(INSTINFO *info)
{
    LDF_SA_EX(DWORD,8,dbl);
    return 0;
}

Status ldf8saEx(INSTINFO *info)
{
    LDF_SA_EX(DWORD,8,dword);
    return 0;
}

Status ldfesaEx(INSTINFO *info)
{
    LDF_SA_EX(QWORD,16,ext);
    return 0;
}

Status ldfpssaEx(INSTINFO *info)
{
    DWORD dval;
    int ret = 1;

    LDFP_SPEC(8,SPEC_ADV_LD_ACCESS,&dval,S);
    alat_inval_single_entry(YES, f1);
    if (ALAT_WRITE_CHECK && !ret)
	alat_write(YES, f1, SRC1, 8);
    LDFP_POST_INC_SPEC(8);
    return 0;
}

Status ldfpdsaEx(INSTINFO *info)
{
    QWORD val;
    int ret = 1;

    LDFP_SPEC(16,SPEC_ADV_LD_ACCESS,&val,D);
    alat_inval_single_entry(YES, f1);
    if (ALAT_WRITE_CHECK && !ret)
	alat_write(YES, f1, SRC1, 16);
    LDFP_POST_INC_SPEC(16);
    return 0;
}

Status ldfp8saEx(INSTINFO *info)
{
    QWORD mant;
    int ret = 1;

    LDFP_SPEC(16,SPEC_ADV_LD_ACCESS,&mant,8);
    alat_inval_single_entry(YES, f1);
    if (ALAT_WRITE_CHECK && !ret)
	alat_write(YES, f1, SRC1, 16);
    LDFP_POST_INC_SPEC(16);
    return 0;
}

Status ldfssaPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_SA_EX(WORD,4,sgl);
    POST_INC_SPEC();
    return 0;
}

Status ldfdsaPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_SA_EX(DWORD,8,dbl);
    POST_INC_SPEC();
    return 0;
}

Status ldf8saPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_SA_EX(DWORD,8,dword);
    POST_INC_SPEC();
    return 0;
}

Status ldfesaPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_SA_EX(QWORD,16,ext);
    POST_INC_SPEC();
    return 0;
}

Status ldfscncEx(INSTINFO *info)
{
    LDF_C_NC_EX(WORD,4,sgl);
    return 0;
}

Status ldfdcncEx(INSTINFO *info)
{
    LDF_C_NC_EX(DWORD,8,dbl);
    return 0;
}

Status ldf8cncEx(INSTINFO *info)
{
    LDF_C_NC_EX(DWORD,8,dword);
    return 0;
}

Status ldfecncEx(INSTINFO *info)
{
    LDF_C_NC_EX(QWORD,16,ext);
    return 0;
}

Status ldfpscncEx(INSTINFO *info)
{
    DWORD dval;
    int miss;

    miss = ALAT_WRITE_CHECK ? alat_cmp(YES, f1, NO) : 1;
    if (miss) {
	LDFP_MEM(8,&dval);
	LDFP_DST_S();
	if (ALAT_WRITE_CHECK)
	    alat_write(YES, f1, SRC1, 8);
    } else {
	FDST1 = FrRd(f1);
	FDST2 = FrRd(f2);
    }
    LDFP_POST_INC(8);
    return 0;
}

Status ldfpdcncEx(INSTINFO *info)
{
    QWORD val;
    int miss;

    miss = ALAT_WRITE_CHECK ? alat_cmp(YES, f1, NO) : 1;
    if (miss) {
	LDFP_MEM(16,&val);
	LDFP_DST_D();
	if (ALAT_WRITE_CHECK)
	    alat_write(YES, f1, SRC1, 16);
    } else {
	FDST1 = FrRd(f1);
	FDST2 = FrRd(f2);
    }
    LDFP_POST_INC(16);
    return 0;
}

Status ldfp8cncEx(INSTINFO *info)
{
    QWORD mant;
    int miss;

    miss = ALAT_WRITE_CHECK ? alat_cmp(YES, f1, NO) : 1;
    if (miss) {
	LDFP_MEM(16,&mant);
	LDFP_DST_8();
	if (ALAT_WRITE_CHECK)
	    alat_write(YES, f1, SRC1, 16);
    } else {
	FDST1 = FrRd(f1);
	FDST2 = FrRd(f2);
    }
    LDFP_POST_INC(16);
    return 0;
}

Status ldfscncPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_C_NC_EX(WORD,4,sgl);
    POST_INC();
    return 0;
}

Status ldfdcncPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_C_NC_EX(DWORD,8,dbl);
    POST_INC();
    return 0;
}

Status ldf8cncPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_C_NC_EX(DWORD,8,dword);
    POST_INC();
    return 0;
}

Status ldfecncPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_C_NC_EX(QWORD,16,ext);
    POST_INC();
    return 0;
}

Status ldfscclrEx(INSTINFO *info)
{
    LDF_C_CLR_EX(WORD,4,sgl);
    return 0;
}

Status ldfdcclrEx(INSTINFO *info)
{
    LDF_C_CLR_EX(DWORD,8,dbl);
    return 0;
}

Status ldf8cclrEx(INSTINFO *info)
{
    LDF_C_CLR_EX(DWORD,8,dword);
    return 0;
}

Status ldfecclrEx(INSTINFO *info)
{
    LDF_C_CLR_EX(QWORD,16,ext);
    return 0;
}

Status ldfpscclrEx(INSTINFO *info)
{
    DWORD dval;
    int miss;

    miss = ALAT_WRITE_CHECK ? alat_cmp(YES, f1, YES) : 1;
    if (miss) {
	LDFP_MEM(8,&dval);
	LDFP_DST_S();
    } else {
	FDST1 = FrRd(f1);
	FDST2 = FrRd(f2);
    }
    LDFP_POST_INC(8);
    return 0;
}

Status ldfpdcclrEx(INSTINFO *info)
{
    QWORD val;
    int miss;

    miss = ALAT_WRITE_CHECK ? alat_cmp(YES, f1, YES) : 1;
    if (miss) {
	LDFP_MEM(16,&val);
	LDFP_DST_D();
    } else {
	FDST1 = FrRd(f1);
	FDST2 = FrRd(f2);
    }
    LDFP_POST_INC(16);
    return 0;
}

Status ldfp8cclrEx(INSTINFO *info)
{
    QWORD mant;
    int miss;

    miss = ALAT_WRITE_CHECK ? alat_cmp(YES, f1, YES) : 1;
    if (miss) {
	LDFP_MEM(16,&mant);
	LDFP_DST_8();
    } else {
	FDST1 = FrRd(f1);
	FDST2 = FrRd(f2);
    }
    LDFP_POST_INC(16);
    return 0;
}

Status ldfscclrPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_C_CLR_EX(WORD,4,sgl);
    POST_INC();
    return 0;
}

Status ldfdcclrPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_C_CLR_EX(DWORD,8,dbl);
    POST_INC();
    return 0;
}

Status ldf8cclrPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_C_CLR_EX(DWORD,8,dword);
    POST_INC();
    return 0;
}

Status ldfecclrPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    LDF_C_CLR_EX(QWORD,16,ext);
    POST_INC();
    return 0;
}

Status lfetchEx(INSTINFO *info)
{
    if (!SRCNAT1 && !PSR_ED) {
	(void)memLPF(SRC1, LFETCH_ACCESS);
	FM_mem_cache_update(SRC1, info);
    }
    return 0;
}

Status lfetchFEx(INSTINFO *info)
{
    if (!PSR_ED) {
	if (SRCNAT1) {
	    regNatConsumptionFault(LFETCH_ACCESS);
	    return StFault;
	}
	if (!memLPF(SRC1, LFETCH_FAULT_ACCESS))
	    return StFault;
	FM_mem_cache_update(SRC1, info);
    }
    return 0;
}

Status lfetchPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    if (!SRCNAT1 && !PSR_ED) {
	(void)memLPF(SRC1, LFETCH_ACCESS);
	FM_mem_cache_update(SRC1, info);
    }
    POST_INC_SPEC();
    return 0;
}

Status lfetchFPiEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r3)) {
	illegalOpFault();
	return StFault;
    }
    if (!PSR_ED) {
	if (SRCNAT1) {
	    regNatConsumptionFault(LFETCH_ACCESS);
	    return StFault;
	}
	if (!memLPF(SRC1, LFETCH_FAULT_ACCESS))
	    return StFault;
	FM_mem_cache_update(SRC1, info);
    }
    POST_INC_SPEC();
    return 0;
}

Status stEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(MEM_ST_ACCESS);
	return StFault;
    }
    DST1 = SRC1;
    return 0;
}

Status st16Ex(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(MEM_ST_ACCESS);
	return StFault;
    }
    /* swap the 2 operands because memWrt16 will swap them back */
    if (PSR_BE) {
	DST1 = SRC1;
	DST2 = SRC3;
    } else {
	DST1 = SRC3;
	DST2 = SRC1;
    }
    return 0;
}

Status stSpillEx(INSTINFO *info)
{
    unsigned pos = BitfX(SRC2,55,6);

    if (SRCNAT2) {
	regNatConsumptionFault(MEM_ST_ACCESS);
	return StFault;
    }
    DST1 = SRC1;
    DST2 = (UNAT & ~(1ULL << pos)) | ((REG)SRCNAT1 << pos);
    return 0;
}

Status stfsEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(MEM_ST_ACCESS);
	return StFault;
    }
    DST1 = freg2sgl(FSRC1);
    return 0;
}

Status stfdEx(INSTINFO *info)
{
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(MEM_ST_ACCESS);
	return StFault;
    }
    DST1 = freg2dbl(FSRC1);
    return 0;
}

Status stf8Ex(INSTINFO *info)
{
    BYTE sign;
    WORD exp;

    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(MEM_ST_ACCESS);
	return StFault;
    }
    freg2spill(FSRC1, &sign, &exp, &DST1);
    return 0;
}

Status stfSpillEx(INSTINFO *info)
{
    BYTE sign;
    WORD exp;

    if (SRCNAT2) {
	regNatConsumptionFault(MEM_ST_ACCESS);
	return StFault;
    }
    freg2spill(FSRC1, &sign, &exp, &DST2);
    DST1 = SEXP(sign,exp);
    return 0;
}

Status xchgEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r1)) {
	illegalOpFault();
	return StFault;
    }
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(MEM_SEMA_ACCESS);
	return StFault;
    }
    FM_mem_cache_update(SRC2, info);
    return 0;
}
