/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Floating-Point Routines
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

#include "std.h"
#include "bits.h"
#include "types.h"
#include "fields.h"	/* for SIGN() */
#include "state.h"	/* for FREG */
#include "float.h"
#include "sim.h"	/* for ONES() */

FREG fill2freg(BYTE sign, WORD exp, DWORD mant);

/*
 * The flags/ISR settings as a result of an FP operation is indicated in
 * the return value from functions like "fma" as follows:
 *
 *   Bits  5:0  -- FPSR flags (IUOZDV)
 *   Bits  9:6  -- ISR faults (SWA,Z,D,V)
 *   Bits 14:11 -- ISR traps  (FPA,I,U,O)
 */

#define I_MASK	0x20
#define U_MASK	0x10
#define O_MASK	0x08
#define Z_MASK	0x04
#define D_MASK	0x02
#define V_MASK	0x01
#define IU_MASK	(I_MASK|U_MASK)
#define IO_MASK	(I_MASK|O_MASK)

#define SWA_FAULT	0x200
#define Z_FAULT		0x100
#define D_FAULT		0x080
#define V_FAULT		0x040

#define FPA_TRAP	0x4000
#define I_TRAP		0x2000
#define U_TRAP		0x1000
#define O_TRAP		0x0800
#define IU_TRAP		(I_TRAP|U_TRAP)
#define IO_TRAP		(I_TRAP|O_TRAP)

/* XXX - Is special almost always checked before these are used? */
/* XXX - Should ZERO/UNORM/NORM also have a class? */
/* XXX - Should ZERO be special? */
#define QNAN(x)		((x)->special && (x)->class == CLASS_QNAN)
#define SNAN(x)		((x)->special && (x)->class == CLASS_SNAN)
#define NATV(x)		((x)->special && (x)->class == CLASS_NAT)
#define INF(x)		((x)->special && (x)->class == CLASS_INF)
#define NOT_INF(x)	((x)->special && (x)->class != CLASS_INF)
#define UNSUPP(x)	((x)->special && (x)->class == CLASS_UNSUPP)
#define ZERO(x)		(!(x)->special && (x)->unorm >= 64)
#define UNORM(x)	(!(x)->special && \
			 (((x)->unorm < 64 && (x)->unorm != 0) || \
			  ((x)->unorm == 64 && (x)->exp != 0) || \
			  ((x)->class == CLASS_DE0)))

#define RC(x)	BitfX8(x,2,2)
#define PC(x)	BitfX8(x,4,2)
#define WRE(x)	BitfX8(x,6,1)
#define FTZ(x)	BitfX8(x,7,1)

typedef enum {FPRND_RN, FPRND_RM, FPRND_RP, FPRND_RZ} RMODE;

#define SGL_MANT_WIDTH	24
#define DBL_MANT_WIDTH	53
#define EXT_MANT_WIDTH	64

#define SGL_EXP_WIDTH	 8
#define DBL_EXP_WIDTH	11
#define EXT_EXP_WIDTH	15
#define WRE_EXP_WIDTH	17

#define SGL_BIAS	((1<<(SGL_EXP_WIDTH-1))-1)
#define DBL_BIAS	((1<<(DBL_EXP_WIDTH-1))-1)
#define EXT_BIAS	((1<<(EXT_EXP_WIDTH-1))-1)
#define WRE_BIAS	((1<<(WRE_EXP_WIDTH-1))-1)
#define INT_BIAS	(WRE_BIAS+EXTRA_BIAS)

#define SGL_EMIN	(-(SGL_BIAS-1)+INT_BIAS)
#define DBL_EMIN	(-(DBL_BIAS-1)+INT_BIAS)
#define EXT_EMIN	(-(EXT_BIAS-1)+INT_BIAS)
#define WRE_EMIN	(-(WRE_BIAS-1)+INT_BIAS)

#define SGL_EMAX	(SGL_BIAS+INT_BIAS)
#define DBL_EMAX	(DBL_BIAS+INT_BIAS)
#define EXT_EMAX	(EXT_BIAS+INT_BIAS)
#define WRE_EMAX	(WRE_BIAS+INT_BIAS)

#define Carry(l, r, s) (((l & r) | ((l ^ r) & ~s)) & 0x8000000000000000ULL)

/*################################ Functions ###############################*/

static void mult(DWORD l, DWORD r, DWORD *rh, DWORD *rl)
{
    DWORD lH, lL, rH, rL, lHrL, lLrH;

    lH = l >> 32;
    lL = l & 0xFFFFFFFFULL;
    rH = r >> 32;
    rL = r & 0xFFFFFFFFULL;

    lHrL = lH * rL;
    lLrH = lL * rH;

    *rh = lH * rH + (lHrL >> 32) + (lLrH >> 32);

    lHrL <<= 32;
    lLrH <<= 32;
    lH = lHrL + lLrH;
    if (Carry(lHrL, lLrH, lH))
	(*rh)++;

    lL *= rL;
    *rl = lH + lL;
    if (Carry(lH, lL, *rl))
	(*rh)++;
}

static unsigned sNaN(BYTE traps, const FREG *f, FREG *d)
{
    if (traps & V_MASK) {
	d->special = YES;	/* XXX - needed? */
	d->class = CLASS_QNAN;
	d->sign = f->sign;	/* XXX - check this */
	d->mant = f->mant | 0x4000000000000000ULL;
	return V_MASK;
    } else
	return V_FAULT;
}

static unsigned qNaN(BYTE traps, FREG *d)
{
    if (traps & V_MASK) {
	d->class = CLASS_QNAN;
	d->sign = 1;
	d->mant = 0xC000000000000000ULL;
	return V_MASK;
    } else
	return V_FAULT;
}

static unsigned invInput(BYTE traps)
{
    return traps & V_MASK ? V_MASK : V_FAULT;
}

static unsigned denInput(BYTE traps)
{
    return traps & D_MASK ? D_MASK : D_FAULT;
}

static unsigned unormChk1(const FREG *f1, BYTE traps)
{
    return UNORM(f1) ? denInput(traps) : 0;
}

static unsigned unormChk2(const FREG *f1, const FREG *f2, BYTE traps)
{
    return UNORM(f1) || UNORM(f2) ? denInput(traps) : 0;
}

static unsigned unormChk3(const FREG *f1, const FREG *f2, const FREG *f3,
			  BYTE traps)
{
    return UNORM(f1) || UNORM(f2) || UNORM(f3) ? denInput(traps) : 0;
}

static unsigned special1(const FREG *a, const FREG *b, const FREG *c, FREG *d,
			 BYTE traps)
{
    unsigned flags = 0;

    d->special = YES;

    if (NATV(a) || NATV(b) || NATV(c)) {
	d->class = CLASS_NAT;
	return 0;
    }
    if (UNSUPP(a) || UNSUPP(b) || UNSUPP(c))
	return qNaN(traps, d);
    if (SNAN(a) || SNAN(b) || SNAN(c)) {
	if (traps & V_MASK)
	    flags = V_MASK;
	else
	    return V_FAULT;
    }
    if (SNAN(b)) {
	d->class = CLASS_QNAN;
	d->sign = b->sign;
	d->mant = b->mant | 0x4000000000000000ULL;
	return flags;
    }
    if (QNAN(b)) {
	*d = *b;
	return flags;
    }
    if (SNAN(c)) {
	d->class = CLASS_QNAN;
	d->sign = c->sign;
	d->mant = c->mant | 0x4000000000000000ULL;
	return flags;
    }
    if (QNAN(c)) {
	*d = *c;
	return flags;
    }
    if (SNAN(a)) {
	d->class = CLASS_QNAN;
	d->sign = a->sign;
	d->mant = a->mant | 0x4000000000000000ULL;
	return flags;
    }
    if (QNAN(a)) {
	*d = *a;
	return 0;
    }
    return -2;
}

static unsigned special2(const FREG *a, const FREG *b, const FREG *c, FREG *d,
			 BYTE traps)
{
    /* We know that at least 1 input is an infinity */
    /* d->special has already been set by special1 */

    if (INF(a) || INF(b)) {
	BYTE s = a->sign ^ b->sign;	/* XXX - BOOL? */

	if ((ZERO(a) && !a->exp) || (ZERO(b) && !b->exp)
        || (INF(c) && s != c->sign))
	    return qNaN(traps, d);
	d->sign = s;
    } else	/* c must be an infinity */
	d->sign = c->sign;
    d->class = CLASS_INF;
    return unormChk3(a, b, c, traps);
}

static BOOL incrMant(RMODE rmode, BOOL last, BOOL guard, BOOL sticky, BOOL sign)
{
    switch (rmode) {
	case FPRND_RN:
	    return guard && (last || sticky);
	case FPRND_RM:
	    return sign && (guard || sticky);
	case FPRND_RP:
	    return !sign && (guard || sticky);
	case FPRND_RZ:
	    return NO;
    }
    /*NOTREACHED*/
    return NO;
}

/* Set exp and mant to infinity or max abs norm */
static BOOL overflow(RMODE rmode, unsigned emax, unsigned mwidth, FREG *d)
{
    DWORD ovfMant;
    BOOL inf;

    switch (mwidth) {
	case SGL_MANT_WIDTH:
	    ovfMant = 0xFFFFFF0000000000ULL;
	    break;
	case DBL_MANT_WIDTH:
	    ovfMant = 0xFFFFFFFFFFFFF800ULL;
	    break;
	case EXT_MANT_WIDTH:
	    ovfMant = 0xFFFFFFFFFFFFFFFFULL;
	    break;
    }
    switch (rmode) {
	case FPRND_RN:
	    inf = YES;
	    break;
	case FPRND_RM:
	    inf = d->sign;
	    break;
	case FPRND_RP:
	    inf = !d->sign;
	    break;
	case FPRND_RZ:
	    inf = NO;
	    break;
    }
    if (inf) {
	d->special = YES;
	d->class = CLASS_INF;
    } else {
	d->exp  = emax;
	d->mant = ovfMant;
    }
    return inf;
}

static unsigned round(FREG *d, unsigned mantWidth, RMODE rmode, DWORD resLo)
{
    BOOL last, guard, sticky;
    unsigned fpa = 0;

#if 0
    if (d->exp && d->exp < expMin) {	/* Underflow */
	/* XXX - fill this in */;
    }
#endif
    if (mantWidth == 0) {
	sticky = (d->mant & ONES(63)) || resLo;
	guard  = (d->mant >> 63) & 1;

	if (incrMant(rmode, NO, guard, sticky, d->sign)) {
	    d->mant = 1ULL << 63;
	    d->exp++;
	    fpa = FPA_TRAP;
	} else
	    d->mant = 0;
    } else if (mantWidth != 64) {
	if (mantWidth != 63)
	    sticky = (d->mant & ONES(63-mantWidth)) || resLo;
	else
	    sticky = resLo != 0;
	guard  = (d->mant >> (63-mantWidth)) & 1;
	last   = (d->mant >> (64-mantWidth)) & 1;

	if (incrMant(rmode, last, guard, sticky, d->sign)) {
	    DWORD mant = (d->mant >> (64-mantWidth)) + 1;

	    fpa = FPA_TRAP;
	    if (mant >> mantWidth) {
		d->mant = mant << (63-mantWidth);
		d->exp++;
	    } else
		d->mant = mant << (64-mantWidth);
	} else
	    d->mant = (d->mant >> (64-mantWidth)) << (64-mantWidth);
    } else {
	sticky = (resLo << 1) != 0;
	guard  = SIGN(resLo);
	last   = d->mant & 1;

	if (incrMant(rmode, last, guard, sticky, d->sign)) {
	    d->mant++;
	    fpa = FPA_TRAP;
	    if (!d->mant) {
		d->exp++;
		d->mant = 1ULL<<63;
	    }
	}
    }
    if (!d->mant) {
	d->unorm = 64;
	d->exp = 0;
    }
    return fpa | (guard || sticky ? I_MASK : 0);
}

/*ARGSUSED*/
static BOOL underflowTrap(BYTE traps, BYTE controls)
{
    return !(traps & U_MASK);
}

static void modExp(FREG *d, FPPC prec)
{
    d->exp -= EXTRA_BIAS;
    if (prec == PC_SIMD) {
	d->exp -= WRE_BIAS - SGL_BIAS;
	d->exp %= 1 << SGL_EXP_WIDTH;
	d->exp += WRE_BIAS - SGL_BIAS;
    } else
	d->exp %= 1 << WRE_EXP_WIDTH;
    *d = fill2freg(d->sign, d->exp, d->mant);
}

static unsigned normalize(FREG *d, DWORD low, unsigned flags, RMODE rmode,
			  FPPC prec, BYTE traps, BYTE controls)
{
    unsigned mantWidth, expMin, expMax, f2;
    FREG d2;

    /* Set mantWidth, expMin, & expMax from prec and controls */
    switch (prec) {
	case PC_DYN:
	    switch ((int)PC(controls)) {
		case 0:
		    mantWidth = SGL_MANT_WIDTH;
		    break;
		case 2:
		    mantWidth = DBL_MANT_WIDTH;
		    break;
		case 3:
		    mantWidth = EXT_MANT_WIDTH;
		    break;
	    }
	    expMin = WRE(controls) ? WRE_EMIN : EXT_EMIN;
	    expMax = WRE(controls) ? WRE_EMAX : EXT_EMAX;
	    break;
	case PC_SGL:
	    mantWidth = SGL_MANT_WIDTH;
	    expMin = WRE(controls) ? WRE_EMIN : SGL_EMIN;
	    expMax = WRE(controls) ? WRE_EMAX : SGL_EMAX;
	    break;
	case PC_DBL:
	    mantWidth = DBL_MANT_WIDTH;
	    expMin = WRE(controls) ? WRE_EMIN : DBL_EMIN;
	    expMax = WRE(controls) ? WRE_EMAX : DBL_EMAX;
	    break;
	case PC_SIMD:
	    mantWidth = SGL_MANT_WIDTH;
	    expMin = SGL_EMIN;
	    expMax = SGL_EMAX;
	    break;
    }
    d->class = CLASS_NONE;
    d2 = *d;
    f2 = flags;

    flags |= round(d, mantWidth, rmode, low);
    if (d->exp > expMax) {		/* Overflow */
	if (traps & O_MASK) {
	    BOOL fpa;

	    /* XXX - SWA trap if ignore O disable? */
	    fpa = overflow(rmode, expMax, mantWidth, d);
	    flags |= IO_MASK;
	    if (!(traps & I_MASK)) {
		flags |= I_TRAP;
		if (fpa)
		    flags |= FPA_TRAP;
	    }
	} else {
	    modExp(d, prec);
	    flags |= O_MASK | O_TRAP;
	    if (flags & I_MASK)
		flags |= I_TRAP;
	}
    } else if (d->exp < expMin) {	/* Underflow */
	if (underflowTrap(traps, controls)) {
	    modExp(d, prec);
	    flags |= U_MASK | U_TRAP;
	    if (flags & I_MASK)
		flags |= I_TRAP;
	} else if (FTZ(controls)) {
	    d->unorm = 64;
	    d->exp = 0;
	    flags |= IU_MASK;
	    if (!(traps & I_MASK)) {
		flags |= I_TRAP;
		flags &= ~FPA_TRAP;
	    }
	} else {
	    unsigned ufl = expMin - d2.exp;

	    *d = d2;
	    flags = f2;
	    if (ufl < mantWidth)
		mantWidth -= ufl;
	    else {
		d->exp = expMin - mantWidth;
		if (ufl > mantWidth)
		    d->mant = 0x1;
		mantWidth = 0;
	    }
	    flags |= round(d, mantWidth, rmode, low);
	    if (d->mant) {
		d->unorm = expMin - d->exp;
		if (expMin == EXT_EMIN && d->unorm)
		    d->class = CLASS_DE0;
	    }
	    if (flags & I_MASK) {
		flags |= U_MASK;
		if (!(traps & I_MASK))
		    flags |= I_TRAP;
	    }
	}
    } else if ((flags & I_MASK) && !(traps & I_MASK))
	flags |= I_TRAP;
    return flags;
}

static int numLz(DWORD *hi, DWORD *lo)
{
    int lzeroes = 0;

    if (!(*hi >> 32)) {
	*hi = *hi << 32 | *lo >> 32;
	*lo <<= 32;
	lzeroes += 32;
    }
    if (!(*hi >> 48)) {
	*hi = *hi << 16 | *lo >> 48;
	*lo <<= 16;
	lzeroes += 16;
    }
    if (!(*hi >> 56)) {
	*hi = *hi << 8 | *lo >> 56;
	*lo <<= 8;
	lzeroes += 8;
    }
    if (!(*hi >> 60)) {
	*hi = *hi << 4 | *lo >> 60;
	*lo <<= 4;
	lzeroes += 4;
    }
    if (!(*hi >> 62)) {
	*hi = *hi << 2 | *lo >> 62;
	*lo <<= 2;
	lzeroes += 2;
    }
    if (!(*hi >> 63)) {
	*hi = *hi << 1 | *lo >> 63;
	*lo <<= 1;
	lzeroes += 1;
    }
    return lzeroes;
}

/* Sets d = a * b + c */
static unsigned fmpyadd(const FREG *a, const FREG *b, FREG *c, FREG *d,
			FPPC prec, BYTE traps, BYTE controls)
{
    unsigned flags;
    RMODE rmode;
    FREG m;
    DWORD mLo, mant, mantLo, cLo, left, leftLo, right, rightLo;
    BOOL extent;
    int lzeroes, delta;

    if ((flags = unormChk3(a, b, c, traps)) & FP_FAULT)
	return flags;

    rmode = RC(controls);
    m.sign = a->sign ^ b->sign;
    if (ZERO(a) || ZERO(b)) {	/* m is zero */
	if (c->unorm == CLASS_FR0) {	/* IEEE multiply */
	    d->special = NO;
	    d->class = CLASS_NONE;
	    d->sign = m.sign;
	    d->unorm = 64;
	    d->exp = 0;
	    return flags;
	}
	*d = *c;
	d->special = NO; /* XXX - not needed since c->special must be NO? */
	if (ZERO(c)) {
	    d->class = CLASS_NONE;
	    d->exp = 0;
	    d->sign = (m.sign != c->sign && rmode == FPRND_RM) ||
		      (m.sign && c->sign) ? 1 : 0;
	    return flags;
	} else {
	    d->unorm = 0;
	    return normalize(d, 0, flags, rmode, prec, traps, controls);
	}
    }
    m.special = NO;
    m.exp = a->exp + b->exp - INT_BIAS + 1;
    mult(a->mant, b->mant, &m.mant, &mLo);
    if (!SIGN(m.mant)) {	/* post-normalize */
	m.mant = m.mant << 1 | SIGN(mLo);
	mLo <<= 1;
	m.exp--;
    }
    if (ZERO(c)) {
	*d = m;
	d->unorm = 0;
	return normalize(d, mLo, flags, rmode, prec, traps, controls);
    }
    d->special = NO;
    delta = m.exp - c->exp;
    if (!delta) {
	d->exp = m.exp;
	extent = NO;
	cLo = 0;
    } else if (delta > 0) {		/* right shift 3rd operand */
	d->exp = m.exp;
	if (delta < 64) {
	    extent = NO;
	    cLo = (c->mant & ONES(delta)) << (64-delta);
	    c->mant >>= delta;
	} else {
	    if (delta == 64) {
		extent = NO;
		cLo = c->mant;
	    } else if (delta < 128) {
		extent = (c->mant & ONES(delta-64)) != 0;
		cLo = c->mant >> (delta-64);
	    } else {
		extent = YES;
		cLo = 0;
	    }
	    c->mant = 0;
	}
    } else {				/* right shift product */
	d->exp = c->exp;
	cLo = 0;
	if (delta > -64) {
	    extent = (mLo & ONES(-delta)) != 0;
	    mLo = (m.mant & ONES(-delta)) << (64+delta) | (mLo >> -delta);
	    m.mant >>= -delta;
	} else {
	    if (delta == -64) {
		extent = mLo != 0;
		mLo = m.mant;
	    } else if (delta > -128) {
		extent = mLo != 0 || (m.mant & ONES(-delta-64)) != 0;
		mLo = m.mant >> (-delta-64);
	    } else {
		extent = YES;
		mLo = 0;
	    }
	    m.mant = 0;
	}
    }
    if (m.sign == c->sign) {	/* Signs are the same -- perform addition */
	d->sign = m.sign;
	d->unorm = 0;
	mant = m.mant + c->mant;
	mantLo = mLo + cLo;
	if (Carry(mLo, cLo, mantLo))	/* overflow into high part */
	    mant++;		/* XXX - can this overflow? */
	if (Carry(m.mant, c->mant, mant)) {	/* overflow */
	    d->exp++;
	    d->mant = 0x8000000000000000ULL | (mant >> 1);
	    if (mantLo & 1)
		extent = YES;
	    mantLo = (mant & 1) << 63 | (mantLo >> 1);
	} else
	    d->mant = mant;
	if (extent)
	    mantLo |= 1;
	return normalize(d, mantLo, flags, rmode, prec, traps, controls);
    }
    /* Signs are opposite -- perform subtraction */
    if (!delta && m.mant == c->mant && !mLo) {		/* |m| = |c| */
	d->sign = rmode == FPRND_RM ? 1 : 0;
	d->class = CLASS_NONE;
	d->unorm = 64;
	d->exp = 0;
	return flags;
    }
    if (delta > 0 || (!delta && m.mant >= c->mant)) {	/* |m| > |c| */
	d->sign = m.sign;
	left = m.mant;
	leftLo = mLo;
	right = c->mant;
	rightLo = cLo;
    } else {						/* |m| < |c| */
	d->sign = c->sign;
	left = c->mant;
	leftLo = cLo;
	right = m.mant;
	rightLo = mLo;
	delta = -delta;
    }
    mant = left - right;
    if (!delta) {
	if (!rightLo)
	    mantLo = leftLo;
	else {
	    mantLo = -rightLo;
	    mant--;
	}
	if (!mant) {
	    lzeroes = 64;
	    mant = mantLo;
	    mantLo = 0;
	} else
	    lzeroes = 0;
	lzeroes += numLz(&mant, &mantLo);
    } else if (delta == 1) {
	if (extent) {
	    mantLo = -rightLo - 1;
	    mant--;
	} else {
	    mantLo = leftLo - rightLo;
	    if (leftLo < rightLo)
		mant--;
	}
	if (!mant) {
	    lzeroes = 64;
	    mant = mantLo;
	    /* XXX - I think extent has to be YES here */
	    mantLo = (DWORD)extent << 63;
	} else {
	    lzeroes = 0;
	    if (extent)
		mantLo |= 1;
	}
	lzeroes += numLz(&mant, &mantLo);
    } else {
	if (extent) {
	    mantLo = leftLo - rightLo - 1;
	    if (leftLo <= rightLo)
		mant--;
	} else {
	    mantLo = leftLo - rightLo;
	    if (leftLo < rightLo)
		mant--;
	}
	if (!SIGN(mant)) {
	    mant = mant << 1 | SIGN(mantLo);
	    mantLo <<= 1;
	    lzeroes = 1;
	} else
	    lzeroes = 0;
	if (extent)
	    mantLo |= 1;
    }
    d->unorm = 0;
    d->exp -= lzeroes;
    d->mant = mant;
    return normalize(d, mantLo, flags, rmode, prec, traps, controls);
}

/* Sets d = a * b + c */
unsigned fma_(FREG *a, FREG *b, FREG *c, FREG *d, FPPC prec, BYTE traps,
	      BYTE controls)
{
    unsigned flags;

    if (!a->special && !b->special && !c->special)
	return fmpyadd(a, b, c, d, prec, traps, controls);
    flags = special1(a, b, c, d, traps);
    if (flags != -2)
	return flags;
    return special2(a, b, c, d, traps);
}

/* Sets d = a * b - c */
unsigned fms(FREG *a, FREG *b, FREG *c, FREG *d, FPPC prec, BYTE traps,
	     BYTE controls)
{
    unsigned flags;

    if (!a->special && !b->special && !c->special) {
	c->sign ^= 1;
	return fmpyadd(a, b, c, d, prec, traps, controls);
    }
    flags = special1(a, b, c, d, traps);
    if (flags != -2)
	return flags;
    c->sign ^= 1;
    return special2(a, b, c, d, traps);
}

/* Sets d = -a * b + c */
unsigned fnma(FREG *a, FREG *b, FREG *c, FREG *d, FPPC prec, BYTE traps,
	      BYTE controls)
{
    unsigned flags;

    if (!a->special && !b->special && !c->special) {
	a->sign ^= 1;
	return fmpyadd(a, b, c, d, prec, traps, controls);
    }
    flags = special1(a, b, c, d, traps);
    if (flags != -2)
	return flags;
    a->sign ^= 1;
    return special2(a, b, c, d, traps);
}


/*****************************/
/* Integer Multiply Routines */
/*****************************/
DWORD xmah(DWORD m1, DWORD m2, DWORD m3)
{
    DWORD resHi, resLo, sum;

    mult(m1, m2, &resHi, &resLo);
    if ((long long)m1 < 0)
	resHi -= m2;
    if ((long long)m2 < 0)
	resHi -= m1;
    sum = resLo + m3;
    if (Carry(resLo, m3, sum))
	resHi++;
    return resHi;
}

DWORD xmahu(DWORD m1, DWORD m2, DWORD m3)
{
    DWORD resHi, resLo, sum;

    mult(m1, m2, &resHi, &resLo);
    sum = resLo + m3;
    if (Carry(resLo, m3, sum))
	resHi++;
    return resHi;
}

FREG xmpyl(const FREG *f1, const FREG *f2)
{
    FREG f;
    DWORD m1, m2, resHi, resLo;

    f.sign = 0;
    f.special = NO;
    if (ZERO(f1) || ZERO(f2)) {
	f.exp   = INTEGER_EXP;
	f.unorm = 64;
	return f;
    }
    m1 = f1->mant >> f1->unorm;
    m2 = f2->mant >> f2->unorm;
    mult(m1, m2, &resHi, &resLo);
    if (resLo) {
	int exp;
	DWORD mant = resLo;

    exp = 63;
    if (!(mant >> 32)) {
	mant <<= 32;
	exp -= 32;
    }
    if (!(mant >> 48)) {
	mant <<= 16;
	exp -= 16;
    }
    if (!(mant >> 56)) {
	mant <<= 8;
	exp -= 8;
    }
    if (!(mant >> 60)) {
	mant <<= 4;
	exp -= 4;
    }
    if (!(mant >> 62)) {
	mant <<= 2;
	exp -= 2;
    }
    if (!(mant >> 63)) {
	mant <<= 1;
	exp -= 1;
    }
	f.unorm  = exp;
	f.exp  = INTEGER_EXP - exp;
	f.mant = mant;
	f.class = CLASS_NONE;
    } else {
	f.unorm = 64;
	f.exp  = INTEGER_EXP;
    }
    return f;
}


/*************************************/
/* Reciprocal Approximation Routines */
/*************************************/
static BOOL frcpaLimitsChk(const FREG *f2, const FREG *f3)
{
    int est_exp = f2->exp - f3->exp;

    if (INF(f2) || ZERO(f2) || INF(f3))
	return NO;
    if (f3->exp <= EXTRA_BIAS || f3->exp >= (INT_BIAS+WRE_BIAS-2))
	return YES;
    if (est_exp <= (2-WRE_BIAS) || est_exp >= WRE_BIAS)
	return YES;
    if (f2->exp <= EXTRA_BIAS+64)
	return YES;
    return NO;
}

/* Sets b to an approximation (to 8 bits of precision) of 1/a */
static void ieee_recip(const FREG *a, FREG *b)
{
    static const HWORD recip_table[] = {
	0x7fc, 0x7f4, 0x7ec, 0x7e4, 0x7dd, 0x7d5, 0x7cd, 0x7c6,
	0x7be, 0x7b7, 0x7af, 0x7a8, 0x7a1, 0x799, 0x792, 0x78b,
	0x784, 0x77d, 0x776, 0x76f, 0x768, 0x761, 0x75b, 0x754,
	0x74d, 0x746, 0x740, 0x739, 0x733, 0x72c, 0x726, 0x720,
	0x719, 0x713, 0x70d, 0x707, 0x700, 0x6fa, 0x6f4, 0x6ee,
	0x6e8, 0x6e2, 0x6dc, 0x6d7, 0x6d1, 0x6cb, 0x6c5, 0x6bf,
	0x6ba, 0x6b4, 0x6af, 0x6a9, 0x6a3, 0x69e, 0x699, 0x693,
	0x68e, 0x688, 0x683, 0x67e, 0x679, 0x673, 0x66e, 0x669,
	0x664, 0x65f, 0x65a, 0x655, 0x650, 0x64b, 0x646, 0x641,
	0x63c, 0x637, 0x632, 0x62e, 0x629, 0x624, 0x61f, 0x61b,
	0x616, 0x611, 0x60d, 0x608, 0x604, 0x5ff, 0x5fb, 0x5f6,
	0x5f2, 0x5ed, 0x5e9, 0x5e5, 0x5e0, 0x5dc, 0x5d8, 0x5d4,
	0x5cf, 0x5cb, 0x5c7, 0x5c3, 0x5bf, 0x5bb, 0x5b6, 0x5b2,
	0x5ae, 0x5aa, 0x5a6, 0x5a2, 0x59e, 0x59a, 0x597, 0x593,
	0x58f, 0x58b, 0x587, 0x583, 0x57f, 0x57c, 0x578, 0x574,
	0x571, 0x56d, 0x569, 0x566, 0x562, 0x55e, 0x55b, 0x557,
	0x554, 0x550, 0x54d, 0x549, 0x546, 0x542, 0x53f, 0x53b,
	0x538, 0x534, 0x531, 0x52e, 0x52a, 0x527, 0x524, 0x520,
	0x51d, 0x51a, 0x517, 0x513, 0x510, 0x50d, 0x50a, 0x507,
	0x503, 0x500, 0x4fd, 0x4fa, 0x4f7, 0x4f4, 0x4f1, 0x4ee,
	0x4eb, 0x4e8, 0x4e5, 0x4e2, 0x4df, 0x4dc, 0x4d9, 0x4d6,
	0x4d3, 0x4d0, 0x4cd, 0x4ca, 0x4c8, 0x4c5, 0x4c2, 0x4bf,
	0x4bc, 0x4b9, 0x4b7, 0x4b4, 0x4b1, 0x4ae, 0x4ac, 0x4a9,
	0x4a6, 0x4a4, 0x4a1, 0x49e, 0x49c, 0x499, 0x496, 0x494,
	0x491, 0x48e, 0x48c, 0x489, 0x487, 0x484, 0x482, 0x47f,
	0x47c, 0x47a, 0x477, 0x475, 0x473, 0x470, 0x46e, 0x46b,
	0x469, 0x466, 0x464, 0x461, 0x45f, 0x45d, 0x45a, 0x458,
	0x456, 0x453, 0x451, 0x44f, 0x44c, 0x44a, 0x448, 0x445,
	0x443, 0x441, 0x43f, 0x43c, 0x43a, 0x438, 0x436, 0x433,
	0x431, 0x42f, 0x42d, 0x42b, 0x429, 0x426, 0x424, 0x422,
	0x420, 0x41e, 0x41c, 0x41a, 0x418, 0x415, 0x413, 0x411,
	0x40f, 0x40d, 0x40b, 0x409, 0x407, 0x405, 0x403, 0x401
    };

    b->sign = a->sign;
    b->mant = (DWORD)recip_table[BitfX(a->mant,1,8)] << 53;
    b->exp = (2*INT_BIAS - 1) - a->exp;
    b->unorm = 0;
    b->special = NO;
    b->class = CLASS_NONE;
}

/* Sets b to an approximation (to 8 bits of precision) of 1/sqrt(a) */
static void ieee_recip_sqrt(const FREG *a, FREG *b)
{
    int i;

    static const HWORD recip_sqrt_table[] = {
	0x5a5, 0x5a0, 0x59a, 0x595, 0x58f, 0x58a, 0x585, 0x580,
	0x57a, 0x575, 0x570, 0x56b, 0x566, 0x561, 0x55d, 0x558,
	0x553, 0x54e, 0x54a, 0x545, 0x540, 0x53c, 0x538, 0x533,
	0x52f, 0x52a, 0x526, 0x522, 0x51e, 0x51a, 0x515, 0x511,
	0x50d, 0x509, 0x505, 0x501, 0x4fd, 0x4fa, 0x4f6, 0x4f2,
	0x4ee, 0x4ea, 0x4e7, 0x4e3, 0x4df, 0x4dc, 0x4d8, 0x4d5,
	0x4d1, 0x4ce, 0x4ca, 0x4c7, 0x4c3, 0x4c0, 0x4bd, 0x4b9,
	0x4b6, 0x4b3, 0x4b0, 0x4ad, 0x4a9, 0x4a6, 0x4a3, 0x4a0,
	0x49d, 0x49a, 0x497, 0x494, 0x491, 0x48e, 0x48b, 0x488,
	0x485, 0x482, 0x47f, 0x47d, 0x47a, 0x477, 0x474, 0x471,
	0x46f, 0x46c, 0x469, 0x467, 0x464, 0x461, 0x45f, 0x45c,
	0x45a, 0x457, 0x454, 0x452, 0x44f, 0x44d, 0x44a, 0x448,
	0x445, 0x443, 0x441, 0x43e, 0x43c, 0x43a, 0x437, 0x435,
	0x433, 0x430, 0x42e, 0x42c, 0x429, 0x427, 0x425, 0x423,
	0x420, 0x41e, 0x41c, 0x41a, 0x418, 0x416, 0x414, 0x411,
	0x40f, 0x40d, 0x40b, 0x409, 0x407, 0x405, 0x403, 0x401,
	0x7fc, 0x7f4, 0x7ec, 0x7e5, 0x7dd, 0x7d5, 0x7ce, 0x7c7,
	0x7bf, 0x7b8, 0x7b1, 0x7aa, 0x7a3, 0x79c, 0x795, 0x78e,
	0x788, 0x781, 0x77a, 0x774, 0x76d, 0x767, 0x761, 0x75a,
	0x754, 0x74e, 0x748, 0x742, 0x73c, 0x736, 0x730, 0x72b,
	0x725, 0x71f, 0x71a, 0x714, 0x70f, 0x709, 0x704, 0x6fe,
	0x6f9, 0x6f4, 0x6ee, 0x6e9, 0x6e4, 0x6df, 0x6da, 0x6d5,
	0x6d0, 0x6cb, 0x6c6, 0x6c1, 0x6bd, 0x6b8, 0x6b3, 0x6ae,
	0x6aa, 0x6a5, 0x6a1, 0x69c, 0x698, 0x693, 0x68f, 0x68a,
	0x686, 0x682, 0x67d, 0x679, 0x675, 0x671, 0x66d, 0x668,
	0x664, 0x660, 0x65c, 0x658, 0x654, 0x650, 0x64c, 0x649,
	0x645, 0x641, 0x63d, 0x639, 0x635, 0x632, 0x62e, 0x62a,
	0x627, 0x623, 0x620, 0x61c, 0x618, 0x615, 0x611, 0x60e,
	0x60a, 0x607, 0x604, 0x600, 0x5fd, 0x5f9, 0x5f6, 0x5f3,
	0x5f0, 0x5ec, 0x5e9, 0x5e6, 0x5e3, 0x5df, 0x5dc, 0x5d9,
	0x5d6, 0x5d3, 0x5d0, 0x5cd, 0x5ca, 0x5c7, 0x5c4, 0x5c1,
	0x5be, 0x5bb, 0x5b8, 0x5b5, 0x5b2, 0x5af, 0x5ac, 0x5aa
    };

    i = ((a->exp - EXTRA_BIAS) & 1) << 7 | BitfX(a->mant,1,7);
    b->mant = (DWORD)recip_sqrt_table[i] << 53;
    b->exp = (((INT_BIAS-1 - ((a->exp - INT_BIAS)>>1)) - EXTRA_BIAS)
		& ONES(WRE_EXP_WIDTH)) + EXTRA_BIAS;
    b->sign = 0;
    b->unorm = 0;
    b->special = NO;
    b->class = CLASS_NONE;
}

unsigned frcpa(const FREG *num, const FREG *den, FREG *ft, BOOL *pt,
	       BYTE traps)
{
    unsigned flags = 0;

    ft->special = YES;
    *pt = NO;

    if (UNSUPP(num) || UNSUPP(den))
	return qNaN(traps, ft);
    if (SNAN(num) || SNAN(den)) {
	if (traps & V_MASK)
	    flags = V_MASK;
	else
	    return V_FAULT;
    }
    if (SNAN(num)) {
	ft->class = CLASS_QNAN;
	ft->sign = num->sign;
	ft->mant = num->mant | 0x4000000000000000ULL;
	return flags;
    }
    if (QNAN(num)) {
	*ft = *num;
	return flags;
    }
    if (SNAN(den)) {
	ft->class = CLASS_QNAN;
	ft->sign = den->sign;
	ft->mant = den->mant | 0x4000000000000000ULL;
	return flags;
    }
    if (QNAN(den)) {
	*ft = *den;
	return flags;
    }
    if ((INF(num) && INF(den)) || (ZERO(num) && ZERO(den)))
	return qNaN(traps, ft);
    if (!INF(num) && ZERO(den)) {
	if (traps & Z_MASK) {
	    ft->class = CLASS_INF;
	    ft->sign = num->sign ^ den->sign;
	    return Z_MASK;
	} else
	    return Z_FAULT;
    }
    if (frcpaLimitsChk(num, den))
	return SWA_FAULT;
    if ((flags = unormChk2(num, den, traps)) & FP_FAULT)
	return flags;
    if (INF(num)) {
	ft->class = CLASS_INF;
	ft->sign = num->sign ^ den->sign;
    } else if (ZERO(num) || INF(den)) {
	ft->special = NO;
	ft->class = CLASS_NONE;
	ft->unorm = 64;
	ft->sign = num->sign ^ den->sign;
	ft->exp = 0;
    } else {
	ieee_recip(den, ft);
	*pt = YES;
    }
    return flags;
}

unsigned fprcpa(const FREG *num, const FREG *den, FREG *ft, BOOL *pt,
		BYTE traps)
{
    unsigned flags = 0;

    ft->special = YES;
    *pt = NO;

    if (SNAN(num) || SNAN(den)) {
	if (traps & V_MASK)
	    flags = V_MASK;
	else
	    return V_FAULT;
    }
    if (SNAN(num)) {
	ft->class = CLASS_QNAN;
	ft->sign = num->sign;
	ft->mant = num->mant | 0x4000000000000000ULL;
	return flags;
    }
    if (QNAN(num)) {
	*ft = *num;
	return flags;
    }
    if (SNAN(den)) {
	ft->class = CLASS_QNAN;
	ft->sign = den->sign;
	ft->mant = den->mant | 0x4000000000000000ULL;
	return flags;
    }
    if (QNAN(den)) {
	*ft = *den;
	return flags;
    }
    if ((INF(num) && INF(den)) || (ZERO(num) && ZERO(den)))
	return qNaN(traps, ft);
    if (!INF(num) && ZERO(den)) {
	if (traps & Z_MASK) {
	    ft->class = CLASS_INF;
	    ft->sign = num->sign ^ den->sign;
	    return Z_MASK;
	} else
	    return Z_FAULT;
    }
    if ((flags = unormChk2(num, den, traps)) & FP_FAULT)
	return flags;
    if (INF(num)) {
	ft->class = CLASS_INF;
	ft->sign = num->sign ^ den->sign;
    } else if (ZERO(num) || INF(den)) {
	ft->special = NO;
	ft->class = CLASS_NONE;
	ft->sign = num->sign ^ den->sign;
	ft->unorm = 64;
	ft->exp = 0;
    } else if (UNORM(den)) {
	ft->class = CLASS_INF;
	ft->sign = den->sign;
    } else if (den->exp >= (INT_BIAS+SGL_BIAS-2)) {
	ft->special = NO;
	ft->class = CLASS_NONE;
	ft->sign = den->sign;
	ft->unorm = 64;
	ft->exp = 0;
    } else {
	int est_exp = num->exp - den->exp;

	ieee_recip(den, ft);
	if (est_exp >= (int)SGL_BIAS ||
	    est_exp <= (2-(int)SGL_BIAS) ||
	    num->exp <= INT_BIAS-SGL_BIAS+SGL_MANT_WIDTH)
	    *pt = NO;
	else
	    *pt = YES;
    }
    return flags;
}

unsigned frsqrta(const FREG *f1, FREG *ft, BOOL *pt, BYTE traps)
{
    unsigned flags;

    ft->special = YES;
    *pt = NO;

    if (UNSUPP(f1))
	return qNaN(traps, ft);
    if (SNAN(f1))
	return sNaN(traps, f1, ft);
    if (QNAN(f1)) {
	*ft = *f1;
	return 0;
    }
    if (f1->sign && !ZERO(f1))
	return qNaN(traps, ft);
    if (INF(f1)) {
	ft->class = CLASS_INF;
	ft->sign = 0;
	return 0;
    }
    if (!ZERO(f1) && f1->exp <= EXTRA_BIAS+64)
	return SWA_FAULT;
    if ((flags = unormChk1(f1, traps)) & FP_FAULT)
	return flags;
    if (ZERO(f1)) {
	ft->special = NO;
	ft->class = CLASS_NONE;
	ft->sign = f1->sign;
	ft->unorm = 64;
	ft->exp = 0;
    } else {
	ieee_recip_sqrt(f1, ft);
	*pt = YES;
    }
    return flags;
}

unsigned fprsqrta(const FREG *f1, FREG *ft, BOOL *pt, BYTE traps)
{
    unsigned flags;

    ft->special = YES;
    *pt = NO;

    if (SNAN(f1))
	return sNaN(traps, f1, ft);
    if (QNAN(f1)) {
	*ft = *f1;
	return 0;
    }
    if (f1->sign && !ZERO(f1))
	return qNaN(traps, ft);
    if (INF(f1)) {
	ft->special = NO;
	ft->class = CLASS_NONE;
	ft->sign = 0;
	ft->unorm = 64;
	ft->exp = 0;
	return 0;
    }
    if ((flags = unormChk1(f1, traps)) & FP_FAULT)
	return flags;
    if (ZERO(f1)) {
	ft->class = CLASS_INF;
	ft->sign = f1->sign;
    } else {
	ieee_recip_sqrt(f1, ft);
	*pt = f1->exp > (INT_BIAS-SGL_BIAS+24);
    }
    return flags;
}


/********************/
/* Min/Max Routines */
/********************/
static BOOL fpLessThan(const FREG *f1, const FREG *f2)
{
    if (!f1->special && INF(f2))
	return !f2->sign;
    if (INF(f1) && !f2->special)
	return f1->sign;
    if (INF(f1) && INF(f2))
	return f1->sign & !f2->sign;
    return NO;
}

static unsigned full_minmax(const FREG *f1, const FREG *f2, BYTE traps)
{
    return NOT_INF(f1) || NOT_INF(f2)
		? invInput(traps)
		: unormChk2(f1, f2, traps);
}

static unsigned full_famax(FREG *f1, FREG *f2, FREG *ft, BYTE traps)
{
    unsigned flags;

    ft->special = YES;
    if (NATV(f1) || NATV(f2)) {
	ft->class = CLASS_NAT;
	return 0;
    }
    if (!((flags = full_minmax(f1, f2, traps)) & FP_FAULT)) {
	FREG l = *f2, r = *f1;

	l.sign = r.sign = 0;
	*ft = fpLessThan(&l, &r) ? *f1 : *f2;
	if (ft->unorm == CLASS_FR0)
	    ft->unorm = 64;
    }
    return flags;
}

unsigned famax(FREG *f1, FREG *f2, FREG *ft, BYTE traps)
{
    unsigned flags;

    if (f1->special || f2->special)
	return full_famax(f1, f2, ft, traps);
    if ((flags = unormChk2(f1, f2, traps)) & FP_FAULT)
	return flags;
    if (ZERO(f1) && ZERO(f2))
	*ft = *f2;
    else if (f1->exp > f2->exp)
	*ft = *f1;
    else if (f1->exp < f2->exp)
	*ft = *f2;
    else if (f1->mant > f2->mant)
	*ft = *f1;
    else
	*ft = *f2;
    if (ft->unorm == CLASS_FR0)
	ft->unorm = 64;
    return flags;
}

static unsigned full_famin(FREG *f1, FREG *f2, FREG *ft, BYTE traps)
{
    unsigned flags;

    ft->special = YES;
    if (NATV(f1) || NATV(f2)) {
	ft->class = CLASS_NAT;
	return 0;
    }
    if (!((flags = full_minmax(f1, f2, traps)) & FP_FAULT)) {
	FREG l = *f1, r = *f2;

	l.sign = r.sign = 0;
	*ft = fpLessThan(&l, &r) ? *f1 : *f2;
	if (ft->unorm == CLASS_FR0)
	    ft->unorm = 64;
    }
    return flags;
}

unsigned famin(FREG *f1, FREG *f2, FREG *ft, BYTE traps)
{
    unsigned flags;

    if (f1->special || f2->special)
	return full_famin(f1, f2, ft, traps);
    if ((flags = unormChk2(f1, f2, traps)) & FP_FAULT)
	return flags;
    if (ZERO(f1) && ZERO(f2))
	*ft = *f2;
    else if (f1->exp < f2->exp)
	*ft = *f1;
    else if (f1->exp > f2->exp)
	*ft = *f2;
    else if (f1->mant < f2->mant)
	*ft = *f1;
    else
	*ft = *f2;
    if (ft->unorm == CLASS_FR0)
	ft->unorm = 64;
    return flags;
}

static unsigned full_fmax(FREG *f1, FREG *f2, FREG *ft, BYTE traps)
{
    unsigned flags;

    ft->special = YES;
    if (NATV(f1) || NATV(f2)) {
	ft->class = CLASS_NAT;
	return 0;
    }
    if (!((flags = full_minmax(f1, f2, traps)) & FP_FAULT))
	*ft = fpLessThan(f2, f1) ? *f1 : *f2;
    if (ft->unorm == CLASS_FR0)
	ft->unorm = 64;
    return flags;
}

unsigned fmax(FREG *f1, FREG *f2, FREG *ft, BYTE traps)
{
    unsigned flags;

    if (f1->special || f2->special)
	return full_fmax(f1, f2, ft, traps);
    if ((flags = unormChk2(f1, f2, traps)) & FP_FAULT)
	return flags;
    if (ZERO(f1) && ZERO(f2))
	*ft = *f2;
    else if (f1->sign < f2->sign)
	*ft = *f1;
    else if (f1->sign > f2->sign)
	*ft = *f2;
    else if (f1->exp > f2->exp)
	*ft = f1->sign ? *f2 : *f1;
    else if (f1->exp < f2->exp)
	*ft = f1->sign ? *f1 : *f2;
    else if (f1->mant > f2->mant)
	*ft = f1->sign ? *f2 : *f1;
    else if (f1->mant < f2->mant)
	*ft = f1->sign ? *f1 : *f2;
    else
	*ft = *f2;
    if (ft->unorm == CLASS_FR0)
	ft->unorm = 64;
    return flags;
}

static unsigned full_fmin(FREG *f1, FREG *f2, FREG *ft, BYTE traps)
{
    unsigned flags;

    ft->special = YES;
    if (NATV(f1) || NATV(f2)) {
	ft->class = CLASS_NAT;
	return 0;
    }
    if (!((flags = full_minmax(f1, f2, traps)) & FP_FAULT))
	*ft = fpLessThan(f1, f2) ? *f1 : *f2;
    if (ft->unorm == CLASS_FR0)
	ft->unorm = 64;
    return flags;
}

unsigned fmin(FREG *f1, FREG *f2, FREG *ft, BYTE traps)
{
    unsigned flags;

    if (f1->special || f2->special)
	return full_fmin(f1, f2, ft, traps);
    if ((flags = unormChk2(f1, f2, traps)) & FP_FAULT)
	return flags;
    if (ZERO(f1) && ZERO(f2))
	*ft = *f2;
    else if (f1->sign > f2->sign)
	*ft = *f1;
    else if (f1->sign < f2->sign)
	*ft = *f2;
    else if (f1->exp < f2->exp)
	*ft = f1->sign ? *f2 : *f1;
    else if (f1->exp > f2->exp)
	*ft = f1->sign ? *f1 : *f2;
    else if (f1->mant < f2->mant)
	*ft = f1->sign ? *f2 : *f1;
    else if (f1->mant > f2->mant)
	*ft = f1->sign ? *f1 : *f2;
    else
	*ft = *f2;
    if (ft->unorm == CLASS_FR0)
	ft->unorm = 64;
    return flags;
}


/**************************/
/* FP Comparison Routines */
/**************************/
static unsigned full_fcmpeq(const FREG *f1, const FREG *f2, BOOL *res,
			    BYTE traps)
{
    *res = NO;
    if (UNSUPP(f1) || SNAN(f1) || UNSUPP(f2) || SNAN(f2))
	return invInput(traps);
    if (QNAN(f1) || QNAN(f2))
	return 0;
    if (INF(f1) && INF(f2) && f1->sign == f2->sign)
	*res = YES;
    return unormChk2(f1, f2, traps);
}

static unsigned full_fcmp(const FREG *f1, const FREG *f2, BOOL *res, BYTE traps)
{
    *res = NO;
    return full_minmax(f1, f2, traps);
}

static unsigned fcmpUnordX(const FREG *f1, const FREG *f2, BOOL *res,
			   BYTE traps)
{
    if (UNSUPP(f1) || SNAN(f1) || UNSUPP(f2) || SNAN(f2)) {
	*res = YES;
	return invInput(traps);
    }
    if (QNAN(f1) || QNAN(f2)) {
	*res = YES;
	return 0;
    } else
	*res = NO;
    return unormChk2(f1, f2, traps);
}

static BOOL fcmpeqX(const FREG *f1, const FREG *f2)
{
    if (f1->unorm >= 64 && f2->unorm >= 64)
	return YES;
    if (f1->sign == f2->sign && f1->exp == f2->exp && f1->mant == f2->mant)
	return YES;
    return NO;
}

static BOOL fcmpltX(const FREG *f1, const FREG *f2)
{
    if (f1->unorm >= 64 && f2->unorm >= 64)
	return NO;
    else if (f1->sign > f2->sign)
	return YES;
    else if (f1->sign < f2->sign)
	return NO;
    else {	/* f1->sign == f2->sign */
	if (f1->exp < f2->exp)
	    return f1->sign ? NO : YES;
	else if (f1->exp > f2->exp)
	    return f1->sign ? YES : NO;
	else if (f1->mant < f2->mant)
	    return f1->sign ? NO : YES;
	else if (f1->mant > f2->mant)
	    return f1->sign ? YES : NO;
	else
	    return NO;
    }
}

unsigned fcmpEq(const FREG *f1, const FREG *f2, BOOL qual, BOOL *res1,
		BOOL *res2, BYTE traps)
{
    unsigned flags;

    if (!qual) {
	*res1 = NO;
	*res2 = NO;
	return 0;
    }
    if (f1->special || f2->special) {
	if (NATV(f1) || NATV(f2)) {
	    *res1 = NO;
	    *res2 = NO;
	    return 0;
	}
	if ((flags = full_fcmpeq(f1, f2, res1, traps)) & FP_FAULT)
	    return flags;
    } else {
	if ((flags = unormChk2(f1, f2, traps)) & FP_FAULT)
	    return flags;
	*res1 = fcmpeqX(f1, f2);
    }
    *res2 = !*res1;
    return flags;
}

unsigned fcmpLt(const FREG *f1, const FREG *f2, BOOL qual, BOOL *res1,
		BOOL *res2, BYTE traps)
{
    unsigned flags;

    if (!qual) {
	*res1 = NO;
	*res2 = NO;
	return 0;
    }
    if (f1->special || f2->special) {
	if (NATV(f1) || NATV(f2)) {
	    *res1 = NO;
	    *res2 = NO;
	    return 0;
	}
	if ((flags = full_fcmp(f1, f2, res1, traps)) & FP_FAULT)
	    return flags;
	/* Infinity checks */
	if (!f1->special && INF(f2))
	   *res1 = !f2->sign;
	else if (INF(f1) && !f2->special)
	   *res1 = f1->sign;
	else if (INF(f1) && INF(f2))
	   *res1 = f1->sign & !f2->sign;
    } else {
	if ((flags = unormChk2(f1, f2, traps)) & FP_FAULT)
	    return flags;
	*res1 = fcmpltX(f1, f2);
    }
    *res2 = !*res1;
    return flags;
}

unsigned fcmpLe(const FREG *f1, const FREG *f2, BOOL qual, BOOL *res1,
		BOOL *res2, BYTE traps)
{
    unsigned flags;

    if (!qual) {
	*res1 = NO;
	*res2 = NO;
	return 0;
    }
    if (f1->special || f2->special) {
	if (NATV(f1) || NATV(f2)) {
	    *res1 = NO;
	    *res2 = NO;
	    return 0;
	}
	if ((flags = full_fcmp(f1, f2, res1, traps)) & FP_FAULT)
	    return flags;
	/* Infinity checks */
	if (!f1->special && INF(f2))
	   *res1 = !f2->sign;
	else if (INF(f1) && !f2->special)
	   *res1 = f1->sign;
	else if (INF(f1) && INF(f2))
	   *res1 = f1->sign | !f2->sign;
    } else {
	if ((flags = unormChk2(f1, f2, traps)) & FP_FAULT)
	    return flags;
	*res1 = fcmpltX(f1, f2) || fcmpeqX(f1, f2);
    }
    *res2 = !*res1;
    return flags;
}

unsigned fcmpUnord(const FREG *f1, const FREG *f2, BOOL qual, BOOL *res1,
		   BOOL *res2, BYTE traps)
{
    unsigned flags;

    if (!qual) {
	*res1 = NO;
	*res2 = NO;
	return 0;
    }
    if (f1->special || f2->special) {
	if (NATV(f1) || NATV(f2)) {
	    *res1 = NO;
	    *res2 = NO;
	    return 0;
	}
	if ((flags = fcmpUnordX(f1, f2, res1, traps)) & FP_FAULT)
	    return flags;
    } else {
	if ((flags = unormChk2(f1, f2, traps)) & FP_FAULT)
	    return flags;
	*res1 = NO;
    }
    *res2 = !*res1;
    return flags;
}


/***********************/
/* Conversion Routines */
/***********************/
/* Rounds FP value in *f1 to integer, result in *ft, return inexact/fpa */
static unsigned fx(const FREG *f1, FREG *ft, RMODE rmode)
{
    BOOL last, guard, sticky;
    unsigned flags;
    int n;

    n = INTEGER_EXP - f1->exp;	/* 63 - unbiased exponent, >= 0 */
    if (!n) {
	ft->unorm = 0;
	ft->mant = f1->mant;
	return 0;
    }
    if (n == 1) {
	sticky = NO;
	guard  = f1->mant & 1;
	last   = (f1->mant >> 1) & 1;
    } else if (n < 64) {
	sticky = (f1->mant & ONES(n-1)) != 0;
	guard  = (f1->mant >> (n-1)) & 1;
	last   = (f1->mant >> n) & 1;
    } else if (n == 64) {
	sticky = (f1->mant << 1) != 0;
	guard  = YES;
	last   = NO;
    } else {	/* n > 64 */
	sticky = YES;
	guard  = NO;
	last   = NO;
    }
    if (incrMant(rmode, last, guard, sticky, f1->sign)) {
	flags = FPA_TRAP | I_MASK;
	if (n < 64) {
	    DWORD mant = (f1->mant >> n) + 1;

	    ft->mant = mant << (n-1);
	    if (SIGN(ft->mant)) {
		ft->unorm = n - 1;
		ft->exp = f1->exp + 1;
	    } else {
		ft->unorm = n;
		ft->exp = f1->exp;
		ft->mant <<= 1;
	    }
	} else {
	    ft->unorm = 63;
	    ft->exp = INTEGER_EXP - 63;
	    ft->mant = 1ULL << 63;
	}
    } else {
	flags = guard || sticky ? I_MASK : 0;
	if (n < 64) {
	    ft->unorm = n;
	    ft->exp = f1->exp;
	    ft->mant = (f1->mant >> n) << n;
	} else
	    ft->unorm = 64;
    }
    return flags;
}

unsigned fcvtfx(const FREG *f1, FREG *ft, BYTE traps, BYTE controls)
{
    unsigned flags;
    int n;

    if (NATV(f1)) {
	ft->special = YES;
	ft->class = CLASS_NAT;
	return 0;
    }
    ft->special = NO;
    ft->class = CLASS_NONE;
    ft->sign = 0;
    ft->exp = INTEGER_EXP;
    if (ZERO(f1)) {
	ft->unorm = 64;
	return unormChk1(f1, traps);
    }
    if (f1->special || f1->exp > INTEGER_EXP) {
	if (traps & V_MASK) {
	    ft->unorm = 0;
	    ft->mant = 0x8000000000000000ULL;
	    return V_MASK;
	} else
	    return V_FAULT;
    }
    flags = fx(f1, ft, RC(controls));
    if (ZERO(ft)) {
	if ((flags |= unormChk1(f1, traps)) & FP_FAULT)
	    return flags;
	if ((flags & I_MASK) && !(traps & I_MASK))
	    flags |= I_TRAP;
	return flags;
    }
    n = INTEGER_EXP - ft->exp;	/* 63 - unbiased exponent */
    if (n < 0 || (!n && (!f1->sign || (f1->sign && ft->mant != 1ULL<<63)))) {
	if (traps & V_MASK) {
	    ft->unorm = 0;
	    ft->exp = INTEGER_EXP;
	    ft->mant = 0x8000000000000000ULL;
	    return V_MASK;
	} else
	    return V_FAULT;
    }
    if ((flags |= unormChk1(f1, traps)) & FP_FAULT)
	return flags;
    if (f1->sign) {
	ft->unorm = 0;
	ft->exp = INTEGER_EXP;
	ft->mant = -(ft->mant >> n);
    }
    if ((flags & I_MASK) && !(traps & I_MASK))
	flags |= I_TRAP;
    return flags;
}

unsigned fcvtfu(const FREG *f1, FREG *ft, BYTE traps, BYTE controls)
{
    unsigned flags;
    int n;

    if (NATV(f1)) {
	ft->special = YES;
	ft->class = CLASS_NAT;
	return 0;
    }
    ft->special = NO;
    ft->class = CLASS_NONE;
    ft->sign = 0;
    ft->exp = INTEGER_EXP;
    if (ZERO(f1)) {
	ft->unorm = 64;
	return unormChk1(f1, traps);
    }
    if (f1->special || f1->exp > INTEGER_EXP) {
	if (traps & V_MASK) {
	    ft->unorm = 0;
	    ft->mant = 0x8000000000000000ULL;
	    return V_MASK;
	} else
	    return V_FAULT;
    }
    flags = fx(f1, ft, RC(controls));
    if (ZERO(ft)) {
	if ((flags |= unormChk1(f1, traps)) & FP_FAULT)
	    return flags;
	if ((flags & I_MASK) && !(traps & I_MASK))
	    flags |= I_TRAP;
	return flags;
    }
    n = INTEGER_EXP - ft->exp;	/* 63 - unbiased exponent */
    if (n < 0 || f1->sign) {
	if (traps & V_MASK) {
	    ft->unorm = 0;
	    ft->exp = INTEGER_EXP;
	    ft->mant = 0x8000000000000000ULL;
	    return V_MASK;
	} else
	    return V_FAULT;
    }
    if ((flags |= unormChk1(f1, traps)) & FP_FAULT)
	return flags;
    if ((flags & I_MASK) && !(traps & I_MASK))
	flags |= I_TRAP;
    return flags;
}

unsigned fpcvtfx(const FREG *f1, FREG *ft, BYTE traps, BYTE controls)
{
    unsigned flags;
    int n;

    ft->special = NO;
    ft->class = CLASS_NONE;
    ft->sign = 0;
    ft->exp = INTEGER_EXP;
    if (ZERO(f1)) {
	ft->unorm = 64;
	return 0;
    }
    if (f1->special || f1->exp > INTEGER_EXP-32) {
	if (traps & V_MASK) {
	    ft->unorm = 0;
	    ft->mant = 0x8000000000000000ULL;
	    return V_MASK;
	} else
	    return V_FAULT;
    }
    flags = fx(f1, ft, RC(controls));
    if (ZERO(ft)) {
	if ((flags |= unormChk1(f1, traps)) & FP_FAULT)
	    return flags;
	if ((flags & I_MASK) && !(traps & I_MASK))
	    flags |= I_TRAP;
	return flags;
    }
    n = INTEGER_EXP - ft->exp - 32;	/* 31 - unbiased exponent */
    if ((n < 0 || (!n && (!f1->sign || (f1->sign && ft->mant != 1ULL<<63))))){
	if (traps & V_MASK) {
	    ft->unorm = 0;
	    ft->mant = 0x8000000000000000ULL;
	    return V_MASK;
	} else
	    return V_FAULT;
    }
    if ((flags |= unormChk1(f1, traps)) & FP_FAULT)
	return flags;
    if (f1->sign) {
	ft->unorm = 0;
	ft->exp = INTEGER_EXP;
	ft->mant = -(ft->mant >> n);
    } else
	ft->unorm -= 32;
    if ((flags & I_MASK) && !(traps & I_MASK))
	flags |= I_TRAP;
    return flags;
}

unsigned fpcvtfu(const FREG *f1, FREG *ft, BYTE traps, BYTE controls)
{
    unsigned flags;
    int n;

    ft->special = NO;
    ft->class = CLASS_NONE;
    ft->sign = 0;
    ft->exp = INTEGER_EXP;
    if (ZERO(f1)) {
	ft->unorm = 64;
	return 0;
    }
    if (f1->special || f1->exp > INTEGER_EXP-32) {
	if (traps & V_MASK) {
	    ft->unorm = 0;
	    ft->mant = 0x8000000000000000ULL;
	    return V_MASK;
	} else
	    return V_FAULT;
    }
    flags = fx(f1, ft, RC(controls));
    if (ZERO(ft)) {
	if ((flags |= unormChk1(f1, traps)) & FP_FAULT)
	    return flags;
	if ((flags & I_MASK) && !(traps & I_MASK))
	    flags |= I_TRAP;
	return flags;
    }
    n = INTEGER_EXP - ft->exp - 32;	/* 31 - unbiased exponent */
    if (n < 0 || f1->sign) {
	if (traps & V_MASK) {
	    ft->unorm = 0;
	    ft->mant = 0x8000000000000000ULL;
	    return V_MASK;
	} else
	    return V_FAULT;
    }
    if ((flags |= unormChk1(f1, traps)) & FP_FAULT)
	return flags;
    ft->unorm -= 32;
    if ((flags & I_MASK) && !(traps & I_MASK))
	flags |= I_TRAP;
    return flags;
}

/* FP Classification */
BOOL fclass(const FREG *f1, DWORD cl)
{
    if (UNSUPP(f1))
	return NO;
    if (NATV(f1))
	return !!BitfX(cl,55,1);
    if (QNAN(f1))
	return !!BitfX(cl,56,1);
    if (SNAN(f1))
	return !!BitfX(cl,57,1);
    if ((!BitfX(cl,62,1) && f1->sign) || (!BitfX(cl,63,1) && !f1->sign))
	return NO;
    if (INF(f1))
	return !!BitfX(cl,58,1);
    if (UNORM(f1))
	return !!BitfX(cl,60,1);
    if (ZERO(f1))
	return !!BitfX(cl,61,1);
    /* Must be a normal number */
    return !!BitfX(cl,59,1);
}
