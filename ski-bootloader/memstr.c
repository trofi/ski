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

int strncmp (const char * s1, const char * s2, unsigned long len) {
	while (len--) {
		char c1 = *s1++;
		char c2 = *s2++;

		if (c1 < c2) return -1;
		if (c1 > c2) return  1;

		if (c1 == '\0') break;
	}
	return 0;
}
