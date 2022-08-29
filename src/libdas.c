/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Disassembly Library Routines
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

#include <ctype.h>
#include <string.h>

#include "EMInst.h"
#include "std.h"
#include "bits.h"
#include "types.h"
#include "das_instr.h"
#include "decoder.h"
#include "encoder.h"
#include "libdas.h"
#include "libsym.h"
#include "ski_elf.h"

#ifndef R_IA_64_IMM14
#define R_IA_64_IMM14           R_IA_64_IMM15
#endif

#define MAX_DAS_DECIMAL		 255
#define MIN_DAS_DECIMAL		-256

ADDR dasAddress();
void symAddrtoDefFunc(ADDR, char[], int, int, int);
/* default disassembly parameters - can be changed by dasInit() */
static DasOptions dasOpts = DasTemplate | DasTemplateCommas | DasPseudoOps
				| DasRegNames;

static unsigned ins, locs, outs, rots;
static char targetStr[MAX_DAS_LEN], regStr[10];
static size_t dasWidth = 80 - 21, trailLen = 5, templLen = 5;
static size_t formatLen = 0, commentLen = 0, sepLen = 0, addrLen = 0;

static const char *dasSep = ",", *dasEqualSep = "=";
static int symLen;

static const char *arStr[] = {
    ".k0",     ".k1",  ".k2",       ".k3",   ".k4",   ".k5",  ".k6",  ".k7",
    "8",       "9",    "10",        "11",    "12",    "13",   "14",   "15",
    ".rsc",    ".bsp", ".bspstore", ".rnat", "20",    ".fcr", "22",   "23",
    ".eflags", ".csd", ".ssd",      ".cflg", ".fsr",  ".fir", ".fdr", "31",
    ".ccv",    "33",   "34",        "35",    ".unat", "37",   "38",   "39",
    ".fpsr",   "41",   "42",        "43",    ".itc",  "45",   "46",   "47",
    "48",      "49",   "50",        "51",    "52",    "53",   "54",   "55",
    "56",      "57",   "58",        "59",    "60",    "61",   "62",   "63",
    ".pfs",    ".lc",  ".ec",       "67",    "68",    "69",   "70",   "71",
    "72",      "73",   "74",        "75",    "76",    "77",   "78",   "79",
    "80",      "81",   "82",        "83",    "84",    "85",   "86",   "87",
    "88",      "89",   "90",        "91",    "92",    "93",   "94",   "95",
    "96",      "97",   "98",        "99",    "100",   "101",  "102",  "103",
    "104",     "105",  "106",       "107",   "108",   "109",  "110",  "111",
    "112",     "113",  "114",       "115",   "116",   "117",  "118",  "119",
    "120",     "121",  "122",       "123",   "124",   "125",  "126",  "127"
};

static const char *crStr[] = {
    ".dcr",  ".itm",  ".iva",  "3",    "4",     "5",     "6",     "7",
    ".pta",  ".gpta", "10",    "11",   "12",    "13",    "14",    "15",
    ".ipsr", ".isr",  "18",    ".iip", ".ifa",  ".itir", ".iipa", ".ifs",
    ".iim",  ".iha",  "26",    "27",   "28",    "29",    "30",    "31",
    "32",    "33",    "34",    "35",   "36",    "37",    "38",    "39",
    "40",    "41",    "42",    "43",   "44",    "45",    "46",    "47",
    "48",    "49",    "50",    "51",   "52",    "53",    "54",    "55",
    "56",    "57",    "58",    "59",   "60",    "61",    "62",    "63",
    ".lid",  ".ivr",  ".tpr",  ".eoi", ".irr0", ".irr1", ".irr2", ".irr3",
    ".itv",  ".pmv",  ".cmcv", "75",   "76",    "77",    "78",    "79",
    ".lrr0", ".lrr1", "82",    "83",   "84",    "85",    "86",    "87",
    "88",    "89",    "90",    "91",   "92",    "93",    "94",    "95",
    "96",    "97",    "98",    "99",   "100",   "101",   "102",   "103",
    "104",   "105",   "106",   "107",  "108",   "109",   "110",   "111",
    "112",   "113",   "114",   "115",  "116",   "117",   "118",   "119",
    "120",   "121",   "122",   "123",  "124",   "125",   "126",   "127"
};

static const char *mbtype4Str[] = {
    "@brcst", "0x1",   "0x2",  "0x3",  "0x4", "0x5", "0x6", "0x7",
    "@mix",   "@shuf", "@alt", "@rev", "0xc", "0xd", "0xe", "0xf"
};

#define MNEM_LEN 16

/* specialized sprintf, equivalent to sprintf(s, "%-*s ", MNEM_LEN, mnem) */
static int sprint_mnem(char *s, const char *mnem)
{
    int len = strlen(mnem);

    (void)strcpy(s, mnem);
    if (len >= MNEM_LEN)
	*(s + len) = ' ';
    else {
	(void)memset(s + len, ' ', MNEM_LEN - len + 1);
	len = MNEM_LEN;
    }

    *(s + ++len) = '\0';
    return len;
}

/* specialized sprintf, equivalent to sprintf(s, "<str1>%s<str3>", str2) */
static int sprint_s(char *s, const char *str1, const char *str2,
		    const char *str3)
{
    int len = 0;

    if (str1) {
	(void)strcpy(s, str1);
	len += strlen(str1);
    }
    if (str2) {
	(void)strcpy(s + len, str2);
	len += strlen(str2);
    }
    if (str3) {
	(void)strcpy(s + len, str3);
	len += strlen(str3);
    }
    return len;
}

#define MAX_DIGITS 10

/* specialized sprintf, equivalent to sprintf(s, "<str1>%u<str2>", num) */
static int sprint_u(char *s, const char *str1, unsigned int num,
		    const char *str2)
{
    int len = 0;

    if (str1) {
	(void)strcpy(s, str1);
	len += strlen(str1);
    }
    if (num <= 9) {	/* single digit */
	*(s + len) = (char)num + '0';
	len++;
    } else {		/* multiple digits */
	char buf[MAX_DIGITS], *bufp = &buf[MAX_DIGITS];
	unsigned bufLen = 0;

	do {
	    *(--bufp) = (char)(num % 10) + '0';
	    bufLen++;
	} while ((num /= 10) != 0);
	(void)memcpy(s + len, bufp, bufLen);
	len += bufLen;
    }
    if (str2) {
	(void)strcpy(s + len, str2);
	len += strlen(str2);
    } else
	*(s + len) = '\0';
    return len;
}

static char *grName(char *grstr, unsigned reg)
{
    if (dasOpts & DasRegNames) {
	if (reg == 1)
	    (void)strcpy(grstr, "gp");
	else if (reg == 12)
	    (void)strcpy(grstr, "sp");
	else if (reg >= 8 && reg <= 11)
	    (void)sprint_u(grstr, "ret", reg - 8, 0);
	else if (reg == 13)
	    (void)strcpy(grstr, "tp");
	else if ((dasOpts & DasStackedRegNames) == DasStackedRegNames) {
	    if (reg >= 32 && reg < (32 + ins))
		(void)sprint_u(grstr, "in", reg - 32, 0);
	    else if (reg >= (32 + ins) && reg < (32 + ins + locs))
		(void)sprint_u(grstr, "loc", reg - (32 + ins), 0);
	    else if (reg >= (32 + ins + locs) &&
		     reg < (32 + ins + locs + outs))
		(void)sprint_u(grstr, "out", reg - (32 + ins + locs), 0);
	    else
		(void)sprint_u(grstr, "r", reg, 0);
	} else
	    (void)sprint_u(grstr, "r", reg, 0);
    } else
	(void)sprint_u(grstr, "r", reg, 0);
    return grstr;
}

static char *ipRelStr(char *relstr, ADDR addr, DWORD val)
{
#if 0
    (void)sprintf(relstr, "0x%llx", addr+val);
#else
#if 0
    if ((long long)val < 0)
	(void)sprintf(relstr, ".-0x%llx", -val);
    else
	(void)sprintf(relstr, ".+0x%llx", val);
#else
    symAddrtoDefFunc(addr + val, relstr, 4, YES, -symLen);
#endif
#endif
    return relstr;
}

static void relocPlusMinus(char *relocstr, char *head, char *name, char *mid,
			   long long addend, char *tail)
{
    if (addend >= MIN_DAS_DECIMAL && addend <= MAX_DAS_DECIMAL) {
	int len = symLen;
	size_t hLen, mLen, tLen;

	hLen = strlen(head);
	mLen = strlen(mid);
	tLen = strlen(tail);

	if (addend) {
	    char addendStr[20], *sepStr;
	    int aLen;

	    if (addend < 0) {
		sepStr = " - ";
		addend = -addend;
	    } else
		sepStr = " + ";
	    aLen = sprintf(addendStr, "%lld", addend);
	    len -= hLen + mLen + 3 + aLen + tLen;
	    (void)sprintf(relocstr, "%s%.*s%s%s%s%s", head, len, name,
			  mid, sepStr, addendStr, tail);
	} else {	/* addend == 0 */
	    len -= hLen + (mLen ? mLen : tLen);
	    (void)sprintf(relocstr, "%s%.*s%s", head, len, name,
			  mLen ? mid : tail);
	}
    } else
	(void)sprintf(relocstr, "%s%s%s + 0x%llx%s", head, name, mid,
		      addend, tail);
}

#define RELOC_TYPE(x)	(x & ~7)
#define R_DIR		RELOC_TYPE(R_IA_64_IMM14)
#define R_GPREL		RELOC_TYPE(R_IA_64_GPREL22)
#define R_LTOFF		RELOC_TYPE(R_IA_64_LTOFF22)
#define R_PLTOFF	RELOC_TYPE(R_IA_64_PLTOFF22)
#define R_FPTR		RELOC_TYPE(R_IA_64_FPTR64I)
#define R_PCREL		RELOC_TYPE(R_IA_64_PCREL60B)
#define R_LTOFF_FPTR	RELOC_TYPE(R_IA_64_LTOFF_FPTR22)
#define R_SEGREL	RELOC_TYPE(R_IA_64_SEGREL32MSB)
#define R_SECREL	RELOC_TYPE(R_IA_64_SECREL32MSB)
#define R_REL		RELOC_TYPE(R_IA_64_REL32MSB)
#define R_LTV		RELOC_TYPE(R_IA_64_LTV32MSB)
#define R_PLT		RELOC_TYPE(R_IA_64_IPLTMSB)
#define R_TPREL		RELOC_TYPE(R_IA_64_TPREL22)
#define R_LTOFF_TPREL	RELOC_TYPE(R_IA_64_LTOFF_TPREL22)

static char *relocStr(char *relocstr, DasRelocationPtr reloc)
{
    long long addend = reloc->addend;
    char *name = reloc->name;
    unsigned int type = reloc->type;

    if (type == R_IA_64_LTOFF22X)	// map LTOFF22X to LTOFF22
	type = R_IA_64_LTOFF22;
    else if (type == R_IA_64_PCREL21BI)	// map PCREL21BI to PCREL60B
	type = R_IA_64_PCREL60B;

    switch (RELOC_TYPE(type)) {
    /* XXX - add remaining reloc types */
    case R_DIR:
	relocPlusMinus(relocstr, "", name, "", addend, "");
	break;
    case R_GPREL:
	relocPlusMinus(relocstr, "@gprel(", name, "", addend, ")");
	break;
    case R_LTOFF:
	relocPlusMinus(relocstr, "@ltoff(", name, "", addend, ")");
	break;
    case R_PLTOFF:
	relocPlusMinus(relocstr, "@pltoff(", name, ")", addend, "");
	break;
    case R_FPTR:
	relocPlusMinus(relocstr, "@fptr(", name, "", addend, ")");
	break;
    case R_PCREL:
	if (addend)
	    (void)sprintf(relocstr, "%s + 0x%llx", name, addend);
	else if (symLen) {
	    (void)sprintf(relocstr, "%.*s", symLen, name);
	    symLen = 0;
	} else
	    (void)sprintf(relocstr, "%s", name);
	break;
    case R_LTOFF_FPTR:
	relocPlusMinus(relocstr, "@ltoff(@fptr(", name, "", addend, "))");
	break;
    case R_SEGREL:
	relocPlusMinus(relocstr, "@segrel(", name, "", addend, ")");
	break;
    case R_SECREL:
	relocPlusMinus(relocstr, "@secrel(", name, "", addend, ")");
	break;
    case R_LTV:
	relocPlusMinus(relocstr, "", name, "", addend, "");
	break;
    case R_TPREL:
	relocPlusMinus(relocstr, "@tprel(", name, "", addend, ")");
	break;
    case R_LTOFF_TPREL:
	relocPlusMinus(relocstr, "@ltoff(@tprel(", name, "", addend, "))");
	break;
    case R_REL:		/* XXX - what should this do? */
#if 0
    case R_PLT:		/* XXX - what should this do? */
#endif
    default:
#if 0
	(void)sprintf(relocstr, "*** unknown reloc 0x%x: %s + 0x%llx ***",
		      reloc->type, name, addend);
#else
	(void)sprintf(relocstr, "*ur 0x%x*", reloc->type);
#endif
	break;
    }
    return relocstr;
}

typedef BOOL    (*PCHKFN)(OperandsPtr);
typedef void	(*PADJFN)(OperandsPtr);

static BOOL qp0(OperandsPtr opnds)
{
    return opnds->qp == 0;
}

static BOOL f2eq0_F1(OperandsPtr opnds)
{
    return opnds->u.F1.f2.bits == 0;
}

static BOOL f2eq0_F2(OperandsPtr opnds)
{
    return opnds->u.F2.f2.bits == 0;
}

static BOOL f2eq0_F9(OperandsPtr opnds)
{
    return opnds->u.F9.f2.bits == 0;
}

static BOOL f4eq1_F1(OperandsPtr opnds)
{
    return opnds->u.F1.f4.bits == 1;
}

static BOOL f2eqf3_F9(OperandsPtr opnds)
{
    return opnds->u.F9.f2.bits == opnds->u.F9.f3.bits;
}

static BOOL f4eq1_f2eq0_F1(OperandsPtr opnds)
{
    return opnds->u.F1.f4.bits == 1 && opnds->u.F1.f2.bits == 0;
}

static BOOL imm14eq0_A4(OperandsPtr opnds)
{
    return opnds->u.A4.imm14.bits == 0;
}

static BOOL r3eq0_A5(OperandsPtr opnds)
{
    return opnds->u.A5.r3.bits == 0;
}

static BOOL tag13eq0_I21(OperandsPtr opnds)
{
    return opnds->u.I21.tag13.bits == 0;
}

static BOOL posleneq64_I11(OperandsPtr opnds)
{
    return opnds->u.I11.pos6.bits + opnds->u.I11.len6.bits == 64;
}

static BOOL posleneq64_I12(OperandsPtr opnds)
{
    return opnds->u.I12.pos6.bits + opnds->u.I12.len6.bits == 64;
}

static BOOL p1eq0_p2ne0(OperandsPtr opnds)
{
    return opnds->u.A6.p1.bits == 0 && opnds->u.A6.p2.bits != 0;
}

static BOOL mask17_all(OperandsPtr opnds)
{
    return opnds->u.I23.mask17.bits == (-1 << 1);
}

static void no_f2_F9(OperandsPtr opnds)
{
    opnds->u.F9.f2.bits = opnds->u.F9.f3.bits;
    opnds->u.F9.f3.type = EM_OPTYPE_NONE;
}

static void no_f4_F1(OperandsPtr opnds)
{
    opnds->u.F1.f4.bits = opnds->u.F1.f2.bits;
    opnds->u.F1.f2.type = EM_OPTYPE_NONE;
}

static void no_f2_F1(OperandsPtr opnds)
{
    opnds->u.F1.f2.type = EM_OPTYPE_NONE;
}

static void no_f4_f2_F1(OperandsPtr opnds)
{
    opnds->u.F2.f4.type = EM_OPTYPE_NONE;
    opnds->u.F1.f2.type = EM_OPTYPE_NONE;
}

static void no_f2_F2(OperandsPtr opnds)
{
    opnds->u.F2.f2.type = EM_OPTYPE_NONE;
}

static void no_imm14_A4(OperandsPtr opnds)
{
    opnds->u.A4.imm14 = opnds->u.A4.r3;
    opnds->u.A4.r3.type = EM_OPTYPE_NONE;
}

static void no_r3_A5(OperandsPtr opnds)
{
    opnds->u.A5.r3.type = EM_OPTYPE_NONE;
}

static void no_tag_I21(OperandsPtr opnds)
{
    opnds->u.I21.tag13.type = EM_OPTYPE_NONE;
}

static void no_len6_I12(OperandsPtr opnds)
{
    opnds->u.I12.len6.type = EM_OPTYPE_NONE;
}

static void no_len6_I11(OperandsPtr opnds)
{
    opnds->u.I11.len6.type = EM_OPTYPE_NONE;
}

static void no_mask17(OperandsPtr opnds)
{
    opnds->u.I23.mask17.type = EM_OPTYPE_NONE;
}

static void imm64_X2(OperandsPtr opnds)
{
    opnds->u.X2.imm64.type = EM_OPTYPE_IMM64;
}

static void swap_p1p2(OperandsPtr opnds)
{
    unsigned long long tmp;

    tmp = opnds->u.A6.p1.bits;
    opnds->u.A6.p1.bits = opnds->u.A6.p2.bits;
    opnds->u.A6.p2.bits = tmp;
}

static void hex_imm(OperandsPtr opnds)
{
    unsigned o;

    for (o = 0; (o < MAX_OPERANDS && opnds->u.op[o].type); o++)
	if (OPCLASS(opnds->u.op[o].type) == EM_OPTYPE_IMM)
	    opnds->u.op[o].type = EM_OPTYPE_HIMM;
}


static void hint_imm(OperandsPtr opnds)
{
    opnds->u.I18.imm21.type = EM_OPTYPE_HINT_IMM;
}

static void alloc_imm(OperandsPtr opnds)
{
    /* record register stack state */
    dasSetFrameSize((unsigned int)opnds->u.M34.i.bits,
		    (unsigned int)opnds->u.M34.l.bits,
		    (unsigned int)opnds->u.M34.o.bits,
		    (unsigned int)opnds->u.M34.r.bits);
}

void dasSetFrameSize(unsigned int i, unsigned int l, unsigned int o,
		     unsigned int r)
{
    /* set register stack state */
    ins  = i;
    locs = l;
    outs = o;
    rots = r;
}

static void iDasm(unsigned slot, InstID instID, OperandsPtr opnds, char *buf,
		  SB stop)
{
    char *mnem, *p;
    const char *sep;
    unsigned i, o, reg;
    unsigned long long bits;
    long long sbits;
    BOOL first = YES;
    ADDR addr;
    DasRelocation reloc;
    static struct {
	InstID instID;
	PCHKFN chkFn;
	char *mnem;
	PADJFN adjFn;
    } pseudo[] = {
	{EM_INST_NONE,              0,              0,            0},
	{EM_ADDL_R1_IMM22_R3,       r3eq0_A5,       "mov",        no_r3_A5},
	{EM_ADDS_R1_IMM14_R3,       imm14eq0_A4,    "mov",        no_imm14_A4},
	{EM_ALLOC_R1_AR_PFS_I_L_O_R,       0,       0,            alloc_imm},
	{EM_ANDCM_R1_IMM8_R3,       0,              0,            hex_imm},
	{EM_AND_R1_IMM8_R3,         0,              0,            hex_imm},
	{EM_BR_COND_SPTK_FEW_TARGET25,      qp0,    "br",         0},
	{EM_BR_COND_SPTK_MANY_TARGET25,     qp0,    "br.many",    0},
	{EM_BR_COND_SPTK_FEW_CLR_TARGET25,  qp0,    "br.clr",     0},
	{EM_BR_COND_SPTK_MANY_CLR_TARGET25, qp0,    "br.many.clr",0},
	{EM_BR_COND_SPTK_FEW_B2,            qp0,    "br",         0},
	{EM_BR_COND_SPTK_MANY_B2,           qp0,    "br.many",    0},
	{EM_BR_COND_SPTK_FEW_CLR_B2,        qp0,    "br.clr",     0},
	{EM_BR_COND_SPTK_MANY_CLR_B2,       qp0,    "br.many.clr",0},
	{EM_BREAK_B_IMM21,          0,              0,            hex_imm},
	{EM_BREAK_F_IMM21,          0,              0,            hex_imm},
	{EM_BREAK_I_IMM21,          0,              0,            hex_imm},
	{EM_BREAK_M_IMM21,          0,              0,            hex_imm},
	{EM_BREAK_X_IMM62,          0,              0,            hex_imm},
	{EM_CMP_LT_P1_P2_R2_R3,     p1eq0_p2ne0,    "cmp.ge",     swap_p1p2},
	{EM_CMP_LTU_P1_P2_R2_R3,    p1eq0_p2ne0,    "cmp.geu",    swap_p1p2},
	{EM_CMP_EQ_P1_P2_R2_R3,     p1eq0_p2ne0,    "cmp.ne",     swap_p1p2},
	{EM_CMP_LT_UNC_P1_P2_R2_R3, p1eq0_p2ne0,    "cmp.ge.unc", swap_p1p2},
	{EM_CMP_LTU_UNC_P1_P2_R2_R3,p1eq0_p2ne0,    "cmp.geu.unc",swap_p1p2},
	{EM_CMP_EQ_UNC_P1_P2_R2_R3, p1eq0_p2ne0,    "cmp.ne.unc", swap_p1p2},
	{EM_CMP_EQ_AND_P1_P2_R2_R3, p1eq0_p2ne0,      0,          swap_p1p2},
	{EM_CMP_EQ_OR_P1_P2_R2_R3,  p1eq0_p2ne0,      0,          swap_p1p2},
	{EM_CMP_EQ_OR_ANDCM_P1_P2_R2_R3, p1eq0_p2ne0, "cmp.ne.and.orcm",
	 swap_p1p2},
	{EM_CMP_NE_AND_P1_P2_R2_R3, p1eq0_p2ne0,      0,          swap_p1p2},
	{EM_CMP_NE_OR_P1_P2_R2_R3,  p1eq0_p2ne0,      0,          swap_p1p2},
	{EM_CMP_NE_OR_ANDCM_P1_P2_R2_R3, p1eq0_p2ne0, "cmp.eq.and.orcm",
	 swap_p1p2},
	{EM_CMP4_LT_P1_P2_R2_R3,     p1eq0_p2ne0,    "cmp4.ge",     swap_p1p2},
	{EM_CMP4_LTU_P1_P2_R2_R3,    p1eq0_p2ne0,    "cmp4.geu",    swap_p1p2},
	{EM_CMP4_EQ_P1_P2_R2_R3,     p1eq0_p2ne0,    "cmp4.ne",     swap_p1p2},
	{EM_CMP4_LT_UNC_P1_P2_R2_R3, p1eq0_p2ne0,    "cmp4.ge.unc", swap_p1p2},
	{EM_CMP4_LTU_UNC_P1_P2_R2_R3,p1eq0_p2ne0,    "cmp4.geu.unc",swap_p1p2},
	{EM_CMP4_EQ_UNC_P1_P2_R2_R3, p1eq0_p2ne0,    "cmp4.ne.unc", swap_p1p2},
	{EM_CMP4_EQ_AND_P1_P2_R2_R3, p1eq0_p2ne0,      0,          swap_p1p2},
	{EM_CMP4_EQ_OR_P1_P2_R2_R3,  p1eq0_p2ne0,      0,          swap_p1p2},
	{EM_CMP4_EQ_OR_ANDCM_P1_P2_R2_R3, p1eq0_p2ne0, "cmp4.ne.and.orcm",
	 swap_p1p2},
	{EM_CMP4_NE_AND_P1_P2_R2_R3, p1eq0_p2ne0,      0,          swap_p1p2},
	{EM_CMP4_NE_OR_P1_P2_R2_R3,  p1eq0_p2ne0,      0,          swap_p1p2},
	{EM_CMP4_NE_OR_ANDCM_P1_P2_R2_R3, p1eq0_p2ne0, "cmp4.eq.and.orcm",
	 swap_p1p2},
	{EM_CMP_GT_AND_P1_P2_R0_R3,     p1eq0_p2ne0,    0,     swap_p1p2},
	{EM_CMP_GT_OR_P1_P2_R0_R3,      p1eq0_p2ne0,    0,     swap_p1p2},
	{EM_CMP_GT_OR_ANDCM_P1_P2_R0_R3,p1eq0_p2ne0,    "cmp.le.and.orcm",
	 swap_p1p2},
	{EM_CMP_LE_AND_P1_P2_R0_R3,     p1eq0_p2ne0,    0,     swap_p1p2},
	{EM_CMP_LE_OR_P1_P2_R0_R3,      p1eq0_p2ne0,    0,     swap_p1p2},
	{EM_CMP_LE_OR_ANDCM_P1_P2_R0_R3,p1eq0_p2ne0,    "cmp.gt.and.orcm",
	 swap_p1p2},
	{EM_CMP_GE_AND_P1_P2_R0_R3,     p1eq0_p2ne0,    0,     swap_p1p2},
	{EM_CMP_GE_OR_P1_P2_R0_R3,      p1eq0_p2ne0,    0,     swap_p1p2},
	{EM_CMP_GE_OR_ANDCM_P1_P2_R0_R3,p1eq0_p2ne0,    "cmp.lt.and.orcm",
	 swap_p1p2},
	{EM_CMP_LT_AND_P1_P2_R0_R3,     p1eq0_p2ne0,    0,     swap_p1p2},
	{EM_CMP_LT_OR_P1_P2_R0_R3,      p1eq0_p2ne0,    0,     swap_p1p2},
	{EM_CMP_LT_OR_ANDCM_P1_P2_R0_R3,p1eq0_p2ne0,    "cmp.ge.and.orcm",
	 swap_p1p2},
	{EM_CMP4_GT_AND_P1_P2_R0_R3,     p1eq0_p2ne0,    0,     swap_p1p2},
	{EM_CMP4_GT_OR_P1_P2_R0_R3,      p1eq0_p2ne0,    0,     swap_p1p2},
	{EM_CMP4_GT_OR_ANDCM_P1_P2_R0_R3,p1eq0_p2ne0,    "cmp4.le.and.orcm",
	 swap_p1p2},
	{EM_CMP4_LE_AND_P1_P2_R0_R3,     p1eq0_p2ne0,    0,     swap_p1p2},
	{EM_CMP4_LE_OR_P1_P2_R0_R3,      p1eq0_p2ne0,    0,     swap_p1p2},
	{EM_CMP4_LE_OR_ANDCM_P1_P2_R0_R3,p1eq0_p2ne0,    "cmp4.gt.and.orcm",
	 swap_p1p2},
	{EM_CMP4_GE_AND_P1_P2_R0_R3,     p1eq0_p2ne0,    0,     swap_p1p2},
	{EM_CMP4_GE_OR_P1_P2_R0_R3,      p1eq0_p2ne0,    0,     swap_p1p2},
	{EM_CMP4_GE_OR_ANDCM_P1_P2_R0_R3,p1eq0_p2ne0,    "cmp4.lt.and.orcm",
	 swap_p1p2},
	{EM_CMP4_LT_AND_P1_P2_R0_R3,     p1eq0_p2ne0,    0,     swap_p1p2},
	{EM_CMP4_LT_OR_P1_P2_R0_R3,      p1eq0_p2ne0,    0,     swap_p1p2},
	{EM_CMP4_LT_OR_ANDCM_P1_P2_R0_R3,p1eq0_p2ne0,    "cmp4.ge.and.orcm",
	 swap_p1p2},
	{EM_CMP_LT_P1_P2_IMM8_R3,     p1eq0_p2ne0,    "cmp.ge",     swap_p1p2},
	{EM_CMP_LTU_P1_P2_IMM8_R3,    p1eq0_p2ne0,    "cmp.geu",    swap_p1p2},
	{EM_CMP_EQ_P1_P2_IMM8_R3,     p1eq0_p2ne0,    "cmp.ne",     swap_p1p2},
	{EM_CMP_LT_UNC_P1_P2_IMM8_R3, p1eq0_p2ne0,    "cmp.ge.unc", swap_p1p2},
	{EM_CMP_LTU_UNC_P1_P2_IMM8_R3,p1eq0_p2ne0,    "cmp.geu.unc",swap_p1p2},
	{EM_CMP_EQ_UNC_P1_P2_IMM8_R3, p1eq0_p2ne0,    "cmp.ne.unc", swap_p1p2},
	{EM_CMP_EQ_AND_P1_P2_IMM8_R3, p1eq0_p2ne0,      0,          swap_p1p2},
	{EM_CMP_EQ_OR_P1_P2_IMM8_R3,  p1eq0_p2ne0,      0,          swap_p1p2},
	{EM_CMP_EQ_OR_ANDCM_P1_P2_IMM8_R3, p1eq0_p2ne0, "cmp.ne.and.orcm",
	 swap_p1p2},
	{EM_CMP_NE_AND_P1_P2_IMM8_R3, p1eq0_p2ne0,      0,          swap_p1p2},
	{EM_CMP_NE_OR_P1_P2_IMM8_R3,  p1eq0_p2ne0,      0,          swap_p1p2},
	{EM_CMP_NE_OR_ANDCM_P1_P2_IMM8_R3, p1eq0_p2ne0, "cmp.eq.and.orcm",
	 swap_p1p2},
	{EM_CMP4_LT_P1_P2_IMM8_R3,     p1eq0_p2ne0,  "cmp4.ge",     swap_p1p2},
	{EM_CMP4_LTU_P1_P2_IMM8_R3,    p1eq0_p2ne0,  "cmp4.geu",    swap_p1p2},
	{EM_CMP4_EQ_P1_P2_IMM8_R3,     p1eq0_p2ne0,  "cmp4.ne",     swap_p1p2},
	{EM_CMP4_LT_UNC_P1_P2_IMM8_R3, p1eq0_p2ne0,  "cmp4.ge.unc", swap_p1p2},
	{EM_CMP4_LTU_UNC_P1_P2_IMM8_R3,p1eq0_p2ne0,  "cmp4.geu.unc",swap_p1p2},
	{EM_CMP4_EQ_UNC_P1_P2_IMM8_R3, p1eq0_p2ne0,  "cmp4.ne.unc", swap_p1p2},
	{EM_CMP4_EQ_AND_P1_P2_IMM8_R3, p1eq0_p2ne0,    0,          swap_p1p2},
	{EM_CMP4_EQ_OR_P1_P2_IMM8_R3,  p1eq0_p2ne0,    0,          swap_p1p2},
	{EM_CMP4_EQ_OR_ANDCM_P1_P2_IMM8_R3, p1eq0_p2ne0, "cmp4.ne.and.orcm",
	 swap_p1p2},
	{EM_CMP4_NE_AND_P1_P2_IMM8_R3, p1eq0_p2ne0,    0,          swap_p1p2},
	{EM_CMP4_NE_OR_P1_P2_IMM8_R3,  p1eq0_p2ne0,    0,          swap_p1p2},
	{EM_CMP4_NE_OR_ANDCM_P1_P2_IMM8_R3, p1eq0_p2ne0, "cmp4.eq.and.orcm",
	 swap_p1p2},
	{EM_DEP_Z_R1_R2_POS6_LEN6,  posleneq64_I12, "shl",        no_len6_I12},
	{EM_EXTR_R1_R3_POS6_LEN6,   posleneq64_I11, "shr",        no_len6_I11},
	{EM_EXTR_U_R1_R3_POS6_LEN6, posleneq64_I11, "shr.u",      no_len6_I11},
	{EM_FCLASS_M_P1_P2_F2_FCLASS9, p1eq0_p2ne0, "fclass.nm",  swap_p1p2},
	{EM_FCLASS_M_UNC_P1_P2_F2_FCLASS9, p1eq0_p2ne0, "fclass.nm.unc",
	 swap_p1p2},
	{EM_FCMP_EQ_S0_P1_P2_F2_F3, p1eq0_p2ne0,     "fcmp.neq.s0", swap_p1p2},
	{EM_FCMP_EQ_S1_P1_P2_F2_F3, p1eq0_p2ne0,     "fcmp.neq.s1", swap_p1p2},
	{EM_FCMP_EQ_S2_P1_P2_F2_F3, p1eq0_p2ne0,     "fcmp.neq.s2", swap_p1p2},
	{EM_FCMP_EQ_S3_P1_P2_F2_F3, p1eq0_p2ne0,     "fcmp.neq.s3", swap_p1p2},
	{EM_FCMP_LT_S0_P1_P2_F2_F3, p1eq0_p2ne0,     "fcmp.nlt.s0", swap_p1p2},
	{EM_FCMP_LT_S1_P1_P2_F2_F3, p1eq0_p2ne0,     "fcmp.nlt.s1", swap_p1p2},
	{EM_FCMP_LT_S2_P1_P2_F2_F3, p1eq0_p2ne0,     "fcmp.nlt.s2", swap_p1p2},
	{EM_FCMP_LT_S3_P1_P2_F2_F3, p1eq0_p2ne0,     "fcmp.nlt.s3", swap_p1p2},
	{EM_FCMP_LE_S0_P1_P2_F2_F3, p1eq0_p2ne0,     "fcmp.nle.s0", swap_p1p2},
	{EM_FCMP_LE_S1_P1_P2_F2_F3, p1eq0_p2ne0,     "fcmp.nle.s1", swap_p1p2},
	{EM_FCMP_LE_S2_P1_P2_F2_F3, p1eq0_p2ne0,     "fcmp.nle.s2", swap_p1p2},
	{EM_FCMP_LE_S3_P1_P2_F2_F3, p1eq0_p2ne0,     "fcmp.nle.s3", swap_p1p2},
	{EM_FCMP_UNORD_S0_P1_P2_F2_F3, p1eq0_p2ne0,  "fcmp.ord.s0", swap_p1p2},
	{EM_FCMP_UNORD_S1_P1_P2_F2_F3, p1eq0_p2ne0,  "fcmp.ord.s1", swap_p1p2},
	{EM_FCMP_UNORD_S2_P1_P2_F2_F3, p1eq0_p2ne0,  "fcmp.ord.s2", swap_p1p2},
	{EM_FCMP_UNORD_S3_P1_P2_F2_F3, p1eq0_p2ne0,  "fcmp.ord.s3", swap_p1p2},
	{EM_FCMP_EQ_UNC_S0_P1_P2_F2_F3, p1eq0_p2ne0,    "fcmp.neq.unc.s0",
	 swap_p1p2},
	{EM_FCMP_EQ_UNC_S1_P1_P2_F2_F3, p1eq0_p2ne0,    "fcmp.neq.unc.s1",
	 swap_p1p2},
	{EM_FCMP_EQ_UNC_S2_P1_P2_F2_F3, p1eq0_p2ne0,    "fcmp.neq.unc.s2",
	 swap_p1p2},
	{EM_FCMP_EQ_UNC_S3_P1_P2_F2_F3, p1eq0_p2ne0,    "fcmp.neq.unc.s3",
	 swap_p1p2},
	{EM_FCMP_LT_UNC_S0_P1_P2_F2_F3, p1eq0_p2ne0,    "fcmp.nlt.unc.s0",
	 swap_p1p2},
	{EM_FCMP_LT_UNC_S1_P1_P2_F2_F3, p1eq0_p2ne0,    "fcmp.nlt.unc.s1",
	 swap_p1p2},
	{EM_FCMP_LT_UNC_S2_P1_P2_F2_F3, p1eq0_p2ne0,    "fcmp.nlt.unc.s2",
	 swap_p1p2},
	{EM_FCMP_LT_UNC_S3_P1_P2_F2_F3, p1eq0_p2ne0,    "fcmp.nlt.unc.s3",
	 swap_p1p2},
	{EM_FCMP_LE_UNC_S0_P1_P2_F2_F3, p1eq0_p2ne0,    "fcmp.nle.unc.s0",
	 swap_p1p2},
	{EM_FCMP_LE_UNC_S1_P1_P2_F2_F3, p1eq0_p2ne0,    "fcmp.nle.unc.s1",
	 swap_p1p2},
	{EM_FCMP_LE_UNC_S2_P1_P2_F2_F3, p1eq0_p2ne0,    "fcmp.nle.unc.s2",
	 swap_p1p2},
	{EM_FCMP_LE_UNC_S3_P1_P2_F2_F3, p1eq0_p2ne0,    "fcmp.nle.unc.s3",
	 swap_p1p2},
	{EM_FCMP_UNORD_UNC_S0_P1_P2_F2_F3, p1eq0_p2ne0, "fcmp.ord.unc.s0",
	 swap_p1p2},
	{EM_FCMP_UNORD_UNC_S1_P1_P2_F2_F3, p1eq0_p2ne0, "fcmp.ord.unc.s1",
	 swap_p1p2},
	{EM_FCMP_UNORD_UNC_S2_P1_P2_F2_F3, p1eq0_p2ne0, "fcmp.ord.unc.s2",
	 swap_p1p2},
	{EM_FCMP_UNORD_UNC_S3_P1_P2_F2_F3, p1eq0_p2ne0, "fcmp.ord.unc.s3",
	 swap_p1p2},
	{EM_FMA_D_S0_F1_F3_F4_F2,   f4eq1_f2eq0_F1, "fnorm.d.s0", no_f4_f2_F1},
	{EM_FMA_D_S0_F1_F3_F4_F2,   f2eq0_F1,       "fmpy.d.s0",  no_f2_F1},
	{EM_FMA_D_S0_F1_F3_F4_F2,   f4eq1_F1,       "fadd.d.s0",  no_f4_F1},
	{EM_FMA_D_S1_F1_F3_F4_F2,   f4eq1_f2eq0_F1, "fnorm.d.s1", no_f4_f2_F1},
	{EM_FMA_D_S1_F1_F3_F4_F2,   f2eq0_F1,       "fmpy.d.s1",  no_f2_F1},
	{EM_FMA_D_S1_F1_F3_F4_F2,   f4eq1_F1,       "fadd.d.s1",  no_f4_F1},
	{EM_FMA_D_S2_F1_F3_F4_F2,   f4eq1_f2eq0_F1, "fnorm.d.s2", no_f4_f2_F1},
	{EM_FMA_D_S2_F1_F3_F4_F2,   f2eq0_F1,       "fmpy.d.s2",  no_f2_F1},
	{EM_FMA_D_S2_F1_F3_F4_F2,   f4eq1_F1,       "fadd.d.s2",  no_f4_F1},
	{EM_FMA_D_S3_F1_F3_F4_F2,   f4eq1_f2eq0_F1, "fnorm.d.s3", no_f4_f2_F1},
	{EM_FMA_D_S3_F1_F3_F4_F2,   f2eq0_F1,       "fmpy.d.s3",  no_f2_F1},
	{EM_FMA_D_S3_F1_F3_F4_F2,   f4eq1_F1,       "fadd.d.s3",  no_f4_F1},
	{EM_FMA_S0_F1_F3_F4_F2,     f4eq1_f2eq0_F1, "fnorm.s0",   no_f4_f2_F1},
	{EM_FMA_S0_F1_F3_F4_F2,     f2eq0_F1,       "fmpy.s0",    no_f2_F1},
	{EM_FMA_S0_F1_F3_F4_F2,     f4eq1_F1,       "fadd.s0",    no_f4_F1},
	{EM_FMA_S1_F1_F3_F4_F2,     f4eq1_f2eq0_F1, "fnorm.s1",   no_f4_f2_F1},
	{EM_FMA_S1_F1_F3_F4_F2,     f2eq0_F1,       "fmpy.s1",    no_f2_F1},
	{EM_FMA_S1_F1_F3_F4_F2,     f4eq1_F1,       "fadd.s1",    no_f4_F1},
	{EM_FMA_S2_F1_F3_F4_F2,     f4eq1_f2eq0_F1, "fnorm.s2",   no_f4_f2_F1},
	{EM_FMA_S2_F1_F3_F4_F2,     f2eq0_F1,       "fmpy.s2",    no_f2_F1},
	{EM_FMA_S2_F1_F3_F4_F2,     f4eq1_F1,       "fadd.s2",    no_f4_F1},
	{EM_FMA_S3_F1_F3_F4_F2,     f4eq1_f2eq0_F1, "fnorm.s3",   no_f4_f2_F1},
	{EM_FMA_S3_F1_F3_F4_F2,     f2eq0_F1,       "fmpy.s3",    no_f2_F1},
	{EM_FMA_S3_F1_F3_F4_F2,     f4eq1_F1,       "fadd.s3",    no_f4_F1},
	{EM_FMA_S_S0_F1_F3_F4_F2,   f4eq1_f2eq0_F1, "fnorm.s.s0", no_f4_f2_F1},
	{EM_FMA_S_S0_F1_F3_F4_F2,   f2eq0_F1,       "fmpy.s.s0",  no_f2_F1},
	{EM_FMA_S_S0_F1_F3_F4_F2,   f4eq1_F1,       "fadd.s.s0",  no_f4_F1},
	{EM_FMA_S_S1_F1_F3_F4_F2,   f4eq1_f2eq0_F1, "fnorm.s.s1", no_f4_f2_F1},
	{EM_FMA_S_S1_F1_F3_F4_F2,   f2eq0_F1,       "fmpy.s.s1",  no_f2_F1},
	{EM_FMA_S_S1_F1_F3_F4_F2,   f4eq1_F1,       "fadd.s.s1",  no_f4_F1},
	{EM_FMA_S_S2_F1_F3_F4_F2,   f4eq1_f2eq0_F1, "fnorm.s.s2", no_f4_f2_F1},
	{EM_FMA_S_S2_F1_F3_F4_F2,   f2eq0_F1,       "fmpy.s.s2",  no_f2_F1},
	{EM_FMA_S_S2_F1_F3_F4_F2,   f4eq1_F1,       "fadd.s.s2",  no_f4_F1},
	{EM_FMA_S_S3_F1_F3_F4_F2,   f4eq1_f2eq0_F1, "fnorm.s.s3", no_f4_f2_F1},
	{EM_FMA_S_S3_F1_F3_F4_F2,   f2eq0_F1,       "fmpy.s.s3",  no_f2_F1},
	{EM_FMA_S_S3_F1_F3_F4_F2,   f4eq1_F1,       "fadd.s.s3",  no_f4_F1},
	{EM_FMERGE_NS_F1_F2_F3,     f2eqf3_F9,      "fneg",       no_f2_F9},
	{EM_FMERGE_NS_F1_F2_F3,     f2eq0_F9,       "fnegabs",    no_f2_F9},
	{EM_FMERGE_S_F1_F2_F3,      f2eqf3_F9,      "mov",        no_f2_F9},
	{EM_FMERGE_S_F1_F2_F3,      f2eq0_F9,       "fabs",       no_f2_F9},
	{EM_FMS_D_S0_F1_F3_F4_F2,   f4eq1_F1,       "fsub.d.s0",  no_f4_F1},
	{EM_FMS_D_S1_F1_F3_F4_F2,   f4eq1_F1,       "fsub.d.s1",  no_f4_F1},
	{EM_FMS_D_S2_F1_F3_F4_F2,   f4eq1_F1,       "fsub.d.s2",  no_f4_F1},
	{EM_FMS_D_S3_F1_F3_F4_F2,   f4eq1_F1,       "fsub.d.s3",  no_f4_F1},
	{EM_FMS_S0_F1_F3_F4_F2,     f4eq1_F1,       "fsub.s0",    no_f4_F1},
	{EM_FMS_S1_F1_F3_F4_F2,     f4eq1_F1,       "fsub.s1",    no_f4_F1},
	{EM_FMS_S2_F1_F3_F4_F2,     f4eq1_F1,       "fsub.s2",    no_f4_F1},
	{EM_FMS_S3_F1_F3_F4_F2,     f4eq1_F1,       "fsub.s3",    no_f4_F1},
	{EM_FMS_S_S0_F1_F3_F4_F2,   f4eq1_F1,       "fsub.s.s0",  no_f4_F1},
	{EM_FMS_S_S1_F1_F3_F4_F2,   f4eq1_F1,       "fsub.s.s1",  no_f4_F1},
	{EM_FMS_S_S2_F1_F3_F4_F2,   f4eq1_F1,       "fsub.s.s2",  no_f4_F1},
	{EM_FMS_S_S3_F1_F3_F4_F2,   f4eq1_F1,       "fsub.s.s3",  no_f4_F1},
	{EM_FNMA_D_S0_F1_F3_F4_F2,  f2eq0_F1,       "fnmpy.d.s0", no_f2_F1},
	{EM_FNMA_D_S1_F1_F3_F4_F2,  f2eq0_F1,       "fnmpy.d.s1", no_f2_F1},
	{EM_FNMA_D_S2_F1_F3_F4_F2,  f2eq0_F1,       "fnmpy.d.s2", no_f2_F1},
	{EM_FNMA_D_S3_F1_F3_F4_F2,  f2eq0_F1,       "fnmpy.d.s3", no_f2_F1},
	{EM_FNMA_S0_F1_F3_F4_F2,    f2eq0_F1,       "fnmpy.s0",   no_f2_F1},
	{EM_FNMA_S1_F1_F3_F4_F2,    f2eq0_F1,       "fnmpy.s1",   no_f2_F1},
	{EM_FNMA_S2_F1_F3_F4_F2,    f2eq0_F1,       "fnmpy.s2",   no_f2_F1},
	{EM_FNMA_S3_F1_F3_F4_F2,    f2eq0_F1,       "fnmpy.s3",   no_f2_F1},
	{EM_FNMA_S_S0_F1_F3_F4_F2,  f2eq0_F1,       "fnmpy.s.s0", no_f2_F1},
	{EM_FNMA_S_S1_F1_F3_F4_F2,  f2eq0_F1,       "fnmpy.s.s1", no_f2_F1},
	{EM_FNMA_S_S2_F1_F3_F4_F2,  f2eq0_F1,       "fnmpy.s.s2", no_f2_F1},
	{EM_FNMA_S_S3_F1_F3_F4_F2,  f2eq0_F1,       "fnmpy.s.s3", no_f2_F1},
	{EM_FPMA_S0_F1_F3_F4_F2,    f2eq0_F1,       "fpmpy.s0",   no_f2_F1},
	{EM_FPMA_S1_F1_F3_F4_F2,    f2eq0_F1,       "fpmpy.s1",   no_f2_F1},
	{EM_FPMA_S2_F1_F3_F4_F2,    f2eq0_F1,       "fpmpy.s2",   no_f2_F1},
	{EM_FPMA_S3_F1_F3_F4_F2,    f2eq0_F1,       "fpmpy.s3",   no_f2_F1},
	{EM_FPMERGE_NS_F1_F2_F3,    f2eqf3_F9,      "fpneg",      no_f2_F9},
	{EM_FPMERGE_NS_F1_F2_F3,    f2eq0_F9,       "fpnegabs",   no_f2_F9},
	{EM_FPMERGE_S_F1_F2_F3,     f2eq0_F9,       "fpabs",      no_f2_F9},
	{EM_FPNMA_S0_F1_F3_F4_F2,   f2eq0_F1,       "fpnmpy.s0",  no_f2_F1},
	{EM_FPNMA_S1_F1_F3_F4_F2,   f2eq0_F1,       "fpnmpy.s1",  no_f2_F1},
	{EM_FPNMA_S2_F1_F3_F4_F2,   f2eq0_F1,       "fpnmpy.s2",  no_f2_F1},
	{EM_FPNMA_S3_F1_F3_F4_F2,   f2eq0_F1,       "fpnmpy.s3",  no_f2_F1},
	{EM_FSETC_S0_AMASK7_OMASK7, 0,              0,            hex_imm},
	{EM_FSETC_S1_AMASK7_OMASK7, 0,              0,            hex_imm},
	{EM_FSETC_S2_AMASK7_OMASK7, 0,              0,            hex_imm},
	{EM_FSETC_S3_AMASK7_OMASK7, 0,              0,            hex_imm},
	{EM_HINT_B_IMM21,           0,              0,            hint_imm},
	{EM_HINT_F_IMM21,           0,              0,            hint_imm},
	{EM_HINT_I_IMM21,           0,              0,            hint_imm},
	{EM_HINT_M_IMM21,           0,              0,            hint_imm},
	{EM_HINT_X_IMM62,           0,              0,            hint_imm},
	{EM_MOVL_R1_IMM64,          0,              0,            imm64_X2},
	{EM_MOV_B1_R2_TAG13,        tag13eq0_I21,   "mov",        no_tag_I21},
	{EM_MOV_PR_R2_MASK17,       mask17_all,     0,            no_mask17},
	{EM_MUX2_R1_R2_MHTYPE8,     0,              0,            hex_imm},
	{EM_NOP_B_IMM21,            0,              0,            hex_imm},
	{EM_NOP_F_IMM21,            0,              0,            hex_imm},
	{EM_NOP_I_IMM21,            0,              0,            hex_imm},
	{EM_NOP_M_IMM21,            0,              0,            hex_imm},
	{EM_NOP_X_IMM62,            0,              0,            hex_imm},
	{EM_OR_R1_IMM8_R3,          0,              0,            hex_imm},
	{EM_TBIT_Z_P1_P2_R3_POS6,     p1eq0_p2ne0,  "tbit.nz",    swap_p1p2},
	{EM_TBIT_Z_UNC_P1_P2_R3_POS6, p1eq0_p2ne0,  "tbit.nz.unc",swap_p1p2},
	{EM_TBIT_Z_AND_P1_P2_R3_POS6, p1eq0_p2ne0,  0,            swap_p1p2},
	{EM_TBIT_NZ_AND_P1_P2_R3_POS6,p1eq0_p2ne0,  0,            swap_p1p2},
	{EM_TBIT_Z_OR_P1_P2_R3_POS6,  p1eq0_p2ne0,  0,            swap_p1p2},
	{EM_TBIT_NZ_OR_P1_P2_R3_POS6, p1eq0_p2ne0,  0,            swap_p1p2},
	{EM_TBIT_Z_OR_ANDCM_P1_P2_R3_POS6,  p1eq0_p2ne0,  "tbit.nz.and.orcm",
	 swap_p1p2},
	{EM_TBIT_NZ_OR_ANDCM_P1_P2_R3_POS6, p1eq0_p2ne0,  "tbit.z.and.orcm",
	 swap_p1p2},
	{EM_TNAT_Z_P1_P2_R3,           p1eq0_p2ne0, "tnat.nz",    swap_p1p2},
	{EM_TNAT_Z_UNC_P1_P2_R3,       p1eq0_p2ne0, "tnat.nz.unc",swap_p1p2},
	{EM_TNAT_Z_AND_P1_P2_R3,       p1eq0_p2ne0, 0,            swap_p1p2},
	{EM_TNAT_NZ_AND_P1_P2_R3,      p1eq0_p2ne0, 0,            swap_p1p2},
	{EM_TNAT_Z_OR_P1_P2_R3,        p1eq0_p2ne0, 0,            swap_p1p2},
	{EM_TNAT_NZ_OR_P1_P2_R3,       p1eq0_p2ne0, 0,            swap_p1p2},
	{EM_TNAT_Z_OR_ANDCM_P1_P2_R3,  p1eq0_p2ne0, "tnat.nz.and.orcm",
	 swap_p1p2},
	{EM_TNAT_NZ_OR_ANDCM_P1_P2_R3, p1eq0_p2ne0, "tnat.z.and.orcm",
	 swap_p1p2},
	{EM_XMA_HU_F1_F3_F4_F2,     f2eq0_F2,       "xmpy.hu",    no_f2_F2},
	{EM_XMA_H_F1_F3_F4_F2,      f2eq0_F2,       "xmpy.h",     no_f2_F2},
	{EM_XMA_L_F1_F3_F4_F2,      f2eq0_F2,       "xmpy.l",     no_f2_F2},
	{EM_XOR_R1_IMM8_R3,         0,              0,            hex_imm},
	{EM_INST_NONE,              0,              0,            0},
    };

    if (dasOpts & DasNoMnemonicPad) {
	p = buf;
	if (opnds->qp != 0)
	    p += sprint_u(p, "(p", opnds->qp, ") ");
    } else {
	if (opnds->qp == 0)
	    (void)strcpy(buf, "      ");
	else if (opnds->qp < 10)	/* one digit */
	    (void)sprint_u(buf, "(p", opnds->qp, ")  ");
	else			/* two digits */
	    (void)sprint_u(buf, "(p", opnds->qp, ") ");
	p = &buf[6];
    }
    mnem = dasInstrs[instID].mnem;

    /* check for pseudo-ops, if enabled */
    if ((dasOpts & DasPseudoOps) && (i = dasInstrs[instID].pseudo))
	while (instID == pseudo[i].instID) {
	    if (!pseudo[i].chkFn || pseudo[i].chkFn(opnds)) {
		if (pseudo[i].mnem)
		    mnem = pseudo[i].mnem;
		if (pseudo[i].adjFn)
		    pseudo[i].adjFn(opnds);
		break;
	    }
	    i++;
	}

    if (opnds->u.op[0].type) {	/* one or more operands */
	if (dasOpts & DasNoMnemonicPad)
	    p += sprint_s(p, mnem, " ", 0);
	else
	    p += sprint_mnem(p, mnem);

	for (o = 0; (o < MAX_OPERANDS && opnds->u.op[o].type); o++) {
	    if (o > 0) {
		/* skip second pred if 0 */
		if ((dasOpts & DasPseudoOps) &&
		    opnds->u.op[o - 1].type == EM_OPTYPE_PREG &&
		    opnds->u.op[o].type == EM_OPTYPE_PREG &&
		    opnds->u.op[o].bits == 0)
		    o++;
		if (opnds->u.op[o - 1].role != opnds->u.op[o].role)
		    sep = dasEqualSep;
		else
		    sep = dasSep;
		p += sprint_s(p, sep, 0, 0);
	    }
	    bits = opnds->u.op[o].bits;
	    switch (opnds->u.op[o].type) {
	    case EM_OPTYPE_IREG:	/* integer register */
	    case EM_OPTYPE_IREG_R0_3:	/* r0-r3 */
	    case EM_OPTYPE_IREG_R0:	/* r0 */
	    case EM_OPTYPE_IREG_NUM:	/* ireg number - for chk.a/invala */
	    case EM_OPTYPE_IREG_R1_127:	/* r1-r127 */
		p += sprint_s(p, grName(regStr, (unsigned)bits), 0, 0);
		break;
	    case EM_OPTYPE_FREG:	/* FP register */
	    case EM_OPTYPE_FREG_F2_127:	/* f2-f127 */
	    case EM_OPTYPE_FREG_NUM:	/* freg number - for chk.a/invala */
		reg = (unsigned)bits;
		if ((dasOpts & DasRegNames) && (reg >= 8 && reg <= 15))
		    p += sprint_u(p, "farg", reg - 8, 0);
		else
		    p += sprint_u(p, "f", reg, 0);
		break;
	    case EM_OPTYPE_BR:		/* branch register */
		reg = (unsigned)bits;
		if ((dasOpts & DasRegNames) && (reg == 0))
		    p += sprint_s(p, "rp", 0, 0);
		else
		    p += sprint_u(p, "b", reg, 0);
		break;
	    case EM_OPTYPE_IP:		/* instruction pointer */
		p += sprint_s(p, "ip", 0, 0);
		break;
	    case EM_OPTYPE_PREGS_ALL:	/* predicate register file */
		p += sprint_s(p, "pr", 0, 0);
		break;
	    case EM_OPTYPE_PREGS_ROT:	/* rotating predicates */
		p += sprint_s(p, "pr.rot", 0, 0);
		break;
	    case EM_OPTYPE_PREG:	/* predicate register */
		p += sprint_u(p, "p", (unsigned)bits, 0);
		break;
	    case EM_OPTYPE_APP_REG:		/* Application register */
	    case EM_OPTYPE_APP_REG_GRP_LOW:	/* ARs 0-63 */
	    case EM_OPTYPE_APP_REG_GRP_HIGH:	/* ARs 64-127 */
	    case EM_OPTYPE_APP_CCV:		/* ar.ccv */
	    case EM_OPTYPE_APP_PFS:		/* ar.pfs */
	    case EM_OPTYPE_APP_CSD:		/* ar.csd */
		if (dasOpts & DasRegNames)
		    p += sprint_s(p, "ar", arStr[(unsigned)bits], 0);
		else
		    p += sprint_u(p, "ar", (unsigned)bits, 0);
		break;
	    case EM_OPTYPE_PSR:		/* psr */
		p += sprint_s(p, "psr", 0, 0);
		break;
	    case EM_OPTYPE_PSR_L:	/* psr.l */
		p += sprint_s(p, "psr.l", 0, 0);
		break;
	    case EM_OPTYPE_PSR_UM:	/* psr.um */
		p += sprint_s(p, "psr.um", 0, 0);
		break;
	    case EM_OPTYPE_CR:		/* control register */
		if (dasOpts & DasRegNames)
		    p += sprint_s(p, "cr", crStr[(unsigned)bits], 0);
		else
		    p += sprint_u(p, "cr", (unsigned)bits, 0);
		break;
	    case EM_OPTYPE_PMC:		/* indirect register files */
		p += sprint_s(p, "pmc[", grName(regStr, (unsigned)bits), "]");
		break;
	    case EM_OPTYPE_PMD:
		p += sprint_s(p, "pmd[", grName(regStr, (unsigned)bits), "]");
		break;
	    case EM_OPTYPE_PKR:
		p += sprint_s(p, "pkr[", grName(regStr, (unsigned)bits), "]");
		break;
	    case EM_OPTYPE_RR:
		p += sprint_s(p, "rr[", grName(regStr, (unsigned)bits), "]");
		break;
	    case EM_OPTYPE_IBR:
		p += sprint_s(p, "ibr[", grName(regStr, (unsigned)bits), "]");
		break;
	    case EM_OPTYPE_DBR:
		p += sprint_s(p, "dbr[", grName(regStr, (unsigned)bits), "]");
		break;
	    case EM_OPTYPE_ITR:
		p += sprint_s(p, "itr[", grName(regStr, (unsigned)bits), "]");
		break;
	    case EM_OPTYPE_DTR:
		p += sprint_s(p, "dtr[", grName(regStr, (unsigned)bits), "]");
		break;
	    case EM_OPTYPE_MSR:
		p += sprint_s(p, "msr[", grName(regStr, (unsigned)bits), "]");
		break;
	    case EM_OPTYPE_CPUID:
		p += sprint_s(p, "cpuid[",
			      grName(regStr, (unsigned)bits), "]");
		break;
	    case EM_OPTYPE_SIMM:	/* signed immediate */
	    case EM_OPTYPE_SEMAPHORE_INC:   /* immediate which encodes the
					       values -16, -8, -4, -1, 1, 4,
					       8, or 16 only - for fetchadd */
		symLen = dasWidth - trailLen - (p - buf) - (stop ? 2 : 0);
		if (dasRelocation(dasAddress() | slot, &reloc))
		    p += sprint_s(p, relocStr(targetStr, &reloc), 0, 0);
		else if ((sbits = (long long)bits) >= MIN_DAS_DECIMAL &&
			 sbits <= MAX_DAS_DECIMAL)
		    p += sprintf(p, "%lld", sbits);
		else
		    p += sprintf(p, "0x%llx", bits);
		break;
	    case EM_OPTYPE_SSHIFT_REL:	/* IP relative signed immediate */
		addr = dasAddress();
		symLen = dasWidth - trailLen - (p - buf) - (stop ? 2 : 0);
		if (first && opnds->u.op[o+1].type == EM_OPTYPE_SSHIFT_REL)
		    symLen = (symLen + 1 - strlen(dasSep)) / 2;
		/* check for relocation except for brp "tag" operand */
		if (first && dasRelocation(addr | slot, &reloc))
		    p += sprint_s(p, relocStr(targetStr, &reloc), 0, 0);
		else
#if 1
		    p += sprint_s(p, ipRelStr(targetStr, addr, bits), 0, 0);
#else
		    p += sprint_s(p, ipRelStr(targetStr, addr|slot, bits), 0, 0);
#endif
		first = NO;
		break;
	    case EM_OPTYPE_UIMM:	/* unsigned immediate */
	    case EM_OPTYPE_UDEC:	/* unsigned immediate which was
					   incremented by 1 */
	    case EM_OPTYPE_CCOUNT:	/* (31 - count) - for pshl */
	    case EM_OPTYPE_CPOS:	/* (63 - pos) - for  dep */
	    case EM_OPTYPE_ONE:		/* the number 1 - not encoded */
	    case EM_OPTYPE_EIGHT:	/* the number 8 - not encoded */
	    case EM_OPTYPE_SIXTEEN:	/* the number 16 - not encoded */
	    case EM_OPTYPE_CMP_UIMM:	/* unsigned immediate of cmp ltu */
	    case EM_OPTYPE_CMP4_UIMM:	/* unsigned immediate of cmp4 ltu */
	    case EM_OPTYPE_ALLOC_IOL:	/* ins, locals, and outs: can be 0-96 -
					   for alloc */
	    case EM_OPTYPE_ALLOC_ROT:	/* rotating: can be 0-96, must
					   be left-shifted by 3 - for alloc */
	    case EM_OPTYPE_COUNT_123:	/* immediate which can have the values
					   1, 2, or 3 only */
	    case EM_OPTYPE_COUNT_PACK:	/* immediate which has the values 0,
					   7, 15, or 16 only - for pmpyshr */
		if (bits <= MAX_DAS_DECIMAL)
		    p += sprint_u(p, 0, (unsigned)bits, 0);
		else
		    p += sprintf(p, "0x%llx", bits);
		break;
	    case EM_OPTYPE_MUX1:	/* immediate which can have the values
					   0, 8, 9, 0xa, or 0xb - for mux1 */
		p += sprint_s(p, mbtype4Str[(unsigned)bits], 0, 0);
		break;
	    case EM_OPTYPE_FCLASS:	/* fclass immediate */
		/* XXX - need to handle cases where "@" notation will be
		   misinterpreted by the assembler so we need to print the
		   class in hex instead */
		if (bits & 0x100ULL) {
		    p += sprint_s(p, "@nat", 0, 0);
		    first = NO;
		}
		if (bits & 0x080ULL)
		    p += sprint_s(p, first ? first = NO, "" : "|", "@qnan", 0);
		if (bits & 0x040ULL)
		    p += sprint_s(p, first ? first = NO, "" : "|", "@snan", 0);
		if (!((dasOpts & DasPseudoOps) &&
		      ((bits & 0x003ULL) == 0x003ULL) && (bits & 0x03cULL))) {
		    if (bits & 0x001ULL)
			p += sprint_s(p, first ? first = NO, "" : "|",
				      "@pos", 0);
		    if (bits & 0x002ULL)
			p += sprint_s(p, first ? first = NO, "" : "|",
				      "@neg", 0);
		}
		if (!((dasOpts & DasPseudoOps) && (bits & 0x003ULL) &&
		      ((bits & 0x03cULL) == 0x03cULL))) {
		    if (bits & 0x004ULL)
			p += sprint_s(p, first ? first = NO, "" : "|",
				      "@zero", 0);
		    if (bits & 0x008ULL)
			p += sprint_s(p, first ? first = NO, "" : "|",
				      "@unorm", 0);
		    if (bits & 0x010ULL)
			p += sprint_s(p, first ? first = NO, "" : "|",
				      "@norm", 0);
		    if (bits & 0x020ULL)
			p += sprint_s(p, first ? first = NO, "" : "|",
				      "@inf", 0);
		}
		break;
	    case EM_OPTYPE_IMM64:	/* movl immediate */
		if (dasOpts & DasNoSymMovlImm)
		    p += sprintf(p, "0x%llx", bits);
		else if (dasRelocation(dasAddress() | slot, &reloc))
		    p += sprint_s(p, relocStr(targetStr, &reloc), 0, 0);
		else {
		    symLen = dasWidth - trailLen - (p - buf) -
			(stop ? 2 : 0);
		    symAddrtoName(bits, targetStr, 0, YES, -symLen);
		    p += sprint_s(p, targetStr, 0, 0);
		}
		break;
	    case EM_OPTYPE_HINT_IMM:	/* hint immediate */
		if (bits == 0) {
		    p += sprint_s(p, "@pause", 0, 0);
		    break;
		}
		/* no break */
	    case EM_OPTYPE_SSHIFT_1:	/* signed immediate - for mov pr */
	    case EM_OPTYPE_SSHIFT_16:	/* signed immediate - for mov pr.rot */
	    case EM_OPTYPE_HIMM:	/* hex immediate */
		p += sprintf(p, "0x%llx", bits);
		break;
	    case EM_OPTYPE_MEM:		/* memory address */
		p += sprint_s(p, "[", grName(regStr, (unsigned)bits), "]");
		break;
	    default:			/* unknown optype */
		p += sprint_u(p, "?", opnds->u.op[o].type, "?");
	    }
	}
    } else			/* no operands */
	p += sprint_s(p, mnem, 0, 0);
    if (stop)
	(void)strcpy(p, ";;");
}

static char unitName(const Unit unit, DasOptions opts)
{
    char ret;

    switch(unit) {
    case I_Unit:
	ret = 'I';
	break;
    case M_Unit:
	ret = 'M';
	break;
    case F_Unit:
	ret = 'F';
	break;
    case B_Unit:
	ret = 'B';
	break;
    case L_Unit:
	ret = 'L';
	break;
    case X_Unit:
	ret = 'X';
	break;
    default:
	ret = '?';
	break;
    }

    return (opts & DasTemplateLC) ? tolower(ret) : ret;
}

static size_t templName(const TemplateInfoPtr t, DasOptions opts, char name[])
{
    size_t i = 1;

    name[0] = unitName(t->slot[0].unit, opts);
    if ((opts & DasTemplateCommas) && t->slot[0].stop)
	name[i++] = ',';
    name[i++] = unitName(t->slot[1].unit, opts);
    if ((opts & DasTemplateCommas) && t->slot[1].stop)
	name[i++] = ',';
    name[i++] = unitName(t->slot[2].unit, opts);
    if ((opts & DasTemplateCommas) && t->slot[2].stop)
	name[i++] = ',';
    name[i] = '\0';
    return i;
}

static void hexAddr(ADDR addr, char *str)
{
    sprintf(str, "%08x", (unsigned int)(addr >> 32));
    str[8] = '.';
    sprintf(str+9, "%08x", (unsigned int)(addr & 0xffffffff));
}

void dasBundleSBT(Bundle *bndl, char *i0Str, char *i1Str, char *i2Str,
		  SB sb[SLOTS_PER_BUNDLE], char *tStr)
{
    TemplateInfoPtr t;
    DecodedInstr instr[SLOTS_PER_BUNDLE];
    char *formatName, tName[7];
    SB semi = (dasOpts & DasSepStopLines) ? SB_Cont : SB_Stop;
    ADDR addr;
    size_t iLen, tLen;
    t = bundle_decode(bndl, instr, 1);
    if (addrLen) {
	addr = dasAddress();
	hexAddr(addr, i0Str);
	i0Str[17] = ' ';
    }
    iDasm(0, instr[0].instID, &instr[0].opnds, i0Str+addrLen,
	  semi & t->slot[0].stop);
    if (templLen || formatLen) {
	iLen = strlen(i0Str);
	(void)memset(i0Str+iLen, (int)' ', dasWidth-iLen);
    }
    if (commentLen)	/* add comment chars to i0Str */
	(void)memset(i0Str+dasWidth-templLen-sepLen-formatLen-commentLen,
		     '/', 2);
    if (templLen) {	/* add template info to i0Str */
	tLen = templName(t, dasOpts, tName);
	(void)memcpy(i0Str+dasWidth-templLen-sepLen-formatLen, tName, tLen);
    }
    if (formatLen) {	/* add format number to i0Str */
	formatName = instFormatName(instr[0].instID);
	(void)strcpy(i0Str+dasWidth-formatLen, formatName);
    } else		/* terminate i0Str */
	*(i0Str+dasWidth) = '\0';
    if (t->slot[1].unit == L_Unit && (instr[2].opnds.flags & EM_FLAG_TWO_SLOT))
	i1Str[0] = '\0';
    else {
	if (addrLen)
	    (void)memset(i1Str, (int)' ', addrLen);
	iDasm(1, instr[1].instID, &instr[1].opnds, i1Str+addrLen,
	      semi & t->slot[1].stop);
	if (formatLen) {      /* add format number to i1Str */
	    iLen = strlen(i1Str);
	    formatName = instFormatName(instr[1].instID);
	    (void)memset(i1Str+iLen, (int)' ', dasWidth-formatLen-iLen);
	    if (commentLen)
		(void)memset(i1Str+dasWidth-templLen-sepLen-formatLen-commentLen,
			     (int)'/', 2);
	    (void)strcpy(i1Str+dasWidth-formatLen, formatName);
	}
    }
    if (addrLen)
	    (void)memset(i2Str, (int)' ', addrLen);
    iDasm(2, instr[2].instID, &instr[2].opnds, i2Str+addrLen,
	  semi & t->slot[2].stop);
    if (formatLen) {      /* add format number to i2Str */
	iLen = strlen(i2Str);
	formatName = instFormatName(instr[2].instID);
	(void)memset(i2Str+iLen, (int)' ', dasWidth-formatLen-iLen);
	if (commentLen)
	    (void)memset(i2Str+dasWidth-templLen-sepLen-formatLen-commentLen,
			 '/', 2);
	(void)strcpy(i2Str+dasWidth-formatLen, formatName);
    }
    if (sb) {
	sb[0] = t->slot[0].stop;
	sb[1] = t->slot[1].stop;
	sb[2] = t->slot[2].stop;
    }
    if (tStr)
	templName(t, DasTemplateLC, tStr);
}

char *hexBundle(Bundle *bndl, char *str)
{
    BundleParts parts;

    parts = bundle_parts(bndl);
    (void)sprintf(str, "%x %d %011llx %011llx %011llx",
		 parts.templSB >> 1, parts.templSB & 1,
		 parts.slot[0], parts.slot[1], parts.slot[2]);
    return str;
}

void dasInit(DasOptions opts, int width)
{
    dasOpts = opts;
    dasWidth = width;
    if (opts & DasSpaceOperands) {
	dasSep = ", ";
	dasEqualSep = " = ";
    } else {
	dasSep = ",";
	dasEqualSep = "=";
    }
    addrLen = (dasOpts & DasHexAddresses) ? 18 : 0;
    commentLen = ((dasOpts & DasTemplateComment) == DasTemplateComment ||
		  (dasOpts & DasFormatComment) == DasFormatComment) ? 3 : 0;
    templLen = (dasOpts & DasTemplate) ?
	((dasOpts & DasTemplateCommas) ? 5 : 3) : 0;
    formatLen = (dasOpts & DasFormatNumbers) ? 5 : 0;
    sepLen = (formatLen && templLen) ? 1 : 0;
    trailLen = commentLen + templLen + sepLen + formatLen;
}

char *dasInst(Unit unit, const unsigned long long instBits,
	      const unsigned long long extraInstBits, char *iStr)
{
    InstID instID;
    Operands opnds;

    if (unit == L_Unit || unit == X_Unit) {
	instID = instr_decode(L_Unit, extraInstBits, &opnds);
	instID = instr_decode(X_Unit, instBits, &opnds);
    } else {
	if (unit == A_Unit)	/* real unit is M or I - arbitrarily pick I */
	    unit = I_Unit;
	instID = instr_decode(unit, instBits, &opnds);
	if (opnds.flags & EM_FLAG_TWO_SLOT)
	    instID = EM_ILLEGALOP;
    }

    /* XXX - what slot should be passed here? */
    iDasm(0, instID, &opnds, iStr, SB_Cont);
    return iStr;
}

char *dasEncodedInst(EncodedInstPtr inst, char *iStr)
{
    InstID instID;
    Operands opnds;
    Unit unit = inst->unit;

    if (unit == L_Unit || unit == X_Unit) {
	instID = instr_decode(L_Unit, inst->extra_bits, &opnds);
	instID = instr_decode(X_Unit, inst->bits, &opnds);
    } else {
	if (unit == A_Unit)	/* real unit is M or I - arbitrarily pick I */
	    unit = I_Unit;
	instID = instr_decode(unit, inst->bits, &opnds);
	if (opnds.flags & EM_FLAG_TWO_SLOT)
	    instID = EM_ILLEGALOP;
    }

    iDasm(inst->slot, instID, &opnds, iStr, inst->stop);
    return iStr;
}

unsigned int bundleTargets(Bundle *bndl, unsigned long long tgtAdr[])
{
    unsigned int s, num = 0;
    unsigned long long addr;
    short tgtOp, tagOp;
    TemplateInfoPtr t;
    DecodedInstr instr[SLOTS_PER_BUNDLE];
    DasRelocation reloc;

    t = bundle_decode(bndl, instr, 0);
    for (s = 0; s < SLOTS_PER_BUNDLE; s++) {
	tgtOp = dasInstrs[instr[s].instID].tgtOp;
	tagOp = dasInstrs[instr[s].instID].tagOp;
	if (tgtOp != OPINDEX_NONE || tagOp != OPINDEX_NONE) {
	    /* XXX - should there be an operand_decode() to avoid re-decoding
	       the instruction? */
	    (void)instr_decode(t->slot[s].unit, instr[s].instrBits,
			       &instr[s].opnds);
	    addr = dasAddress();
	    if (tgtOp != OPINDEX_NONE && !dasRelocation(addr | s, &reloc))
		tgtAdr[num++] = addr + instr[s].opnds.u.op[tgtOp].bits;
	    if (tagOp != OPINDEX_NONE &&
		!(instr[s].instID == EM_MOV_B1_R2_TAG13 &&
		  instr[s].opnds.u.op[tagOp].bits == 0))
		tgtAdr[num++] = addr + instr[s].opnds.u.op[tagOp].bits;
	}
    }
    return num;
}
