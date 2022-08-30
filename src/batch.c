/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Batch Interface Routines
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

#include "batch.h"

#include "std.h"
#include "types.h"
#include "ski.h"
#include "ssDCmd.h"	/* just for exitSim() */
#include "libcore.h"
#define cproc 0
#include "data.h"

BOOL bstats = NO;

void scrnInitBatch(void)
{
    datInfo.numDataWds = 4;
}

void cmdwPrintBatch(const char *s)
{
    (void)fputs(s, stderr);
}

void cmdOutBatch(const char *hdr, const char *buf)
{
    (void)fputs(hdr, stderr);
    (void)fputs(buf, stderr);
}

void cmdLoopBatch(void)
{
    if (fileLoaded && !getExited())
	runIt(bstats);
    else
	(void)fprintf(stderr, "Nothing to run\n");
    exitSim(grGet(0, 8));
}
