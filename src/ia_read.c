/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator IA Instruction Read Routines
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
#include "simmem.h"
#include "ia_types.h"
#include "ia_state.h"
#include "ia_exec.h"
#include "ia_read.h"

/* ARGSUSED */
Status immIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
	       ADDR *adr, ADDR *adr2)
{
    *val = info->imm32;
    return 1;
}

static Status r8IARd(BYTE reg, IADWORD *val)
{
    switch (reg) {
    case AL_REG:
	*val = AL;
	break;
    case CL_REG:
	*val = CL;
	break;
    case DL_REG:
	*val = DL;
	break;
    case BL_REG:
	*val = BL;
	break;
    case AH_REG:
	*val = AH;
	break;
    case CH_REG:
	*val = CH;
	break;
    case DH_REG:
	*val = DH;
	break;
    case BH_REG:
	*val = BH;
	break;
    }
    return 1;
}

static Status r16IARd(BYTE reg, IADWORD *val)
{
    switch (reg) {
    case AX_REG:
	*val = AX;
	break;
    case CX_REG:
	*val = CX;
	break;
    case DX_REG:
	*val = DX;
	break;
    case BX_REG:
	*val = BX;
	break;
    case SP_REG:
	*val = SP;
	break;
    case BP_REG:
	*val = BP;
	break;
    case SI_REG:
	*val = SI;
	break;
    case DI_REG:
	*val = DI;
	break;
    case NO_REG:
	*val = 0;
	break;
    }
    return 1;
}

static Status r32IARd(BYTE reg, IADWORD *val)
{
    switch (reg) {
    case EAX_REG:
	*val = EAX;
	break;
    case ECX_REG:
	*val = ECX;
	break;
    case EDX_REG:
	*val = EDX;
	break;
    case EBX_REG:
	*val = EBX;
	break;
    case ESP_REG:
	*val = ESP;
	break;
    case EBP_REG:
	*val = EBP;
	break;
    case ESI_REG:
	*val = ESI;
	break;
    case EDI_REG:
	*val = EDI;
	break;
    case NO_REG:
	*val = 0;
	break;
    }
    return 1;
}

/* ARGSUSED */
Status reg8IARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		ADDR *adr, ADDR *adr2)
{
    return r8IARd(info->reg, val);
}

/* ARGSUSED */
Status reg16IARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		 ADDR *adr, ADDR *adr2)
{
    return r16IARd(info->reg, val);
}

/* ARGSUSED */
Status reg32IARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		 ADDR *adr, ADDR *adr2)
{
    return r32IARd(info->reg, val);
}

/* ARGSUSED */
Status base8IARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		 ADDR *adr, ADDR *adr2)
{
    return r8IARd(info->base, val);
}

/* ARGSUSED */
Status base16IARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		  ADDR *adr, ADDR *adr2)
{
    return r16IARd(info->base, val);
}

/* ARGSUSED */
Status base32IARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		  ADDR *adr, ADDR *adr2)
{
    return r32IARd(info->base, val);
}

/* ARGSUSED */
Status segRegIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		  ADDR *adr, ADDR *adr2)
{
    *val = SEGREG(info->reg + ES_ID);
    return 1;
}

/* ARGSUSED */
Status segRegUpdateIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
			ADDR *adr, ADDR *adr2)
{
    /* XXX - add protected mode & VM86 checks */
    *val = SEGREG(info->cond_seg + ES_ID);
    return 1;
}

/* 16-bit Addressing Read Functions */

Status modrm16_dispIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
			ADDR *adr, ADDR *adr2)
{
    IADWORD base, index;
    ADDR2 ofs;

    (void)r16IARd(info->base, &base);
    (void)r16IARd(info->index, &index);
    ofs = base + index + info->disp32;
    return memIARd(info->segment, ofs, info->opSize, acc, adr, adr2, val);
}

Status load_far_ptr_16IARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
			   ADDR *adr, ADDR *adr2)
{
    IADWORD base, index;
    ADDR2 ofs;

    (void)r16IARd(info->base, &base);
    (void)r16IARd(info->index, &index);
    ofs = base + index + info->disp32;
    return memIARd(info->segment, ofs, info->opSize + 2, acc, adr, adr2, val);
}

Status lea16IARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		 ADDR *adr, ADDR *adr2)
{
    IADWORD base, index;
    ADDR2 ofs;

    (void)r16IARd(info->base, &base);
    (void)r16IARd(info->index, &index);
    ofs = base + index + info->disp32;
    *val = ofs;
    return 1;
}

Status bound16IARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		   ADDR *adr, ADDR *adr2)
{
    IADWORD base, index;
    ADDR2 ofs, ofs2;

    (void)r16IARd(info->base, &base);
    (void)r16IARd(info->index, &index);
    ofs = base + index + info->disp32;			/* lower bound */
    ofs2 = base + index + info->disp32 + info->opSize;	/* upper bound */
    if (!memIARd(info->segment, ofs, info->opSize, acc, adr, adr2, val))
	return 0;
    return memIARd(info->segment, ofs2, info->opSize, acc, adr, adr2, val + 1);
}

Status push_spIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		   ADDR *adr, ADDR *adr2)
{
    ADDR2 ofs;

    ofs = (ADDR2)(SP - info->opSize);
    return memIARd(SS_ID, ofs, info->opSize, acc, adr, adr2, val);
}

Status pop_spIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		  ADDR *adr, ADDR *adr2)
{
    ADDR2 ofs;

    ofs = (ADDR2)SP;
    return memIARd(SS_ID, ofs, info->opSize, acc, adr, adr2, val);
}

Status pusha_spIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		    ADDR *adr, ADDR *adr2)
{
    ADDR2 ofs;

    ofs = (ADDR2)(SP - 8 * info->opSize);
    return memIARd(SS_ID, ofs, 8 * info->opSize, acc, adr, adr2, val);
}

Status popa_spIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		   ADDR *adr, ADDR *adr2)
{
    ADDR2 ofs;

    ofs = (ADDR2)SP;
    return memIARd(SS_ID, ofs, 8 * info->opSize, acc, adr, adr2, val);
}

Status str_siIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		  ADDR *adr, ADDR *adr2)
{
    ADDR2 ofs;

    ofs = (ADDR2)SI;
    return memIARd(info->segment, ofs, info->opSize, acc, adr, adr2, val);
}

Status str_diIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		  ADDR *adr, ADDR *adr2)
{
    ADDR2 ofs;

    ofs = (ADDR2)DI;
    return memIARd(ES_ID, ofs, info->opSize, acc, adr, adr2, val);
}

/* 32-bit Addressing Read Functions */

Status modrm32_dispIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
			ADDR *adr, ADDR *adr2)
{
    IADWORD base, index;
    ADDR4 ofs;

    (void)r32IARd(info->base, &base);
    (void)r32IARd(info->index, &index);
    ofs = base + index + info->disp32;
    return memIARd(info->segment, ofs, info->opSize, acc, adr, adr2, val);
}

Status load_far_ptr_32IARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
			   ADDR *adr, ADDR *adr2)
{
    IADWORD base, index;
    ADDR4 ofs;

    (void)r32IARd(info->base, &base);
    (void)r32IARd(info->index, &index);
    ofs = base + index + info->disp32;
    return memIARd(info->segment, ofs, info->opSize + 2, acc, adr, adr2, val);
}

Status sib_dispIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		    ADDR *adr, ADDR *adr2)
{
    IADWORD base, index;
    ADDR4 ofs;

    (void)r32IARd(info->base, &base);
    (void)r32IARd(info->index, &index);
    ofs = base + (index * info->scale) + info->disp32;
    return memIARd(info->segment, ofs, info->opSize, acc, adr, adr2, val);
}

Status load_far_ptr_sibIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
			    ADDR *adr, ADDR *adr2)
{
    IADWORD base, index;
    ADDR4 ofs;

    (void)r32IARd(info->base, &base);
    (void)r32IARd(info->index, &index);
    ofs = base + (index * info->scale) + info->disp32;
    return memIARd(info->segment, ofs, info->opSize + 2, acc, adr, adr2, val);
}

Status lea32IARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		 ADDR *adr, ADDR *adr2)
{
    IADWORD base, index;
    ADDR4 ofs;

    (void)r32IARd(info->base, &base);
    (void)r32IARd(info->index, &index);
    ofs = base + index + info->disp32;
    *val = ofs;
    return 1;
}

Status bound32IARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		   ADDR *adr, ADDR *adr2)
{
    IADWORD base, index;
    ADDR4 ofs, ofs2;

    (void)r32IARd(info->base, &base);
    (void)r32IARd(info->index, &index);
    ofs = base + index + info->disp32;			/* lower bound */
    ofs2 = base + index + info->disp32 + info->opSize;	/* upper bound */
    if (!memIARd(info->segment, ofs, info->opSize, acc, adr, adr2, val))
	return 0;
    return memIARd(info->segment, ofs2, info->opSize, acc, adr, adr2, val + 1);
}

Status leaSibIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		  ADDR *adr, ADDR *adr2)
{
    IADWORD base, index;
    ADDR4 ofs;

    (void)r32IARd(info->base, &base);
    (void)r32IARd(info->index, &index);
    ofs = base + (index * info->scale) + info->disp32;
    *val = ofs;
    return 1;
}

Status boundSibIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		    ADDR *adr, ADDR *adr2)
{
    IADWORD base, index;
    ADDR4 ofs, ofs2;

    (void)r32IARd(info->base, &base);
    (void)r32IARd(info->index, &index);
    ofs = base + (index * info->scale) + info->disp32;
    ofs2 = base + (index * info->scale) + info->disp32 + info->opSize;
    if (!memIARd(info->segment, ofs, info->opSize, acc, adr, adr2, val))
	return 0;
    return memIARd(info->segment, ofs2, info->opSize, acc, adr, adr2, val + 1);
}

Status push_espIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		    ADDR *adr, ADDR *adr2)
{
    ADDR4 ofs;

    ofs = (ADDR4)(ESP - info->opSize);
    return memIARd(SS_ID, ofs, info->opSize, acc, adr, adr2, val);
}

Status pop_espIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		   ADDR *adr, ADDR *adr2)
{
    ADDR4 ofs;

    ofs = (ADDR4)ESP;
    return memIARd(SS_ID, ofs, info->opSize, acc, adr, adr2, val);
}

Status pusha_espIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		     ADDR *adr, ADDR *adr2)
{
    ADDR4 ofs;

    ofs = (ADDR4)(ESP - 8 * info->opSize);
    return memIARd(SS_ID, ofs, 8 * info->opSize, acc, adr, adr2, val);
}

Status popa_espIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		    ADDR *adr, ADDR *adr2)
{
    ADDR4 ofs;

    ofs = (ADDR4)ESP;
    return memIARd(SS_ID, ofs, 8 * info->opSize, acc, adr, adr2, val);
}

Status str_esiIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		   ADDR *adr, ADDR *adr2)
{
    ADDR4 ofs;

    ofs = (ADDR4)ESI;
    return memIARd(info->segment, ofs, info->opSize, acc, adr, adr2, val);
}

Status str_ediIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		   ADDR *adr, ADDR *adr2)
{
    ADDR4 ofs;

    ofs = (ADDR4)EDI;
    return memIARd(ES_ID, ofs, info->opSize, acc, adr, adr2, val);
}

/* Special Read Functions */

Status call_farIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		    ADDR *adr, ADDR *adr2)
{
    ADDR4 ofs;

    if (StackAddrSize(info->mode) == IA16)
	ofs = (ADDR2)(SP - (2 * info->opSize));
    else
	ofs = (ADDR4)(ESP - (2 * info->opSize));

    return memIARd(SS_ID, ofs, info->opSize, acc, adr, adr2, val);
}

Status iretIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		ADDR *adr, ADDR *adr2)
{
    ADDR4 ofs;

    if (StackAddrSize(info->mode) == IA16)
	ofs = (ADDR2)(SP + 2 * info->opSize);
    else
	ofs = (ADDR4)(ESP + 2 * info->opSize);

    return memIARd(SS_ID, ofs, info->opSize, acc, adr, adr2, val);
}

Status ret_farIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		   ADDR *adr, ADDR *adr2)
{
    ADDR4 ofs;

    if (StackAddrSize(info->mode) == IA16)
	ofs = (ADDR2)(SP + info->opSize);
    else
	ofs = (ADDR4)(ESP + info->opSize);

    return memIARd(SS_ID, ofs, info->opSize, acc, adr, adr2, val);
}

Status xlatIARd(IAinstInfoPtr info, Accesstype acc, IADWORD *val,
		ADDR *adr, ADDR *adr2)
{
    ADDR4 ofs;

    if (info->addrSize == IA16)
	ofs = (ADDR2)(BX + AL);
    else
	ofs = (ADDR4)(EBX + AL);

    return memIARd(info->segment, ofs, 1, acc, adr, adr2, val);
}
