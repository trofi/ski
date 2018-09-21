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

BOOL illpredecode(DWORD, INSTINFO *);
BOOL illQPpredecode(DWORD, INSTINFO *);
BOOL illBQPpredecode(DWORD, INSTINFO *);
BOOL ignpredecode(DWORD, INSTINFO *);
BOOL lUnitImm41predecode(DWORD, INSTINFO *);
BOOL A1predecode(DWORD, INSTINFO *);
BOOL A2predecode(DWORD, INSTINFO *);
BOOL A3predecode(DWORD, INSTINFO *);
BOOL A4predecode(DWORD, INSTINFO *);
BOOL A5predecode(DWORD, INSTINFO *);
BOOL A6predecode(DWORD, INSTINFO *);
BOOL A7predecode(DWORD, INSTINFO *);
BOOL A8predecode(DWORD, INSTINFO *);
BOOL A9predecode(DWORD, INSTINFO *);
BOOL A10predecode(DWORD, INSTINFO *);
BOOL I1predecode(DWORD, INSTINFO *);
BOOL I2predecode(DWORD, INSTINFO *);
BOOL I3predecode(DWORD, INSTINFO *);
BOOL I4predecode(DWORD, INSTINFO *);
BOOL I5predecode(DWORD, INSTINFO *);
BOOL I6predecode(DWORD, INSTINFO *);
BOOL I7predecode(DWORD, INSTINFO *);
BOOL I8predecode(DWORD, INSTINFO *);
BOOL I9predecode(DWORD, INSTINFO *);
BOOL I10predecode(DWORD, INSTINFO *);
BOOL I11predecode(DWORD, INSTINFO *);
BOOL I12predecode(DWORD, INSTINFO *);
BOOL I13predecode(DWORD, INSTINFO *);
BOOL I14predecode(DWORD, INSTINFO *);
BOOL I15predecode(DWORD, INSTINFO *);
BOOL I16predecode(DWORD, INSTINFO *);
BOOL I17predecode(DWORD, INSTINFO *);
BOOL I18predecode(DWORD, INSTINFO *);
BOOL I19predecode(DWORD, INSTINFO *);
BOOL I20predecode(DWORD, INSTINFO *);
BOOL I21predecode(DWORD, INSTINFO *);
BOOL I22predecode(DWORD, INSTINFO *);
BOOL I23predecode(DWORD, INSTINFO *);
BOOL I24predecode(DWORD, INSTINFO *);
BOOL I25predecode(DWORD, INSTINFO *);
BOOL I26predecode(DWORD, INSTINFO *);
BOOL I27predecode(DWORD, INSTINFO *);
BOOL I28predecode(DWORD, INSTINFO *);
BOOL I29predecode(DWORD, INSTINFO *);
BOOL M1predecode(DWORD, INSTINFO *);
BOOL M2predecode(DWORD, INSTINFO *);
BOOL M3predecode(DWORD, INSTINFO *);
BOOL M4predecode(DWORD, INSTINFO *);
BOOL M5predecode(DWORD, INSTINFO *);
BOOL M6predecode(DWORD, INSTINFO *);
BOOL M7predecode(DWORD, INSTINFO *);
BOOL M8predecode(DWORD, INSTINFO *);
BOOL M9predecode(DWORD, INSTINFO *);
BOOL M10predecode(DWORD, INSTINFO *);
BOOL M11predecode(DWORD, INSTINFO *);
BOOL M12predecode(DWORD, INSTINFO *);
BOOL M13predecode(DWORD, INSTINFO *);
BOOL M14predecode(DWORD, INSTINFO *);
BOOL M15predecode(DWORD, INSTINFO *);
BOOL M16predecode(DWORD, INSTINFO *);
BOOL M17predecode(DWORD, INSTINFO *);
BOOL M18predecode(DWORD, INSTINFO *);
BOOL M19predecode(DWORD, INSTINFO *);
BOOL M20predecode(DWORD, INSTINFO *);
BOOL M21predecode(DWORD, INSTINFO *);
BOOL M22predecode(DWORD, INSTINFO *);
BOOL M23predecode(DWORD, INSTINFO *);
BOOL M24predecode(DWORD, INSTINFO *);
BOOL M25predecode(DWORD, INSTINFO *);
BOOL M26predecode(DWORD, INSTINFO *);
BOOL M27predecode(DWORD, INSTINFO *);
BOOL M28predecode(DWORD, INSTINFO *);
BOOL M29predecode(DWORD, INSTINFO *);
BOOL M30predecode(DWORD, INSTINFO *);
BOOL M31predecode(DWORD, INSTINFO *);
BOOL M32predecode(DWORD, INSTINFO *);
BOOL M33predecode(DWORD, INSTINFO *);
BOOL M34predecode(DWORD, INSTINFO *);
BOOL M35predecode(DWORD, INSTINFO *);
BOOL M36predecode(DWORD, INSTINFO *);
BOOL M37predecode(DWORD, INSTINFO *);
BOOL M38predecode(DWORD, INSTINFO *);
BOOL M39predecode(DWORD, INSTINFO *);
BOOL M40predecode(DWORD, INSTINFO *);
BOOL M41predecode(DWORD, INSTINFO *);
BOOL M42predecode(DWORD, INSTINFO *);
BOOL M43predecode(DWORD, INSTINFO *);
BOOL M44predecode(DWORD, INSTINFO *);
BOOL M45predecode(DWORD, INSTINFO *);
BOOL M46predecode(DWORD, INSTINFO *);
BOOL M47predecode(DWORD, INSTINFO *);
BOOL M48predecode(DWORD, INSTINFO *);
BOOL B1predecode(DWORD, INSTINFO *);
BOOL B2predecode(DWORD, INSTINFO *);
BOOL B3predecode(DWORD, INSTINFO *);
BOOL B4predecode(DWORD, INSTINFO *);
BOOL B5predecode(DWORD, INSTINFO *);
BOOL B6predecode(DWORD, INSTINFO *);
BOOL B7predecode(DWORD, INSTINFO *);
BOOL B8predecode(DWORD, INSTINFO *);
BOOL B9predecode(DWORD, INSTINFO *);
BOOL F1predecode(DWORD, INSTINFO *);
BOOL F2predecode(DWORD, INSTINFO *);
BOOL F3predecode(DWORD, INSTINFO *);
BOOL F4predecode(DWORD, INSTINFO *);
BOOL F5predecode(DWORD, INSTINFO *);
BOOL F6predecode(DWORD, INSTINFO *);
BOOL F7predecode(DWORD, INSTINFO *);
BOOL F8predecode(DWORD, INSTINFO *);
BOOL F9predecode(DWORD, INSTINFO *);
BOOL F10predecode(DWORD, INSTINFO *);
BOOL F11predecode(DWORD, INSTINFO *);
BOOL F12predecode(DWORD, INSTINFO *);
BOOL F13predecode(DWORD, INSTINFO *);
BOOL F14predecode(DWORD, INSTINFO *);
BOOL F15predecode(DWORD, INSTINFO *);
BOOL F16predecode(DWORD, INSTINFO *);
BOOL X1predecode(DWORD, INSTINFO *);
BOOL X2predecode(DWORD, INSTINFO *);
BOOL X3predecode(DWORD, INSTINFO *);
BOOL X4predecode(DWORD, INSTINFO *);
BOOL X5predecode(DWORD, INSTINFO *);
