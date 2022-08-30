/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Trace UI Routines
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

#include <fcntl.h>	/* for fcntl */

#include "traceui.h"

#include "std.h"
#include "types.h"
#include "state.h"
#include "ssDCmd.h"
#include "libtrace.h"
#include "exportui.h"
#include "coreui.h"
#include "libcore.h"
#include "ssDPrs.h"
#include "ui.h"

/*##################### Functions and Subroutines ##########################*/

/*--------------------------------------------------------------------------
 * trace$Generate - Enable/disable trace generation depending on the
 *  expression given in argv[0].
 *--------------------------------------------------------------------------*/
BOOL traceGen(unsigned argc, char *argv[])
{
    REG expr;

    if (!evalExpr(argv[0], HEXEXP, &expr))
	return NO;
    if (!setTraceEnb(!!expr)) {
	cmdWarn("No trace file specified yet\n");
	return NO;
    }
    return YES;
}

/*--------------------------------------------------------------------------
 * trace$File - Open the file specified by argv[0] for trace output.  argv[1]
 *  indicates whether the trace overwrites ("w") or appends to ("a") the file.
 *  The default is to overwrite the trace file if the second argument is not
 *  present.  The function closes any previously opened trace file
 *--------------------------------------------------------------------------*/
BOOL traceFile(unsigned argc, char *argv[])
{
    static char tmode[] = { '\0' };
    CTR ti = getTotalInsts();

    if (argc > 1) {
	tmode[0] = argv[1][0];
	if (argv[1][1] || (tmode[0] != 'w' && tmode[0] != 'a')) {
	    cmdErr("Illegal output file specification: %s\n", argv[1]);
	    return NO;
	}
    } else
	tmode[0] = 'w';
    if (!traceOutInit(argv[0], tmode, ti, getSimID()))
	return NO;
    initTraceVcy();
    initTraceFlags();
    return YES;
}
