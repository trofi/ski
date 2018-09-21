/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- 
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
#include <assert.h>
#include <encoder.h>
#include <_decoder.h>
#include "EMInst.h"

#define deposit(val, pos, len) \
    (((unsigned long long)(val) << (64 - (len)) >> (64 - (len) - (pos))))

#define ILL_SEM		8
static unsigned char sem_bits[] = {
/*	  0	   1	    2	     3	      4	       5	6	 7 */
          4, ILL_SEM, ILL_SEM, ILL_SEM, ILL_SEM, ILL_SEM, ILL_SEM, ILL_SEM,
/*	  8	   9	   10	    11       12	      13       14	15 */
	  5, ILL_SEM, ILL_SEM, ILL_SEM,       6, ILL_SEM, ILL_SEM,       7,
/*	 16	  17	   18	    19       20	      21       22	23 */
    ILL_SEM,       3, ILL_SEM, ILL_SEM,       2, ILL_SEM, ILL_SEM, ILL_SEM,
/*	 24	  25	   26	    27       28	      29       30	31 */
          1, ILL_SEM, ILL_SEM, ILL_SEM, ILL_SEM, ILL_SEM, ILL_SEM, ILL_SEM,
/*	 32 */
          0
};

EncStatus encode_inst(InstID instID, OperandsPtr opnds,
		      EncodedInstPtr inst)
{
    EMDB_info *inst_info;
    EMDB_encode *inst_format;
    unsigned long long bits, op_bits, i = 0, l = 0;
    unsigned int o, mf, adv, f;

    if (instID <= EM_INST_NONE || instID >= EM_INST_LAST)
	return EncBadInstID;

    inst_info = &InstTable[instID];
    inst_format = &EncodingTable[inst_info->format];
    inst->unit = inst_info->unit;
    /* XXX - need to check various inst_info->flags for validity */

    /* start with the base encoding of the instruction */
    bits = inst_info->base_encoding;

    /* if the instruction is predicated, the qualifying/branch predicate is  
       passed in as opnds->qp */

    /* two-slot instructions (movl/brl) are special */
    if (inst_info->flags & EM_FLAG_TWO_SLOT) {
	inst->extra_bits = 0;		/* real value filled in below */
	inst->valid_extra_bits = 1;
    } else
	inst->valid_extra_bits = 0;

    if (inst_info->flags & EM_FLAG_PRED) {
	if (opnds->qp > 63)
	    return EncBadQP;
	bits |= deposit(opnds->qp, EM_PREDICATE_POS, EM_PREDICATE_LEN);
    } else if (opnds->qp)
	return EncBadQP;

    /* the remaining fields are specified by the operand layout encoding */

    for (f = mf = o = 0; o < MAX_OPERANDS; o++, f += adv) {
#ifdef OPERAND_COUNT
	if (o >= opnds->count)
	    if (inst_info->op[o] == EM_OPTYPE_NONE)
		continue;
	    else
		return EncBadOperand + o;
#endif /* OPERAND_COUNT */
	if (!((opnds->u.op[o].type == inst_info->op[o]) ||
	      (opnds->u.op[o].type == OPCLASS(inst_info->op[o]))))
	    return EncBadOperand + o;
	op_bits = opnds->u.op[o].bits;	/* default op_bits */
	adv = 0;	/* assume we will not advance to next field */
	switch (inst_info->op[o]) {
	    /* the following operand types are not encoded in any instr */
	case EM_OPTYPE_NONE:
	case EM_OPTYPE_IP:		/* instruction pointer */
	case EM_OPTYPE_PREGS_ALL:	/* predicate register file */
	case EM_OPTYPE_PREGS_ROT:	/* rotating predicates */
	case EM_OPTYPE_PSR:		/* psr */
	case EM_OPTYPE_PSR_L:		/* psr.l */
	case EM_OPTYPE_PSR_UM:		/* psr.um */
	    continue;
	case EM_OPTYPE_IREG_R0:		/* Integer register r0: not encoded */
	    if (op_bits != 0)
		return EncBadOperand + o;
	    continue;
	case EM_OPTYPE_APP_CCV:		/* ar.ccv: not encoded */
	    if (op_bits != 32)
		return EncBadOperand + o;
	    continue;
	case EM_OPTYPE_APP_PFS:		/* ar.pfs: not encoded */
	    if (op_bits != 64)
		return EncBadOperand + o;
	    continue;
	case EM_OPTYPE_APP_CSD:		/* ar.csd: not encoded */
	    if (op_bits != 25)
		return EncBadOperand + o;
	    continue;
	case EM_OPTYPE_ONE:		/* the number 1 - not encoded */
	    if (op_bits != 1)
		return EncBadOperand + o;
	    continue;
	case EM_OPTYPE_EIGHT:		/* the number 8 - not encoded */
	    if (op_bits != 8)
		return EncBadOperand + o;
	    continue;
	case EM_OPTYPE_SIXTEEN:		/* the number 16 - not encoded */
	    if (op_bits != 16)
		return EncBadOperand + o;
	    continue;
	case EM_OPTYPE_IREG:		/* Integer register */
	case EM_OPTYPE_FREG:		/* FP register */
	case EM_OPTYPE_CR:		/* control register */
	case EM_OPTYPE_PMC:		/* indirect register files */
	case EM_OPTYPE_PMD:
	case EM_OPTYPE_PKR:
	case EM_OPTYPE_RR:
	case EM_OPTYPE_IBR:
	case EM_OPTYPE_DBR:
	case EM_OPTYPE_ITR:
	case EM_OPTYPE_DTR:
	case EM_OPTYPE_MSR:
	case EM_OPTYPE_CPUID:
	case EM_OPTYPE_IREG_NUM:	/* ireg number - for chk.a/invala */
	case EM_OPTYPE_FREG_NUM:	/* freg number - for chk.a/invala */
	case EM_OPTYPE_MEM:		/* memory address */
	    if (op_bits > 127)
		return EncBadOperand + o;
	    break;
	case EM_OPTYPE_IREG_R0_3:	/* r0-r3 */
	    if (op_bits > 3)
		return EncBadOperand + o;
	    break;
	case EM_OPTYPE_IREG_R1_127:	/* r1-r127 */
	    if (op_bits == 0 || op_bits > 127)
		return EncBadOperand + o;
	    break;
	case EM_OPTYPE_FREG_F2_127:	/* f2-f127 */
	    if (op_bits < 2 || op_bits > 127)
		return EncBadOperand + o;
	    break;
	case EM_OPTYPE_BR:		/* branch register */
	    if (op_bits > 7)
		return EncBadOperand + o;
	    break;
	case EM_OPTYPE_PREG:		/* predicate register */
	case EM_OPTYPE_APP_REG_GRP_LOW:		  /* ARs 0-63 */
	    if (op_bits > 63)
		return EncBadOperand + o;
	    break;
	case EM_OPTYPE_APP_REG_GRP_HIGH:	/* ARs 64-127 */
	    if (op_bits < 64 || op_bits > 127)
		return EncBadOperand + o;
	    break;
	case EM_OPTYPE_UIMM:		/* unsigned immediate */
	case EM_OPTYPE_FCLASS:		/* fclass immediate */
	    if (inst_format->op_layout[o].len < 64 &&
		op_bits >> inst_format->op_layout[o].len != 0)
		return EncBadOperand + o;
	    break;
	case EM_OPTYPE_SIMM:		/* signed immediate */
	case EM_OPTYPE_CMP_UIMM:	/* unsigned immediate of cmp ltu */
	case EM_OPTYPE_CMP4_UIMM:	/* unsigned immediate of cmp4 ltu */
	    /* XXX - need to check that imm fits in field */
	    break;
	case EM_OPTYPE_SSHIFT_REL:	/* IP relative signed immediate
					   which is shifted by 4 */
	    if (op_bits & 0xf)
		return EncBadOperand + o;
	    op_bits = (op_bits - encoderIP()) >> 4;
	    /* XXX - need to check that imm fits in field */
	    break;
	case EM_OPTYPE_SSHIFT_1:	/* signed immediate which has to be
					   shifted 1 bit - for mov pr */
	    op_bits >>= 1;
	    /* XXX - need to check that imm fits in field */
	    break;
	case EM_OPTYPE_SSHIFT_16:	/* signed immediate which has to be
					   shifted 16 bits - for mov pr.rot */
	    if (op_bits & 0xffff)
		return EncBadOperand + o;
	    op_bits >>= 16;
	    /* XXX - need to check that imm fits in field */
	    break;
	case EM_OPTYPE_COUNT_123:	/* immediate which can have the values
					   1, 2, or 3 only and has to be
					   decremented by 1 */
	    op_bits -= 1;
	    if (op_bits > 2)
		return EncBadOperand + o;
	    break;
	case EM_OPTYPE_COUNT_PACK:	/* immediate which can have the values
					   0, 7, 15, or 16 only -
					   for pmpyshr */
	    if (op_bits == 0)
		/* nothing to do */;
	    else if (op_bits == 7)
		op_bits = 1;
	    else if (op_bits == 15)
		op_bits = 2;
	    else if (op_bits == 16)
		op_bits = 3;
	    else
		return EncBadOperand + o;
	    break;
	case EM_OPTYPE_UDEC:		/* unsigned immediate which has to be
					   decremented by 1 */
	    op_bits -= 1;
	    if (inst_format->op_layout[o].len < 64 &&
		op_bits >> inst_format->op_layout[o].len != 0)
		return EncBadOperand + o;
	    break;
	case EM_OPTYPE_CCOUNT:         /* (31 - count) - for pshl */
	    if (op_bits > 31)
		return EncBadOperand + o;
	    op_bits = 31 - op_bits;
	    break;
	case EM_OPTYPE_CPOS:           /* (63 - pos) - for  dep */
	    if (op_bits > 63)
		return EncBadOperand + o;
	    op_bits = 63 - op_bits;
	    break;
	case EM_OPTYPE_SEMAPHORE_INC:	/* immediate which can have the values
					   -16, -8, -4, -1, 1, 4, 8, or 16 
					   only - for fetchadd */
	    if (op_bits + 16u > 32u || sem_bits[op_bits + 16u] == ILL_SEM)
		return EncBadOperand + o;
	    op_bits = sem_bits[op_bits + 16u];
	    break;
	case EM_OPTYPE_ALLOC_IOL:	/* ins, locals, and outs: can be 0-96 -
					   for alloc */
	    if (op_bits > 96)
		return EncBadOperand + o;
	    switch (o) {
	    case 2:				/* "i" */
		i = op_bits;			/* record it */
		continue;
	    case 3:				/* "l" */
		l = op_bits;			/* record it */
		op_bits = i + l;		/* form sol */
		break;
	    case 4:				/* "o" */
		op_bits = i + l + op_bits;	/* form sof */
		break;
	    }
	    break;
	case EM_OPTYPE_ALLOC_ROT:	/* rotating: can be 0-96 by 8, must
					   be right-shifted by 3 - for alloc */
	    if (op_bits > 96 || op_bits & 7)
		return EncBadOperand + o;
	    op_bits >>= 3;
	    break;
	case EM_OPTYPE_MUX1:		/* immediate which can have the values
					   0, 8, 9, 0xa, or 0xb - for mux1 */
	    if (!(op_bits == 0 || (op_bits >= 8 && op_bits <= 0xb)))
		return EncBadOperand + o;
	    break;
	default:
#ifdef DEBUG
	    fprintf(stderr, "unknown OPTYPE: %u\n", inst_info->op[o]);
#endif /* DEBUG */
	    return EncBadOperand + o;
	}

	if (inst_format->op_layout[f].pos != MF) {	/* single field */
	    if (inst_format->op_layout[f].len != 0)
		bits |= deposit(op_bits, inst_format->op_layout[f].pos,
				inst_format->op_layout[f].len);
	} else {					/* multi-field */
	    unsigned int len, pos, mfi = 0;

	    while ((op_bits != 0) &&
		   ((len = inst_format->fmtArrayptr[mf*FORMAT_SIZE+mfi++])
		    != 0)) {
		pos = inst_format->fmtArrayptr[mf*FORMAT_SIZE+mfi++];
		if (len >= (SLOT_SIZE - MAJOR_OP_SIZE))
		    /* the next bits go into extra_bits */
		    inst->extra_bits |= deposit(op_bits, pos, len);
		else
		    bits |= deposit(op_bits, pos, len);
		op_bits >>= len;
	    }
	    mf++;
	}
	adv = 1;	/* if we get here, advance to next field */
    }

    inst->bits = bits;
    return EncOK;
}
