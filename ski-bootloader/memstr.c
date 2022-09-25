// SPDX-License-Identifier: GPL-2.0
/*
 * String function implementation for firmwar.
 * Normally thouse would live in <string.h>
 */

#include "memstr.h"

void * memcpy(void * dst, const void * src, unsigned long len) {
	char * cd = dst;
	const char * cs = src;

	while (len--) {
		*cd++ = *cs++;
	}
	return cd;
}

void *memset (void * dst, int c, unsigned long len) {
	char * cd = dst;
	while (len--) {
		*cd++ = c;
	}
	return cd;
}

char *strcpy(char *dst, const char *src) {
	char * r = dst;
	while (*src) {
		*dst++ = *src++;
	}
	return r;
}
