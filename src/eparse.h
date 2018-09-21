/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     LOR = 258,
     LAND = 259,
     NE = 260,
     EQ = 261,
     GE = 262,
     LE = 263,
     SHR = 264,
     SHL = 265,
     UNARY = 266,
     ISYM = 267,
     SYM = 268,
     NOBASE = 269,
     NPFXHEX = 270,
     BIN = 271,
     OCT = 272,
     DEC = 273,
     HEX = 274,
     UKNWN = 275
   };
#endif
/* Tokens.  */
#define LOR 258
#define LAND 259
#define NE 260
#define EQ 261
#define GE 262
#define LE 263
#define SHR 264
#define SHL 265
#define UNARY 266
#define ISYM 267
#define SYM 268
#define NOBASE 269
#define NPFXHEX 270
#define BIN 271
#define OCT 272
#define DEC 273
#define HEX 274
#define UKNWN 275




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 42 "eparse.y"
{
	    char *str;
	    REG val;
	}
/* Line 1489 of yacc.c.  */
#line 94 "eparse.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

