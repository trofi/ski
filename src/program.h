/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Program Window Structure
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
#ifndef _SKI_PROGRAM_H
#define _SKI_PROGRAM_H

#include "std.h"
#include "types.h"

#ifndef XSIM
typedef int Widget;
#define WIDGET_TYPEDEF
#endif

typedef enum { PrgUseCVA, PrgUseTop, PrgUseBot } PrgCmd;

typedef struct {
    BOOL iofsHex;
    unsigned iofsSymWidth;
    BOOL instHex;
    unsigned instSymWidth;
    BOOL cache;
    BOOL useCbit;
    BOOL virtual;
    BOOL useEbit;
    BOOL bigEndian;
    unsigned overlap;
    unsigned columns;
    /* XXX - are these two needed? */
    BOOL oldpswc;
    struct {
	ADDR currVA, prevTopAddr;
	ADDR topAddr, botAddr;
	unsigned nbndl;
	PrgCmd cmd;
	unsigned rows;
	BOOL is;	/* which instruction set */
	BYTE iAmode;	/* which iA execution mode */
	Widget form, hdr, text, gotoST, gotoSL, gotoOT;
    } perProc[NPROC];
    Widget iofsFmt, instFmt;
} prgStruct;

extern prgStruct prgInfo;

#define prgRows		(prgInfo.perProc[cproc].rows)

#define prgBndl		(prgInfo.perProc[cproc].nbndl)

#define prgCmd		(prgInfo.perProc[cproc].cmd)
#define prgColumns	(prgInfo.columns)

#define prgwCVA		(prgInfo.perProc[cproc].currVA)
#define prgwPrevTop	(prgInfo.perProc[cproc].prevTopAddr)

#define prgwTop		(prgInfo.perProc[cproc].topAddr)
#define prgwBot		(prgInfo.perProc[cproc].botAddr)

#define prgwIS		(prgInfo.perProc[cproc].is)
#define prgwIAmode	(prgInfo.perProc[cproc].iAmode)

#define prgOverlap	1

void setPrgwIP(ADDR);
char *prgwLine(ADDR ofs, unsigned *srcrows, unsigned *asmrows);
char *prgwLineIA(ADDR adr, BYTE iAmode, BOOL bp_ip, unsigned *nbytes);

#endif 	/* _SKI_PROGRAM_H */
