/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Floating-Point Header
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
#ifndef _SKI_FLOAT_H
#define _SKI_FLOAT_H

#include "state.h"

#define FP_FLAGS	0x3F
#define FP_FAULT	0x3C0
#define FP_TRAP		0x3800
#define TD(x)		BitfX8(x,1,1)
#define SEXP(s,e)	((WORD)s<<17|e)
#define NATVAL(s,e,m)	(!s && e == 0x1FFFE && !m)

#define EXTRA_BIAS	0x1007C
#define INTEGER_EXP	(0x1003E + EXTRA_BIAS)

typedef enum {PC_DYN, PC_SGL, PC_DBL, PC_SIMD} FPPC;

/* XXX - should these be named FPCLASS_xxx? */
typedef enum {
    CLASS_NONE, CLASS_QNAN, CLASS_SNAN, CLASS_NAT, CLASS_INF, CLASS_UNSUPP,
    CLASS_DE0, CLASS_FR0 = 0xFF
} FPCLASS;

/* avoid collision with c99's fma() */
unsigned fma_(FREG *a, FREG *b, FREG *c, FREG *d, FPPC prec, BYTE traps,
	      BYTE controls);
unsigned fms(FREG *a, FREG *b, FREG *c, FREG *d, FPPC prec, BYTE traps,
	     BYTE controls);
unsigned fnma(FREG *a, FREG *b, FREG *c, FREG *d, FPPC prec, BYTE traps,
	      BYTE controls);
unsigned fcmpEq(const FREG *f1, const FREG *f2, BOOL qual, BOOL *res1,
		BOOL *res2, BYTE traps);
unsigned fcmpLt(const FREG *f1, const FREG *f2, BOOL qual, BOOL *res1,
		BOOL *res2, BYTE traps);
unsigned fcmpLe(const FREG *f1, const FREG *f2, BOOL qual, BOOL *res1,
		BOOL *res2, BYTE traps);
unsigned fcmpUnord(const FREG *f1, const FREG *f2, BOOL qual, BOOL *res1,
		   BOOL *res2, BYTE traps);
DWORD xmah(DWORD m1, DWORD m2, DWORD m3);
DWORD xmahu(DWORD m1, DWORD m2, DWORD m3);
void xmpy(FREG *f1, FREG *f2, WORD pos, int sign, FREG *ft);
unsigned frcpa(const FREG *num, const FREG *den, FREG *ft, BOOL *pt,
	       BYTE traps);
unsigned fprcpa(const FREG *num, const FREG *den, FREG *ft, BOOL *pt,
		BYTE traps);
unsigned frsqrta(const FREG *f1, FREG *ft, BOOL *pt, BYTE traps);
unsigned fprsqrta(const FREG *f1, FREG *ft, BOOL *pt, BYTE traps);
unsigned famax(FREG *f1, FREG *f2, FREG *ft, BYTE traps);
unsigned famin(FREG *f1, FREG *f2, FREG *ft, BYTE traps);
/* avoid collision with c99's fmin()/fmax() */
unsigned fmax_(FREG *f1, FREG *f2, FREG *ft, BYTE traps);
unsigned fmin_(FREG *f1, FREG *f2, FREG *ft, BYTE traps);
unsigned fcvtfx(const FREG *f1, FREG *ft, BYTE traps, BYTE controls);
unsigned fcvtfu(const FREG *f1, FREG *ft, BYTE traps, BYTE controls);
unsigned fpcvtfx(const FREG *f1, FREG *ft, BYTE traps, BYTE controls);
unsigned fpcvtfu(const FREG *f1, FREG *ft, BYTE traps, BYTE controls);
BOOL fclass(const FREG *f1, DWORD cl);

#endif	/* _SKI_FLOAT_H */
