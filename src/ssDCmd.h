/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Debugger Command Interpreter Constants and Prototypes
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
#ifndef _SKI_SSDCMD_H
#define _SKI_SSDCMD_H

#include <stdio.h>

#include "std.h"

#define MAXCMDS	10
#define MAXARGS	64

#define DECEXP	10
#define HEXEXP	16

#ifdef NEW_MP
extern unsigned viewPid;
#else
#define viewPid 0
#endif

/* XXX - the following declarations probably belong in state.h */

extern unsigned cproc;              /* current processor number */
extern unsigned nproc;              /* user-specified number of processors */

/***/

/*
 * Prototypes for the supporting functions used for command execution.
 */

void cmdOut(const char *name, const char *hdr, const char *buf, FILE *f);

/* ssDCmd.c */
BOOL cmdExLin(const char *cmdlin);
BOOL readCmdFile(char *fname);
FILE *cmdOpenFile(char *name, char *mode);
void exitSim(int exitval);

/* ssDPrs.c */
void catArgs(unsigned start, unsigned cnt, char *argv[], char *buf);

/* menu.c */
BOOL stateRest(unsigned argc, char *argv[]);

BOOL cmdSysEsc(unsigned argc, char *argv[]);
BOOL cmdComment(unsigned argc, char *argv[]);
BOOL cmdEval(unsigned argc, char *argv[]);

BOOL cmdExit(unsigned argc, char *argv[]);
BOOL cmdSource(unsigned argc, char *argv[]);
BOOL cmdGoto(unsigned argc, char *argv[]);

BOOL cmdLoad(unsigned argc, char *argv[]);
BOOL cmdIALoad(unsigned argc, char *argv[]);
BOOL cmdROMLoad(unsigned argc, char *argv[]);
BOOL cmdFWLoad(unsigned argc, char *argv[]);
BOOL cmdIf(unsigned argc, char *argv[]);

#endif	/* _SKI_SSDCMD_H */
