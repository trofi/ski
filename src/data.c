/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Data Window Manipulation Routines
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

#include <string.h>
#include <ctype.h>
#include "std.h"
#include "bits.h"
#include "types.h"
/*typedef struct instr *InstrPtr;*/
#include "decoder/decoder.h"
#include "ssDCmd.h"
#include "exportui.h"
#include "coreui.h"
#include "data.h"
#include "libsym.h"
#include "libcore.h"
#include "sim.h"
#include "fields.h"
#include "state.h"
#include "simmem.h"

#define PRCH(c)		(isprint(c) ? (c) : '.')

/*##################### Globals - Exports ##################################*/

datStruct datInfo;
BOOL asciimode = NO, datwInHex = YES;
#define BYT_P_WD 4
#define BYT_P_DW 8
    /* BYT_P_xx is defined in ssDefs.h.  Change this if an equivalent
       define is found elsewhere or if ssDefs.h is included here */

/*##################### Functions and Subroutines ##########################*/

/*--------------------------------------------------------------------------
 * dataWindow$Expression - Set an expression for the data window (default
 *  leave data window position alone).
 *--------------------------------------------------------------------------*/
BOOL datwExpr(unsigned argc, char *argv[])
{
    if (argc == 1)
	(void)strcpy(datExpr, argv[0]);
    else
	datExpr[0] = '\0';
    datwUpdate();
    return YES;
}

/*--------------------------------------------------------------------------
 * dataWindow$Forward - Scroll data window forward n lines (default
 *  one screenful with one line overlap).
 *--------------------------------------------------------------------------*/
BOOL datwFwd(unsigned argc, char *argv[])
{
    REG lines = datwSz("Data") - datOverlap;

    if (argc == 1 && !evalExpr(argv[0], DECEXP, &lines))
	return NO;
    datwLVA  = datwCVA;
    datwCVA += lines * datInfo.numDataWds * BYT_P_WD;
    (void)datwMakeActivew("Data");
    return YES;
}

/*--------------------------------------------------------------------------
 * dataWindow$Backward - Scroll data window backward n lines (default
 *  one screenful with one line overlap).
 *--------------------------------------------------------------------------*/
BOOL datwBkwd(unsigned argc, char *argv[])
{
    REG lines = datwSz("Data") - datOverlap;

    if (argc == 1 && !evalExpr(argv[0], DECEXP, &lines))
	return NO;
    datwLVA  = datwCVA;
    datwCVA -= lines * datInfo.numDataWds * BYT_P_WD;
    (void)datwMakeActivew("Data");
    return YES;
}

/*--------------------------------------------------------------------------
 * dataWindow$Jump - Prompt for address and then draw data window starting
 *  at given address.  Stackmode is turned off.
 *--------------------------------------------------------------------------*/
BOOL datwJump(unsigned argc, char *argv[])
{
    ADDR ofs;

    if (argc == 1) {
	if (!evalExpr(argv[0], HEXEXP, &ofs))
	    return NO;
#if 0
/* Enable dwdalign when and if ssDefs.h is included */
	dwdalign(ofs);
#else
	ofs &= ~7;
#endif
	datwLVA = datwCVA;
	datwCVA = ofs;
	datwUpdate();
    } else {
	ADDR temp = datwLVA;

	datwLVA = datwCVA;
	datwCVA = temp;
	datwUpdate();
    }
    return YES;
}

/*--------------------------------------------------------------------------
 * dataWindow$HexMode - Set the data window display mode to Hex.
 *--------------------------------------------------------------------------*/
BOOL datwHex(unsigned argc, char *argv[])
{
    (void)datwMakeActivew("Data");
    datwInHex = YES;
    datwUpdate();
    return YES;
}

/*--------------------------------------------------------------------------
 * dataWindow$BundleMode - Set the data window display mode to Instruction
 *  bundles.
 *--------------------------------------------------------------------------*/
BOOL datwBndl(unsigned argc, char *argv[])
{
    (void)datwMakeActivew("Data");
    datwInHex = NO;
    datwUpdate();
    return YES;
}

/*--------------------------------------------------------------------------
 * displayMemoryBuffer - Return a null-terminated buffer containing "nlines"
 *  of memory data pointed at by datwCVA.
 *--------------------------------------------------------------------------*/
char *dmembuf(unsigned nlines)
{
    unsigned i;
    static char buf[2048];
    char *p;
    ADDR adr = datwCVA;
    unsigned ndbpl = BYT_P_WD*datInfo.numDataWds;

    p = buf;
    *p = '\0';
    for (i = 0; i < nlines; i++, adr += ndbpl)
	p += sprintf(p, "%s\n", datwLine(adr));
    return buf;
}

/*--------------------------------------------------------------------------
 * dataWindow$Line - Returns a newline-terminated string containing the
 *  words at the passed address.
 *--------------------------------------------------------------------------*/
char *datwLine(ADDR ofs)
{
    unsigned nddwpl = datInfo.numDataWds/2;
    char *p, buf[256], label[32];
    static char line[256];

    symAddrtoName(ofs, label, 0, NO, 16);
    buf[0] = '\0';
    p = buf;

    if (datwInHex) {
	char *q, ascii[80];
	unsigned i;
	DWORD dw;
	BYTE *by;

	ascii[0] = '\0';
	q = ascii;
	for (i = 0; i < nddwpl; i++, ofs += BYT_P_DW) {
	    if (memMRd(ofs, 8, &dw)) {
		p += sprintf(p, "%016llx", dw);
		dw = FIX_BYTEORDER_D(PSR_BE && !PSR_IS, dw);
		by = (BYTE *)(&dw);
		q += sprintf(q, "%c%c%c%c%c%c%c%c",
				PRCH(*by),     PRCH(*(by+1)),
				PRCH(*(by+2)), PRCH(*(by+3)),
				PRCH(*(by+4)), PRCH(*(by+5)),
				PRCH(*(by+6)), PRCH(*(by+7)));
	    } else
		p += sprintf(p, "xxxxxxxxxxxxxxxx");
	    p += sprintf(p, " ");
	}
	(void)strcat(buf, ascii);

#if 0
/* no little-endian support just yet */

	/* If PswE = 0, line = label + " " + buf */
	/* If PswE = 1, line = string reverse buf + " " + label */
	if (PswE()) {
	    line[0] = '\0';
	    do {
		p -= 9;
		strncat(line, p, 9);
	    } while (p > buf);
	    (void)strcat(line, label);
	} else
	    (void)sprintf(line, "%s %s", label, buf);
#endif
    } else {
	unsigned i;
	Bundle bndl;
	BYTE templ;
	DWORD slot[3];

	for (i = 0; i < nddwpl/2; i++, ofs += 0x10) {
	    if (memMIRd(ofs, (DWORD *)&bndl)) {
		bparts(&bndl, &templ, slot);
		p += sprintf(p, "%02x %011llx %011llx %011llx ",
			     templ, slot[0], slot[1], slot[2]);
	    } else
		p += sprintf(p, "xx xxxxxxxxxxx xxxxxxxxxxx xxxxxxxxxxx ");
	}
    }
    (void)sprintf(line, "%s %s", label, buf);
    return line;
}

/*--------------------------------------------------------------------------
 * data$Dump - Print contents of memory to specified file (or PAGER) starting
 *   at start address and ending at end address.
 *--------------------------------------------------------------------------*/
BOOL dataDump(unsigned argc, char *argv[])
{
    ADDR sadr, eadr, adr;
    FILE *f = NULL;
    char *p, *buf;
    size_t bufsz;
    unsigned ndbpl = 4*datInfo.numDataWds;

    if (!evalExpr(argv[0], HEXEXP, &sadr))
	return NO;
    if (!evalExpr(argv[1], HEXEXP, &eadr))
	return NO;
    /* Align addresses to 8-byte boundaries */
    sadr &= ~(ADDR)0x7;
    eadr &= ~(ADDR)0x7;
    if (sadr > eadr) {
	cmdErr("Starting address > ending address\n");
	return NO;
    }
    bufsz = 80 * ((eadr - sadr + ndbpl)/ndbpl);
    if (!(p = buf = malloc(bufsz+1))) {
	cmdErr("Could not malloc space for data dump\n");
	return NO;
    }
    if (argc == 3 && !(f = cmdOpenFile(argv[2], "w"))) {
	free(buf);
	return NO;
    }
    buf[0] = '\0';
    for (adr = sadr; adr <= eadr; adr += ndbpl)
	p += sprintf(p, "%s\n", datwLine(adr));
    cmdOut("datadump", "", buf, f);
    free(buf);
    return YES;
}
