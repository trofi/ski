/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * EM instruction encoder header
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
#ifndef _SKI_ENCODER_H
#define _SKI_ENCODER_H

typedef enum { EncOK, EncBadTemplate, EncBadInstID, EncBadQP, EncBadOperand,
	       EncBadOperand1 = EncBadOperand, EncBadOperand2,
	       EncBadOperand3, EncBadOperand4, EncBadOperand5,
	       EncBadOperand6
} EncStatus;

#include "decoder/instID.gen.h"
#include "decoder/decoder.h"
#include "decoder/operands.gen.h"

typedef struct encoded_inst {
    unsigned long long bits;
    unsigned long long extra_bits;
    Unit unit;
    SB	 stop;
    unsigned char valid_extra_bits;
    unsigned char slot;
} EncodedInst, *EncodedInstPtr;

EncStatus encode_inst(InstID instID, OperandsPtr opnds,
		      EncodedInstPtr inst);
unsigned long long encoderIP(void);
char *instFormatName(InstID instID);

#endif /* _SKI_ENCODER_H */
