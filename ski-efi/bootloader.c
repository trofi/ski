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

#include "fw-emu.h"
#include "memstr.h"
#include "ssc.h"
#include "memstr.h"

typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

struct disk_req {
	unsigned long addr;
	unsigned len;
};

struct disk_stat {
	int fd;
	unsigned count;
};

extern void jmp_to_kernel (unsigned long bp, unsigned long e_entry);

void
cons_write (const char *buf)
{
	unsigned long ch;

	while ((ch = *buf++) != '\0') {
		ssc(ch, 0, 0, 0, SSC_PUTCHAR);
		if (ch == '\n')
			ssc('\r', 0, 0, 0, SSC_PUTCHAR);
	}
}

#define MAX_ARGS 32

struct pe_coff_header
{
	uint16_t machine;
	uint16_t num_of_sections;
	uint32_t timestamp;
	uint32_t ptr_to_symtable;
	uint32_t num_symbols;
	uint16_t size_of_opt_header;
	uint16_t characteristics;
};

struct pe_optional_header
{
	uint16_t magic;
	uint16_t linker_version;
	uint32_t code_size;
	uint32_t data_size;
	uint32_t bss_size;
	uint32_t entry_point;
};

struct pe32_section_table
{
	char name[8];
	uint32_t virtual_size;
	uint32_t virtual_address;
	uint32_t raw_data_size;
	uint32_t raw_data_offset;
	uint32_t relocations_offset;
	uint32_t line_numbers_offset;
	uint16_t num_relocations;
	uint16_t num_line_numbers;
	uint32_t characteristics;
};

struct pe32_fixup_block
{
	uint32_t page_rva;
	uint32_t block_size;
	uint16_t entries[0];
};

extern char fake_image_handle;

void
start_bootloader (void)
{
	static char mem[4096];
	static char buffer[1024];
	int fd, i;
	struct disk_req req;
	struct disk_stat stat;
	efi_system_table_t *systab;
	struct pe_coff_header *coff_header;
	struct pe_optional_header *opt_header;
	struct pe32_section_table *sec_hdr;
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
		/* Advance args pointer to skip kernel and whitespace after. */
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

	req.len = sizeof(mem);
	req.addr = (long) mem;
	ssc(fd, 1, (long) &req, 0, SSC_READ);
	ssc((long) &stat, 0, 0, 0, SSC_WAIT_COMPLETION);

	char *msdos_stub = (char *) mem;
	if (strncmp(msdos_stub, "MZ", 2) != 0) {
		cons_write("not a PE file\n");
		return;
	}

	uint32_t pe_offset = *(uint32_t *) (mem + 0x3c);

	req.len = 4;
	req.addr = (long) mem;
	ssc(fd, 1, (long) &req, pe_offset, SSC_READ);
	ssc((long) &stat, 0, 0, 0, SSC_WAIT_COMPLETION);

	if (strncmp(mem, "PE", 2) != 0) {
		cons_write("not a PE file\n");
		return;
	}

	cons_write("loading ");
	cons_write(kpath);
	cons_write("...\n");

	req.len = sizeof(*coff_header);
	req.addr = (long) mem;
	ssc(fd, 1, (long) &req, pe_offset + 4, SSC_READ);
	ssc((long) &stat, 0, 0, 0, SSC_WAIT_COMPLETION);

	coff_header = (void*) mem;

	if (coff_header->machine != 0x200) {
		cons_write("PE file for wrong machine\n");
		return;	    
	}

	uint32_t num_of_sections = coff_header->num_of_sections;

	uint32_t sec_offset0 = pe_offset + 4 + sizeof (*coff_header) + coff_header->size_of_opt_header;

	req.len = sizeof(*opt_header);
	req.addr = (long) mem;
	ssc(fd, 1, (long) &req, pe_offset + 4 + sizeof (*coff_header), SSC_READ);
	ssc((long) &stat, 0, 0, 0, SSC_WAIT_COMPLETION);

	opt_header = (void*) mem;

	uint64_t entry_point = opt_header->entry_point;

	uint64_t image_base = 0x500000, image_size = 0;

	init_memory_map();

	uint64_t sec_end = sec_offset0 + num_of_sections * sizeof(*sec_hdr);

	req.len = sec_end;
	req.addr = __pa(image_base);
	ssc(fd, 1, (long) &req, 0, SSC_READ);
	ssc((long) &stat, 0, 0, 0, SSC_WAIT_COMPLETION);
	mark_memory(image_base, image_base + sec_end, EFI_LOADER_DATA);

	uint32_t sec_offset = sec_offset0;

	for (i = 0; i < num_of_sections; ++i) {
		req.len = sizeof(*sec_hdr);
		req.addr = (long) mem;
		ssc(fd, 1, (long) &req, sec_offset, SSC_READ);
		ssc((long) &stat, 0, 0, 0, SSC_WAIT_COMPLETION);
		if (stat.count != sizeof(*sec_hdr)) {
			cons_write("failed to read phdr\n");
			return;
		}
		sec_offset += sizeof(*sec_hdr);

		sec_hdr = (void *) mem;

		req.len = sec_hdr->raw_data_size;
		req.addr = __pa(sec_hdr->virtual_address + image_base);
		ssc(fd, 1, (long) &req, sec_hdr->raw_data_offset, SSC_READ);
		ssc((long) &stat, 0, 0, 0, SSC_WAIT_COMPLETION);
		memset((char *)__pa(sec_hdr->virtual_address + image_base) + sec_hdr->raw_data_size, 0,
		       sec_hdr->virtual_size - sec_hdr->raw_data_size);
		/* TODO: use LOADER_DATA for data.  */
		mark_memory(sec_hdr->virtual_address + image_base, sec_hdr->virtual_address + image_base + sec_hdr->virtual_size, EFI_LOADER_CODE);
		if (sec_hdr->virtual_address + sec_hdr->virtual_size > image_size)
		  image_size = sec_hdr->virtual_address + sec_hdr->virtual_size;
	}

	sec_offset = sec_offset0;

	for (i = 0; i < num_of_sections; ++i) {
		req.len = sizeof(*sec_hdr);
		req.addr = (long) mem;
		ssc(fd, 1, (long) &req, sec_offset, SSC_READ);
		ssc((long) &stat, 0, 0, 0, SSC_WAIT_COMPLETION);
		if (stat.count != sizeof(*sec_hdr)) {
			cons_write("failed to read phdr\n");
			return;
		}
		sec_offset += sizeof(*sec_hdr);

		sec_hdr = (void *) mem;

		if (strncmp (sec_hdr->name, ".reloc", 8) == 0) {
			uint32_t off = sec_hdr->raw_data_offset;
			uint32_t end = sec_hdr->raw_data_offset + sec_hdr->raw_data_size;
			while (off < end) {
				struct pe32_fixup_block fb;
				req.len = sizeof (fb);
				req.addr = (long) mem;
				ssc(fd, 1, (long) &req, off, SSC_READ);
				ssc((long) &stat, 0, 0, 0, SSC_WAIT_COMPLETION);
				off += sizeof(fb);

				fb = *(struct pe32_fixup_block *)mem;
				for (int i = 0; i < (fb.block_size - sizeof(fb)) / 2; i++) {
					uint16_t fu;
					req.len = sizeof (fu);
					req.addr = (long) mem;
					ssc(fd, 1, (long) &req, off, SSC_READ);
					ssc((long) &stat, 0, 0, 0, SSC_WAIT_COMPLETION);
					off += 2;

					fu = *(uint16_t *)mem;

					void *target = (uint64_t *) (((fu & 0xfff) | fb.page_rva) + image_base);
					uint16_t type = fu >> 12;

					switch (type) {
					case 0:
						break;
					case 10:
						*(uint64_t *) target += image_base;
						break;
					default:
						cons_write("Unknown relocation type\n");
						return;
					}
				}
			}
		}
	}

	ssc(fd, 0, 0, 0, SSC_CLOSE);

	cons_write("starting kernel...\n");

	/* fake an I/O base address. */
	asm volatile ("mov ar0=%0" : : "r"(0xffffc000000ULL) : "memory");

	systab = sys_fw_init(args, arglen, image_base, image_size);

	ssc(0, (long) kpath, 0, 0, SSC_LOAD_SYMBOLS);

	((void (*) (void *, void *))image_base + entry_point) (&fake_image_handle, systab);

	cons_write("kernel returned!\n");
	ssc(-1, 0, 0, 0, SSC_EXIT);
}
