/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* DO NOT EDIT - Automatically generated Thu Aug 23 21:05:39 MDT 2007 using:
 gawk ../../src/encodings/encoding.opcode
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

/* Operands header file for encoder/decoder library */

#ifndef _SKI_OPERANDS_H
#define _SKI_OPERANDS_H

#ifndef MAX_OPERANDS
#define MAX_OPERANDS	6
#endif

typedef unsigned long Flags;

typedef enum {
	EM_OPTYPE_NONE = 0,
	EM_OPTYPE_IREG = 0x20,		/* Integer register */
	EM_OPTYPE_IREG_R0_3,		/* r0-r3 */
	EM_OPTYPE_IREG_R0,		/* Integer register R0 */
	EM_OPTYPE_IREG_R1_127,		/* r1-r127 */
	EM_OPTYPE_IREG_NUM,		/* ireg number - for chk.a/invala */
	EM_OPTYPE_FREG = 0x40,		/* FP register */
	EM_OPTYPE_FREG_F2_127,		/* f2-f127 */
	EM_OPTYPE_FREG_NUM,		/* freg number - for chk.a/invala */
	EM_OPTYPE_BR = 0x60,		/* branch register */
	EM_OPTYPE_IP = 0x80,		/* instruction pointer */
	EM_OPTYPE_PREGS_ALL,		/* predicate register file */
	EM_OPTYPE_PREGS_ROT,		/* rotating predicates */
	EM_OPTYPE_PREG = 0xa0,		/* predicate register */
	EM_OPTYPE_APP_REG = 0xc0,	/* application register */
	EM_OPTYPE_APP_REG_GRP_LOW,	/* application registers 0-63 */
	EM_OPTYPE_APP_REG_GRP_HIGH,	/* application registers 64-127 */
	EM_OPTYPE_APP_CCV,		/* ar.ccv */
	EM_OPTYPE_APP_PFS,		/* ar.pfs */
	EM_OPTYPE_APP_CSD,		/* ar.csd */
	EM_OPTYPE_PSR = 0xe0,		/* psr */
	EM_OPTYPE_PSR_L,		/* psr.l */
	EM_OPTYPE_PSR_UM,		/* psr.um */
	EM_OPTYPE_CR = 0x100,		/* control register */
	EM_OPTYPE_IND_REG_FILE = 0x120,	/* indirect register files */
	EM_OPTYPE_PMC,
	EM_OPTYPE_PMD,
	EM_OPTYPE_PKR,
	EM_OPTYPE_RR,
	EM_OPTYPE_IBR,
	EM_OPTYPE_DBR,
	EM_OPTYPE_ITR,
	EM_OPTYPE_DTR,
	EM_OPTYPE_MSR,
	EM_OPTYPE_CPUID,
	EM_OPTYPE_IMM = 0x140,		/* generic immediate */
	EM_OPTYPE_SIMM,			/* signed immediate */
	EM_OPTYPE_SEMAPHORE_INC,	/* immediate which can have the values
					   -16, -8, -4, -1, 1, 4, 8, or 16 
					   only - for fetchadd */
	EM_OPTYPE_SSHIFT_1,		/* signed immediate which has to be
					   shifted 1 bit - for mov pr */
	EM_OPTYPE_SSHIFT_16,		/* signed immediate which has to be
					   shifted 16 bits - for mov pr.rot */
	EM_OPTYPE_SSHIFT_REL,		/* IP relative signed immediate
					   which is shifted by 4 */
	EM_OPTYPE_UIMM,			/* unsigned immediate */
	EM_OPTYPE_UDEC,			/* unsigned immediate which has to be
					   decremented by 1 */
	EM_OPTYPE_CCOUNT,		/* (31 - count) - for pshl */
	EM_OPTYPE_CPOS,			/* (63 - pos) - for  dep */
	EM_OPTYPE_ONE,			/* the number 1 */
	EM_OPTYPE_EIGHT,		/* the number 8 */
	EM_OPTYPE_SIXTEEN,		/* the number 16 */
	EM_OPTYPE_CMP_UIMM,		/* unsigned immediate of cmp ltu */
	EM_OPTYPE_CMP4_UIMM,		/* unsigned immediate of cmp4 ltu */
	EM_OPTYPE_ALLOC_IOL,		/* ins, locals, and outs: can be 0-96 -
					   for alloc */
	EM_OPTYPE_ALLOC_ROT,		/* rotating: can be 0-96 by 8, must
					   be right-shifted by 3 - for alloc */
	EM_OPTYPE_COUNT_123,		/* immediate which can have the values
					   1, 2, or 3 only and has to be
					   decremented by 1 */
	EM_OPTYPE_COUNT_PACK,		/* immediate which can have the values
					   0, 7, 15, or 16 only -
					   for pmpyshr */
	EM_OPTYPE_PMPYSHR = EM_OPTYPE_COUNT_PACK,
	EM_OPTYPE_MUX1,			/* immediate which can have the values
					   0, 8, 9, 0xa, or 0xb - for mux1 */
	EM_OPTYPE_FCLASS,		/* fclass immediate */
	EM_OPTYPE_IMM64,		/* movl immediate */
	EM_OPTYPE_HIMM,			/* hex immediate */
	EM_OPTYPE_HINT_IMM,		/* hint immediate */
	EM_OPTYPE_MEM = 0x160		/* memory address */
} Operand_type;

typedef enum {
	EM_OPROLE_NONE = 0,
	EM_OPROLE_SRC,
	EM_OPROLE_DEST
} Operand_role;

typedef struct {
    unsigned long long	bits;
    Operand_type	type;
    Operand_role	role;
} Operand, *OperandPtr;

typedef struct {
    union {
	Operand op[MAX_OPERANDS];

	struct { Operand r1,r2,r3; } A1;
	struct { Operand r1,r2,count2,r3; } A2;
	struct { Operand r1,imm8,r3; } A3;
	struct { Operand r1,imm14,r3; } A4;
	struct { Operand r1,imm22,r3; } A5;
	struct { Operand p1,p2,r2,r3; } A6;
	struct { Operand p1,p2,r0,r3; } A7;
	struct { Operand p1,p2,imm8,r3; } A8;
	struct { Operand r1,r2,r3; } A9;
	struct { Operand r1,r2,count2,r3; } A10;
	struct { Operand r1,r2,r3,count2; } I1;
	struct { Operand r1,r2,r3; } I2;
	struct { Operand r1,r2,mbtype4; } I3;
	struct { Operand r1,r2,mhtype8; } I4;
	struct { Operand r1,r3,r2; } I5;
	struct { Operand r1,r3,count5; } I6;
	struct { Operand r1,r2,r3; } I7;
	struct { Operand r1,r2,count5; } I8;
	struct { Operand r1,r3; } I9;
	struct { Operand r1,r2,r3,count6; } I10;
	struct { Operand r1,r3,pos6,len6; } I11;
	struct { Operand r1,r2,pos6,len6; } I12;
	struct { Operand r1,imm8,pos6,len6; } I13;
	struct { Operand r1,imm1,r3,pos6,len6; } I14;
	struct { Operand r1,r2,r3,pos6,len4; } I15;
	struct { Operand p1,p2,r3,pos6; } I16;
	struct { Operand p1,p2,r3; } I17;
	struct { Operand imm21; } I18;
	struct { Operand imm21; } I19;
	struct { Operand r2,target25; } I20;
	struct { Operand b1,r2,tag13; } I21;
	struct { Operand r1,b2; } I22;
	struct { Operand pr,r2,mask17; } I23;
	struct { Operand pr_rot,imm44; } I24;
	struct { Operand r1,ip; } I25;
	struct { Operand ar3,r2; } I26;
	struct { Operand ar3,imm8; } I27;
	struct { Operand r1,ar3; } I28;
	struct { Operand r1,r3; } I29;
	struct { Operand r1,r3; } M1;
	struct { Operand r1,r3,r2; } M2;
	struct { Operand r1,r3,imm9; } M3;
	struct { Operand r3,r2; } M4;
	struct { Operand r3,r2,imm9; } M5;
	struct { Operand f1,r3; } M6;
	struct { Operand f1,r3,r2; } M7;
	struct { Operand f1,r3,imm9; } M8;
	struct { Operand r3,f2; } M9;
	struct { Operand r3,f2,imm9; } M10;
	struct { Operand f1,f2,r3; } M11;
	struct { Operand f1,f2,r3,imm; } M12;
	struct { Operand r3; } M13;
	struct { Operand r3,r2; } M14;
	struct { Operand r3,imm9; } M15;
	struct { Operand r1,r3,r2,ar_ccv; } M16;
	struct { Operand r1,r3,inc3; } M17;
	struct { Operand f1,r2; } M18;
	struct { Operand r1,f2; } M19;
	struct { Operand r2,target25; } M20;
	struct { Operand f2,target25; } M21;
	struct { Operand r1,target25; } M22;
	struct { Operand f1,target25; } M23;
	struct { Operand r1; } M26;
	struct { Operand f1; } M27;
	struct { Operand r3; } M28;
	struct { Operand ar3,r2; } M29;
	struct { Operand ar3,imm8; } M30;
	struct { Operand r1,ar3; } M31;
	struct { Operand cr3,r2; } M32;
	struct { Operand r1,cr3; } M33;
	struct { Operand r1,ar_pfs,i,l,o,r; } M34;
	struct { Operand psr_l,r2; } M35;
	struct { Operand r1,psr; } M36;
	struct { Operand imm21; } M37;
	struct { Operand r1,r3,r2; } M38;
	struct { Operand r1,r3,imm2; } M39;
	struct { Operand r3,imm2; } M40;
	struct { Operand r2; } M41;
	struct { Operand r3,r2; } M42;
	struct { Operand r1,r3; } M43;
	struct { Operand imm24; } M44;
	struct { Operand r3,r2; } M45;
	struct { Operand r1,r3; } M46;
	struct { Operand r3; } M47;
	struct { Operand imm21; } M48;
	struct { Operand target25; } B1;
	struct { Operand target25; } B2;
	struct { Operand b1,target25; } B3;
	struct { Operand b2; } B4;
	struct { Operand b1,b2; } B5;
	struct { Operand target25,tag13; } B6;
	struct { Operand b2,tag13; } B7;
	struct { Operand imm21; } B9;
	struct { Operand f1,f3,f4,f2; } F1;
	struct { Operand f1,f3,f4,f2; } F2;
	struct { Operand f1,f3,f4,f2; } F3;
	struct { Operand p1,p2,f2,f3; } F4;
	struct { Operand p1,p2,f2,fclass9; } F5;
	struct { Operand f1,p2,f2,f3; } F6;
	struct { Operand f1,p2,f3; } F7;
	struct { Operand f1,f2,f3; } F8;
	struct { Operand f1,f2,f3; } F9;
	struct { Operand f1,f2; } F10;
	struct { Operand f1,f2; } F11;
	struct { Operand amask7,omask7; } F12;
	struct { Operand target25; } F14;
	struct { Operand imm21; } F15;
	struct { Operand imm21; } F16;
	struct { Operand imm62; } X1;
	struct { Operand r1,imm64; } X2;
	struct { Operand target64; } X3;
	struct { Operand b1,target64; } X4;
	struct { Operand imm62; } X5;
    } u;
    unsigned char qp;
    Flags flags;
} Operands, *OperandsPtr;

#endif /* _OPERANDS_H */
