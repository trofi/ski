/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Trace Library Header
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
#ifndef _SKI_PACKAGE_H
#define _SKI_PACKAGE_H


/**************************************************************************
 * File package constants
 **************************************************************************/

/* Defines for magic numbers */
#define SKI_PACKAGE_MAGIC	0xf4841436	/* magic_number */
#define SKI_PACKAGE_MAGIC_SP	0xf4841445	/* previous magic_number */
	/* SP: Small Package.  Kept for compatibility (6/99) */

/* Defines for type */
#define TRACE_PACKAGE	0x10000		/* trace records */
#define ELF_PACKAGE	0x20000		/* an elf package */
#define STATE_PACKAGE	0x30000		/* processor & system state */


/**************************************************************************
 * Auxiliary structs
 **************************************************************************/

typedef struct {
    unsigned short  version;		/* major version number */
    unsigned short  revision;		/* minor revision number */
} Package_Type;


/**************************************************************************
 * File package struct
 **************************************************************************/

typedef struct {
    unsigned		magic_number;	/* Marks as Ski Trace Package */
    Package_Type	type;		/* The type of package */
    unsigned long long	total_length;	/* Package length in bytes */
} Package_Header;

#if 0
/* Package_Header for SKI_PACKAGE_MAGIC_SP */
typedef struct {
    unsigned magic_number;	/* Marks as Ski Trace Package */
    Package_Type type;		/* The type of package */
    unsigned total_length;	/* Package length in bytes */
    unsigned reserved;		/* Reserved, should be 0 */
} Package_Header;
#endif

#endif	/* _SKI_PACKAGE_H */
