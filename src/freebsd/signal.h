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
#ifndef _SKI_SIGNAL_H
#define _SKI_SIGNAL_H

typedef long long	ptr64 __attribute__((aligned(8)));

struct sigaction64 {
	union {
		ptr64	__sa_handler;
		ptr64	__sa_sigaction;
	} __sigaction_u;
	int	sa_flags;
	sigset_t sa_mask;
};

struct siginfo64 {
	int	si_signo;
	int	si_errno;
	int	si_code;
	int	si_pid;
	int	si_uid;
	int	si_status;
	ptr64	si_addr;
	union {
		int	sigval_int;
		ptr64	sigval_ptr;
	} si_value;
	long	si_band;
	int	__spare__[7];
};

int signal_get_handler(int, struct sigaction64 *);
int signal_set_handler(int, struct sigaction64 *);

#endif	/* _SKI_SIGNAL_H */
