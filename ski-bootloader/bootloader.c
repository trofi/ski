// SPDX-License-Identifier: GPL-2.0
/*
 * arch/ia64/hp/sim/boot/bootloader.c
 *
 * Loads an ELF kernel.
 *
 * Copyright (C) 1998-2003 Hewlett-Packard Co
 *	David Mosberger-Tang <davidm@hpl.hp.com>
 *	Stephane Eranian <eranian@hpl.hp.com>
 *
 * 01/07/99 S.Eranian modified to pass command line arguments to kernel
 */

#include "bootloader.h"

#include "ssc.h"

struct disk_req {
	unsigned long addr;
	unsigned len;
};

struct disk_stat {
	int fd;
	unsigned count;
};

extern void jmp_to_kernel (unsigned long bp, unsigned long e_entry);
extern struct ia64_boot_param *sys_fw_init (const char *args, int arglen);

static void
ssc_cons_write (const char *buf)
{
	unsigned long ch;

	while ((ch = *buf++) != '\0') {
		ssc(ch, 0, 0, 0, SSC_PUTCHAR);
		if (ch == '\n')
			ssc('\r', 0, 0, 0, SSC_PUTCHAR);
	}
}

static void
cons_write (const char *buf)
{
	ssc_cons_write(buf);
}

#define MAX_ARGS 32

void
start_bootloader (void)
{
	static char mem[4096];
	static char buffer[1024];
	unsigned long off;
	int fd, i;
	struct disk_req req;
	struct disk_stat stat;
	struct elf64_hdr *elf;
	struct elf64_phdr *elf_phdr;	/* program header */
	unsigned long e_entry, e_phoff, e_phnum;
	struct ia64_boot_param *bp;
	char *kpath, *args;
	long arglen = 0;

	ssc(0, 0, 0, 0, SSC_CONSOLE_INIT);

	/*
	 * S.Eranian: extract the commandline argument from the simulator
	 *
	 * The expected format is as follows:
         *
	 *     kernelname args...
	 *
	 * Both are optional but you can't have the second one without the first.
	 */
	arglen = ssc((long) buffer, 0, 0, 0, SSC_GET_ARGS);

	kpath = "vmlinux";
	args = buffer;
	if (arglen > 0) {
		kpath = buffer;
		while (*args != ' ' && *args != '\0')
			++args, --arglen;
		if (*args == ' ')
			*args++ = '\0', --arglen;
	}

	if (arglen <= 0) {
		args = "";
		arglen = 1;
	}

	fd = ssc((long) kpath, 1, 0, 0, SSC_OPEN);

	if (fd < 0) {
		cons_write(kpath);
		cons_write(": file not found, reboot now\n");
		for(;;);
	}
	stat.fd = fd;
	off = 0;

	req.len = sizeof(mem);
	req.addr = (long) mem;
	ssc(fd, 1, (long) &req, off, SSC_READ);
	ssc((long) &stat, 0, 0, 0, SSC_WAIT_COMPLETION);

	elf = (struct elf64_hdr *) mem;
	if (elf->e_ident[0] == 0x7f && !(elf->e_ident[1] == 'E' && elf->e_ident[2] == 'L' && elf->e_ident[3] == 'F')) {
		cons_write("not an ELF file\n");
		return;
	}
	if (elf->e_type != ET_EXEC) {
		cons_write("not an ELF executable\n");
		return;
	}

	if (elf->e_machine != EM_IA_64) {
		cons_write("kernel not for this processor\n");
		return;
	}

	e_entry = elf->e_entry;
	e_phnum = elf->e_phnum;
	e_phoff = elf->e_phoff;

	cons_write("loading ");
	cons_write(kpath);
	cons_write("...\n");

	for (i = 0; i < e_phnum; ++i) {
		req.len = sizeof(*elf_phdr);
		req.addr = (long) mem;
		ssc(fd, 1, (long) &req, e_phoff, SSC_READ);
		ssc((long) &stat, 0, 0, 0, SSC_WAIT_COMPLETION);
		if (stat.count != sizeof(*elf_phdr)) {
			cons_write("failed to read phdr\n");
			return;
		}
		e_phoff += sizeof(*elf_phdr);

		elf_phdr = (struct elf64_phdr *) mem;

		if (elf_phdr->p_type != PT_LOAD)
			continue;

		req.len = elf_phdr->p_filesz;
		req.addr = __pa(elf_phdr->p_paddr);
		ssc(fd, 1, (long) &req, elf_phdr->p_offset, SSC_READ);
		ssc((long) &stat, 0, 0, 0, SSC_WAIT_COMPLETION);
		memset((char *)__pa(elf_phdr->p_paddr) + elf_phdr->p_filesz, 0,
		       elf_phdr->p_memsz - elf_phdr->p_filesz);
	}
	ssc(fd, 0, 0, 0, SSC_CLOSE);

	cons_write("starting kernel...\n");

	/* fake an I/O base address. */
	asm volatile ("mov ar0=%0" : : "r"(0xffffc000000ULL) : "memory");

	bp = sys_fw_init(args, arglen);

	ssc(0, (long) kpath, 0, 0, SSC_LOAD_SYMBOLS);

	jmp_to_kernel((unsigned long) bp, e_entry);

	cons_write("kernel returned!\n");
	ssc(-1, 0, 0, 0, SSC_EXIT);
}
