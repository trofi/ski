/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Instruction Info Structure Definitions
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
#ifndef _SKI_INSTINFO_H
#define _SKI_INSTINFO_H

#define imm64	info->immed64
#define imm62	info->immed64

#define qpred   extrainfo[0]
#define qp	info->extrainfo[0]
#define bp	info->extrainfo[0]

#define r1	info->extrainfo[1]
#define f1	info->extrainfo[1]
#define p1	info->extrainfo[1]
#define b1	info->extrainfo[1]

#define r2	info->extrainfo[2]
#define f2	info->extrainfo[2]
#define b2	info->extrainfo[2]
#define il	info->extrainfo[2]

#define r3	info->extrainfo[3]
#define f3	info->extrainfo[3]
#define ar3	info->extrainfo[3]
#define o	info->extrainfo[3]

#define f4	info->extrainfo[4]
#define p2	info->extrainfo[4]
#define cr3	info->extrainfo[4]
#define r	info->extrainfo[4]

#define imm1	info->immed64
#define imm2	info->immed64
#define imm7	info->immed64
#define imm8	info->immed64
#define imm9	info->immed64
#define imm14	info->immed64
#define imm21	info->immed64
#define imm22	info->immed64
#define imm24	info->immed64
#define imm44	info->immed64
#define inc3	info->immed64
#define mask17	info->immed64
#define fclass9	info->immed64
#define target25	info->immed64
#define target64	info->immed64

#define tag13	*((int*)&info->extrainfo[4])

#define mbtype4	info->extrainfo[5]
#define mhtype8	info->extrainfo[5]
#define len4	info->extrainfo[5]
#define len6	info->extrainfo[5]
#define count2	info->extrainfo[5]
#define count5	info->extrainfo[5]
#define count6	info->extrainfo[5]
#define amask7	info->extrainfo[5]

#define pos6	info->extrainfo[6]
#define omask7	info->extrainfo[6]

#define reserved	255
#define reservedQP	255
#define _brcst		0
#define _mix		8
#define _shuf		9
#define _alt		0xa
#define _rev		0xb

#endif	/* _SKI_INSTINFO_H */
