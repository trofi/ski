/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Implementation-Dependent Routines
 * (This file might need to be split into a processor and a platform
 *  implementation-dependent file.)
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

#include "impl.h"

#include "std.h"
#include "types.h"
#include "libsrs.h"

const char *getImplStr(void)
{
    return "SDM 2.1";
}

/*
 * Save/Restore Routines
 */

BOOL saveProcImpl(FILE *f, unsigned cproc)
{
    fprintf(f, "# No processor implementation-dependent state saved\n");
    return YES;
}

BOOL saveSysImpl(FILE *f)
{
    fprintf(f, "# No system implementation-dependent state saved\n");
    return YES;
}

RstStat restoreProcImpl(FILE *f, char *name, unsigned cproc)
{
    return NOT_FOUND;
}

RstStat restoreSysImpl(FILE *f, char *name)
{
    return NOT_FOUND;
}
