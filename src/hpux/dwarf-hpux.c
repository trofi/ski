/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator File DWARF Support Routines
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

#include "std.h"
#include "types.h"
#include "exportui.h"
#include "coreui.h"
#include "program.h"

#define FNAME_LEN	54
#define EMPTY_FILENAME	"???"
#define NOT_A_LINE	0xffffffff

#if defined HAVE_DWARF_SUPPORT

#include <ftypes.h>
#include <lib_dw2.h>

/*##################### Datatypes ##########################################*/

#if 0
typedef struct {
    tgt_addr_t	adr;
    uint8	slot;
    uint32	line;
    uint32	col;
    uint32	finx;
} DwLineRow_t;
#endif

typedef struct {
    int		unit;
    char	*name;
    BOOL	prnerr;
    tgt_addr_t	low_pc, high_pc;
    Dw_Line_Table  *dwt;
    char	**files;
#if 0
    DwLineRow_t	*dwrow;
#endif
} DwUnitInfo_t;


/*##################### Globals - Imports ##################################*/

extern PrgwMode prgwm;
extern unsigned cproc;

/*##################### Globals - Exports ##################################*/

/*##################### Local Variables ####################################*/

static BOOL dwInfo;
static DwUnitInfo_t *unitTbl;
static unsigned nunits = 0;
static err_t err;
static Dw_Unit *gdwu;

/*##################### Functions ##########################################*/

static void dwerr(char *msg)
{
   /* XXX - cmdErr() fails in X when the interface is not up yet. */
    if (interface == CURSES_INTERFACE)
	cmdErr("%s: %s\n", msg, Dw_Error_message(err));
}

static int cmpDUaddr(const void *elm1, const void *elm2)
{
    const DwUnitInfo_t *dwu1 = elm1;
    const DwUnitInfo_t *dwu2 = elm2;

    if (dwu1->low_pc == dwu2->low_pc)
	return 0;
    if (dwu1->low_pc < dwu2->low_pc)
	return -1;
    if (dwu1->low_pc > dwu2->low_pc)
	return 1;
    /*NOTREACHED*/
}

static unsigned getLines(ADDR ofs, DwUnitInfo_t *unit, char *srcln)
{
    Dw_Line_Table_Row dwrow;
    Dw_File_Info dwfi;
    unsigned i, startln, prevln = 1, count = 0;
    char prevfn[80] = "";
    bool tblok;
    char *p = srcln;
    FILE *fp;

    unit->dwt = Dw_Line_Table_construct_empty(&err);
    Dw_Unit_assign_line_table(gdwu, unit->dwt, &err);
    Dw_Line_Table_reset(unit->dwt, &err);
    if (err) {
	dwerr("Reset Line Table");
	return 0;
    }
    do {
	tblok = Dw_Line_Table_assign_next_row(unit->dwt, &dwrow, &err);
	if (err) {
	    dwerr("Get next table row");
	    break;
	}
	dwfi = Dw_Line_Table_file(unit->dwt, dwrow.file_info_index, &err);
	if (err) {
	    dwerr("Get line table file");
	    break;
	}
	if (strcmp(prevfn, dwfi.name)) {
	    (void)strcpy(prevfn, dwfi.name);
	    prevln = 1;
	}
	if (ofs == dwrow.address) {
	    if (!(fp = fopen(dwfi.name, "r"))) {
		if (unit->prnerr) {
		    cmdErr("Couldn't open file '%s'.  Was ski "
			   "started in the right directory?\n",
			   dwfi.name);
		    unit->prnerr = NO;
		}
		break;
	    }
	    if ((dwrow.line_num - prevln + 1) > prgRows/3) {
		startln = dwrow.line_num - prgRows/3 + 2;
		p += sprintf(p, "%03d ...\n", startln-1);
		count = 1;
	    } else {
		startln = prevln;
		count = 0;
	    }
	    for (i = 1; i <= dwrow.line_num; i++) {
		char tmpbuf[BUFSIZ];

		if (fgets(tmpbuf, 160, fp) == NULL) {
		    cmdErr("Error reading '%s' line: %d\n",
			   dwfi.name, i);
		    break;
		}
		if (i >= startln && i <= dwrow.line_num) {
		    if (interface == CURSES_INTERFACE) {
			tmpbuf[75] = '\n';	/* to fit in curses window */
			tmpbuf[76] = '\0';
		    }
		    p += sprintf(p, "%03d %s", i, tmpbuf);
		    count++;
		}
	    }
	    (void)fclose(fp);
	}
	prevln = dwrow.line_num + 1;
	if (!tblok)				/* end of table */
	    break;
    } while (ofs > dwrow.address);

    Dw_Line_Table_reset(unit->dwt, &err);
    if (err) {
	dwerr("Reset Line Table");
	srcln[0] = '\0';
	count = 0;
    }
    return count;
}

static void getName(ADDR ofs, DwUnitInfo_t *unit, char *fname)
{
    Dw_Line_Table_Row dwrow;
    Dw_File_Info dwfi;
    bool tblok;

    fname[0] = '\0';
    unit->dwt = Dw_Line_Table_construct_empty(&err);
    Dw_Unit_assign_line_table(gdwu, unit->dwt, &err);
    Dw_Line_Table_reset(unit->dwt, &err);
    if (err) {
	dwerr("Reset Line Table");
	fname[0] = '\0';
    }
    do {
	tblok = Dw_Line_Table_assign_next_row(unit->dwt, &dwrow, &err);
	if (err) {
	    dwerr("Get next table row");
	    break;
	}
	dwfi = Dw_Line_Table_file(unit->dwt, dwrow.file_info_index, &err);
	if (err) {
	    dwerr("Get line table file");
	    break;
	}
	if (ofs <= dwrow.address)
	    (void)strcpy(fname, dwfi.name);
	if (!tblok)				/* end of table */
	    break;
    } while (ofs > dwrow.address);
    Dw_Line_Table_reset(unit->dwt, &err);
    if (err) {
	dwerr("Reset Line Table");
	fname[0] = '\0';
    }
}

void initDwUnitTbl(int fd)
{
    Dw_Image *dwi;
    Dw_Unit *dwu;
    unsigned i;

    dwInfo = NO;
#if 0
    return;
#endif

    dwi = Dw_Image_construct_from_elf(fd, &err);
    if (err) {
#if 0
/* XXX - can we distinguish real errors from just not having a DWARF section? */
	dwerr("Construct DWARF image");
#endif
	return;
    }

    nunits = Dw_Image_get_num_units(dwi, &err);
    if (err) {
	dwerr("Get # of units");
	return;
    }

    unitTbl = malloc(nunits * sizeof *unitTbl);

    for (i = 0; i < nunits; i++) {
	dwu = Dw_Image_get_unit(dwi, i, &err);
gdwu = dwu;
	if (err) {
	    dwerr("Get Unit");
	    return;
	}
	unitTbl[i].unit = i;
	unitTbl[i].name = Dw_Unit_get_name(dwu, &err);
	if (err) {
	    dwerr("Get unit name");
	    return;
	}
	unitTbl[i].prnerr = YES;
	unitTbl[i].low_pc = Dw_Unit_get_low_pc(dwu, &err);
	if (err) {
	    dwerr("Get unit low_pc");
	    return;
	}
	unitTbl[i].high_pc = Dw_Unit_get_high_pc(dwu, &err);
	if (err) {
	    dwerr("Get unit high_pc");
	    return;
	}
	unitTbl[i].dwt = Dw_Line_Table_construct_empty(&err);
	if (err) {
	    dwerr("Construct empty line table");
	    Dw_Line_Table_destroy(unitTbl[i].dwt);
	    return;
	}
	Dw_Unit_assign_line_table(dwu, unitTbl[i].dwt, &err);
	if (err) {
	    dwerr("Assign unit to line table");
	    Dw_Line_Table_destroy(unitTbl[i].dwt);
	    return;
	}
    }

    /* Sort unitTbl to allow binary searches */
    qsort(unitTbl, nunits, sizeof *unitTbl, cmpDUaddr);
    dwInfo = YES;
}

/*------------------------------------------------------------------------
 * This routine returns the source lines that precede the executable address
 *  given and the number of lines is returned.  The lines are returned in a
 *  statically allocated buffer.
 *
 *  The routine has a caveat:
 *    -	The routine returns a pointer to a static buffer of fixed size.
 *	There is always the possibility that the size of the source lines
 *	returned exceeds this size (causing havoc!).  Change to use malloc?
 *
 *  And possibilities to improve:
 *    - The search algorithms are linear.  The performance of these
 *	algorithms can be improved by making them binary searches.
 *
 *    -	The line table is always accessed from the DWARF data structures.
 *	The performance can be improved by caching the line table.
 *-----------------------------------------------------------------------*/
char *getSrcLines(ADDR ofs, unsigned *count)
{
    unsigned i;
    static char srcln[80 * 25];		/* 25 lines, 80 char/line */

    *count = 0;
    srcln[0] = '\0';
    if (dwInfo && prgwm != ASM)
	for (i = 0; i < nunits; i++)
	    if (ofs >= unitTbl[i].low_pc && ofs < unitTbl[i].high_pc)
		*count = getLines(ofs, &unitTbl[i], srcln);
    return srcln;
}

char *getSrcName(ADDR ofs)
{
    unsigned i;
    static char fname[80];

    fname[0] = '\0';
    if (dwInfo && prgwm != ASM)
	for (i = 0; i < nunits; i++)
	    if (ofs >= unitTbl[i].low_pc && ofs < unitTbl[i].high_pc)
		getName(ofs, &unitTbl[i], fname);
    return fname;
}

#else /* !defined HAVE_DWARF_SUPPORT */

char *
getSrcLines (ADDR ofs, unsigned *count)
{
  *count = 0;
  return "";
}

char *
getSrcName (ADDR ofs)
{
  static char fname[FNAME_LEN];
  strcpy (fname, EMPTY_FILENAME);
  return fname;
}

void
initDwUnitTbl (int fd)
{
}

#endif /* !defined HAVE_DWARF_SUPPORT */
