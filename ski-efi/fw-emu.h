// SPDX-License-Identifier: GPL-2.0
#ifndef _SKI_BOOTLOADER_FW_EMU_H_
#define _SKI_BOOTLOADER_FW_EMU_H_

#include <types.h>
#include <efi.h>

void init_memory_map(void);
void mark_memory(unsigned long start, unsigned long end, unsigned char type);
efi_system_table_t * sys_fw_init (const char *args, int arglen, u64 image_base, u64 image_size);

#endif /* _SKI_BOOTLOADER_FW_EMU_H_ */
