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
#ifndef _SKI_OSLOAD_H
#define _SKI_OSLOAD_H

#include "types.h"

struct os_proc
{
	int	has_rtld;
	int	phdr_count;
	ADDR	phdr_addr;
	ADDR	rtld_base;
	ADDR	proc_entry;
	unsigned int ehdr_flags;
	ADDR	aout_base;
	ADDR	aout_phdr;
};

int	os_elf32_abi(char *, Elf32_Ehdr *, int);
int	os_elf64_abi(char *, Elf64_Ehdr *, int);

/* TODO: can we get rid of _data/_text in favour of _bias on ELF?. */
ADDR	os_rtld32_data(Elf32_Phdr *);
ADDR	os_rtld32_text(Elf32_Phdr *);
ADDR	os_rtld64_data(Elf64_Phdr *);
ADDR	os_rtld64_text(Elf64_Phdr *);

ADDR	os_rtld32_bias(void);
ADDR	os_rtld64_bias(void);
int	os_setup_process(const char *, int, char **, struct os_proc *);

#endif	/* _SKI_OSLOAD_H */
