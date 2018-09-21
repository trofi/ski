/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* DO NOT EDIT - Automatically generated using:
     awk -f fmtimm.awk encodings/encoding.format | awk -f predecode.awk -
 */
/*
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

#include "sign_ext.h"
#define IP (imm64 & ~(ADDR)0xF)
#define PG(a) ((a) & ~(ADDR)0xFFF)
#define count2A2(ct2d) (ct2d + 1)
#define imm8A3(imm7b,s) (sign_ext(s << 7 | imm7b, 8))
#define imm14A4(imm7b,imm6d,s) (sign_ext(s << 13 | imm6d << 7 | imm7b, 14))
#define imm22A5(imm7b,imm5c,imm9d,s) (sign_ext(s << 21 | imm5c << 16 | imm9d << 7 | imm7b, 22))
#define imm8A8(imm7b,s) (sign_ext(s << 7 | imm7b, 8))
#define count2A10(ct2d) ((ct2d > 2) ? reservedQP : ct2d + 1)
#define count2I1(ct2d) ((ct2d == 0) ? 0 : (ct2d == 1) ? 7 : (ct2d == 2) ? 15 : 16)
#define mbtype4I3(mbt4c) ((mbt4c == 0) ? _brcst : (mbt4c == 8) ? _mix : (mbt4c == 9) ? _shuf : (mbt4c ==  0xA) ? _alt : (mbt4c == 0xB) ? _rev : reservedQP)
#define mhtype8I4(mht8c) (mht8c)
#define count5I6(count5b) (count5b)
#define count5I8(ccount5c) (31 - ccount5c)
#define count6I10(count6d) (count6d)
#define len6I11(pos6b,len6d) (len6d + 1)
#define pos6I11(pos6b,len6d) (pos6b)
#define len6I12(cpos6c,len6d) (len6d + 1)
#define pos6I12(cpos6c,len6d) (63 - cpos6c)
#define len6I13(imm7b,cpos6c,len6d,s) (len6d + 1)
#define pos6I13(imm7b,cpos6c,len6d,s) (63 - cpos6c)
#define imm8I13(imm7b,cpos6c,len6d,s) (sign_ext(s << 7 | imm7b, 8))
#define len6I14(cpos6b,len6d,s) (len6d + 1)
#define pos6I14(cpos6b,len6d,s) (63 - cpos6b)
#define imm1I14(cpos6b,len6d,s) (sign_ext(s, 1))
#define len4I15(len4d,cpos6d) (len4d + 1)
#define pos6I15(len4d,cpos6d) (63 - cpos6d)
#define pos6I16(pos6b) (pos6b)
#define imm21I18(imm20a,i) (i << 20 | imm20a)
#define imm21I19(imm20a,i) (i << 20 | imm20a)
#define target25I20(imm7a,imm13c,s) (IP + (sign_ext(s << 20 | imm13c << 7 | imm7a, 21) << 4))
#define tag13I21(timm9c) (IP + (sign_ext(timm9c, 9) << 4))
#define mask17I23(mask7a,mask8c,s) (sign_ext(s << 16 | mask8c << 8 | mask7a << 1, 17))
#define imm44I24(imm27a,s) (sign_ext(s << 43 | imm27a << 16, 44))
#define imm8I27(imm7b,s) (sign_ext(s << 7 | imm7b, 8))
#define imm9M3(imm7b,i,s) (sign_ext(s << 8 | i << 7 | imm7b, 9))
#define imm9M5(imm7a,i,s) (sign_ext(s << 8 | i << 7 | imm7a, 9))
#define imm9M8(imm7b,i,s) (sign_ext(s << 8 | i << 7 | imm7b, 9))
#define imm9M10(imm7a,i,s) (sign_ext(s << 8 | i << 7 | imm7a, 9))
#define imm9M15(imm7b,i,s) (sign_ext(s << 8 | i << 7 | imm7b, 9))
#define inc3M17(i2b,s) (sign_ext(((s) ? -1 : 1) * ((i2b == 3) ? 1 : 1 << (4 - i2b)), 6))
#define target25M20(imm7a,imm13c,s) (IP + (sign_ext(s << 20 | imm13c << 7 | imm7a, 21) << 4))
#define target25M21(imm7a,imm13c,s) (IP + (sign_ext(s << 20 | imm13c << 7 | imm7a, 21) << 4))
#define target25M22(imm20b,s) (IP + (sign_ext(s << 20 | imm20b, 21) << 4))
#define target25M23(imm20b,s) (IP + (sign_ext(s << 20 | imm20b, 21) << 4))
#define imm8M30(imm7b,s) (sign_ext(s << 7 | imm7b, 8))
#define ilM34(sof,sol,sor) (sol)
#define oM34(sof,sol,sor) (sof - sol)
#define rM34(sof,sol,sor) (sor << 3)
#define imm21M37(imm20a,i) (i << 20 | imm20a)
#define imm2M39(i2b) (i2b)
#define imm2M40(i2b) (i2b)
#define imm24M44(imm21a,i2d,i) (i << 23 | i2d << 21 | imm21a)
#define imm21M48(imm20a,i) (i << 20 | imm20a)
#define target25B1(imm20b,s) (IP + (sign_ext(s << 20 | imm20b, 21) << 4))
#define target25B2(imm20b,s) (IP + (sign_ext(s << 20 | imm20b, 21) << 4))
#define target25B3(imm20b,s) (IP + (sign_ext(s << 20 | imm20b, 21) << 4))
#define target25B6(timm7a,imm20b,t2e,s) (IP + (sign_ext(s << 20 | imm20b, 21) << 4))
#define tag13B6(timm7a,imm20b,t2e,s) (IP + (sign_ext(t2e << 7 | timm7a, 9) << 4))
#define tag13B7(timm7a,t2e) (IP + (sign_ext(t2e << 7 | timm7a, 9) << 4))
#define imm21B9(imm20a,i) (i << 20 | imm20a)
#define fclass9F5(fclass7c,fc2) (fclass7c << 2 | fc2)
#define amask7F12(amask7b,omask7c) (amask7b)
#define omask7F12(amask7b,omask7c) (omask7c)
#define target25F14(imm20a,s) (IP + (sign_ext(s << 20 | imm20a, 21) << 4))
#define imm21F15(imm20a,i) (i << 20 | imm20a)
#define imm21F16(imm20a,i) (i << 20 | imm20a)
#define imm62X1(imm41,imm20a,i) (imm41 << 21 | i << 20 | imm20a)
#define imm64X2(imm41,imm7b,ic,imm5c,imm9d,i) (i << 63 | imm41 << 22 | ic << 21 | imm5c << 16 | imm9d << 7 | imm7b)
#define target64X3(imm39,imm20b,i) (IP + ((i << 59 | imm39 << 20 | imm20b) << 4))
#define target64X4(imm39,imm20b,i) (IP + ((i << 59 | imm39 << 20 | imm20b) << 4))
#define imm62X5(imm41,imm20a,i) (imm41 << 21 | i << 20 | imm20a)
