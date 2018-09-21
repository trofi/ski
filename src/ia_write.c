/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator IA Instruction Write Routines
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
#include "ia_types.h"
#include "ia_state.h"
#include "ia_exec.h"


static void r8IAWr(BYTE reg, IADWORD val)
{
    switch (reg) {
    case AL_REG:
	AL = val;
	break;
    case CL_REG:
	CL = val;
	break;
    case DL_REG:
	DL = val;
	break;
    case BL_REG:
	BL = val;
	break;
    case AH_REG:
	AH = val;
	break;
    case CH_REG:
	CH = val;
	break;
    case DH_REG:
	DH = val;
	break;
    case BH_REG:
	BH = val;
	break;
    }
}

static void r16IAWr(BYTE reg, IADWORD val)
{
    switch (reg) {
    case AX_REG:
	AX = val;
	break;
    case CX_REG:
	CX = val;
	break;
    case DX_REG:
	DX = val;
	break;
    case BX_REG:
	BX = val;
	break;
    case SP_REG:
	IASP = val;
	break;
    case BP_REG:
	BP = val;
	break;
    case SI_REG:
	SI = val;
	break;
    case DI_REG:
	DI = val;
	break;
    }
}

static void r32IAWr(BYTE reg, IADWORD val)
{
    switch (reg) {
    case EAX_REG:
	EAX = val;
	break;
    case ECX_REG:
	ECX = val;
	break;
    case EDX_REG:
	EDX = val;
	break;
    case EBX_REG:
	EBX = val;
	break;
    case ESP_REG:
	ESP = val;
	break;
    case EBP_REG:
	EBP = val;
	break;
    case ESI_REG:
	ESI = val;
	break;
    case EDI_REG:
	EDI = val;
	break;
    }
}

/* ARGSUSED */
void reg8IAWr(IAinstInfoPtr info, IADWORD val, ADDR adr, ADDR adr2)
{
    r8IAWr(info->reg, val);
}

/* ARGSUSED */
void reg16IAWr(IAinstInfoPtr info, IADWORD val, ADDR adr, ADDR adr2)
{
    r16IAWr(info->reg, val);
}

/* ARGSUSED */
void reg32IAWr(IAinstInfoPtr info, IADWORD val, ADDR adr, ADDR adr2)
{
    r32IAWr(info->reg, val);
}

void regIAWr(IAinstInfoPtr info, IADWORD val, ADDR adr, ADDR adr2)
{
    switch (info->opSize) {
    case 1:
	reg8IAWr(info, val, adr, adr2);
	break;
    case 2:
	reg16IAWr(info, val, adr, adr2);
	break;
    case 4:
	reg32IAWr(info, val, adr, adr2);
	break;
    }
}

/* ARGSUSED */
void base8IAWr(IAinstInfoPtr info, IADWORD val, ADDR adr, ADDR adr2)
{
    r8IAWr(info->base, val);
}

/* ARGSUSED */
void base16IAWr(IAinstInfoPtr info, IADWORD val, ADDR adr, ADDR adr2)
{
    r16IAWr(info->base, val);
}

/* ARGSUSED */
void base32IAWr(IAinstInfoPtr info, IADWORD val, ADDR adr, ADDR adr2)
{
    r32IAWr(info->base, val);
}

/* ARGSUSED */
void segRegIAWr(IAinstInfoPtr info, IADWORD val, ADDR adr, ADDR adr2)
{
    SEGREG(info->cond_seg + ES_ID) = val;
    /* XXX - add protected mode & VM86 execution */
    SEGD(info->cond_seg + ES_ID).base = val << 4;
    SEGD(info->cond_seg + ES_ID).lim = 0xffff;
}

void memIAWr(IAinstInfoPtr info, IADWORD val, ADDR adr, ADDR adr2)
{
    memIAWrt(info->opSize, adr, adr2, val);
}

void push_spIAWr(IAinstInfoPtr info, IADWORD val, ADDR adr, ADDR adr2)
{
    SP -= info->opSize;
    memIAWrt(info->opSize, adr, adr2, val);
}

void push_espIAWr(IAinstInfoPtr info, IADWORD val, ADDR adr, ADDR adr2)
{
    ESP -= info->opSize;
    memIAWrt(info->opSize, adr, adr2, val);
}

void pusha_spIAWr(IAinstInfoPtr info, IADWORD val, ADDR adr, ADDR adr2)
{
    memIAWrt(8 * info->opSize, adr, adr2, val);
}

void pusha_espIAWr(IAinstInfoPtr info, IADWORD val, ADDR adr, ADDR adr2)
{
    memIAWrt(8 * info->opSize, adr, adr2, val);
}
