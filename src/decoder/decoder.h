/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Decoder Header File
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
#ifndef _SKI_DECODER_H
#define _SKI_DECODER_H

#include <instID.gen.h>
#include <operands.gen.h>

#define SLOTS_PER_BUNDLE	3

typedef struct {
    unsigned long long right;
    unsigned long long left;
} Bundle, *BundlePtr;

typedef struct bundle_parts {
    unsigned long long slot[SLOTS_PER_BUNDLE];
    unsigned char templSB;
} BundleParts, *BundlePartsPtr;

typedef struct decoded_instr {
    InstID instID;
    unsigned long long instrBits;
    Operands opnds;
} DecodedInstr, *DecodedInstPtr;

typedef enum { I_Unit, M_Unit, F_Unit, B_Unit, X_Unit, L_Unit, A_Unit,
	       No_Unit } Unit;
typedef enum { SB_Cont, SB_Stop } SB;

typedef struct template_info {
    struct {
	Unit unit;
	SB   stop;
    } slot[SLOTS_PER_BUNDLE];
} TemplateInfo, *TemplateInfoPtr;

TemplateInfoPtr bundle_decode(const BundlePtr bundle,
			      DecodedInstr instrs[SLOTS_PER_BUNDLE],
			      const unsigned int include_opnds);
BundleParts bundle_parts(const BundlePtr bundle);
Bundle bundle_from_parts(const BundlePartsPtr parts);
InstID instr_decode(const Unit unit, const unsigned long long slot,
		    OperandsPtr opnds);
TemplateInfoPtr template_info(const unsigned char templSB);
unsigned char templSB_from_info(const TemplateInfoPtr t);
#ifdef DECODERIP
unsigned long long decoderIP(void);
#endif /* DECODERIP */
#endif /* _SKI_DECODER_H */
