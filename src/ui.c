/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator User Interface Routines
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
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include "ui.h"

#include "std.h"
#include "batch.h"
#include "bits.h"
#include "types.h"
#include "fields.h"
#include "impl.h"
#include "ski.h"
#include "sim.h"
#include "ssDCmd.h"
#include "exportui.h"
#include "coreui.h"
#include "program.h"
#include "data.h"
#include "libsym.h"
#include "libcore.h"
#include "iadas_types.gen.h"	/* for ADDR4 */

BYTE getIAmode(void);
#undef EIPfromIP
ADDR4 EIPfromIP(ADDR adr);	/* in program.c */

static char *urLine(unsigned i);
static char *grLine(unsigned i);
static char *frLine(unsigned i);
static char *srLine(unsigned i);
static char *iarLine(unsigned i);

Regw regwtbl[WINTBLSIZ] = {
    { "Urs",  "User Registers",           urLine,  0, "18", YES },
    { "Grs",  "General Registers",        grLine,  0, "((32+sof+3)/4)", YES },
    { "Frs",  "Floating-point Registers", frLine,  0, "64", YES },
    { "Srs",  "System Registers",         srLine,  0, "67", YES },
    { "IArs", "IA Registers",             iarLine, 0, "14",  YES },
    { "", "", 0, 0, "", NO }
};

Datw datwtbl[WINTBLSIZ] = {
    { "Data", "Data",    0, dmembuf,  YES },
    { "", "", 0, 0, NO }
};

Regw *actRegw = &regwtbl[1];		/* default is GRs */
Datw *actDatw = &datwtbl[0];		/* default is to display memory */

unsigned topregw, topdatw;

static void regwTblInit(void);
static void datwTblInit(void);

/*##################### Functions and Subroutines ##########################*/

void uiInit(void)
{
    isymInit();
    menuInit();
    regwTblInit();
    datwTblInit();
}

void cmdOut(const char *name, const char *hdr, const char *buf, FILE *f)
{
    if (f) {
	(void)fputs(hdr, f);
	(void)fputs(buf, f);
	(void)fclose(f);
    } else
	switch (interface) {
	    case BATCH:
		cmdOutBatch(hdr, buf);
		break;
	    case X_INTERFACE:
#ifdef HAVE_MOTIF
		cmdOutX(name, hdr, buf);
#endif
		break;
	    case CURSES_INTERFACE:
		cmdOutCur(hdr, buf);
		break;
            case GTK_INTERFACE:
#ifdef HAVE_GTK
		cmdOutGtk(name, hdr, buf);
#endif
		break;
	}
    /* free(buf); */
}

/****************************/
/* Register Window Routines */
/****************************/

/*--------------------------------------------------------------------------
 * registerWindow$Table$Initialize - Set the value of topregw, the current
 *  size of the register window table.  This value will be modified by the
 *  implementation-dependent routines that adds new reg windows to the table.
 *--------------------------------------------------------------------------*/
static void regwTblInit(void)
{
    Regw *pregw;

    topregw = 0;
    for (pregw = regwtbl; strcmp(pregw->tag, ""); pregw++)
	topregw++;
}

static BOOL regwMakeActivew(const char *tag)
{
    Regw *pregw = regwtbl;

    for (pregw = regwtbl; pregw < &regwtbl[topregw]; pregw++)
	if (!strcmp(pregw->tag, tag)) {
	    actRegw = pregw;
	    actRegw->show = !actRegw->show;	/* Toggle display.  For X */
	    regwUpdate();
	    return YES;
	}
    (void)fprintf(stderr, "Error: didn't find register window tag: %s\n", tag);
    return NO;
}

void regwUpdate(void)
{
    switch (interface) {
	case X_INTERFACE:
#ifdef HAVE_MOTIF
	    regwUpdateX();
#endif
	    break;
	case CURSES_INTERFACE:
	    regwUpdateCur();
	    break;
        case GTK_INTERFACE:
#ifdef HAVE_GTK
	    regwUpdateGtk();
#endif
	    break;
	case BATCH:
	    break;
    }
}

BOOL regwFwd(unsigned argc, char *argv[])
{
    switch (interface) {
	case BATCH:
	case X_INTERFACE:
	    return YES;		/* rf command is a NOP */
	case CURSES_INTERFACE:
	    return regwFwdCur(argc, argv);
    }
    return NO;
    /*NOTREACHED*/
}

BOOL regwBkwd(unsigned argc, char *argv[])
{
    switch (interface) {
	case BATCH:
	case X_INTERFACE:
	    return YES;		/* rb command is a NOP */
	case CURSES_INTERFACE:
	    return regwBkwdCur(argc, argv);
    }
    return NO;
    /*NOTREACHED*/
}

BOOL regwDump(unsigned argc, char *argv[])
{
    FILE *f = NULL;
    REG lines;
    unsigned i, j, tlines = 0;
    char *buf, *p;

    /* Compute buffer size and allocate it */
    for (i = 0; i < topregw; i++) {
	(void)evalExpr(regwtbl[i].size, DECEXP, &lines);
	tlines += lines + 1;
    }
    if (argc == 1 && !(f = cmdOpenFile(argv[0], "w")))
	return NO;
    if (!(p = buf = malloc(tlines * 80))) {
	cmdErr("Could not allocate buffer for register dump\n");
	(void)fclose(f);
	return NO;
    }
    *p = '\0';
    for (i = 0; i < topregw; i++) {
	(void)evalExpr(regwtbl[i].size, DECEXP, &lines);
	p += sprintf(p, "%s\n", regwtbl[i].title);
	for (j = 0; j < lines; j++)
	    p += sprintf(p, "%s\n", regwtbl[i].fcn(j));
    }
    cmdOut("regdump", "", buf, f);
    free(buf);
    return YES;
}

BOOL showUrs(unsigned argc, char *argv[])
{
    (void)regwMakeActivew("Urs");
    return YES;
}

BOOL showGrs(unsigned argc, char *argv[])
{
    (void)regwMakeActivew("Grs");
    return YES;
}

BOOL showFrs(unsigned argc, char *argv[])
{
    (void)regwMakeActivew("Frs");
    return YES;
}

BOOL showSrs(unsigned argc, char *argv[])
{
    (void)regwMakeActivew("Srs");
    return YES;
}

BOOL showIArs(unsigned argc, char *argv[])
{
    (void)regwMakeActivew("IArs");
    return YES;
}

/*
 * urLine - Return the ith line of the user registers
 */
static char *urLine(unsigned i)
{
    static char buf[81];
    char *p, tgtbuf[20];
    unsigned j;
    int sinx;
    REG br, curr_fm, psrval, ipval, pfsval, rscval, id[2];

    switch (i) {
	case 0:
	    psrval = psrGet(viewPid);
	    ipval = ipGet(viewPid);
	    symAddrtoName(X_PSR_IS(psrval) ? ipval : ipval & ~(ADDR)0xF,
			  tgtbuf, 4, NO, 16);
	    (void)sprintf(buf, "ip  %s  psr.um ", tgtbuf);
	    p = buf + strlen(buf);
	    p += sprintf(p, "%s|", X_PSR_MFH(psrval) ? "MFH" : "mfh");
	    p += sprintf(p, "%s|", X_PSR_MFL(psrval) ? "MFL" : "mfl");
	    p += sprintf(p, "%s|", X_PSR_AC(psrval) ? "AC" : "ac");
	    p += sprintf(p, "%s|", X_PSR_UP(psrval) ? "UP" : "up");
	    p += sprintf(p, "%s",  X_PSR_BE(psrval) ? "BE" : "be");
	    break;
	case 1:
	    (void)strcpy(buf, "prs");
	    p = buf + strlen(buf);
	    for (j = 0; j < /*NPRS*/64; j++) {
		if (!(j % 8))
		    p += sprintf(p, " ");
		p += sprintf(p, "%1d", (int)prGet(viewPid, j));
	    }
	    break;
	case 2:
	case 3:
	    (void)strcpy(buf, "");
	    p = buf;
	    i = (i - 2) * 4;
	    for (j = i; j < i+4; j++) {
		if (!(j%2))
		    p += sprintf(p, "b%d  ", j);
		br = brGet(viewPid, j);
		symAddrtoName(br, tgtbuf, 4, NO, 16);
		p += sprintf(p, "%s ", tgtbuf);
	    }
	    break;
	case 4:
	    (void)sprintf(buf, "%47s rrbp rrbf rrbg  sor  sol  sof", "");
	    break;
	case 5:
	    curr_fm = cfmGet(viewPid);
	    (void)sprintf(buf,
		"lc  %"RFT"  ec %02llx  bol %-4llu      cfm"
		"  %3u  %3u  %3u   %3u  %3u  %3u",
		arGet(viewPid, LC_ID), arGet(viewPid, EC_ID) & 0x3F,
		bolGet(viewPid),
		(unsigned) XFM_RRBP(curr_fm), (unsigned) XFM_RRBF(curr_fm),
		(unsigned) XFM_RRBG(curr_fm), (unsigned) XFM_SOR(curr_fm) << 3,
		(unsigned) XFM_SOIL(curr_fm), (unsigned) XFM_SOF(curr_fm));
	    break;
	case 6:
	    pfsval = arGet(viewPid, PFS_ID);
	    rscval = arGet(viewPid, RSC_ID);
	    (void)sprintf(buf,
		"rsc %04x %u %u %u       pec %02x  ppl %u         "
		"pfm  %3u  %3u  %3u   %3u  %3u  %3u",
		(unsigned) XRSC_LOADRS(rscval), (unsigned) XRSC_BE(rscval),
		(unsigned) XRSC_PL(rscval), (unsigned) XRSC_MODE(rscval),
		(unsigned) XFS_PEC(pfsval), (unsigned) XFS_PPL(pfsval),
		(unsigned) XFM_RRBP(pfsval), (unsigned) XFM_RRBF(pfsval),
		(unsigned) XFM_RRBG(pfsval), (unsigned) XFM_SOR(pfsval) << 3,
		(unsigned) XFM_SOIL(pfsval), (unsigned) XFM_SOF(pfsval));
	    break;
	case 7:
	    (void)sprintf(buf, "clean %-2d  cleanNaT %d  "
			       "dirty %-2d  dirtyNaT %d  "
			       "invalid %-2d  cfle %d",
				(int)cleanGet(viewPid),
				(int)cleanNatGet(viewPid),
				(int)dirtyGet(viewPid),
				(int)dirtyNatGet(viewPid),
				(int)invalidGet(viewPid), cfleGet(viewPid));
	    break;
	case 8:
	    (void)sprintf(buf, "bsp  %"RFT" bspst %"RFT,
			  arGet(viewPid, BSP_ID), arGet(viewPid, BSPST_ID));
	    break;
	case 9:
	    (void)sprintf(buf, "rnat %"RFT" unat  %"RFT,
			  arGet(viewPid, RNAT_ID), arGet(viewPid, UNAT_ID));
	    break;
	case 10:
	    (void)sprintf(buf, "fpsr %"RFT" itc   %"RFT" ccv  %"RFT,
			  arGet(viewPid, FPSR_ID), arGet(viewPid, ITC_ID),
			  arGet(viewPid, CCV_ID));
	    break;
	case 11:
	    (void)sprintf(buf, "k0   %"RFT" %"RFT" k2  %"RFT" %"RFT,
			  arGet(viewPid, K0_ID), arGet(viewPid, K1_ID),
			  arGet(viewPid, K2_ID), arGet(viewPid, K3_ID));
	    break;
	case 12:
	    (void)sprintf(buf, "k4   %"RFT" %"RFT" k6  %"RFT" %"RFT,
			  arGet(viewPid, K4_ID), arGet(viewPid, K5_ID),
			  arGet(viewPid, K6_ID), arGet(viewPid, K7_ID));
	    break;
	case 13:
	    (void)sprintf(buf, "eflags %"RFT" cflg %"RFT,
			  arGet(viewPid, EFLAGS_ID), arGet(viewPid, CFLG_ID));
	    break;
	case 14:
	    (void)sprintf(buf, "csd    %"RFT" ssd  %"RFT,
			  arGet(viewPid, CSD_AR_ID), arGet(viewPid, SSD_AR_ID));
	    break;
	case 15:
	    (void)sprintf(buf, "fsr    %"RFT" fir  %"RFT" fdr %"RFT,
			  arGet(viewPid, FSR_ID), arGet(viewPid, FIR_ID),
			  arGet(viewPid, FDR_ID));
	    break;
	case 16:
	    id[0] = cpuidGet(viewPid, 0);
	    id[1] = cpuidGet(viewPid, 1);
	    (void)sprintf(buf, "cpuid0 %"RFT" %"RFT" ", id[0], id[1]);
	    p = buf + strlen(buf);
	    for (j = 0; j < 2; j++)
		for (sinx = 7; sinx >= 0; sinx--) {
		    char vend = Bn(id[j],sinx);

		    p += sprintf(p, "%c", isprint((int)vend) ? vend : '.');
		}
	    break;
	case 17:
	    (void)sprintf(buf, "cpuid2 %"RFT" %"RFT" %"RFT,
			  cpuidGet(viewPid, 2), cpuidGet(viewPid, 3),
			  cpuidGet(viewPid, 4));
	    break;
    }
    return buf;
}

/*
 * grLine - Return the ith line of the GRs
 */
static char *grLine(unsigned i)
{
    static char buf[81];
    char *p = buf;
    static REG curr_fm;
    int j;

    buf[0] = '\0';
    p += sprintf(p, "r%-3u", i*4);
    if (!i)
	curr_fm = cfmGet(viewPid);
    for (j = 0; j < 4 && (i*4 + j) < (GR_STK_BASE+XFM_SOF(curr_fm)); j++) {
	REG val;
	BOOL nat;

	val = grGet(viewPid, i*4+j);
	nat = grNatGet(viewPid, i*4+j);
	p += sprintf(p, " %c%"RFT, nat ? '*' : ' ', val);
    }
    return buf;
}

/*
 * fp2string - convert a floating-point number to a string
 */
/* XXX - workaround compiler bug! */
#if (defined __hppa || defined __ia64) && !defined __GNUC__
#pragma OPTIMIZE OFF
#endif
static char *fp2string(BYTE sign, WORD exp, DWORD mant)
{
    union {
	DWORD dbl[2];
	long double ldbl;
    } u;
    static char retstr[20];

    if (NATVAL(sign, exp, mant))
	return "--NaTVal---";
    if (exp == 0x1FFFF) {
	if (SIGN(mant)) {
	    if (mant == 0x8000000000000000ULL)
		return sign ? "-- -Inf ---" : "-- +Inf ---";
	    return BitfX(mant,1,1) ? "---qNaN----" : "---sNaN----";
	}
	return "Unsupported";
    } else if (exp == 0x1003E) {
	(void)sprintf(retstr, "%11.4Le", (long double)mant);
	return retstr;
    }
#ifndef __linux__
    BitfR(u.dbl[0],0,1) = sign;
    if (!exp)
	exp = 0xC001;
    if (!mant && exp) {
	BitfR(u.dbl[0],1,63) = 0;
	u.dbl[1] = 0;
	(void)sprintf(retstr, "%11.4Le", u.ldbl);
	return retstr;
    }
    while (!BitfX(mant,0,1) && exp > 0) {
	mant <<= 1;
	exp--;
    }
    BitfR(u.dbl[0],1,15) = BitfX32(exp,15,17) - 0xFFFF + 0x3FFF;
    BitfR(u.dbl[0],16,48) = BitfX(mant,1,48);
    u.dbl[1] = BitfX(mant,49,15) << 49;
#else
    u.ldbl = mant;
    if (sign)
      u.ldbl = -u.ldbl;
    u.ldbl = ldexp (u.ldbl, exp - 63 - 65535);
#endif
    (void)sprintf(retstr, "%11.4Le", u.ldbl);
    return retstr;
}
#if (defined __hppa || defined __ia64) && !defined __GNUC__
#pragma OPTIMIZE ON
#endif

/*
 * frLine - Return the ith line of the FRs
 */
static char *frLine(unsigned i)
{
    static char buf[81];
    char *p;
    BYTE sign;
    WORD exp;
    DWORD mant;

    p = buf;
    sign = frSignGet(viewPid, i*2);
    exp = frExpGet(viewPid, i*2);
    mant = frMantGet(viewPid, i*2);
    p += sprintf(buf, "f%-3u %05x%"RFT" (%s)",
		 i*2, (WORD)sign<<17|exp, mant, fp2string(sign, exp, mant));
    sign = frSignGet(viewPid, i*2+1);
    exp = frExpGet(viewPid, i*2+1);
    mant = frMantGet(viewPid, i*2+1);
    (void)sprintf(p, "  %05x%"RFT" (%s)",
		  (WORD)sign<<17|exp, mant, fp2string(sign, exp, mant));
    return buf;
}

/*
 * srLine - Return the ith line of the system registers
 */
static char *srLine(unsigned i)
{
    static char buf[81];
    char str1[20], str2[20];

    switch (i) {
	case 0:
	    (void)sprintf(buf, "psr  %"RFT" ipsr %"RFT" dcr  %"RFT,
			  psrGet(viewPid), crGet(viewPid, IPSR_ID),
			  crGet(viewPid, DCR_ID));
	    return buf;
	case 1:
	    symAddrtoName(crGet(viewPid, IVA_ID), str1, 4, NO, 16);
	    symAddrtoName(crGet(viewPid, GPTA_ID), str2, 4, NO, 16);
	    (void)sprintf(buf, "iva  %s pta  %"RFT" gpta %s",
			  str1, crGet(viewPid, PTA_ID), str2);
	    return buf;
	case 2:
	    symAddrtoName(crGet(viewPid, IIP_ID), str1, 4, NO, 16);
	    symAddrtoName(crGet(viewPid, IFA_ID), str2, 4, NO, 16);
	    (void)sprintf(buf, "iip  %s ifa  %s ifs  %"RFT,
			  str1, str2, crGet(viewPid, IFS_ID));
	    return buf;
	case 3:
	    symAddrtoName(crGet(viewPid, IHA_ID), str1, 4, NO, 16);
	    (void)sprintf(buf, "isr  %"RFT" iha  %s iim  %"RFT,
			  crGet(viewPid, ISR_ID), str1, crGet(viewPid, IIM_ID));
	    return buf;
	case 4:
	    symAddrtoName(crGet(viewPid, IIPA_ID), str1, 4, NO, 16);
	    (void)sprintf(buf, "itir %"RFT" itm  %"RFT" iipa %s",
			  crGet(viewPid, ITIR_ID), crGet(viewPid, ITM_ID),
			  str1);
	    return buf;
	case 5:
	    (void)sprintf(buf, "lid  %"RFT" ivr  %"RFT" tpr  %"RFT,
			  crGet(viewPid, LID_ID), crGet(viewPid, IVR_ID),
			  crGet(viewPid, TPR_ID));
	    return buf;
	case 6:
	    (void)sprintf(buf, "irr0 %"RFT" irr1 %"RFT" irr2 %"RFT,
			  crGet(viewPid, IRR0_ID), crGet(viewPid, IRR1_ID),
			  crGet(viewPid, IRR2_ID));

	    return buf;
	case 7:
	    (void)sprintf(buf, "irr3 %"RFT" eoi  %"RFT,
			  crGet(viewPid, IRR3_ID), crGet(viewPid, EOI_ID));
	    return buf;
	case 8:
	    (void)sprintf(buf, "itv  %"RFT" pmv  %"RFT" cmcv %"RFT,
			  crGet(viewPid, ITV_ID), crGet(viewPid, PMV_ID),
			  crGet(viewPid, CMCV_ID));
	    return buf;
	case 9:
	    (void)sprintf(buf, "lrr0 %"RFT" lrr1 %"RFT,
			  crGet(viewPid, LRR0_ID), crGet(viewPid, LRR1_ID));
	    return buf;
	case 10:
	case 13:
	case 18:
	case 23:
	case 28: /* blank line before pmc */
	case 45: /* blank line before pmd */
	case 62: /* blank line before kr */
	    (void)strcpy(buf, "");
	    return buf;
    }
    i -= 11;
    if (i < 2) {
	(void)sprintf(buf, "rr%-3d %"RFT" %"RFT" rr%-3d %"RFT" %"RFT,
		i*4,   rrGet(viewPid, i*4),   rrGet(viewPid, i*4+1),
		i*4+2, rrGet(viewPid, i*4+2), rrGet(viewPid, i*4+3));
    } else if (i < 7) {
	i -= 3;
	(void)sprintf(buf, "pkr%-2d %"RFT" %"RFT" pkr%-2d %"RFT" %"RFT,
		i*4,   pkrGet(viewPid, i*4),   pkrGet(viewPid, i*4+1),
		i*4+2, pkrGet(viewPid, i*4+2), pkrGet(viewPid, i*4+3));
    } else if (i < 12) {
	i -= 8;
	(void)sprintf(buf, "dbr%-2d %"RFT" %"RFT" dbr%-2d %"RFT" %"RFT,
		i*4,   dbrGet(viewPid, i*4),   dbrGet(viewPid, i*4+1),
		i*4+2, dbrGet(viewPid, i*4+2), dbrGet(viewPid, i*4+3));
    } else if (i < 17) {
	i -= 13;
	(void)sprintf(buf, "ibr%-2d %"RFT" %"RFT" ibr%-2d %"RFT" %"RFT,
		i*4,   ibrGet(viewPid, i*4),   ibrGet(viewPid, i*4+1),
		i*4+2, ibrGet(viewPid, i*4+2), ibrGet(viewPid, i*4+3));
    } else if (i < 34) {
	i -= 18;
	(void)sprintf(buf, "pmc%-2d %"RFT" %"RFT" pmc%-2d %"RFT" %"RFT,
		i*4,   pmcGet(viewPid, i*4),   pmcGet(viewPid, i*4+1),
		i*4+2, pmcGet(viewPid, i*4+2), pmcGet(viewPid, i*4+3));
    } else if (i < 51) {
	i -= 35;
	(void)sprintf(buf, "pmd%-2d %"RFT" %"RFT" pmd%-2d %"RFT" %"RFT,
		i*4,   pmdGet(viewPid, i*4),   pmdGet(viewPid, i*4+1),
		i*4+2, pmdGet(viewPid, i*4+2), pmdGet(viewPid, i*4+3));
    } else {
	i -= 52;
	(void)sprintf(buf, "bkr%-2d %c%"RFT" %c%"RFT" %c%"RFT" %c%"RFT,
		i*4,
		bkrNatGet(viewPid, i*4)   ? '*' : ' ', bkrGet(viewPid, i*4),
		bkrNatGet(viewPid, i*4+1) ? '*' : ' ', bkrGet(viewPid, i*4+1),
		bkrNatGet(viewPid, i*4+2) ? '*' : ' ', bkrGet(viewPid, i*4+2),
		bkrNatGet(viewPid, i*4+3) ? '*' : ' ', bkrGet(viewPid, i*4+3));
    }
    return buf;
}

/*
 * iarLine - Return the ith line of the IA registers
 */
static char *iarLine(unsigned i)
{
    static char buf[256];	/* XXX no bounds checking :-( */
    REG tmpreg;
    IAeflags efl;
    IASegDesc csd, dsd, ssd, esd, fsd, gsd;
    IASegDesc ldtd, gdtd, tssd, idtd;
    IAcflg cfl;
    IAcr3cr2 cr3cr2;
    IAdr6dr7 dr6dr7;

    switch (i) {
    case 0:
	(void)sprintf(buf, "eax %08x ebx %08x ecx %08x edx %08x"
		      "   eip %04hx:%08x",
	      (WORD)grGet(viewPid, EAX_ID), (WORD)grGet(viewPid, EBX_ID),
	      (WORD)grGet(viewPid, ECX_ID), (WORD)grGet(viewPid, EDX_ID),
	      (HWORD)grGet(viewPid, CS_ID), (WORD)EIPfromIP(ipGet(viewPid)));
	break;
    case 1:
	(void)sprintf(buf, "esi %08x edi %08x ebp %08x esp %08x",
	      (WORD)grGet(viewPid, ESI_ID), (WORD)grGet(viewPid, EDI_ID),
	      (WORD)grGet(viewPid, EBP_ID), (WORD)grGet(viewPid, ESP_ID));
	break;
    case 2:
	(void)sprintf(buf, "cs %04hx ds %04hx es %04hx fs %04hx "
		      "gs %04hx ss %04hx ldt %04hx tss %04hx",
	      (HWORD)grGet(viewPid, CS_ID),  (HWORD)grGet(viewPid, DS_ID),
	      (HWORD)grGet(viewPid, ES_ID),  (HWORD)grGet(viewPid, FS_ID),
	      (HWORD)grGet(viewPid, GS_ID),  (HWORD)grGet(viewPid, SS_ID),
	      (HWORD)grGet(viewPid, LDT_ID), (HWORD)grGet(viewPid, TSS_ID));
	break;
    case 3:
	tmpreg = arGet(viewPid, EFLAGS_ID);
	efl = *(IAeflagsPtr)&tmpreg;
	(void)sprintf(buf, "eflags %08llx [%s|%s|%s|%s|%s|%s|%s|%s|%u|"
	    	      "%s|%s|%s|%s|%s|%s|%s|%s|%s]",
		      (unsigned long long)tmpreg,
		      efl.le ? "LE" : "le", efl.be ? "BE" : "be",
		      efl.lt ? "LT" : "lt", efl.id ? "ID" : "id",
		      efl.ac ? "AC" : "ac", efl.vm ? "VM" : "vm",
		      efl.rf ? "RF" : "rf", efl.nt ? "NT" : "nt",
		      (unsigned int)efl.iopl,
		      efl.of  ? "OF" : "of", efl.df ? "DF" : "df",
		      efl.if_ ? "IF" : "if", efl.tf ? "TF" : "tf",
		      efl.sf  ? "SF" : "sf", efl.zf ? "ZF" : "zf",
		      efl.af  ? "AF" : "af", efl.pf ? "PF" : "pf",
		      efl.cf  ? "CF" : "cf");
	break;
    case 4:
	tmpreg = grGet(viewPid, CSD_ID);
	csd = *(IASegDescPtr)&tmpreg;
	tmpreg = grGet(viewPid, DSD_ID);
	dsd = *(IASegDescPtr)&tmpreg;
	(void)sprintf(buf, "csd  %03llx %05x %08llx [%s|%s|%s|%u|%s|%x]"
		      "  dsd  %03llx %05x %08llx [%s|%s|%s|%u|%s|%x]",
		      (unsigned long long)BitfX(*(DWORD*)&csd, 0, 12),
		      (unsigned int)csd.lim, (unsigned long long)csd.base,
		      csd.g ? "G" : "g", csd.d ? "D" : "d",
		      csd.p ? "P" : "p", (unsigned int)csd.dpl,
		      csd.s ? "S" : "s", (unsigned int)csd.type,
		      (unsigned long long)BitfX(*(DWORD*)&dsd, 0, 12),
		      (unsigned int)dsd.lim,
		      (unsigned long long)dsd.base,
		      dsd.g ? "G" : "g", dsd.d ? "D" : "d",
		      dsd.p ? "P" : "p", (unsigned int)dsd.dpl,
		      dsd.s ? "S" : "s", (unsigned int)dsd.type);
	break;
    case 5:
	tmpreg = grGet(viewPid, SSD_ID);
	ssd = *(IASegDescPtr)&tmpreg;
	tmpreg = grGet(viewPid, ESD_ID);
	esd = *(IASegDescPtr)&tmpreg;
	(void)sprintf(buf, "ssd  %03llx %05x %08llx [%s|%s|%s|%u|%s|%x]"
		      "  esd  %03llx %05x %08llx [%s|%s|%s|%u|%s|%x]",
		      (unsigned long long)BitfX(*(DWORD*)&ssd, 0, 12),
		      (unsigned int)ssd.lim, (unsigned long long)ssd.base,
		      ssd.g ? "G" : "g", ssd.d ? "B" : "b",
		      ssd.p ? "P" : "p", (unsigned int)ssd.dpl,
		      ssd.s ? "S" : "s", (unsigned int)ssd.type,
		      (unsigned long long)BitfX(*(DWORD*)&esd, 0, 12),
		      (unsigned int)esd.lim, (unsigned long long)esd.base,
		      esd.g ? "G" : "g", esd.d ? "D" : "d",
		      esd.p ? "P" : "p", (unsigned int)esd.dpl,
		      esd.s ? "S" : "s", (unsigned int)esd.type);
	break;
    case 6:
	tmpreg = grGet(viewPid, FSD_ID);
	fsd = *(IASegDescPtr)&tmpreg;
	tmpreg = grGet(viewPid, GSD_ID);
	gsd = *(IASegDescPtr)&tmpreg;
	(void)sprintf(buf, "fsd  %03llx %05x %08llx [%s|%s|%s|%u|%s|%x]"
		      "  gsd  %03llx %05x %08llx [%s|%s|%s|%u|%s|%x]",
		      (unsigned long long)BitfX(*(DWORD*)&fsd, 0, 12),
		      (unsigned int)fsd.lim, (unsigned long long)fsd.base,
		      fsd.g ? "G" : "g", fsd.d ? "D" : "d",
		      fsd.p ? "P" : "p", (unsigned int)fsd.dpl,
		      fsd.s ? "S" : "s", (unsigned int)fsd.type,
		      (unsigned long long)BitfX(*(DWORD*)&gsd, 0, 12),
		      (unsigned int)gsd.lim, (unsigned long long)gsd.base,
		      gsd.g ? "G" : "g", gsd.d ? "D" : "d",
		      gsd.p ? "P" : "p", (unsigned int)gsd.dpl,
		      gsd.s ? "S" : "s", (unsigned int)gsd.type);
	break;
    case 7:
	tmpreg = grGet(viewPid, LDTD_ID);
	ldtd = *(IASegDescPtr)&tmpreg;
	tmpreg = grGet(viewPid, GDTD_ID);
	gdtd = *(IASegDescPtr)&tmpreg;
	(void)sprintf(buf, "ldtd %03llx %05x %08llx  "
		      "              gdtd %03llx %05x %08llx",
		      (unsigned long long)BitfX(*(DWORD*)&ldtd, 0, 12),
		      (unsigned int)ldtd.lim, (unsigned long long)ldtd.base,
		      (unsigned long long)BitfX(*(DWORD*)&gdtd, 0, 12),
		      (unsigned int)gdtd.lim, (unsigned long long)gdtd.base);
	break;
    case 8:
	tmpreg = arGet(viewPid, TSSD_ID);
	tssd = *(IASegDescPtr)&tmpreg;
	tmpreg = arGet(viewPid, IDTD_ID);
	idtd = *(IASegDescPtr)&tmpreg;
	(void)sprintf(buf, "tssd %03llx %05x %08llx  "
		      "              idtd %03llx %05x %08llx",
		      (unsigned long long)BitfX(*(DWORD*)&tssd, 0, 12),
		      (unsigned int)tssd.lim, (unsigned long long)tssd.base,
		      (unsigned long long)BitfX(*(DWORD*)&idtd, 0, 12),
		      (unsigned int)idtd.lim, (unsigned long long)idtd.base);
	break;
    case 9:
	tmpreg = arGet(viewPid, CFLG_ID);
	cfl = *(IAcflgPtr)&tmpreg;
	(void)sprintf(buf, "cr0 %08llx [%s|%s|%s|%s|%s|%s|%s|%s|%s] "
		      "iobase %"RFT,
		      BitfX(*(DWORD*)&cfl, 0, 32),
		      cfl.pce ? "PCE" : "pce", cfl.pge ? "PGE" : "pge",
		      cfl.mce ? "MCE" : "mce", cfl.pae ? "PAE" : "pae",
		      cfl.pse ? "PSE" : "pse", cfl.de  ? "DE"  : "de",
		      cfl.tsd ? "TSD" : "tsd", cfl.pvi ? "PVI" : "pvi",
		      cfl.vme ? "VME" : "vme",
		      (unsigned long long)arGet(viewPid, IOBASE_ID));
	break;
    case 10:
	tmpreg = arGet(viewPid, CR3_CR2_ID);
	cr3cr2 = *(IAcr3cr2Ptr)&tmpreg;
	(void)sprintf(buf, "cr2 %08llx cr3 %08llx [%05x|%s|%s]",
		      (unsigned long long)cr3cr2.pfla,
		      (unsigned long long)BitfX(*(DWORD*)&cr3cr2, 32, 32),
		      (unsigned int)cr3cr2.pdb,
		      cr3cr2.pcd ? "PCD" : "pcd",
		      cr3cr2.pwt ? "PWT" : "pwt");
	break;
    case 11:
	tmpreg = arGet(viewPid, CFLG_ID);
	cfl = *(IAcflgPtr)&tmpreg;
	(void)sprintf(buf, "cr4 %08llx [%s|%s|%s|%s|%s|"
		      "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s]",
		      BitfX(*(DWORD*)&cfl, 32, 32),
		      cfl.pg  ? "PG" : "pg", cfl.cd ? "CD" : "cd",
		      cfl.nw  ? "NW" : "nw", cfl.am ? "AM" : "am",
		      cfl.wp  ? "WP" : "wp",
		      cfl.nm  ? "NM" : "nm", cfl.ii ? "II" : "ii",
		      cfl.if_ ? "IF" : "if", cfl.io ? "IO" : "io",
		      cfl.ne  ? "NE" : "ne", cfl.et ? "ET" : "et",
		      cfl.ts  ? "TS" : "ts", cfl.em ? "EM" : "em",
		      cfl.mp  ? "MP" : "mp", cfl.pe ? "PE" : "pe");
	break;
    case 12:
	tmpreg = arGet(viewPid, DR6_DR7_ID);
	dr6dr7 = *(IAdr6dr7Ptr)&tmpreg;
	(void)sprintf(buf, "dr6 %08llx [%s|%s|%s|%s|%s|%s|%s]",
		      BitfX(*(DWORD*)&dr6dr7, 32, 32),
		      dr6dr7.bt   ? "BT" : "bt", dr6dr7.bs  ? "BS" : "bs",
		      dr6dr7.bd   ? "BD" : "bd", dr6dr7.b3_ ? "B3" : "b3",
		      dr6dr7.b2_  ? "B2" : "b2", dr6dr7.b1_ ? "B1" : "b1",
		      dr6dr7.b0_  ? "B0" : "b0");
	break;
    case 13:
	tmpreg = arGet(viewPid, DR6_DR7_ID);
	dr6dr7 = *(IAdr6dr7Ptr)&tmpreg;
	(void)sprintf(buf, "dr7 %08llx [%u|%u|%u|%u|%u|%u|%u|%u|"
		      "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s]",
		      (unsigned long long)BitfX(*(DWORD*)&dr6dr7, 0, 32),
		      (unsigned int)dr6dr7.len3, (unsigned int)dr6dr7.rw3,
		      (unsigned int)dr6dr7.len2, (unsigned int)dr6dr7.rw2,
		      (unsigned int)dr6dr7.len1, (unsigned int)dr6dr7.rw1,
		      (unsigned int)dr6dr7.len0, (unsigned int)dr6dr7.rw0,
		      dr6dr7.gd ? "GD" : "gd",
		      dr6dr7.ge ? "GE" : "ge", dr6dr7.le ? "LE" : "le",
		      dr6dr7.g3 ? "G3" : "g3", dr6dr7.l3 ? "L3" : "l3",
		      dr6dr7.g2 ? "G2" : "g2", dr6dr7.l2 ? "L2" : "l2",
		      dr6dr7.g1 ? "G1" : "g1", dr6dr7.l1 ? "L1" : "l1",
		      dr6dr7.g0 ? "G0" : "g0", dr6dr7.l0 ? "L0" : "l0");
	break;
    }
    return buf;
}


/****************************/
/* Program Window Routines */
/****************************/

void prgwUpdate(void)
{
    switch (interface) {
	case X_INTERFACE:
#ifdef HAVE_MOTIF
	    prgwUpdateX();
#endif
	    break;
	case CURSES_INTERFACE:
	    prgwUpdateCur();
	    break;
        case GTK_INTERFACE:
#ifdef HAVE_GTK
	    prgwUpdateGtk();
#endif
	    break;
	case BATCH:
	    break;
    }
}

void prgwPCRedraw(void)
{
    REG psrval;
#if 0 /* MP support? */
    unsigned save = cproc;

    for (cproc = 0; cproc < nproc; cproc++) {
	prgwCVA = ipGet(viewPid) & ~0xFULL;
    }
    cproc = save;
#endif
    psrval = psrGet(viewPid);
    if (X_PSR_IS(psrval))		/* iA */
	prgwCVA = ipGet(viewPid);
    else
	prgwCVA = ipGet(viewPid) & ~0xFULL;
    prgwIS = X_PSR_IS(psrval);
    prgwIAmode = getIAmode();
    prgCmd = PrgUseCVA;
    prgwUpdate();
}

void prgwDraw(void)
{
    switch (interface) {
	case X_INTERFACE:
#ifdef HAVE_MOTIF
	    prgwDrawX();
#endif
	    break;
	case CURSES_INTERFACE:
	    prgwDrawCur();
	    break;
        case GTK_INTERFACE:
#ifdef HAVE_GTK
	    prgwDrawGtk();
#endif
	    break;
	case BATCH:
	    break;
    }
}


/************************/
/* Data Window Routines */
/************************/

/*--------------------------------------------------------------------------
 * dataWindow$Table$Initialize - Set the value of topdatw, the current size of
 *  the data window table.  This value will be modified by the implementation-
 *  dependent routines that adds new data windows to the table.
 *--------------------------------------------------------------------------*/
static void datwTblInit(void)
{
    Datw *pdatw;

    topdatw = 0;
    for (pdatw = datwtbl; strcmp(pdatw->tag, ""); pdatw++)
	topdatw++;
}

/*---------------------------------------------------------------------------
 * dataWindow$Make$ActiveWindow - Make this the active data window.
 *---------------------------------------------------------------------------*/
BOOL datwMakeActivew(const char *tag)
{
    Datw *pdatw = datwtbl;

    for (pdatw = datwtbl; pdatw < &datwtbl[topdatw]; pdatw++)
	if (!strcmp(pdatw->tag, tag)) {
	    actDatw = pdatw;
	    actDatw->show = YES;
	    datwUpdate();
	    return YES;
	}
    (void)fprintf(stderr, "Error: didn't find data window tag: %s\n", tag);
    return NO;
}

unsigned datwSz(const char *tag)
{
    Datw *pdatw = datwtbl;

    for (pdatw = datwtbl; pdatw < &datwtbl[topdatw]; pdatw++)
	if (!strcmp(pdatw->tag, tag))
	    return pdatw->size;
    (void)fprintf(stderr, "Error: didn't find data window tag: %s\n", tag);
    return 0;
}

void datwUpdate(void)
{
    ADDR adr;

    if (datExpr[0] && evalExpr(datExpr, HEXEXP, &adr)) {
	datwLVA = datwCVA;
	datwCVA = adr;
    }
    switch (interface) {
	case X_INTERFACE:
#ifdef HAVE_MOTIF
	    datwUpdateX();
#endif
	    break;
	case CURSES_INTERFACE:
	    datwUpdateCur();
	    break;
        case GTK_INTERFACE:
#ifdef HAVE_GTK
	    datwUpdateGtk();
#endif
	    break;
	case BATCH:
	    break;
    }
}

/***************************/
/* Command Window Routines */
/***************************/

void cmdwPrint(const char *fmt, ...)
{
    va_list ap;
    char s[200];

    va_start(ap, fmt);
    vsprintf(s, fmt, ap);
    va_end(ap);
    switch (interface) {
	case BATCH:
	    cmdwPrintBatch(s);
	    break;
	case X_INTERFACE:
#ifdef HAVE_MOTIF
	    cmdwPrintX(s);
#endif
	    break;
        case GTK_INTERFACE:
#ifdef HAVE_GTK
	    cmdwPrintGtk(s);
#endif
	    break;
	case CURSES_INTERFACE:
	    cmdwPrintCur(s);
	    break;
    }
}

void msgwPrint(const char *fmt, ...)
{
    va_list ap;
    char s[200];

    va_start(ap, fmt);
    vsprintf(s, fmt, ap);
    va_end(ap);
    /* XXX - add msgwPrint*() to BATCH & CURSES_INTERFACE */
    switch (interface) {
	case BATCH:
	    cmdwPrintBatch(s);
	    break;
	case X_INTERFACE:
#ifdef HAVE_MOTIF
	    msgwPrintX(s);
#endif
	    break;
        case GTK_INTERFACE:
#ifdef HAVE_GTK
	    msgwPrintGtk(s);
#endif
	    break;
	case CURSES_INTERFACE:
	    cmdwPrintCur(s);
	    break;
    }
}

void cmdwSetStatus(const char *msg)
{
    switch (interface) {
	case X_INTERFACE:
#ifdef HAVE_MOTIF
	    cmdwSetStatusX(msg);
#endif
	    break;
	case CURSES_INTERFACE:
	    cmdwSetStatusCur(msg);
	    break;
        case GTK_INTERFACE:
#ifdef HAVE_GTK
	    cmdwSetStatusGtk(msg);
#endif
	    break;
	case BATCH:
	    break;
    }
}

/*--------------------------------------------------------------------------
 * command$ErrorPrint - Print an error message in command window preceded
 *  by a bell.  Flush any further input.
 *--------------------------------------------------------------------------*/
void cmdErr(const char *fmt, ...)
{
    va_list ap;
    char s[100];

    va_start(ap, fmt);
    vsprintf(s, fmt, ap);
    va_end(ap);
    switch (interface) {
	case BATCH:
	    cmdwPrintBatch(s);
	    break;
	case X_INTERFACE:
#ifdef HAVE_MOTIF
	    msgwPrintX(s);
#endif
	    break;
        case GTK_INTERFACE:
#ifdef HAVE_GTK
	    msgwPrintGtk(s);
#endif
	    break;
	case CURSES_INTERFACE:
	    cmdwPrintCur(s);
	    (void)putchar(BEL);
	    break;
    }
}

/*--------------------------------------------------------------------------
 * command$WarningPrint - Print a warning message in command window.
 * Flush any further input.
 *--------------------------------------------------------------------------*/
void cmdWarn(const char *fmt, ...)
{
    va_list ap;
    char s[100];

    va_start(ap, fmt);
    vsprintf(s, fmt, ap);
    va_end(ap);
    switch (interface) {
	case BATCH:
	    cmdwPrintBatch(s);
	    break;
	case X_INTERFACE:
#ifdef HAVE_MOTIF
	    msgwPrintX(s);
#endif
	    break;
        case GTK_INTERFACE:
#ifdef HAVE_GTK
	    msgwPrintGtk(s);
#endif
	    break;
	case CURSES_INTERFACE:
	    cmdwPrintCur(s);
	    break;
    }
}

static void cmdwUpdate(void)
{
    switch (interface) {
	case X_INTERFACE:
#ifdef HAVE_MOTIF
	    cmdwUpdateX();
#endif
	    break;
	case CURSES_INTERFACE:
	    cmdwUpdateCur();
	    break;
        case GTK_INTERFACE:
#ifdef HAVE_GTK
	    cmdwUpdateGtk();
#endif
	    break;
	case BATCH:
	    break;
    }
}

/****************************************************************************
 * Screen routines
 ***************************************************************************/

void scrnInit(void)
{
    switch (interface) {
	case BATCH:
	    scrnInitBatch();
	    break;
	case X_INTERFACE:
#ifdef HAVE_MOTIF
	    scrnInitX();
#else
	    fprintf (stderr, "Error: program was built without X support\n");
	    exit (EXIT_FAILURE);
#endif
	    break;
	case CURSES_INTERFACE:
	    scrnInitCur();
	    break;
        case GTK_INTERFACE:
#ifdef HAVE_GTK
	    scrnInitGtk();
#else
	    fprintf (stderr, "Error: program was build without GTK support\n");
	    exit(EXIT_FAILURE);
#endif
	    break;

    }
}

void scrnUpdate(void)
{
    regwUpdate();
    prgwPCRedraw();
    datwUpdate();
    cmdwUpdate();
}

void scrnEnd(void)
{
    switch (interface) {
	case X_INTERFACE:
#ifdef HAVE_MOTIF
	    scrnEndX();
#endif
	    break;
	case CURSES_INTERFACE:
	    scrnEndCur();
	    break;
        case GTK_INTERFACE:
#ifdef HAVE_GTK
	    scrnEndGtk();
#endif
	    break;
	case BATCH:
	    break;
    }
}

ssize_t sysWrite(int fd, void *buf, size_t len)
{
    /* special case for curses when fildes is stdout or stderr */
    if (interface == CURSES_INTERFACE && (fd == 1 || fd == 2)) {
	static char scrout[BUFSIZ];

	(void)strncpy(scrout, buf, len);
	scrout[len > 200 ? 200 : len] = '\0';
	cmdwPrint("%s", scrout);
	return len;
    } else
	return write(fd, buf, len);
}

void cmdLoop(void)
{
    switch (interface) {
	case BATCH:
	    cmdLoopBatch();
	    break;
	case X_INTERFACE:
#ifdef HAVE_MOTIF
	    keepXLoopAlive = YES;
	    cmdLoopX();
#endif
	    break;
        case GTK_INTERFACE:
#ifdef HAVE_GTK
	    cmdLoopGtk();
#endif
	    break;
	case CURSES_INTERFACE:
	    cmdLoopCur();
	    break;
    }
}

/* return Ski ID */
char *skiID(void)
{
    static char buf[80];

    (void)sprintf(buf, "Version %s (%s)", VERSION, getImplStr());
    return buf;
}

/*--------------------------------------------------------------------------
 * product$Info - Return a string describing the simulator.
 *--------------------------------------------------------------------------*/
char *productInfo(void)
{
    static char buf[300];

    (void)sprintf(buf,
		  "IA-64 Debugger/Simulator\n"
		  "%s\n\n"
		  "Copyright (c) 1995-2007\n"
		  "Hewlett-Packard Development Company, L.P.\n"
		  "\n"
		  "Redistribution governed by the GNU General Public License;\n"
		  "please see the file COPYING for details.\n"
		  , skiID());
    return buf;
}

/*--------------------------------------------------------------------------
 * get$Simulator$ID - Return a string with the simulator name and id.
 *--------------------------------------------------------------------------*/
char *getSimID(void)
{
    static char buf[80];

    (void)sprintf(buf, "Ski Simulator %s", skiID());
    return buf;
}
