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

#include <sys/types.h>
#include <elf.h>
#include <string.h>
#include <unistd.h>

#include "std.h"
#include "types.h"
#include "osload.h"
#include "state.h"
#include "libcore.h"
#include "sim.h"
#include "memory.h"
#include "fields.h"

extern char **environ;

#define BSP64_ADDR	0x9FFFFFFF7F600000ULL

int
os_elf32_abi(char *ident, Elf32_Ehdr *ehdr, int set)
{
	/* No ILP32. */
	return (0);
}

int
os_elf64_abi(char *ident, Elf64_Ehdr *ehdr, int set)
{
	extern BOOL force_user, force_system;
	int region, userland;

	if (ident[EI_OSABI] != ELFOSABI_SYSV)
		return (0);

	region = ehdr->e_entry >> 61;
	userland = !force_system &&
		(force_user || (ehdr->e_type == ET_DYN || region > 0));
	if (set) {
		unixABI = userland;
	} else {
		if (unixABI != userland)
			return (0);
	}

	return (1);
}

ADDR
os_rtld32_text(Elf32_Phdr *phdr)
{
	/* No ILP32. */
	return (0);
}

ADDR
os_rtld32_data(Elf32_Phdr *phdr)
{
	/* No ILP32. */
	return (0);
}

ADDR
os_rtld64_text(Elf64_Phdr *phdr)
{
	return (0x2000000000000000ULL);
}

ADDR
os_rtld64_data(Elf64_Phdr *phdr)
{
	return (os_rtld64_text(phdr) + phdr->p_vaddr);
}

/* On Linux, the stack is layed out like this:

	+------------+
	|   strings  |
	+------------+
	| AUX table  |
	+------------+
	|    NULL    |
	+------------+
	|   env[N]   |
	+------------+
	/	     /
	/	     /
	+------------+
	|    NULL    |
	+------------+
	|   arg[N]   |
	+------------+
	/	     /
	/	     /
	+------------+
	|   arg[0]   |
	+------------+
	|   argc     |
	+------------+
*/

#define	NEW_AUX_ENT(name,value)					\
    do {							\
	aux_sz += 16;						\
	pmemLookup_p(page_base(str_p - aux_sz));		\
	memMWrt(str_p - aux_sz + 0, 8, (name));	\
	memMWrt(str_p - aux_sz + 8, 8, (value));	\
    } while (0)

#ifndef AT_CLKTCK
#define	AT_CLKTCK	17	/* XXX fix me when v2.4 linux is wide-spread */
#endif

int
os_setup_process(const char *file_name, int s_argc, char *s_argv[],
    struct os_proc *proc)
{
	ADDR arg_p, sp, str_p;
	size_t argv_sz, aux_sz, env_sz;
	int i, n_env;

	sp = 0xA000000000000000ULL;
	setMaxSP(sp);

	for (argv_sz = i = 0; i < s_argc; i++)
		argv_sz += strlen(s_argv[i]) + 1;
	for (env_sz = i = 0; environ[i]; i++)
		env_sz += strlen(environ[i]) + 1;
	n_env = i;

	str_p = (sp - (argv_sz + env_sz)) & ~(ADDR)0xf;
	if ((s_argc + n_env + 3) & 1)
		str_p -= 8;
	aux_sz = 0;

	NEW_AUX_ENT(AT_NULL, 0);
	NEW_AUX_ENT(AT_HWCAP, 0);
	NEW_AUX_ENT(AT_PAGESZ, page_size);
	NEW_AUX_ENT(AT_CLKTCK, sysconf(_SC_CLK_TCK));
	if (proc->has_rtld) {
		NEW_AUX_ENT(AT_PHDR, proc->phdr_addr);
		NEW_AUX_ENT(AT_PHENT, sizeof (Elf64_Phdr));
		NEW_AUX_ENT(AT_PHNUM, proc->phdr_count);
		NEW_AUX_ENT(AT_BASE, proc->rtld_base);
		NEW_AUX_ENT(AT_FLAGS, 0);
		NEW_AUX_ENT(AT_ENTRY, proc->proc_entry);
		NEW_AUX_ENT(AT_UID, getuid());
		NEW_AUX_ENT(AT_EUID, geteuid());
		NEW_AUX_ENT(AT_GID, getgid());
		NEW_AUX_ENT(AT_EGID, getegid());
	}

	arg_p = (str_p - aux_sz - ((s_argc + n_env + 3) * 8));
	grSet(0, SP_ID, arg_p - 16);	/* leave 16 byte scratch space */
	pmemLookup_p(page_base(arg_p));
	memMWrt(arg_p, 8, s_argc);
	arg_p += 8;
	for (i = 0; i < s_argc; i++) {
		size_t size = strlen(s_argv[i]) + 1;
		pmemLookup_p(page_base(str_p));
		pmemLookup_p(page_base(str_p + size));
		pmemLookup_p(page_base(arg_p));
		memBBWrt(str_p, (BYTE *)s_argv[i], size);
		memMWrt(arg_p, 8, str_p);
		str_p += size;
		arg_p += 8;
	}
	memMWrt(arg_p, 8, 0);
	arg_p += 8;

	for (i = 0; environ[i]; i++) {
		pmemLookup_p(page_base(str_p));
		pmemLookup_p(page_base(arg_p));
		memBBWrt(str_p, (BYTE *)environ[i],
		    strlen(environ[i]) + 1);
		memMWrt(arg_p, 8, str_p);
		str_p += strlen(environ[i]) + 1;
		arg_p += 8;
	}
	memMWrt(arg_p, 8, 0);

	/* Backing Store */
	(void)arSet(curPid, BSP_ID, BSP64_ADDR);
	(void)arSet(curPid, BSPST_ID, BSP64_ADDR);

	return (0);
}
