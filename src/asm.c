/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Mini-Assembler
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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm.h"
#include "decoder.h"
#include "encoder.h"
#include "std.h"
#include "libsym.h"
#include "types.h"
#include "fields.h"
#include "state.h"
#include "ssDCmd.h"
#include "ssDPrs.h"

typedef enum { AsmStart, AsmPred, AsmMnem, AsmOperands, AsmAssign } AsmState;

typedef struct {
    unsigned char symType;
    Operand_type  opType;
} SymInfo, *SymInfoPtr;

static SymInfo grInfo =    { 'R',  EM_OPTYPE_IREG };
static SymInfo frInfo =    { 'F',  EM_OPTYPE_FREG };
static SymInfo brInfo =    { 'B',  EM_OPTYPE_BR };
static SymInfo prInfo =    { 'P',  EM_OPTYPE_PREG };
static SymInfo arInfo =    { 'A',  EM_OPTYPE_APP_REG };
static SymInfo crInfo =    { 'C',  EM_OPTYPE_CR };
static SymInfo labelInfo = { 'I',  EM_OPTYPE_SSHIFT_REL };
static SymInfo immInfo =   { 'I',  EM_OPTYPE_IMM };

static struct {
    char *name;
    unsigned long long value;
    SymInfo info;
} sym[] = {
    {"gp",	 GP_ID, {'R',  EM_OPTYPE_IREG}},
    {"sp",	 SP_ID, {'R' , EM_OPTYPE_IREG}},
    {"rp",	     0, {'B',  EM_OPTYPE_BR}},
    {"ip",	     0, {'\0', EM_OPTYPE_IP}},
    {"pr",	     0, {'\0', EM_OPTYPE_PREGS_ALL}},
    {"psr",	     0, {'\0', EM_OPTYPE_PSR}},
    {"psr.l",	     0, {'\0', EM_OPTYPE_PSR_L}},
    {"psr.um",	     0, {'\0', EM_OPTYPE_PSR_UM}},
    {"pr.rot",	     0, {'\0', EM_OPTYPE_PREGS_ROT}},
    {"ar.k0",	 K0_ID, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.k1",	 K1_ID, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.k2",	 K2_ID, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.k3",	 K3_ID, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.k4",	 K4_ID, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.k5",	 K5_ID, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.k6",	 K6_ID, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.k7",	 K7_ID, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.rsc",	    16, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.bsp",	    17, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.bspstore", 18, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.rnat",	    19, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.ccv",	    32, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.unat",	    36, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.fpsr",	    40, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.itc",	    44, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.pfs",	    64, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.lc",	    65, {'A',  EM_OPTYPE_APP_REG}},
    {"ar.ec",	    66, {'A',  EM_OPTYPE_APP_REG}},
    {"@nat",	 0x100, {'I',  EM_OPTYPE_IMM}},
    {"@qnan",	 0x080, {'I',  EM_OPTYPE_IMM}},
    {"@snan",	 0x040, {'I',  EM_OPTYPE_IMM}},
    {"@pos",	 0x001, {'I',  EM_OPTYPE_IMM}},
    {"@neg",	 0x002, {'I',  EM_OPTYPE_IMM}},
    {"@zero",	 0x004, {'I',  EM_OPTYPE_IMM}},
    {"@unorm",	 0x008, {'I',  EM_OPTYPE_IMM}},
    {"@norm",	 0x010, {'I',  EM_OPTYPE_IMM}},
    {"@inf",	 0x020, {'I',  EM_OPTYPE_IMM}},
    {"@alt",	   0xa, {'I',  EM_OPTYPE_IMM}},
    {"@brcst",	     0, {'I',  EM_OPTYPE_IMM}},
    {"@mix",	     8, {'I',  EM_OPTYPE_IMM}},
    {"@rev",	   0xb, {'I',  EM_OPTYPE_IMM}},
    {"@shuf",	     9, {'I',  EM_OPTYPE_IMM}},
    {"cpuid",	     0, {'\0', EM_OPTYPE_CPUID}},
    {"dbr",	     0, {'\0', EM_OPTYPE_DBR}},
    {"dtr",	     0, {'\0', EM_OPTYPE_DTR}},
    {"ibr",	     0, {'\0', EM_OPTYPE_IBR}},
    {"itr",	     0, {'\0', EM_OPTYPE_ITR}},
    {"msr",	     0, {'\0', EM_OPTYPE_MSR}},
    {"pkr",	     0, {'\0', EM_OPTYPE_PKR}},
    {"pmc",	     0, {'\0', EM_OPTYPE_PMC}},
    {"pmd",	     0, {'\0', EM_OPTYPE_PMD}},
    {"rr",	     0, {'\0', EM_OPTYPE_RR}},
    {"cr.dcr",	     0, {'C',  EM_OPTYPE_CR}},
    {"cr.itm",	     1, {'C',  EM_OPTYPE_CR}},
    {"cr.iva",	     2, {'C',  EM_OPTYPE_CR}},
    {"cr.pta",	     8, {'C',  EM_OPTYPE_CR}},
    {"cr.gpta",	     9, {'C',  EM_OPTYPE_CR}},
    {"cr.ipsr",	    16, {'C',  EM_OPTYPE_CR}},
    {"cr.isr",	    17, {'C',  EM_OPTYPE_CR}},
    {"cr.iip",	    19, {'C',  EM_OPTYPE_CR}},
    {"cr.ifa",	    20, {'C',  EM_OPTYPE_CR}},
    {"cr.itir",	    21, {'C',  EM_OPTYPE_CR}},
    {"cr.iipa",	    22, {'C',  EM_OPTYPE_CR}},
    {"cr.ifs",	    23, {'C',  EM_OPTYPE_CR}},
    {"cr.iim",	    24, {'C',  EM_OPTYPE_CR}},
    {"cr.iha",	    25, {'C',  EM_OPTYPE_CR}},
    {"cr.lid",	    64, {'C',  EM_OPTYPE_CR}},
    {"cr.ivr",	    65, {'C',  EM_OPTYPE_CR}},
    {"cr.tpr",	    66, {'C',  EM_OPTYPE_CR}},
    {"cr.eoi",	    67, {'C',  EM_OPTYPE_CR}},
    {"cr.irr0",	    68, {'C',  EM_OPTYPE_CR}},
    {"cr.irr1",	    69, {'C',  EM_OPTYPE_CR}},
    {"cr.irr2",	    70, {'C',  EM_OPTYPE_CR}},
    {"cr.irr3",	    71, {'C',  EM_OPTYPE_CR}},
    {"cr.itv",	    72, {'C',  EM_OPTYPE_CR}},
    {"cr.pmv",	    73, {'C',  EM_OPTYPE_CR}},
    {"cr.cmcv",	    74, {'C',  EM_OPTYPE_CR}},
    {"cr.lrr0",	    80, {'C',  EM_OPTYPE_CR}},
    {"cr.lrr1",	    81, {'C',  EM_OPTYPE_CR}}
};

#define INIT_SYM_SIZE	(sizeof(sym)/sizeof(sym[0]))

static SymTablePtr asmST;

AsmStatus asm_init(void)
{
    unsigned i;
    char name[10];

    asmST = symNewTable();
    for (i = 0; i < INIT_SYM_SIZE; i++)
	symInsertX(asmST, sym[i].name, sym[i].value, &sym[i].info);

    for (i = 0; i < NPRS; i++) {
	(void)sprintf(name, "p%u", i);
	symInsertX(asmST, name, (unsigned long long)i, &prInfo);
    }

    for (i = 0; i < (GR_STK_BASE+N_STACK_VIRT); i++) {
	(void)sprintf(name, "r%u", i);
	symInsertX(asmST, name, (unsigned long long)i, &grInfo);
    }

    for (i = 0; i < NFRS; i++) {
	(void)sprintf(name, "f%u", i);
	symInsertX(asmST, name, (unsigned long long)i, &frInfo);
    }

    for (i = 0; i < NARS; i++) {
	(void)sprintf(name, "ar%u", i);
	symInsertX(asmST, name, (unsigned long long)i, &arInfo);
    }

    for (i = 0; i < NCRS; i++) {
	(void)sprintf(name, "cr%u", i);
	symInsertX(asmST, name, (unsigned long long)i, &crInfo);
    }

    for (i = 0; i < NBRS; i++) {
	(void)sprintf(name, "b%u", i);
	symInsertX(asmST, name, (unsigned long long)i, &brInfo);
    }

    return AsmOK;
}

AsmStatus asm_inst(const char *str, InstID *instID, OperandsPtr ops,
		   EncodedInstPtr inst)
{
    char *tok, *isp, *p, instStr[80], buf[BUFSIZ];
    unsigned long long val;
    unsigned op_num = 0, equal = 0, mem, indirect;
    SymInfoPtr symInfo, indSymInfo;
    AsmIdPtr asmID;
    AsmState state = AsmStart;

    (void)strcpy(buf, str);
    instStr[0] = '\0';
    isp = instStr;

    for (tok = strtok(buf, " \t"); (tok); tok = strtok(NULL, " \t,")) {
	/* XXX - add support for labels & tags */
	/* XXX - deal with buffer overruns */
	if (tok[0] == '/' && tok[1] == '/')		/* comment */
	    break;
	switch (state) {
	case AsmStart:
	case AsmPred:
	    if (tok[0] == '(' && tok[1] == 'p') {	/* qp/bp */
		ops->qp = (unsigned char)strtoul(&tok[2], &p, 10);
		if (p == &tok[2])
		    return AsmBadQP;
		state = AsmMnem;
	    } else if (tok[0] == ';' && tok[1] == ';') {	/* stop */
		if (tok[2] == '\0') {
		    inst->stop = SB_Stop;
		    return AsmStop;
		} else
		    return AsmBadInst;
	    } else if ((p = strchr(tok, ':'))) {		/* label/tag */
		*p = '\0';
		if (tok[0] == '[')	/* tag */
		    tok++;
		symInsertX(asmST, tok, encoderIP(), &labelInfo);
	    } else {					/* assume mnemonic */
		ops->qp = 0;
		(void)strcpy(instStr, tok);
		isp += strlen(tok);
		state = AsmOperands;
	    }
	    break;
	case AsmMnem:
	    (void)strcpy(instStr, tok);
	    isp += strlen(tok);
	    state = AsmOperands;
	    break;
	case AsmOperands:
	    mem = indirect = 0;
	    if (tok[0] == '=') {
		if (equal)
		    return AsmBadEquals;
		else {
		    equal = 1;
		    if (op_num == 0)
			state = AsmAssign;
		    continue;
		}
        }
	    if (op_num == 0) {
		*isp++ = '_';
		*isp = '\0';
	    }
	    if ((p = strchr(tok, '['))){	/* mem/indirect reg file */
		if (p == tok) {			/* mem */
		    mem = 1;
		    tok++;
		    if ((p = strchr(tok, ']')))
			*p = '\0';
		    else
			return AsmBadOperand + op_num;
		} else {			/* indirect reg file */
		    /* XXX - finish this */
		    indirect = 1;
		    *p = '\0';
		    if (symNameLookupX(asmST, tok, &val,
				       (void *)&indSymInfo)) {
			(void)strcat(instStr, tok);
			isp += strlen(tok);
			tok = p + 1;
			if ((p = strchr(tok, ']')))
			    *p = '\0';
			else
			    return AsmBadOperand + op_num;
		    } else
			return AsmBadOperand + op_num;
		}
        }
	    /* XXX - add expression evaluation & symbol lookup */
	    val = strtoull(tok, &p, 0);
	    if (p != tok) {
		ops->u.op[op_num].bits = val;
		ops->u.op[op_num].type = EM_OPTYPE_IMM;
		*isp++ = 'I';
		*isp = '\0';
	    } else if (symNameLookupX(asmST, tok, &ops->u.op[op_num].bits,
				      (void *)&symInfo)) {
		ops->u.op[op_num].type = symInfo->opType;
		if (mem) {
		    ops->u.op[op_num].type = EM_OPTYPE_MEM;
		    *isp++ = 'M';
		    *isp = '\0';
		} else if (indirect) {
		    ops->u.op[op_num].type = indSymInfo->opType;
		} else if (symInfo->symType) {
		    *isp++ = symInfo->symType;
		    *isp = '\0';
		} else {
		    (void)strcat(instStr, tok);
		    isp += strlen(tok);
		}
	    } else if (evalExpr(tok, HEXEXP, &ops->u.op[op_num].bits)) {
		ops->u.op[op_num].type = EM_OPTYPE_IMM;
		*isp++ = 'I';
		*isp = '\0';
	    } else
		return AsmBadOperand + op_num;
	    op_num++;
	    break;
	case AsmAssign:
	    val = strtoull(tok, &p, 0);
	    if (p != tok)
		symInsertX(asmST, instStr, val, &immInfo);
	    else if (symNameLookupX(asmST, tok, &val, (void *)&symInfo))
		symInsertX(asmST, instStr, val, symInfo);
	    else if (symNameLookup(tok, &val, (void *)&symInfo))
		symInsertX(asmST, instStr, val, &labelInfo);
	    else
		return AsmBadAssign;
	    break;
	}
    }

    /* check for blank/comment/assignment line */
    if (state == AsmStart || state == AsmAssign)
	return AsmNoInst;

    /* pad out remaining operands */
    while (op_num < MAX_OPERANDS)
	ops->u.op[op_num++].type = EM_OPTYPE_NONE;

    if (!(asmID = inst_lookup(instStr)))
	return AsmBadInst;
    *instID = asmID->id;
    return (AsmStatus)encode_inst(asmID->id, ops, inst);
}

/* XXX - this should be defined in the encoder or decoder library */
static Unit unitType(char type)
{
    switch(type) {
	case 'A':
	    return A_Unit;
	case 'B':
	    return B_Unit;
	case 'F':
	    return F_Unit;
	case 'I':
	    return I_Unit;
	case 'L':
	    return L_Unit;
	case 'M':
	    return M_Unit;
	case 'X':
	    return X_Unit;
    }
    return No_Unit;
}

AsmStatus asm_templ(const char *str, unsigned char *templ)
{
    typedef enum { TemplUnit, TemplStopOrUnit } TemplState;
    TemplateInfo tinfo;
    Unit unit;
    unsigned s = 0;
    TemplState state = TemplUnit;

    /* initialize tinfo to an illegal state */
    tinfo.slot[0].unit = tinfo.slot[1].unit = tinfo.slot[2].unit = A_Unit;

    if (*str == '.')	/* skip leading "." */
	str++;

    /* scan str and fill in tinfo */
    while (*str) {
	if (s == SLOTS_PER_BUNDLE)
	    return AsmBadTemplate;

	switch (state) {
	case TemplUnit:		/* expecting a unit char */
	    if ((unit = unitType(toupper(*str))) == No_Unit)
		return AsmBadTemplate;
	    tinfo.slot[s].unit = unit;
	    tinfo.slot[s].stop = SB_Cont;
	    state = TemplStopOrUnit;
	    break;
	case TemplStopOrUnit:	/* expecting a comma or a unit char */
	    if (*str == ',') {
		tinfo.slot[s++].stop = SB_Stop;
		state = TemplUnit;
	    } else {
		if (s == SLOTS_PER_BUNDLE-1)
		    return AsmBadTemplate;
		if ((unit = unitType(toupper(*str))) == No_Unit)
		    return AsmBadTemplate;
		tinfo.slot[++s].unit = unit;
		tinfo.slot[s].stop = SB_Cont;
	    }
	    break;
	}
	str++;
    }

    /* convert "xLI" to "xLX" */
    if (tinfo.slot[1].unit == L_Unit && tinfo.slot[2].unit == I_Unit)
	tinfo.slot[2].unit = X_Unit;

    /* search for matching template */
    if ((*templ = templSB_from_info(&tinfo)) >= 32)
	return AsmBadTemplate;

    return AsmOK;
}
