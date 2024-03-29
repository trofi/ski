/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Instruction Structure Definition
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
#ifndef _SKI_INSTR_H
#define _SKI_INSTR_H

#include "sim.h"
#include "std.h"

typedef BOOL (*PdecFn)(DWORD, INSTINFO *);

typedef struct instr {
    PdecFn pdecFn;
    PCF combFn;
    unsigned int flags;
    ICNTS cnt;
} Instr, *InstrPtr;

extern InstrPtr instrs; /* TODO: should be const? */

#endif /* _SKI_INSTR_H */
