/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* DO NOT EDIT - Automatically generated using:
     awk -f fmtimm.awk encodings/encoding.format | awk -f predecode.awk -
 */
/*
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

#include "std.h"
#include "bits.h"
#include "types.h"
#include "sim.h"
#include "state.h"
#include "instinfo.h"
#include "immed.h"
#include "predecode.h"

static DWORD imm41;
static ADDR ippg;

BOOL illpredecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    imm64 = instruct;
    return NO;
}

BOOL illQPpredecode(DWORD instruct, INSTINFO *info)
{
    qp = BitfX(instruct,58,6);
    imm64 = instruct;
    return NO;
}

BOOL illBQPpredecode(DWORD instruct, INSTINFO *info)
{
    qp = BitfX(instruct,58,6);
    imm64 = instruct;
    return NO;
}

BOOL lUnitImm41predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    imm41 = instruct;
    return YES;
}

BOOL ignpredecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    imm64 = instruct;
    return YES;
}

BOOL A1predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL A2predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    count2 = count2A2(BitfX(instruct,35,2));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL A3predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    imm8 = imm8A3(BitfX(instruct,44,7),BitfX(instruct,27,1));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL A4predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    imm14 = imm14A4(BitfX(instruct,44,7),BitfX(instruct,31,6),BitfX(instruct,27,1));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL A5predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,42,2);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    imm22 = imm22A5(BitfX(instruct,44,7),BitfX(instruct,37,5),BitfX(instruct,28,9),BitfX(instruct,27,1));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL A6predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    p2 = BitfX(instruct,31,6);
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    p1 = BitfX(instruct,52,6);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL A7predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    p2 = BitfX(instruct,31,6);
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    p1 = BitfX(instruct,52,6);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL A8predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    p2 = BitfX(instruct,31,6);
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    p1 = BitfX(instruct,52,6);
    qp = BitfX(instruct,58,6);
    imm8 = imm8A8(BitfX(instruct,44,7),BitfX(instruct,27,1));
    return YES;
}

BOOL A9predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL A10predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    count2 = count2A10(BitfX(instruct,35,2));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I1predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    count2 = count2I1(BitfX(instruct,32,2));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I2predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I3predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    mbtype4 = mbtype4I3(BitfX(instruct,40,4));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I4predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    mhtype8 = mhtype8I4(BitfX(instruct,36,8));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I5predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I6predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    count5 = count5I6(BitfX(instruct,45,5));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I7predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    r2 = BitfX(instruct,44,7);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I8predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r2 = BitfX(instruct,44,7);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    count5 = count5I8(BitfX(instruct,39,5));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I9predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I10predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r2 = BitfX(instruct,44,7);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    count6 = count6I10(BitfX(instruct,31,6));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I11predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    len6 = len6I11(BitfX(instruct,44,6),BitfX(instruct,31,6));
    pos6 = pos6I11(BitfX(instruct,44,6),BitfX(instruct,31,6));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I12predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r2 = BitfX(instruct,44,7);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    len6 = len6I12(BitfX(instruct,38,6),BitfX(instruct,31,6));
    pos6 = pos6I12(BitfX(instruct,38,6),BitfX(instruct,31,6));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I13predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    len6 = len6I13(BitfX(instruct,44,7),BitfX(instruct,38,6),BitfX(instruct,31,6),BitfX(instruct,27,1));
    pos6 = pos6I13(BitfX(instruct,44,7),BitfX(instruct,38,6),BitfX(instruct,31,6),BitfX(instruct,27,1));
    imm8 = imm8I13(BitfX(instruct,44,7),BitfX(instruct,38,6),BitfX(instruct,31,6),BitfX(instruct,27,1));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I14predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    len6 = len6I14(BitfX(instruct,44,6),BitfX(instruct,31,6),BitfX(instruct,27,1));
    pos6 = pos6I14(BitfX(instruct,44,6),BitfX(instruct,31,6),BitfX(instruct,27,1));
    imm1 = imm1I14(BitfX(instruct,44,6),BitfX(instruct,31,6),BitfX(instruct,27,1));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I15predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    r2 = BitfX(instruct,44,7);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    len4 = len4I15(BitfX(instruct,33,4),BitfX(instruct,27,6));
    pos6 = pos6I15(BitfX(instruct,33,4),BitfX(instruct,27,6));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I16predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    p2 = BitfX(instruct,31,6);
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    p1 = BitfX(instruct,52,6);
    qp = BitfX(instruct,58,6);
    pos6 = pos6I16(BitfX(instruct,44,6));
    return YES;
}

BOOL I17predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    p2 = BitfX(instruct,31,6);
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    p1 = BitfX(instruct,52,6);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL I18predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    qp = BitfX(instruct,58,6);
    imm21 = imm21I18(BitfX(instruct,38,20),BitfX(instruct,27,1));
    return YES;
}

BOOL I19predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    qp = BitfX(instruct,58,6);
    imm21 = imm21I19(BitfX(instruct,38,20),BitfX(instruct,27,1));
    return YES;
}

BOOL I20predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    qp = BitfX(instruct,58,6);
    ippg = PG(IP);
    target25 = target25I20(BitfX(instruct,51,7),BitfX(instruct,31,13),BitfX(instruct,27,1));
    info->samepg = ippg == PG(target25);
    return YES;
}

BOOL I21predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    b1 = BitfX(instruct,55,3);
    qp = BitfX(instruct,58,6);
    tag13 = tag13I21(BitfX(instruct,31,9));
    return YES;
}

BOOL I22predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    b2 = BitfX(instruct,48,3);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I23predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    qp = BitfX(instruct,58,6);
    mask17 = mask17I23(BitfX(instruct,51,7),BitfX(instruct,32,8),BitfX(instruct,27,1));
    return YES;
}

BOOL I24predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    qp = BitfX(instruct,58,6);
    imm44 = imm44I24(BitfX(instruct,31,27),BitfX(instruct,27,1));
    return YES;
}

BOOL I25predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    imm64 = IP;
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I26predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    ar3 = BitfX(instruct,37,7);
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL I27predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    ar3 = BitfX(instruct,37,7);
    qp = BitfX(instruct,58,6);
    imm8 = imm8I27(BitfX(instruct,44,7),BitfX(instruct,27,1));
    return YES;
}

BOOL I28predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    ar3 = BitfX(instruct,37,7);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL I29predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    if (r3 < 32) {
	info->pgrr3 = r3 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL M1predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL M2predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL M3predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    imm9 = imm9M3(BitfX(instruct,44,7),BitfX(instruct,36,1),BitfX(instruct,27,1));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL M4predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M5predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    qp = BitfX(instruct,58,6);
    imm9 = imm9M5(BitfX(instruct,51,7),BitfX(instruct,36,1),BitfX(instruct,27,1));
    return YES;
}

BOOL M6predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    f1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M7predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    f1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M8predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    f1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    imm9 = imm9M8(BitfX(instruct,44,7),BitfX(instruct,36,1),BitfX(instruct,27,1));
    return YES;
}

BOOL M9predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    f2 = BitfX(instruct,44,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M10predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    f2 = BitfX(instruct,44,7);
    qp = BitfX(instruct,58,6);
    imm9 = imm9M10(BitfX(instruct,51,7),BitfX(instruct,36,1),BitfX(instruct,27,1));
    return YES;
}

BOOL M11predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    f2 = BitfX(instruct,44,7);
    f1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M12predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    f2 = BitfX(instruct,44,7);
    f1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M13predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M14predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M15predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    qp = BitfX(instruct,58,6);
    imm9 = imm9M15(BitfX(instruct,44,7),BitfX(instruct,36,1),BitfX(instruct,27,1));
    return YES;
}

BOOL M16predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL M17predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    inc3 = inc3M17(BitfX(instruct,49,2),BitfX(instruct,48,1));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL M18predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    f1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M19predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    f2 = BitfX(instruct,44,7);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL M20predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    qp = BitfX(instruct,58,6);
    ippg = PG(IP);
    target25 = target25M20(BitfX(instruct,51,7),BitfX(instruct,31,13),BitfX(instruct,27,1));
    info->samepg = ippg == PG(target25);
    return YES;
}

BOOL M21predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    f2 = BitfX(instruct,44,7);
    qp = BitfX(instruct,58,6);
    ippg = PG(IP);
    target25 = target25M21(BitfX(instruct,51,7),BitfX(instruct,31,13),BitfX(instruct,27,1));
    info->samepg = ippg == PG(target25);
    return YES;
}

BOOL M22predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    ippg = PG(IP);
    target25 = target25M22(BitfX(instruct,31,20),BitfX(instruct,27,1));
    info->samepg = ippg == PG(target25);
    return YES;
}

BOOL M23predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    f1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    ippg = PG(IP);
    target25 = target25M23(BitfX(instruct,31,20),BitfX(instruct,27,1));
    info->samepg = ippg == PG(target25);
    return YES;
}

BOOL M24predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M25predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    return YES;
}

BOOL M26predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M27predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    f1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M28predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M29predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    ar3 = BitfX(instruct,37,7);
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M30predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    ar3 = BitfX(instruct,37,7);
    qp = BitfX(instruct,58,6);
    imm8 = imm8M30(BitfX(instruct,44,7),BitfX(instruct,27,1));
    return YES;
}

BOOL M31predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    ar3 = BitfX(instruct,37,7);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL M32predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    cr3 = BitfX(instruct,37,7);
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M33predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    cr3 = BitfX(instruct,37,7);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL M34predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    il = ilM34(BitfX(instruct,44,7),BitfX(instruct,37,7),BitfX(instruct,33,4));
    o = oM34(BitfX(instruct,44,7),BitfX(instruct,37,7),BitfX(instruct,33,4));
    r = rM34(BitfX(instruct,44,7),BitfX(instruct,37,7),BitfX(instruct,33,4));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL M35predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M36predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL M37predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    qp = BitfX(instruct,58,6);
    imm21 = imm21M37(BitfX(instruct,38,20),BitfX(instruct,27,1));
    return YES;
}

BOOL M38predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL M39predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    imm2 = imm2M39(BitfX(instruct,49,2));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL M40predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    qp = BitfX(instruct,58,6);
    imm2 = imm2M40(BitfX(instruct,49,2));
    return YES;
}

BOOL M41predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M42predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M43predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL M44predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    qp = BitfX(instruct,58,6);
    imm24 = imm24M44(BitfX(instruct,37,21),BitfX(instruct,31,2),BitfX(instruct,27,1));
    return YES;
}

BOOL M45predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r2 = BitfX(instruct,44,7);
    if (r2 < 32) {
	info->pgrr2 = r2 + 1;
    }
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M46predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL M47predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r3 = BitfX(instruct,37,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL M48predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    qp = BitfX(instruct,58,6);
    imm21 = imm21M48(BitfX(instruct,38,20),BitfX(instruct,27,1));
    return YES;
}

BOOL B1predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    qp = BitfX(instruct,58,6);
    ippg = PG(IP);
    target25 = target25B1(BitfX(instruct,31,20),BitfX(instruct,27,1));
    info->samepg = ippg == PG(target25);
    return YES;
}

BOOL B2predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    ippg = PG(IP);
    target25 = target25B2(BitfX(instruct,31,20),BitfX(instruct,27,1));
    info->samepg = ippg == PG(target25);
    return YES;
}

BOOL B3predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    b1 = BitfX(instruct,55,3);
    qp = BitfX(instruct,58,6);
    ippg = PG(IP);
    target25 = target25B3(BitfX(instruct,31,20),BitfX(instruct,27,1));
    info->samepg = ippg == PG(target25);
    return YES;
}

BOOL B4predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    b2 = BitfX(instruct,48,3);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL B5predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    b2 = BitfX(instruct,48,3);
    b1 = BitfX(instruct,55,3);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL B6predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    ippg = PG(IP);
    target25 = target25B6(BitfX(instruct,51,7),BitfX(instruct,31,20),BitfX(instruct,29,2),BitfX(instruct,27,1));
    info->samepg = ippg == PG(target25);
    tag13 = tag13B6(BitfX(instruct,51,7),BitfX(instruct,31,20),BitfX(instruct,29,2),BitfX(instruct,27,1));
    return YES;
}

BOOL B7predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    b2 = BitfX(instruct,48,3);
    tag13 = tag13B7(BitfX(instruct,51,7),BitfX(instruct,29,2));
    return YES;
}

BOOL B8predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    return YES;
}

BOOL B9predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    qp = BitfX(instruct,58,6);
    imm21 = imm21B9(BitfX(instruct,38,20),BitfX(instruct,27,1));
    return YES;
}

BOOL F1predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    f4 = BitfX(instruct,30,7);
    f3 = BitfX(instruct,37,7);
    f2 = BitfX(instruct,44,7);
    f1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL F2predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    f4 = BitfX(instruct,30,7);
    f3 = BitfX(instruct,37,7);
    f2 = BitfX(instruct,44,7);
    f1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL F3predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    f4 = BitfX(instruct,30,7);
    f3 = BitfX(instruct,37,7);
    f2 = BitfX(instruct,44,7);
    f1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL F4predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    p2 = BitfX(instruct,31,6);
    f3 = BitfX(instruct,37,7);
    f2 = BitfX(instruct,44,7);
    p1 = BitfX(instruct,52,6);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL F5predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    p2 = BitfX(instruct,31,6);
    f2 = BitfX(instruct,44,7);
    p1 = BitfX(instruct,52,6);
    qp = BitfX(instruct,58,6);
    fclass9 = fclass9F5(BitfX(instruct,37,7),BitfX(instruct,29,2));
    return YES;
}

BOOL F6predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    p2 = BitfX(instruct,31,6);
    f3 = BitfX(instruct,37,7);
    f2 = BitfX(instruct,44,7);
    f1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL F7predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    p2 = BitfX(instruct,31,6);
    f3 = BitfX(instruct,37,7);
    f1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL F8predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    f3 = BitfX(instruct,37,7);
    f2 = BitfX(instruct,44,7);
    f1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL F9predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    f3 = BitfX(instruct,37,7);
    f2 = BitfX(instruct,44,7);
    f1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL F10predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    f2 = BitfX(instruct,44,7);
    f1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL F11predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    f2 = BitfX(instruct,44,7);
    f1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL F12predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    qp = BitfX(instruct,58,6);
    amask7 = amask7F12(BitfX(instruct,44,7),BitfX(instruct,37,7));
    omask7 = omask7F12(BitfX(instruct,44,7),BitfX(instruct,37,7));
    return YES;
}

BOOL F13predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    qp = BitfX(instruct,58,6);
    return YES;
}

BOOL F14predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    qp = BitfX(instruct,58,6);
    ippg = PG(IP);
    target25 = target25F14(BitfX(instruct,38,20),BitfX(instruct,27,1));
    info->samepg = ippg == PG(target25);
    return YES;
}

BOOL F15predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    qp = BitfX(instruct,58,6);
    imm21 = imm21F15(BitfX(instruct,38,20),BitfX(instruct,27,1));
    return YES;
}

BOOL F16predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    qp = BitfX(instruct,58,6);
    imm21 = imm21F16(BitfX(instruct,38,20),BitfX(instruct,27,1));
    return YES;
}

BOOL X1predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    qp = BitfX(instruct,58,6);
    imm62 = imm62X1(imm41,BitfX(instruct,38,20),BitfX(instruct,27,1));
    return YES;
}

BOOL X2predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    r1 = BitfX(instruct,51,7);
    qp = BitfX(instruct,58,6);
    imm64 = imm64X2(imm41,BitfX(instruct,44,7),BitfX(instruct,42,1),BitfX(instruct,37,5),BitfX(instruct,28,9),BitfX(instruct,27,1));
    if (!r1)
	return NO;
    if (r1 < 32) {
	info->pgrtgt = r1 + 1;
    }
    return YES;
}

BOOL X3predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    qp = BitfX(instruct,58,6);
    ippg = PG(IP);
    target64 = target64X3(BitfX(imm41,23,39),BitfX(instruct,31,20),BitfX(instruct,27,1));
    info->samepg = ippg == PG(target64);
    return YES;
}

BOOL X4predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    b1 = BitfX(instruct,55,3);
    qp = BitfX(instruct,58,6);
    ippg = PG(IP);
    target64 = target64X4(BitfX(imm41,23,39),BitfX(instruct,31,20),BitfX(instruct,27,1));
    info->samepg = ippg == PG(target64);
    return YES;
}

BOOL X5predecode(DWORD instruct, INSTINFO *info)
{
    qp = 0;
    info->samepg = 0;
    info->pgrtgt = 0;
    info->pgrr2 = info->pgrr3 = 0;
    qp = BitfX(instruct,58,6);
    imm62 = imm62X5(imm41,BitfX(instruct,38,20),BitfX(instruct,27,1));
    return YES;
}
