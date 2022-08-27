/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator IA Instruction Disassembly Routines
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
#include "sim.h"
#include "libsym.h"
#include "libcore.h"	/* XXX - Just for grGet used by IPfromEIP which is
			 * only used in a call to symAddrtoName -- redesign? */
#include "iadas_types.h"
#include "ia_decoder.h"
#include "ia_das.h"

ADDR dasAddress(void);

#define MNE	"%-12s"		/* field width for assembly mnemonic */
#define HEX	16		/* force hex display of immediates */

static ADDR IPfromEIP(ADDR4 eip)
{
    REG tmpreg;
    IASegDesc csd;

    tmpreg = grGet(0, CSD_ID);
    csd = *(IASegDescPtr)&tmpreg;
    return eip + (ADDR4)csd.base;
}

static char *imm(IAinstInfoPtr info, unsigned size, unsigned hex)
{
    static char ret[32];
    long sval = (long)info->imm32;

    if (!hex && sval > -100 && sval < 100)
	(void)sprintf(ret, "%ld", sval);
    else
	switch (size) {
	case 1:
	    (void)sprintf(ret, "0x%02x", info->imm32 & 0xff);
	    break;
	case 2:
	    (void)sprintf(ret, "0x%04x", info->imm32 & 0xffff);
	    break;
	case 4:
	    (void)sprintf(ret, "0x%08x", info->imm32);
	    break;
	}
    return ret;
}

static char *disp(IAinstInfoPtr info, unsigned size)
{
    static char ret[32];
    long sval = (long)info->disp32;

    if (sval > -100 && sval < 100)
	(void)sprintf(ret, "%ld", sval);
    else
	switch (size) {
	case 1:
	    (void)sprintf(ret, "0x%02x", info->disp32 & 0xff);
	    break;
	case 2:
	    (void)sprintf(ret, "0x%04x", info->disp32 & 0xffff);
	    break;
	case 4:
	    (void)sprintf(ret, "0x%08x", info->disp32);
	    break;
	}
    return ret;
}

static char *segOfs(IAinstInfoPtr info, unsigned size)
{
    static char ret[41] = "far ptr ";
    ADDR tip;

    /* XXX - add protected-mode support */
    if (size == IA16)
	tip = (ADDR)(((info->imm32 & 0xffff) << 4) + (info->disp32 & 0xffff));
    else
	tip = (ADDR)(((info->imm32 & 0xffff) << 4) + info->disp32);
    symAddrtoName(tip, &ret[8], 4, NO, -32);
    return ret;
}

static char *ofs(IAinstInfoPtr info, unsigned size)
{
    static char ret[33];
    ADDR4 eip;

    if (size == IA16)
	eip = (ADDR4)(dasAddress() + info->delta + info->disp32) & 0xffff;
    else
	eip = (ADDR4)(dasAddress() + info->delta + info->disp32);
    symAddrtoName(IPfromEIP(eip), ret, 4, NO, -32);
    return ret;
}

static char *r8Name[8] = {
  "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"
};

static char *r16Name[8] = {
  "ax", "cx", "dx", "bx", "sp", "bp", "si", "di"
};

static char *r32Name[8] = {
  "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"
};

static char *segName[8] = {
  "es", "cs", "ss", "ds", "fs", "gs"
};

static char *rName(BYTE reg, BYTE size)
{
    switch (size) {
    case 1:
	return r8Name[reg];
    case 2:
	return r16Name[reg];
    case 4:
	return r32Name[reg];
    }
    /*NOTREACHED*/
    return (char*)"";
}

static char *regName(IAinstInfoPtr info)
{
    return rName(info->reg, info->opSize);
}

static char *baseName(IAinstInfoPtr info)
{
    return rName(info->base, info->opSize);
}

static char *segRegName(BYTE segID)
{
    return segName[segID - ES_ID];
}

static char *segOverride(IAinstInfoPtr info, BYTE defSeg)
{
    static char seg[] = "xx:";

    if (info->segment == defSeg)
	seg[0] = '\0';
    else
	(void)strncpy(seg, segRegName(info->segment), 2);
    return seg;
}

static char *rep(IAinstInfoPtr info, char *mne)
{
    static char buf[20], *p;

    switch (info->lock_rep.rep) {
    case REP:
	(void)strcpy(buf, "rep ");
	p = buf + 4;
	break;
    case REPE:
	(void)strcpy(buf, "repe ");
	p = buf + 5;
	break;
    case REPNE:
	(void)strcpy(buf, "repne ");
	p = buf + 6;
	break;
    default:
	p = buf;
	break;
    }
    (void)strcpy(p, mne);
    return buf;
}

static void sibEA(IAinstInfoPtr info, BYTE mod, char *buf, size_t buf_sz)
{
    char baseStr[6], scaleStr[5], indexStr[8];
    BYTE defSeg;

    if (info->base == NO_REG) {
	defSeg = DS_ID;
	baseStr[0] = '\0';
    } else {
	if (info->base == ESP_REG || info->base == EBP_REG)
	    defSeg = SS_ID;
	else
	    defSeg = DS_ID;
	(void)sprintf(baseStr, "[%s]", r32Name[info->base]);
    }

    if (info->scale == 1)
	scaleStr[0] = '\0';
    else
	(void)snprintf(scaleStr, sizeof (scaleStr), "*%d", info->scale);

    if (info->index == NO_REG)
	indexStr[0] = '\0';
    else
	(void)sprintf(indexStr, "[%s%s]", r32Name[info->index], scaleStr);

    switch (mod) {
    case 0:
	if (info->base == NO_REG)
	    (void)snprintf(buf, buf_sz, "%s%s%s", segOverride(info, defSeg),
			  disp(info, 4), indexStr);
	else
	    (void)snprintf(buf, buf_sz, "%s%s%s", segOverride(info, defSeg),
			  baseStr, indexStr);
	break;
    case 1:
	(void)snprintf(buf, buf_sz, "%s%s%s%s", segOverride(info, defSeg),
		      disp(info, 1), baseStr, indexStr);
	break;
    case 2:
	(void)snprintf(buf, buf_sz, "%s%s%s%s", segOverride(info, defSeg),
		      disp(info, 4), baseStr, indexStr);
	break;
    }
}

static char *modrmEA(IAinstInfoPtr info)
{
    static char buf[80];
    EaInfoPtr eai;

    if (info->addrSize == IA16) {	/* 16-bit addressing forms */
	switch (info->modrm.mod) {
	case 0:
	    eai = ea16Info(info->modrm);
	    if (info->modrm.rm == DISP16_RM)	/* disp16 */
		/* XXX - add symbol lookup */
		(void)sprintf(buf, eai->fmt, segOverride(info, eai->defSeg),
			      disp(info, 2));
	    else
		(void)sprintf(buf, eai->fmt, segOverride(info, eai->defSeg));
	    break;
	case 1:
	case 2:
	    eai = ea16Info(info->modrm);
	    (void)sprintf(buf, eai->fmt, segOverride(info, eai->defSeg),
			  disp(info, info->modrm.mod));
	    break;
	case 3:
	    (void)strcpy(buf, baseName(info));
	    break;
	}
    } else {				/* 32-bit addressing forms */
	switch (info->modrm.mod) {
	case 0:
	    eai = ea32Info(info->modrm);
	    if (info->modrm.rm == SIB_RM)		/* SIB */
		sibEA(info, info->modrm.mod, buf, sizeof (buf));
	    else if (info->modrm.rm == DISP32_RM)	/* disp32 */
		/* XXX - add symbol lookup */
		(void)sprintf(buf, eai->fmt, segOverride(info, eai->defSeg),
			      disp(info, 4));
	    else
		(void)sprintf(buf, eai->fmt, segOverride(info, eai->defSeg));
	    break;
	case 1:
	case 2:
	    if (info->modrm.rm == SIB_RM)		/* SIB */
		sibEA(info, info->modrm.mod, buf, sizeof (buf));
	    else {
		eai = ea32Info(info->modrm);
		(void)sprintf(buf, eai->fmt, segOverride(info, eai->defSeg),
			      disp(info, 3 * info->modrm.mod - 2));
	    }
	    break;
	case 3:
	    (void)strcpy(buf, baseName(info));
	    break;
	}
    }
    return buf;
}

static char *ptrStr(IAinstInfoPtr info)
{
    if (info->modrm.mod != 3)
	switch (info->opSize) {
	case 1:
	    return "byte ptr ";
	case 2:
	    return "word ptr ";
	case 4:
	    return "dword ptr ";
	}
	return "";
    /*NOTREACHED*/
}

void add_ExGx_das(IAinstInfoPtr info, char *str)	/* 00 - 01 */
{
    (void)sprintf(str, MNE"%s, %s", "add", modrmEA(info), regName(info));
}

void add_GxEx_das(IAinstInfoPtr info, char *str)	/* 02 - 03 */
{
    (void)sprintf(str, MNE"%s, %s", "add", regName(info), modrmEA(info));
}

void add_reg_imm_das(IAinstInfoPtr info, char *str)	/* 04 - 05 */
{
    (void)sprintf(str, MNE"%s, %s", "add", regName(info),
		  imm(info, info->opSize, 0));
}

void push_seg_das(IAinstInfoPtr info, char *str)	/* 06 */
{
    (void)sprintf(str, MNE"%s", "push", segName[info->reg]);
}

void pop_seg_das(IAinstInfoPtr info, char *str)		/* 07 */
{
    (void)sprintf(str, MNE"%s", "pop", segName[info->cond_seg]);
}

void or_ExGx_das(IAinstInfoPtr info, char *str)		/* 08 - 09 */
{
    (void)sprintf(str, MNE"%s, %s", "or", modrmEA(info), regName(info));
}

void or_GxEx_das(IAinstInfoPtr info, char *str)		/* 0A - 0B */
{
    (void)sprintf(str, MNE"%s, %s", "or", regName(info), modrmEA(info));
}

void or_reg_imm_das(IAinstInfoPtr info, char *str)	/* 0C - 0D */
{
    (void)sprintf(str, MNE"%s, %s", "or", regName(info),
		  imm(info, info->opSize, HEX));
}

/* push_CS uses push_seg_das (06) */			/* 0E */

/* two-byte escape */					/* 0F */

void adc_ExGx_das(IAinstInfoPtr info, char *str)	/* 10 - 11 */
{
    (void)sprintf(str, MNE"%s, %s", "adc", modrmEA(info), regName(info));
}

void adc_GxEx_das(IAinstInfoPtr info, char *str)	/* 12 - 13 */
{
    (void)sprintf(str, MNE"%s, %s", "adc", regName(info), modrmEA(info));
}

void adc_reg_imm_das(IAinstInfoPtr info, char *str)	/* 14 - 15 */
{
    (void)sprintf(str, MNE"%s, %s", "adc", regName(info),
		  imm(info, info->opSize, 0));
}

/* push_SS uses push_seg_das (06) */			/* 16 */

/* pop_SS uses pop_seg_das (07) */			/* 17 */

void sbb_ExGx_das(IAinstInfoPtr info, char *str)	/* 18 - 19 */
{
    (void)sprintf(str, MNE"%s, %s", "sbb", modrmEA(info), regName(info));
}

void sbb_GxEx_das(IAinstInfoPtr info, char *str)	/* 1A - 1B */
{
    (void)sprintf(str, MNE"%s, %s", "sbb", regName(info), modrmEA(info));
}

void sbb_reg_imm_das(IAinstInfoPtr info, char *str)	/* 1C - 1D */
{
    (void)sprintf(str, MNE"%s, %s", "sbb", regName(info),
		  imm(info, info->opSize, 0));
}

/* push_DS uses push_seg_das (06) */			/* 1E */

/* pop_DS uses pop_seg_das (07) */			/* 1F */

void and_ExGx_das(IAinstInfoPtr info, char *str)	/* 20 - 21 */
{
    (void)sprintf(str, MNE"%s, %s", "and", modrmEA(info), regName(info));
}

void and_GxEx_das(IAinstInfoPtr info, char *str)	/* 22 - 23 */
{
    (void)sprintf(str, MNE"%s, %s", "and", regName(info), modrmEA(info));
}

void and_reg_imm_das(IAinstInfoPtr info, char *str)	/* 24 - 25 */
{
    (void)sprintf(str, MNE"%s, %s", "and", regName(info),
		  imm(info, info->opSize, HEX));
}

/* ES segment override */				/* 26 */

/* ARGSUSED */
void daa_das(IAinstInfoPtr info, char *str)		/* 27 */
{
    (void)sprintf(str, MNE, "daa");
}

void sub_ExGx_das(IAinstInfoPtr info, char *str)	/* 28 - 29 */
{
    (void)sprintf(str, MNE"%s, %s", "sub", modrmEA(info), regName(info));
}

void sub_GxEx_das(IAinstInfoPtr info, char *str)	/* 2A - 2B */
{
    (void)sprintf(str, MNE"%s, %s", "sub", regName(info), modrmEA(info));
}

void sub_reg_imm_das(IAinstInfoPtr info, char *str)	/* 2C - 2D */
{
    (void)sprintf(str, MNE"%s, %s", "sub", regName(info),
		  imm(info, info->opSize, 0));
}

/* CS segment override */				/* 2E */

/* ARGSUSED */
void das_das(IAinstInfoPtr info, char *str)		/* 2F */
{
    (void)sprintf(str, MNE, "das");
}

void xor_ExGx_das(IAinstInfoPtr info, char *str)	/* 30 - 31 */
{
    (void)sprintf(str, MNE"%s, %s", "xor", modrmEA(info), regName(info));
}

void xor_GxEx_das(IAinstInfoPtr info, char *str)	/* 32 - 33 */
{
    (void)sprintf(str, MNE"%s, %s", "xor", regName(info), modrmEA(info));
}

void xor_reg_imm_das(IAinstInfoPtr info, char *str)	/* 34 - 35 */
{
    (void)sprintf(str, MNE"%s, %s", "xor", regName(info),
		  imm(info, info->opSize, HEX));
}

/* SS segment override */				/* 36 */

/* ARGSUSED */
void aaa_das(IAinstInfoPtr info, char *str)		/* 37 */
{
    (void)sprintf(str, MNE, "aaa");
}

void cmp_ExGx_das(IAinstInfoPtr info, char *str)	/* 38 - 39 */
{
    (void)sprintf(str, MNE"%s, %s", "cmp", modrmEA(info), regName(info));
}

void cmp_GxEx_das(IAinstInfoPtr info, char *str)	/* 3A - 3B */
{
    (void)sprintf(str, MNE"%s, %s", "cmp", regName(info), modrmEA(info));
}

void cmp_reg_imm_das(IAinstInfoPtr info, char *str)	/* 3C - 3D */
{
    (void)sprintf(str, MNE"%s, %s", "cmp", regName(info),
		  imm(info, info->opSize, 0));
}

/* ES segment override */				/* 3E */

/* ARGSUSED */
void aas_das(IAinstInfoPtr info, char *str)		/* 3F */
{
    (void)sprintf(str, MNE, "aas");
}

void inc_reg_das(IAinstInfoPtr info, char *str)		/* 40 - 47 */
{
    (void)sprintf(str, MNE"%s", "inc", regName(info));
}

void dec_reg_das(IAinstInfoPtr info, char *str)		/* 48 - 4F */
{
    (void)sprintf(str, MNE"%s", "dec", regName(info));
}

void push_reg_das(IAinstInfoPtr info, char *str)	/* 50 - 57 */
{
    (void)sprintf(str, MNE"%s", "push", regName(info));
}

void pop_reg_das(IAinstInfoPtr info, char *str)		/* 58 - 5F */
{
    (void)sprintf(str, MNE"%s", "pop", regName(info));
}

void pusha_das(IAinstInfoPtr info, char *str)		/* 60 */
{
    (void)sprintf(str, MNE, (info->opSize == IA16) ? "pusha" : "pushad");
}

void popa_das(IAinstInfoPtr info, char *str)		/* 61 */
{
    (void)sprintf(str, MNE, (info->opSize == IA16) ? "popa" : "popad");
}

void bound_GvMa_das(IAinstInfoPtr info, char *str)	/* 62 */
{
    (void)sprintf(str, MNE"%s, %s", "bound", regName(info), modrmEA(info));
}

void arpl_EwGw_das(IAinstInfoPtr info, char *str)	/* 63 */
{
    (void)sprintf(str, MNE"%s, %s", "arpl", modrmEA(info), regName(info));
}

/* FS segment override */				/* 64 */
/* GS segment override */				/* 65 */
/* operand size override */				/* 66 */
/* address size override */				/* 67 */

void push_imm_das(IAinstInfoPtr info, char *str)	/* 68 */
{
    (void)sprintf(str, MNE"%s", "push", imm(info, info->opSize, 0));
}

void imul_GvEvIv_das(IAinstInfoPtr info, char *str)	/* 69 */
{
    if (info->modrm.mod == 3 && info->reg == info->base)    /* 2-operand */
	(void)sprintf(str, MNE"%s, %s", "imul", regName(info),
		      imm(info, info->opSize, 0));
    else
	(void)sprintf(str, MNE"%s, %s, %s", "imul", regName(info),
		      modrmEA(info), imm(info, info->opSize, 0));
}

/* push_Ib uses push_imm_das (68) */			/* 6A */

void imul_GvEvIb_das(IAinstInfoPtr info, char *str)	/* 6B */
{
    if (info->modrm.mod == 3 && info->reg == info->base)    /* 2-operand */
	(void)sprintf(str, MNE"%s, %s", "imul", regName(info),
		      imm(info, 1, 0));
    else
	(void)sprintf(str, MNE"%s, %s, %s", "imul", regName(info),
		      modrmEA(info), imm(info, 1, 0));
}

void ins_YxDX_das(IAinstInfoPtr info, char *str)	/* 6C - 6D */
{
    (void)sprintf(str, MNE"%s%s, %s", rep(info, "ins"),
		  ptrStr(info), rName(EDI_REG, info->addrSize),
		  rName(EDX_REG, IA16));
}

/* ARGSUSED */
void outs_DXXx_das(IAinstInfoPtr info, char *str)	/* 6E - 6F */
{
    (void)sprintf(str, MNE"%s, %s%s%s", rep(info, "outs"),
		  rName(EDX_REG, IA16),
		  ptrStr(info), segOverride(info, DS_ID),
		  rName(EDI_REG, info->addrSize));
}

void jcc_das(IAinstInfoPtr info, char *str)		/* 70 - 7F */
{
    static char *mne[16] = {
	"jo", "jno", "jb", "jnb", "jz", "jnz", "jbe", "jnbe",
	"js", "jns", "jp", "jnp", "jl", "jnl", "jle", "jnle"
    };

    (void)sprintf(str, MNE"%s", mne[info->cond_seg], ofs(info, info->opSize));
}

void group1_das(IAinstInfoPtr info, char *str)		/* 80 */
{
    static char *mne[8] = {
	"add", "or", "adc", "sbb", "and", "sub", "xor", "cmp"
    };
    unsigned hex = 0;

    switch (info->modrm.reg_op) {
    case 1:		/* or  */
    case 4:		/* and */
    case 6:		/* xor */
	hex = HEX;
	break;
    }

    (void)sprintf(str, MNE"%s%s, %s", mne[info->modrm.reg_op],
		  ptrStr(info), modrmEA(info), imm(info, info->opSize, hex));
}

/* group1_EvIv uses group1_das (80) */			/* 81 */

void movb_ALIb_das(IAinstInfoPtr info, char *str)	/* 82 */
{
    (void)sprintf(str, MNE, "** movb_ALIb **");
}

/* group1_EvIb uses group1_das (80) */			/* 83 */

void test_ExGx_das(IAinstInfoPtr info, char *str)	/* 84 - 85 */
{
    (void)sprintf(str, MNE"%s, %s", "test", modrmEA(info), regName(info));
}

void xchg_ExGx_das(IAinstInfoPtr info, char *str)	/* 86 - 87 */
{
    (void)sprintf(str, MNE"%s, %s", "xchg", modrmEA(info), regName(info));
}

void mov_ExGx_das(IAinstInfoPtr info, char *str)	/* 88 - 89 */
{
    (void)sprintf(str, MNE"%s, %s", "mov", modrmEA(info), regName(info));
}

void mov_GxEx_das(IAinstInfoPtr info, char *str)	/* 8A - 8B */
{
    (void)sprintf(str, MNE"%s, %s", "mov", regName(info), modrmEA(info));
}

void mov_EwSw_das(IAinstInfoPtr info, char *str)	/* 8C */
{
    (void)sprintf(str, MNE"%s, %s", "mov", modrmEA(info), segName[info->reg]);
}

void lea_GvM_das(IAinstInfoPtr info, char *str)		/* 8D */
{
    (void)sprintf(str, MNE"%s, %s", "lea", regName(info), modrmEA(info));
}

void mov_SwEw_das(IAinstInfoPtr info, char *str)	/* 8E */
{
    (void)sprintf(str, MNE"%s, %s", "mov", segName[info->cond_seg],
		  modrmEA(info));
}

void pop_Ev_das(IAinstInfoPtr info, char *str)		/* 8F */
{
    (void)sprintf(str, MNE"%s", "pop", modrmEA(info));
}

/* ARGSUSED */
void nop_das(IAinstInfoPtr info, char *str)		/* 90 */
{
    (void)sprintf(str, MNE, "nop");
}

void xchg_reg_reg_das(IAinstInfoPtr info, char *str)	/* 91 - 97 */
{
    (void)sprintf(str, MNE"%s, %s", "xchg", regName(info), baseName(info));
}

void cbw_das(IAinstInfoPtr info, char *str)		/* 98 */
{
    (void)sprintf(str, MNE, (info->opSize == IA16) ? "cbw" : "cwde");
}

void cwd_das(IAinstInfoPtr info, char *str)		/* 99 */
{
    (void)sprintf(str, MNE, (info->opSize == IA16) ? "cwd" : "cdq");
}

void call_Ap_das(IAinstInfoPtr info, char *str)		/* 9A */
{
    (void)sprintf(str, MNE"%s", "call", segOfs(info, info->opSize));
}

/* ARGSUSED */
void wait_das(IAinstInfoPtr info, char *str)		/* 9B */
{
    (void)sprintf(str, MNE, "wait");
}

void pushf_Fv_das(IAinstInfoPtr info, char *str)	/* 9C */
{
    (void)sprintf(str, MNE, (info->opSize == IA16) ? "pushf" : "pushfd");
}

void popf_Fv_das(IAinstInfoPtr info, char *str)		/* 9D */
{
    (void)sprintf(str, MNE, (info->opSize == IA16) ? "popf" : "popfd");
}

/* ARGSUSED */
void sahf_das(IAinstInfoPtr info, char *str)		/* 9E */
{
    (void)sprintf(str, MNE, "sahf");
}

/* ARGSUSED */
void lahf_das(IAinstInfoPtr info, char *str)		/* 9F */
{
    (void)sprintf(str, MNE, "lahf");
}

void mov_reg_Ox_das(IAinstInfoPtr info, char *str)	/* A0 - A1 */
{
    (void)sprintf(str, MNE"%s, %s[%s]", "mov", regName(info),
		  segOverride(info, DS_ID), disp(info, info->addrSize));
}

void mov_Ox_reg_das(IAinstInfoPtr info, char *str)	/* A2 - A3 */
{
    (void)sprintf(str, MNE"%s[%s], %s", "mov", segOverride(info, DS_ID),
		  disp(info, info->addrSize), regName(info));
}

void movs_YxXx_das(IAinstInfoPtr info, char *str)	/* A4 - A5 */
{
    (void)sprintf(str, MNE"%s%s:%s, %s%s", rep(info, "movs"),
		  ptrStr(info),
		  segRegName(ES_ID), rName(EDI_REG, info->addrSize),
		  segOverride(info, DS_ID), rName(ESI_REG, info->addrSize));
}

void cmps_XxYx_das(IAinstInfoPtr info, char *str)	/* A6 - A7 */
{
    (void)sprintf(str, MNE"%s%s%s, %s:%s", rep(info, "cmps"),
		  ptrStr(info),
		  segOverride(info, DS_ID), rName(ESI_REG, info->addrSize),
		  segRegName(ES_ID), rName(EDI_REG, info->addrSize));
}

void test_reg_imm_das(IAinstInfoPtr info, char *str)	/* A8 - A9 */
{
    (void)sprintf(str, MNE"%s, %s", "test", regName(info),
		  imm(info, info->opSize, HEX));
}

void stos_Yx_reg_das(IAinstInfoPtr info, char *str)	/* AA - AB */
{
    (void)sprintf(str, MNE"%s%s:%s", rep(info, "stos"),
		  ptrStr(info),
		  segRegName(ES_ID), rName(EDI_REG, info->addrSize));
}

void lods_reg_Xx_das(IAinstInfoPtr info, char *str)	/* AC - AD */
{
    (void)sprintf(str, MNE"%s%s%s", rep(info, "lods"),
		  ptrStr(info),
		  segOverride(info, DS_ID), rName(ESI_REG, info->addrSize));
}

void scas_reg_Yx_das(IAinstInfoPtr info, char *str)	/* AE - AF */
{
    (void)sprintf(str, MNE"%s%s:%s", rep(info, "scas"),
		  ptrStr(info),
		  segRegName(ES_ID), rName(EDI_REG, info->addrSize));
}

void mov_reg_imm_das(IAinstInfoPtr info, char *str)	/* B0 - BF */
{
    (void)sprintf(str, MNE"%s, %s", "mov", regName(info),
		  imm(info, info->opSize, 0));
}

void group2_ExIb_das(IAinstInfoPtr info, char *str)	/* C0 - C1 */
{
    static char *mne[8] = {
	"rol", "ror", "rcl", "rcr", "shl", "shr", "reserved", "sar"
    };

    (void)sprintf(str, MNE"%s%s, %s", mne[info->modrm.reg_op],
		  ptrStr(info), modrmEA(info), imm(info, info->opSize, 0));
}

void ret_near_Iw_das(IAinstInfoPtr info, char *str)	/* C2 */
{
    (void)sprintf(str, MNE"%s", "ret", imm(info, 2, 0));
}

/* ARGSUSED */
void ret_near_das(IAinstInfoPtr info, char *str)	/* C3 */
{
    (void)sprintf(str, MNE, "ret");
}

void load_far_ptr_das(IAinstInfoPtr info, char *str)	/* C4 */
{
    static char *mne[6] = {
	"les", "l**", "lss", "lds", "lfs", "lgs"
    };

    (void)sprintf(str, MNE"%s, %s", mne[info->cond_seg], regName(info),
		  modrmEA(info));
}

/* lds_GvMp uses load_far_ptr_das (C4) */		/* C5 */

void mov_ExIx_das(IAinstInfoPtr info, char *str)	/* C6 - C7 */
{
    (void)sprintf(str, MNE"%s%s, %s", "mov", ptrStr(info), modrmEA(info),
		  imm(info, info->opSize, 0));
}

void enter_IwIb_das(IAinstInfoPtr info, char *str)	/* C8 */
{
    (void)sprintf(str, MNE"%s, %u", "enter", imm(info, 2, 0), info->disp32);
}

/* ARGSUSED */
void leave_das(IAinstInfoPtr info, char *str)		/* C9 */
{
    (void)sprintf(str, MNE, "leave");
}

void ret_far_Iw_das(IAinstInfoPtr info, char *str)	/* CA */
{
    (void)sprintf(str, MNE"%s", "retf", imm(info, 2, 0));
}

/* ARGSUSED */
void ret_far_das(IAinstInfoPtr info, char *str)		/* CB */
{
    (void)sprintf(str, MNE, "retf");
}

/* int3 uses int_Ib_das (CD) */				/* CC */

void int_Ib_das(IAinstInfoPtr info, char *str)		/* CD */
{
    (void)sprintf(str, MNE"%s", "int", imm(info, 1, HEX));
}

/* ARGSUSED */
void into_das(IAinstInfoPtr info, char *str)		/* CE */
{
    (void)sprintf(str, MNE, "into");
}

void iret_das(IAinstInfoPtr info, char *str)		/* CF */
{
    (void)sprintf(str, MNE, (info->opSize == IA16) ? "iret" : "iretd");
}

/* group2_Eb1 uses group2_ExIb_das (C0 - C1) */		/* D0 */

/* group2_Ev1 uses group2_ExIb_das (C0 - C1) */		/* D1 */

void group2_ExCL_das(IAinstInfoPtr info, char *str)	/* D2 - D3 */
{
    static char *mne[8] = {
	"rol", "ror", "rcl", "rcr", "shl", "shr", "reserved", "sar"
    };

    (void)sprintf(str, MNE"%s, cl", mne[info->modrm.reg_op], modrmEA(info));
}

/* ARGSUSED */
void aam_Ib_das(IAinstInfoPtr info, char *str)		/* D4 */
{
    if (info->imm32 == 10)
	(void)sprintf(str, MNE, "aam");
    else
	(void)sprintf(str, MNE"%s", "aam", imm(info, 1, 0));
}

/* ARGSUSED */
void aad_Ib_das(IAinstInfoPtr info, char *str)		/* D5 */
{
    if (info->imm32 == 10)
	(void)sprintf(str, MNE, "aad");
    else
	(void)sprintf(str, MNE"%s", "aad", imm(info, 1, 0));
}

/* salc is an undocumented opcode */
void salc_das(IAinstInfoPtr info, char *str)		/* D6 */
{
    (void)sprintf(str, MNE, "** salc **");
}

void xlat_das(IAinstInfoPtr info, char *str)		/* D7 */
{
    (void)sprintf(str, MNE"%s", "xlat", segOverride(info, DS_ID));
}

void esc_D8_das(IAinstInfoPtr info, char *str)		/* D8 */
{
    (void)sprintf(str, MNE, "** esc_D8 **");
}

void esc_D9_das(IAinstInfoPtr info, char *str)		/* D9 */
{
    (void)sprintf(str, MNE, "** esc_D9 **");
}

void esc_DA_das(IAinstInfoPtr info, char *str)		/* DA */
{
    (void)sprintf(str, MNE, "** esc_DA **");
}

void esc_DB_das(IAinstInfoPtr info, char *str)		/* DB */
{
    (void)sprintf(str, MNE, "** esc_DB **");
}

void esc_DC_das(IAinstInfoPtr info, char *str)		/* DC */
{
    (void)sprintf(str, MNE, "** esc_DC **");
}

void esc_DD_das(IAinstInfoPtr info, char *str)		/* DD */
{
    (void)sprintf(str, MNE, "** esc_DD **");
}

void esc_DE_das(IAinstInfoPtr info, char *str)		/* DE */
{
    (void)sprintf(str, MNE, "** esc_DE **");
}

void esc_DF_das(IAinstInfoPtr info, char *str)		/* DF */
{
    (void)sprintf(str, MNE, "** esc_DF **");
}

void loop_das(IAinstInfoPtr info, char *str)		/* E0 - E2 */
{
    static char *mne[6] = {
	"loopnzw", "loopzw", "loopw",
	"loopnzd", "loopzd", "loopd"
    };

    (void)sprintf(str, MNE"%s", mne[(info->opSize-2)/2 + info->cond_seg],
		  ofs(info, 1));
}

void jcxz_Jb_das(IAinstInfoPtr info, char *str)		/* E3 */
{
    (void)sprintf(str, MNE"%s", (info->opSize == IA16) ? "jcxz" : "jecxz",
		  ofs(info, info->opSize));
}

void in_reg_imm_das(IAinstInfoPtr info, char *str)	/* E4 - E5 */
{
    (void)sprintf(str, MNE"%s, %s", "in", regName(info),
		  imm(info, info->opSize, HEX));
}

void out_imm_reg_das(IAinstInfoPtr info, char *str)	/* E6 - E7 */
{
    (void)sprintf(str, MNE"%s, %s", "out", imm(info, info->opSize, HEX),
		  regName(info));
}

void call_Jv_das(IAinstInfoPtr info, char *str)		/* E8 */
{
    (void)sprintf(str, MNE"%s", "call", ofs(info, info->opSize));
}

void jmp_Jv_das(IAinstInfoPtr info, char *str)		/* E9 */
{
    (void)sprintf(str, MNE"%s", "jmp", ofs(info, info->opSize));
}

void jmp_Ap_das(IAinstInfoPtr info, char *str)		/* EA */
{
    (void)sprintf(str, MNE"%s", "jmp", segOfs(info, info->opSize));
}

void jmp_Jb_das(IAinstInfoPtr info, char *str)		/* EB */
{
    (void)sprintf(str, MNE"%s", "jmp", ofs(info, info->opSize));
}

void in_reg_DX_das(IAinstInfoPtr info, char *str)	/* EC - ED */
{
    (void)sprintf(str, MNE"%s, dx", "in", regName(info));
}

void out_DX_reg_das(IAinstInfoPtr info, char *str)	/* EE - EF */
{
    (void)sprintf(str, MNE"dx, %s", "out", regName(info));
}

/* ARGSUSED */
void lock_das(IAinstInfoPtr info, char *str)		/* F0 */
{
    (void)sprintf(str, MNE, "lock");
}

/* ARGSUSED */
void reserved_das(IAinstInfoPtr info, char *str)	/* F1 */
{
    (void)sprintf(str, MNE, "reserved");
}

/* repne prefix */					/* F2 */

/* rep prefix */					/* F3 */

/* ARGSUSED */
void hlt_das(IAinstInfoPtr info, char *str)		/* F4 */
{
    (void)sprintf(str, MNE, "hlt");
}

/* ARGSUSED */
void cmc_das(IAinstInfoPtr info, char *str)		/* F5 */
{
    (void)sprintf(str, MNE, "cmc");
}

void group3_das(IAinstInfoPtr info, char *str)		/* F6 - F7 */
{
    static char *mne[8] = {
	"test", "reserved", "not", "neg", "mul", "imul", "div", "idiv"
    };

    if (info->modrm.reg_op == 0)	/* test */
	(void)sprintf(str, MNE"%s%s, %s", mne[info->modrm.reg_op],
		      ptrStr(info), modrmEA(info),
		      imm(info, info->opSize, HEX));
    else
	(void)sprintf(str, MNE"%s%s", mne[info->modrm.reg_op],
		      ptrStr(info), modrmEA(info));
}

/* ARGSUSED */
void clc_das(IAinstInfoPtr info, char *str)		/* F8 */
{
    (void)sprintf(str, MNE, "clc");
}

/* ARGSUSED */
void stc_das(IAinstInfoPtr info, char *str)		/* F9 */
{
    (void)sprintf(str, MNE, "stc");
}

/* ARGSUSED */
void cli_das(IAinstInfoPtr info, char *str)		/* FA */
{
    (void)sprintf(str, MNE, "cli");
}

/* ARGSUSED */
void sti_das(IAinstInfoPtr info, char *str)		/* FB */
{
    (void)sprintf(str, MNE, "sti");
}

/* ARGSUSED */
void cld_das(IAinstInfoPtr info, char *str)		/* FC */
{
    (void)sprintf(str, MNE, "cld");
}

/* ARGSUSED */
void std_das(IAinstInfoPtr info, char *str)		/* FD */
{
    (void)sprintf(str, MNE, "std");
}

void group4_das(IAinstInfoPtr info, char *str)		/* FE */
{
    static char *mne[8] = {
	"inc", "dec", "reserved", "reserved",
	"reserved", "reserved", "reserved", "reserved"
    };

    (void)sprintf(str, MNE"%s%s", mne[info->modrm.reg_op], ptrStr(info),
		  modrmEA(info));
}

void group5_das(IAinstInfoPtr info, char *str)		/* FF */
{
    static char *mne[8] = {
	"inc", "dec", "call", "call", "jmp", "jmp", "push", "reserved"
    };

    if (info->modrm.reg_op == 3 || info->modrm.reg_op == 5) /* far call/jmp */
	(void)sprintf(str, MNE"far %s%s", mne[info->modrm.reg_op],
		      ptrStr(info), modrmEA(info));
    else
	(void)sprintf(str, MNE"%s%s", mne[info->modrm.reg_op], ptrStr(info),
		      modrmEA(info));
}

void group6_das(IAinstInfoPtr info, char *str)		/* 0F,00 */
{
    static char *mne[8] = {
	"sldt", "str", "lldt", "ltr", "verr", "verw", "jmpe", "reserved"
    };

    (void)sprintf(str, MNE"%s", mne[info->modrm.reg_op], modrmEA(info));
}

void group7_das(IAinstInfoPtr info, char *str)		/* 0F,01 */
{
    static char *mne[8] = {
	"sgdt", "sidt", "lgdt", "lidt", "smsw", "reserved", "lmsw", "invlpg"
    };

    (void)sprintf(str, MNE"%s", mne[info->modrm.reg_op], modrmEA(info));
}

void lar_GvEw_das(IAinstInfoPtr info, char *str)	/* 0F,02 */
{
    (void)sprintf(str, MNE"%s, %s", "lar", regName(info), modrmEA(info));
}

void lsl_GvEw_das(IAinstInfoPtr info, char *str)	/* 0F,03 */
{
    (void)sprintf(str, MNE"%s, %s", "lsl", regName(info), modrmEA(info));
}

/* ARGSUSED */
void clts_das(IAinstInfoPtr info, char *str)		/* 0F,06 */
{
    (void)sprintf(str, MNE, "clts");
}

/* ARGSUSED */
void invd_das(IAinstInfoPtr info, char *str)		/* 0F,08 */
{
    (void)sprintf(str, MNE, "invd");
}

/* ARGSUSED */
void wbinvd_das(IAinstInfoPtr info, char *str)		/* 0F,09 */
{
    (void)sprintf(str, MNE, "wbinvd");
}

/* ARGSUSED */
void ud2_das(IAinstInfoPtr info, char *str)		/* 0F,0B */
{
    (void)sprintf(str, MNE, "ud2");
}

void mov_RdCd_das(IAinstInfoPtr info, char *str)	/* 0F,20 */
{
    (void)sprintf(str, MNE"%s, cr%d", "mov", modrmEA(info), info->reg);
}

void mov_RdDd_das(IAinstInfoPtr info, char *str)	/* 0F,21 */
{
    (void)sprintf(str, MNE"%s, dr%d", "mov", modrmEA(info), info->reg);
}

void mov_CdRd_das(IAinstInfoPtr info, char *str)	/* 0F,22 */
{
    (void)sprintf(str, MNE"cr%d, %s", "mov", info->reg, modrmEA(info));
}

void mov_DdRd_das(IAinstInfoPtr info, char *str)	/* 0F,23 */
{
    (void)sprintf(str, MNE"dr%d, %s", "mov", info->reg, modrmEA(info));
}

void mov_RdTd_das(IAinstInfoPtr info, char *str)	/* 0F,24 */
{
    (void)sprintf(str, MNE"%s, tr%d", "mov", modrmEA(info), info->reg);
}

void mov_TdRd_das(IAinstInfoPtr info, char *str)	/* 0F,26 */
{
    (void)sprintf(str, MNE"tr%d, %s", "mov", info->reg, modrmEA(info));
}

void wrmsr_das(IAinstInfoPtr info, char *str)		/* 0F,30 */
{
    (void)sprintf(str, MNE, "wrmsr");
}

void rdtsc_das(IAinstInfoPtr info, char *str)		/* 0F,31 */
{
    (void)sprintf(str, MNE, "rdtsc");
}

void rdmsr_das(IAinstInfoPtr info, char *str)		/* 0F,32 */
{
    (void)sprintf(str, MNE, "rdmsr");
}

void rdpmc_das(IAinstInfoPtr info, char *str)		/* 0F,33 */
{
    (void)sprintf(str, MNE, "rdpmc");
}

void cmovcc_das(IAinstInfoPtr info, char *str)		/* 0F,40 - 0F,4F */
{
    static char *mne[16] = {
	"cmovo", "cmovno", "cmovb",  "cmovnb",
	"cmovz", "cmovnz", "cmovbe", "cmovnbe",
	"cmovs", "cmovns", "cmovp",  "cmovnp",
	"cmovl", "cmovnl", "cmovle", "cmovnle"
    };

    (void)sprintf(str, MNE"%s, %s", mne[info->cond_seg], regName(info),
		  modrmEA(info));
}

/* uses jcc_das (70 - 7F) */				/* 0F,80 - 0F,8F */

void setcc_das(IAinstInfoPtr info, char *str)		/* 0F,90 - 0F,9F */
{
    static char *mne[16] = {
	"seto", "setno", "setb", "setnb", "setz", "setnz", "setbe", "setnbe",
	"sets", "setns", "setp", "setnp", "setl", "setnl", "setle", "setnle"
    };

    (void)sprintf(str, MNE"%s%s", mne[info->cond_seg], ptrStr(info),
		  modrmEA(info));
}

/* push_FS uses push_seg_das (06) */			/* 0F,A0 */

/* pop_FS uses pop_seg_das (07) */			/* 0F,A1 */

/* ARGSUSED */
void cpuid_das(IAinstInfoPtr info, char *str)		/* 0F,A2 */
{
    (void)sprintf(str, MNE, "cpuid");
}

void bt_EvGv_das(IAinstInfoPtr info, char *str)		/* 0F,A3 */
{
    (void)sprintf(str, MNE"%s, %s", "bt", modrmEA(info), regName(info));
}

void shld_EvGvIb_das(IAinstInfoPtr info, char *str)	/* 0F,A4 */
{
    (void)sprintf(str, MNE"%s, %s, %s", "shld", modrmEA(info), regName(info),
		  imm(info, 1, 0));
}

void shld_EvGvCL_das(IAinstInfoPtr info, char *str)	/* 0F,A5 */
{
    (void)sprintf(str, MNE"%s, %s, cl", "shld", modrmEA(info), regName(info));
}

void cmpxchg_XBTS_das(IAinstInfoPtr info, char *str)	/* 0F,A6 */
{
    (void)sprintf(str, MNE, "** cmpxchg_XBTS **");
}

void cmpxchg_IBTS_das(IAinstInfoPtr info, char *str)	/* 0F,A7 */
{
    (void)sprintf(str, MNE, "** cmpxchg_IBTS **");
}

/* push_GS uses push_seg_das (06) */			/* 0F,A8 */

/* pop_GS uses pop_seg_das (07) */			/* 0F,A9 */

/* ARGSUSED */
void rsm_das(IAinstInfoPtr info, char *str)		/* 0F,AA */
{
    (void)sprintf(str, MNE, "rsm");
}

void bts_EvGv_das(IAinstInfoPtr info, char *str)	/* 0F,AB */
{
    (void)sprintf(str, MNE"%s, %s", "bts", modrmEA(info), regName(info));
}

void shrd_EvGvIb_das(IAinstInfoPtr info, char *str)	/* 0F,AC */
{
    (void)sprintf(str, MNE"%s, %s, %s", "shrd", modrmEA(info), regName(info),
		  imm(info, 1, 0));
}

void shrd_EvGvCL_das(IAinstInfoPtr info, char *str)	/* 0F,AD */
{
    (void)sprintf(str, MNE"%s, %s, cl", "shrd", modrmEA(info), regName(info));
}

void imul_GvEv_das(IAinstInfoPtr info, char *str)	/* 0F,AF */
{
    (void)sprintf(str, MNE"%s, %s", "imul", regName(info), modrmEA(info));
}

void cmpxchg_ExGx_das(IAinstInfoPtr info, char *str)	/* 0F,B0 - 0F,B1 */
{
    (void)sprintf(str, MNE"%s, %s", "cmpxchg", modrmEA(info), regName(info));
}

/* lss_GvMp uses load_far_ptr_das (C4) */		/* 0F,B2 */

void btr_EvGv_das(IAinstInfoPtr info, char *str)	/* 0F,B3 */
{
    (void)sprintf(str, MNE"%s, %s", "btr", modrmEA(info), regName(info));
}

/* lfs_GvMp uses load_far_ptr_das (C4) */		/* 0F,B4 */

/* lgs_GvMp uses load_far_ptr_das (C4) */		/* 0F,B5 */

void movzx_GvEb_das(IAinstInfoPtr info, char *str)	/* 0F,B6 */
{
    char *reg = regName(info);
    BYTE opSize = info->opSize;		/* save */

    /* force opSize to 1 to get correct EA disassembly */
    info->opSize = 1;
    (void)sprintf(str, MNE"%s, %s%s", "movzx", reg, ptrStr(info),
		  modrmEA(info));
    info->opSize = opSize;		/* restore */
}

void movzx_GvEw_das(IAinstInfoPtr info, char *str)	/* 0F,B7 */
{
    char *reg = regName(info);
    BYTE opSize = info->opSize;		/* save */

    /* force opSize to 2 to get correct EA disassembly */
    info->opSize = 2;
    (void)sprintf(str, MNE"%s, %s%s", "movzx", reg, ptrStr(info),
		  modrmEA(info));
    info->opSize = opSize;		/* restore */
}

void jmpe_Jv_das(IAinstInfoPtr info, char *str)		/* 0F,B8 */
{
    (void)sprintf(str, MNE"%s", "jmpe", ofs(info, info->opSize));
}

void group8_das(IAinstInfoPtr info, char *str)		/* 0F,BA */
{
    static char *mne[8] = {
	"reserved", "reserved", "reserved", "reserved",
	"bt", "bts", "btr", "btc"
    };

    (void)sprintf(str, MNE"%s, %s", mne[info->modrm.reg_op], modrmEA(info),
		  imm(info, 1, 0));
}

void btc_EvGv_das(IAinstInfoPtr info, char *str)	/* 0F,BB */
{
    (void)sprintf(str, MNE"%s, %s", "btc", modrmEA(info), regName(info));
}

void bsf_GvEv_das(IAinstInfoPtr info, char *str)	/* 0F,BC */
{
    (void)sprintf(str, MNE"%s, %s", "bsf", regName(info), modrmEA(info));
}

void bsr_GvEv_das(IAinstInfoPtr info, char *str)	/* 0F,BD */
{
    (void)sprintf(str, MNE"%s, %s", "bsr", regName(info), modrmEA(info));
}

void movsx_GvEb_das(IAinstInfoPtr info, char *str)	/* 0F,BE */
{
    char *reg = regName(info);
    BYTE opSize = info->opSize;		/* save */

    /* force opSize to 1 to get correct EA disassembly */
    info->opSize = 1;
    (void)sprintf(str, MNE"%s, %s%s", "movsx", reg, ptrStr(info),
		  modrmEA(info));
    info->opSize = opSize;		/* restore */
}

void movsx_GvEw_das(IAinstInfoPtr info, char *str)	/* 0F,BF */
{
    char *reg = regName(info);
    BYTE opSize = info->opSize;		/* save */

    /* force opSize to 2 to get correct EA disassembly */
    info->opSize = 2;
    (void)sprintf(str, MNE"%s, %s%s", "movsx", reg, ptrStr(info),
		  modrmEA(info));
    info->opSize = opSize;		/* restore */
}

void xadd_ExGx_das(IAinstInfoPtr info, char *str)	/* 0F,C0 - 0F,C1 */
{
    (void)sprintf(str, MNE"%s, %s", "xadd", modrmEA(info), regName(info));
}

void group9_das(IAinstInfoPtr info, char *str)		/* 0F,C7 */
{
    static char *mne[8] = {
	"reserved", "cmpxchg8b", "reserved", "reserved",
	"reserved", "reserved",  "reserved", "reserved"
    };

    (void)sprintf(str, MNE"%s", mne[info->modrm.reg_op], modrmEA(info));
}

void bswap_reg_das(IAinstInfoPtr info, char *str)	/* 0F,C8 - 0F,CF */
{
    (void)sprintf(str, MNE"%s", "bswap", regName(info));
}
