%{
/*
 * Simulator Expression Parser
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
#include "memui.h"
#include "exportui.h"
#include "coreui.h"

REG  expval;
ADDR thash(ADDR);

extern	char *yytext;
extern	int  yylineno;
extern int yylex (void );
static REG convert(char);
static REG rd8(REG);
static REG strtonum(char *, int);
static void yyerror(char *);

%}

%union	{
	    char *str;
	    REG val;
	}

%left   <str> ','
%right  <str> '='
%left   <str> LOR
%left   <str> LAND
%left   <str> '|'
%left   <str> '^'
%left   <str> '&'
%left   <str> EQ   NE
%left   <str> '<'   LE   '>'   GE
%left   <str> SHL  SHR
%left   <str> '+'  '-'
%left   <str> '*'  '/'  '%'
%left   <str> UNARY

%token	<val> ISYM
%token	<val> SYM
%token  <str> NOBASE
%token  <str> NPFXHEX
%token  <str> BIN
%token  <str> OCT
%token  <str> DEC
%token  <str> HEX
%token	UKNWN

%type   <val> cntexp
%type   <val> numexp
%type   <val> count
%type   <val> number
%type   <val> based

%%
expr	: '!' cntexp		{ expval = $2; }
	| '#' numexp		{ expval = $2; }

cntexp  : count                 { $$ = $1; }
	| ISYM			{ $$ = $1; }
	| SYM			{ $$ = $1; }
        | '(' cntexp ')'        { $$ = $2; }
        | '*' cntexp    %prec UNARY     { $$ = rd8($2); }
        | '-' cntexp    %prec UNARY     { $$ = -$2; }
        | '+' cntexp    %prec UNARY     { $$ = $2; }
        | '~' cntexp    %prec UNARY     { $$ = ~$2; }
        | '!' cntexp    %prec UNARY     { $$ = !$2; }
        | cntexp '*' cntexp     { $$ = $1 * $3; }
        | cntexp '/' cntexp     { if ($3)
				      $$ = $1 / $3;
				  else {
				      yyerror("division by zero");
				      YYABORT;
				  }
				}
        | cntexp '%' cntexp     { if ($3)
				      $$ = $1 % $3;
				  else {
				      yyerror("division by zero");
				      YYABORT;
				  }
				}
        | cntexp '+' cntexp     { $$ = $1 + $3; }
        | cntexp '-' cntexp     { $$ = $1 - $3; }
        | cntexp SHL cntexp     { $$ = $1 << $3; }
        | cntexp SHR cntexp     { $$ = $1 >> $3; }
        | cntexp '<' cntexp     { $$ = $1 < $3; }
        | cntexp LE cntexp      { $$ = $1 <= $3; }
        | cntexp '>' cntexp     { $$ = $1 > $3; }
        | cntexp GE cntexp      { $$ = $1 >= $3; }
        | cntexp EQ cntexp      { $$ = $1 == $3; }
        | cntexp NE cntexp      { $$ = $1 != $3; }
        | cntexp '&' cntexp     { $$ = $1 & $3; }
        | cntexp '^' cntexp     { $$ = $1 ^ $3; }
        | cntexp '|' cntexp     { $$ = $1 | $3; }
        | cntexp LAND cntexp    { $$ = $1 && $3; }
        | cntexp LOR cntexp     { $$ = $1 || $3; }
        | cntexp ',' cntexp     { $$ = $3; }
        ;

numexp  : number                { $$ = $1; }
	| ISYM			{ $$ = $1; }
	| SYM			{ $$ = $1; }
        | '(' numexp ')'        { $$ = $2; }
        | '*' numexp    %prec UNARY     { $$ = rd8($2); }
        | '-' numexp    %prec UNARY     { $$ = -$2; }
        | '+' numexp    %prec UNARY     { $$ = $2; }
        | '~' numexp    %prec UNARY     { $$ = ~$2; }
        | '!' numexp    %prec UNARY     { $$ = !$2; }
        | '@' numexp    %prec UNARY     { $$ = thash($2); }
        | numexp '*' numexp     { $$ = $1 * $3; }
        | numexp '/' numexp     { if ($3)
				      $$ = $1 / $3;
				  else {
				      yyerror("division by zero");
				      YYABORT;
				  }
				}
        | numexp '%' numexp     { if ($3)
				      $$ = $1 % $3;
				  else {
				      yyerror("division by zero");
				      YYABORT;
				  }
				}
        | numexp '+' numexp     { $$ = $1 + $3; }
        | numexp '-' numexp     { $$ = $1 - $3; }
        | numexp SHL numexp     { $$ = $1 << $3; }
        | numexp SHR numexp     { $$ = $1 >> $3; }
        | numexp '<' numexp     { $$ = $1 < $3; }
        | numexp LE numexp      { $$ = $1 <= $3; }
        | numexp '>' numexp     { $$ = $1 > $3; }
        | numexp GE numexp      { $$ = $1 >= $3; }
        | numexp EQ numexp      { $$ = $1 == $3; }
        | numexp NE numexp      { $$ = $1 != $3; }
        | numexp '&' numexp     { $$ = $1 & $3; }
        | numexp '^' numexp     { $$ = $1 ^ $3; }
        | numexp '|' numexp     { $$ = $1 | $3; }
        | numexp LAND numexp    { $$ = $1 && $3; }
        | numexp LOR numexp     { $$ = $1 || $3; }
        | numexp ',' numexp     { $$ = $3; }
        ;

count   : based
        | NOBASE                { $$ = strtonum($1, 10); }
        ;

number  : based
        | NOBASE                { $$ = strtonum($1, 16); }
        | NPFXHEX               { $$ = strtonum($1, 16); }
        ;

based   : BIN                   { $$ = strtonum($1, 2); }
        | OCT                   { $$ = strtonum($1, 8); }
        | DEC                   { $$ = strtonum($1, 10); }
        | HEX                   { $$ = strtonum($1, 16); }
        ;

%%

static REG convert(char c)
{
    if (c >= '0' && c <= '9')
	return c - '0';
    else if (c >= 'a' && c <= 'f')
	return c - 'a' + 10;
    else /* (c >= 'A' && c <= 'F') */
	return c - 'A' + 10;
}

static REG rd8(REG addr)
{
    DWORD dval;

    return memMRd(0, addr, 8, &dval) ? dval : 0;
}

static REG strtonum(char *str, int base)
{
    REG num = 0;

    while (*str != '\0') {
	num = num * base + convert(*str);
	str++;
    }
    return num;
}

static void yyerror(char *s)
{
    cmdErr("%s\n", s);
}
