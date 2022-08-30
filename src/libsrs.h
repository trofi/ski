/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Save/Restore Header
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
#ifndef _SKI_LIBSRS_H
#define _SKI_LIBSRS_H

#include <stdio.h>

#include "std.h"

typedef enum { ERROR, FOUND, NOT_FOUND } RstStat;

/*
 * Routines to invoke the Save/Restore functionality
 */

BOOL srs_saveState(char *fname);
BOOL srs_restoreState(char *fname);


/* Utility functions */

BOOL srs_nextRstVal(FILE *f, const char *format, ...);
char *srs_errmsgGet(void);
void srs_errmsgSet(const char *fmt, ...);


/*
 * Simulator- and implementation-dependent routines called by the
 * Save/Restore functionality
 */

/* Functions for saving state */

BOOL saveProcImpl(FILE *f, unsigned cproc);
BOOL saveSysImpl(FILE *f);
BOOL saveSimState(FILE *f);


/* Functions for restoring state */

RstStat restoreProcImpl(FILE *f, char *name, unsigned cproc);
RstStat restoreSysImpl(FILE *f, char *name);
RstStat restoreSimState(FILE *f, char *name);

#endif	/* _SKI_LIBSRS_H */
