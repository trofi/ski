/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Program Window Manipulation Routines
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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "std.h"
#include "bits.h"
#include "types.h"
#include "fields.h"
#include "sim.h"
#include "ssDCmd.h"
#include "coreui.h"
#include "program.h"
#include "decoder/decoder.h"
#include "libcore.h"
#include "libdas.h"
#include "libsym.h"
#include "iadas_types.gen.h"	/* for IAinstInfo/IAinstInfoPtr */
#include "ia_decoder.h"
#include "ia_exec.h"
#include "state.h"
#include "simmem.h"
#include "ssDBT.h"
#include "ssDPrs.h"
#include "syscall_api.h"

static char instPtr(ADDR addr, char *dline);
static IAinstInfoPtr addrToIAinfo(ADDR adr, BYTE iAmode, unsigned *nbytes);

extern BOOL iadas_decode(ADDR4, IAinstInfoPtr);


/*##################### Globals - Exports ##################################*/

prgStruct prgInfo = {YES, 13, NO, 45};
PrgwMode prgwm = MIXED;

/*##################### Functions and Subroutines ##########################*/

static ADDR dasAddr;

static void prgwForwardN(int n)
{
    prgwPrevTop = prgwTop;
    if (prgwIS) {	/* iA */
	prgwTop = iAinstAddr(prgwTop, prgwIAmode, n);
	prgCmd = PrgUseTop;
    } else {		/* EM */
	prgwBot += n*16;
	prgCmd = PrgUseBot;
    }
    prgwDraw();
}

static void prgwForwardScreen(void)
{
    prgwPrevTop = prgwTop;
    if (prgwIS)		/* iA */
	prgwTop = iAinstAddr(prgwTop, prgwIAmode, prgBndl - prgOverlap);
    else		/* EM */
	prgwTop = prgwBot + 16 - prgOverlap*16;
    prgCmd = PrgUseTop;
    prgwDraw();
}

/*--------------------------------------------------------------------------
 * programWindow$Forward - Scroll program window forward n bundles (default
 *  one screenful with one bundle overlap).
 *--------------------------------------------------------------------------*/
BOOL prgwFwd(unsigned argc, char *argv[])
{
    if (argc == 1) {	/* get # of bundles to scroll forward */
	REG bndls;

	if (!evalExpr(argv[0], DECEXP, &bndls))
	    return NO;
	prgwForwardN((int)bndls);
    } else		/* forward 1 screen */
	prgwForwardScreen();
    return YES;
}

static void prgwBackN(int n)
{
    prgwPrevTop = prgwTop;
    if (prgwIS)		/* iA */
	prgwTop = iAinstAddr(prgwTop, prgwIAmode, -n);
    else		/* EM */
	prgwTop -= n*16;
    prgCmd = PrgUseTop;
    prgwDraw();
}

static void prgwBackScreen(void)
{
    prgwPrevTop = prgwTop;
    if (prgwIS) {	/* iA */
	prgwTop = iAinstAddr(prgwTop, prgwIAmode,
			     -(int)(prgBndl - prgOverlap));
	prgCmd = PrgUseTop;
    } else {		/* EM */
	prgwBot = prgwTop - 16 + prgOverlap*16;
	prgCmd = PrgUseBot;
    }
    prgwDraw();
}

/*--------------------------------------------------------------------------
 * programWindow$Backward - Scroll program window backward n bundles (default
 *  one screenful with one bundle overlap).
 *--------------------------------------------------------------------------*/
BOOL prgwBkwd(unsigned argc, char *argv[])
{
    if (argc == 1) {	/* get # of bundles to scroll backward */
	REG bndls;

	if (!evalExpr(argv[0], DECEXP, &bndls))
	    return NO;
	prgwBackN((int)bndls);
    } else		/* backward 1 screen */
	prgwBackScreen();
    return YES;
}

/*--------------------------------------------------------------------------
 * programWindow$Jump - Prompt for address and then draw program window
 *  starting at given address.
 *--------------------------------------------------------------------------*/
BOOL prgwJump(unsigned argc, char *argv[])
{
    ADDR ofs;

    if (argc == 1) {		/* jump to specified addr */
	if (!evalExpr(argv[0], HEXEXP, &ofs))
	    return NO;
	prgwPrevTop = prgwTop;
	if (prgwIS)		/* iA */
	    prgwTop = iAinstAddr(ofs, prgwIAmode, -prgOverlap);
	else			/* EM */
	    prgwTop = (ofs & ~0xF) - prgOverlap*16;
	prgCmd = PrgUseTop;
	prgwDraw();
    } else {			/* jump to previous context */
	ADDR temp = prgwPrevTop;

	prgwPrevTop = prgwTop;
	prgwTop = temp;
	prgCmd = PrgUseTop;
	prgwDraw();
    }
    return YES;
}

/*--------------------------------------------------------------------------
 * programWindow$Assembly - Set the program window mode to display the
 *  executable in assembly only.
 *--------------------------------------------------------------------------*/
BOOL prgwAsm(unsigned argc, char *argv[])
{
    prgwm = ASM;
    prgwDraw();
    return YES;
}

/*--------------------------------------------------------------------------
 * programWindow$Mixed - Set the program window mode to display the
 *  executable in assembly and source intermixed.
 *--------------------------------------------------------------------------*/
BOOL prgwMixed(unsigned argc, char *argv[])
{
    prgwm = MIXED;
    prgwDraw();
    return YES;
}

/*--------------------------------------------------------------------------
 * instruction$Pointer - The routine returns the character to prefix the
 *  instruction line with:
 *	>	addr == ip && qp == YES
 *	*	addr == ip && qp == NO
 *	<sp>	addr != ip && qp == YES
 *	!	addr != ip && qp == NO
 *--------------------------------------------------------------------------*/
static char instPtr(ADDR addr, char *dline)
{
    BOOL pred = YES;

    if (*dline == '(') {
	int qp = atoi(dline+2);

	pred = prGet(viewPid, qp);
    }
    return ((ipGet(viewPid) == addr)
		? pred ? '>' : '*'
		: pred ? ' ' : '!');
}

/*--------------------------------------------------------------------------
 * programWindow$Line - Takes the contents of the passed address and
 *  interprets it as a bundle.  Returns a newline-terminated string
 *  describing the bundle and sets the number of screen rows needed.
 *--------------------------------------------------------------------------*/
char *prgwLine(ADDR ofs, unsigned *srcrows, unsigned *asmrows)
{
    Bundle bndl;
    char bpn, ipp, buf[20], *srcp;
    int i;
    static char line[400];

    symAddrtoName(ofs, buf, 4, NO, 16);
    bpn = ((i = isbpt(ofs)) >= 0) ? (i + '0') : ' ';
    if (memMIRd(ofs, (DWORD *)&bndl)) {
	char *p = line, *i0Str, *i1Str, *i2Str;
	/* XXX - memory leak, these are never freed */
	i0Str = malloc(prgColumns - 19);	/* + 1 for NUL */
	i1Str = malloc(prgColumns - 19);
	i2Str = malloc(prgColumns - 19);
	if (!i0Str || !i1Str || !i2Str)
	    goto xxx;

	line[0] = '\0';
	srcp = getSrcLines(ofs, srcrows);
	dasAddr = ofs;
	/* XXX - only need to do this when it changes, so need to call a new
		 special function (or dasInit directly) from cur.c/ssXDSD.c
		 on resize?
	*/
	/* XXX - 20 = 3 (leading chars) + 16 (iaddr) + 1 (blank) */
	dasInit(DasPseudoOps|DasTemplate|DasRegNames, prgColumns - 20);
	dasBundle(&bndl, i0Str, i1Str, i2Str);
	ipp = instPtr(ofs, i0Str);
	p += sprintf(p, "%s%c%c %s %s\r\n", srcp, bpn, ipp, buf, i0Str);
	if (i1Str[0]) {		/* not MLX */
	    bpn = ((i = isbpt(ofs + 4)) >= 0) ? (i + '0') : ' ';
	    ipp = instPtr(ofs + 4, i1Str);
	    p += sprintf(p, "%c%c %16s %s\r\n", bpn, ipp, "", i1Str);
	    bpn = ((i = isbpt(ofs + 8)) >= 0) ? (i + '0') : ' ';
	    ipp = instPtr(ofs + 8, i2Str);
	} else {		/* MLX */
	    bpn = ((i = isbpt(ofs + 4)) >= 0 || (i = isbpt(ofs + 8)) >= 0)
		    ? (i + '0') : ' ';
	    ipp = instPtr(ofs + 4, i2Str);
	}
	p += sprintf(p, "%c%c %16s %s\r\n", bpn, ipp, "", i2Str);
	*asmrows = i1Str[0] ? 3 : 2;
    } else {
xxx:
	ipp = (ipGet(viewPid) == ofs) ? '>' : ' ';
	(void)sprintf(line, "%c%c %s xxxxxxxx\r\n", bpn, ipp, buf);
	*srcrows = 0;
	*asmrows = 1;
    }
    return line;
}

#undef EIPfromIP
ADDR4 EIPfromIP(ADDR adr)
{
    REG tmpreg = grGet(viewPid, CSD_ID);
    IASegDesc csd;

    csd = *(IASegDescPtr)&tmpreg;
    return (ADDR4)adr - (ADDR4)csd.base;
}

static IAinstInfoPtr addrToIAinfo(ADDR adr, BYTE iAmode, unsigned *nbytes)
{
    static IAinstInfo instInfo;

    if (iadas_decode(EIPfromIP(adr), &instInfo)) {
	*nbytes = instInfo.delta;
	return &instInfo;
    } else {
	*nbytes = 1;
	return 0;
    }
}

/* return the address of the iA instruction "delta" instructions before
 * (delta < 0) or after (delta >= 0) baseAddr
 */
ADDR iAinstAddr(ADDR baseAddr, BYTE iAmode, int delta)
{
    ADDR adr, try;
    unsigned nbytes, count, tries;

    if (delta < 0)
	for (try = symAddrtoAddr(baseAddr + delta, -1000, 0, 0), tries = 0;
	     tries < 1000; tries++) {
	    for (adr = try, count = 0; adr < baseAddr;
		 count++, adr += nbytes)
		(void)addrToIAinfo(adr, iAmode, &nbytes);
	    if (adr == baseAddr && count >= -delta) {
		for (adr = try; count > -delta; count--, adr += nbytes)
		    (void)addrToIAinfo(adr, iAmode, &nbytes);
		break;
	    } else if (count >= -delta)
		try++;
	    else
		try = symAddrtoAddr(try - 1, -1000, 0, 0);
	}
    else
	for (adr = baseAddr; delta; delta--, adr += nbytes)
	    (void)addrToIAinfo(adr, iAmode, &nbytes);
    return adr;
}

char *prgwLineIA(ADDR adr, BYTE iAmode, BOOL bp_ip, unsigned *nbytes)
{
    IAinstInfoPtr info;
    char symAddr[20], iStr[80] = "xx", bpIpStr[4] = { '\0', '\0', ' ' };
    int i;
    static char line[100];

    symAddrtoName(adr, symAddr, 4, NO, 16);
    if ((info = addrToIAinfo(adr, iAmode, nbytes))) {
	dasAddr = EIPfromIP(adr);
	info->dasFn(info, iStr);
    }
    if (bp_ip) {
	REG ipval = ipGet(viewPid);

	bpIpStr[0] = ((i = isbpt(adr)) >= 0) ? (i + '0') : ' ';
	bpIpStr[1] = (ipval == adr) ? '>' :
	    (ipval > adr && ipval < adr + *nbytes) ? '+' : ' ';
    }
    (void)sprintf(line, "%s%s %s\n", bpIpStr, symAddr, iStr);
    return line;
}

void setPrgwIP(ADDR adr)
{
    prgwCVA = adr;
}

/*--------------------------------------------------------------------------
 * disassembly$Dump - Print disassembled contents of memory to specified file
 *  (or PAGER) starting at start address and ending at end address.
 *--------------------------------------------------------------------------*/
BOOL dasmDump(unsigned argc, char *argv[])
{
    ADDR sadr, eadr, adr;
    REG psrval = psrGet(viewPid);
    FILE *f = NULL;
    char *p, *buf, adrStr[20];
    size_t bufsz;
    unsigned inc = 0x10;
    Bundle bndl;

    if (!evalExpr(argv[0], HEXEXP, &sadr))
	return NO;
    if (!evalExpr(argv[1], HEXEXP, &eadr))
	return NO;
    if (!X_PSR_IS(psrval)) {	/* align EM addresses */
	sadr &= ~(ADDR)0xf;
	eadr &= ~(ADDR)0xf;
    }
    if (sadr > eadr) {
	cmdErr("Starting address > ending address\n");
	return NO;
    }
    bufsz = 80 * (eadr - sadr + 1);	/* XXX - worst case 1 line/byte */
    if (!(p = buf = malloc(bufsz+1))) {
	cmdErr("Could not malloc space for program dump\n");
	return NO;
    }
    if (argc == 3 && !(f = cmdOpenFile(argv[2], "w"))) {
	free(buf);
	return NO;
    }
    buf[0] = '\0';
    for (adr = sadr; adr <= eadr; adr += inc) {
	if (X_PSR_IS(psrval))	/* IA */
	    (void)strcat(p, prgwLineIA(adr, getIAmode(), NO, &inc));
	else {		/* EM */
/* XXX - why not call prgwLine here? */
	    symAddrtoName(adr, adrStr, 4, NO, 16);
	    if (memMIRd(adr, (DWORD *)&bndl)) {
		char i0Str[80], i1Str[80], i2Str[80];

		dasAddr = adr;
		dasBundle(&bndl, i0Str, i1Str, i2Str);
		p += sprintf(p, "%s %s\n", adrStr, i0Str);
		if (i1Str[0])
		    p += sprintf(p, "%16s %s\n", "", i1Str);
		p += sprintf(p, "%16s %s\n", "", i2Str);
	    } else
		p += sprintf(p, "%s xxxxxxxx\n", adrStr);
	}
    }
    cmdOut("progdump", "", buf, f);
    free(buf);
    return YES;
}

ADDR dasAddress(void)
{
    return dasAddr;
}

/* ski doesn't support relocations */
unsigned dasRelocation(unsigned long long addr, DasRelocationPtr reloc)
{
    return 0;
}
