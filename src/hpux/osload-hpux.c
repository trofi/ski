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

#include <elf.h>
#include <elf_em.h>
#include <string.h>

#include "std.h"
#include "types.h"
#include "osload.h"
#include "state.h"
#include "sim.h"
#include "libcore.h"
#include "fields.h"

extern char **environ;

#define	BSP32_ADDR	0x2000000062800000ULL
#define	BSP64_ADDR	0x9FFFFFFF7F600000ULL

#define	LI_TRACE	0x2

#define	SYSCALL_CODE	0xE000000000000000ULL

#define	ULD_TADDR	0x4000000080001000ULL
#define	ULD_DADDR	0x4000000080100000ULL

static int
os_elf_abi(int osabi, int set)
{
	BOOL abi;

	if (osabi != ELFOSABI_HPUX && osabi != ELFOSABI_STANDALONE)
		return (0);

	abi = (osabi == ELFOSABI_HPUX) ? YES : NO;
	if (set) 
		unixABI = abi;
	else if (unixABI != abi)
		return (0);

	return (1);
}

int
os_elf32_abi(char *ident, Elf32_Ehdr *ehdr, int set)
{
	return (os_elf_abi(ident[EI_OSABI], set));
}

int
os_elf64_abi(char *ident, Elf64_Ehdr *ehdr, int set)
{
	return (os_elf_abi(ident[EI_OSABI], set));
}

ADDR
os_rtld32_text(Elf32_Phdr *phdr)
{
        return (ULD_TADDR);
}

ADDR
os_rtld32_data(Elf32_Phdr *phdr)
{
	return (ULD_DADDR);
}

ADDR
os_rtld64_text(Elf64_Phdr *phdr)
{
        return (ULD_TADDR);
}

ADDR
os_rtld64_data(Elf64_Phdr *phdr)
{
	return (ULD_DADDR);
}

int
os_setup_process(const char *file_name, int s_argc, char *s_argv[],
    struct os_proc *proc)
{
	ADDR aout_path, aout_sz;
	ADDR addr, arg_p, arg_sz, base_sp, loadinfoAddr;
	ADDR sp, str_p, str_sz, sysvecAddr;
	size_t argv_sz, env_sz;
	unsigned int ptr_sz;
	int i, n_env;

	ptr_sz = (lp64) ? sizeof(DWORD) : sizeof(WORD);

	sp = (lp64) ? 0xA000000000000000ULL : 0x2000000080000000ULL;
	setMaxSP(sp);

	argv_sz = env_sz = 0;
	for (i = 0; i < s_argc; i++)
		argv_sz += strlen(s_argv[i]) + 1;
	for (i = 0; environ[i]; i++)
		env_sz += strlen(environ[i]) + 1;
	n_env = i;

	aout_sz = strlen(file_name) + 1;
	str_sz = argv_sz + env_sz + aout_sz;
	arg_sz = (s_argc + n_env + 2) * ptr_sz;
	arg_p = base_sp = (sp - (str_sz + arg_sz)) & ~(ADDR)0xF;
	str_p = base_sp + arg_sz;
	aout_path = sp - aout_sz;

	/* Allocate all necessary pages */
	for (addr = page_base(base_sp); addr < sp; addr += page_size)
		memZerosPageSet(addr);

	(void)grSet(curPid, SP_ID, base_sp);
	(void)grSet(curPid, 32, s_argc);
	(void)grSet(curPid, 33, arg_p);
	for (i = 0; i < s_argc; i++) {
		memBBWrt(str_p, (BYTE *)s_argv[i],
		    strlen(s_argv[i]) + 1);
		memMWrt(arg_p, ptr_sz, str_p);
		str_p += strlen(s_argv[i]) + 1;
		arg_p += ptr_sz;
	}
	memMWrt(arg_p, ptr_sz, 0);
	arg_p += ptr_sz;

	(void)grSet(curPid, 34, arg_p);
	for (i = 0; environ[i]; i++) {
		memBBWrt(str_p, (BYTE *)environ[i],
		    strlen(environ[i]) + 1);
		memMWrt(arg_p, ptr_sz, str_p);
		str_p += strlen(environ[i]) + 1;
		arg_p += ptr_sz;
	}
	memMWrt(arg_p, ptr_sz, 0);

	memBBWrt(aout_path, (BYTE *)file_name, aout_sz);

	if (lp64) {
		loadinfoAddr = 0xE000000000001000ULL;
		sysvecAddr   = 0xE000000000002000ULL;
	} else {
		loadinfoAddr = BSP32_ADDR - 0x10000ULL;
		sysvecAddr   = loadinfoAddr + 0x1000ULL;
	}
	memZerosPageSet(SYSCALL_CODE);
	memZerosPageSet(loadinfoAddr);
	memZerosPageSet(sysvecAddr);
	/* To support up to 1024 syscalls, need a second sysvec page */
	memZerosPageSet(sysvecAddr+page_size);
	(void)grSet(curPid, 35, loadinfoAddr);

	/* Syscall entrypoint table */
	/* XXX - in the future, might be only 512 syscalls (should also use
	   a #define for the max syscall number) */
	for (i = 0; i < 1024; i++)
		memMWrt(sysvecAddr + i*8, 8, SYSCALL_CODE);

	/* Handcoded bundle:
	   break.m         0x0
	   nop.f           0x0
	   br.ret.sptk.few b6;;
	*/
	memMWrt(SYSCALL_CODE+0, 8, 0x1D00000000000000ULL);
	memMWrt(SYSCALL_CODE+8, 8, 0x0002008060008400ULL);

	/* Fill in load_info struct */
	memMWrt(loadinfoAddr+0x00, 8, 1ULL);
	memMWrt(loadinfoAddr+0x08, 8, 0x90ULL);
	memMWrt(loadinfoAddr+0x10, 8, 0ULL);
	memMWrt(loadinfoAddr+0x18, 8, aout_path);
	memMWrt(loadinfoAddr+0x20, 8, proc->aout_base);
	memMWrt(loadinfoAddr+0x28, 8, proc->aout_phdr);
	memMWrt(loadinfoAddr+0x30, 8, ULD_TADDR);
	memMWrt(loadinfoAddr+0x38, 8, ULD_DADDR);
	memMWrt(loadinfoAddr+0x40, 8, 0ULL); /* user_sendsig text addr */
	memMWrt(loadinfoAddr+0x48, 8, 0ULL); /* user_sendsig text size */
	memMWrt(loadinfoAddr+0x50, 8, 0ULL); /* user_sendsig unwind info */
	memMWrt(loadinfoAddr+0x58, 8, (unsigned long long)LI_TRACE);
	memMWrt(loadinfoAddr+0x60, 8, 0ULL);	/* service manager */
	memMWrt(loadinfoAddr+0x68, 8, 0ULL);	/* DLD text */
	memMWrt(loadinfoAddr+0x70, 8, 0ULL);	/* DLD data */
	memMWrt(loadinfoAddr+0x78, 8, 0ULL);	/* RSE backing store start */
	memMWrt(loadinfoAddr+0x80, 8, 0ULL);	/* Private MMF start */
	memMWrt(loadinfoAddr+0x88, 8, 0ULL);	/* PA entry address */

	/* Place location of sysvec table in ar.k7 */
	arSet(curPid, K7_ID, sysvecAddr);

	/* Backing Store */
	if (lp64) {
		(void)arSet(curPid, BSP_ID, BSP64_ADDR);
		(void)arSet(curPid, BSPST_ID, BSP64_ADDR);
	} else {
		(void)arSet(curPid, BSP_ID, BSP32_ADDR);
		(void)arSet(curPid, BSPST_ID, BSP32_ADDR);
	}

	if ((proc->ehdr_flags & EF_IA_64_TRAPNIL) == 0)
		memZerosPageSet(0);

	return (0);
}
