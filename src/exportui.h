/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Exported User Interface Header
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
#ifndef _SKI_EXPORTUI_H
#define _SKI_EXPORTUI_H

#include "std.h"
#include "types.h"

/*--------------------------------------------------------------------------
 *  Interface to expression evaluation
 *-------------------------------------------------------------------------*/

BOOL evalExpr(const char *expr, unsigned base, REG *retval);
/*
    char *expr		Expression.  Must not have embedded spaces.
    unsigned base	Default base for constants: 10 or 16.
    REG *retval		Evaluated expression.
    (retval)		Did the expression evaluation succeed?
 */


/*--------------------------------------------------------------------------
 *  Interface to internal symbol registration
 *-------------------------------------------------------------------------*/

typedef enum {
    SINGLE_SYM,
    REGSET_SYM,
    BITF_SYM,
    RS_BITF_SYM,
    REG_SYM,			/* generic 64-bit data items */
    BYTE_SYM,			/* generic 8-bit data items */
    BOOL_SYM,			/* boolean data items */
    STR_SYM,			/* string data items */
    IP_SYM,			/* IP */
    PSR_SYM,			/* PSR */
    GR_SYM,			/* GRs */
    GRNAT_SYM,			/* GR Nats */
    PR_SYM,			/* PRs */
#if 0
    FR_SYM,			/* FRs */
#endif
    FRMANT_SYM,			/* FR mantissas */
    FREXP_SYM,			/* FR exponents */
    FRSGN_SYM,			/* FR signs */
    RRBGF_SYM,			/* rrbg and rrbf */
    RRBP_SYM			/* rrbp */
} Symtyp;

void isymIns(const char *sname, void *pval, Symtyp type, BOOL ronly, int len,
	     int start, REG align, int dbase);
/*
    const char *sname	symbol name
    void *pval		pointer to the symbol data item
    Symtyp type		symbol type
    BOOL ronly		is the symbol read-only?
    int len		symbol bit field size extending to the right of
			"start" (if 0, symbol is not a subfield)
    int start		symbol bit-field starting position in little-endian
			notation (ignored if "len" is 0)
    REG align		symbol alignment (if 1, no align restriction)
    int dbase		default base for "=" cmd: 10 or 16
 */


/*--------------------------------------------------------------------------
 *  Interface to command registration
 *-------------------------------------------------------------------------*/

typedef BOOL	(*PFV)(unsigned argc, char **argv);
/*
    argc		command argument count
    argv		command arguments
    (retval)		error detected when running the command?
 */

extern FILE *cmdFile;	/* handle to the "." (command) file */

BOOL menuIns(const char *keywd, int minargs, int maxargs, const char *descrip,
	     PFV fcn, const char *format);
/*
    char *keywd		command name
    int minargs		command minimum arguments
    int maxargs		command maximum arguments
    char *descrip	command description (needed for the help cmd)
    PFV fcn		command function
    char *format	command format (needed for the help cmd)
    (retval)		call succeeded
 */


/*--------------------------------------------------------------------------
 *  Interface to register window registration and use
 *-------------------------------------------------------------------------*/

typedef char *	(*PLF)(unsigned line);
/*
    line		register window line to display
    (retval)		pointer to a buffer containing the line to display
 */

BOOL regwIns(const char *tag, const char *title, PLF fcn, const char *size);
/*
    char *tag		registered window name.
			Ski creates two X resources: "show<tag>" and
			"regw<tag>" which can be used to control the register
			window display in X
    char *title		register window title
    PLF fcn		function that returns a register window line
    char *size		expression for the register window size
    (retval)		call succeeded
 */

BOOL regwMakeActivew(const char *tag);
/*
    char *tag		registered window name
    (retval)		was the register window made active?
 */


/*--------------------------------------------------------------------------
 *  Interface to data window registration and use
 *-------------------------------------------------------------------------*/

typedef char *	(*PBF)(unsigned datwSz);	/* ptr to buffer display fcn */
/*
    datwSz		available data window size
    (retval)		pointer to the display buffer
 */

BOOL datwIns(const char *tag, const char *title, PBF bdfcn);
/*
    char *tag		registered data window name
			Ski creates the X resources: "show<tag>" which can
			be used to control the data window display in X (see
			the apps-default file)
    char *title		data window title
    PBF bdfcn		function that returns a data window buffer
    (retval)		call succeeded
 */

BOOL datwMakeActivew(const char *tag);
/*
    char *tag		registered data window name
    (retval)		was the data window made active?
 */

unsigned datwSz(const char *tag);
/*
    char *tag		registered data window name
    (retval)		data window size
 */


/*--------------------------------------------------------------------------
 *  Interface to command line option registration
 *-------------------------------------------------------------------------*/

#define IFACELEN	4
#define ARGSIZ		30

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

/* return -1 for unrecognized option, 0 for BOOL option, 1 for the rest */
extern int lookupOption(char *argname, char *argval);

/* return index of first argument after options or -1 on failure */
extern int parseOptions(int argc, char *argv[]);

/* display all known options */
extern void displayOptions(void);

#endif	/* _SKI_EXPORTUI_H */
