/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 *
 * Simulator UI Header
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
#ifndef _SKI_SKI_H
#define _SKI_SKI_H

#include "std.h"

#define GR_STK_BASE	32
#define NATVAL(s,e,m)	(!s && e == 0x1FFFE && !m)

extern BOOL fileLoaded;
extern BOOL keepXLoopAlive;

#define IFACELEN 4
#define ARGSIZ 30

typedef enum {
    ARG_BOOL, ARG_INT4, ARG_INT8, ARG_STRING
} ARG;

BOOL argIns(char *name, void *var, ARG kind, char *iface, char *descr);
/*
    char *name		argument (cmd line option) name.  Must include "-"
    void *var		pointer to variable controlling the arg functionality
    ARG kind		variable type
    char *iface		interface for which the argument is valid: "bcx" makes
			an option valid for batch, curses, and X interfaces
    char *descr		argument description
    (retval)		was the cmd line option inserted in the table?
 */

void displayOptions(void);
int parseOptions(int argc, char *argv[]);

void runIt(BOOL showIrate);
BOOL runProg(unsigned argc, char *argv[]);
BOOL stepProg(unsigned argc, char *argv[]);

BOOL stepUntil_loopX(CTR delay, REG val, const char *expr);

#endif	/* _SKI_SKI_H */
