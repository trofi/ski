/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * EM instruction header
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
#ifndef _SKI_EMINST_H
#define _SKI_EMINST_H

/* Flags */
#define EM_FLAG_PRED				0x0001
#define EM_FLAG_PRIVILEGED			0x0002
#define EM_FLAG_LMEM				0x0004
#define EM_FLAG_SMEM		       		0x0008
#define EM_FLAG_CHECK_BASE_EQ_DST		0x0010
#define EM_FLAG_FIRST_IN_INSTRUCTION_GROUP	0x0020
#define EM_FLAG_LAST_IN_INSTRUCTION_GROUP	0x0040
#define EM_FLAG_CHECK_SAME_DSTS			0x0080
#define EM_FLAG_SLOT2_ONLY			0x0100
#define EM_FLAG_TWO_SLOT			0x0200
#define EM_FLAG_OK_IN_MLI			0x0400
#define EM_FLAG_CHECK_EVEN_ODD_FREGS		0x0800
#define EM_FLAG_CTYPE_UNC			0x1000

#define EM_FLAG_GR0_DEST			0x80000000
#define EM_FLAG_FR0_FR1_DEST			0x40000000
#define EM_FLAG_BAD_APP_REG_GRP			0x20000000
#define EM_FLAG_BAD_COUNT			0x10000000
#define EM_FLAG_BAD_ALLOC			0x08000000
#define EM_FLAG_BAD_MBTYPE			0x04000000

/* Architecture Revisions and Implementations */
#define ArchRev0				0x0
#define ArchRev1				0x1
#define Impl_Ipref				0x100
#define Impl_Brl				0x200
#define Impl_Ifetch				0x400
#define Impl_AtomicOps				0x800
#define Impl_VM					0x10000

#include "decoder/decoder.h"
#include "decoder/operands.gen.h"
#include "encoder/EMformats.gen.h"

typedef unsigned long long UInt64;
typedef char Major_opcode;

#define OPCLASS(op)		((op) & ~0x1f)

typedef struct EMDB_info_s {
	Unit		unit;
	Operand_type	op[MAX_OPERANDS];
	unsigned char	num_dests;
	Flags		flags;
	Flags		impl;
	Format		format;
	UInt64		base_encoding;
} EMDB_info;

#define MF 255

#define FORMAT_SIZE     16

extern EMDB_info InstTable[];
extern char formatptr[][FORMAT_SIZE];

typedef struct {
	unsigned char pos;	/* zero-based bit position of right end */
	unsigned char len;	/* length */
} layout;

typedef struct EMDB_encode_s {
	layout op_layout[MAX_OPERANDS];
	char *genLayoutStr;
	char *fmtArrayptr;
} EMDB_encode;

extern EMDB_encode EncodingTable[];

#define EM_PREDICATE_POS	0
#define EM_PREDICATE_LEN	6

#endif /* _SKI_EMINST_H */
