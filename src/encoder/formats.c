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

/* Instruction Format Names */

#include <encoder.h>
#include <EMInst.h>

static char *formatName[];

char *instFormatName(InstID instID)
{
    EMDB_info *inst_info;
    Format format;

    if (instID <= EM_INST_NONE || instID >= EM_INST_LAST)
	return formatName[EM_FORMAT_NONE];

    inst_info = &InstTable[instID];
    format = inst_info->format;
    return formatName[format];
}

static char *formatName[NUM_FORMATS+1] = {
    "???",
    "A1",
    "A2",
    "A3",
    "A4",
    "A5",
    "A6",
    "A7",
    "A8",
    "A9",
    "A10",
    "I1",
    "I2",
    "I3",
    "I4",
    "I5",
    "I6",
    "I7",
    "I8",
    "I9",
    "I10",
    "I11",
    "I12",
    "I13",
    "I14",
    "I15",
    "I16",
    "I17",
    "I18",
    "I19",
    "I20",
    "I21",
    "I22",
    "I23",
    "I24",
    "I25",
    "I26",
    "I27",
    "I28",
    "I29",
    "M1",
    "M2",
    "M3",
    "M4",
    "M5",
    "M6",
    "M7",
    "M8",
    "M9",
    "M10",
    "M11",
    "M12",
    "M13",
    "M14",
    "M15",
    "M16",
    "M17",
    "M18",
    "M19",
    "M20",
    "M21",
    "M22",
    "M23",
    "M24",
    "M25",
    "M26",
    "M27",
    "M28",
    "M29",
    "M30",
    "M31",
    "M32",
    "M33",
    "M34",
    "M35",
    "M36",
    "M37",
    "M38",
    "M39",
    "M40",
    "M41",
    "M42",
    "M43",
    "M44",
    "M45",
    "M46",
    "M47",
    "M48",
    "B1",
    "B2",
    "B3",
    "B4",
    "B5",
    "B6",
    "B7",
    "B8",
    "B9",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "F16",
    "X1",
    "X2",
    "X3",
    "X4",
    "X5",
    "X41"
};
