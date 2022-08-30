/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator IA Execution Functions
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


#include "std.h"
#include "bits.h"
#include "dos.h"
#include "types.h"
#include "fields.h"
#include "sign_ext.h"
#include "state.h"
#include "sim.h"
#include "libcore.h"
#include "simmem.h"
#include "libsym.h"
#include "interruption.h"
#include "ia_types.h"
#include "ia_state.h"
#include "ia_read.h"
#include "ia_write.h"
#include "ia_exec.h"
#include "ssDBT.h"

/* IA-32 Trap Code for INT and INTO instructions (from Seemless Spec) */
#define INT_TRAP_CODE		(PSR_CPL << 15 | SSD.d << 14 | PSR_SS << 3)
	/* XXX - need to include real/protected/VM86 in bits 11, 12, & 13 */

/* Internal IA Simulator State - not mapped onto EM State */

BYTE iAmode;	/* IA execution mode - 32- or 16-bit operands/addresses */
BOOL dosABI = NO;	/* emulate DOS ABI when YES */
IADWORD EIP;	/* IA instruction pointer */

void setIAmode(void)
{
    iAmode = 0;

    if (!EFLAGS.vm && CSD.d)
	iAmode |= OpAddrSizeMask;
    if (!EFLAGS.vm && SSD.d)
	iAmode |= StackAddrSizeMask;
    /* XXX - need to include real/protected/VM86 in iAmode */
}

BYTE getIAmode(void)
{
    return iAmode;
}

/* Even-parity lookup table for generating EFLAGS.pf from the low
 * byte of the result of an instruction.
 */
static const BYTE parity[256] = {
/*  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  */
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, /* 00 */
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, /* 10 */
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, /* 20 */
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, /* 30 */
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, /* 40 */
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, /* 50 */
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, /* 60 */
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, /* 70 */
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, /* 80 */
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, /* 90 */
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, /* A0 */
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, /* B0 */
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, /* C0 */
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, /* D0 */
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, /* E0 */
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1  /* F0 */
};

ArithFlags arithFlags;

#define overflow8(src1, src2, res)	(((src1) & 0x80) == ((src2) & 0x80) &&\
					 ((src1) & 0x80) != ((res) & 0x80))

#define overflow16(src1, src2, res)	(((src1) & 0x8000) == \
					 ((src2) & 0x8000) && \
					 ((src1) & 0x8000) != \
					 ((res) & 0x8000))

#define overflow32(src1, src2, res)	(((src1) & 0x80000000) == \
					 ((src2) & 0x80000000) && \
					 ((src1) & 0x80000000) != \
					 ((res) & 0x80000000))

#define sign8(res)			(((res) & 0x80) != 0)
#define sign16(res)			(((res) & 0x8000) != 0)
#define sign32(res)			(((res) & 0x80000000) != 0)

#define carry8(src, res, cf)		(((res) < ((src) & 0xff)) || \
					 (cf && (res) == ((src) & 0xff)))
#define carry16(src, res, cf)		(((res) < ((src) & 0xffff)) || \
					 (cf && (res) == ((src) & 0xffff)))
#define carry32(src, res, cf)		(((res) < (src)) || \
					 (cf && (res) == (src)))

#define borrow8(src, res, cf)		(((res) > ((src) & 0xff)) || \
					 (cf && (res) == ((src) & 0xff)))
#define borrow16(src, res, cf)		(((res) > ((src) & 0xffff)) || \
					 (cf && (res) == ((src) & 0xffff)))
#define borrow32(src, res, cf)		(((res) > (src)) || \
					 (cf && (res) == (src)))

#define auxCarry(src1, src2, res)	(((((src1) ^ (src2)) ^ (res)) & \
					 0x10) != 0)

#define bit0(res)			((res) & 1)
#define bit6(res)			(((res) >> 6) & 1)
#define bit8(res)			(((res) >> 8) & 1)
#define bit14(res)			(((res) >> 14) & 1)
#define bit16(res)			(((res) >> 16) & 1)
#define bit30(res)			(((res) >> 30) & 1)
#define bit31(res)			(((res) >> 31) & 1)
#define bit32(res)			(((res) >> 32) & 1)


/* Utility Functions - these exist so their callers don't need to know about
 * IA data types and structures
 */

static void eflagsFromArithFlags(void)
{
    EFLAGS.le = (arithFlags.sf ^ arithFlags.of) | arithFlags.zf;
    EFLAGS.be = arithFlags.cf | arithFlags.zf;
    EFLAGS.lt = arithFlags.sf ^ arithFlags.of;
    EFLAGS.of = arithFlags.of;
    EFLAGS.sf = arithFlags.sf;
    EFLAGS.zf = arithFlags.zf;
    EFLAGS.af = arithFlags.af;
    EFLAGS.pf = arithFlags.pf;
    EFLAGS.cf = arithFlags.cf;
}

void arithFlagsFromEflags(void)
{
    arithFlags.of = EFLAGS.of;
    arithFlags.sf = EFLAGS.sf;
    arithFlags.zf = EFLAGS.zf;
    arithFlags.af = EFLAGS.af;
    arithFlags.pf = EFLAGS.pf;
    arithFlags.cf = EFLAGS.cf;
}

void setEIPfromIP(void)
{
    EIP = (ADDR4)ip - CSD.base;
}

void setSegGRsFromARs(void)
{
    CSD = CSD_AR;
    SSD = SSD_AR;
}

static void setSegARsFromGRs(void)
{
    CSD_AR = CSD;
    SSD_AR = SSD;
}


/* IA Execution Functions */

/* These are in alphabetical order with one function corresponding to each
 * "instruction page" in the Pentium Pro Programmer's Reference Manual.
 * The functions use the names in the manual, in lower-case, with "IAEx"
 * appended.
 */

Status aaaIAEx(IAinstInfoPtr info)
{
    /* aaa modifies CF and AF; SF, ZF, PF, and OF are undefined */
    if (((AL & 0xf) > 9) || arithFlags.af) {
	AL += 6;
	AH++;
	arithFlags.af = arithFlags.cf = 1;
    } else
	arithFlags.af = arithFlags.cf = 0;
    AL &= 0xf;
    return StSuccess;
}

Status aadIAEx(IAinstInfoPtr info)
{
    /* aad modifies SF, ZF, and PF; OF, AF, and CF are undefined */
    AL = (AH * info->imm32 + AL) & 0xff;	/* in general imm32 == 10 */
    AH = 0;
    arithFlags.sf = sign8(AL);
    arithFlags.zf = (AL == 0);
    arithFlags.pf = parity[AL];
    return StSuccess;
}

Status aamIAEx(IAinstInfoPtr info)
{
    /* aad modifies SF, ZF, and PF; OF, AF, and CF are undefined */
    AH = AL / info->imm32;			/* in general imm32 == 10 */
    AL = AL % info->imm32;
    arithFlags.sf = sign8(AL);
    arithFlags.zf = (AL == 0);
    arithFlags.pf = parity[AL];
    return StSuccess;
}

Status aasIAEx(IAinstInfoPtr info)
{
    /* aas modifies CF and AF; SF, ZF, PF, and OF are undefined */
    if ((AL & 0xf) > 9 || arithFlags.af) {
	AL -= 6;
	AH--;
	arithFlags.af = arithFlags.cf = 1;
    } else
	arithFlags.af = arithFlags.cf = 0;
    AL &= 0xf;
    return StSuccess;
}

Status adcIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* add modifies OF, SF, ZF, AF, CF, PF */
    switch (info->opSize) {
    case 1:
	res = (dest + src + arithFlags.cf) & 0xff;
	arithFlags.of = overflow8(src, dest, res);
	arithFlags.sf = sign8(res);
	arithFlags.cf = carry8(src, res, arithFlags.cf);
	break;
    case 2:
	res = (dest + src + arithFlags.cf) & 0xffff;
	arithFlags.of = overflow16(src, dest, res);
	arithFlags.sf = sign16(res);
	arithFlags.cf = carry16(src, res, arithFlags.cf);
	break;
    case 4:
	res = dest + src + arithFlags.cf;
	arithFlags.of = overflow32(src, dest, res);
	arithFlags.sf = sign32(res);
	arithFlags.cf = carry32(src, res, arithFlags.cf);
	break;
    }
    arithFlags.zf = (res == 0);
    arithFlags.af = auxCarry(src, dest, res);
    arithFlags.pf = parity[res & 0xff];
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

Status addIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* add modifies OF, SF, ZF, AF, CF, PF */
    switch (info->opSize) {
    case 1:
	res = (dest + src) & 0xff;
	arithFlags.of = overflow8(src, dest, res);
	arithFlags.sf = sign8(res);
	arithFlags.cf = carry8(src, res, 0);
	break;
    case 2:
	res = (dest + src) & 0xffff;
	arithFlags.of = overflow16(src, dest, res);
	arithFlags.sf = sign16(res);
	arithFlags.cf = carry16(src, res, 0);
	break;
    case 4:
	res = dest + src;
	arithFlags.of = overflow32(src, dest, res);
	arithFlags.sf = sign32(res);
	arithFlags.cf = carry32(src, res, 0);
	break;
    }
    arithFlags.zf = (res == 0);
    arithFlags.af = auxCarry(src, dest, res);
    arithFlags.pf = parity[res & 0xff];
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

Status andIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* and clears OF & CF; modifies SF, ZF, PF; AF is undefined */
    switch (info->opSize) {
    case 1:
	res = (dest & src) & 0xff;
	arithFlags.sf = sign8(res);
	break;
    case 2:
	res = (dest & src) & 0xffff;
	arithFlags.sf = sign16(res);
	break;
    case 4:
	res = dest & src;
	arithFlags.sf = sign32(res);
	break;
    }
    arithFlags.of = arithFlags.cf = 0;
    arithFlags.zf = (res == 0);
    arithFlags.pf = parity[res & 0xff];
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

Status arplIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status boundIAEx(IAinstInfoPtr info)
{
    IADWORD src[2], dest;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LD_ACCESS, &dest, &adr, &adr2))
	return StFault;

    if (dest < src[0] || dest > (src[1] + info->opSize)) {
	ia32ExceptionFault(IA32_BOUNDS_EXCEPTION, 0);
	return StFault;
    }
    return StSuccess;
}

Status bsfIAEx(IAinstInfoPtr info)
{
    IADWORD src, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2))
	return StFault;

    /* bsf modifies ZF; CF, SF, OF, PF, and AF are undefined */
    if (info->opSize == IA16)
	src &= 0xffff;
    if (!src)
	arithFlags.zf = 1;
    else {
	arithFlags.zf = 0;
	res = 0;
	while (!(src & 0x1)) {
	    res++;
	    src >>= 1;
	}
    }
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

Status bsrIAEx(IAinstInfoPtr info)
{
    IADWORD src, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2))
	return StFault;

    /* bsr modifies ZF; CF, SF, OF, PF, and AF are undefined */
    if (info->opSize == IA16)
	src = (src & 0xffff) << 16;
    if (!src)
	arithFlags.zf = 1;
    else {
	arithFlags.zf = 0;
	res = (info->opSize == IA16) ? 15 : 31;
	while (!sign32(src)) {
	    res--;
	    src <<= 1;
	}
    }
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;

}

Status bswapIAEx(IAinstInfoPtr info)
{
    IADWORD dest, res;
    ADDR adr, adr2;

    if (!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    res = ((dest & 0xff000000) >> 24) | ((dest & 0x00ff0000) >> 8) |
	((dest & 0x0000ff00) << 8) | ((dest & 0x000000ff) << 24);
    info->destWrFn(info, res, adr, adr2);
    /* bswap modifies no flags */
    return StSuccess;
}

Status btIAEx(IAinstInfoPtr info)
{
    IADWORD base, offset;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &offset, &adr, &adr2))
	return StFault;

    /* bt modifies CF; ZF, SF, OF, PF, and AF are undefined */
    if (info->modrm.mod != 3 && info->srcRdFn != immIARd) {
	/* offset range is -2^31 to 2^31-1 if offset is a reg and base in mem */
	if (info->opSize == IA32)
	    info->disp32 += 4 * sign_ext32(offset >> 5, 27);
	else
	    info->disp32 += 2 * sign_ext32(offset >> 4, 12);
    }
    if (!info->destRdFn(info, MEM_LDST_ACCESS, &base, &adr, &adr2))
	return StFault;

    offset &= (info->opSize == IA16) ? 0xf : 0x1f;
    arithFlags.cf = (base >> offset) & 0x1;
    return StSuccess;
}

Status btcIAEx(IAinstInfoPtr info)
{
    IADWORD base, offset, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &offset, &adr, &adr2))
	return StFault;

    /* btc modifies CF; ZF, SF, OF, PF, and AF are undefined */
    if (info->modrm.mod != 3 && info->srcRdFn != immIARd) {
	/* offset range is -2^31 to 2^31-1 if offset is a reg and base in mem */
	if (info->opSize == IA32)
	    info->disp32 += 4 * sign_ext32(offset >> 5, 27);
	else
	    info->disp32 += 2 * sign_ext32(offset >> 4, 12);
    }
    if (!info->destRdFn(info, MEM_LDST_ACCESS, &base, &adr, &adr2))
	return StFault;

    offset &= (info->opSize == IA16) ? 0xf : 0x1f;
    arithFlags.cf = (base >> offset) & 0x1;
    res = base ^ (1 << offset);
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

Status btrIAEx(IAinstInfoPtr info)
{
    IADWORD base, offset, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &offset, &adr, &adr2))
	return StFault;

    /* btr modifies CF; ZF, SF, OF, PF, and AF are undefined */
    if (info->modrm.mod != 3 && info->srcRdFn != immIARd) {
	/* offset range is -2^31 to 2^31-1 if offset is a reg and base in mem */
	if (info->opSize == IA32)
	    info->disp32 += 4 * sign_ext32(offset >> 5, 27);
	else
	    info->disp32 += 2 * sign_ext32(offset >> 4, 12);
    }
    if (!info->destRdFn(info, MEM_LDST_ACCESS, &base, &adr, &adr2))
	return StFault;

    offset &= (info->opSize == IA16) ? 0xf : 0x1f;
    arithFlags.cf = (base >> offset) & 0x1;
    res = base & ~(1 << offset);
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

Status btsIAEx(IAinstInfoPtr info)
{
    IADWORD base, offset, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &offset, &adr, &adr2))
	return StFault;

    /* bts modifies CF; ZF, SF, OF, PF, and AF are undefined */
    if (info->modrm.mod != 3 && info->srcRdFn != immIARd) {
	/* offset range is -2^31 to 2^31-1 if offset is a reg and base in mem */
	if (info->opSize == IA32)
	    info->disp32 += 4 * sign_ext32(offset >> 5, 27);
	else
	    info->disp32 += 2 * sign_ext32(offset >> 4, 12);
    }
    if (!info->destRdFn(info, MEM_LDST_ACCESS, &base, &adr, &adr2))
	return StFault;

    offset &= (info->opSize == IA16) ? 0xf : 0x1f;
    arithFlags.cf = (base >> offset) & 0x1;
    res = base | (1 << offset);
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

/* call has been split into 4 functions */

Status call_near_relIAEx(IAinstInfoPtr info)
{
    IADWORD dest, retEIP;
    ADDR adr, adr2;

    if (!info->destRdFn(info, MEM_ST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    if (info->opSize == IA32) {
	retEIP = EIP + info->delta;
	EIP += info->delta + info->disp32;
    } else {		/* IA16 */
	retEIP = (EIP + info->delta) & 0xffff;
	EIP = (EIP + info->delta + info->disp32) & 0xffff;
    }
    /* XXX - add code segment limit check */
    ip = IPfromEIP(EIP);
    info->destWrFn(info, retEIP, adr, adr2);
    return StTakenBr;
}

Status call_near_indIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest, retEIP;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_ST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    if (info->opSize == IA32)
	retEIP = EIP + info->delta;
    else		/* IA16 */
	retEIP = (EIP + info->delta) & 0xffff;
    EIP = src;
    /* XXX - add code segment limit check */
    ip = IPfromEIP(EIP);
    info->destWrFn(info, retEIP, adr, adr2);
    return StTakenBr;
}

Status call_far_absIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest, retEIP, retCS;
    ADDR adr, adr2, segAdr, segAdr2, ofsAdr, ofsAdr2;

    if (!segRegUpdateIARd(info, MEM_ST_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_ST_ACCESS, &dest, &segAdr, &segAdr2) ||
	!call_farIARd(info, MEM_ST_ACCESS, &dest, &ofsAdr, &ofsAdr2))
	return StFault;

    if (info->opSize == IA32)
	retEIP = EIP + info->delta;
    else		/* IA16 */
	retEIP = (EIP + info->delta) & 0xffff;
    retCS = CS;
    /* XXX - add code segment limit check */
    EIP = info->disp32;
    segRegIAWr(info, info->imm32, adr, adr2);
    ip = IPfromEIP(EIP);
    info->destWrFn(info, retCS,  segAdr, segAdr2);
    info->destWrFn(info, retEIP, ofsAdr, ofsAdr2);
    return StTakenBr;
}

Status call_far_indIAEx(IAinstInfoPtr info)
{
    IADWORD src[2], dest, retEIP, retCS, tgtEIP, tgtCS;
    ADDR adr, adr2, segAdr, segAdr2, ofsAdr, ofsAdr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, src, &adr, &adr2) ||
	!segRegUpdateIARd(info, MEM_ST_ACCESS, &dest, &adr, &adr2) ||
	!info->destRdFn(info, MEM_ST_ACCESS, &dest, &segAdr, &segAdr2) ||
	!call_farIARd(info, MEM_ST_ACCESS, &dest, &ofsAdr, &ofsAdr2))
	return StFault;

    if (info->opSize == IA32)
	retEIP = EIP + info->delta;
    else		/* IA16 */
	retEIP = (EIP + info->delta) & 0xffff;
    retCS = CS;
    /* XXX - add code segment limit check */
    if (info->opSize == IA16) {
	tgtEIP = src[0] & 0xffff;
	tgtCS  = src[0] >> 16;
    } else {		/* IA32 */
	tgtEIP = src[0];
	tgtCS  = src[1];
    }
    EIP = tgtEIP;
    segRegIAWr(info, tgtCS, adr, adr2);
    ip = IPfromEIP(EIP);
    info->destWrFn(info, retCS,  segAdr, segAdr2);
    info->destWrFn(info, retEIP, ofsAdr, ofsAdr2);
    return StTakenBr;
}

Status cbwIAEx(IAinstInfoPtr info)
{
    IADWORD src, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2))
	return StFault;

    if (info->opSize == IA16)
	res = (IASDWORD)(IASBYTE)src;
    else		/* IA32 */
	res = (IASDWORD)(IASWORD)src;
    info->destWrFn(info, res, adr, adr2);
    /* cbw modifies no flags */
    return StSuccess;
}

/* ARGSUSED */
Status clcIAEx(IAinstInfoPtr info)
{
    /* current CF is in arithFlags, not EFLAGS */
    arithFlags.cf = 0;
    return StSuccess;
}

/* ARGSUSED */
Status cldIAEx(IAinstInfoPtr info)
{
    EFLAGS.df = 0;
    return StSuccess;
}

/* ARGSUSED */
Status cliIAEx(IAinstInfoPtr info)
{
    /* XXX - add protected mode & VM86 stuff */
    /* XXX - add intercept stuff according to Seamless 2.0 */
    EFLAGS.if_ = 0;
    return StSuccess;
}

Status cltsIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

/* ARGSUSED */
Status cmcIAEx(IAinstInfoPtr info)
{
    /* current CF is in arithFlags, not EFLAGS */
    arithFlags.cf = !arithFlags.cf;
    return StSuccess;
}

Status cmovccIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status cmpIAEx(IAinstInfoPtr info)
{
    IADWORD src2, src1, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src2, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LD_ACCESS, &src1, &adr, &adr2))
	return StFault;

    /* cmp modifies OF, SF, ZF, AF, CF, PF */
    switch (info->opSize) {
    case 1:
	res = (src1 - src2) & 0xff;
	arithFlags.of = overflow8(src1, -src2, res);
	arithFlags.sf = sign8(res);
	arithFlags.cf = borrow8(src1, res, 0);
	break;
    case 2:
	res = (src1 - src2) & 0xffff;
	arithFlags.of = overflow16(src1, -src2, res);
	arithFlags.sf = sign16(res);
	arithFlags.cf = borrow16(src1, res, 0);
	break;
    case 4:
	res = src1 - src2;
	arithFlags.of = overflow32(src1, -src2, res);
	arithFlags.sf = sign32(res);
	arithFlags.cf = borrow32(src1, res, 0);
	break;
    }
    arithFlags.zf = (res == 0);
    arithFlags.af = auxCarry(src2, src1, res);
    arithFlags.pf = parity[res & 0xff];
    return StSuccess;
}

Status cmpsIAEx(IAinstInfoPtr info)
{
    IADWORD inc;
    Status st;

    if ((st = cmpIAEx(info)) == StFault)
	return StFault;

    inc = (1 - (2 * EFLAGS.df)) * info->opSize;
    if (info->addrSize == IA16) {
	SI += inc;
	DI += inc;
    } else {		/* IA32 */
	ESI += inc;
	EDI += inc;
    }
    return st;
}

Status cmpxchgIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest, accum, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* cmpxchg modifies OF, SF, ZF, AF, CF, PF */
    switch (info->opSize) {
    case 1:
	accum = AL;
	res = (dest - accum) & 0xff;
	arithFlags.of = overflow8(dest, -accum, res);
	arithFlags.sf = sign8(res);
	arithFlags.cf = borrow8(dest, res, 0);
	AL = dest;
	break;
    case 2:
	accum = AX;
	res = (dest - accum) & 0xffff;
	arithFlags.of = overflow16(dest, -accum, res);
	arithFlags.sf = sign16(res);
	arithFlags.cf = borrow16(dest, res, 0);
	AX = dest;
	break;
    case 4:
	accum = EAX;
	res = dest - accum;
	arithFlags.of = overflow32(dest, -accum, res);
	arithFlags.sf = sign32(res);
	arithFlags.cf = borrow32(dest, res, 0);
	EAX = dest;
	break;
    }
    arithFlags.af = auxCarry(accum, dest, res);
    arithFlags.pf = parity[res & 0xff];
    if ((arithFlags.zf = (res == 0)))	/* accum == dest */
	info->destWrFn(info, src, adr, adr2);
    return StSuccess;
}

Status cmpxchg8bIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status cpuidIAEx(IAinstInfoPtr info)
{
    /* XXX - fill in real info when known */
    /* XXX - can this state be shared with EM cpuid registers? */
    switch ((IADWORD)EAX) {
    case 0:
	EAX = 2;
	EBX = 0x756e6547;	/* "Genu" */
	EDX = 0x49656e69;	/* "ineI" */
	ECX = 0x6c65746e;	/* "ntel" */
	break;
    case 1:
	EAX = 0x00000710;
	EBX = 0;
	ECX = 0;
	EDX = 0x00008137;
	break;
    case 2:
	EAX = 0x03020101;
	EBX = 0;
	ECX = 0;
	EDX = 0x06040a42;
	break;
    }
    return StSuccess;
}

Status cwdIAEx(IAinstInfoPtr info)
{
    IADWORD src, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2))
	return StFault;

    if (info->opSize == IA16)
	res = ((IADWORD)(IASDWORD)(IASWORD)src) >> 16;
    else		/* IA32 */
	res = (IADWORD)(((IAQWORD)(IASQWORD)(IASDWORD)src) >> 32);
    info->destWrFn(info, res, adr, adr2);
    /* cwd modifies no flags */
    return StSuccess;
}

Status daaIAEx(IAinstInfoPtr info)
{
    BYTE res = AL;

    /* daa modifies CF, AF, SF, ZF, PF; OF is undefined */
    if (((res & 0xf) > 9) || arithFlags.af) {
	res += 6;
	arithFlags.cf |= carry8(AL, res, 0);
	arithFlags.af = 1;
    } else
	arithFlags.af = 0;

    if ((res > 0x90) || arithFlags.cf) {
	res += 0x60;
	arithFlags.cf = 1;
    } else
	arithFlags.cf = 0;

    arithFlags.sf = sign8(res);
    arithFlags.zf = (res == 0);
    arithFlags.pf = parity[res];
    AL = res;
    return StSuccess;
}

Status dasIAEx(IAinstInfoPtr info)
{
    BYTE res = AL;

    /* das modifies CF, AF, SF, ZF, PF; OF is undefined */
    if (((res & 0xf) > 9) || arithFlags.af) {
	res -= 6;
	arithFlags.cf |= borrow8(AL, res, 0);
	arithFlags.af = 1;
    } else
	arithFlags.af = 0;

    if ((res > 0x9f) || arithFlags.cf) {
	res -= 0x60;
	arithFlags.cf = 1;
    } else
	arithFlags.cf = 0;

    arithFlags.sf = sign8(res);
    arithFlags.zf = (res == 0);
    arithFlags.pf = parity[res];
    AL = res;
    return StSuccess;
}

Status decIAEx(IAinstInfoPtr info)
{
    IADWORD dest, res;
    ADDR adr, adr2;

    if (!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* dec modifies OF, SF, ZF, AF, PF; CF is unchanged */
    switch (info->opSize) {
    case 1:
	res = (dest - 1) & 0xff;
	arithFlags.of = overflow8(dest, -1, res);
	arithFlags.sf = sign8(res);
	break;
    case 2:
	res = (dest - 1) & 0xffff;
	arithFlags.of = overflow16(dest, -1, res);
	arithFlags.sf = sign16(res);
	break;
    case 4:
	res = dest - 1;
	arithFlags.of = overflow32(dest, -1, res);
	arithFlags.sf = sign32(res);
	break;
    }
    arithFlags.zf = (res == 0);
    arithFlags.af = auxCarry(1, dest, res);
    arithFlags.pf = parity[res & 0xff];
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

Status divIAEx(IAinstInfoPtr info)
{
    IADWORD eAX, divisor, res;
    IAQWORD dividend;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &divisor, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &eAX, &adr, &adr2))
	return StFault;

    if (divisor == 0) {
	ia32ExceptionFault(IA32_DIVIDE_EXCEPTION, 0);
	return StFault;
    }
    /* div makes OF, SF, ZF, AF, CF, PF undefined */
    switch (info->opSize) {
    case 1:
	/* AX / rm8; AL = quotient, AH = remainder */
	if ((res = eAX / divisor) > 0xff) {
	    ia32ExceptionFault(IA32_DIVIDE_EXCEPTION, 0);
	    return StFault;
	}
	res |= (eAX % divisor) << 8;
	break;
    case 2:
	/* DX:AX / rm16; AX = quotient, DX = remainder */
	dividend = (DX << 16) | eAX;
	if ((res = dividend / divisor) > 0xffff) {
	    ia32ExceptionFault(IA32_DIVIDE_EXCEPTION, 0);
	    return StFault;
	}
	DX = dividend % divisor;
	break;
    case 4:
	/* EDX:EAX / rm32; EAX = quotient, EDX = remainder */
	dividend = ((REG)EDX << 32) | eAX;
	if ((res = dividend / divisor) > 0xffffffff) {
	    ia32ExceptionFault(IA32_DIVIDE_EXCEPTION, 0);
	    return StFault;
	}
	EDX = dividend % divisor;
	break;
    }
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

Status enterIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

/* XXX - add FP ops */

/* ARGSUSED */
Status hltIAEx(IAinstInfoPtr info)
{
    char buf[40];

    symAddrtoName(ip, buf, 4, NO, -32);
    progExit("Stopping at %s -- IA-32 halt instruction\n", buf);
    /*NOTREACHED*/
    return StSuccess;
}

Status idivIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status imulIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status inIAEx(IAinstInfoPtr info)
{
    IADWORD port, res;
    ADDR adr, adr2, portAdr;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &port, &adr, &adr2))
	return StFault;

    /* XXX - add protected-mode & VM86 IOPL/permission bit checks */
    portAdr = IOBASE | ((port & ~3) << 10) | (port & 3);
    if (!memIAadrRd(portAdr, info->opSize, MEM_LD_ACCESS, &adr, &adr2, &res))
	return StFault;
    /* in modifies no flags */
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

Status incIAEx(IAinstInfoPtr info)
{
    IADWORD dest, res;
    ADDR adr, adr2;

    if (!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* inc modifies OF, SF, ZF, AF, PF; CF is unchanged */
    switch (info->opSize) {
    case 1:
	res = (dest + 1) & 0xff;
	arithFlags.of = overflow8(dest, 1, res);
	arithFlags.sf = sign8(res);
	break;
    case 2:
	res = (dest + 1) & 0xffff;
	arithFlags.of = overflow16(dest, 1, res);
	arithFlags.sf = sign16(res);
	break;
    case 4:
	res = dest + 1;
	arithFlags.of = overflow32(dest, 1, res);
	arithFlags.sf = sign32(res);
	break;
    }
    arithFlags.zf = (res == 0);
    arithFlags.af = auxCarry(1, dest, res);
    arithFlags.pf = parity[res & 0xff];
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

Status insIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status intIAEx(IAinstInfoPtr info)
{
    if (info->imm32 == 3)
	if (isbpt(ip) != -1) {
	    static char sym[40];

	    symAddrtoName(ip, sym, 4, NO, -32);
	    progStop("Breakpoint (IA-32) at %s\n", sym);
	    /* counts and commands in breakpoints are not implemented yet. */
	} else {
	    ia32ExceptionTrap(IA32_BREAK_EXCEPTION, INT_TRAP_CODE, info->delta);
	    return StFault;
	}
    else if (dosABI && info->imm32 == 0x21)	/* system function */
	return dosInt21(AH, AL);
    else if (dosABI && info->imm32 == 0x20)	/* terminate program */
	progExit("IA-32 program terminated\n");
    else {					/* generic software interrupt */
	ia32InterruptTrap(info->imm32, INT_TRAP_CODE, info->delta);
	return StFault;
    }
    return StFault;
}

Status intoIAEx(IAinstInfoPtr info)
{
    if (arithFlags.of) {
	ia32ExceptionTrap(IA32_OVERFLOW_EXCEPTION, INT_TRAP_CODE, info->delta);
	return StFault;
    }
    return StSuccess;
}

Status invdIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status invlpgIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status iretIAEx(IAinstInfoPtr info)
{
    IADWORD dest, retEIP, retCS, retEFLAGS;
    DWORD tmpEFLAGS;
    ADDR adr, adr2, ofsAdr, ofsAdr2, segAdr, segAdr2, eflAdr, eflAdr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &retEIP, &ofsAdr, &ofsAdr2) ||
	!segRegUpdateIARd(info, MEM_ST_ACCESS, &dest, &adr, &adr2) ||
	!ret_farIARd(info, MEM_LD_ACCESS, &retCS, &segAdr, &segAdr2) ||
	!iretIARd(info, MEM_LD_ACCESS, &retEFLAGS, &eflAdr, &eflAdr2))
	return StFault;

    if (StackAddrSize(info->mode) == IA32)
	ESP += 3 * info->opSize;
    else			/* IA16 */
	SP += 3 * info->opSize;

    if (info->opSize == IA32) {
	EIP = retEIP;
	tmpEFLAGS = (retEFLAGS & 0x257fd5) | 0x1a0002;
    } else {			/* IA16 */
	EIP = retEIP & 0xffff;
	tmpEFLAGS = (retEFLAGS & 0x7fd5) | 0x0002;
    }
    segRegIAWr(info, retCS, adr, adr2);
    /* XXX - add code segment limit check */
    ip = IPfromEIP(EIP);
    EFLAGS = *(IAeflagsPtr)&tmpEFLAGS;
    arithFlagsFromEflags();
    return StTakenBr;
}

Status jccIAEx(IAinstInfoPtr info)
{
    BOOL cond;

    switch (info->cond_seg) {
    case JO_COND:
	cond = arithFlags.of;
	break;
    case JNO_COND:
	cond = !arithFlags.of;
	break;
    case JB_COND:
	cond = arithFlags.cf;
	break;
    case JNB_COND:
	cond = !arithFlags.cf;
	break;
    case JZ_COND:
	cond = arithFlags.zf;
	break;
    case JNZ_COND:
	cond = !arithFlags.zf;
	break;
    case JBE_COND:
	cond = arithFlags.cf || arithFlags.zf;
	break;
    case JNBE_COND:
	cond = !(arithFlags.cf || arithFlags.zf);
	break;
    case JS_COND:
	cond = arithFlags.sf;
	break;
    case JNS_COND:
	cond = !arithFlags.sf;
	break;
    case JP_COND:
	cond = arithFlags.pf;
	break;
    case JNP_COND:
	cond = !arithFlags.pf;
	break;
    case JL_COND:
	cond = arithFlags.sf != arithFlags.of;
	break;
    case JNL_COND:
	cond = arithFlags.sf == arithFlags.of;
	break;
    case JLE_COND:
	cond = (arithFlags.sf != arithFlags.of) || arithFlags.zf;
	break;
    case JNLE_COND:
	cond = (arithFlags.sf == arithFlags.of) && !arithFlags.zf;
	break;
    case JCXZ_COND:
	if (info->opSize == IA16)
	    cond = CX == 0;
	else	/* IA32 */
	    cond = ECX == 0;
	break;
    }

    /* jcc modifies no flags */
    if (cond) {
	if (info->opSize == IA32)
	    EIP += info->delta + info->disp32;
	else
	    EIP = (EIP + info->delta + info->disp32) & 0xffff;
	/* XXX - add code segment limit check */
	ip = IPfromEIP(EIP);
	return StTakenBr;
    }
    return StSuccess;
}

/* jmp has been split into 4 functions */

Status jmp_near_relIAEx(IAinstInfoPtr info)
{
    if (info->opSize == IA32)
	EIP += info->delta + info->disp32;
    else		/* IA16 */
	EIP = (EIP + info->delta + info->disp32) & 0xffff;
    /* XXX - add code segment limit check */
    ip = IPfromEIP(EIP);
    return StTakenBr;
}

Status jmp_near_indIAEx(IAinstInfoPtr info)
{
    IADWORD src;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2))
	return StFault;

    EIP = src;
    /* XXX - add code segment limit check */
    ip = IPfromEIP(EIP);
    return StTakenBr;
}

Status jmp_far_absIAEx(IAinstInfoPtr info)
{
    IADWORD dest;
    ADDR adr, adr2;

    if (!segRegUpdateIARd(info, MEM_ST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* XXX - add code segment limit check */
    EIP = info->disp32;
    segRegIAWr(info, info->imm32, adr, adr2);
    ip = IPfromEIP(EIP);
    return StTakenBr;
}

Status jmp_far_indIAEx(IAinstInfoPtr info)
{
    IADWORD src[2], dest, tgtEIP, tgtCS;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, src, &adr, &adr2) ||
	!segRegUpdateIARd(info, MEM_ST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* XXX - add code segment limit check */
    if (info->opSize == IA16) {
	tgtEIP = src[0] & 0xffff;
	tgtCS  = src[0] >> 16;
    } else {		/* IA32 */
	tgtEIP = src[0];
	tgtCS  = src[1];
    }
    EIP = tgtEIP;
    segRegIAWr(info, tgtCS, adr, adr2);
    ip = IPfromEIP(EIP);
    return StTakenBr;
}

/* jmpe has been split into 2 functions */

Status jmpe_relIAEx(IAinstInfoPtr info)
{
    IADWORD tgt;
    Status st = StTakenBr;

    if (PSR_DI) {
	disabledInstSetTransitionFault();
	return StFault;
#if 0
    /* XXX - enable this when FP implemented */
    } else if (pendingIA32FPexceptions) {
	ia32FPfault();
	return StFault;
#endif
    }

    if (info->opSize == IA32)
	tgt = EIP + info->delta + info->disp32;
    else		/* IA16 */
	tgt = (EIP + info->delta + info->disp32) & 0xffff;
    JMPE_RP = (IADWORD)(EIP + CSD.base + info->delta);
    ip = IPfromEIP(tgt) & ~(ADDR)0xF;	/* 16-byte align IP */
    icp = NULL;
    eflagsFromArithFlags();
    setSegARsFromGRs();
    PSR_IS = 0;
    abi = unixABI;
    /* XXX - add taken branch trap */
    if (executionMode == RUN_IA)
	while (!PSR_IS) {
	    if (st & ST_STOP)
		total_cycles++;
	    if (st & ST_INST_COUNT)
		total_insts++;
	    st = unixABI ? iCycleApp() : iCycleSys();
	}
    return st;
}

Status jmpe_indIAEx(IAinstInfoPtr info)
{
    IADWORD tgt;
    ADDR adr, adr2;
    Status st = StTakenBr;

    if (PSR_DI) {
	disabledInstSetTransitionFault();
	return StFault;
#if 0
    /* XXX - enable this when FP implemented */
    } else if (pendingIA32FPexceptions) {
	ia32FPfault();
	return StFault;
#endif
    }
    if (!info->srcRdFn(info, MEM_LD_ACCESS, &tgt, &adr, &adr2))
	return StFault;

    JMPE_RP = (IADWORD)(EIP + CSD.base + info->delta);
    /* XXX - need opSize/addrSize masking ??? */
    ip = IPfromEIP(tgt) & ~(ADDR)0xF;	/* 16-byte align IP */
    icp = NULL;
    eflagsFromArithFlags();
    setSegARsFromGRs();
    PSR_IS = 0;
    abi = unixABI;
    /* XXX - add taken branch trap */
    if (executionMode == RUN_IA)
	while (!PSR_IS) {
	    if (st & ST_STOP)
		total_cycles++;
	    if (st & ST_INST_COUNT)
		total_insts++;
	    st = unixABI ? iCycleApp() : iCycleSys();
	}
    return st;
}

/* ARGSUSED */
Status lahfIAEx(IAinstInfoPtr info)
{
    IAeflags newFlags;

    /* lahf modifies no flags */
    newFlags.sf = arithFlags.sf;
    newFlags.zf = arithFlags.zf;
    newFlags.rv5 = 0;
    newFlags.af = arithFlags.af;
    newFlags.rv3 = 0;
    newFlags.pf = arithFlags.pf;
    newFlags.rv1 = 1;
    newFlags.cf = arithFlags.cf;
    AH = *((IAQWORD *)&newFlags);
    return StSuccess;
}

Status larIAEx(IAinstInfoPtr info)
{
    BYTE opSize = info->opSize;		/* save */

    /* force opSize to 2 so only 2 bytes will be read from memory */
    info->opSize = 2;
    /* XXX - finish this */

    info->opSize = opSize;		/* restore */
    return unimpIAEx(info);
}

Status leaIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_ST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* lea modifies no flags */
    info->destWrFn(info, src, adr, adr2);
    return StSuccess;
}

Status leaveIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status lgdtIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status lidtIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status lldtIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status lmswIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

/* used for lds, les, lfs, lgs, lss */
Status load_far_ptrIAEx(IAinstInfoPtr info)
{
    IADWORD src[2], regDest, segDest;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, src, &adr, &adr2) ||
	!segRegUpdateIARd(info, MEM_ST_ACCESS, &segDest, &adr, &adr2) ||
	!info->destRdFn(info, MEM_ST_ACCESS, &regDest, &adr, &adr2))
	return StFault;

    /* load_far_ptr modifies no flags */
    if (info->opSize == IA16) {
	info->destWrFn(info, src[0] & 0xffff, adr, adr2);
	segRegIAWr(info, src[0] >> 16, adr, adr2);
    } else {		/* IA32 */
	info->destWrFn(info, src[0], adr, adr2);
	segRegIAWr(info, src[1], adr, adr2);
    }
    return StSuccess;
}

Status lockIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status lodsIAEx(IAinstInfoPtr info)
{
    IADWORD inc;
    Status st;

    if ((st = movIAEx(info)) == StFault)
	return StFault;

    inc = (1 - (2 * EFLAGS.df)) * info->opSize;
    if (info->addrSize == IA16)
	SI += inc;
    else		/* IA32 */
	ESI += inc;
    return st;
}

Status loopIAEx(IAinstInfoPtr info)
{
    BOOL cond;
    IADWORD count;

    if (info->addrSize == IA16)
	count = --CX;
    else	/* IA32 */
	count = --ECX;

    switch (info->cond_seg) {
    case LOOPNZ_COND:
	cond = (count != 0) && !arithFlags.zf;
	break;
    case LOOPZ_COND:
	cond = (count != 0) && arithFlags.zf;
	break;
    case LOOP_COND:
	cond = count != 0;
	break;
    }

    /* loop modifies no flags */
    if (cond) {
	if (info->opSize == IA32)
	    EIP += info->delta + info->disp32;
	else
	    EIP = (EIP + info->delta + info->disp32) & 0xffff;
	ip = IPfromEIP(EIP);
	return StTakenBr;
    }
    return StSuccess;
}

Status lslIAEx(IAinstInfoPtr info)
{
    BYTE opSize = info->opSize;		/* save */

    /* force opSize to 2 so only 2 bytes will be read from memory */
    info->opSize = 2;
    /* XXX - finish this */

    info->opSize = opSize;		/* restore */
    return unimpIAEx(info);
}

Status ltrIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status movIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_ST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* mov modifies no flags */
    info->destWrFn(info, src, adr, adr2);
    return StSuccess;
}

Status movCRIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status movDRIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status movsIAEx(IAinstInfoPtr info)
{
    IADWORD inc;
    Status st;

    if ((st = movIAEx(info)) == StFault)
	return StFault;

    inc = (1 - (2 * EFLAGS.df)) * info->opSize;
    if (info->addrSize == IA16) {
	SI += inc;
	DI += inc;
    } else {		/* IA32 */
	ESI += inc;
	EDI += inc;
    }
    return st;
}

/* movsx has been split into 2 functions */

Status movsx8IAEx(IAinstInfoPtr info)
{
    IADWORD src, res;
    ADDR adr, adr2;
    BYTE opSize = info->opSize;		/* save */

    /* force opSize to 1 so only 1 byte will be read from memory */
    info->opSize = 1;
    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2))
	return StFault;
    info->opSize = opSize;		/* restore */

    /* movsx modifies no flags */
    res = (IASDWORD)(IASBYTE)src;
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

Status movsx16IAEx(IAinstInfoPtr info)
{
    IADWORD src, res;
    ADDR adr, adr2;
    BYTE opSize = info->opSize;		/* save */

    /* force opSize to 2 so only 2 bytes will be read from memory */
    info->opSize = 2;
    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2))
	return StFault;
    info->opSize = opSize;		/* restore */

    /* movsx modifies no flags */
    res = (IASDWORD)(IASWORD)src;
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

/* movzx has been split into 2 functions */

Status movzx8IAEx(IAinstInfoPtr info)
{
    IADWORD src, res;
    ADDR adr, adr2;
    BYTE opSize = info->opSize;		/* save */

    /* force opSize to 1 so only 1 byte will be read from memory */
    info->opSize = 1;
    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2))
	return StFault;
    info->opSize = opSize;		/* restore */

    /* movzx modifies no flags */
    res = (IADWORD)(BYTE)src;
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

Status movzx16IAEx(IAinstInfoPtr info)
{
    IADWORD src, res;
    ADDR adr, adr2;
    BYTE opSize = info->opSize;		/* save */

    /* force opSize to 2 so only 2 bytes will be read from memory */
    info->opSize = 2;
    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2))
	return StFault;
    info->opSize = opSize;		/* restore */

    /* movzx modifies no flags */
    res = (IADWORD)(IAWORD)src;
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

Status mulIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest;
    IAQWORD res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* mul modifies OF, CF; SF, ZF, AF, PF are undefined */
    switch (info->opSize) {
    case 1:
	/* AX = AL * rm8 */
	res = src * dest;
	arithFlags.of = arithFlags.cf = ((res >> 8) != 0);
	break;
    case 2:
	/* DX:AX = AX * rm16 */
	res = src * dest;
	DX = res >> 16;
	arithFlags.of = arithFlags.cf = ((res >> 16) != 0);
	break;
    case 4:
	/* EDX:EAX = EAX * rm32 */
	res = (IAQWORD)src * (IAQWORD)dest;
	EDX = res >> 32;
	arithFlags.of = arithFlags.cf = ((res >> 32) != 0);
	break;
    }
    info->destWrFn(info, (IADWORD)res, adr, adr2);
    return StSuccess;
}

Status negIAEx(IAinstInfoPtr info)
{
    IADWORD dest, res;
    ADDR adr, adr2;

    if (!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* neg modifies OF, SF, ZF, AF, CF, PF */
    arithFlags.cf = (dest != 0);
    switch (info->opSize) {
    case 1:
	res = (-dest) & 0xff;
	arithFlags.of = overflow8(-dest, 0, res);
	arithFlags.sf = sign8(res);
	break;
    case 2:
	res = (-dest) & 0xffff;
	arithFlags.of = overflow16(-dest, 0, res);
	arithFlags.sf = sign16(res);
	break;
    case 4:
	res = -dest;
	arithFlags.of = overflow32(-dest, 0, res);
	arithFlags.sf = sign32(res);
	break;
    }
    arithFlags.zf = (res == 0);
    arithFlags.af = auxCarry(dest, 0, res);
    arithFlags.pf = parity[res & 0xff];
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

/* ARGSUSED */
Status nopIAEx(IAinstInfoPtr info)
{
    return StSuccess;
}

Status notIAEx(IAinstInfoPtr info)
{
    IADWORD dest;
    ADDR adr, adr2;

    if (!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* not modifies no flags */
    info->destWrFn(info, ~dest, adr, adr2);
    return StSuccess;
}

Status orIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* or clears OF & CF; modifies SF, ZF, PF; AF is undefined */
    switch (info->opSize) {
    case 1:
	res = (dest | src) & 0xff;
	arithFlags.sf = sign8(res);
	break;
    case 2:
	res = (dest | src) & 0xffff;
	arithFlags.sf = sign16(res);
	break;
    case 4:
	res = dest | src;
	arithFlags.sf = sign32(res);
	break;
    }
    arithFlags.of = arithFlags.cf = 0;
    arithFlags.zf = (res == 0);
    arithFlags.pf = parity[res & 0xff];
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

Status outIAEx(IAinstInfoPtr info)
{
    IADWORD port, src;
    ADDR adr, adr2, portAdr;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LD_ACCESS, &port, &adr, &adr2))
	return StFault;

    /* XXX - add protected-mode & VM86 IOPL/permission bit checks */
    portAdr = IOBASE | ((port & ~3) << 10) | (port & 3);
    if (!memIAadrRd(portAdr, info->opSize, MEM_ST_ACCESS, &adr, &adr2, &src))
	return StFault;
    /* out modifies no flags */
    info->destWrFn(info, src, adr, adr2);
    return StSuccess;
}

Status outsIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status popIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2))
	return StFault;

    /* ESP must be adjusted BEFORE destRdFn() is called in case the dest
     * addressing mode is one which includes ESP as the base register.
     */
    if (StackAddrSize(info->mode) == IA16)
	SP += info->opSize;
    else			/* IA32 */
	ESP += info->opSize;

    if (!info->destRdFn(info, MEM_ST_ACCESS, &dest, &adr, &adr2)) {
	if (StackAddrSize(info->mode) == IA16)
	    SP -= info->opSize;
	else			/* IA32 */
	    ESP -= info->opSize;
	return StFault;
    }

    info->destWrFn(info, src, adr, adr2);
    return StSuccess;
}

/*
 * Stack Layout for popa/popad
 *
 * address
 *          +-----+
 *  high    |     |
 *          |     |        <--eSP (after)
 *          +-----+
 *   pop    | eAX | src[7]
 *    ^     | eCX | src[6]
 *    |     | eDX | src[5]
 *    |     | eBX | src[4]
 *    |     |*eSP*| src[3]  (ignored)
 *    |     | eBP | src[2]
 *    v     | eSI | src[1]
 *   push   | eDI | src[0]  <--eSP (before)
 *          +-----+
 *          |     |
 *  low     |     |
 *          +-----+
 */

Status popaIAEx(IAinstInfoPtr info)
{
    IADWORD src[8];
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, src, &adr, &adr2))
	return StFault;

    if (StackAddrSize(info->mode) == IA16)
	SP += 8 * info->opSize;
    else			/* IA32 */
	ESP += 8 * info->opSize;

    if (info->opSize == IA32) {
	EDI = src[0];
	ESI = src[1];
	EBP = src[2];
	/* skip ESP */
	EBX = src[4];
	EDX = src[5];
	ECX = src[6];
	EAX = src[7];
    } else {		/* IA16 */
	DI = src[0];
	SI = src[1];
	BP = src[2];
	/* skip SP */
	BX = src[4];
	DX = src[5];
	CX = src[6];
	AX = src[7];
    }
    return StSuccess;
}

Status popfIAEx(IAinstInfoPtr info)
{
    IADWORD src;
    DWORD newFlags;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2))
	return StFault;

    if (StackAddrSize(info->mode) == IA16)
	SP += info->opSize;
    else			/* IA32 */
	ESP += info->opSize;

    /* XXX - add VM86 & protected mode checks & system flag intercepts */
    if (info->opSize == IA16) {
	newFlags = (*(DWORD *)&EFLAGS & ~(DWORD)0xffff) |
	    (DWORD)(src & 0xffff);
	EFLAGS = *(IAeflagsPtr)&newFlags;
    } else {		/* IA32 */
	newFlags = (DWORD)src;
	EFLAGS = *(IAeflagsPtr)&newFlags;
    }
    EFLAGS.vip = EFLAGS.vif = 0;
    /* XXX - do we need to check reserved fields or just write them to 0/1? */
    arithFlagsFromEflags();
    return StSuccess;
}

Status pushIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_ST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    info->destWrFn(info, src, adr, adr2);	/* also adjusts eSP */
    return StSuccess;
}

/*
 * Stack Layout for pusha/pushad
 *
 * address
 *          +-----+
 *  high    |     |
 *          |     |        <--eSP (before)
 *          +-----+
 *   pop    | eAX | dest[7]
 *    ^     | eCX | dest[6]
 *    |     | eDX | dest[5]
 *    |     | eBX | dest[4]
 *    |     | eSP | dest[3]  ("before" value)
 *    |     | eBP | dest[2]
 *    v     | eSI | dest[1]
 *   push   | eDI | dest[0]  <--eSP (after)
 *          +-----+
 *          |     |
 *  low     |     |
 *          +-----+
 */

Status pushaIAEx(IAinstInfoPtr info)
{
    IADWORD dest[8];
    ADDR adr, adr2;

    /* adr returned by destRdFn() is eSP (after) */
    if (!info->destRdFn(info, MEM_ST_ACCESS, dest, &adr, &adr2))
	return StFault;

    if (info->opSize == IA32) {
	dest[0] = EDI;
	dest[1] = ESI;
	dest[2] = EBP;
	dest[3] = ESP;	/* (before) */
	dest[4] = EBX;
	dest[5] = EDX;
	dest[6] = ECX;
	dest[7] = EAX;
    } else {		/* IA16 */
	dest[0] = DI;
	dest[1] = SI;
	dest[2] = BP;
	dest[3] = SP;	/* (before) */
	dest[4] = BX;
	dest[5] = DX;
	dest[6] = CX;
	dest[7] = AX;
    }

    /* XXX - this is way more complicated than it should be */
    /* must loop because destWrFn() only gets a single IADWORD */
    if (adr2 == adr + 8 * info->opSize) {	/* single page */
	unsigned i;

	for (i = 0; i < 8; i++, adr += info->opSize)
	    info->destWrFn(info, dest[i], adr, 0);
    } else {					/* page crossing */
	unsigned i, p1wds;

	p1wds = (page_base(adr) + page_size + info->opSize - 1 - adr)
	    / info->opSize;
	for (i = 0; i < p1wds; i++, adr += info->opSize)
	    info->destWrFn(info, dest[i], adr, adr2);
	/* second loop for second page */
	for (adr2 += adr % info->opSize; i < 8; i++, adr2 += info->opSize)
	    info->destWrFn(info, dest[i], adr2, 0);
    }

    /* destWrFn() does not adjust eSP, so do it here */
    if (StackAddrSize(info->mode) == IA16)
	SP -= 8 * info->opSize;
    else			/* IA32 */
	ESP -= 8 * info->opSize;

    return StSuccess;
}

Status pushfIAEx(IAinstInfoPtr info)
{
    IADWORD dest, flags;
    ADDR adr, adr2;

    if (!info->destRdFn(info, MEM_ST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    eflagsFromArithFlags();
    /* XXX - add VM86 IOPL checks */
    if (info->opSize == IA32)		/* VM & RF not pushed */
	flags = (IADWORD)*(DWORD *)&EFLAGS & 0x00fcffff;
    else				/* lower 16 bits only */
	flags = (IADWORD)*(DWORD *)&EFLAGS & 0xffff;

    info->destWrFn(info, flags, adr, adr2);	/* also adjusts eSP */
    return StSuccess;
}

Status rclIAEx(IAinstInfoPtr info)
{
    IADWORD count, dest;
    IAQWORD res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &count, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    if ((count &= 0x1F) == 0)	/* mask count to 0 - 31 & check for 0 */
	return StSuccess;		/* nop */
    /* XXX - the reference manuals contain inconsistent statements about
     * whether count is modulo operand size
     */

    /* rcl modifies OF, CF; OF is undefined if count > 1 */
    switch (info->opSize) {
    case 1:
	count %= 9;
	res = dest << 9 | (IADWORD)arithFlags.cf << 8 | dest;
	arithFlags.cf = (res >> (8 - count)) & 0x1;
	res = (res >> (9 - count)) & 0xff;
	arithFlags.of = arithFlags.cf ^ sign8(res);
	break;
    case 2:
	count %= 17;
	res = dest << 17 | (IADWORD)arithFlags.cf << 16 | dest;
	arithFlags.cf = (res >> (16 - count)) & 0x1;
	res = (res >> (17 - count)) & 0xffff;
	arithFlags.of = arithFlags.cf ^ sign16(res);
	break;
    case 4:
	res = (IAQWORD)dest << 33 | (IAQWORD)arithFlags.cf << 32 |
	    (IAQWORD)dest;
	arithFlags.cf = (res >> (32 - count)) & 0x1;
	res = (res >> (33 - count)) & 0xffffffff;
	arithFlags.of = arithFlags.cf ^ sign32(res);
	break;
    }
    info->destWrFn(info, (IADWORD)res, adr, adr2);
    return StSuccess;
}

Status rcrIAEx(IAinstInfoPtr info)
{
    IADWORD count, dest;
    IAQWORD res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &count, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    if ((count &= 0x1F) == 0)	/* mask count to 0 - 31 & check for 0 */
	return StSuccess;		/* nop */
    /* XXX - the reference manuals contain inconsistent statements about
     * whether count is modulo operand size
     */

    /* rcr modifies OF, CF; OF is undefined if count > 1 */
    switch (info->opSize) {
    case 1:
	count %= 9;
	res = dest << 9 | (IADWORD)arithFlags.cf << 8 | dest;
	arithFlags.cf = (res >> (count - 1)) & 0x1;
	res = (res >> count) & 0xff;
	arithFlags.of = sign8(res) ^ bit6(res);
	break;
    case 2:
	count %= 17;
	res = dest << 17 | (IADWORD)arithFlags.cf << 16 | dest;
	arithFlags.cf = (res >> (count - 1)) & 0x1;
	res = (res >> count) & 0xffff;
	arithFlags.of = sign16(res) ^ bit14(res);
	break;
    case 4:
	res = (IAQWORD)dest << 33 | (IAQWORD)arithFlags.cf << 32 |
	    (IAQWORD)dest;
	arithFlags.cf = (res >> (count - 1)) & 0x1;
	res = (res >> count) & 0xffffffff;
	arithFlags.of = sign32(res) ^ bit30(res);
	break;
    }
    info->destWrFn(info, (IADWORD)res, adr, adr2);
    return StSuccess;
}

Status rdmsrIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status rdpmcIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status rdtscIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

/* rep has 3 common functions for rep, repe, and repne, plus one function
 * per string instruction which call the appropriate common function */

static Status repIAEx(IAinstInfoPtr info, PIAEF execFn)
{
    Status st;

    if (info->addrSize == IA16) {
	if (CX == 0)
	    return StSuccess;
    } else {		/* IA32 */
	if (ECX == 0)
	    return StSuccess;
    }

    if ((st = execFn(info)) == StFault)
	return StFault;

    if (info->addrSize == IA16) {
	if (--CX == 0)
	    return st;
    } else {		/* IA32 */
	if (--ECX == 0)
	    return st;
    }

    return st & ~ST_IP_INC;
}

static Status repeIAEx(IAinstInfoPtr info, PIAEF execFn)
{
    Status st;

    if (info->addrSize == IA16) {
	if (CX == 0)
	    return StSuccess;
    } else {		/* IA32 */
	if (ECX == 0)
	    return StSuccess;
    }

    if ((st = execFn(info)) == StFault)
	return StFault;

    if (info->addrSize == IA16) {
	if (--CX == 0 || arithFlags.zf == 0)
	    return st;
    } else {		/* IA32 */
	if (--ECX == 0 || arithFlags.zf == 0)
	    return st;
    }

    return st & ~ST_IP_INC;
}

static Status repneIAEx(IAinstInfoPtr info, PIAEF execFn)
{
    Status st;

    if (info->addrSize == IA16) {
	if (CX == 0)
	    return StSuccess;
    } else {		/* IA32 */
	if (ECX == 0)
	    return StSuccess;
    }

    if ((st = execFn(info)) == StFault)
	return StFault;

    if (info->addrSize == IA16) {
	if (--CX == 0 || arithFlags.zf == 1)
	    return st;
    } else {		/* IA32 */
	if (--ECX == 0 || arithFlags.zf == 1)
	    return st;
    }

    return st & ~ST_IP_INC;
}

Status rep_insIAEx(IAinstInfoPtr info)
{
    return repIAEx(info, insIAEx);
}

Status rep_lodsIAEx(IAinstInfoPtr info)
{
    return repIAEx(info, lodsIAEx);
}

Status rep_movsIAEx(IAinstInfoPtr info)
{
    return repIAEx(info, movsIAEx);
}

Status rep_outsIAEx(IAinstInfoPtr info)
{
    return repIAEx(info, outsIAEx);
}

Status rep_stosIAEx(IAinstInfoPtr info)
{
    return repIAEx(info, stosIAEx);
}

Status repe_cmpsIAEx(IAinstInfoPtr info)
{
    return repeIAEx(info, cmpsIAEx);
}

Status repe_scasIAEx(IAinstInfoPtr info)
{
    return repeIAEx(info, scasIAEx);
}

Status repne_cmpsIAEx(IAinstInfoPtr info)
{
    return repneIAEx(info, cmpsIAEx);
}

Status repne_scasIAEx(IAinstInfoPtr info)
{
    return repneIAEx(info, scasIAEx);
}

/* ret has been split into 2 functions */

Status ret_nearIAEx(IAinstInfoPtr info)
{
    IADWORD retEIP;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &retEIP, &adr, &adr2))
	return StFault;

    if (StackAddrSize(info->mode) == IA16)
	SP += info->opSize + info->imm32;
    else			/* IA32 */
	ESP += info->opSize + info->imm32;

    if (info->opSize == IA32)
	EIP = retEIP;
    else	/* IA16 */
	EIP = retEIP & 0xffff;
    /* XXX - add code segment limit check */
    ip = IPfromEIP(EIP);
    return StTakenBr;
}

Status ret_farIAEx(IAinstInfoPtr info)
{
    IADWORD dest, retEIP, retCS;
    ADDR adr, adr2, ofsAdr, ofsAdr2, segAdr, segAdr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &retEIP, &ofsAdr, &ofsAdr2) ||
	!segRegUpdateIARd(info, MEM_ST_ACCESS, &dest, &adr, &adr2) ||
	!ret_farIARd(info, MEM_LD_ACCESS, &retCS, &segAdr, &segAdr2))
	return StFault;

    if (StackAddrSize(info->mode) == IA16)
	SP += (2 * info->opSize) + info->imm32;
    else			/* IA32 */
	ESP += (2 * info->opSize) + info->imm32;

    if (info->opSize == IA32)
	EIP = retEIP;
    else	/* IA16 */
	EIP = retEIP & 0xffff;
    segRegIAWr(info, retCS, adr, adr2);
    /* XXX - add code segment limit check */
    ip = IPfromEIP(EIP);
    return StTakenBr;
}

Status rolIAEx(IAinstInfoPtr info)
{
    IADWORD count, dest;
    IAQWORD res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &count, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    if ((count &= 0x1F) == 0)	/* mask count to 0 - 31 & check for 0 */
	return StSuccess;		/* nop */
    /* XXX - the reference manuals contain inconsistent statements about
     * whether count is modulo operand size
     */

    /* rol modifies OF, CF; OF is undefined if count > 1 */
    switch (info->opSize) {
    case 1:
	count %= 8;
	res = ((dest << 8 | dest) >> (8 - count)) & 0xff;
	arithFlags.cf = bit0(res);
	arithFlags.of = arithFlags.cf ^ sign8(res);
	break;
    case 2:
	count %= 16;
	res = ((dest << 16 | dest) >> (16 - count)) & 0xffff;
	arithFlags.cf = bit0(res);
	arithFlags.of = arithFlags.cf ^ sign16(res);
	break;
    case 4:
	res = (((IAQWORD)dest << 32 | (IAQWORD)dest) >> (32 - count))
	    & 0xffffffff;
	arithFlags.cf = bit0(res);
	arithFlags.of = arithFlags.cf ^ sign32(res);
	break;
    }
    info->destWrFn(info, (IADWORD)res, adr, adr2);
    return StSuccess;
}

Status rorIAEx(IAinstInfoPtr info)
{
    IADWORD count, dest;
    IAQWORD res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &count, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    if ((count &= 0x1F) == 0)	/* mask count to 0 - 31 & check for 0 */
	return StSuccess;		/* nop */
    /* XXX - the reference manuals contain inconsistent statements about
     * whether count is modulo operand size
     */

    /* ror modifies OF, CF; OF is undefined if count > 1 */
    switch (info->opSize) {
    case 1:
	count %= 8;
	res = ((dest << 8 | dest) >> count) & 0xff;
	arithFlags.cf = sign8(res);
	arithFlags.of = arithFlags.cf ^ bit6(res);
	break;
    case 2:
	count %= 16;
	res = ((dest << 16 | dest) >> count) & 0xffff;
	arithFlags.cf = sign16(res);
	arithFlags.of = arithFlags.cf ^ bit14(res);
	break;
    case 4:
	res = (((IAQWORD)dest << 32 | (IAQWORD)dest) >> count) & 0xffffffff;
	arithFlags.cf = sign32(res);
	arithFlags.of = arithFlags.cf ^ bit30(res);
	break;
    }
    info->destWrFn(info, (IADWORD)res, adr, adr2);
    return StSuccess;
}

Status rsmIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

/* ARGSUSED */
Status sahfIAEx(IAinstInfoPtr info)
{
    IAQWORD ah = AH;
    IAeflags newFlags = *((IAeflagsPtr)&ah);

    /* sahf modifies SF, ZF, AF, PF, CF */
    arithFlags.sf = newFlags.sf;
    arithFlags.zf = newFlags.zf;
    arithFlags.af = newFlags.af;
    arithFlags.pf = newFlags.pf;
    arithFlags.cf = newFlags.cf;
    return StSuccess;
}

Status sarIAEx(IAinstInfoPtr info)
{
    IADWORD count, dest;
    IAQWORD res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &count, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    if ((count &= 0x1F) == 0)	/* mask count to 0 - 31 & check for 0 */
	return StSuccess;		/* nop */

    /* sar modifies OF, SF, ZF, CF, PF; AF is undefined, OF is undefined
     * if count > 1
     */
    switch (info->opSize) {
    case 1:
	res = ((IASQWORD)(IASBYTE)dest << 32) >> count;
	arithFlags.cf = bit31(res);
	res = (res >> 32) & 0xff;
	arithFlags.sf = sign8(res);
	break;
    case 2:
	res = ((IASQWORD)(IASWORD)dest << 32) >> count;
	arithFlags.cf = bit31(res);
	res = (res >> 32) & 0xffff;
	arithFlags.sf = sign16(res);
	break;
    case 4:
	res = ((IASQWORD)(IASDWORD)dest << 32) >> count;
	arithFlags.cf = bit31(res);
	res = (res >> 32) & 0xffffffff;
	arithFlags.sf = sign32(res);
	break;
    }
    arithFlags.of = 0;
    arithFlags.zf = (res == 0);
    arithFlags.pf = parity[res & 0xff];
    info->destWrFn(info, (IADWORD)res, adr, adr2);
    return StSuccess;
}

Status sbbIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* sbb modifies OF, SF, ZF, AF, CF, PF */
    switch (info->opSize) {
    case 1:
	res = (dest - (src + arithFlags.cf)) & 0xff;
	arithFlags.of = overflow8(src, dest, res);
	arithFlags.sf = sign8(res);
	arithFlags.cf = borrow8(dest, res, arithFlags.cf);
	break;
    case 2:
	res = (dest - (src + arithFlags.cf)) & 0xffff;
	arithFlags.of = overflow16(src, dest, res);
	arithFlags.sf = sign16(res);
	arithFlags.cf = borrow16(dest, res, arithFlags.cf);
	break;
    case 4:
	res = dest - (src + arithFlags.cf);
	arithFlags.of = overflow32(src, dest, res);
	arithFlags.sf = sign32(res);
	arithFlags.cf = borrow32(dest, res, arithFlags.cf);
	break;
    }
    arithFlags.zf = (res == 0);
    arithFlags.af = auxCarry(src, dest, res);
    arithFlags.pf = parity[res & 0xff];
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

Status scasIAEx(IAinstInfoPtr info)
{
    IADWORD inc;
    Status st;

    if ((st = cmpIAEx(info)) == StFault)
	return StFault;

    inc = (1 - (2 * EFLAGS.df)) * info->opSize;
    if (info->addrSize == IA16)
	DI += inc;
    else		/* IA32 */
	EDI += inc;
    return st;
}

Status setccIAEx(IAinstInfoPtr info)
{
    IADWORD dest;
    ADDR adr, adr2;
    BOOL cond;

    if (!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    switch (info->cond_seg) {
    case JO_COND:
	cond = arithFlags.of;
	break;
    case JNO_COND:
	cond = !arithFlags.of;
	break;
    case JB_COND:
	cond = arithFlags.cf;
	break;
    case JNB_COND:
	cond = !arithFlags.cf;
	break;
    case JZ_COND:
	cond = arithFlags.zf;
	break;
    case JNZ_COND:
	cond = !arithFlags.zf;
	break;
    case JBE_COND:
	cond = arithFlags.cf || arithFlags.zf;
	break;
    case JNBE_COND:
	cond = !(arithFlags.cf || arithFlags.zf);
	break;
    case JS_COND:
	cond = arithFlags.sf;
	break;
    case JNS_COND:
	cond = !arithFlags.sf;
	break;
    case JP_COND:
	cond = arithFlags.pf;
	break;
    case JNP_COND:
	cond = !arithFlags.pf;
	break;
    case JL_COND:
	cond = arithFlags.sf != arithFlags.of;
	break;
    case JNL_COND:
	cond = arithFlags.sf == arithFlags.of;
	break;
    case JLE_COND:
	cond = (arithFlags.sf != arithFlags.of) || arithFlags.zf;
	break;
    case JNLE_COND:
	cond = (arithFlags.sf == arithFlags.of) && !arithFlags.zf;
	break;
    }

    dest = (cond) ? 1 : 0;
    /* setcc modifies no flags */
    info->destWrFn(info, dest, adr, adr2);
    return StSuccess;
}

Status sgdtIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status shlIAEx(IAinstInfoPtr info)
{
    IADWORD count, dest;
    IAQWORD res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &count, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    if ((count &= 0x1F) == 0)	/* mask count to 0 - 31 & check for 0 */
	return StSuccess;		/* nop */

    /* shl modifies OF, SF, ZF, CF, PF; AF is undefined, OF is undefined
     * if count > 1
     */
    switch (info->opSize) {
    case 1:
	res = (IAQWORD)dest << count;
	arithFlags.sf = sign8(res);
	arithFlags.cf = bit8(res);
	res &= 0xff;
	break;
    case 2:
	res = (IAQWORD)dest << count;
	arithFlags.sf = sign16(res);
	arithFlags.cf = bit16(res);
	res &= 0xffff;
	break;
    case 4:
	res = (IAQWORD)dest << count;
	arithFlags.sf = sign32(res);
	arithFlags.cf = bit32(res);
	res &= 0xffffffff;
	break;
    }
    arithFlags.of = arithFlags.sf ^ arithFlags.cf;
    arithFlags.zf = (res == 0);
    arithFlags.pf = parity[res & 0xff];
    info->destWrFn(info, (IADWORD)res, adr, adr2);
    return StSuccess;
}

/* shld has been split into 2 functions which read the count and call a
 * common function to do the work */

static Status shld(IAinstInfoPtr info, IADWORD count)
{
    IADWORD src, dest;
    IAQWORD res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    if ((count &= 0x1F) == 0)	/* mask count to 0 - 31 & check for 0 */
	return StSuccess;		/* nop */

    /* shld modifies OF, SF, ZF, CF, PF; AF is undefined, OF is undefined
     * if count > 1; all flags are undefined if count > opSize
     */
    switch (info->opSize) {
    case 2:
	res = ((dest << 16 | src) << count) >> 16;
	arithFlags.sf = sign16(res);
	arithFlags.cf = bit16(res);
	res &= 0xffff;
	break;
    case 4:
	res = (((IAQWORD)dest << 32 | (IAQWORD)src) << count) >> 32;
	arithFlags.sf = sign32(res);
	arithFlags.cf = bit32(res);
	res &= 0xffffffff;
	break;
    }
    arithFlags.of = arithFlags.sf ^ arithFlags.cf;
    arithFlags.zf = (res == 0);
    arithFlags.pf = parity[res & 0xff];
    info->destWrFn(info, (IADWORD)res, adr, adr2);
    return StSuccess;
}

Status shld_immIAEx(IAinstInfoPtr info)
{
    return shld(info, info->imm32);
}

Status shld_CLIAEx(IAinstInfoPtr info)
{
    return shld(info, CL);
}

Status shrIAEx(IAinstInfoPtr info)
{
    IADWORD count, dest;
    IAQWORD res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &count, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    if ((count &= 0x1F) == 0)	/* mask count to 0 - 31 & check for 0 */
	return StSuccess;		/* nop */

    /* shr modifies OF, SF, ZF, CF, PF; AF is undefined, OF is undefined
     * if count > 1
     */
    switch (info->opSize) {
    case 1:
	arithFlags.of = sign8(dest);
	res = ((IAQWORD)dest << 32) >> count;
	arithFlags.cf = bit31(res);
	res = (res >> 32) & 0xff;
	arithFlags.sf = sign8(res);
	break;
    case 2:
	arithFlags.of = sign16(dest);
	res = ((IAQWORD)dest << 32) >> count;
	arithFlags.cf = bit31(res);
	res = (res >> 32) & 0xffff;
	arithFlags.sf = sign16(res);
	break;
    case 4:
	arithFlags.of = sign32(dest);
	res = ((IAQWORD)dest << 32) >> count;
	arithFlags.cf = bit31(res);
	res = (res >> 32) & 0xffffffff;
	arithFlags.sf = sign32(res);
	break;
    }
    arithFlags.zf = (res == 0);
    arithFlags.pf = parity[res & 0xff];
    info->destWrFn(info, (IADWORD)res, adr, adr2);
    return StSuccess;
}

/* shrd has been split into 2 functions which read the count and call a
 * common function to do the work */

static Status shrd(IAinstInfoPtr info, IADWORD count)
{
    IADWORD src, dest;
    IAQWORD res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    if ((count &= 0x1F) == 0)	/* mask count to 0 - 31 & check for 0 */
	return StSuccess;		/* nop */

    /* shld modifies OF, SF, ZF, CF, PF; AF is undefined, OF is undefined
     * if count > 1; all flags are undefined if count > opSize
     */
    switch (info->opSize) {
    case 2:
	arithFlags.cf = (dest >> (count - 1)) & 0x1;
	res = ((src << 16 | dest) >> count) & 0xffff;
	arithFlags.sf = sign16(res);
	arithFlags.of = arithFlags.sf ^ bit14(res);
	break;
    case 4:
	arithFlags.cf = (dest >> (count - 1)) & 0x1;
	res = (((IAQWORD)src << 32 | (IAQWORD)dest) >> count) & 0xffffffff;
	arithFlags.sf = sign32(res);
	arithFlags.of = arithFlags.sf ^ bit30(res);
	break;
    }
    arithFlags.zf = (res == 0);
    arithFlags.pf = parity[res & 0xff];
    info->destWrFn(info, (IADWORD)res, adr, adr2);
    return StSuccess;
}

Status shrd_immIAEx(IAinstInfoPtr info)
{
    return shrd(info, info->imm32);
}

Status shrd_CLIAEx(IAinstInfoPtr info)
{
    return shrd(info, CL);
}

Status sidtIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status sldtIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status smswIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

/* ARGSUSED */
Status stcIAEx(IAinstInfoPtr info)
{
    /* current CF is in arithFlags, not EFLAGS */
    arithFlags.cf = 1;
    return StSuccess;
}

/* ARGSUSED */
Status stdIAEx(IAinstInfoPtr info)
{
    EFLAGS.df = 1;
    return StSuccess;
}

/* ARGSUSED */
Status stiIAEx(IAinstInfoPtr info)
{
    /* XXX - add protected mode & VM86 stuff */
    /* XXX - add intercept stuff according to Seamless 2.0 */
    /* XXX - how do we keep interrupts disabled until after next inst ? */
    EFLAGS.if_ = 1;
    return StSuccess;
}

Status stosIAEx(IAinstInfoPtr info)
{
    IADWORD inc;
    Status st;

    if ((st = movIAEx(info)) == StFault)
	return StFault;

    inc = (1 - (2 * EFLAGS.df)) * info->opSize;
    if (info->addrSize == IA16)
	DI += inc;
    else		/* IA32 */
	EDI += inc;
    return st;
}

Status strIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status subIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* sub modifies OF, SF, ZF, AF, CF, PF */
    switch (info->opSize) {
    case 1:
	res = (dest - src) & 0xff;
	arithFlags.of = overflow8(dest, -src, res);
	arithFlags.sf = sign8(res);
	arithFlags.cf = borrow8(dest, res, 0);
	break;
    case 2:
	res = (dest - src) & 0xffff;
	arithFlags.of = overflow16(dest, -src, res);
	arithFlags.sf = sign16(res);
	arithFlags.cf = borrow16(dest, res, 0);
	break;
    case 4:
	res = dest - src;
	arithFlags.of = overflow32(dest, -src, res);
	arithFlags.sf = sign32(res);
	arithFlags.cf = borrow32(dest, res, 0);
	break;
    }
    arithFlags.zf = (res == 0);
    arithFlags.af = auxCarry(src, dest, res);
    arithFlags.pf = parity[res & 0xff];
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

Status testIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LD_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* test clears OF & CF; modifies SF, ZF, PF; AF is undefined */
    switch (info->opSize) {
    case 1:
	res = (dest & src) & 0xff;
	arithFlags.sf = sign8(res);
	break;
    case 2:
	res = (dest & src) & 0xffff;
	arithFlags.sf = sign16(res);
	break;
    case 4:
	res = dest & src;
	arithFlags.sf = sign32(res);
	break;
    }
    arithFlags.of = arithFlags.cf = 0;
    arithFlags.zf = (res == 0);
    arithFlags.pf = parity[res & 0xff];
    return StSuccess;
}

Status ud2IAEx(IAinstInfoPtr info)
{
    return reservedIAEx(info);
}

Status verrIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status verwIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status waitIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status wbinvdIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status wrmsrIAEx(IAinstInfoPtr info)
{
    return unimpIAEx(info);
}

Status xaddIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LDST_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* xadd modifies OF, SF, ZF, AF, CF, PF */
    switch (info->opSize) {
    case 1:
	res = (dest + src) & 0xff;
	arithFlags.of = overflow8(src, dest, res);
	arithFlags.sf = sign8(res);
	arithFlags.cf = carry8(src, res, 0);
	break;
    case 2:
	res = (dest + src) & 0xffff;
	arithFlags.of = overflow16(src, dest, res);
	arithFlags.sf = sign16(res);
	arithFlags.cf = carry16(src, res, 0);
	break;
    case 4:
	res = dest + src;
	arithFlags.of = overflow32(src, dest, res);
	arithFlags.sf = sign32(res);
	arithFlags.cf = carry32(src, res, 0);
	break;
    }
    arithFlags.zf = (res == 0);
    arithFlags.af = auxCarry(src, dest, res);
    arithFlags.pf = parity[res & 0xff];
    info->destWrFn(info, res, adr, adr2);
    regIAWr(info, dest, adr, adr2);
    return StSuccess;
}

Status xchgIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LDST_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* xchg modifies no flags */
    info->destWrFn(info, src, adr, adr2);
    regIAWr(info, dest, adr, adr2);
    return StSuccess;
}

Status xlatIAEx(IAinstInfoPtr info)
{
    IADWORD src;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2))
	return StFault;

    AL = src;
    /* xlat modifies no flags */
    return StSuccess;
}

Status xorIAEx(IAinstInfoPtr info)
{
    IADWORD src, dest, res;
    ADDR adr, adr2;

    if (!info->srcRdFn(info, MEM_LD_ACCESS, &src, &adr, &adr2) ||
	!info->destRdFn(info, MEM_LDST_ACCESS, &dest, &adr, &adr2))
	return StFault;

    /* xor clears OF & CF; modifies SF, ZF, PF; AF is undefined */
    switch (info->opSize) {
    case 1:
	res = (dest ^ src) & 0xff;
	arithFlags.sf = sign8(res);
	break;
    case 2:
	res = (dest ^ src) & 0xffff;
	arithFlags.sf = sign16(res);
	break;
    case 4:
	res = dest ^ src;
	arithFlags.sf = sign32(res);
	break;
    }
    arithFlags.of = arithFlags.cf = 0;
    arithFlags.zf = (res == 0);
    arithFlags.pf = parity[res & 0xff];
    info->destWrFn(info, res, adr, adr2);
    return StSuccess;
}

/* extra function for instructions not yet implemented */
/* XXX - delete when no longer needed */
/* ARGSUSED */
Status unimpIAEx(IAinstInfoPtr info)
{
    char buf[40];

    symAddrtoName(ip, buf, 4, NO, -32);
    progStop("Stopping at %s -- unimplemented IA-32 instruction\n", buf);
    /*NOTREACHED*/
    return StSuccess;
}

/* extra function for reserved instructions */
/* ARGSUSED */
Status reservedIAEx(IAinstInfoPtr info)
{
    char buf[40];

    /* XXX - need to cause fault */
    symAddrtoName(ip, buf, 4, NO, -32);
    progExit("Stopping at %s -- reserved IA-32 instruction\n", buf);
    /*NOTREACHED*/
    return StSuccess;
}
