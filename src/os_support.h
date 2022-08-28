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
#ifndef _SKI_OS_SUPPORT_H
#define _SKI_OS_SUPPORT_H

#include "std.h"
#include "types.h"

typedef struct {
	WORD	simSec;
	WORD	simUsec;
	WORD	simMinuteswest;
	WORD	simDsttime;
} SimTime;

BOOL setInitialTime(char *datestr);
void getSimTime(SimTime *st);

int simroot(REG arg, char *path, int creat);

#endif	/* _SKI_OS_SUPPORT_H */
