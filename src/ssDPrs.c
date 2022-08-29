/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Debugger Input Parser Routines
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
#include "std.h"
#include "types.h"
#include "exportui.h"
#include "coreui.h"
#include "ssDCmd.h"

#define ARGLEN 4000

extern char *expptr;
extern REG expval;
int yyparse(void);

/*--------------------------------------------------------------------------
 * evaluate$Expression - Evaluate the arithmetic expression pointed at by
 *  the 'expr' string and return the expression value in 'retval'.  Numbers
 *  without a base prefix are assumed to be of base 'base'.  Return NO if
 *  the expression is illegal or longer than ARGLEN characters.
 *--------------------------------------------------------------------------*/
BOOL evalExpr(const char *expr, unsigned base, REG *retval)
{
    char expstr[ARGLEN+2];

    if (strlen(expr) >= ARGLEN) {
	cmdErr("More than %d characters in expression: %.10s...\n",
	       ARGLEN, expr);
	return NO;
    }
    if (base == DECEXP)
	(void)sprintf(expstr, "! %s", expr);
    else
	(void)sprintf(expstr, "# %s", expr);
    expptr = expstr;
    if (!yyparse()) {
	*retval = expval;
	return YES;
    } else {
	/* XXX - this will fail for long expressions since the cmdErr string
		 can only be 100 total chars */
	cmdErr("Illegal expression: %s\n", expr);
	return NO;
    }
}

/*--------------------------------------------------------------------------
 * catenate$Arguments - concatenate the arguments passed in 'argv' into one
 *  string and return then in the buffer passed.  Begin with argument at
 *  index 'start' and finish with argument at index 'end'.
 *--------------------------------------------------------------------------*/
void catArgs(unsigned start, unsigned end, char *argv[], char *buf)
{
    unsigned i;

    *buf = '\0';
    for (i = start; i <= end; i++) {
	(void)strcat(buf, " ");
	(void)strcat(buf, argv[i]);
    }
}
