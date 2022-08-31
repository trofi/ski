%{
/*
 * Simulator Expression Scanner
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

#include "std.h"
#include "types.h"
#include "libsym.h"
#include "coreui.h"
#include "eparse.h"

char *expptr;

#if defined FLEX_SCANNER

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) { \
    int c = *expptr++; \
    result = ( c == 0 ) ? YY_NULL : ( buf[ 0 ] = c, 1 ); \
}
#define YY_NO_UNPUT

#else /* !FLEX_SCANNER */

#undef input
#undef unput
#define input() (*expptr++)
#define unput(c) (*--expptr = c)

#endif /* !FLEX_SCANNER */

static struct isym *symptr;
static ADDR ofs;

static REG getVal(struct isym *psym);

%}
%%

[ \t]+ {
    ; /* skip blanks and tabs */
}

(0b|0B)[01]+ {
    yylval.str = &yytext[2];
    return BIN;
}

(0o|0O)[0-7]+ {
    yylval.str = &yytext[2];
    return OCT;
}

(0d|0D)[0-9]+ {
    yylval.str = &yytext[2];
    return DEC;
}

(0x|0X)[0-9a-fA-F]+ {
    yylval.str = &yytext[2];
    return HEX;
}

[0-9]+ {
    yylval.str = yytext;
    return NOBASE;
}

[a-fA-F][0-9a-fA-F]* {
    if ((symptr = isymVLkp((char *)yytext)) != NULL) {
	yylval.val = getVal(symptr);
	return ISYM;
    } else if (symNametoAddr((char *)yytext, &ofs)) {
	yylval.val = ofs;
	return SYM;
    } else {
	yylval.str = yytext;
	return NPFXHEX;
    }
}

[a-zA-Z$_?.][a-zA-Z$_0-9?.@]* {
    if ((symptr = isymVLkp((char *)yytext)) != NULL) {
	yylval.val = getVal(symptr);
	return ISYM;
    } else if (symNametoAddr((char *)yytext, &ofs)) {
	yylval.val = ofs;
	return SYM;
    } else {
	yylval.str = yytext;
	cmdErr("Unrecognized symbol name: %s\n", yytext);
	return UKNWN;
    }
}

[0-9a-fA-F]+ {
    yylval.str = yytext;
    return NPFXHEX;
}

"<<" {
    return SHL;
}

">>" {
    return SHR;
}

"<=" {
    return LE;
}

">=" {
    return GE;
}

"==" {
    return EQ;
}

"!=" {
    return NE;
}

"&&" {
    return LAND;
}

"||" {
    return LOR;
}

\n {
    return '\n';
}

. {
    return yytext[0];
}

%%

int yywrap() { return 1; }

/* XXX - copied from ssDCmd.h */
#ifdef NEW_MP
extern unsigned viewPid;
#else
#define viewPid 0
#endif
static REG getVal(struct isym *psym)
{
    switch (psym->type) {
	case SINGLE_SYM:
		return psym->getFn(viewPid);
	case REGSET_SYM:
		return psym->getFn(viewPid, psym->ndx);
	case BITF_SYM:
		return (psym->getFn(viewPid) << (63 - psym->start)) >> (64 - psym->len);
	case RS_BITF_SYM:
		return (psym->getFn(viewPid, psym->ndx) << (63 - psym->start)) >> (64 - psym->len);
    default:
        return 0;
    }
    /*NOTREACHED*/
}
