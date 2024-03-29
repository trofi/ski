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
#include <sys/elf32.h>
#include <sys/elf64.h>
#include <sys/exec.h>
#include <machine/elf.h>

#include "std.h"
#include "types.h"
#include "osload.h"
#include "state.h"
#include "fields.h"
#include "sim.h"
#include "simmem.h"

extern char **environ;

int
os_elf32_abi(char *ident, Elf32_Ehdr *ehdr, int set)
{
	/* No ILP32. */
	return (0);
}

int
os_elf64_abi(char *ident, Elf64_Ehdr *ehdr, int set)
{
	int region;
	BOOL abi;

	if (ident[EI_OSABI] != ELFOSABI_FREEBSD)
		return (0);

	region = ehdr->e_entry >> 61;
	abi = (ehdr->e_type == ET_DYN || (region > 0 && region < 5))
	    ? YES : NO;
	if (set)
		unixABI = abi;
	else if (unixABI != abi)
		return (0);

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

/*
 * Initial stack layout:
 *	USRSTACK		+------------------------+
 *				|    struct ps_strings   |
 *	PS_STRINGS		+------------------------+
 *				| signal trampoline code |
 *				+------------------------+
 *				|     SPARE_USRSPACE     |
 *				+------------------------+
 *				|   arg & env strings    |
 *				+------------------------+
 *				|      AUX entries       |
 *				+------------------------+
 *				|   environ[envc + 1]    |
 *				+------------------------+
 *				|     argv[argc + 1]     |
 *				+------------------------+
 *				|          argc          |
 *	SP			+------------------------+
 *
 * AUX entries required by rtld:
 *	AT_BASE
 *	AT_ENTRY
 *	AT_PHDR
 *	AT_PHENT
 *	AT_PHNUM
 */
struct ia64_ps_strings {
	uint64_t	ps_argvstr;	/* VA of first argument string. */
	int		ps_nargvstr;	/* Number of argument strings. */
	int		ps_pad0;
	uint64_t	ps_envstr;	/* VA of first environment string. */
	int		ps_nenvstr;	/* Number of environment strings. */
	int		ps_pad1;
};

#define	AUX(sp, tag, ptr)			\
	do {					\
		(void)memWrt8(sp, tag);		\
		(void)memWrt8(sp + 8, ptr);	\
		sp += 16;			\
	} while (0)

int
os_setup_process(const char *file_name, int argc, char *argv[],
    struct os_proc *proc)
{
	struct ia64_ps_strings ps_strings;
	ADDR sp, strings, strp;
	size_t argsz, envsz, strsz;
	int envc, i;

	/* Initialize FP state (architected default value). */
	arSet(0, FPSR_ID, 0x0009804c0270033fULL);

	sp = 0xA000000000000000ULL;
	sp -= 1048576;			/* 1MB gateway page for EPC support. */
	setMaxSP(sp);			/* =USRSTACK */

	sp -= sizeof(ps_strings);
	strings = sp;			/* =PS_STRINGS */

	/* XXX no signal trampoline code. */

	sp -= SPARE_USRSPACE;

	argsz = 0;
	for (i = 0; i < argc; i++)
		argsz += strlen(argv[i]) + 1;
	envsz = 0;
	for (envc = 0; environ[envc]; envc++)
		envsz += strlen(environ[envc]) + 1;
	sp -= (argsz + envsz + 7) & ~7;
	strp = sp;

	if (proc->has_rtld)
		sp -= 6 * 16;			/* AUX entries */

	sp -= (envc + 1) * 8;			/* envp[] */
	ps_strings.ps_envstr = sp;
	ps_strings.ps_nenvstr = envc;
	sp -= (argc + 1) * 8;			/* argv[] */
	ps_strings.ps_argvstr = sp;
	ps_strings.ps_nargvstr = argc;

	sp -= 8;				/* argc */

	grSet(0, 32, sp);
	grSet(0, 33, strings);
	grSet(0, 34, 0);

	sp &= ~15;
	grSet(0, SP_ID, sp - 16);

	/* Map all pages. */
	sp = page_base(sp);
	do {
		pmemLookup_p(sp);
		sp += page_size;
	} while (sp < getMaxSP());

	/* Copy out arguments. */
	sp = ps_strings.ps_argvstr;
	memWrt8(sp - 8, argc);
	for (i = 0; i < argc; i++) {
		strsz = strlen(argv[i]) + 1;
		memBBWrt(strp, (BYTE *)argv[i], strsz);
		memWrt8(sp, strp);
		strp += strsz;
		sp += 8;
	}
	memWrt8(sp, 0);
	sp += 8;

	/* Copy out environment strings. */
	for (i = 0; i < envc; i++) {
		strsz = strlen(environ[i]) + 1;
		memBBWrt(strp, (BYTE *)environ[i], strsz);
		memWrt8(sp, strp);
		strp += strsz;
		sp += 8;
	}
	memWrt8(sp, 0);
	sp += 8;

	if (proc->has_rtld) {
		AUX(sp, AT_BASE, proc->rtld_base);
		AUX(sp, AT_ENTRY, proc->proc_entry);
		AUX(sp, AT_PHDR, proc->phdr_addr);
		AUX(sp, AT_PHENT, sizeof(Elf64_Phdr));
		AUX(sp, AT_PHNUM, proc->phdr_count);
		AUX(sp, AT_NULL, NULL);
	}

	/* Copy out ps_strings. */
	memBBWrt(strings, (BYTE *)&ps_strings, sizeof(ps_strings));

	/* Set backing store. */
	arSet(0, BSP_ID,   0x9FFFFFFF80000000ULL);
	arSet(0, BSPST_ID, 0x9FFFFFFF80000000ULL);

	return (0);
}
