/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * IA Decoding Routines
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


#include <limits.h>	/* Just for INT_MIN as DECODE_FAILED? */
#undef MIN
#undef MAX
#include "std.h"
#include "bits.h"
#include "types.h"
#include "fields.h"
#include "sim.h"
#include "memui.h"
#include "iadas_types.h"
#include "ia_exec.h"
#include "ia_das.h"
#include "sign_ext.h"

extern BYTE iAmode;

#define DECODE_FAILED	INT_MIN
#define overrideSize(x)	((x) ^= 6)	/* flips between IA16 (2) & IA32 (4) */

typedef enum {
    RegSrcEaDest,  RegSrcNoDest,
    EaSrcRegDest,  EaSrcNoDest,
    NoSrcEaDest,   NoSrcRegDest,
    LeaSrcRegDest,
    LfpSrcRegDest, LfpSrcNoDest,
    BoundSrcRegDest
} SrcDestType;

typedef const struct {
    int	execFn;
    BYTE	opSize;
    SrcDestType	sdt;
} GroupInfo, *GroupInfoPtr;

/* decode function */
typedef int (*PIADF)(ADDR4, struct ia_instinfo *);

static int ia_decode2(ADDR4 eip, IAinstInfoPtr info);
static const PIADF two_byte_opcode[256];


/* Utility Functions */

/* Fetch and Assemble a "size"-byte Immediate */

static int iAimm(ADDR4 eip, IADWORD *val, BYTE size)
{
    BYTE imm[4];

    if (!memMIAIRd(eip, imm, size))
	return DECODE_FAILED;

    switch (size) {
    case 1:
	*val = sign_ext32(imm[0], 8);
	break;
    case 2:
	*val = sign_ext32(imm[1]<<8 | imm[0], 16);
	break;
    case 4:
	*val = imm[3]<<24 | imm[2]<<16 | imm[1]<<8 | imm[0];
	break;
    }
    return size;
}

/* SIB  Decode Function */

static int sib_decode(ADDR4 eip, IAinstInfoPtr info, BYTE mod,
		      int *eaRdFn, int *eaWrFn, BYTE *defSeg)
{
    SIB  sib;
    int len = 1;

    if (!memMIAIRd(eip, (BYTE *)&sib, 1))
	return DECODE_FAILED;

    info->scale = 1 << sib.ss;
    info->index = (sib.index == ESP_REG) ? NO_REG : sib.index;
    info->base = sib.base;

    if (sib.base == ESP_REG)
	*defSeg = SS_ID;
    else if (sib.base == EBP_REG)
	if (mod == 0) {			/* disp32 */
	    len += iAimm(eip+1, &info->disp32, 4);
	    info->base = NO_REG;
	    *defSeg = DS_ID;
	} else				/* EBP */
	    *defSeg = SS_ID;
    else
	*defSeg = DS_ID;

    *eaRdFn = 0;
    *eaWrFn = 0;
    return len;
}

/* ModR/M Decode Tables */

static EaInfo ea16_info[3][8] = {		  /* Mod R/M */
    {{ BX_REG, SI_REG,	DS_ID, "%s[bx][si]"   },  /*  0   0  */
     { BX_REG, DI_REG,	DS_ID, "%s[bx][di]"   },  /*  0   1  */
     { BP_REG, SI_REG,	SS_ID, "%s[bp][si]"   },  /*  0   2  */
     { BP_REG, DI_REG,	SS_ID, "%s[bp][di]"   },  /*  0   3  */
     { SI_REG, NO_REG, 	DS_ID, "%s[si]"	      },  /*  0   4  */
     { DI_REG, NO_REG,	DS_ID, "%s[di]"	      },  /*  0   5  */
     { NO_REG, NO_REG,	DS_ID, "%s[%s]"	      },  /*  0   6  */
     { BX_REG, NO_REG,	DS_ID, "%s[bx]"	      }}, /*  0   7  */
    {{ BX_REG, SI_REG,	DS_ID, "%s%s[bx][si]" },  /*  1   0  */
     { BX_REG, DI_REG,	DS_ID, "%s%s[bx][di]" },  /*  1   1  */
     { BP_REG, SI_REG,	SS_ID, "%s%s[bp][si]" },  /*  1   2  */
     { BP_REG, DI_REG,	SS_ID, "%s%s[bp][di]" },  /*  1   3  */
     { SI_REG, NO_REG, 	DS_ID, "%s%s[si]"     },  /*  1   4  */
     { DI_REG, NO_REG,	DS_ID, "%s%s[di]"     },  /*  1   5  */
     { BP_REG, NO_REG,	SS_ID, "%s%s[bp]"     },  /*  1   6  */
     { BX_REG, NO_REG,	DS_ID, "%s%s[bx]"     }}, /*  1   7  */
    {{ BX_REG, SI_REG,	DS_ID, "%s%s[bx][si]" },  /*  2   0  */
     { BX_REG, DI_REG,	DS_ID, "%s%s[bx][di]" },  /*  2   1  */
     { BP_REG, SI_REG,	SS_ID, "%s%s[bp][si]" },  /*  2   2  */
     { BP_REG, DI_REG,	SS_ID, "%s%s[bp][di]" },  /*  2   3  */
     { SI_REG, NO_REG, 	DS_ID, "%s%s[si]"     },  /*  2   4  */
     { DI_REG, NO_REG,	DS_ID, "%s%s[di]"     },  /*  2   5  */
     { BP_REG, NO_REG,	SS_ID, "%s%s[bp]"     },  /*  2   6  */
     { BX_REG, NO_REG,	DS_ID, "%s%s[bx]"     }}  /*  2   7  */
};

static EaInfo ea32_info[3][8] = {		  /* Mod R/M */
    {{ EAX_REG, NO_REG,	DS_ID, "%s[eax]"      },  /*  0   0  */
     { ECX_REG, NO_REG,	DS_ID, "%s[ecx]"      },  /*  0   1  */
     { EDX_REG, NO_REG,	DS_ID, "%s[edx]"      },  /*  0   2  */
     { EBX_REG, NO_REG,	DS_ID, "%s[ebx]"      },  /*  0   3  */
     { NO_REG,	NO_REG,	DS_ID, "[--][--]"     },  /*  0   4  */ /* SIB */
     { NO_REG,	NO_REG,	DS_ID, "%s[%s]"	      },  /*  0   5  */
     { ESI_REG, NO_REG,	DS_ID, "%s[esi]"      },  /*  0   6  */
     { EDI_REG, NO_REG,	DS_ID, "%s[edi]"      }}, /*  0   7  */
    {{ EAX_REG, NO_REG,	DS_ID, "%s%s[eax]"    },  /*  1   0  */
     { ECX_REG, NO_REG,	DS_ID, "%s%s[ecx]"    },  /*  1   1  */
     { EDX_REG, NO_REG,	DS_ID, "%s%s[edx]"    },  /*  1   2  */
     { EBX_REG, NO_REG,	DS_ID, "%s%s[ebx]"    },  /*  1   3  */
     { NO_REG,	NO_REG,	DS_ID, "[--][--]"     },  /*  1   4  */ /* SIB */
     { EBP_REG, NO_REG,	SS_ID, "%s%s[ebp]"    },  /*  1   5  */
     { ESI_REG, NO_REG,	DS_ID, "%s%s[esi]"    },  /*  1   6  */
     { EDI_REG, NO_REG,	DS_ID, "%s%s[edi]"    }}, /*  1   7  */
    {{ EAX_REG, NO_REG,	DS_ID, "%s%s[eax]"    },  /*  2   0  */
     { ECX_REG, NO_REG,	DS_ID, "%s%s[ecx]"    },  /*  2   1  */
     { EDX_REG, NO_REG,	DS_ID, "%s%s[edx]"    },  /*  2   2  */
     { EBX_REG, NO_REG,	DS_ID, "%s%s[ebx]"    },  /*  2   3  */
     { NO_REG,	NO_REG,	DS_ID, "[--][--]"     },  /*  2   4  */ /* SIB */
     { EBP_REG, NO_REG,	SS_ID, "%s%s[ebp]"    },  /*  2   5  */
     { ESI_REG, NO_REG,	DS_ID, "%s%s[esi]"    },  /*  2   6  */
     { EDI_REG, NO_REG,	DS_ID, "%s%s[eax]"    }}  /*  2   7  */
};

EaInfoPtr ea16Info(ModRM modrm)
{
    if (modrm.mod < 3)
	return &ea16_info[modrm.mod][modrm.rm];
    return 0;
}

EaInfoPtr ea32Info(ModRM modrm)
{
    if (modrm.mod < 3)
	return &ea32_info[modrm.mod][modrm.rm];
    return 0;
}

/* ModR/M Decode Function */

static int modrm_decode(ADDR4 eip, IAinstInfoPtr info, int execFn,
			PIADASF dasFn, SrcDestType sdt)
{
    int regRdFn, eaRdFn;
    int regWrFn, eaWrFn;
    ModRM modrm;
    BYTE defSeg;
    int len = 1;

    if (!memMIAIRd(eip, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    info->modrm = modrm;
    info->reg = modrm.reg_op;
    switch (info->opSize) {
    case 1:
	regRdFn = 0;
	regWrFn = 0;
	break;
    case 2:
	regRdFn = 0;
	regWrFn = 0;
	break;
    case 4:
	regRdFn = 0;
	regWrFn = 0;
	break;
    }

    if (info->addrSize == IA16) {	/* 16-bit addressing forms */
	switch (modrm.mod) {
	case 0:
	    if (modrm.rm == DISP16_RM)		/* disp16 */
		len += iAimm(eip+1, &info->disp32, 2);
	    else
		info->disp32 = 0;
	    info->base = ea16_info[modrm.mod][modrm.rm].base;
	    info->index = ea16_info[modrm.mod][modrm.rm].index;
	    eaRdFn = 0;
	    eaWrFn = 0;
	    defSeg = ea16_info[modrm.mod][modrm.rm].defSeg;
	    break;
	case 1:
	case 2:
	    len += iAimm(eip+1, &info->disp32, modrm.mod);
	    info->base = ea16_info[modrm.mod][modrm.rm].base;
	    info->index = ea16_info[modrm.mod][modrm.rm].index;
	    eaRdFn = 0;
	    eaWrFn = 0;
	    defSeg = ea16_info[modrm.mod][modrm.rm].defSeg;
	    break;
	case 3:
	    info->base = modrm.rm;
	    switch (info->opSize) {
	    case 1:
		eaRdFn = 0;
		eaWrFn = 0;
		break;
	    case 2:
		eaRdFn = 0;
		eaWrFn = 0;
		break;
	    case 4:
		eaRdFn = 0;
		eaWrFn = 0;
		break;
	    }
	    break;
	}
    } else {				/* 32-bit addressing forms */
	switch (modrm.mod) {
	case 0:
	    info->disp32 = 0;
	    if (modrm.rm == SIB_RM)
		len += sib_decode(++eip, info, modrm.mod, &eaRdFn, &eaWrFn,
				  &defSeg);
	    else {
		if (modrm.rm == DISP32_RM)	/* disp32 */
		    len += iAimm(eip+1, &info->disp32, 4);
		info->base = ea32_info[modrm.mod][modrm.rm].base;
		info->index = ea32_info[modrm.mod][modrm.rm].index;
		eaRdFn = 0;
		eaWrFn = 0;
		defSeg = ea32_info[modrm.mod][modrm.rm].defSeg;
	    }
	    break;
	case 1:
	case 2:
	    if (modrm.rm == SIB_RM)
		len += sib_decode(++eip, info, modrm.mod, &eaRdFn, &eaWrFn,
				  &defSeg);
	    else {
		info->base = ea32_info[modrm.mod][modrm.rm].base;
		info->index = ea32_info[modrm.mod][modrm.rm].index;
		eaRdFn = 0;
		eaWrFn = 0;
		defSeg = ea32_info[modrm.mod][modrm.rm].defSeg;
	    }
	    len += iAimm(eip+1, &info->disp32, 3 * modrm.mod - 2);
	    break;
	case 3:
	    info->base = modrm.rm;
	    switch (info->opSize) {
	    case 1:
		eaRdFn = 0;
		eaWrFn = 0;
		break;
	    case 2:
		eaRdFn = 0;
		eaWrFn = 0;
		break;
	    case 4:
		eaRdFn = 0;
		eaWrFn = 0;
		break;
	    }
	    break;
	}
    }

    /* fill in info src/dest rd/wr functions based on source/dest type */
    switch (sdt) {
    case RegSrcEaDest:
	info->srcRdFn  = regRdFn;
	info->destRdFn = eaRdFn;
	info->destWrFn = eaWrFn;
	break;
    case RegSrcNoDest:
	info->srcRdFn  = regRdFn;
	info->destRdFn = 0;
	info->destWrFn = 0;
	break;
    case EaSrcRegDest:
	info->srcRdFn  = eaRdFn;
	info->destRdFn = regRdFn;
	info->destWrFn = regWrFn;
	break;
    case EaSrcNoDest:
	info->srcRdFn  = eaRdFn;
	info->destRdFn = 0;
	info->destWrFn = 0;
	break;
    case NoSrcEaDest:
	info->srcRdFn  = 0;
	info->destRdFn = eaRdFn;
	info->destWrFn = eaWrFn;
	break;
    case NoSrcRegDest:
	info->srcRdFn  = 0;
	info->destRdFn = regRdFn;
	info->destWrFn = regWrFn;
	break;
    case LeaSrcRegDest:
	/* change srcRdFn from the standard memory read functions that
	 * were set up above to special functions that just return the
	 * effective address */
	if (eaRdFn == 0)
	    info->srcRdFn = 0;
	else if (eaRdFn == 0)
	    info->srcRdFn = 0;
	else	/* 0 */
	    info->srcRdFn = 0;
	info->destRdFn = regRdFn;
	info->destWrFn = regWrFn;
	break;
    case LfpSrcRegDest:
	/* change srcRdFn from the standard memory read functions that
	 * were set up above to special functions that use opSize + 2
	 * for the size of the memory read */
	if (eaRdFn == 0)
	    info->srcRdFn = 0;
	else if (eaRdFn == 0)
	    info->srcRdFn = 0;
	else	/* 0 */
	    info->srcRdFn = 0;
	info->destRdFn = regRdFn;
	info->destWrFn = regWrFn;
	break;
    case LfpSrcNoDest:
	/* change srcRdFn from the standard memory read functions that
	 * were set up above to special functions that use opSize + 2
	 * for the size of the memory read */
	if (eaRdFn == 0)
	    info->srcRdFn = 0;
	else if (eaRdFn == 0)
	    info->srcRdFn = 0;
	else	/* 0 */
	    info->srcRdFn = 0;
	info->destRdFn = 0;
	info->destWrFn = 0;
	break;
    case BoundSrcRegDest:
	/* change srcRdFn from the standard memory read functions that
	 * were set up above to special functions that read both bound
	 * values */
	if (eaRdFn == 0)
	    info->srcRdFn = 0;
	else if (eaRdFn == 0)
	    info->srcRdFn = 0;
	else	/* 0 */
	    info->srcRdFn = 0;
	info->destRdFn = regRdFn;
	info->destWrFn = regWrFn;
	break;
    }
    if (!info->segment)		/* no segment override or SIB */
	info->segment = defSeg;
    info->execFn = execFn;
    info->dasFn = dasFn;
    return len;
}

/* Decode a Reg, Imm Instruction */

static int iAregImm(ADDR4 eip, IAinstInfoPtr info, int execFn,
		    PIADASF dasFn, BYTE reg)
{
    info->srcRdFn = 0;
    switch (info->opSize) {
    case 1:
	info->destRdFn = 0;
	info->destWrFn = 0;
	break;
    case 2:
	info->destRdFn = 0;
	info->destWrFn = 0;
	break;
    case 4:
	info->destRdFn = 0;
	info->destWrFn = 0;
	break;
    }
    info->reg = reg;
    info->execFn = execFn;
    info->dasFn = dasFn;
    return 1 + iAimm(eip+1, &info->imm32, info->opSize);
}

/* Decode a Reg Instruction */

static int iAreg(IAinstInfoPtr info, int execFn, PIADASF dasFn, BYTE reg)
{
    info->srcRdFn = 0;
    switch (info->opSize) {
    case 1:
	info->destRdFn = 0;
	info->destWrFn = 0;
	break;
    case 2:
	info->destRdFn = 0;
	info->destWrFn = 0;
	break;
    case 4:
	info->destRdFn = 0;
	info->destWrFn = 0;
	break;
    }
    info->reg = reg;
    info->execFn = execFn;
    info->dasFn = dasFn;
    return 1;
}

/* Decode a Reg, Reg Instruction */

static int iAregReg(IAinstInfoPtr info, int execFn, PIADASF dasFn,
		    BYTE dest, BYTE src)
{
    switch (info->opSize) {
    case 1:
	info->srcRdFn  = 0;
	info->destRdFn = 0;
	info->destWrFn = 0;
	break;
    case 2:
	info->srcRdFn  = 0;
	info->destRdFn = 0;
	info->destWrFn = 0;
	break;
    case 4:
	info->srcRdFn  = 0;
	info->destRdFn = 0;
	info->destWrFn = 0;
	break;
    }
    info->base = dest;
    info->reg = src;
    info->execFn = execFn;
    info->dasFn = dasFn;
    return 1;
}

/* Decode a Push Segment Register Instruction */

static int iApushSeg(IAinstInfoPtr info, BYTE src)
{
    info->srcRdFn = 0;
    info->reg = src;
    if (StackAddrSize(info->mode) == IA16) {
	info->destRdFn = 0;
	info->destWrFn = 0;
    } else {	/* IA32 */
	info->destRdFn = 0;
	info->destWrFn = 0;
    }
    info->execFn = 0;
    info->dasFn = push_seg_das;
    return 1;
}

/* Decode a Push Register Instruction */

static int iApushReg(IAinstInfoPtr info, BYTE src)
{
    if (info->opSize == IA16)
	info->srcRdFn = 0;
    else	/* IA32 */
	info->srcRdFn = 0;
    info->reg = src;
    if (StackAddrSize(info->mode) == IA16) {
	info->destRdFn = 0;
	info->destWrFn = 0;
    } else {	/* IA32 */
	info->destRdFn = 0;
	info->destWrFn = 0;
    }
    info->execFn = 0;
    info->dasFn = push_reg_das;
    return 1;
}

/* Decode a Pop Segment Register Instruction */

static int iApopSeg(IAinstInfoPtr info, BYTE dest)
{
    if (StackAddrSize(info->mode) == IA16)
	info->srcRdFn = 0;
    else	/* IA32 */
	info->srcRdFn = 0;
    info->destRdFn = 0;
    info->destWrFn = 0;
    info->cond_seg = dest;
    info->execFn = 0;
    info->dasFn = pop_seg_das;
    return 1;
}

/* Decode a Pop Register Instruction */

static int iApopReg(IAinstInfoPtr info, BYTE dest)
{
    if (StackAddrSize(info->mode) == IA16)
	info->srcRdFn = 0;
    else	/* IA32 */
	info->srcRdFn = 0;
    if (info->opSize == IA16) {
	info->destRdFn = 0;
	info->destWrFn = 0;
    } else {	/* IA32 */
	info->destRdFn = 0;
	info->destWrFn = 0;
    }
    info->reg = dest;
    info->execFn = 0;
    info->dasFn = pop_reg_das;
    return 1;
}


/* One-Byte Opcode Decode Functions */

static int add_EbGb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 00 */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, add_ExGx_das, RegSrcEaDest);
}

static int add_EvGv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 01 */
{
    return 1 + modrm_decode(eip+1, info, 0, add_ExGx_das, RegSrcEaDest);
}

static int add_GbEb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 02 */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, add_GxEx_das, EaSrcRegDest);
}

static int add_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 03 */
{
    return 1 + modrm_decode(eip+1, info, 0, add_GxEx_das, EaSrcRegDest);
}

static int add_ALIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 04 */
{
    info->opSize = 1;
    return iAregImm(eip, info, 0, add_reg_imm_das, AL_REG);
}

static int add_eAXIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 05 */
{
    return iAregImm(eip, info, 0, add_reg_imm_das, EAX_REG);
}

/* ARGSUSED */
static int push_ES_decode(ADDR4 eip, IAinstInfoPtr info)	/* 06 */
{
    return iApushSeg(info, ES_REG);
}

/* ARGSUSED */
static int pop_ES_decode(ADDR4 eip, IAinstInfoPtr info)		/* 07 */
{
    return iApopSeg(info, ES_REG);
}

static int or_EbGb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 08 */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, or_ExGx_das, RegSrcEaDest);
}

static int or_EvGv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 09 */
{
    return 1 + modrm_decode(eip+1, info, 0, or_ExGx_das, RegSrcEaDest);
}

static int or_GbEb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0A */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, or_GxEx_das, EaSrcRegDest);
}

static int or_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0B */
{
    return 1 + modrm_decode(eip+1, info, 0, or_GxEx_das, EaSrcRegDest);
}

static int or_ALIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0C */
{
    info->opSize = 1;
    return iAregImm(eip, info, 0, or_reg_imm_das, AL_REG);
}

static int or_eAXIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0D */
{
    return iAregImm(eip, info, 0, or_reg_imm_das, EAX_REG);
}

/* ARGSUSED */
static int push_CS_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0E */
{
    return iApushSeg(info, CS_REG);
}

static int two_byte_escape(ADDR4 eip, IAinstInfoPtr info)	/* 0F */
{
    BYTE op2;

    if (!memMIAIRd(eip+1, &op2, 1))
	return DECODE_FAILED;

    return two_byte_opcode[op2](eip+1, info) + 1;
}

static int adc_EbGb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 10 */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, adc_ExGx_das, RegSrcEaDest);
}

static int adc_EvGv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 11 */
{
    return 1 + modrm_decode(eip+1, info, 0, adc_ExGx_das, RegSrcEaDest);
}

static int adc_GbEb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 12 */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, adc_GxEx_das, EaSrcRegDest);
}

static int adc_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 13 */
{
    return 1 + modrm_decode(eip+1, info, 0, adc_GxEx_das, EaSrcRegDest);
}

static int adc_ALIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 14 */
{
    info->opSize = 1;
    return iAregImm(eip, info, 0, adc_reg_imm_das, AL_REG);
}

static int adc_eAXIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 15 */
{
    return iAregImm(eip, info, 0, adc_reg_imm_das, EAX_REG);
}

/* ARGSUSED */
static int push_SS_decode(ADDR4 eip, IAinstInfoPtr info)	/* 16 */
{
    return iApushSeg(info, SS_REG);
}

/* ARGSUSED */
static int pop_SS_decode(ADDR4 eip, IAinstInfoPtr info)		/* 17 */
{
    return iApopSeg(info, SS_REG);
}

static int sbb_EbGb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 18 */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, sbb_ExGx_das, RegSrcEaDest);
}

static int sbb_EvGv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 19 */
{
    return 1 + modrm_decode(eip+1, info, 0, sbb_ExGx_das, RegSrcEaDest);
}

static int sbb_GbEb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 1A */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, sbb_GxEx_das, EaSrcRegDest);
}

static int sbb_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 1B */
{
    return 1 + modrm_decode(eip+1, info, 0, sbb_GxEx_das, EaSrcRegDest);
}

static int sbb_ALIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 1C */
{
    info->opSize = 1;
    return iAregImm(eip, info, 0, sbb_reg_imm_das, AL_REG);
}

static int sbb_eAXIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 1D */
{
    return iAregImm(eip, info, 0, sbb_reg_imm_das, EAX_REG);
}

/* ARGSUSED */
static int push_DS_decode(ADDR4 eip, IAinstInfoPtr info)	/* 1E */
{
    return iApushSeg(info, DS_REG);
}

/* ARGSUSED */
static int pop_DS_decode(ADDR4 eip, IAinstInfoPtr info)		/* 1F */
{
    return iApopSeg(info, DS_REG);
}

static int and_EbGb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 20 */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, and_ExGx_das, RegSrcEaDest);
}

static int and_EvGv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 21 */
{
    return 1 + modrm_decode(eip+1, info, 0, and_ExGx_das, RegSrcEaDest);
}

static int and_GbEb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 22 */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, and_GxEx_das, EaSrcRegDest);
}

static int and_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 23 */
{
    return 1 + modrm_decode(eip+1, info, 0, and_GxEx_das, EaSrcRegDest);
}

static int and_ALIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 24 */
{
    info->opSize = 1;
    return iAregImm(eip, info, 0, and_reg_imm_das, AL_REG);
}

static int and_eAXIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 25 */
{
    return iAregImm(eip, info, 0, and_reg_imm_das, EAX_REG);
}

static int seg_ES_decode(ADDR4 eip, IAinstInfoPtr info)		/* 26 */
{
    info->segment = ES_ID;
    return ia_decode2(eip+1, info) + 1;
}

/* ARGSUSED */
static int daa_decode(ADDR4 eip, IAinstInfoPtr info)		/* 27 */
{
    info->execFn = 0;
    info->dasFn = daa_das;
    return 1;
}

static int sub_EbGb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 28 */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, sub_ExGx_das, RegSrcEaDest);
}

static int sub_EvGv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 29 */
{
    return 1 + modrm_decode(eip+1, info, 0, sub_ExGx_das, RegSrcEaDest);
}

static int sub_GbEb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 2A */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, sub_GxEx_das, EaSrcRegDest);
}

static int sub_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 2B */
{
    return 1 + modrm_decode(eip+1, info, 0, sub_GxEx_das, EaSrcRegDest);
}

static int sub_ALIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 2C */
{
    info->opSize = 1;
    return iAregImm(eip, info, 0, sub_reg_imm_das, AL_REG);
}

static int sub_eAXIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 2D */
{
    return iAregImm(eip, info, 0, sub_reg_imm_das, EAX_REG);
}

static int seg_CS_decode(ADDR4 eip, IAinstInfoPtr info)		/* 2E */
{
    info->segment = CS_ID;
    return ia_decode2(eip+1, info) + 1;
}

/* ARGSUSED */
static int das_decode(ADDR4 eip, IAinstInfoPtr info)		/* 2F */
{
    info->execFn = 0;
    info->dasFn = das_das;
    return 1;
}

static int xor_EbGb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 30 */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, xor_ExGx_das, RegSrcEaDest);
}

static int xor_EvGv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 31 */
{
    return 1 + modrm_decode(eip+1, info, 0, xor_ExGx_das, RegSrcEaDest);
}

static int xor_GbEb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 32 */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, xor_GxEx_das, EaSrcRegDest);
}

static int xor_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 33 */
{
    return 1 + modrm_decode(eip+1, info, 0, xor_GxEx_das, EaSrcRegDest);
}

static int xor_ALIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 34 */
{
    info->opSize = 1;
    return iAregImm(eip, info, 0, xor_reg_imm_das, AL_REG);
}

static int xor_eAXIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 35 */
{
    return iAregImm(eip, info, 0, xor_reg_imm_das, EAX_REG);
}

static int seg_SS_decode(ADDR4 eip, IAinstInfoPtr info)		/* 36 */
{
    info->segment = SS_ID;
    return ia_decode2(eip+1, info) + 1;
}

/* ARGSUSED */
static int aaa_decode(ADDR4 eip, IAinstInfoPtr info)		/* 37 */
{
    info->execFn = 0;
    info->dasFn = aaa_das;
    return 1;
}

static int cmp_EbGb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 38 */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, cmp_ExGx_das, RegSrcEaDest);
}

static int cmp_EvGv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 39 */
{
    return 1 + modrm_decode(eip+1, info, 0, cmp_ExGx_das, RegSrcEaDest);
}

static int cmp_GbEb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 3A */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, cmp_GxEx_das, EaSrcRegDest);
}

static int cmp_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 3B */
{
    return 1 + modrm_decode(eip+1, info, 0, cmp_GxEx_das, EaSrcRegDest);
}

static int cmp_ALIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 3C */
{
    info->opSize = 1;
    return iAregImm(eip, info, 0, cmp_reg_imm_das, AL_REG);
}

static int cmp_eAXIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 3D */
{
    return iAregImm(eip, info, 0, cmp_reg_imm_das, EAX_REG);
}

static int seg_DS_decode(ADDR4 eip, IAinstInfoPtr info)		/* 3E */
{
    info->segment = DS_ID;
    return ia_decode2(eip+1, info) + 1;
}

/* ARGSUSED */
static int aas_decode(ADDR4 eip, IAinstInfoPtr info)		/* 3F */
{
    info->execFn = 0;
    info->dasFn = aas_das;
    return 1;
}

/* ARGSUSED */
static int inc_eAX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 40 */
{
    return iAreg(info, 0, inc_reg_das, EAX_REG);
}

/* ARGSUSED */
static int inc_eCX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 41 */
{
    return iAreg(info, 0, inc_reg_das, ECX_REG);
}

/* ARGSUSED */
static int inc_eDX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 42 */
{
    return iAreg(info, 0, inc_reg_das, EDX_REG);
}

/* ARGSUSED */
static int inc_eBX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 43 */
{
    return iAreg(info, 0, inc_reg_das, EBX_REG);
}

/* ARGSUSED */
static int inc_eSP_decode(ADDR4 eip, IAinstInfoPtr info)	/* 44 */
{
    return iAreg(info, 0, inc_reg_das, ESP_REG);
}

/* ARGSUSED */
static int inc_eBP_decode(ADDR4 eip, IAinstInfoPtr info)	/* 45 */
{
    return iAreg(info, 0, inc_reg_das, EBP_REG);
}

/* ARGSUSED */
static int inc_eSI_decode(ADDR4 eip, IAinstInfoPtr info)	/* 46 */
{
    return iAreg(info, 0, inc_reg_das, ESI_REG);
}

/* ARGSUSED */
static int inc_eDI_decode(ADDR4 eip, IAinstInfoPtr info)	/* 47 */
{
    return iAreg(info, 0, inc_reg_das, EDI_REG);
}

/* ARGSUSED */
static int dec_eAX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 48 */
{
    return iAreg(info, 0, dec_reg_das, EAX_REG);
}

/* ARGSUSED */
static int dec_eCX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 49 */
{
    return iAreg(info, 0, dec_reg_das, ECX_REG);
}

/* ARGSUSED */
static int dec_eDX_decode(ADDR4 eip, IAinstInfoPtr info)       	/* 4A */
{
    return iAreg(info, 0, dec_reg_das, EDX_REG);
}

/* ARGSUSED */
static int dec_eBX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 4B */
{
    return iAreg(info, 0, dec_reg_das, EBX_REG);
}

/* ARGSUSED */
static int dec_eSP_decode(ADDR4 eip, IAinstInfoPtr info)	/* 4C */
{
    return iAreg(info, 0, dec_reg_das, ESP_REG);
}

/* ARGSUSED */
static int dec_eBP_decode(ADDR4 eip, IAinstInfoPtr info)	/* 4D */
{
    return iAreg(info, 0, dec_reg_das, EBP_REG);
}

/* ARGSUSED */
static int dec_eSI_decode(ADDR4 eip, IAinstInfoPtr info)	/* 4E */
{
    return iAreg(info, 0, dec_reg_das, ESI_REG);
}

/* ARGSUSED */
static int dec_eDI_decode(ADDR4 eip, IAinstInfoPtr info)	/* 4F */
{
    return iAreg(info, 0, dec_reg_das, EDI_REG);
}

/* ARGSUSED */
static int push_eAX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 50 */
{
    return iApushReg(info, EAX_REG);
}

/* ARGSUSED */
static int push_eCX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 51 */
{
    return iApushReg(info, ECX_REG);
}

/* ARGSUSED */
static int push_eDX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 52 */
{
    return iApushReg(info, EDX_REG);
}

/* ARGSUSED */
static int push_eBX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 53 */
{
    return iApushReg(info, EBX_REG);
}

/* ARGSUSED */
static int push_eSP_decode(ADDR4 eip, IAinstInfoPtr info)	/* 54 */
{
    return iApushReg(info, ESP_REG);
}

/* ARGSUSED */
static int push_eBP_decode(ADDR4 eip, IAinstInfoPtr info)	/* 55 */
{
    return iApushReg(info, EBP_REG);
}

/* ARGSUSED */
static int push_eSI_decode(ADDR4 eip, IAinstInfoPtr info)	/* 56 */
{
    return iApushReg(info, ESI_REG);
}

/* ARGSUSED */
static int push_eDI_decode(ADDR4 eip, IAinstInfoPtr info)	/* 57 */
{
    return iApushReg(info, EDI_REG);
}

/* ARGSUSED */
static int pop_eAX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 58 */
{
    return iApopReg(info, EAX_REG);
}

/* ARGSUSED */
static int pop_eCX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 59 */
{
    return iApopReg(info, ECX_REG);
}

/* ARGSUSED */
static int pop_eDX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 5A */
{
    return iApopReg(info, EDX_REG);
}

/* ARGSUSED */
static int pop_eBX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 5B */
{
    return iApopReg(info, EBX_REG);
}

/* ARGSUSED */
static int pop_eSP_decode(ADDR4 eip, IAinstInfoPtr info)	/* 5C */
{
    return iApopReg(info, ESP_REG);
}

/* ARGSUSED */
static int pop_eBP_decode(ADDR4 eip, IAinstInfoPtr info)	/* 5D */
{
    return iApopReg(info, EBP_REG);
}

/* ARGSUSED */
static int pop_eSI_decode(ADDR4 eip, IAinstInfoPtr info)	/* 5E */
{
    return iApopReg(info, ESI_REG);
}

/* ARGSUSED */
static int pop_eDI_decode(ADDR4 eip, IAinstInfoPtr info)	/* 5F */
{
    return iApopReg(info, EDI_REG);
}

/* ARGSUSED */
static int pusha_decode(ADDR4 eip, IAinstInfoPtr info)		/* 60 */
{
    if (StackAddrSize(info->mode) == IA16) {
	info->destRdFn = 0;
	info->destWrFn = 0;
    } else {	/* IA32 */
	info->destRdFn = 0;
	info->destWrFn = 0;
    }
    info->execFn = 0;
    info->dasFn = pusha_das;
    return 1;
}

/* ARGSUSED */
static int popa_decode(ADDR4 eip, IAinstInfoPtr info)		/* 61 */
{
    if (StackAddrSize(info->mode) == IA16)
	info->srcRdFn = 0;
    else	/* IA32 */
	info->srcRdFn = 0;
    info->execFn = 0;
    info->dasFn = popa_das;
    return 1;
}

static int bound_GvMa_decode(ADDR4 eip, IAinstInfoPtr info)	/* 62 */
{
    int len = 1;

    len += modrm_decode(eip+1, info, 0, bound_GvMa_das,
			BoundSrcRegDest);

    /* only memory address forms allowed */
    if (info->modrm.mod == 3)
	info->execFn = 0;

    return len;
}

static int arpl_EwGw_decode(ADDR4 eip, IAinstInfoPtr info)	/* 63 */
{
    return 1 + modrm_decode(eip+1, info, 0, arpl_EwGw_das,
			    RegSrcEaDest);
}

static int seg_FS_decode(ADDR4 eip, IAinstInfoPtr info)		/* 64 */
{
    info->segment = FS_ID;
    return ia_decode2(eip+1, info) + 1;
}

static int seg_GS_decode(ADDR4 eip, IAinstInfoPtr info)		/* 65 */
{
    info->segment = GS_ID;
    return ia_decode2(eip+1, info) + 1;
}

static int operand_size_decode(ADDR4 eip, IAinstInfoPtr info)	/* 66 */
{
    overrideSize(info->opSize);
    return ia_decode2(eip+1, info) + 1;
}

static int address_size_decode(ADDR4 eip, IAinstInfoPtr info)	/* 67 */
{
    overrideSize(info->addrSize);
    return ia_decode2(eip+1, info) + 1;
}

static int push_Iv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 68 */
{
    info->srcRdFn = 0;
    if (StackAddrSize(info->mode) == IA16) {
	info->destRdFn = 0;
	info->destWrFn = 0;
    } else {	/* IA32 */
	info->destRdFn = 0;
	info->destWrFn = 0;
    }
    info->execFn = 0;
    info->dasFn = push_imm_das;
    return 1 + iAimm(eip+1, &info->imm32, info->opSize);
}

static int imul_GvEvIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 69 */
{
    int len = 1;

    len += modrm_decode(eip+1, info, 0, imul_GvEvIv_das, EaSrcRegDest);
    info->destRdFn = info->srcRdFn;
    info->srcRdFn = 0;
    len += iAimm(eip+len, &info->imm32, info->opSize);
    return len;
}

static int push_Ib_decode(ADDR4 eip, IAinstInfoPtr info)	/* 6A */
{
    info->srcRdFn = 0;
    if (StackAddrSize(info->mode) == IA16) {
	info->destRdFn = 0;
	info->destWrFn = 0;
    } else {	/* IA32 */
	info->destRdFn = 0;
	info->destWrFn = 0;
    }
    info->execFn = 0;
    info->dasFn = push_imm_das;
    return 1 + iAimm(eip+1, &info->imm32, 1);
}

static int imul_GvEvIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 6B */
{
    int len = 1;

    len += modrm_decode(eip+1, info, 0, imul_GvEvIb_das, EaSrcRegDest);
    info->destRdFn = info->srcRdFn;
    info->srcRdFn = 0;
    len += iAimm(eip+len, &info->imm32, 1);
    return len;
}

/* ARGSUSED */
static int insb_YbDX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 6C */
{
    info->opSize = 1;
    if (info->addrSize == IA16)
	info->destRdFn = 0;
    else
	info->destRdFn = 0;
    info->destWrFn = 0;
    info->segment = ES_ID;
    info->modrm.mod = 0;
    switch (info->lock_rep.rep) {
    case REP:
	info->execFn = 0;
	break;
    case REPNE:
	info->execFn = 0;
	break;
    default:
	info->execFn = 0;
	break;
    }
    info->dasFn = ins_YxDX_das;
    return 1;
}

/* ARGSUSED */
static int insw_YvDX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 6D */
{
    if (info->addrSize == IA16)
	info->destRdFn = 0;
    else
	info->destRdFn = 0;
    info->destWrFn = 0;
    info->segment = ES_ID;
    info->modrm.mod = 0;
    switch (info->lock_rep.rep) {
    case REP:
	info->execFn = 0;
	break;
    case REPNE:
	info->execFn = 0;
	break;
    default:
	info->execFn = 0;
	break;
    }
    info->dasFn = ins_YxDX_das;
    return 1;
}

/* ARGSUSED */
static int outsb_DXXb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 6E */
{
    info->opSize = 1;
    if (info->addrSize == IA16)
	info->srcRdFn = 0;
    else
	info->srcRdFn = 0;
    if (!info->segment)		/* no segment override */
	info->segment = DS_ID;
    info->modrm.mod = 0;
    info->destRdFn = 0;
    info->base = DX_REG;
    info->destWrFn = 0;
    switch (info->lock_rep.rep) {
    case REP:
	info->execFn = 0;
	break;
    case REPNE:
	info->execFn = 0;
	break;
    default:
	info->execFn = 0;
	break;
    }
    info->dasFn = outs_DXXx_das;
    return 1;
}

/* ARGSUSED */
static int outsw_DXXv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 6F */
{
    if (info->addrSize == IA16)
	info->srcRdFn = 0;
    else
	info->srcRdFn = 0;
    if (!info->segment)		/* no segment override */
	info->segment = DS_ID;
    info->modrm.mod = 0;
    info->destRdFn = 0;
    info->base = DX_REG;
    info->destWrFn = 0;
    switch (info->lock_rep.rep) {
    case REP:
	info->execFn = 0;
	break;
    case REPNE:
	info->execFn = 0;
	break;
    default:
	info->execFn = 0;
	break;
    }
    info->dasFn = outs_DXXx_das;
    return 1;
}

static int jo_Jb_decode(ADDR4 eip, IAinstInfoPtr info)		/* 70 */
{
    info->opSize = 1;
    info->cond_seg = JO_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int jno_Jb_decode(ADDR4 eip, IAinstInfoPtr info)		/* 71 */
{
    info->opSize = 1;
    info->cond_seg = JNO_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int jb_Jb_decode(ADDR4 eip, IAinstInfoPtr info)		/* 72 */
{
    info->opSize = 1;
    info->cond_seg = JB_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int jnb_Jb_decode(ADDR4 eip, IAinstInfoPtr info)		/* 73 */
{
    info->opSize = 1;
    info->cond_seg = JNB_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int jz_Jb_decode(ADDR4 eip, IAinstInfoPtr info)		/* 74 */
{
    info->opSize = 1;
    info->cond_seg = JZ_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int jnz_Jb_decode(ADDR4 eip, IAinstInfoPtr info)		/* 75 */
{
    info->opSize = 1;
    info->cond_seg = JNZ_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int jbe_Jb_decode(ADDR4 eip, IAinstInfoPtr info)		/* 76 */
{
    info->opSize = 1;
    info->cond_seg = JBE_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int jnbe_Jb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 77 */
{
    info->opSize = 1;
    info->cond_seg = JNBE_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int js_Jb_decode(ADDR4 eip, IAinstInfoPtr info)		/* 78 */
{
    info->opSize = 1;
    info->cond_seg = JS_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int jns_Jb_decode(ADDR4 eip, IAinstInfoPtr info)		/* 79 */
{
    info->opSize = 1;
    info->cond_seg = JNS_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int jp_Jb_decode(ADDR4 eip, IAinstInfoPtr info)		/* 7A */
{
    info->opSize = 1;
    info->cond_seg = JP_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int jnp_Jb_decode(ADDR4 eip, IAinstInfoPtr info)		/* 7B */
{
    info->opSize = 1;
    info->cond_seg = JNP_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int jl_Jb_decode(ADDR4 eip, IAinstInfoPtr info)		/* 7C */
{
    info->opSize = 1;
    info->cond_seg = JL_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int jnl_Jb_decode(ADDR4 eip, IAinstInfoPtr info)		/* 7D */
{
    info->opSize = 1;
    info->cond_seg = JNL_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int jle_Jb_decode(ADDR4 eip, IAinstInfoPtr info)		/* 7E */
{
    info->opSize = 1;
    info->cond_seg = JLE_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int jnle_Jb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 7F */
{
    info->opSize = 1;
    info->cond_seg = JNLE_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static GroupInfo group1_info[8] = {     /* reg_op */
    { 0,	    0, NoSrcEaDest },	/* 0 */
    { 0,	    0, NoSrcEaDest },	/* 1 */
    { 0,	    0, NoSrcEaDest },	/* 2 */
    { 0,	    0, NoSrcEaDest },	/* 3 */
    { 0,	    0, NoSrcEaDest },	/* 4 */
    { 0,	    0, NoSrcEaDest },	/* 5 */
    { 0,	    0, NoSrcEaDest },	/* 6 */
    { 0,	    0, NoSrcEaDest }	/* 7 */
};

static int group1_EbIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 80 */
{
    ModRM modrm;
    GroupInfoPtr gp;
    int len = 1;

    if (!memMIAIRd(eip+1, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    gp = &group1_info[modrm.reg_op];
    info->opSize = 1;
    len += modrm_decode(eip+1, info, gp->execFn, group1_das, gp->sdt);
    len += iAimm(eip+len, &info->imm32, 1);
    info->srcRdFn = 0;
    return len;
}

static int group1_EvIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 81 */
{
    ModRM modrm;
    GroupInfoPtr gp;
    int len = 1;

    if (!memMIAIRd(eip+1, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    gp = &group1_info[modrm.reg_op];
    len += modrm_decode(eip+1, info, gp->execFn, group1_das, gp->sdt);
    len += iAimm(eip+len, &info->imm32, info->opSize);
    info->srcRdFn = 0;
    return len;
}

/* XXX - why do the Pentium & PPro opcode tables show 82 as Reserved? */
static int movb_ALIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 82 */
{
    info->execFn = 0;
    info->dasFn = movb_ALIb_das;
    return 1;
}

static int group1_EvIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 83 */
{
    ModRM modrm;
    GroupInfoPtr gp;
    int len = 1;

    if (!memMIAIRd(eip+1, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    gp = &group1_info[modrm.reg_op];

    len += modrm_decode(eip+1, info, gp->execFn, group1_das, gp->sdt);
    len += iAimm(eip+len, &info->imm32, 1);
    info->srcRdFn = 0;
    return len;
}

static int test_EbGb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 84 */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, test_ExGx_das,
			    RegSrcEaDest);
}

static int test_EvGv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 85 */
{
    return 1 + modrm_decode(eip+1, info, 0, test_ExGx_das,
			    RegSrcEaDest);
}

static int xchg_EbGb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 86 */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, xchg_ExGx_das,
			    RegSrcEaDest);
}

static int xchg_EvGv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 87 */
{
    return 1 + modrm_decode(eip+1, info, 0, xchg_ExGx_das,
			    RegSrcEaDest);
}

static int mov_EbGb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 88 */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, mov_ExGx_das, RegSrcEaDest);
}

static int mov_EvGv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 89 */
{
    return 1 + modrm_decode(eip+1, info, 0, mov_ExGx_das, RegSrcEaDest);
}

static int mov_GbEb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 8A */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, mov_GxEx_das, EaSrcRegDest);
}

static int mov_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 8B */
{
    return 1 + modrm_decode(eip+1, info, 0, mov_GxEx_das, EaSrcRegDest);
}

static int mov_EwSw_decode(ADDR4 eip, IAinstInfoPtr info)	/* 8C */
{
    int len = 1;

    /* XXX - PPro allows 4-byte mov if dest is a register */
    info->opSize = 2;
    len += modrm_decode(eip+1, info, 0, mov_EwSw_das, NoSrcEaDest);
    info->srcRdFn = 0;
    return len;
}

static int lea_GvM_decode(ADDR4 eip, IAinstInfoPtr info)	/* 8D */
{
    int len = 1;

    len += modrm_decode(eip+1, info, 0, lea_GvM_das, LeaSrcRegDest);

    /* only memory address forms allowed */
    if (info->modrm.mod == 3)
	info->execFn = 0;

    return len;
}

static int mov_SwEw_decode(ADDR4 eip, IAinstInfoPtr info)	/* 8E */
{
    int len = 1;

    /* XXX - PPro allows 4-byte mov if src is a register */
    info->opSize = 2;
    len += modrm_decode(eip+1, info, 0, mov_SwEw_das, EaSrcNoDest);
    info->destRdFn = 0;
    info->destWrFn = 0;
    if (info->reg == CS_REG || info->reg > GS_REG)
	info->execFn = 0;
    info->cond_seg = info->reg;
    return len;
}

static int pop_Ev_decode(ADDR4 eip, IAinstInfoPtr info)		/* 8F */
{
    int len = 1;

    len += modrm_decode(eip+1, info, 0, pop_Ev_das, NoSrcEaDest);
    if (StackAddrSize(info->mode) == IA16)
	info->srcRdFn = 0;
    else	/* IA32 */
	info->srcRdFn = 0;
    return len;
}

/* ARGSUSED */
static int nop_decode(ADDR4 eip, IAinstInfoPtr info)		/* 90 */
{
    info->execFn = 0;
    info->dasFn = nop_das;
    return 1;
}

/* ARGSUSED */
static int xchg_eAXeCX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 91 */
{
    return iAregReg(info, 0, xchg_reg_reg_das, EAX_REG, ECX_REG);
}

/* ARGSUSED */
static int xchg_eAXeDX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 92 */
{
    return iAregReg(info, 0, xchg_reg_reg_das, EAX_REG, EDX_REG);
}

/* ARGSUSED */
static int xchg_eAXeBX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 93 */
{
    return iAregReg(info, 0, xchg_reg_reg_das, EAX_REG, EBX_REG);
}

/* ARGSUSED */
static int xchg_eAXeSP_decode(ADDR4 eip, IAinstInfoPtr info)	/* 94 */
{
    return iAregReg(info, 0, xchg_reg_reg_das, EAX_REG, ESP_REG);
}

/* ARGSUSED */
static int xchg_eAXeBP_decode(ADDR4 eip, IAinstInfoPtr info)	/* 95 */
{
    return iAregReg(info, 0, xchg_reg_reg_das, EAX_REG, EBP_REG);
}

/* ARGSUSED */
static int xchg_eAXeSI_decode(ADDR4 eip, IAinstInfoPtr info)	/* 96 */
{
    return iAregReg(info, 0, xchg_reg_reg_das, EAX_REG, ESI_REG);
}

static int xchg_eAXeDI_decode(ADDR4 eip, IAinstInfoPtr info)	/* 97 */
{
    return iAregReg(info, 0, xchg_reg_reg_das, EAX_REG, EDI_REG);
}

/* ARGSUSED */
static int cbw_decode(ADDR4 eip, IAinstInfoPtr info)		/* 98 */
{
    if (info->opSize == IA16) {
	info->srcRdFn = 0;
	info->destWrFn = 0;
    } else {
	info->srcRdFn = 0;
	info->destWrFn = 0;
    }
    info->reg = EAX_REG;
    info->execFn = 0;
    info->dasFn = cbw_das;
    return 1;
}

/* ARGSUSED */
static int cwd_decode(ADDR4 eip, IAinstInfoPtr info)		/* 99 */
{
    if (info->opSize == IA16) {
	info->srcRdFn = 0;
	info->destWrFn = 0;
    } else {
	info->srcRdFn = 0;
	info->destWrFn = 0;
    }
    info->reg = EAX_REG;
    info->base = EDX_REG;
    info->execFn = 0;
    info->dasFn = cwd_das;
    return 1;
}

static int call_Ap_decode(ADDR4 eip, IAinstInfoPtr info)	/* 9A */
{
    int len = 1;

    len += iAimm(eip+1, &info->disp32, info->opSize);	/* offset */
    len += iAimm(eip+len, &info->imm32, 2);		/* segment */
    if (StackAddrSize(info->mode) == IA16) {
	info->destRdFn = 0;
	info->destWrFn = 0;
    } else {	/* IA32 */
	info->destRdFn = 0;
	info->destWrFn = 0;
    }
    info->cond_seg = CS_REG;
    info->execFn = 0;
    info->dasFn = call_Ap_das;
    return len;
}

/* ARGSUSED */
static int wait_decode(ADDR4 eip, IAinstInfoPtr info)		/* 9B */
{
    info->execFn = 0;
    info->dasFn = wait_das;
    return 1;
}

/* ARGSUSED */
static int pushf_Fv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 9C */
{
    if (StackAddrSize(info->mode) == IA16) {
	info->destRdFn = 0;
	info->destWrFn = 0;
    } else {	/* IA32 */
	info->destRdFn = 0;
	info->destWrFn = 0;
    }
    info->execFn = 0;
    info->dasFn = pushf_Fv_das;
    return 1;
}

/* ARGSUSED */
static int popf_Fv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 9D */
{
    if (StackAddrSize(info->mode) == IA16)
	info->srcRdFn = 0;
    else	/* IA32 */
	info->srcRdFn = 0;
    info->execFn = 0;
    info->dasFn = popf_Fv_das;
    return 1;
}

/* ARGSUSED */
static int sahf_decode(ADDR4 eip, IAinstInfoPtr info)		/* 9E */
{
    info->execFn = 0;
    info->dasFn = sahf_das;
    return 1;
}

/* ARGSUSED */
static int lahf_decode(ADDR4 eip, IAinstInfoPtr info)		/* 9F */
{
    info->execFn = 0;
    info->dasFn = lahf_das;
    return 1;
}

static int mov_ALOb_decode(ADDR4 eip, IAinstInfoPtr info)	/* A0 */
{
    info->opSize = 1;
    info->base = info->index = NO_REG;
    info->srcRdFn = 0;
    if (!info->segment)		/* no segment override */
	info->segment = DS_ID;
    info->destRdFn = 0;
    info->destWrFn = 0;
    info->reg = AL_REG;
    info->execFn = 0;
    info->dasFn = mov_reg_Ox_das;
    return 1 + iAimm(eip+1, &info->disp32, info->addrSize);
}

static int mov_eAXOv_decode(ADDR4 eip, IAinstInfoPtr info)	/* A1 */
{
    info->base = info->index = NO_REG;
    info->srcRdFn = 0;
    if (!info->segment)		/* no segment override */
	info->segment = DS_ID;
    if (info->opSize == IA16) {
	info->destRdFn = 0;
	info->destWrFn = 0;
    } else {
	info->destRdFn = 0;
	info->destWrFn = 0;
    }
    info->reg = EAX_REG;
    info->execFn = 0;
    info->dasFn = mov_reg_Ox_das;
    return 1 + iAimm(eip+1, &info->disp32, info->addrSize);
}

static int mov_ObAL_decode(ADDR4 eip, IAinstInfoPtr info)	/* A2 */
{
    info->opSize = 1;
    info->srcRdFn = 0;
    info->reg = AL_REG;
    info->base = info->index = NO_REG;
    info->destRdFn = 0;
    info->destWrFn = 0;
    if (!info->segment)		/* no segment override */
	info->segment = DS_ID;
    info->execFn = 0;
    info->dasFn = mov_Ox_reg_das;
    return 1 + iAimm(eip+1, &info->disp32, info->addrSize);
}

static int mov_OveAX_decode(ADDR4 eip, IAinstInfoPtr info)	/* A3 */
{
    if (info->opSize == IA16)
	info->srcRdFn = 0;
    else
	info->srcRdFn = 0;
    info->reg = EAX_REG;
    info->base = info->index = NO_REG;
    info->destRdFn = 0;
    info->destWrFn = 0;
    if (!info->segment)		/* no segment override */
	info->segment = DS_ID;
    info->execFn = 0;
    info->dasFn = mov_Ox_reg_das;
    return 1 + iAimm(eip+1, &info->disp32, info->addrSize);
}

static int movsb_YbXb_decode(ADDR4 eip, IAinstInfoPtr info)	/* A4 */
{
    info->opSize = 1;
    if (info->addrSize == IA16) {
	info->srcRdFn = 0;
	info->destRdFn = 0;
    } else {
	info->srcRdFn = 0;
	info->destRdFn = 0;
    }
    info->destWrFn = 0;
    if (!info->segment)		/* no segment override */
	info->segment = DS_ID;
    info->modrm.mod = 0;
    switch (info->lock_rep.rep) {
    case REP:
	info->execFn = 0;
	break;
    case REPNE:
	info->execFn = 0;
	break;
    default:
	info->execFn = 0;
	break;
    }
    info->dasFn = movs_YxXx_das;
    return 1;
}

static int movsw_YvXv_decode(ADDR4 eip, IAinstInfoPtr info)	/* A5 */
{
    if (info->addrSize == IA16) {
	info->srcRdFn = 0;
	info->destRdFn = 0;
    } else {
	info->srcRdFn = 0;
	info->destRdFn = 0;
    }
    info->destWrFn = 0;
    if (!info->segment)		/* no segment override */
	info->segment = DS_ID;
    info->modrm.mod = 0;
    switch (info->lock_rep.rep) {
    case REP:
	info->execFn = 0;
	break;
    case REPNE:
	info->execFn = 0;
	break;
    default:
	info->execFn = 0;
	break;
    }
    info->dasFn = movs_YxXx_das;
    return 1;
}

static int cmpsb_XbYb_decode(ADDR4 eip, IAinstInfoPtr info)	/* A6 */
{
    info->opSize = 1;
    if (info->addrSize == IA16) {
	info->srcRdFn = 0;
	info->destRdFn = 0;
    } else {
	info->srcRdFn = 0;
	info->destRdFn = 0;
    }
    if (!info->segment)		/* no segment override */
	info->segment = DS_ID;
    info->modrm.mod = 0;
    switch (info->lock_rep.rep) {
    case REP:
	info->lock_rep.rep = REPE;
	info->execFn = 0;
	break;
    case REPNE:
	info->execFn = 0;
	break;
    default:
	info->execFn = 0;
	break;
    }
    info->dasFn = cmps_XxYx_das;
    return 1;
}

static int cmpsw_XvYv_decode(ADDR4 eip, IAinstInfoPtr info)	/* A7 */
{
    if (info->addrSize == IA16) {
	info->srcRdFn = 0;
	info->destRdFn = 0;
    } else {
	info->srcRdFn = 0;
	info->destRdFn = 0;
    }
    if (!info->segment)		/* no segment override */
	info->segment = DS_ID;
    info->modrm.mod = 0;
    switch (info->lock_rep.rep) {
    case REP:
	info->lock_rep.rep = REPE;
	info->execFn = 0;
	break;
    case REPNE:
	info->execFn = 0;
	break;
    default:
	info->execFn = 0;
	break;
    }
    info->dasFn = cmps_XxYx_das;
    return 1;
}

static int test_ALIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* A8 */
{
    info->opSize = 1;
    return iAregImm(eip, info, 0, test_reg_imm_das, AL_REG);
}

static int test_eAXIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* A9 */
{
    return iAregImm(eip, info, 0, test_reg_imm_das, EAX_REG);
}

static int stosb_YbAL_decode(ADDR4 eip, IAinstInfoPtr info)	/* AA */
{
    info->opSize = 1;
    info->srcRdFn = 0;
    info->reg = AL_REG;
    if (info->addrSize == IA16)
	info->destRdFn = 0;
    else
	info->destRdFn = 0;
    info->destWrFn = 0;
    info->segment = ES_ID;
    info->modrm.mod = 0;
    switch (info->lock_rep.rep) {
    case REP:
	info->execFn = 0;
	break;
    case REPNE:
	info->execFn = 0;
	break;
    default:
	info->execFn = 0;
	break;
    }
    info->dasFn = stos_Yx_reg_das;
    return 1;
}

static int stosw_YveAX_decode(ADDR4 eip, IAinstInfoPtr info)	/* AB */
{
    if (info->opSize == IA16)
	info->srcRdFn = 0;
    else
	info->srcRdFn = 0;
    info->reg = EAX_REG;
    if (info->addrSize == IA16)
	info->destRdFn = 0;
    else
	info->destRdFn = 0;
    info->destWrFn = 0;
    info->segment = ES_ID;
    info->modrm.mod = 0;
    switch (info->lock_rep.rep) {
    case REP:
	info->execFn = 0;
	break;
    case REPNE:
	info->execFn = 0;
	break;
    default:
	info->execFn = 0;
	break;
    }
    info->dasFn = stos_Yx_reg_das;
    return 1;
}

static int lodsb_ALXb_decode(ADDR4 eip, IAinstInfoPtr info)	/* AC */
{
    info->opSize = 1;
    if (info->addrSize == IA16)
	info->srcRdFn = 0;
    else
	info->srcRdFn = 0;
    if (!info->segment)		/* no segment override */
	info->segment = DS_ID;
    info->destRdFn = 0;
    info->destWrFn = 0;
    info->reg = AL_REG;
    info->modrm.mod = 0;
    switch (info->lock_rep.rep) {
    case REP:
	info->execFn = 0;
	break;
    case REPNE:
	info->execFn = 0;
	break;
    default:
	info->execFn = 0;
	break;
    }
    info->dasFn = lods_reg_Xx_das;
    return 1;
}

static int lodsw_eAXXv_decode(ADDR4 eip, IAinstInfoPtr info)	/* AD */
{
    if (info->addrSize == IA16)
	info->srcRdFn = 0;
    else
	info->srcRdFn = 0;
    if (!info->segment)		/* no segment override */
	info->segment = DS_ID;
    if (info->opSize == IA16) {
	info->destRdFn = 0;
	info->destWrFn = 0;
    } else {
	info->destRdFn = 0;
	info->destWrFn = 0;
    }
    info->reg = EAX_REG;
    info->modrm.mod = 0;
    switch (info->lock_rep.rep) {
    case REP:
	info->execFn = 0;
	break;
    case REPNE:
	info->execFn = 0;
	break;
    default:
	info->execFn = 0;
	break;
    }
    info->dasFn = lods_reg_Xx_das;
    return 1;
}

static int scasb_ALYb_decode(ADDR4 eip, IAinstInfoPtr info)	/* AE */
{
    info->opSize = 1;
    if (info->addrSize == IA16)
	info->srcRdFn = 0;
    else
	info->srcRdFn = 0;
    info->segment = ES_ID;
    info->destRdFn = 0;
    info->reg = AL_REG;
    info->modrm.mod = 0;
    switch (info->lock_rep.rep) {
    case REP:
	info->lock_rep.rep = REPE;
	info->execFn = 0;
	break;
    case REPNE:
	info->execFn = 0;
	break;
    default:
	info->execFn = 0;
	break;
    }
    info->dasFn = scas_reg_Yx_das;
    return 1;
}

static int scasw_eAXYv_decode(ADDR4 eip, IAinstInfoPtr info)	/* AF */
{
    if (info->addrSize == IA16)
	info->srcRdFn = 0;
    else
	info->srcRdFn = 0;
    info->segment = ES_ID;
    if (info->opSize == IA16)
	info->destRdFn = 0;
    else
	info->destRdFn = 0;
    info->reg = EAX_REG;
    info->modrm.mod = 0;
    switch (info->lock_rep.rep) {
    case REP:
	info->lock_rep.rep = REPE;
	info->execFn = 0;
	break;
    case REPNE:
	info->execFn = 0;
	break;
    default:
	info->execFn = 0;
	break;
    }
    info->dasFn = scas_reg_Yx_das;
    return 1;
}

static int mov_ALIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* B0 */
{
    info->opSize = 1;
    return iAregImm(eip, info, 0, mov_reg_imm_das, AL_REG);
}

static int mov_CLIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* B1 */
{
    info->opSize = 1;
    return iAregImm(eip, info, 0, mov_reg_imm_das, CL_REG);
}

static int mov_DLIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* B2 */
{
    info->opSize = 1;
    return iAregImm(eip, info, 0, mov_reg_imm_das, DL_REG);
}

static int mov_BLIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* B3 */
{
    info->opSize = 1;
    return iAregImm(eip, info, 0, mov_reg_imm_das, BL_REG);
}

static int mov_AHIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* B4 */
{
    info->opSize = 1;
    return iAregImm(eip, info, 0, mov_reg_imm_das, AH_REG);
}

static int mov_CHIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* B5 */
{
    info->opSize = 1;
    return iAregImm(eip, info, 0, mov_reg_imm_das, CH_REG);
}

static int mov_DHIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* B6 */
{
    info->opSize = 1;
    return iAregImm(eip, info, 0, mov_reg_imm_das, DH_REG);
}

static int mov_BHIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* B7 */
{
    info->opSize = 1;
    return iAregImm(eip, info, 0, mov_reg_imm_das, BH_REG);
}

static int mov_eAXIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* B8 */
{
    return iAregImm(eip, info, 0, mov_reg_imm_das, EAX_REG);
}

static int mov_eCXIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* B9 */
{
    return iAregImm(eip, info, 0, mov_reg_imm_das, ECX_REG);
}

static int mov_eDXIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* BA */
{
    return iAregImm(eip, info, 0, mov_reg_imm_das, EDX_REG);
}

static int mov_eBXIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* BB */
{
    return iAregImm(eip, info, 0, mov_reg_imm_das, EBX_REG);
}

static int mov_eSPIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* BC */
{
    return iAregImm(eip, info, 0, mov_reg_imm_das, ESP_REG);
}

static int mov_eBPIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* BD */
{
    return iAregImm(eip, info, 0, mov_reg_imm_das, EBP_REG);
}

static int mov_eSIIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* BE */
{
    return iAregImm(eip, info, 0, mov_reg_imm_das, ESI_REG);
}

static int mov_eDIIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* BF */
{
    return iAregImm(eip, info, 0, mov_reg_imm_das, EDI_REG);
}

static GroupInfo group2_info[8] = {     /* reg_op */
    { 0,	    0, NoSrcEaDest },	/* 0 */
    { 0,	    0, NoSrcEaDest },	/* 1 */
    { 0,	    0, NoSrcEaDest },	/* 2 */
    { 0,	    0, NoSrcEaDest },	/* 3 */
    { 0,	    0, NoSrcEaDest },	/* 4 */
    { 0,	    0, NoSrcEaDest },	/* 5 */
    { 0, 0, NoSrcEaDest },	/* 6 - opSize & sdt arbitrary */
    { 0,	    0, NoSrcEaDest }	/* 7 */
};

static int group2_EbIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* C0 */
{
    ModRM modrm;
    GroupInfoPtr gp;
    int len = 1;

    if (!memMIAIRd(eip+1, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    gp = &group2_info[modrm.reg_op];
    info->opSize = 1;
    len += modrm_decode(eip+1, info, gp->execFn, group2_ExIb_das, gp->sdt);
    len += iAimm(eip+len, &info->imm32, 1);
    info->srcRdFn = 0;
    return len;
}

static int group2_EvIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* C1 */
{
    ModRM modrm;
    GroupInfoPtr gp;
    int len = 1;

    if (!memMIAIRd(eip+1, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    gp = &group2_info[modrm.reg_op];
    len += modrm_decode(eip+1, info, gp->execFn, group2_ExIb_das, gp->sdt);
    len += iAimm(eip+len, &info->imm32, 1);
    info->srcRdFn = 0;
    return len;
}

static int ret_near_Iw_decode(ADDR4 eip, IAinstInfoPtr info)	/* C2 */
{
    if (StackAddrSize(info->mode) == IA16)
	info->srcRdFn = 0;
    else	/* IA32 */
	info->srcRdFn = 0;
    info->execFn = 0;
    info->dasFn = ret_near_Iw_das;
    return 1 + iAimm(eip+1, &info->imm32, 2);
}

/* ARGSUSED */
static int ret_near_decode(ADDR4 eip, IAinstInfoPtr info)	/* C3 */
{
    if (StackAddrSize(info->mode) == IA16)
	info->srcRdFn = 0;
    else	/* IA32 */
	info->srcRdFn = 0;
    info->execFn = 0;
    info->dasFn = ret_near_das;
    info->imm32 = 0;
    return 1;
}

static int les_GvMp_decode(ADDR4 eip, IAinstInfoPtr info)	/* C4 */
{
    int len = 1;

    len += modrm_decode(eip+1, info, 0, load_far_ptr_das,
			LfpSrcRegDest);
    info->cond_seg = ES_REG;
    /* only memory address forms allowed */
    if (info->modrm.mod == 3)
	info->execFn = 0;

    return len;
}

static int lds_GvMp_decode(ADDR4 eip, IAinstInfoPtr info)	/* C5 */
{
    int len = 1;

    len += modrm_decode(eip+1, info, 0, load_far_ptr_das,
			LfpSrcRegDest);
    info->cond_seg = DS_REG;
    /* only memory address forms allowed */
    if (info->modrm.mod == 3)
	info->execFn = 0;

    return len;
}

static int mov_EbIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* C6 */
{
    int len = 1;

    info->opSize = 1;
    len += modrm_decode(eip+1, info, 0, mov_ExIx_das, NoSrcEaDest);
    len += iAimm(eip+len, &info->imm32, 1);
    info->srcRdFn = 0;
    return len;
}

static int mov_EvIv_decode(ADDR4 eip, IAinstInfoPtr info)	/* C7 */
{
    int len = 1;

    len += modrm_decode(eip+1, info, 0, mov_ExIx_das, NoSrcEaDest);
    len += iAimm(eip+len, &info->imm32, info->opSize);
    info->srcRdFn = 0;
    return len;
}

static int enter_IwIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* C8 */
{
    int len = 1;

    if (StackAddrSize(info->mode) == IA16) {
	info->destRdFn = 0;
	info->destWrFn = 0;
    } else {	/* IA32 */
	info->destRdFn = 0;
	info->destWrFn = 0;
    }
    len += iAimm(eip+1, &info->imm32, 2);	/* imm16 */
    len += iAimm(eip+len, &info->disp32, 1);	/* imm8 */
    info->execFn = 0;
    info->dasFn = enter_IwIb_das;
    return len;
}

/* ARGSUSED */
static int leave_decode(ADDR4 eip, IAinstInfoPtr info)		/* C9 */
{
    if (StackAddrSize(info->mode) == IA16)
	info->srcRdFn = 0;
    else	/* IA32 */
	info->srcRdFn = 0;
    info->execFn = 0;
    info->dasFn = leave_das;
    return 1;
}

static int ret_far_Iw_decode(ADDR4 eip, IAinstInfoPtr info)	/* CA */
{
    if (StackAddrSize(info->mode) == IA16)
	info->srcRdFn = 0;
    else	/* IA32 */
	info->srcRdFn = 0;
    info->cond_seg = CS_REG;
    info->execFn = 0;
    info->dasFn = ret_far_Iw_das;
    return 1 + iAimm(eip+1, &info->imm32, 2);
}

/* ARGSUSED */
static int ret_far_decode(ADDR4 eip, IAinstInfoPtr info)	/* CB */
{
    if (StackAddrSize(info->mode) == IA16)
	info->srcRdFn = 0;
    else	/* IA32 */
	info->srcRdFn = 0;
    info->cond_seg = CS_REG;
    info->execFn = 0;
    info->dasFn = ret_far_das;
    info->imm32 = 0;
    return 1;
}

/* ARGSUSED */
static int int3_decode(ADDR4 eip, IAinstInfoPtr info)		/* CC */
{
    info->execFn = 0;
    info->dasFn = int_Ib_das;
    info->imm32 = 3;
    return 1;
}

static int int_Ib_decode(ADDR4 eip, IAinstInfoPtr info)		/* CD */
{
    info->execFn = 0;
    info->dasFn = int_Ib_das;
    return 1 + iAimm(eip+1, &info->imm32, 1);
}

/* ARGSUSED */
static int into_decode(ADDR4 eip, IAinstInfoPtr info)		/* CE */
{
    info->execFn = 0;
    info->dasFn = into_das;
    return 1;
}

/* ARGSUSED */
static int iret_decode(ADDR4 eip, IAinstInfoPtr info)		/* CF */
{
    if (StackAddrSize(info->mode) == IA16)
	info->srcRdFn = 0;
    else	/* IA32 */
	info->srcRdFn = 0;
    info->cond_seg = CS_REG;
    info->execFn = 0;
    info->dasFn = iret_das;
    return 1;
}

static int group2_Eb1_decode(ADDR4 eip, IAinstInfoPtr info)	/* D0 */
{
    ModRM modrm;
    GroupInfoPtr gp;
    int len = 1;

    if (!memMIAIRd(eip+1, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    gp = &group2_info[modrm.reg_op];
    info->opSize = 1;
    len += modrm_decode(eip+1, info, gp->execFn, group2_ExIb_das, gp->sdt);
    info->imm32 = 1;
    info->srcRdFn = 0;
    return len;
}

static int group2_Ev1_decode(ADDR4 eip, IAinstInfoPtr info)	/* D1 */
{
    ModRM modrm;
    GroupInfoPtr gp;
    int len = 1;

    if (!memMIAIRd(eip+1, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    gp = &group2_info[modrm.reg_op];
    len += modrm_decode(eip+1, info, gp->execFn, group2_ExIb_das, gp->sdt);
    info->imm32 = 1;
    info->srcRdFn = 0;
    return len;
}

static int group2_EbCL_decode(ADDR4 eip, IAinstInfoPtr info)	/* D2 */
{
    ModRM modrm;
    GroupInfoPtr gp;
    int len = 1;

    if (!memMIAIRd(eip+1, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    gp = &group2_info[modrm.reg_op];
    info->opSize = 1;
    len += modrm_decode(eip+1, info, gp->execFn, group2_ExCL_das, gp->sdt);
    info->reg = CL_REG;
    info->srcRdFn = 0;
    return len;
}

static int group2_EvCL_decode(ADDR4 eip, IAinstInfoPtr info)	/* D3 */
{
    ModRM modrm;
    GroupInfoPtr gp;
    int len = 1;

    if (!memMIAIRd(eip+1, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    gp = &group2_info[modrm.reg_op];
    len += modrm_decode(eip+1, info, gp->execFn, group2_ExCL_das, gp->sdt);
    info->reg = CL_REG;
    info->srcRdFn = 0;
    return len;
}

static int aam_Ib_decode(ADDR4 eip, IAinstInfoPtr info)		/* D4 */
{
    info->execFn = 0;
    info->dasFn = aam_Ib_das;
    return 1 + iAimm(eip+1, &info->imm32, 1);
}

static int aad_Ib_decode(ADDR4 eip, IAinstInfoPtr info)		/* D5 */
{
    info->execFn = 0;
    info->dasFn = aad_Ib_das;
    return 1 + iAimm(eip+1, &info->imm32, 1);
}

/* salc is an undocumented opcode */
static int salc_decode(ADDR4 eip, IAinstInfoPtr info)		/* D6 */
{
    info->execFn = 0;
    info->dasFn = salc_das;
    return 1;
}

/* ARGSUSED */
static int xlat_decode(ADDR4 eip, IAinstInfoPtr info)		/* D7 */
{
    if (!info->segment)		/* no segment override */
	info->segment = DS_ID;
    info->srcRdFn = 0;
    info->execFn = 0;
    info->dasFn = xlat_das;
    info->modrm.mod = 0;
    return 1;
}

static int esc_D8_decode(ADDR4 eip, IAinstInfoPtr info)		/* D8 */
{
    info->execFn = 0;
    info->dasFn = esc_D8_das;
    return 1;
}

static int esc_D9_decode(ADDR4 eip, IAinstInfoPtr info)		/* D9 */
{
    info->execFn = 0;
    info->dasFn = esc_D9_das;
    return 1;
}

static int esc_DA_decode(ADDR4 eip, IAinstInfoPtr info)		/* DA */
{
    info->execFn = 0;
    info->dasFn = esc_DA_das;
    return 1;
}

static int esc_DB_decode(ADDR4 eip, IAinstInfoPtr info)		/* DB */
{
    info->execFn = 0;
    info->dasFn = esc_DB_das;
    return 1;
}

static int esc_DC_decode(ADDR4 eip, IAinstInfoPtr info)		/* DC */
{
    info->execFn = 0;
    info->dasFn = esc_DC_das;
    return 1;
}

static int esc_DD_decode(ADDR4 eip, IAinstInfoPtr info)		/* DD */
{
    info->execFn = 0;
    info->dasFn = esc_DD_das;
    return 1;
}

static int esc_DE_decode(ADDR4 eip, IAinstInfoPtr info)		/* DE */
{
    info->execFn = 0;
    info->dasFn = esc_DE_das;
    return 1;
}

static int esc_DF_decode(ADDR4 eip, IAinstInfoPtr info)		/* DF */
{
    info->execFn = 0;
    info->dasFn = esc_DF_das;
    return 1;
}

static int loopn_Jb_decode(ADDR4 eip, IAinstInfoPtr info)	/* E0 */
{
    info->cond_seg = LOOPNZ_COND;
    info->execFn = 0;
    info->dasFn = loop_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int loope_Jb_decode(ADDR4 eip, IAinstInfoPtr info)	/* E1 */
{
    info->cond_seg = LOOPZ_COND;
    info->execFn = 0;
    info->dasFn = loop_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int loop_Jb_decode(ADDR4 eip, IAinstInfoPtr info)	/* E2 */
{
    info->cond_seg = LOOP_COND;
    info->execFn = 0;
    info->dasFn = loop_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int jcxz_Jb_decode(ADDR4 eip, IAinstInfoPtr info)	/* E3 */
{
    info->cond_seg = JCXZ_COND;
    info->execFn = 0;
    info->dasFn = jcxz_Jb_das;
    return 1 + iAimm(eip+1, &info->disp32, 1);
}

static int in_ALIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* E4 */
{
    int len = 1;

    len += iAimm(eip+1, &info->imm32, 1);
    info->imm32 &= 0xff;	/* zero-extend immediate */
    info->opSize = 1;
    info->srcRdFn = 0;
    info->destRdFn = 0;
    info->destWrFn = 0;
    info->reg = AL_REG;
    info->execFn = 0;
    info->dasFn = in_reg_imm_das;
    return len;
}

static int in_eAXIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* E5 */
{
    int len = 1;

    len += iAimm(eip+1, &info->imm32, 1);
    info->imm32 &= 0xff;	/* zero-extend immediate */
    info->srcRdFn = 0;
    info->destRdFn = 0;
    if (info->opSize == IA16)
	info->destWrFn = 0;
    else
	info->destWrFn = 0;
    info->reg = EAX_REG;
    info->execFn = 0;
    info->dasFn = in_reg_imm_das;
    return len;
}

static int out_IbAL_decode(ADDR4 eip, IAinstInfoPtr info)	/* E6 */
{
    int len = 1;

    len += iAimm(eip+1, &info->imm32, 1);
    info->imm32 &= 0xff;	/* zero-extend immediate */
    info->opSize = 1;
    info->srcRdFn = 0;
    info->reg = AL_REG;
    info->destRdFn = 0;
    info->destWrFn = 0;
    info->execFn = 0;
    info->dasFn = out_imm_reg_das;
    return len;
}

static int out_IbeAX_decode(ADDR4 eip, IAinstInfoPtr info)	/* E7 */
{
    int len = 1;

    len += iAimm(eip+1, &info->imm32, 1);
    info->imm32 &= 0xff;	/* zero-extend immediate */
    if (info->opSize == IA16)
	info->srcRdFn = 0;
    else
	info->srcRdFn = 0;
    info->reg = EAX_REG;
    info->destRdFn = 0;
    info->destWrFn = 0;
    info->execFn = 0;
    info->dasFn = out_imm_reg_das;
    return len;
}

static int call_Jv_decode(ADDR4 eip, IAinstInfoPtr info)	/* E8 */
{
    int len = 1;

    len += iAimm(eip+1, &info->disp32, info->opSize);
    if (StackAddrSize(info->mode) == IA16) {
	info->destRdFn = 0;
	info->destWrFn = 0;
    } else {	/* IA32 */
	info->destRdFn = 0;
	info->destWrFn = 0;
    }
    info->execFn = 0;
    info->dasFn = call_Jv_das;
    return len;
}

static int jmp_Jv_decode(ADDR4 eip, IAinstInfoPtr info)		/* E9 */
{
    int len = 1;

    len += iAimm(eip+1, &info->disp32, info->opSize);
    info->execFn = 0;
    info->dasFn = jmp_Jv_das;
    return len;
}

static int jmp_Ap_decode(ADDR4 eip, IAinstInfoPtr info)		/* EA */
{
    int len = 1;

    len += iAimm(eip+1, &info->disp32, info->opSize);	/* offset */
    len += iAimm(eip+len, &info->imm32, 2);		/* segment */
    info->cond_seg = CS_REG;
    info->execFn = 0;
    info->dasFn = jmp_Ap_das;
    return len;
}

static int jmp_Jb_decode(ADDR4 eip, IAinstInfoPtr info)		/* EB */
{
    int len = 1;

    len += iAimm(eip+1, &info->disp32, 1);
    info->execFn = 0;
    info->dasFn = jmp_Jb_das;
    return len;
}

/* ARGSUSED */
static int in_ALDX_decode(ADDR4 eip, IAinstInfoPtr info)	/* EC */
{
    info->opSize = 1;
    info->srcRdFn = 0;
    info->base = DX_REG;
    info->destRdFn = 0;
    info->destWrFn = 0;
    info->reg = AL_REG;
    info->execFn = 0;
    info->dasFn = in_reg_DX_das;
    return 1;
}

/* ARGSUSED */
static int in_eAXDX_decode(ADDR4 eip, IAinstInfoPtr info)	/* ED */
{
    info->srcRdFn = 0;
    info->base = DX_REG;
    info->destRdFn = 0;
    if (info->opSize == IA16)
	info->destWrFn = 0;
    else
	info->destWrFn = 0;
    info->reg = EAX_REG;
    info->execFn = 0;
    info->dasFn = in_reg_DX_das;
    return 1;
}

/* ARGSUSED */
static int out_DXAL_decode(ADDR4 eip, IAinstInfoPtr info)	/* EE */
{
    info->opSize = 1;
    info->srcRdFn = 0;
    info->reg = AL_REG;
    info->destRdFn = 0;
    info->base = DX_REG;
    info->destWrFn = 0;
    info->execFn = 0;
    info->dasFn = out_DX_reg_das;
    return 1;
}

/* ARGSUSED */
static int out_DXeAX_decode(ADDR4 eip, IAinstInfoPtr info)	/* EF */
{
    if (info->opSize == IA16)
	info->srcRdFn = 0;
    else
	info->srcRdFn = 0;
    info->reg = EAX_REG;
    info->destRdFn = 0;
    info->base = DX_REG;
    info->destWrFn = 0;
    info->execFn = 0;
    info->dasFn = out_DX_reg_das;
    return 1;
}

/* ARGSUSED */
static int lock_decode(ADDR4 eip, IAinstInfoPtr info)		/* F0 */
{
    info->execFn = 0;
    info->dasFn = lock_das;
    return 1;
}

/* ARGSUSED */
static int reserved_decode(ADDR4 eip, IAinstInfoPtr info)	/* F1 */
{
    info->execFn = 0;
    info->dasFn = reserved_das;
    return 1;
}

static int repne_decode(ADDR4 eip, IAinstInfoPtr info)		/* F2 */
{
    info->lock_rep.rep = REPNE;
    return ia_decode2(eip+1, info) + 1;
}

static int rep_decode(ADDR4 eip, IAinstInfoPtr info)		/* F3 */
{
    info->lock_rep.rep = REP;
    return ia_decode2(eip+1, info) + 1;
}

/* ARGSUSED */
static int hlt_decode(ADDR4 eip, IAinstInfoPtr info)		/* F4 */
{
    info->execFn = 0;
    info->dasFn = hlt_das;
    return 1;
}

/* ARGSUSED */
static int cmc_decode(ADDR4 eip, IAinstInfoPtr info)		/* F5 */
{
    info->execFn = 0;
    info->dasFn = cmc_das;
    return 1;
}

static GroupInfo group3_info[8] = {	/* reg_op */
    { 0,	    0, NoSrcEaDest },	/* 0 */
    { 0, 0, NoSrcEaDest },	/* 1 - opSize & sdt arbitrary */
    { 0,	    0, NoSrcEaDest },	/* 2 */
    { 0,	    0, NoSrcEaDest },	/* 3 */
    { 0,	    0, EaSrcNoDest },	/* 4 */
    { 0,	    0, EaSrcNoDest },	/* 5 */
    { 0,	    0, EaSrcNoDest },	/* 6 */
    { 0,	    0, EaSrcNoDest }	/* 7 */
};

static int group3_Eb_decode(ADDR4 eip, IAinstInfoPtr info)	/* F6 */
{
    ModRM modrm;
    GroupInfoPtr gp;
    int len = 1;

    if (!memMIAIRd(eip+1, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    gp = &group3_info[modrm.reg_op];
    info->opSize = 1;
    len += modrm_decode(eip+1, info, gp->execFn, group3_das, gp->sdt);
    switch (modrm.reg_op) {
    case 0:	/* test */
	len += iAimm(eip+len, &info->imm32, 1);
	info->srcRdFn = 0;
	break;
    case 4:	/* mul */
    case 5:	/* imul */
	info->destRdFn = 0;
	info->destWrFn = 0;
	info->reg = AX_REG;
	break;
    case 6:	/* div */
    case 7:	/* idiv */
	info->destRdFn = 0;
	info->destWrFn = 0;
	info->reg = AX_REG;
	break;
    }
    return len;
}

static int group3_Ev_decode(ADDR4 eip, IAinstInfoPtr info)	/* F7 */
{
    ModRM modrm;
    GroupInfoPtr gp;
    int len = 1;

    if (!memMIAIRd(eip+1, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    gp = &group3_info[modrm.reg_op];
    len += modrm_decode(eip+1, info, gp->execFn, group3_das, gp->sdt);
    switch (modrm.reg_op) {
    case 0:	/* test */
	len += iAimm(eip+len, &info->imm32, info->opSize);
	info->srcRdFn = 0;
	break;
    case 4:	/* mul */
    case 5:	/* imul */
    case 6:	/* div */
    case 7:	/* idiv */
	if (info->opSize == IA16) {
	    info->destRdFn = 0;
	    info->destWrFn = 0;
	} else {
	    info->destRdFn = 0;
	    info->destWrFn = 0;
	}
	info->reg = AX_REG;
	break;
    }
    return len;
}

/* ARGSUSED */
static int clc_decode(ADDR4 eip, IAinstInfoPtr info)		/* F8 */
{
    info->execFn = 0;
    info->dasFn = clc_das;
    return 1;
}

/* ARGSUSED */
static int stc_decode(ADDR4 eip, IAinstInfoPtr info)		/* F9 */
{
    info->execFn = 0;
    info->dasFn = stc_das;
    return 1;
}

/* ARGSUSED */
static int cli_decode(ADDR4 eip, IAinstInfoPtr info)		/* FA */
{
    info->execFn = 0;
    info->dasFn = cli_das;
    return 1;
}

/* ARGSUSED */
static int sti_decode(ADDR4 eip, IAinstInfoPtr info)		/* FB */
{
    info->execFn = 0;
    info->dasFn = sti_das;
    return 1;
}

/* ARGSUSED */
static int cld_decode(ADDR4 eip, IAinstInfoPtr info)		/* FC */
{
    info->execFn = 0;
    info->dasFn = cld_das;
    return 1;
}

/* ARGSUSED */
static int std_decode(ADDR4 eip, IAinstInfoPtr info)		/* FD */
{
    info->execFn = 0;
    info->dasFn = std_das;
    return 1;
}

static GroupInfo group4_info[8] = {	/* reg_op */
    { 0,	    1, NoSrcEaDest },	/* 0 */
    { 0,	    1, NoSrcEaDest },	/* 1 */
    { 0, 0, EaSrcNoDest },	/* 2 - opSize & sdt arbitrary */
    { 0, 0, EaSrcNoDest },	/* 3 - opSize & sdt arbitrary */
    { 0, 0, EaSrcNoDest },	/* 4 - opSize & sdt arbitrary */
    { 0, 0, EaSrcNoDest },	/* 5 - opSize & sdt arbitrary */
    { 0, 0, EaSrcNoDest },	/* 6 - opSize & sdt arbitrary */
    { 0, 0, EaSrcNoDest }	/* 7 - opSize & sdt arbitrary */
};

static int group4_decode(ADDR4 eip, IAinstInfoPtr info)		/* FE */
{
    ModRM modrm;
    GroupInfoPtr gp;
    int len = 1;

    if (!memMIAIRd(eip+1, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    gp = &group4_info[modrm.reg_op];
    if (gp->opSize)
	info->opSize = gp->opSize;
    len += modrm_decode(eip+1, info, gp->execFn, group4_das, gp->sdt);
    return len;
}

static GroupInfo group5_info[8] = {	     /* reg_op */
    { 0,		 0, NoSrcEaDest  },  /* 0 */
    { 0,		 0, NoSrcEaDest  },  /* 1 */
    { 0, 0, EaSrcNoDest  },  /* 2 */
    { 0,	 0, LfpSrcNoDest },  /* 3 */
    { 0,	 0, EaSrcNoDest  },  /* 4 */
    { 0,	 0, LfpSrcNoDest },  /* 5 */
    { 0,		 0, EaSrcNoDest  },  /* 6 */
    { 0,	 0, EaSrcNoDest  }   /* 7 - opSize & sdt arbitrary */
};

static int group5_decode(ADDR4 eip, IAinstInfoPtr info)		/* FF */
{
    ModRM modrm;
    GroupInfoPtr gp;
    int len = 1;

    if (!memMIAIRd(eip+1, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    gp = &group5_info[modrm.reg_op];
    len += modrm_decode(eip+1, info, gp->execFn, group5_das, gp->sdt);

    /* call_near, call_far, push */
    if (modrm.reg_op == 2 || modrm.reg_op == 3 || modrm.reg_op == 6) {
	if (StackAddrSize(info->mode) == IA16) {
	    info->destRdFn = 0;
	    info->destWrFn = 0;
	} else {	/* IA32 */
	    info->destRdFn = 0;
	    info->destWrFn = 0;
	}
    }
    /* call_far, jmp_far */
    if (modrm.reg_op == 3 || modrm.reg_op == 5)
	info->cond_seg = CS_REG;

    return len;
}

/* One-Byte Opcode Table */

static const PIADF one_byte_opcode[256] = {
    add_EbGb_decode,		/* 00 */
    add_EvGv_decode,		/* 01 */
    add_GbEb_decode,		/* 02 */
    add_GvEv_decode,		/* 03 */
    add_ALIb_decode,		/* 04 */
    add_eAXIv_decode,		/* 05 */
    push_ES_decode,		/* 06 */
    pop_ES_decode,		/* 07 */
    or_EbGb_decode,		/* 08 */
    or_EvGv_decode,		/* 09 */
    or_GbEb_decode,		/* 0A */
    or_GvEv_decode,		/* 0B */
    or_ALIb_decode,		/* 0C */
    or_eAXIv_decode,		/* 0D */
    push_CS_decode,		/* 0E */
    two_byte_escape,		/* 0F */
    adc_EbGb_decode,		/* 10 */
    adc_EvGv_decode,		/* 11 */
    adc_GbEb_decode,		/* 12 */
    adc_GvEv_decode,		/* 13 */
    adc_ALIb_decode,		/* 14 */
    adc_eAXIv_decode,		/* 15 */
    push_SS_decode,		/* 16 */
    pop_SS_decode,		/* 17 */
    sbb_EbGb_decode,		/* 18 */
    sbb_EvGv_decode,		/* 19 */
    sbb_GbEb_decode,		/* 1A */
    sbb_GvEv_decode,		/* 1B */
    sbb_ALIb_decode,		/* 1C */
    sbb_eAXIv_decode,		/* 1D */
    push_DS_decode,		/* 1E */
    pop_DS_decode,		/* 1F */
    and_EbGb_decode,		/* 20 */
    and_EvGv_decode,		/* 21 */
    and_GbEb_decode,		/* 22 */
    and_GvEv_decode,		/* 23 */
    and_ALIb_decode,		/* 24 */
    and_eAXIv_decode,		/* 25 */
    seg_ES_decode,		/* 26 */
    daa_decode,			/* 27 */
    sub_EbGb_decode,		/* 28 */
    sub_EvGv_decode,		/* 29 */
    sub_GbEb_decode,		/* 2A */
    sub_GvEv_decode,		/* 2B */
    sub_ALIb_decode,		/* 2C */
    sub_eAXIv_decode,		/* 2D */
    seg_CS_decode,		/* 2E */
    das_decode,			/* 2F */
    xor_EbGb_decode,		/* 30 */
    xor_EvGv_decode,		/* 31 */
    xor_GbEb_decode,		/* 32 */
    xor_GvEv_decode,		/* 33 */
    xor_ALIb_decode,		/* 34 */
    xor_eAXIv_decode,		/* 35 */
    seg_SS_decode,		/* 36 */
    aaa_decode,			/* 37 */
    cmp_EbGb_decode,		/* 38 */
    cmp_EvGv_decode,		/* 39 */
    cmp_GbEb_decode,		/* 3A */
    cmp_GvEv_decode,		/* 3B */
    cmp_ALIb_decode,		/* 3C */
    cmp_eAXIv_decode,		/* 3D */
    seg_DS_decode,		/* 3E */
    aas_decode,			/* 3F */
    inc_eAX_decode,		/* 40 */
    inc_eCX_decode,		/* 41 */
    inc_eDX_decode,		/* 42 */
    inc_eBX_decode,		/* 43 */
    inc_eSP_decode,		/* 44 */
    inc_eBP_decode,		/* 45 */
    inc_eSI_decode,		/* 46 */
    inc_eDI_decode,		/* 47 */
    dec_eAX_decode,		/* 48 */
    dec_eCX_decode,		/* 49 */
    dec_eDX_decode,		/* 4A */
    dec_eBX_decode,		/* 4B */
    dec_eSP_decode,		/* 4C */
    dec_eBP_decode,		/* 4D */
    dec_eSI_decode,		/* 4E */
    dec_eDI_decode,		/* 4F */
    push_eAX_decode,		/* 50 */
    push_eCX_decode,		/* 51 */
    push_eDX_decode,		/* 52 */
    push_eBX_decode,		/* 53 */
    push_eSP_decode,		/* 54 */
    push_eBP_decode,		/* 55 */
    push_eSI_decode,		/* 56 */
    push_eDI_decode,		/* 57 */
    pop_eAX_decode,		/* 58 */
    pop_eCX_decode,		/* 59 */
    pop_eDX_decode,		/* 5A */
    pop_eBX_decode,		/* 5B */
    pop_eSP_decode,		/* 5C */
    pop_eBP_decode,		/* 5D */
    pop_eSI_decode,		/* 5E */
    pop_eDI_decode,		/* 5F */
    pusha_decode,		/* 60 */
    popa_decode,		/* 61 */
    bound_GvMa_decode,		/* 62 */
    arpl_EwGw_decode,		/* 63 */
    seg_FS_decode,		/* 64 */
    seg_GS_decode,		/* 65 */
    operand_size_decode,	/* 66 */
    address_size_decode,	/* 67 */
    push_Iv_decode,		/* 68 */
    imul_GvEvIv_decode,		/* 69 */
    push_Ib_decode,		/* 6A */
    imul_GvEvIb_decode,		/* 6B */
    insb_YbDX_decode,		/* 6C */
    insw_YvDX_decode,		/* 6D */
    outsb_DXXb_decode,		/* 6E */
    outsw_DXXv_decode,		/* 6F */
    jo_Jb_decode,		/* 70 */
    jno_Jb_decode,		/* 71 */
    jb_Jb_decode,		/* 72 */
    jnb_Jb_decode,		/* 73 */
    jz_Jb_decode,		/* 74 */
    jnz_Jb_decode,		/* 75 */
    jbe_Jb_decode,		/* 76 */
    jnbe_Jb_decode,		/* 77 */
    js_Jb_decode,		/* 78 */
    jns_Jb_decode,		/* 79 */
    jp_Jb_decode,		/* 7A */
    jnp_Jb_decode,		/* 7B */
    jl_Jb_decode,		/* 7C */
    jnl_Jb_decode,		/* 7D */
    jle_Jb_decode,		/* 7E */
    jnle_Jb_decode,		/* 7F */
    group1_EbIb_decode,		/* 80 */
    group1_EvIv_decode,		/* 81 */
/* XXX - why do the Pentium & PPro opcode tables show 82 as Reserved? */
    movb_ALIb_decode,		/* 82 */
    group1_EvIb_decode,		/* 83 */
    test_EbGb_decode,		/* 84 */
    test_EvGv_decode,		/* 85 */
    xchg_EbGb_decode,		/* 86 */
    xchg_EvGv_decode,		/* 87 */
    mov_EbGb_decode,		/* 88 */
    mov_EvGv_decode,		/* 89 */
    mov_GbEb_decode,		/* 8A */
    mov_GvEv_decode,		/* 8B */
    mov_EwSw_decode,		/* 8C */
    lea_GvM_decode,		/* 8D */
    mov_SwEw_decode,		/* 8E */
    pop_Ev_decode,		/* 8F */
    nop_decode,			/* 90 */
    xchg_eAXeCX_decode,       	/* 91 */
    xchg_eAXeDX_decode,       	/* 92 */
    xchg_eAXeBX_decode,       	/* 93 */
    xchg_eAXeSP_decode,       	/* 94 */
    xchg_eAXeBP_decode,       	/* 95 */
    xchg_eAXeSI_decode,       	/* 96 */
    xchg_eAXeDI_decode,       	/* 97 */
    cbw_decode,			/* 98 */
    cwd_decode,			/* 99 */
    call_Ap_decode,		/* 9A */
    wait_decode,	       	/* 9B */
    pushf_Fv_decode,		/* 9C */
    popf_Fv_decode,		/* 9D */
    sahf_decode,	       	/* 9E */
    lahf_decode,	       	/* 9F */
    mov_ALOb_decode,		/* A0 */
    mov_eAXOv_decode,		/* A1 */
    mov_ObAL_decode,		/* A2 */
    mov_OveAX_decode,		/* A3 */
    movsb_YbXb_decode,		/* A4 */
    movsw_YvXv_decode,		/* A5 */
    cmpsb_XbYb_decode,		/* A6 */
    cmpsw_XvYv_decode,		/* A7 */
    test_ALIb_decode,		/* A8 */
    test_eAXIv_decode,		/* A9 */
    stosb_YbAL_decode,		/* AA */
    stosw_YveAX_decode,		/* AB */
    lodsb_ALXb_decode,		/* AC */
    lodsw_eAXXv_decode,		/* AD */
    scasb_ALYb_decode,		/* AE */
    scasw_eAXYv_decode,		/* AF */
    mov_ALIb_decode,		/* B0 */
    mov_CLIb_decode,		/* B1 */
    mov_DLIb_decode,		/* B2 */
    mov_BLIb_decode,		/* B3 */
    mov_AHIb_decode,		/* B4 */
    mov_CHIb_decode,		/* B5 */
    mov_DHIb_decode,		/* B6 */
    mov_BHIb_decode,		/* B7 */
    mov_eAXIv_decode,		/* B8 */
    mov_eCXIv_decode,		/* B9 */
    mov_eDXIv_decode,		/* BA */
    mov_eBXIv_decode,		/* BB */
    mov_eSPIv_decode,		/* BC */
    mov_eBPIv_decode,		/* BD */
    mov_eSIIv_decode,		/* BE */
    mov_eDIIv_decode,		/* BF */
    group2_EbIb_decode,		/* C0 */
    group2_EvIb_decode,		/* C1 */
    ret_near_Iw_decode,		/* C2 */
    ret_near_decode,		/* C3 */
    les_GvMp_decode,		/* C4 */
    lds_GvMp_decode,		/* C5 */
    mov_EbIb_decode,		/* C6 */
    mov_EvIv_decode,		/* C7 */
    enter_IwIb_decode,		/* C8 */
    leave_decode,	       	/* C9 */
    ret_far_Iw_decode,		/* CA */
    ret_far_decode,		/* CB */
    int3_decode,	       	/* CC */
    int_Ib_decode,		/* CD */
    into_decode,	       	/* CE */
    iret_decode,	       	/* CF */
    group2_Eb1_decode,		/* D0 */
    group2_Ev1_decode,		/* D1 */
    group2_EbCL_decode,		/* D2 */
    group2_EvCL_decode,		/* D3 */
    aam_Ib_decode,		/* D4 */
    aad_Ib_decode,		/* D5 */
    salc_decode,		/* D6 - undocumented opcode */
    xlat_decode,	       	/* D7 */
    esc_D8_decode,		/* D8 */
    esc_D9_decode,		/* D9 */
    esc_DA_decode,		/* DA */
    esc_DB_decode,		/* DB */
    esc_DC_decode,		/* DC */
    esc_DD_decode,		/* DD */
    esc_DE_decode,		/* DE */
    esc_DF_decode,		/* DF */
    loopn_Jb_decode,		/* E0 */
    loope_Jb_decode,		/* E1 */
    loop_Jb_decode,		/* E2 */
    jcxz_Jb_decode,		/* E3 */
    in_ALIb_decode,		/* E4 */
    in_eAXIb_decode,		/* E5 */
    out_IbAL_decode,		/* E6 */
    out_IbeAX_decode,		/* E7 */
    call_Jv_decode,		/* E8 */
    jmp_Jv_decode,		/* E9 */
    jmp_Ap_decode,		/* EA */
    jmp_Jb_decode,		/* EB */
    in_ALDX_decode,		/* EC */
    in_eAXDX_decode,		/* ED */
    out_DXAL_decode,		/* EE */
    out_DXeAX_decode,		/* EF */
    lock_decode,	       	/* F0 */
    reserved_decode,		/* F1 */
    repne_decode,	       	/* F2 */
    rep_decode,			/* F3 */
    hlt_decode,			/* F4 */
    cmc_decode,			/* F5 */
    group3_Eb_decode,		/* F6 */
    group3_Ev_decode,		/* F7 */
    clc_decode,			/* F8 */
    stc_decode,			/* F9 */
    cli_decode,			/* FA */
    sti_decode,			/* FB */
    cld_decode,			/* FC */
    std_decode,			/* FD */
    group4_decode,		/* FE */
    group5_decode		/* FF */
};


static GroupInfo group6_info[8] = {	/* reg_op */
    { 0,	    2, NoSrcEaDest },	/* 0 */
    { 0,	    2, NoSrcEaDest },	/* 1 */
    { 0,	    2, EaSrcNoDest },	/* 2 */
    { 0,	    2, EaSrcNoDest },	/* 3 */
    { 0,	    2, EaSrcNoDest },	/* 4 */
    { 0,	    2, EaSrcNoDest },	/* 5 */
    { 0, 0, EaSrcNoDest },	/* 6 */
    { 0, 0, EaSrcNoDest }	/* 7 - opSize & sdt arbitrary */
};

/* Two-Byte Opcode Decode Functions */

static int group6_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,00 */
{
    ModRM modrm;
    GroupInfoPtr gp;

    if (!memMIAIRd(eip+1, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    gp = &group6_info[modrm.reg_op];
    if (gp->opSize)
	info->opSize = gp->opSize;
    return 1 + modrm_decode(eip+1, info, gp->execFn, group6_das, gp->sdt);
}

static GroupInfo group7_info[8] = {	/* reg_op */
    { 0,	    6, NoSrcEaDest },	/* 0 */
    { 0,	    6, NoSrcEaDest },	/* 1 */
    { 0,	    6, EaSrcNoDest },	/* 2 */
    { 0,	    6, EaSrcNoDest },	/* 3 */
    { 0,	    2, NoSrcEaDest },	/* 4 */
    { 0, 0, EaSrcNoDest },	/* 5 - opSize & sdt arbitrary */
    { 0,	    2, EaSrcNoDest },	/* 6 */
    { 0,   0, EaSrcNoDest }	/* 7 */
};

static int group7_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,01 */
{
    ModRM modrm;
    GroupInfoPtr gp;
    int len = 1;

    if (!memMIAIRd(eip+1, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    gp = &group7_info[modrm.reg_op];
    if (gp->opSize)
	info->opSize = gp->opSize;
    len += modrm_decode(eip+1, info, gp->execFn, group7_das, gp->sdt);

    /* all but smsw & lmsw: only memory address forms allowed */
    if ((modrm.reg_op < 4 || modrm.reg_op == 7) && modrm.mod == 3)
	info->execFn = 0;

    return len;
}

static int lar_GvEw_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,02 */
{
    return 1 + modrm_decode(eip+1, info, 0, lar_GvEw_das, EaSrcRegDest);
}

static int lsl_GvEw_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,03 */
{
    return 1 + modrm_decode(eip+1, info, 0, lsl_GvEw_das, EaSrcRegDest);
}

/* reserved */							/* 0F,04-05 */

/* ARGSUSED */
static int clts_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,06 */
{
    info->execFn = 0;
    info->dasFn = clts_das;
    return 1;
}

/* reserved */						       	/* 0F,07 */

/* ARGSUSED */
static int invd_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,08 */
{
    info->execFn = 0;
    info->dasFn = invd_das;
    return 1;
}

/* ARGSUSED */
static int wbinvd_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,09 */
{
    info->execFn = 0;
    info->dasFn = wbinvd_das;
    return 1;
}

/* reserved */						       	/* 0F,0A */

/* ARGSUSED */
static int ud2_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,0B */
{
    info->execFn = 0;
    info->dasFn = ud2_das;
    return 1;
}

/* reserved */							/* 0F,0C-0F */

/* reserved */							/* 0F,10-1F */

static int mov_RdCd_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,20 */
{
    int len = 1;

    info->opSize = 4;
    len += modrm_decode(eip+1, info, 0, mov_RdCd_das, NoSrcEaDest);
    /* XXX - add CR srcRdFn */

    /* only CR0, CR2, CR3, CR4 and register address forms allowed */
    if (info->reg > 4 || info->reg == 1 || info->modrm.mod != 3)
	info->execFn = 0;

    return len;
}

static int mov_RdDd_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,21 */
{
    int len = 1;

    info->opSize = 4;
    len += modrm_decode(eip+1, info, 0, mov_RdDd_das, NoSrcEaDest);
    /* XXX - add DR srcRdFn */

    if (info->modrm.mod != 3)
	info->execFn = 0;

    return len;
}

static int mov_CdRd_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,22 */
{
    int len = 1;

    info->opSize = 4;
    len += modrm_decode(eip+1, info, 0, mov_CdRd_das, EaSrcNoDest);
    /* XXX - add CR destRdFn & destWrFn */

    /* only CR0, CR2, CR3, CR4 and register address forms allowed */
    if (info->reg > 4 || info->reg == 1 || info->modrm.mod != 3)
	info->execFn = 0;

    return len;
}

static int mov_DdRd_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,23 */
{
    int len = 1;

    info->opSize = 4;
    len += modrm_decode(eip+1, info, 0, mov_DdRd_das, EaSrcNoDest);
    /* XXX - add DR destRdFn & destWrFn */

    if (info->modrm.mod != 3)
	info->execFn = 0;

    return len;
}

/* move to/from test registers are now reserved instructions */
static int mov_RdTd_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,24 */
{
    info->opSize = 4;
    return 1 + modrm_decode(eip+1, info, 0, mov_RdTd_das,
			    NoSrcEaDest);
}

/* reserved */						       	/* 0F,25 */

static int mov_TdRd_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,26 */
{
    info->opSize = 4;
    return 1 + modrm_decode(eip+1, info, 0, mov_TdRd_das,
			    EaSrcNoDest);
}

/* reserved */							/* 0F,27-2F */

static int wrmsr_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,30 */
{
    info->execFn = 0;
    info->dasFn = wrmsr_das;
    return 1;
}

static int rdtsc_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,31 */
{
    info->execFn = 0;
    info->dasFn = rdtsc_das;
    return 1;
}

static int rdmsr_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,32 */
{
    info->execFn = 0;
    info->dasFn = rdmsr_das;
    return 1;
}

static int rdpmc_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,33 */
{
    info->execFn = 0;
    info->dasFn = rdpmc_das;
    return 1;
}

/* reserved */							/* 0F,34-3F */

static int cmovo_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,40 */
{
    info->cond_seg = JO_COND;
    return 1 + modrm_decode(eip+1, info, 0, cmovcc_das, EaSrcRegDest);
}

static int cmovno_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,41 */
{
    info->cond_seg = JNO_COND;
    return 1 + modrm_decode(eip+1, info, 0, cmovcc_das, EaSrcRegDest);
}

static int cmovb_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,42 */
{
    info->cond_seg = JB_COND;
    return 1 + modrm_decode(eip+1, info, 0, cmovcc_das, EaSrcRegDest);
}

static int cmovnb_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,43 */
{
    info->cond_seg = JNB_COND;
    return 1 + modrm_decode(eip+1, info, 0, cmovcc_das, EaSrcRegDest);
}

static int cmovz_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,44 */
{
    info->cond_seg = JZ_COND;
    return 1 + modrm_decode(eip+1, info, 0, cmovcc_das, EaSrcRegDest);
}

static int cmovnz_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,45 */
{
    info->cond_seg = JNZ_COND;
    return 1 + modrm_decode(eip+1, info, 0, cmovcc_das, EaSrcRegDest);
}

static int cmovbe_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,46 */
{
    info->cond_seg = JBE_COND;
    return 1 + modrm_decode(eip+1, info, 0, cmovcc_das, EaSrcRegDest);
}

static int cmovnbe_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,47 */
{
    info->cond_seg = JNBE_COND;
    return 1 + modrm_decode(eip+1, info, 0, cmovcc_das, EaSrcRegDest);
}

static int cmovs_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,48 */
{
    info->cond_seg = JS_COND;
    return 1 + modrm_decode(eip+1, info, 0, cmovcc_das, EaSrcRegDest);
}

static int cmovns_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,49 */
{
    info->cond_seg = JNS_COND;
    return 1 + modrm_decode(eip+1, info, 0, cmovcc_das, EaSrcRegDest);
}

static int cmovp_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,4A */
{
    info->cond_seg = JP_COND;
    return 1 + modrm_decode(eip+1, info, 0, cmovcc_das, EaSrcRegDest);
}

static int cmovnp_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,4B */
{
    info->cond_seg = JNP_COND;
    return 1 + modrm_decode(eip+1, info, 0, cmovcc_das, EaSrcRegDest);
}

static int cmovl_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,4C */
{
    info->cond_seg = JL_COND;
    return 1 + modrm_decode(eip+1, info, 0, cmovcc_das, EaSrcRegDest);
}

static int cmovnl_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,4D */
{
    info->cond_seg = JNL_COND;
    return 1 + modrm_decode(eip+1, info, 0, cmovcc_das, EaSrcRegDest);
}

static int cmovle_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,4E */
{
    info->cond_seg = JLE_COND;
    return 1 + modrm_decode(eip+1, info, 0, cmovcc_das, EaSrcRegDest);
}

static int cmovnle_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,4F */
{
    info->cond_seg = JNLE_COND;
    return 1 + modrm_decode(eip+1, info, 0, cmovcc_das, EaSrcRegDest);
}

/* reserved */							/* 0F,50-7F */

static int jo_Jv_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,80 */
{
    info->cond_seg = JO_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, info->opSize);
}

static int jno_Jv_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,81 */
{
    info->cond_seg = JNO_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, info->opSize);
}

static int jb_Jv_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,82 */
{
    info->cond_seg = JB_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, info->opSize);
}

static int jnb_Jv_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,83 */
{
    info->cond_seg = JNB_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, info->opSize);
}

static int jz_Jv_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,84 */
{
    info->cond_seg = JZ_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, info->opSize);
}

static int jnz_Jv_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,85 */
{
    info->cond_seg = JNZ_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, info->opSize);
}

static int jbe_Jv_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,86 */
{
    info->cond_seg = JBE_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, info->opSize);
}

static int jnbe_Jv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,87 */
{
    info->cond_seg = JNBE_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, info->opSize);
}

static int js_Jv_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,88 */
{
    info->cond_seg = JS_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, info->opSize);
}

static int jns_Jv_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,89 */
{
    info->cond_seg = JNS_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, info->opSize);
}

static int jp_Jv_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,8A */
{
    info->cond_seg = JP_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, info->opSize);
}

static int jnp_Jv_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,8B */
{
    info->cond_seg = JNP_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, info->opSize);
}

static int jl_Jv_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,8C */
{
    info->cond_seg = JL_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, info->opSize);
}

static int jnl_Jv_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,8D */
{
    info->cond_seg = JNL_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, info->opSize);
}

static int jle_Jv_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,8E */
{
    info->cond_seg = JLE_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, info->opSize);
}

static int jnle_Jv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,8F */
{
    info->cond_seg = JNLE_COND;
    info->execFn = 0;
    info->dasFn = jcc_das;
    return 1 + iAimm(eip+1, &info->disp32, info->opSize);
}

static int seto_Eb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,90 */
{
    info->opSize = 1;
    info->cond_seg = JO_COND;
    return 1 + modrm_decode(eip+1, info, 0, setcc_das, NoSrcEaDest);
}

static int setno_Eb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,91 */
{
    info->opSize = 1;
    info->cond_seg = JNO_COND;
    return 1 + modrm_decode(eip+1, info, 0, setcc_das, NoSrcEaDest);
}

static int setb_Eb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,92 */
{
    info->opSize = 1;
    info->cond_seg = JB_COND;
    return 1 + modrm_decode(eip+1, info, 0, setcc_das, NoSrcEaDest);
}

static int setnb_Eb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,93 */
{
    info->opSize = 1;
    info->cond_seg = JNB_COND;
    return 1 + modrm_decode(eip+1, info, 0, setcc_das, NoSrcEaDest);
}

static int setz_Eb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,94 */
{
    info->opSize = 1;
    info->cond_seg = JZ_COND;
    return 1 + modrm_decode(eip+1, info, 0, setcc_das, NoSrcEaDest);
}

static int setnz_Eb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,95 */
{
    info->opSize = 1;
    info->cond_seg = JNZ_COND;
    return 1 + modrm_decode(eip+1, info, 0, setcc_das, NoSrcEaDest);
}

static int setbe_Eb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,96 */
{
    info->opSize = 1;
    info->cond_seg = JBE_COND;
    return 1 + modrm_decode(eip+1, info, 0, setcc_das, NoSrcEaDest);
}

static int setnbe_Eb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,97 */
{
    info->opSize = 1;
    info->cond_seg = JNBE_COND;
    return 1 + modrm_decode(eip+1, info, 0, setcc_das, NoSrcEaDest);
}

static int sets_Eb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,98 */
{
    info->opSize = 1;
    info->cond_seg = JS_COND;
    return 1 + modrm_decode(eip+1, info, 0, setcc_das, NoSrcEaDest);
}

static int setns_Eb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,99 */
{
    info->opSize = 1;
    info->cond_seg = JNS_COND;
    return 1 + modrm_decode(eip+1, info, 0, setcc_das, NoSrcEaDest);
}

static int setp_Eb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,9A */
{
    info->opSize = 1;
    info->cond_seg = JP_COND;
    return 1 + modrm_decode(eip+1, info, 0, setcc_das, NoSrcEaDest);
}

static int setnp_Eb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,9B */
{
    info->opSize = 1;
    info->cond_seg = JNP_COND;
    return 1 + modrm_decode(eip+1, info, 0, setcc_das, NoSrcEaDest);
}

static int setl_Eb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,9C */
{
    info->opSize = 1;
    info->cond_seg = JL_COND;
    return 1 + modrm_decode(eip+1, info, 0, setcc_das, NoSrcEaDest);
}

static int setnl_Eb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,9D */
{
    info->opSize = 1;
    info->cond_seg = JNL_COND;
    return 1 + modrm_decode(eip+1, info, 0, setcc_das, NoSrcEaDest);
}

static int setle_Eb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,9E */
{
    info->opSize = 1;
    info->cond_seg = JLE_COND;
    return 1 + modrm_decode(eip+1, info, 0, setcc_das, NoSrcEaDest);
}

static int setnle_Eb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,9F */
{
    info->opSize = 1;
    info->cond_seg = JNLE_COND;
    return 1 + modrm_decode(eip+1, info, 0, setcc_das, NoSrcEaDest);
}

/* ARGSUSED */
static int push_FS_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,A0 */
{
    return iApushSeg(info, FS_REG);
}

/* ARGSUSED */
static int pop_FS_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,A1 */
{
    return iApopSeg(info, FS_REG);
}

/* ARGSUSED */
static int cpuid_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,A2 */
{
    info->execFn = 0;
    info->dasFn = cpuid_das;
    return 1;
}

static int bt_EvGv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,A3 */
{
    return 1 + modrm_decode(eip+1, info, 0, bt_EvGv_das, RegSrcEaDest);
}

static int shld_EvGvIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,A4 */
{
    int len = 1;

    len += modrm_decode(eip+1, info, 0, shld_EvGvIb_das,
			RegSrcEaDest);
    len += iAimm(eip+len, &info->imm32, 1);
    return len;
}

static int shld_EvGvCL_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,A5 */
{
    return 1 + modrm_decode(eip+1, info, 0, shld_EvGvCL_das,
			    RegSrcEaDest);
}

/* XXX - why do the Pentium & PPro opcode tables show A6 & A7 as Reserved? */
static int cmpxchg_XBTS_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,A6 */
{
    info->execFn = 0;
    info->dasFn = cmpxchg_XBTS_das;
    return 1;
}

static int cmpxchg_IBTS_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,A7 */
{
    info->execFn = 0;
    info->dasFn = cmpxchg_IBTS_das;
    return 1;
}

/* ARGSUSED */
static int push_GS_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,A8 */
{
    return iApushSeg(info, GS_REG);
}

/* ARGSUSED */
static int pop_GS_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,A9 */
{
    return iApopSeg(info, GS_REG);
}

/* ARGSUSED */
static int rsm_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,AA */
{
    info->execFn = 0;
    info->dasFn = rsm_das;
    return 1;
}

static int bts_EvGv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,AB */
{
    return 1 + modrm_decode(eip+1, info, 0, bts_EvGv_das, RegSrcEaDest);
}

static int shrd_EvGvIb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,AC */
{
    int len = 1;

    len += modrm_decode(eip+1, info, 0, shrd_EvGvIb_das,
			RegSrcEaDest);
    len += iAimm(eip+len, &info->imm32, 1);
    return len;
}

static int shrd_EvGvCL_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,AD */
{
    return 1 + modrm_decode(eip+1, info, 0, shrd_EvGvCL_das,
			    RegSrcEaDest);
}

/* reserved */							/* 0F,AE */

static int imul_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,AF */
{
    return 1 + modrm_decode(eip+1, info, 0, imul_GvEv_das,
			    EaSrcRegDest);
}

static int cmpxchg_EbGb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,B0 */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, cmpxchg_ExGx_das,
			    RegSrcEaDest);
}

static int cmpxchg_EvGv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,B1 */
{
    return 1 + modrm_decode(eip+1, info, 0, cmpxchg_ExGx_das,
			    RegSrcEaDest);
}

static int lss_Mp_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,B2 */
{
    int len = 1;

    len += modrm_decode(eip+1, info, 0, load_far_ptr_das,
			LfpSrcRegDest);
    info->cond_seg = SS_REG;
    /* only memory address forms allowed */
    if (info->modrm.mod == 3)
	info->execFn = 0;

    return len;
}

static int btr_EvGv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,B3 */
{
    return 1 + modrm_decode(eip+1, info, 0, btr_EvGv_das, RegSrcEaDest);
}

static int lfs_Mp_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,B4 */
{
    int len = 1;

    len += modrm_decode(eip+1, info, 0, load_far_ptr_das,
			LfpSrcRegDest);
    info->cond_seg = FS_REG;
    /* only memory address forms allowed */
    if (info->modrm.mod == 3)
	info->execFn = 0;

    return len;
}

static int lgs_Mp_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,B5 */
{
    int len = 1;

    len += modrm_decode(eip+1, info, 0, load_far_ptr_das,
			LfpSrcRegDest);
    info->cond_seg = GS_REG;
    /* only memory address forms allowed */
    if (info->modrm.mod == 3)
	info->execFn = 0;

    return len;
}

static int movzx_GvEb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,B6 */
{
    int len = 1;
    BYTE opSize = info->opSize;			/* save */

    /* force opSize to 1 to get correct srcRdFn */
    info->opSize = 1;
    len += modrm_decode(eip+1, info, 0, movzx_GvEb_das,
			EaSrcNoDest);
    if ((info->opSize = opSize) == IA16)	/* restore */
	info->destWrFn = 0;
    else
	info->destWrFn = 0;
    return len;
}

static int movzx_GvEw_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,B7 */
{
    int len = 1;
    BYTE opSize = info->opSize;			/* save */

    /* force opSize to 2 to get correct srcRdFn */
    info->opSize = 2;
    len += modrm_decode(eip+1, info, 0, movzx_GvEw_das,
			EaSrcNoDest);
    info->opSize = opSize;			/* restore */
    info->destWrFn = 0;
    return len;
}

static int jmpe_Jv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,B8 */
{
    int len = 1;

    len += iAimm(eip+1, &info->disp32, info->opSize);
    info->execFn = 0;
    info->dasFn = jmpe_Jv_das;
    return len;
}

/* reserved */							/* 0F,B9 */

static GroupInfo group8_info[8] = {	/* reg_op */
    { 0, 0, NoSrcEaDest },	/* 0 - opSize & sdt arbitrary */
    { 0, 0, NoSrcEaDest },	/* 1 - opSize & sdt arbitrary */
    { 0, 0, NoSrcEaDest },	/* 2 - opSize & sdt arbitrary */
    { 0, 0, NoSrcEaDest },	/* 3 - opSize & sdt arbitrary */
    { 0,	    0, NoSrcEaDest },	/* 4 */
    { 0,	    0, NoSrcEaDest },	/* 5 */
    { 0,	    0, NoSrcEaDest },	/* 6 */
    { 0,	    0, NoSrcEaDest }	/* 7 */
};

static int group8_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,BA */
{
    ModRM modrm;
    GroupInfoPtr gp;
    int len = 1;

    if (!memMIAIRd(eip+1, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    gp = &group8_info[modrm.reg_op];
    len += modrm_decode(eip+1, info, gp->execFn, group8_das, gp->sdt);
    len += iAimm(eip+len, &info->imm32, 1);
    info->srcRdFn = 0;
    return len;
}

static int btc_EvGv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,BB */
{
    return 1 + modrm_decode(eip+1, info, 0, btc_EvGv_das, RegSrcEaDest);
}

static int bsf_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,BC */
{
    return 1 + modrm_decode(eip+1, info, 0, bsf_GvEv_das,
			    EaSrcRegDest);
}

static int bsr_GvEv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,BD */
{
    return 1 + modrm_decode(eip+1, info, 0, bsr_GvEv_das,
			    EaSrcRegDest);
}

static int movsx_GvEb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,BE */
{
    int len = 1;
    BYTE opSize = info->opSize;			/* save */

    /* force opSize to 1 to get correct srcRdFn */
    info->opSize = 1;
    len += modrm_decode(eip+1, info, 0, movsx_GvEb_das,
			EaSrcNoDest);
    if ((info->opSize = opSize) == IA16)	/* restore */
	info->destWrFn = 0;
    else
	info->destWrFn = 0;
    return len;
}

static int movsx_GvEw_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,BF */
{
    int len = 1;
    BYTE opSize = info->opSize;			/* save */

    /* force opSize to 2 to get correct srcRdFn */
    info->opSize = 2;
    len += modrm_decode(eip+1, info, 0, movsx_GvEw_das,
			EaSrcNoDest);
    info->opSize = opSize;			/* restore */
    info->destWrFn = 0;
    return len;
}

static int xadd_EbGb_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,C0 */
{
    info->opSize = 1;
    return 1 + modrm_decode(eip+1, info, 0, xadd_ExGx_das,
			    RegSrcEaDest);
}

static int xadd_EvGv_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,C1 */
{
    return 1 + modrm_decode(eip+1, info, 0, xadd_ExGx_das,
			    RegSrcEaDest);
}

/* reserved */							/* 0F,C2-C6 */

static GroupInfo group9_info[8] = {	/* reg_op */
    { 0,  0, NoSrcEaDest },	/* 0 - opSize & sdt arbitrary*/
    { 0, 8, NoSrcEaDest },	/* 1 */
    { 0,  0, NoSrcEaDest },	/* 2 - opSize & sdt arbitrary*/
    { 0,  0, NoSrcEaDest },	/* 3 - opSize & sdt arbitrary*/
    { 0,  0, NoSrcEaDest },	/* 4 - opSize & sdt arbitrary*/
    { 0,  0, NoSrcEaDest },	/* 5 - opSize & sdt arbitrary*/
    { 0,  0, NoSrcEaDest },	/* 6 - opSize & sdt arbitrary*/
    { 0,  0, NoSrcEaDest }	/* 7 - opSize & sdt arbitrary*/
};

static int group9_decode(ADDR4 eip, IAinstInfoPtr info)		/* 0F,C7 */
{
    ModRM modrm;
    GroupInfoPtr gp;
    int len = 1;

    if (!memMIAIRd(eip+1, (BYTE *)&modrm, 1))
	return DECODE_FAILED;

    gp = &group9_info[modrm.reg_op];
    if (gp->opSize)
	info->opSize = gp->opSize;
    len += modrm_decode(eip+1, info, gp->execFn, group9_das, gp->sdt);

    /* cmpxchg8b: only memory address forms allowed */
    if (modrm.reg_op == 1 && modrm.mod == 3)
	info->execFn = 0;

    return len;
}

/* ARGSUSED */
static int bswap_EAX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,C8 */
{
    return iAreg(info, 0, bswap_reg_das, EAX_REG);
}

/* ARGSUSED */
static int bswap_ECX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,C9 */
{
    return iAreg(info, 0, bswap_reg_das, ECX_REG);
}

/* ARGSUSED */
static int bswap_EDX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,CA */
{
    return iAreg(info, 0, bswap_reg_das, EDX_REG);
}

/* ARGSUSED */
static int bswap_EBX_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,CB */
{
    return iAreg(info, 0, bswap_reg_das, EBX_REG);
}

/* ARGSUSED */
static int bswap_ESP_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,CC */
{
    return iAreg(info, 0, bswap_reg_das, ESP_REG);
}

/* ARGSUSED */
static int bswap_EBP_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,CD */
{
    return iAreg(info, 0, bswap_reg_das, EBP_REG);
}

/* ARGSUSED */
static int bswap_ESI_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,CE */
{
    return iAreg(info, 0, bswap_reg_das, ESI_REG);
}

/* ARGSUSED */
static int bswap_EDI_decode(ADDR4 eip, IAinstInfoPtr info)	/* 0F,CF */
{
    return iAreg(info, 0, bswap_reg_das, EDI_REG);
}

/* reserved */							/* 0F,D0-FF */


/* Two-Byte Opcode Table */

static const PIADF two_byte_opcode[256] = {
    group6_decode,		/* 0F,00 */
    group7_decode,		/* 0F,01 */
    lar_GvEw_decode,		/* 0F,02 */
    lsl_GvEw_decode,		/* 0F,03 */
    reserved_decode,		/* 0F,04 */
    reserved_decode,		/* 0F,05 */
    clts_decode,		/* 0F,06 */
    reserved_decode,		/* 0F,07 */
    invd_decode,		/* 0F,08 */
    wbinvd_decode,		/* 0F,09 */
    reserved_decode,		/* 0F,0A */
    ud2_decode,			/* 0F,0B */
    reserved_decode,		/* 0F,0C */
    reserved_decode,		/* 0F,0D */
    reserved_decode,		/* 0F,0E */
    reserved_decode,		/* 0F,0F */
    reserved_decode,		/* 0F,10 */
    reserved_decode,		/* 0F,11 */
    reserved_decode,		/* 0F,12 */
    reserved_decode,		/* 0F,13 */
    reserved_decode,		/* 0F,14 */
    reserved_decode,		/* 0F,15 */
    reserved_decode,		/* 0F,16 */
    reserved_decode,		/* 0F,17 */
    reserved_decode,		/* 0F,18 */
    reserved_decode,		/* 0F,19 */
    reserved_decode,		/* 0F,1A */
    reserved_decode,		/* 0F,1B */
    reserved_decode,		/* 0F,1C */
    reserved_decode,		/* 0F,1D */
    reserved_decode,		/* 0F,1E */
    reserved_decode,		/* 0F,1F */
    mov_RdCd_decode,		/* 0F,20 */
    mov_RdDd_decode,		/* 0F,21 */
    mov_CdRd_decode,		/* 0F,22 */
    mov_DdRd_decode,		/* 0F,23 */
    mov_RdTd_decode,		/* 0F,24 */
    reserved_decode,		/* 0F,25 */
    mov_TdRd_decode,		/* 0F,26 */
    reserved_decode,		/* 0F,27 */
    reserved_decode,		/* 0F,28 */
    reserved_decode,		/* 0F,29 */
    reserved_decode,		/* 0F,2A */
    reserved_decode,		/* 0F,2B */
    reserved_decode,		/* 0F,2C */
    reserved_decode,		/* 0F,2D */
    reserved_decode,		/* 0F,2E */
    reserved_decode,		/* 0F,2F */
    wrmsr_decode,		/* 0F,30 */
    rdtsc_decode,		/* 0F,31 */
    rdmsr_decode,		/* 0F,32 */
    rdpmc_decode,		/* 0F,33 */
    reserved_decode,		/* 0F,34 */
    reserved_decode,		/* 0F,35 */
    reserved_decode,		/* 0F,36 */
    reserved_decode,		/* 0F,37 */
    reserved_decode,		/* 0F,38 */
    reserved_decode,		/* 0F,39 */
    reserved_decode,		/* 0F,3A */
    reserved_decode,		/* 0F,3B */
    reserved_decode,		/* 0F,3C */
    reserved_decode,		/* 0F,3D */
    reserved_decode,		/* 0F,3E */
    reserved_decode,		/* 0F,3F */
    cmovo_GvEv_decode,		/* 0F,40 */
    cmovno_GvEv_decode,		/* 0F,41 */
    cmovb_GvEv_decode,		/* 0F,42 */
    cmovnb_GvEv_decode,		/* 0F,43 */
    cmovz_GvEv_decode,		/* 0F,44 */
    cmovnz_GvEv_decode,		/* 0F,45 */
    cmovbe_GvEv_decode,		/* 0F,46 */
    cmovnbe_GvEv_decode,	/* 0F,47 */
    cmovs_GvEv_decode,		/* 0F,48 */
    cmovns_GvEv_decode,		/* 0F,49 */
    cmovp_GvEv_decode,		/* 0F,4A */
    cmovnp_GvEv_decode,		/* 0F,4B */
    cmovl_GvEv_decode,		/* 0F,4C */
    cmovnl_GvEv_decode,		/* 0F,4D */
    cmovle_GvEv_decode,		/* 0F,4E */
    cmovnle_GvEv_decode,	/* 0F,4F */
    reserved_decode,		/* 0F,50 */
    reserved_decode,		/* 0F,51 */
    reserved_decode,		/* 0F,52 */
    reserved_decode,		/* 0F,53 */
    reserved_decode,		/* 0F,54 */
    reserved_decode,		/* 0F,55 */
    reserved_decode,		/* 0F,56 */
    reserved_decode,		/* 0F,57 */
    reserved_decode,		/* 0F,58 */
    reserved_decode,		/* 0F,59 */
    reserved_decode,		/* 0F,5A */
    reserved_decode,		/* 0F,5B */
    reserved_decode,		/* 0F,5C */
    reserved_decode,		/* 0F,5D */
    reserved_decode,		/* 0F,5E */
    reserved_decode,		/* 0F,5F */
    reserved_decode,		/* 0F,60 */
    reserved_decode,		/* 0F,61 */
    reserved_decode,		/* 0F,62 */
    reserved_decode,		/* 0F,63 */
    reserved_decode,		/* 0F,64 */
    reserved_decode,		/* 0F,65 */
    reserved_decode,		/* 0F,66 */
    reserved_decode,		/* 0F,67 */
    reserved_decode,		/* 0F,68 */
    reserved_decode,		/* 0F,69 */
    reserved_decode,		/* 0F,6A */
    reserved_decode,		/* 0F,6B */
    reserved_decode,		/* 0F,6C */
    reserved_decode,		/* 0F,6D */
    reserved_decode,		/* 0F,6E */
    reserved_decode,		/* 0F,6F */
    reserved_decode,		/* 0F,70 */
    reserved_decode,		/* 0F,71 */
    reserved_decode,		/* 0F,72 */
    reserved_decode,		/* 0F,73 */
    reserved_decode,		/* 0F,74 */
    reserved_decode,		/* 0F,75 */
    reserved_decode,		/* 0F,76 */
    reserved_decode,		/* 0F,77 */
    reserved_decode,		/* 0F,78 */
    reserved_decode,		/* 0F,79 */
    reserved_decode,		/* 0F,7A */
    reserved_decode,		/* 0F,7B */
    reserved_decode,		/* 0F,7C */
    reserved_decode,		/* 0F,7D */
    reserved_decode,		/* 0F,7E */
    reserved_decode,		/* 0F,7F */
    jo_Jv_decode,		/* 0F,80 */
    jno_Jv_decode,		/* 0F,81 */
    jb_Jv_decode,		/* 0F,82 */
    jnb_Jv_decode,		/* 0F,83 */
    jz_Jv_decode,		/* 0F,84 */
    jnz_Jv_decode,		/* 0F,85 */
    jbe_Jv_decode,		/* 0F,86 */
    jnbe_Jv_decode,		/* 0F,87 */
    js_Jv_decode,		/* 0F,88 */
    jns_Jv_decode,		/* 0F,89 */
    jp_Jv_decode,		/* 0F,8A */
    jnp_Jv_decode,		/* 0F,8B */
    jl_Jv_decode,		/* 0F,8C */
    jnl_Jv_decode,		/* 0F,8D */
    jle_Jv_decode,		/* 0F,8E */
    jnle_Jv_decode,		/* 0F,8F */
    seto_Eb_decode,		/* 0F,90 */
    setno_Eb_decode,		/* 0F,91 */
    setb_Eb_decode,		/* 0F,92 */
    setnb_Eb_decode,		/* 0F,93 */
    setz_Eb_decode,		/* 0F,94 */
    setnz_Eb_decode,		/* 0F,95 */
    setbe_Eb_decode,		/* 0F,96 */
    setnbe_Eb_decode,		/* 0F,97 */
    sets_Eb_decode,		/* 0F,98 */
    setns_Eb_decode,		/* 0F,99 */
    setp_Eb_decode,		/* 0F,9A */
    setnp_Eb_decode,		/* 0F,9B */
    setl_Eb_decode,		/* 0F,9C */
    setnl_Eb_decode,		/* 0F,9D */
    setle_Eb_decode,		/* 0F,9E */
    setnle_Eb_decode,		/* 0F,9F */
    push_FS_decode,		/* 0F,A0 */
    pop_FS_decode,		/* 0F,A1 */
    cpuid_decode,		/* 0F,A2 */
    bt_EvGv_decode,		/* 0F,A3 */
    shld_EvGvIb_decode,		/* 0F,A4 */
    shld_EvGvCL_decode,		/* 0F,A5 */
/* XXX - why do the Pentium & PPro opcode tables show A6 & A7 as Reserved? */
    cmpxchg_XBTS_decode,	/* 0F,A6 */
    cmpxchg_IBTS_decode,	/* 0F,A7 */
    push_GS_decode,		/* 0F,A8 */
    pop_GS_decode,		/* 0F,A9 */
    rsm_decode,			/* 0F,AA */
    bts_EvGv_decode,		/* 0F,AB */
    shrd_EvGvIb_decode,		/* 0F,AC */
    shrd_EvGvCL_decode,		/* 0F,AD */
    reserved_decode,		/* 0F,AE */
    imul_GvEv_decode,		/* 0F,AF */
    cmpxchg_EbGb_decode,	/* 0F,B0 */
    cmpxchg_EvGv_decode,	/* 0F,B1 */
    lss_Mp_decode,		/* 0F,B2 */
    btr_EvGv_decode,		/* 0F,B3 */
    lfs_Mp_decode,		/* 0F,B4 */
    lgs_Mp_decode,		/* 0F,B5 */
    movzx_GvEb_decode,		/* 0F,B6 */
    movzx_GvEw_decode,		/* 0F,B7 */
    jmpe_Jv_decode,		/* 0F,B8 */
    reserved_decode,		/* 0F,B9 */
    group8_decode,		/* 0F,BA */
    btc_EvGv_decode,		/* 0F,BB */
    bsf_GvEv_decode,		/* 0F,BC */
    bsr_GvEv_decode,		/* 0F,BD */
    movsx_GvEb_decode,		/* 0F,BE */
    movsx_GvEw_decode,		/* 0F,BF */
    xadd_EbGb_decode,		/* 0F,C0 */
    xadd_EvGv_decode,		/* 0F,C1 */
    reserved_decode,		/* 0F,C2 */
    reserved_decode,		/* 0F,C3 */
    reserved_decode,		/* 0F,C4 */
    reserved_decode,		/* 0F,C5 */
    reserved_decode,		/* 0F,C6 */
    group9_decode,		/* 0F,C7 */
    bswap_EAX_decode,		/* 0F,C8 */
    bswap_ECX_decode,		/* 0F,C9 */
    bswap_EDX_decode,		/* 0F,CA */
    bswap_EBX_decode,		/* 0F,CB */
    bswap_ESP_decode,		/* 0F,CC */
    bswap_EBP_decode,		/* 0F,CD */
    bswap_ESI_decode,		/* 0F,CE */
    bswap_EDI_decode,		/* 0F,CF */
    reserved_decode,		/* 0F,D0 */
    reserved_decode,		/* 0F,D1 */
    reserved_decode,		/* 0F,D2 */
    reserved_decode,		/* 0F,D3 */
    reserved_decode,		/* 0F,D4 */
    reserved_decode,		/* 0F,D5 */
    reserved_decode,		/* 0F,D6 */
    reserved_decode,		/* 0F,D7 */
    reserved_decode,		/* 0F,D8 */
    reserved_decode,		/* 0F,D9 */
    reserved_decode,		/* 0F,DA */
    reserved_decode,		/* 0F,DB */
    reserved_decode,		/* 0F,DC */
    reserved_decode,		/* 0F,DD */
    reserved_decode,		/* 0F,DE */
    reserved_decode,		/* 0F,DF */
    reserved_decode,		/* 0F,E0 */
    reserved_decode,		/* 0F,E1 */
    reserved_decode,		/* 0F,E2 */
    reserved_decode,		/* 0F,E3 */
    reserved_decode,		/* 0F,E4 */
    reserved_decode,		/* 0F,E5 */
    reserved_decode,		/* 0F,E6 */
    reserved_decode,		/* 0F,E7 */
    reserved_decode,		/* 0F,E8 */
    reserved_decode,		/* 0F,E9 */
    reserved_decode,		/* 0F,EA */
    reserved_decode,		/* 0F,EB */
    reserved_decode,		/* 0F,EC */
    reserved_decode,		/* 0F,ED */
    reserved_decode,		/* 0F,EE */
    reserved_decode,		/* 0F,EF */
    reserved_decode,		/* 0F,F0 */
    reserved_decode,		/* 0F,F1 */
    reserved_decode,		/* 0F,F2 */
    reserved_decode,		/* 0F,F3 */
    reserved_decode,		/* 0F,F4 */
    reserved_decode,		/* 0F,F5 */
    reserved_decode,		/* 0F,F6 */
    reserved_decode,		/* 0F,F7 */
    reserved_decode,		/* 0F,F8 */
    reserved_decode,		/* 0F,F9 */
    reserved_decode,		/* 0F,FA */
    reserved_decode,		/* 0F,FB */
    reserved_decode,		/* 0F,FC */
    reserved_decode,		/* 0F,FD */
    reserved_decode,		/* 0F,FE */
    reserved_decode		/* 0F,FF */
};


/* Main IA Decode Function */

BOOL iadas_decode(ADDR4 eip, IAinstInfoPtr info)
{
    BYTE op;
    int ret;
    static LockRep zero = { 0 };

    info->mode = iAmode;
    info->opSize = info->addrSize = OpAddrSize(iAmode);
    info->segment = 0;
    info->lock_rep = zero;

    if (!memMIAIRd(eip, &op, 1) ||
	(ret = one_byte_opcode[op](eip, info)) <= 0) {
	info->execFn = 0;	/* decode failed */
	return NO;
    }

    info->delta = ret;
    return YES;
}

/* Secondary IA Decode Function */

static int ia_decode2(ADDR4 eip, IAinstInfoPtr info)
{
    BYTE op;

    if (!memMIAIRd(eip, &op, 1))
	return DECODE_FAILED;

    return one_byte_opcode[op](eip, info);
}
