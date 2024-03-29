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
#ifndef _SKI_MACHDEP_H
#define _SKI_MACHDEP_H

#include "types.h"

#define	SKI_NFDS		20

typedef struct {
	ADDR	addr;
	WORD	len;
} SscDiskReq;

typedef struct {
	int	fd;
	WORD	count;
} SscDiskStat;

typedef struct {
	WORD	year;
	WORD	mon;
	WORD	mday;
	WORD	hour;
	WORD	min;
	WORD	sec;
	WORD	msec;
	WORD	wday;
} SscTime;

typedef struct {
	WORD	ssc_Sec;
	WORD	ssc_Usec;
} SscTimeval;

typedef struct {
	WORD	ssc_Minuteswest;
	WORD	ssc_Dsttime;
} SscTimezone;

#endif	/* _SKI_MACHDEP_H */
