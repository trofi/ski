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

/*--------------------------------------------------------------------------
 *  Interface to register window registration and use
 *-------------------------------------------------------------------------*/

typedef char *	(*PLF)(unsigned line);
/*
    line		register window line to display
    (retval)		pointer to a buffer containing the line to display
 */

/*--------------------------------------------------------------------------
 *  Interface to data window registration and use
 *-------------------------------------------------------------------------*/

typedef char *	(*PBF)(unsigned datwSz);	/* ptr to buffer display fcn */
/*
    datwSz		available data window size
    (retval)		pointer to the display buffer
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

/* return index of first argument after options or -1 on failure */
extern int parseOptions(int argc, char *argv[]);

/* display all known options */
extern void displayOptions(void);

#endif	/* _SKI_EXPORTUI_H */
