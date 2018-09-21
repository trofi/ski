/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Bit Manipulation Macros
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
#ifndef _SKI_BITS_H
#define _SKI_BITS_H

#if !BYTE_ORDER || !BIG_ENDIAN || !LITTLE_ENDIAN
# error "Endianness is unknown!"
#endif

/*---------------------------------------------------------------------------
 * Bitfield$Reference - Reference a specified bitfield within a 32-bit or
 *  64-bit integer.  The argument "i" must be an addressable variable (i.e.,
 *  not a register variable or an expression... but see BitfX below), "s" is
 *  the starting bit (big-endian, leftmost) and "l" is the length; "s" and "l"
 *  must be constants.  BitfR may be used as an lvalue or an rvalue.
 *  Translates to a single instruction on PA-RISC, VAX, and DEC-20.
 *---------------------------------------------------------------------------*/

/* eclipse - previous support for GNUC was only enabled for
 * LITTLE_ENDIAN systems. I generalized this support so that we can
 * use gcc on HP-UX. However, even this fix was not enough to prevent
 * the gcc-hppa optimizer from reordering memory instructions, so I
 * added the volatile keyword in the GNUC-BIG_ENDIAN case. */
#if defined __GNUC__
/* If used, will cause unsat symbols */
extern int bad_bitfr (void);
extern int bad_bitfr32 (void);
extern int bad_bitfr8 (void);

/*
 * Wrapping the "struct"s inside a "union" tells gcc not to do
 * type-based alias-analysis, which could cause the BitfR construct to
 * fail, because (from Jim Wilson):
 *
 *	ISO C says that if you access an object using a type other
 *	than what the object was declared with, then the result is
 *	undefined.  There are a few exceptions, signedness doesn't
 *	matter, qualifiers don't matter, aggregate types containing
 *	the type are OK, and char is OK.  (In the 1989 ANSI C
 *	standard, this is in section 3.3 Expressions.)
 *
 * Fortunately, gcc (in "violation" of ANSI C) has another exception:
 * if the type is inside a union, no type-based alias analysis is
 * done.
 */
# if BYTE_ORDER == BIG_ENDIAN
#define BitfR(i,s,l)	(((union {volatile struct {               \
				unsigned long long  : s; \
				unsigned long long f: l; \
			        } u;                     \
			  } *)(&i + (sizeof(i) != 8 ? bad_bitfr() : 0)))->u.f)
#define BitfR32(i,s,l)	(((union {volatile struct {         \
				unsigned int  : s; \
				unsigned int f: l; \
				} u;               \
			  } *)(&i + (sizeof(i) != 4 ? bad_bitfr32() : 0)))->u.f)
#define BitfR8(i,s,l)	(((union {volatile struct {          \
				unsigned char  : s; \
				unsigned char f: l; \
				} u;                \
			  } *)(&i + (sizeof(i) != 1 ? bad_bitfr8() : 0)))->u.f)
# else /* ! BYTE_ORDER == BIG_ENDIAN */
#define BitfR(i,s,l)	(((union {					       \
				 struct {				       \
					 unsigned long long  : 64-(l)-(s);     \
					 unsigned long long f: l;	       \
				 } u;					       \
			 } *)(&i + (sizeof(i) != 8 ? bad_bitfr () : 0)))->u.f)
#define BitfR32(i,s,l)	(((union {					\
				 struct {				\
					 unsigned int  : 32-(l)-(s);	\
					 unsigned int f: l;		\
				 } u;					\
			 } *)(&i + (sizeof(i) != 4 ? bad_bitfr32 () : 0)))->u.f)
#define BitfR8(i,s,l)	(((union {					\
				 struct {				\
					 unsigned char  : 8-(l)-(s);	\
					 unsigned char f: l;		\
				 } u;					\
			 } *)(&i + (sizeof(i) != 1 ? bad_bitfr8 () : 0)))->u.f)
# endif /* !BYTE_ORDER == BIG_ENDIAN */
#else /* !defined __GNUC__ */
# if BYTE_ORDER == BIG_ENDIAN
#define BitfR(i,s,l)	(((struct { \
				unsigned long long  : s; \
				unsigned long long f: l; \
			   } *)&i)->f)
#define BitfR32(i,s,l)	(((struct { \
				unsigned int  : s; \
				unsigned int f: l; \
			   } *)&i)->f)
#define BitfR8(i,s,l)	(((struct { \
				unsigned char  : s; \
				unsigned char f: l; \
			   } *)&i)->f)
# else /* ! BYTE_ORDER == BIG_ENDIAN */
#define BitfR(i,s,l)	((((struct {					\
				unsigned long long  : 64-(l)-(s);	\
				unsigned long long f: l;		\
			 } *)&i)->f))
#define BitfR32(i,s,l)	((((struct {				\
				unsigned int  : 32-(l)-(s);	\
				unsigned int f: l;		\
			   } *)&i)->f))
#define BitfR8(i,s,l)	((((struct {				\
				unsigned char  : 8-(l)-(s);	\
				unsigned char f: l;		\
			   } *)&i)->f))
# endif /* !BYTE_ORDER == BIG_ENDIAN */
#endif /* !defined __GNUC__ */


/*---------------------------------------------------------------------------
 * Bitfield$eXtract - Extract the specified bitfield from a 32-bit or 64-bit
 *  integer.  The "s" and "l" arguments specify the starting bit and length,
 *  however they need no longer be constants.  May only be used as an rvalue.
 *  Translates to two instructions on the VAX, three on the DEC-20, six on
 *  PA-RISC.
 *---------------------------------------------------------------------------*/
#if defined __GNUC__
extern unsigned long long bad_bitfx64 (void);
extern unsigned int       bad_bitfx32 (void);
extern unsigned short     bad_bitfx16 (void);
extern unsigned char      bad_bitfx8 (void);

#define BitfX64(i,s,l) (((unsigned long long)((i) << (s)) >> (64 - (l))) \
		| (sizeof(i) != 8 ? bad_bitfx64 () : 0))
#define BitfX32(i,s,l) (((unsigned int)      ((i) << (s)) >> (32 - (l))) \
		| (sizeof(i) != 4 ? bad_bitfx32 () : 0))
#define BitfX16(i,s,l) (((unsigned short)    ((i) << (s)) >> (16 - (l))) \
		| (sizeof(i) != 2 ? bad_bitfx16 () : 0))
#define BitfX8(i,s,l)  (((unsigned char)     ((i) << (s)) >> ( 8 - (l))) \
		| (sizeof(i) != 1 ? bad_bitfx8  () : 0))

#else /* !defined __GNUC__ */

#define BitfX64(i,s,l) ((unsigned long long)((i) << (s)) >> (64 - (l)))
#define BitfX32(i,s,l) ((unsigned int)      ((i) << (s)) >> (32 - (l)))
#define BitfX16(i,s,l) ((unsigned short)    ((i) << (s)) >> (16 - (l)))
#define BitfX8(i,s,l)  ((unsigned char)     ((i) << (s)) >> ( 8 - (l)))

#endif /* !defined __GNUC__ */

/* set the default to BitfX64 */
#define BitfX(i,s,l) BitfX64((i),(s),(l))

#endif	/* _SKI_BITS_H */
