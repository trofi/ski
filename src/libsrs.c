/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Save/Restore Routines
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

#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "std.h"
#include "types.h"
#include "state.h"
#include "sim.h"
#include "memory.h"
#include "libsrs.h"
#include "libcore.h"
#include "libui.h"

#if 0
#include "libsym.h"
#endif

#define MAJVER		1
#define MINVER		1
#define NDWPERLN	4

#define MAXPROCS	1

typedef BOOL	(*PPSF)(FILE *f, int procno);
typedef RstStat	(*PPRF)(FILE *f, int procno);

typedef struct {
    const char	*varnam;
    PPSF	psvefn;
    PPRF	prstfn;
} PSveRstRec;

/*##################### Globals - Exports ##################################*/


/*##################### Globals - Imports ##################################*/

void symGetInit(void);			/* point to the top of symbol table */
BOOL symGet(char **symnam, ADDR *adr);
void symSet(char *symnam, ADDR adr);

/*##################### Local Variables   ##################################*/

static void saveHdrInfo(FILE *f);
static BOOL saveProcState(FILE *f);
static BOOL saveSysState(FILE *f);
static BOOL saveSymbols(FILE *f);

static BOOL ipSave(FILE *f, int procno);
static BOOL psrSave(FILE *f, int procno);
static BOOL grSave(FILE *f, int procno);
static BOOL grnatSave(FILE *f, int procno);
static BOOL frSave(FILE *f, int procno);
static BOOL prSave(FILE *f, int procno);
static BOOL brSave(FILE *f, int procno);
static BOOL arSave(FILE *f, int procno);
static BOOL cfmSave(FILE *f, int procno);
static BOOL bolSave(FILE *f, int procno);
static BOOL rseSave(FILE *f, int procno);
static BOOL crSave(FILE *f, int procno);
static BOOL bkrSave(FILE *f, int procno);
static BOOL bkrnatSave(FILE *f, int procno);
static BOOL rrSave(FILE *f, int procno);
static BOOL pkrSave(FILE *f, int procno);
static BOOL dbrSave(FILE *f, int procno);
static BOOL ibrSave(FILE *f, int procno);
static BOOL pmcSave(FILE *f, int procno);
static BOOL pmdSave(FILE *f, int procno);
static BOOL dtrSave(FILE *f, int procno);
static BOOL itrSave(FILE *f, int procno);

static BOOL memSave(FILE *f);

static BOOL restoreHdrInfo(FILE *f);
static RstStat restoreProcState(FILE *f);
static RstStat restoreSysState(FILE *f);
static RstStat restoreSymbols(FILE *f);
static RstStat otherSimState(FILE *f);

static RstStat ipRestore(FILE *f, int procno);
static RstStat psrRestore(FILE *f, int procno);
static RstStat grRestore(FILE *f, int procno);
static RstStat grnatRestore(FILE *f, int procno);
static RstStat frRestore(FILE *f, int procno);
static RstStat prRestore(FILE *f, int procno);
static RstStat brRestore(FILE *f, int procno);
static RstStat arRestore(FILE *f, int procno);
static RstStat cfmRestore(FILE *f, int procno);
static RstStat bolRestore(FILE *f, int procno);
static RstStat rseRestore(FILE *f, int procno);
static RstStat crRestore(FILE *f, int procno);
static RstStat bkrRestore(FILE *f, int procno);
static RstStat bkrnatRestore(FILE *f, int procno);
static RstStat rrRestore(FILE *f, int procno);
static RstStat pkrRestore(FILE *f, int procno);
static RstStat dbrRestore(FILE *f, int procno);
static RstStat ibrRestore(FILE *f, int procno);
static RstStat pmcRestore(FILE *f, int procno);
static RstStat pmdRestore(FILE *f, int procno);
static RstStat dtrRestore(FILE *f, int procno);
static RstStat itrRestore(FILE *f, int procno);

static RstStat memRestore(FILE *f);

static PSveRstRec psversttbl[] = {
    { "ip",	ipSave,		ipRestore },
    { "psr",	psrSave,	psrRestore },
    { "gr",	grSave,		grRestore },
    { "grnat",	grnatSave,	grnatRestore },
    { "fr",	frSave,		frRestore },
    { "pr",	prSave,		prRestore },
    { "br",	brSave,		brRestore },
    { "ar",	arSave,		arRestore },
    { "cfm",	cfmSave,	cfmRestore },
    { "bol",	bolSave,	bolRestore },
    { "rse",	rseSave,	rseRestore },
    { "cr",	crSave,		crRestore },
    { "bkr",	bkrSave,	bkrRestore },
    { "bkrnat",	bkrnatSave,	bkrnatRestore },
    { "rr",	rrSave,		rrRestore },
    { "pkr",	pkrSave,	pkrRestore },
    { "dbr",	dbrSave,	dbrRestore },
    { "ibr",	ibrSave,	ibrRestore },
    { "pmc",	pmcSave,	pmcRestore },
    { "pmd",	pmdSave,	pmdRestore },
    { "dtr",	dtrSave,	dtrRestore },
    { "itr",	itrSave,	itrRestore },
    { NULL,	NULL,		NULL }
};

static const char *simprefix[] = {
    "ski_",	"snow_",	NULL
};

static char errmsg[BUFSIZ] = "";

static unsigned lineno;
static REG dwval;
static WORD wval;
static char strval[80], token[80];

static void freeMemlist(Meminfo_p list);
static int nextToken(FILE *f, char *token);

/*##################### Functions and Subroutines ##########################*/

/*
 * Save Routines
 */

BOOL srs_saveState(char *fname)
{
    FILE *f;
    BOOL status;

    if (!(f = fopen(fname, "w"))) {
	sprintf(errmsg, "Could not open save file '%s'", fname);
	return NO;
    }
    saveHdrInfo(f);

    status = saveProcState(f) && saveSysState(f) && saveSymbols(f)
	     && saveSimState(f);

    (void)fclose(f);
    return status;
}

static void saveHdrInfo(FILE *f)
{
    time_t tinsec = time(0);

    (void)fprintf(f, "# Simulator state saved by %s\n", getSimID());
    (void)fprintf(f, "# on %s\n", ctime(&tinsec));
    (void)fprintf(f, "file_ver %d.%d\n", MAJVER, MINVER);
}

static BOOL saveProcState(FILE *f)
{
    int procno;
    BOOL status = YES;
    PSveRstRec *p;

    fprintf(f, "nprocs %d\n", MAXPROCS);
    for (procno = 0; procno < MAXPROCS; procno++) {
	fprintf(f, "proc %d\n", procno);
	for (p = psversttbl; p->psvefn; p++)
	    if (!p->psvefn(f, procno)) {
		status = NO;
		break;
	    }
	if (status)
	    if (!saveProcImpl(f, procno)) {
		status = NO;
		break;
	    }
    }
    return status;
}

static BOOL saveSysState(FILE *f)
{
    BOOL status = YES;

    if (!memSave(f))			status = NO;
    else if (!saveSysImpl(f))		status = NO;

    return status;
}

static BOOL saveSymbols(FILE *f)
{
    char *sym;
    ADDR adr;

    symGetInit();
    while (symGet(&sym, &adr))
	fprintf(f, "sym %llx %s\n", adr, sym);
    return YES;
}

static BOOL ipSave(FILE *f, int proc)
{
    fprintf(f, "ip %llx\n", ipGet(proc));
    return YES;
}

static BOOL psrSave(FILE *f, int proc)
{
    fprintf(f, "psr %llx\n", psrGet(proc));
    return YES;
}

static BOOL grSave(FILE *f, int proc)
{
    int i;

    fprintf(f, "gr");
    for (i = 0; i < NGRS; i++) {
	fprintf(f, " %llx", phyGrGet(proc, i));
	if (i % 8 == 7 || i == (NGRS - 1))
	    fprintf(f, "\n");
    }
    return YES;
}

static BOOL grnatSave(FILE *f, int proc)
{
    REG nats = 0ULL;
    int i, regs = 0;

    fprintf(f, "grnat");
    for (i = 0; i < NGRS; i++) {
	nats = nats << 1 | phyGrNatGet(proc, i);
	if ((i % 64) == 63 || i == (NGRS - 1)) {
	    fprintf(f, " %llx", nats);
	    nats = 0ULL;
	    regs++;
	    if (!(regs % 8) || i == (NGRS - 1))
		fprintf(f, "\n");
	}
    }
    return YES;
}

static BOOL frSave(FILE *f, int proc)
{
    int i;
    BYTE sign;
    WORD exp;
    DWORD mant;

    fprintf(f, "fr");
    for (i = 0; i < NFRS; i++) {
	sign = phyFrSignGet(proc, i);
	exp = phyFrExpGet(proc, i);
	mant = phyFrMantGet(proc, i);
	fprintf(f, " %x %x %llx", sign, exp, mant);
	if (i % 4 == 3)
	    fprintf(f, "\n");
    }
    return YES;
}

static BOOL prSave(FILE *f, int proc)
{
    REG preds = 0ULL;
    int i;

    fprintf(f, "pr");
    for (i = 0; i < NPRS; i++)
	preds = preds << 1 | phyPrGet(proc, i);
    fprintf(f, " %llx\n", preds);
    return YES;
}

static BOOL brSave(FILE *f, int proc)
{
    int i;

    fprintf(f, "br");
    for (i = 0; i < NBRS; i++)
	fprintf(f, " %llx", brGet(proc, i));
    fprintf(f, "\n");
    return YES;
}

static BOOL arSave(FILE *f, int proc)
{
    int i;

    fprintf(f, "ar");
    for (i = 0; i < NARS; i++) {
	fprintf(f, " %llx", arGet(proc, i));
	if (i % 8 == 7)
	    fprintf(f, "\n");
    }
    return YES;
}

static BOOL cfmSave(FILE *f, int proc)
{
    fprintf(f, "cfm %llx\n", cfmGet(proc));
    return YES;
}

static BOOL bolSave(FILE *f, int proc)
{
    fprintf(f, "bol %llu\n", bolGet(proc));
    return YES;
}

static BOOL rseSave(FILE *f, int proc)
{
    (void)fprintf(f, "rse %d %d %d %d %d %d\n",
		  (int)dirtyGet(proc), (int)cleanGet(proc),
		  (int)invalidGet(proc),
		  (int)dirtyNatGet(proc), (int)cleanNatGet(proc),
		  cfleGet(proc));
    return YES;
}

static BOOL crSave(FILE *f, int proc)
{
    int i;

    fprintf(f, "cr");
    for (i = 0; i < NCRS; i++) {
	fprintf(f, " %llx", crGet(proc, i));
	if (i % 8 == 7)
	    fprintf(f, "\n");
    }
    return YES;
}

static BOOL bkrSave(FILE *f, int proc)
{
    int i;

    fprintf(f, "bkr");
    for (i = 0; i < NBGRS; i++) {
	fprintf(f, " %llx", bkrGet(proc, i));
	if (i % 8 == 7)
	    fprintf(f, "\n");
    }
    return YES;
}

static BOOL bkrnatSave(FILE *f, int proc)
{
    HWORD nats = 0;
    int i;

    fprintf(f, "bkrnat");
    for (i = 0; i < NBGRS; i++)
	nats = nats << 1 | bkrNatGet(proc, i);
    fprintf(f, " %hx\n", nats);
    return YES;
}

static BOOL rrSave(FILE *f, int proc)
{
    int i;

    fprintf(f, "rr");
    for (i = 0; i < NRRS; i++) {
	fprintf(f, " %llx", rrGet(proc, i));
	if (i % 8 == 7)
	    fprintf(f, "\n");
    }
    return YES;
}

static BOOL pkrSave(FILE *f, int proc)
{
    int i;

    fprintf(f, "pkr %d", NPKRS);
    for (i = 0; i < NPKRS; i++) {
	fprintf(f, " %llx", pkrGet(proc, i));
	if (i % 8 == 7)
	    fprintf(f, "\n");
    }
    return YES;
}

static BOOL dbrSave(FILE *f, int proc)
{
    int i;

    fprintf(f, "dbr %d", NDBRS);
    for (i = 0; i < NDBRS; i++) {
	fprintf(f, " %llx", dbrGet(proc, i));
	if (i % 8 == 7)
	    fprintf(f, "\n");
    }
    return YES;
}

static BOOL ibrSave(FILE *f, int proc)
{
    int i;

    fprintf(f, "ibr %d", NIBRS);
    for (i = 0; i < NIBRS; i++) {
	fprintf(f, " %llx", ibrGet(proc, i));
	if (i % 8 == 7)
	    fprintf(f, "\n");
    }
    return YES;
}

static BOOL pmcSave(FILE *f, int proc)
{
    int i;

    fprintf(f, "pmc %d", NPMS);
    for (i = 0; i < NPMS; i++) {
	fprintf(f, " %llx", pmcGet(proc, i));
	if (i % 8 == 7)
	    fprintf(f, "\n");
    }
    return YES;
}

static BOOL pmdSave(FILE *f, int proc)
{
    int i;

    fprintf(f, "pmd %d", NPMS);
    for (i = 0; i < NPMS; i++) {
	fprintf(f, " %llx", pmdGet(proc, i));
	if (i % 8 == 7)
	    fprintf(f, "\n");
    }
    return YES;
}

static BOOL dtrSave(FILE *f, int proc)
{
    fprintf(f, "# DTR not saved\n");
    return YES;
}

static BOOL itrSave(FILE *f, int proc)
{
    fprintf(f, "# ITR not saved\n");
    return YES;
}

static BOOL memSave(FILE *f)
{
    Meminfo_p p, start;
    int ndws, i;
    BOOL allZeros, zeroPage;
    DWORD *dwsve, *dwp;

    if (!memGet(&start)) {
	sprintf(errmsg, "malloc failed in memGet");
	return NO;
    }
    for (p = start; p; p = p->next) {
	ndws = (p->numpgs * 4096) >> 3;
	dwsve = p->mdata;
	zeroPage = YES;
	for (i = 0; i < ndws/NDWPERLN; i++) {
	    allZeros = YES;
	    for (dwp = dwsve; dwp < dwsve + NDWPERLN; dwp++)
		if (*dwp) {
		    allZeros = NO;
		    zeroPage = NO;
		    break;
		}
	    if (!allZeros) {
		fprintf(f, "mem %llx %d", p->startadr + i*8*NDWPERLN,
					   NDWPERLN);
		for (dwp = dwsve; dwp < dwsve + NDWPERLN; dwp++)
		    fprintf(f, " %llx", *dwp);
		fprintf(f, "\n");
	    }
	    dwsve += NDWPERLN;
	}
	if (zeroPage)
	    fprintf(f, "mem %llx 0\n", p->startadr);
    }
    freeMemlist(start);
    return YES;
}


/*
 * Restore Routines
 */

BOOL srs_restoreState(char *fname)
{
    FILE *f;
    BOOL status = YES;
    RstStat rstat;

    if (!(f = fopen(fname, "r"))) {
	sprintf(errmsg, "Could not open restore file '%s'", fname);
	return NO;
    }

    lineno = 1;
    if (!restoreHdrInfo(f)) {
	(void)fclose(f);
	return NO;
    }

    while (status) {
	if (nextToken(f, token) == EOF)
	    break;

	if ((rstat = restoreProcState(f)) == ERROR) {
	    status = NO;
	    break;
	} else if (rstat == FOUND)
	    continue;

	if ((rstat = restoreSysState(f)) == ERROR) {
	    status = NO;
	    break;
	} else if (rstat == FOUND)
	    continue;

	if ((rstat = restoreSymbols(f)) == ERROR) {
	    status = NO;
	    break;
	} else if (rstat == FOUND)
	    continue;

	if ((rstat = restoreSimState(f, token)) == ERROR) {
	    status = NO;
	    break;
	} else if (rstat == FOUND)
	    continue;

	if ((rstat = otherSimState(f)) == ERROR) {
	    status = NO;
	    break;
	} else if (rstat == FOUND)
	    continue;

	sprintf(errmsg, "Unrecognized restore file tag in line %d: %s",
			 lineno, token);
	status = NO;
    }

    (void)fclose(f);
    return status;
}

static BOOL restoreHdrInfo(FILE *f)
{
    int maj, min;

    if (!srs_nextRstVal(f, "%s", strval))
	return NO;
    if (strcmp("file_ver", strval)) {
	sprintf(errmsg, "Missing file version number");
	return NO;
    }
    if (!srs_nextRstVal(f, "%d.%d", &maj, &min))
	return NO;
    if (maj != MAJVER) {
	sprintf(errmsg, "Incompatible file version number: expected %d.x,"
			" found %d.%d", MAJVER, maj, min);
	return NO;
    }
    return YES;
}

static RstStat restoreProcState(FILE *f)
{
    static int maxproc = 0, procno = -1;
    PSveRstRec *p;

    if (!strcmp("nprocs", token)) {
	if (!srs_nextRstVal(f, "%d", &maxproc))
	    return ERROR;
	if (maxproc > MAXPROCS) {
	    sprintf(errmsg, "Illegal nproc: must be <= %d (found %d)",
			     MAXPROCS, maxproc);
	    return ERROR;
	} else
	    return FOUND;
    } else if (!strcmp("proc", token)) {
	if (!srs_nextRstVal(f, "%d", &procno))
	    return ERROR;
	if (procno > maxproc) {
	    sprintf(errmsg, "Illegal proc #: must be < %d (found %d)",
			     maxproc, procno);
	    return ERROR;
	} else
	    return FOUND;
    } else {
	for (p = psversttbl; p->prstfn; p++) {
	    if (!strcmp(p->varnam, token))
		return p->prstfn(f, procno);
	}
	return restoreProcImpl(f, token, procno);
    }
}

static RstStat restoreSysState(FILE *f)
{
    if (!strcmp("mem", token))
	return memRestore(f);
    else
	return restoreSysImpl(f, token);
}

static RstStat restoreSymbols(FILE *f)
{
    ADDR adr;

    if (!(strcmp("sym", token))) {
	if (!srs_nextRstVal(f, "%llx", &adr))
	    return ERROR;
	if (!srs_nextRstVal(f, "%s", strval))
	    return ERROR;
	symSet(strval, adr);
	return FOUND;
    } else
	return NOT_FOUND;
}

static RstStat otherSimState(FILE *f)
{
    const char **p = &simprefix[0];

    for (; *p; p++) {
	int count, i;

	if (strlen(token) <= strlen(*p) || strncmp(*p, token, strlen(*p)))
	    continue;
	if (!srs_nextRstVal(f, "%d", &count))
	    return ERROR;
	for (i = 0; i < count; i++)		/* ignore 'count' values */
	    if (!srs_nextRstVal(f, "%s", strval))
		return ERROR;
	return FOUND;
    }
    return NOT_FOUND;
}

static RstStat ipRestore(FILE *f, int procno)
{
    if (!srs_nextRstVal(f, "%llx", &dwval))
	return ERROR;
    return ipSet(procno, dwval) ? FOUND : ERROR;
}

static RstStat psrRestore(FILE *f, int procno)
{
    if (!srs_nextRstVal(f, "%llx", &dwval))
	return ERROR;
    return psrSet(procno, dwval) ? FOUND : ERROR;
}

static RstStat grRestore(FILE *f, int procno)
{
    int i;

    for (i = 0; i < NGRS; i++) {
	if (!srs_nextRstVal(f, "%llx", &dwval))
	    return ERROR;
	if (!phyGrSet(procno, i, dwval)) {
	    sprintf(errmsg, "Can't restore nonzero value to GR0");
	    return ERROR;
	}
    }
    return FOUND;
}

static RstStat grnatRestore(FILE *f, int procno)
{
    int i, j, regs = (NGRS - 1)/64 + 1;

    for (i = 0; i < regs; i++) {
	if (!srs_nextRstVal(f, "%llx", &dwval))
	    return ERROR;
	for (j = 63; j >= 0; j--) {
	    if (!phyGrNatSet(procno, (i * 64 + j), dwval & 0x1)) {
		sprintf(errmsg, "Can't restore nonzero NaT value to GR0");
		return ERROR;
	    }
	    dwval >>= 1;
	}
    }
    return FOUND;
}

static RstStat frRestore(FILE *f, int procno)
{
    WORD sign, exp;
    DWORD mant;
    int i;

    for (i = 0; i < NFRS ; i++) {
	if (!srs_nextRstVal(f, "%x",   &sign) ||
	    !srs_nextRstVal(f, "%x",   &exp)  ||
	    !srs_nextRstVal(f, "%llx", &mant))
	    return ERROR;
	if (!phyFrSignSet(procno, i, sign) ||
	    !phyFrExpSet(procno, i, exp) ||
	    !phyFrMantSet(procno, i, mant)) {
	    sprintf(errmsg, "Illegal value assigned to Fr%d in line %d",
			    i, lineno);
	    return ERROR;
	}
    }
    return FOUND;
}

static RstStat prRestore(FILE *f, int procno)
{
    REG preds;
    int i;

    if (!srs_nextRstVal(f, "%llx", &preds))
	return ERROR;
    for (i = NPRS-1; i >= 0; i--) {
	if (!phyPrSet(procno, i, preds & 0x1)) {
	    sprintf(errmsg, "Illegal value assigned to Pr%d in line %d",
			    i, lineno);
	    return ERROR;
	}
	preds >>= 1;
    }
    return FOUND;
}

static RstStat brRestore(FILE *f, int procno)
{
    int i;

    for (i = 0; i < NBRS; i++) {
	if (!srs_nextRstVal(f, "%llx", &dwval))
	    return ERROR;
	brSet(procno, i, dwval);
    }
    return FOUND;
}

static RstStat arRestore(FILE *f, int procno)
{
    int i;

    for (i = 0; i < NARS; i++) {
	if (!srs_nextRstVal(f, "%llx", &dwval))
	    return ERROR;
	if (!arSet(procno, i, dwval)) {
	    sprintf(errmsg, "Illegal value restored to ar%d in line %d",
			     i, lineno);
	    return ERROR;
	}
    }
    return FOUND;
}

static RstStat cfmRestore(FILE *f, int procno)
{
    if (!srs_nextRstVal(f, "%llx", &dwval))
	return ERROR;
    cfmSet(procno, dwval);
    return FOUND;
}

static RstStat bolRestore(FILE *f, int procno)
{
    if (!srs_nextRstVal(f, "%d", &wval))
	return ERROR;
    bolSet(procno, wval);
    return FOUND;
}

static RstStat rseRestore(FILE *f, int procno)
{
    if (!srs_nextRstVal(f, "%d", &wval))
	return ERROR;
    dirtySet(procno, wval);

    if (!srs_nextRstVal(f, "%d", &wval))
	return ERROR;
    cleanSet(procno, wval);

    if (!srs_nextRstVal(f, "%d", &wval))
	return ERROR;
    invalidSet(procno, wval);

    if (!srs_nextRstVal(f, "%d", &wval))
	return ERROR;
    dirtyNatSet(procno, wval);

    if (!srs_nextRstVal(f, "%d", &wval))
	return ERROR;
    cleanNatSet(procno, wval);

    if (!srs_nextRstVal(f, "%d", &wval))
	return ERROR;
    cfleSet(procno, wval);

    return FOUND;
}

static RstStat crRestore(FILE *f, int procno)
{
    int i;

    for (i = 0; i < NCRS; i++) {
	if (!srs_nextRstVal(f, "%llx", &dwval))
	    return ERROR;
	if (!crSet(procno, i, dwval)) {
	    sprintf(errmsg, "Illegal value restored to cr%d in line %d",
			     i, lineno);
	    return ERROR;
	}
    }
    return FOUND;
}

static RstStat bkrRestore(FILE *f, int procno)
{
    int i;

    for (i = 0; i < NBGRS; i++) {
	if (!srs_nextRstVal(f, "%llx", &dwval))
	    return ERROR;
	bkrSet(procno, i, dwval);
    }
    return FOUND;
}

static RstStat bkrnatRestore(FILE *f, int procno)
{
    REG nats;
    int i;

    if (!srs_nextRstVal(f, "%llx", &nats))
	return ERROR;
    for (i = NBGRS-1; i >= 0; i--) {
	bkrNatSet(procno, i, nats & 0x1);
	nats >>= 1;
    }
    return FOUND;
}

static RstStat rrRestore(FILE *f, int procno)
{
    int i;

    for (i = 0; i < NRRS; i++) {
	if (!srs_nextRstVal(f, "%llx", &dwval))
	    return ERROR;
	rrSet(procno, i, dwval);
    }
    return FOUND;
}

static RstStat pkrRestore(FILE *f, int procno)
{
    int regsz, i;

    if (!srs_nextRstVal(f, "%d", &regsz))
	return ERROR;
    if (regsz > NPKRS) {
	sprintf(errmsg, "Illegal number of PKRs: must be <= %d (found %d)",
		NPKRS, regsz);
	return ERROR;
    }
    for (i = 0; i < regsz; i++) {
	if (!srs_nextRstVal(f, "%llx", &dwval))
	    return ERROR;
	pkrSet(procno, i, dwval);
    }
    return FOUND;
}

static RstStat dbrRestore(FILE *f, int procno)
{
    int regsz, i;

    if (!srs_nextRstVal(f, "%d", &regsz))
	return ERROR;
    if (regsz > NDBRS) {
	sprintf(errmsg, "Illegal number of DBRs: must be <= %d (found %d)",
		NDBRS, regsz);
	return ERROR;
    }
    for (i = 0; i < regsz; i++) {
	if (!srs_nextRstVal(f, "%llx", &dwval))
	    return ERROR;
	dbrSet(procno, i, dwval);
    }
    return FOUND;
}

static RstStat ibrRestore(FILE *f, int procno)
{
    int regsz, i;

    if (!srs_nextRstVal(f, "%d", &regsz))
	return ERROR;
    if (regsz > NIBRS) {
	sprintf(errmsg, "Illegal number of IBRs: must be <= %d (found %d)",
		NIBRS, regsz);
	return ERROR;
    }
    for (i = 0; i < regsz; i++) {
	if (!srs_nextRstVal(f, "%llx", &dwval))
	    return ERROR;
	ibrSet(procno, i, dwval);
    }
    return FOUND;
}

static RstStat pmcRestore(FILE *f, int procno)
{
    int regsz, i;

    if (!srs_nextRstVal(f, "%d", &regsz))
	return ERROR;
    if (regsz > NPMS) {
	sprintf(errmsg, "Illegal number of PMCs: must be <= %d (found %d)",
		NPMS, regsz);
	return ERROR;
    }
    for (i = 0; i < regsz; i++) {
	if (!srs_nextRstVal(f, "%llx", &dwval))
	    return ERROR;
	pmcSet(procno, i, dwval);
    }
    return FOUND;
}

static RstStat pmdRestore(FILE *f, int procno)
{
    int regsz, i;

    if (!srs_nextRstVal(f, "%d", &regsz))
	return ERROR;
    if (regsz > NPMS) {
	sprintf(errmsg, "Illegal number of PMDs: must be <= %d (found %d)",
		NPMS, regsz);
	return ERROR;
    }
    for (i = 0; i < regsz; i++) {
	if (!srs_nextRstVal(f, "%llx", &dwval))
	    return ERROR;
	pmdSet(procno, i, dwval);
    }
    return FOUND;
}

static RstStat dtrRestore(FILE *f, int procno)
{
    return NOT_FOUND;
}

static RstStat itrRestore(FILE *f, int procno)
{
    return NOT_FOUND;
}

static RstStat memRestore(FILE *f)
{
    REG adr;
    int bufsz, i;

    if (!srs_nextRstVal(f, "%llx", &adr) || !srs_nextRstVal(f, "%d", &bufsz))
	return ERROR;
    if (!bufsz) {		/* allocate an empty page */
	if (adr & 0xFFFULL)
	    return ERROR;
	memZerosPageSet(adr);
	return FOUND;
    }
    if (adr & 0x7ULL)
	return ERROR;
    for (i = 0; i < bufsz; i++) {
	if (!srs_nextRstVal(f, "%llx", &dwval))
	    return ERROR;
	memSet(adr + i*8, dwval);
    }
    return FOUND;
}


/*
 * Auxiliary Routines
 */

static void freeMemlist(Meminfo_p list)
{
    Meminfo_p p;

    while (list) {
	p = list;
	list = p->next;
	free(p);
    }
}

BOOL srs_nextRstVal(FILE *f, const char *format, ...)
{
    va_list ap;
    char tok[80];
    int ntstat;
    BOOL retstat = YES;

    if ((ntstat = nextToken(f, tok)) == EOF) {
	sprintf(errmsg, "Unexpected end of file");
	return NO;
    }
    va_start(ap, format);
    if (!ntstat || !vsscanf(tok, format, ap)) {
	sprintf(errmsg, "Unexpected token '%s' in line %d", tok, lineno);
	retstat = NO;
    }
    va_end(ap);
    return retstat;
}

static int nextToken(FILE *f, char *tok)
{
    char *cmt, tmp[80], line[160];
    int status;

    do {
	do {
	    if (fscanf(f, "%[ \t]", tmp) == EOF ||
		(status = fscanf(f, "%1[\n]", tmp)) == EOF)
		return EOF;
	    if (status > 0) {
		lineno++;
		sprintf(errmsg, "line %d", lineno);
	    }
	} while (status);

	if ((status = fscanf(f, "%s", tmp)) == EOF)
	    return EOF;

	if ((cmt = strchr(tmp, '#'))) {	   /* remove comments */
	    lineno++;
	    sprintf(errmsg, "line %d", lineno);
	    *cmt = '\0';
	    if (!fgets(line, sizeof line, f))
		return EOF;
	}
    } while (!*tmp);

    (void)strcpy(tok, tmp);
    return status;
}

char *srs_errmsgGet(void)
{
    return errmsg;
}

void srs_errmsgSet(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    (void)vsprintf(errmsg, fmt, ap);
    va_end(ap);
}
