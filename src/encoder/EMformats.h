/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* DO NOT EDIT - Automatically generated Thu Aug 23 21:05:40 MDT 2007 using:
 gawk ../../src/encodings/encoding.format
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


/* $Header$ */

/* EM instruction formats header */

#ifndef EMFORMATS_H
#define EMFORMATS_H

typedef enum {
    EM_FORMAT_NONE = 0,
    EM_FORMAT_A1,
    EM_FORMAT_A2,
    EM_FORMAT_A3,
    EM_FORMAT_A4,
    EM_FORMAT_A5,
    EM_FORMAT_A6,
    EM_FORMAT_A7,
    EM_FORMAT_A8,
    EM_FORMAT_A9,
    EM_FORMAT_A10,
    EM_FORMAT_I1,
    EM_FORMAT_I2,
    EM_FORMAT_I3,
    EM_FORMAT_I4,
    EM_FORMAT_I5,
    EM_FORMAT_I6,
    EM_FORMAT_I7,
    EM_FORMAT_I8,
    EM_FORMAT_I9,
    EM_FORMAT_I10,
    EM_FORMAT_I11,
    EM_FORMAT_I12,
    EM_FORMAT_I13,
    EM_FORMAT_I14,
    EM_FORMAT_I15,
    EM_FORMAT_I16,
    EM_FORMAT_I17,
    EM_FORMAT_I18,
    EM_FORMAT_I19,
    EM_FORMAT_I20,
    EM_FORMAT_I21,
    EM_FORMAT_I22,
    EM_FORMAT_I23,
    EM_FORMAT_I24,
    EM_FORMAT_I25,
    EM_FORMAT_I26,
    EM_FORMAT_I27,
    EM_FORMAT_I28,
    EM_FORMAT_I29,
    EM_FORMAT_M1,
    EM_FORMAT_M2,
    EM_FORMAT_M3,
    EM_FORMAT_M4,
    EM_FORMAT_M5,
    EM_FORMAT_M6,
    EM_FORMAT_M7,
    EM_FORMAT_M8,
    EM_FORMAT_M9,
    EM_FORMAT_M10,
    EM_FORMAT_M11,
    EM_FORMAT_M12,
    EM_FORMAT_M13,
    EM_FORMAT_M14,
    EM_FORMAT_M15,
    EM_FORMAT_M16,
    EM_FORMAT_M17,
    EM_FORMAT_M18,
    EM_FORMAT_M19,
    EM_FORMAT_M20,
    EM_FORMAT_M21,
    EM_FORMAT_M22,
    EM_FORMAT_M23,
    EM_FORMAT_M24,
    EM_FORMAT_M25,
    EM_FORMAT_M26,
    EM_FORMAT_M27,
    EM_FORMAT_M28,
    EM_FORMAT_M29,
    EM_FORMAT_M30,
    EM_FORMAT_M31,
    EM_FORMAT_M32,
    EM_FORMAT_M33,
    EM_FORMAT_M34,
    EM_FORMAT_M35,
    EM_FORMAT_M36,
    EM_FORMAT_M37,
    EM_FORMAT_M38,
    EM_FORMAT_M39,
    EM_FORMAT_M40,
    EM_FORMAT_M41,
    EM_FORMAT_M42,
    EM_FORMAT_M43,
    EM_FORMAT_M44,
    EM_FORMAT_M45,
    EM_FORMAT_M46,
    EM_FORMAT_M47,
    EM_FORMAT_M48,
    EM_FORMAT_B1,
    EM_FORMAT_B2,
    EM_FORMAT_B3,
    EM_FORMAT_B4,
    EM_FORMAT_B5,
    EM_FORMAT_B6,
    EM_FORMAT_B7,
    EM_FORMAT_B8,
    EM_FORMAT_B9,
    EM_FORMAT_F1,
    EM_FORMAT_F2,
    EM_FORMAT_F3,
    EM_FORMAT_F4,
    EM_FORMAT_F5,
    EM_FORMAT_F6,
    EM_FORMAT_F7,
    EM_FORMAT_F8,
    EM_FORMAT_F9,
    EM_FORMAT_F10,
    EM_FORMAT_F11,
    EM_FORMAT_F12,
    EM_FORMAT_F13,
    EM_FORMAT_F14,
    EM_FORMAT_F15,
    EM_FORMAT_F16,
    EM_FORMAT_X1,
    EM_FORMAT_X2,
    EM_FORMAT_X3,
    EM_FORMAT_X4,
    EM_FORMAT_X5,
    EM_FORMAT_X41
} Format;

#define NUM_FORMATS 118

#endif /* EMFORMATS_H */
