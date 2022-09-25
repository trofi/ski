// SPDX-License-Identifier: GPL-2.0
#ifndef _SKI_BOOTLOADER_FW_EMU_H_
#define _SKI_BOOTLOADER_FW_EMU_H_

#include <ia64/setup.h>
#include <types.h>

struct ia64_boot_param * sys_fw_init (const char *args, int arglen);

#endif /* _SKI_BOOTLOADER_FW_EMU_H_ */
