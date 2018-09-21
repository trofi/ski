/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Data Window Structure
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
#ifndef _SKI_DATA_H
#define _SKI_DATA_H

#ifndef XSIM
#ifndef WIDGET_TYPEDEF
typedef int Widget;
#endif
#define WINTBLSIZ 20	/* XXX - copied from coreui.h */
#endif

typedef struct {
    BOOL cache;
    BOOL useDbit;
    BOOL virtual;
    BOOL useEbit;
    BOOL bigEndian;
    unsigned overlap;
    unsigned numDataWds;
    unsigned columns;
    char expr[40];
    struct {
	ADDR currVA, lastVA;
	unsigned rows;
	/* XXX - will not work when NPROC > 1 !!! */
	Widget form[WINTBLSIZ], hdr[WINTBLSIZ], text[WINTBLSIZ];
    } perProc[NPROC];
} datStruct;

extern datStruct datInfo;

#define datRows		(datInfo.perProc[cproc].rows)
#define datColumns	(datInfo.columns)
#define datExpr		(datInfo.expr)
#define datwCVA		(datInfo.perProc[cproc].currVA)
#define datwLVA		(datInfo.perProc[cproc].lastVA)
#define datOverlap	1

char *datwLine(ADDR ofs);

#endif	/* _SKI_DATA_H */
