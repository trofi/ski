/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Disassembly Library Header
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
#ifndef _SKI_LIBDAS_H
#define _SKI_LIBDAS_H

#include "decoder.h"
#include "encoder.h"

/*
 * The user of this library must provide three functions:
 *     dasAddress
 *     symAddrtoName
 *     dasRelocation
 *
 *  The prototype for dasAddress is:
 *     unsigned long long dasAddress(void)
 *
 *  Its purpose is to return the current disassembly address for use by
 *  instructions which have an ip-relative source (target25 or tag13)
 *
 *  The prototype for symAddrtoName is:
 *     void symAddrtoName(unsigned long long adr, char sname[],
 *                        int rmdr, int add0x, int width)
 *
 *  Its purpose is to fill in sname with a string of the form "sym+hexoffset"
 *  where the value of the symbol "sym" plus the "hexoffset" is equal to the
 *  passed address.  The rmdr argument specifies how "far" the address can be
 *  from the closest symbol and must be >= 0, the add0x argument specifies
 *  whether to add a leading 0x to the hexoffset, and the width argument
 *  specifies the desired width of the generated string (a negative value
 *  means that the result should not be padded but should be truncated to
 *  -width characters).  The symbol chosen should be the symbol whose value
 *  is the greatest value less than or equal to adr that satisfies:
 *            symbol <= adr < symbol + (1 << rmdr*4)
 *  (so rmdr represents the number of hex digits used in "sym+0x0010").
 *
 *  It is called for instructions which have an ip-relative source as
 *     symAddrtoName(adr, name, 4, 1, -40)
 *
 *  and by the movl instruction as
 *     symAddrtoName(adr, name, 0, 1, -40)
 *  which says to use only an exact match for the 64-bit immediate.
 *
 *  The prototype for dasRelocation is:
 *     unsigned int dasRelocation(unsigned long long addr,
 *                                DasRelocationPtr reloc)
 *
 *  Its purpose is to fill in the name, type, and addend fields in the
 *  DasRelocation structure pointed to by reloc for any relocation
 *  corresponding to the instruction at addr (with the instruction's slot
 *  number in the two low-order bits of addr).  A non-zero return value
 *  indicates that there is such a relocation and that the fields in reloc
 *  are valid.
 */

#ifndef _SKI_DECODER_H
typedef struct bundle {
    unsigned long long left, right;
} Bundle;
#endif
#define MAX_DAS_LEN	512

typedef enum {
    DasNoOpts,			/* no options */
    DasTemplate = 0x1,		/* include template type in i0Str */
    DasTemplateComment = 0x3,	/* include template type as comment */
    DasSepStopLines = 0x4,	/* put ";;" (stops) on separate lines */
    DasPseudoOps = 0x8,		/* check for pseudo-ops */
    DasSpaceOperands = 0x10,	/* space-separate operands */
    DasRegNames = 0x20,		/* use register names (gp, sp, rp, ar., etc) */
    DasFormatNumbers = 0x40,	/* include format numbers */
    DasFormatComment = 0xC0,	/* include format number as comment */
    DasTemplateCommas = 0x100,	/* include commas in template types */
    DasHexAddresses = 0x200,	/* include hex addresses */
    DasStackedRegNames = 0x420,	/* use register names (in, loc, out, rot) */
    DasTemplateLC = 0x800,	/* lower case template type */
    DasNoSymMovlImm = 0x1000,	/* movl immediates not symbolic */
    DasNoMnemonicPad = 0x2000	/* mnemonic not padded to 16 spaces */
} DasOptions;

typedef struct {
    char *name;
    unsigned int type;
    long long addend;
} DasRelocation, *DasRelocationPtr;

#define dasBundle(bndl, i0Str, i1Str, i2Str) \
		dasBundleSBT(bndl, i0Str, i1Str, i2Str, 0, 0)
#define dasBundleSB(bndl, i0Str, i1Str, i2Str, sb) \
		dasBundleSBT(bndl, i0Str, i1Str, i2Str, sb, 0)

void dasInit(DasOptions opts, int width);
void dasBundleSBT(Bundle *bndl, char *i0Str, char *i1Str, char *i2Str,
		 SB sb[SLOTS_PER_BUNDLE], char *tStr);
char *hexBundle(Bundle *bndl, char *str);
char *dasInst(Unit unit, unsigned long long instBits,
	      unsigned long long extraInstBits, char *iStr);
char *dasEncodedInst(struct encoded_inst *inst, char *iStr);
unsigned int bundleTargets(Bundle *bndl, unsigned long long tgtAdr[]);
unsigned int dasRelocation(unsigned long long addr, DasRelocationPtr reloc);
void dasSetFrameSize(unsigned int ins, unsigned int locs, unsigned int outs,
		     unsigned int rots);

#endif	/* _SKI_LIBDAS_H */
