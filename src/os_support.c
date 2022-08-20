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

#include <sys/time.h>
#include <string.h>
#include <unistd.h>

#include "std.h"
#include "types.h"
#include "sim.h"
#include "state.h"
#include "simmem.h"
#include "os_support.h"

extern char *sim_root;
extern size_t sim_root_len;

static SimTime simtime0;

BOOL
setInitialTime(char *datestr)
{
        struct timeval tv;
        struct timezone tz;

        if (datestr == NULL) {
                if (gettimeofday (&tv, &tz))
                        return (NO);

                simtime0.simSec = tv.tv_sec;
                simtime0.simUsec = tv.tv_usec;
                simtime0.simMinuteswest = tz.tz_minuteswest;
                simtime0.simDsttime = tz.tz_dsttime;
        } else
                return (NO);

        return (YES);
}

void
getSimTime(SimTime *st)
{
	st->simUsec = simtime0.simUsec + total_insts/mips;
	st->simSec = simtime0.simSec + st->simUsec/1000000U;
	st->simUsec %= 1000000U;
	st->simMinuteswest = simtime0.simMinuteswest;
	st->simDsttime = simtime0.simDsttime;
}

int
simroot(REG arg, char *path, int creat)
{
	char c, *p;

	p = path;
	memBBRd(arg, &c, 1);
	if (c == '/') {
		strcpy(p, sim_root);
		p += sim_root_len;
	}
	memBBRd(arg, p, 0);
	if (p == path)
		return (0);
	if (access(path, F_OK) != 0)
		memBBRd(arg, path, 0);
	return (0);
}
