/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef __IA64_SETUP_H
#define __IA64_SETUP_H

#include <types.h>

extern struct ia64_boot_param {
	u64 command_line;		/* physical address of command line arguments */
	u64 efi_systab;		/* physical address of EFI system table */
	u64 efi_memmap;		/* physical address of EFI memory map */
	u64 efi_memmap_size;		/* size of EFI memory map */
	u64 efi_memdesc_size;		/* size of an EFI memory map descriptor */
	u32 efi_memdesc_version;	/* memory descriptor version */
	struct {
		u16 num_cols;	/* number of columns on console output device */
		u16 num_rows;	/* number of rows on console output device */
		u16 orig_x;	/* cursor's x position */
		u16 orig_y;	/* cursor's y position */
	} console_info;
	u64 fpswa;		/* physical address of the fpswa interface */
	u64 initrd_start;
	u64 initrd_size;
} *ia64_boot_param;

#endif
