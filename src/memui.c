/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Memory UI Code
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


#include "std.h"
#include "bits.h"
#include "types.h"
#include "fields.h"
#include "state.h"
BOOL dtlbMLookup(ADDR va, int at, unsigned pl, BOOL virtual, ADDR *pa);
BOOL itlbMLookup(ADDR va, BOOL virtual, ADDR *pa);

BOOL dataTranslate(ADDR va, BOOL rd, ADDR *pa)
{
    int at = rd ? 0 : 2;

    if (abi) {
	*pa = va;
	return YES;
    }
    return dtlbMLookup(va, at, PSR_CPL, PSR_DT, pa);
}

BOOL instTranslate(ADDR va, ADDR *pa)
{
    if (abi) {
	*pa = va;
	return YES;
    }
    return itlbMLookup(va, PSR_IT, pa);
}
