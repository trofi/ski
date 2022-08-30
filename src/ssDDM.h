#ifndef _SKI_SSDDM_H
#define _SKI_SSDDM_H

#include <stddef.h>

#include "std.h"
#include "types.h"

#if defined HAVE_LIBELF_LIBELF_H
# include <libelf/libelf.h>
#else /* !defined HAVE_LIBELF_LIBELF_H */
# include <libelf.h>
#endif /* !defined HAVE_LIBELF_LIBELF_H */

BOOL valAssign(unsigned argc, char *argv[]);
BOOL byteAssign(unsigned argc, char *argv[]);
BOOL hwAssign(unsigned argc, char *argv[]);
BOOL wordAssign(unsigned argc, char *argv[]);
BOOL dwordAssign(unsigned argc, char *argv[]);
BOOL stringAssign(unsigned argc, char *argv[]);
BOOL instrAssign(unsigned argc, char *argv[]);
BOOL templAssign(unsigned argc, char *argv[]);
BOOL dasmDump(unsigned argc, char *argv[]);
BOOL dataDump(unsigned argc, char *argv[]);

void addLM2(ADDR adr, size_t len);
void addLM5 (const char *name, Elf64_Phdr *phdr, int phnum,
	     ADDR load_base, int discontiguous);
BOOL cstkDisplay(unsigned argc, char *argv[]);
BOOL showLoadModules(unsigned argc, char *argv[]);

#endif /* _SKI_SSDDM_H */
