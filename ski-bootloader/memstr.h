#ifndef _SKI_BOOTLOADER_MEMSTR_H_
#define _SKI_BOOTLOADER_MEMSTR_H_
// SPDX-License-Identifier: GPL-2.0

void * memcpy(void * dst, const void * src, unsigned long len);
void *memset (void * dst, int c, unsigned long len);
char *strcpy(char *dst, const char *src);
int strncmp(const char * s1, const char * s2, unsigned long len);

#endif /* _SKI_BOOTLOADER_MEMSTR_H_ */
