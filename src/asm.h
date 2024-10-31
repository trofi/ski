/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Mini-Assembler Header
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
#ifndef _SKI_ASM_H
#define _SKI_ASM_H

#include <string.h>

#include "encoder/encoder.h"

/* these enum values must match the EncStatus enum values */
typedef enum { AsmOK, AsmBadTemplate, AsmBadInst, AsmBadQP, AsmBadOperand,
	       AsmBadOperand1 = AsmBadOperand, AsmBadOperand2,
	       AsmBadOperand3, AsmBadOperand4, AsmBadOperand5,
	       AsmBadOperand6, AsmNoInst, AsmStop, AsmBadEquals, AsmBadAssign
} AsmStatus;

struct asm_id { char *name; InstID id; };
typedef const struct asm_id *AsmIdPtr;

AsmStatus asm_init(void);
AsmStatus asm_inst(const char *str, InstID *instId, OperandsPtr ops,
		   EncodedInstPtr inst);
AsmStatus asm_templ(const char *str, unsigned char *templ);
AsmIdPtr inst_lookup(const char *str);

#endif	/* _SKI_ASM_H */
