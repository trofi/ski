/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Standard Header
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
#ifndef _SKI_STD_H
#define _SKI_STD_H

/* Standard header to be included in all source files */

#if defined HAVE_CONFIG_H
# include "config.h"
#endif /* defined HAVE_CONFIG_H */

/* Are these 2 really necessary for all files? */
#include <stdlib.h>	/* used for *alloc, free, exit, atoi, atof, atol,
			     strtod, strtol, strtoul, getenv */
#include <stdio.h>	/* used for *printf, *scanf, fopen, fclose, fflush,
			     setbuf, fget*, fput*, get*, put*, unget*, fread,
			     fwrite, perror */

typedef enum {NO, YES}	BOOL;

#define BEL		'\a'

/* Do we have strtoull or __strtoull? */
#if HAVE_STRTOULL
extern unsigned long long strtoull(const char *, char **, int);
#else /* ! HAVE_STRTOULL */
# if HAVE___STRTOULL
extern unsigned long long __strtoull(const char *, char **, int);
#   define strtoull __strtoull
# else /* ! HAVE___STRTOULL */
#   error "You do not have strtoull!"
# endif /* ! HAVE___STRTOULL */
#endif /* ! HAVE_STRTOULL */

/* What is the host endiannes? */
#if defined ENDIANESS_IN_SYS_PARAM_H
# include <sys/types.h>
# include <sys/param.h>
#else /* !defined ENDIANESS_IN_SYS_PARAM_H */
# if !defined HAVE_CONFIG_H
#   warning "Assuming BIG_ENDIAN"
/* In case we do not have config.h assume BIG_ENDIAN */
#   define BIG_ENDIAN 4321
#   define LITTLE_ENDIAN 1234
#   define BYTE_ORDER BIG_ENDIAN
# else
#  define BIG_ENDIAN 4321
#  define LITTLE_ENDIAN 1234
#  ifdef WORDS_BIGENDIAN
#   define BYTE_ORDER BIG_ENDIAN
#  else
#   define BYTE_ORDER LITTLE_ENDIAN
#  endif
# endif /* !defined HAVE_CONFIG_H */
#endif /* !defined ENDIANESS_IN_SYS_PARAM_H */

#endif	/* _SKI_STD_H */
