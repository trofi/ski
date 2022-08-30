/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator File Loading Routines
 *
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


/*##################### Header and Constant Definitions ####################*/

#include "std.h"
#include <stddef.h>
#if defined __linux
#include <stdint.h>
#endif
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#if defined HAVE_LIBELF_LIBELF_H
# include <libelf/libelf.h>
#else /* !defined HAVE_LIBELF_LIBELF_H */
# include <libelf.h>
#endif /* !defined HAVE_LIBELF_LIBELF_H */
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>	/* NEW for mmap */
#include <limits.h>

#include "load.h"

#include "bits.h"
#include "types.h"
#include "fields.h"
#include "state.h"
#include "program.h"
#include "libsym.h"
#include "libcore.h"
#include "exportui.h"
#include "coreui.h"
#include "lm.h"
#include "sim.h"
#include "simmem.h"
#include "ski_elf.h"
#include "osload.h"
#include "ssDDM.h"

#if defined(__FreeBSD__)
/*
 * XXX There's a nasty conflict with the libelf library on FreeBSD and
 * the system provided ELF headers. You either have undeclared symbols
 * or conflicting types. Define what we're missing by hand. It appears
 * that the libelf port is broken in this regard :-(
 */
#define	ELF64_ST_BIND(info)     ((info) >> 4)
#define	ELF64_ST_TYPE(info)     ((info) & 0xf)
#endif

#define BSP32_ADDR	0x2000000062800000ULL
#define BSP64_ADDR	0x9FFFFFFF7F600000ULL

#define ADDPTR(addr)    (addr = (BitfX(addr,32,32) | BitfX(addr,32,2) << 61))

void initDwUnitTbl(int fd);

BOOL force_user = NO;
BOOL force_system = NO;

static ADDR text_base, text_end;
static ADDR entry_ip;
static REG psrval = 0;

/* Command code (how argument) for elfOpen() */
#define	EO_EXEC		0	/* Executable */
#define	EO_DYN		1	/* Interpreter and/or shared libraries */
#define	EO_SYMS		2	/* Any ELF - we want symbols */

#define LI_TRACE	0x2		/* from load_info.h or crt0.h */

/* #define DO_DEBUG_LOAD 1 */
#ifdef DO_DEBUG_LOAD
#    define DEBUG(...) do { \
        (void)fprintf(stderr, "DEBUG: "); \
        (void)fprintf(stderr, __VA_ARGS__); \
    } while (0)
#else
#    define DEBUG(...) do { } while (0)
#endif

char *sim_root = NULL;
size_t sim_root_len = 0;

/*##################### Globals - Exports ##################################*/

ADDR dataStart = 0;	/* used to set initial location of data window */
BOOL alwaysNat = NO;
unsigned numLms = 0;

LMINFO lminfo[MAX_LOAD_MODULES];

/*##################### Functions and Subroutines ##########################*/

#include <sys/mman.h>
#include <sys/param.h>
#if !defined __linux__
#include <sys/pstat.h>
#endif /* !defined __linux__ */
#include <sys/unistd.h>
#include <fcntl.h>

static void vminfo(char *s)
{
#if DO_DEBUG_LOAD
    struct pst_status pst;
    struct pst_vm_status pstvm;
    int target = (int)getpid();

    puts(s);
    pstat_getproc(&pst, sizeof pst, 0, target);
    printf("data real pages = %lx\n", pst.pst_dsize);
    printf("data virt pages = %lx\n", pst.pst_vdsize);
    printf("mmap virt pages = %lx\n\n", pst.pst_vmmsize);
    fflush(stdout);
    fflush(stderr);
#endif
}


/* Insert symbols from the ELF section scn into the symbol table
 *   the symbol's address must be in the range low..high
 *   the st_name field must be nonzero
 *   the binding part of the st_info field must be STB_GLOBAL or STB_LOCAL
 *   the type part of the st_info field must be STT_FUNC or STT_OBJECT
 *   the st_shndx field must not be SHN_UNDEF?
 *   the address in the ELF section will be increased by reloc_addr
 */
static void elf64_symbols(Elf *elfptr, Elf_Scn *scn, Elf64_Word strndx,
			  ADDR reloc_addr, ADDR low, ADDR high)
{
    Elf_Data *data;
    Elf_Scn *strscn;
    Elf64_Sym *syms;
    Elf64_Half shndx;
    int i, nsyms;
    unsigned bind, type;

    /* XXX - will this "fall out" naturally? */
    if (strndx == SHN_UNDEF)	/* No symbols found */
	return;
    data = elf_getdata(scn, 0);
    syms = data->d_buf;
    /* Use sh_entsize instead of sizeof? */
    nsyms = data->d_size / sizeof (Elf64_Sym);
    strscn = elf_getscn(elfptr, strndx);
    data = elf_getdata(strscn, 0);
    for (i = 1; i < nsyms; i++) {
	char *name;
	Elf64_Addr value;

	if (!syms[i].st_name)
	    continue;
	name = (char *)data->d_buf + syms[i].st_name;
	if ((shndx = syms[i].st_shndx) == SHN_UNDEF)
	    continue;
	value = syms[i].st_value;

	if (value < low || value > high)
	    continue;
	if (reloc_addr) {
/*	    value -= elf64_getshdr(elf_getscn(elfptr, shndx))->sh_addr;*/
	    value -= low;
	    value += reloc_addr;
	}
	/* XXX - the cast below is used to silence the 703 Migration warning;
	   it might be better to place this cast in the definition of the
	   ELF macro or to fix the compiler to not complain here */
	bind = ELF64_ST_BIND((unsigned)syms[i].st_info);
	type = ELF64_ST_TYPE(syms[i].st_info);
	/* XXX - Should dataStart be based on something else like address of
		 lowest data segment? */
	if (!strcmp(name, "_data_start") || !strcmp(name, "__data_start")) {
	    dataStart = value;
	    if (!lp64)
		ADDPTR(dataStart);
	}
	/* XXX - what to do with STB_WEAK symbols? */
	/* XXX - hack to avoid inserting $DEBUG_xxx symbols which are
		 currently of type OBJT */
	if (strlen(name) > 12 && !strncmp(name, "$DEBUG_", 7))
	    continue;

	if (bind != STB_GLOBAL && bind != STB_LOCAL)
	    continue;

	if (type != STT_FUNC && type != STT_OBJECT && type != STT_NOTYPE)
	    continue;
	symInsert(name, value, 0);
    }
}

static void elf32_symbols(Elf *elfptr, Elf_Scn *scn, Elf32_Word strndx,
			  ADDR reloc_addr, ADDR low, ADDR high)
{
    Elf_Data *data;
    Elf_Scn *strscn;
    Elf32_Sym *syms;
    Elf32_Half shndx;
    int i, nsyms;
    unsigned bind, type;

    /* XXX - will this "fall out" naturally? */
    if (strndx == SHN_UNDEF)	/* No symbols found */
	return;
    data = elf_getdata(scn, 0);
    syms = data->d_buf;
    /* Use sh_entsize instead of sizeof? */
    nsyms = data->d_size / sizeof (Elf32_Sym);
    strscn = elf_getscn(elfptr, strndx);
    data = elf_getdata(strscn, 0);
    for (i = 1; i < nsyms; i++) {
	char *name;
	ADDR value;

	if (!syms[i].st_name)
	    continue;
	name = (char *)data->d_buf + syms[i].st_name;
	if ((shndx = syms[i].st_shndx) == SHN_UNDEF)
	    continue;
	type = ELF32_ST_TYPE(syms[i].st_info);
	if (type == STT_SECTION)
	    continue;
	value = syms[i].st_value;
	ADDPTR(value);	/* XXX - check lp64? */
	if (value < low || value > high)
	    continue;
	if (reloc_addr) {
/*	    value -= elf32_getshdr(elf_getscn(elfptr, shndx))->sh_addr;*/
	    value -= low;
	    value += reloc_addr;
	}
	/* XXX - the cast below is used to silence the 703 Migration warning;
	   it might be better to place this cast in the definition of the
	   ELF macro or to fix the compiler to not complain here */
	bind = ELF32_ST_BIND((unsigned)syms[i].st_info);
	/* XXX - Should dataStart be based on something else like address of
		 lowest data segment? */
	if (!strcmp(name, "_data_start") || !strcmp(name, "__data_start")) {
	    dataStart = value;
	    if (!lp64)
		ADDPTR(dataStart);
	}
	/* XXX - what to do with STB_WEAK symbols? */
	/* XXX - hack to avoid inserting $DEBUG_xxx symbols which are
		 currently of type OBJT */
	if (strlen(name) > 12 && !strncmp(name, "$DEBUG_", 7))
	    continue;

	if (bind != STB_GLOBAL && bind != STB_LOCAL)
	    continue;

	if (type != STT_FUNC && type != STT_OBJECT)
	    continue;
	symInsert(name, value, 0);
    }
}

static void elfClose(Elf *elfptr, int fd)
{
    (void)elf_end(elfptr);
    (void)close(fd);
}

static int
elf_is_executable(int type)
{
#ifdef __linux
    return (type == ET_EXEC || type == ET_DYN) ? 1 : 0;
#else
    return (type == ET_EXEC) ? 1 : 0;
#endif
}

static int
elf_is_shared_object(int type)
{
#ifdef __hpux
    return (type == ET_EXEC || type == ET_DYN) ? 1 : 0;
#else
    return (type == ET_DYN) ? 1 : 0;
#endif
}

static Elf *elfOpen(const char *file_name, FILE *errf, int *fd, int how,
    unsigned char *class)
{
    Elf *elfptr;
    char *ident;

    /* XXX - use cmdOpenFile and fileno? */
    if ((*fd = open(file_name, O_RDONLY)) == -1) {
	if (errf)
	    (void)fprintf(errf, "%s - %s\n", file_name, strerror(errno));
	return NULL;
    }
    (void)elf_version(EV_CURRENT);
    if (!(elfptr = elf_begin(*fd, ELF_C_READ, 0))) {
	if (errf)
	    (void)fprintf(errf, "%s - %s\n", file_name, elf_errmsg(0));
	elfClose(elfptr, *fd);	/* XXX - OK to call elf_end(NULL)? */
	return NULL;
    }
    if (elf_kind(elfptr) != ELF_K_ELF) {
	if (errf)
	    (void)fprintf(errf, "%s is not an ELF file\n", file_name);
	elfClose(elfptr, *fd);
	return NULL;
    }
    if (!(ident = elf_getident(elfptr, NULL))) {
	if (errf)
	    (void)fprintf(errf, "%s - %s\n", file_name, elf_errmsg(0));
	elfClose(elfptr, *fd);
	return NULL;
    }
    *class = ident[EI_CLASS];
    if (*class != ELFCLASS32 && *class != ELFCLASS64) {
	if (errf)
	    (void)fprintf(errf, "%s - unsupported ELF file class (%u)\n",
			  file_name, *class);
	elfClose(elfptr, *fd);
	return NULL;
    }
    if (ident[EI_DATA] != ELFDATA2LSB && ident[EI_DATA] != ELFDATA2MSB) {
	if (errf)
	    (void)fprintf(errf, "%s - unsupported ELF data encoding (%u)\n",
			  file_name, ident[EI_DATA]);
	elfClose(elfptr, *fd);
	return NULL;
    }

    if (*class == ELFCLASS64) {
	Elf64_Ehdr *ehdr;

	ehdr = elf64_getehdr(elfptr);
	if (ehdr == NULL) {
	    if (errf)
		(void)fprintf(errf, "%s - %s\n", file_name, elf_errmsg(0));
	    elfClose(elfptr, *fd);
	    return NO;
	}
	if (ehdr->e_machine != ELF_IA_64) {
	    if (errf)
		(void)fprintf(errf, "%s - wrong architecture (%u)\n",
		    file_name, ehdr->e_machine);
	    elfClose(elfptr, *fd);
	    return NO;
	}

	if (how == EO_EXEC) {
	    if (!elf_is_executable(ehdr->e_type)) {
		if (errf)
		    (void)fprintf(stderr, "%s - not an executable file\n",
			file_name);
		elfClose(elfptr, *fd);
		return NO;
	    }
	} else if (how == EO_DYN) {
	    if (!elf_is_shared_object(ehdr->e_type)) {
		if (errf)
		    (void)fprintf(stderr, "%s - not a dynamic object\n",
			file_name);
		elfClose(elfptr, *fd);
		return NO;
	    }
	}

	if (!os_elf64_abi(ident, ehdr, (how == EO_EXEC))) {
	    if (errf)
		(void)fprintf(stderr, "%s - unsupported ELF64 ABI\n", file_name);
	    elfClose(elfptr, *fd);
	    return NO;
	}
    } else {
	Elf32_Ehdr *ehdr;

	ehdr = elf32_getehdr(elfptr);
	if (ehdr == NULL) {
	    if (errf)
		(void)fprintf(errf, "%s - %s\n", file_name, elf_errmsg(0));
	    elfClose(elfptr, *fd);
	    return NO;
	}
	if (ehdr->e_machine != ELF_IA_64) {
	    if (errf)
		(void)fprintf(errf, "%s - wrong architecture (%u)\n",
		    file_name, ehdr->e_machine);
	    elfClose(elfptr, *fd);
	    return NO;
	}

	if (how == EO_EXEC) {
	    if (!elf_is_executable(ehdr->e_type)) {
		if (errf)
		    (void)fprintf(stderr, "%s - not an executable file\n",
			file_name);
		elfClose(elfptr, *fd);
		return NO;
	    }
	} else if (how == EO_DYN) {
	    if (!elf_is_shared_object(ehdr->e_type)) {
		if (errf)
		    (void)fprintf(stderr, "%s - not a dynamic object\n",
			file_name);
		elfClose(elfptr, *fd);
		return NO;
	    }
	}

	if (!os_elf32_abi(ident, ehdr, (how == EO_EXEC))) {
	    if (errf)
		(void)fprintf(stderr, "%s - unsupported ELF32 ABI\n", file_name);
	    elfClose(elfptr, *fd);
	    return NO;
	}
    }

    return elfptr;
}

static void
elf64_slurp_all_symbols (Elf *elfptr, Elf64_Ehdr *ehdr, Elf64_Phdr *phdr,
			 ADDR load_base)
{
    Elf64_Shdr *shdr;
    Elf_Scn *scn;
    int i;

    for (scn = NULL; (scn = elf_nextscn(elfptr, scn)) != NULL;) {
	shdr = elf64_getshdr(scn);
	if (shdr->sh_type == SHT_SYMTAB || shdr->sh_type == SHT_DYNSYM) {
	    for (i = 0; i < ehdr->e_phnum; ++i) {
		if (phdr[i].p_type != PT_LOAD)
		    continue;
		elf64_symbols(elfptr, scn, shdr->sh_link, load_base,
			      phdr[i].p_vaddr,
			      phdr[i].p_vaddr + phdr[i].p_memsz - 1);
	    }
	}
    }
}

static void
elf32_slurp_all_symbols (Elf *elfptr, Elf32_Ehdr *ehdr, Elf32_Phdr *phdr,
			 ADDR load_base)
{
    Elf32_Shdr *shdr;
    Elf_Scn *scn;
    int i;

    for (scn = NULL; (scn = elf_nextscn(elfptr, scn)) != NULL;) {
	shdr = elf32_getshdr(scn);
	if (shdr->sh_type == SHT_SYMTAB || shdr->sh_type == SHT_DYNSYM) {
	    for (i = 0; i < ehdr->e_phnum; ++i) {
		if (phdr[i].p_type != PT_LOAD)
		    continue;
		elf32_symbols(elfptr, scn, shdr->sh_link, load_base,
			      phdr[i].p_vaddr,
			      phdr[i].p_vaddr + phdr[i].p_memsz - 1);
	    }
	}
    }
}

void mmapSyms(int fd, ADDR start, ADDR len, ADDR offset)
{
    Elf *elfptr;
    char *ident;
    unsigned char class;
    ADDR end = start + len - 1;

    if (!(elfptr = elf_begin(fd, ELF_C_READ, 0))) {
	(void)elf_end(elfptr);
	return;
    }
    if (elf_kind(elfptr) != ELF_K_ELF) {
	(void)elf_end(elfptr);
	return;
    }
    if (!(ident = elf_getident(elfptr, NULL))) {
	(void)elf_end(elfptr);
	return;
    }
    if ((class = ident[EI_CLASS]) != ELFCLASS32 && class != ELFCLASS64) {
	(void)elf_end(elfptr);
	return;
    }
    if (class == ELFCLASS64) {
	Elf64_Ehdr *ehdr;
	Elf64_Phdr *phdr;
	ADDR pstart = 0, pend = 0;
	unsigned i;

	if (!(ehdr = elf64_getehdr(elfptr))) {
	    (void)elf_end(elfptr);
	    return;
	}

	if (!(phdr = elf64_getphdr(elfptr))) {
	    (void)elf_end(elfptr);
	    return;
	}
	symDeleteAddr(start, end);

	for (i = 0; i < ehdr->e_phnum; i++) {
	    if (phdr[i].p_type == PT_IA_64_UNWIND) {
		addLM5("shlib", phdr, ehdr->e_phnum, start, 0);
		break;
	    }
	}

	for (i = 0; i < ehdr->e_phnum; i++) {
	    ADDR fstart, fend;

	    if (phdr[i].p_type != PT_LOAD)
		continue;
	    fstart = phdr[i].p_offset;
	    fend = fstart + phdr[i].p_filesz;
	    if (offset >= fstart && offset < fend) {
		pstart = phdr[i].p_vaddr + offset - fstart;
		pend = pstart + len - 1;
		break;
	    }
	}
	elf64_slurp_all_symbols(elfptr, ehdr, phdr, start);
    } else {	/* ELFCLASS32 */
	Elf32_Ehdr *ehdr;
	Elf32_Phdr *phdr;
	ADDR pstart = 0, pend = 0;
	unsigned i;

	if (!(ehdr = elf32_getehdr(elfptr))) {
	    (void)elf_end(elfptr);
	    return;
	}

	if (!(phdr = elf32_getphdr(elfptr))) {
	    (void)elf_end(elfptr);
	    return;
	}
	symDeleteAddr(start, end);
	for (i = 0; i < ehdr->e_phnum; i++) {
	    ADDR fstart, fend;

	    if (phdr[i].p_type != PT_LOAD)
		continue;
	    fstart = phdr[i].p_offset;
	    fend = fstart + phdr[i].p_filesz;
	    if (offset >= fstart && offset < fend) {
		pstart = phdr[i].p_vaddr + offset - fstart;
		ADDPTR(pstart);	/* XXX - check lp64? */
		pend = pstart + len - 1;
		break;
	    }
	}
	elf32_slurp_all_symbols(elfptr, ehdr, phdr, start);
    }
}

void munmapSyms(ADDR low, ADDR high)
{
    symDeleteAddr(low, high);
}

void dynBlock(ADDR start, ADDR len)
{
    static char blkName[16];
    static unsigned dynCnt = 0;

    (void)sprintf(blkName, " dynBlock%u", dynCnt++);
    symInsert(blkName, start, 0);
}

static void segload(BYTE *p, ADDR addr, unsigned len, unsigned flags)
{
    unsigned size;

    DEBUG("segload: addr: %#llx - %#llx flags: %#x\n", addr, addr + len, flags);

    for (; len; len -= size, addr += size, p += size) {
	if (len + page_offset(addr) < page_size)
	    size = len;
	else
	    size = page_size - page_offset(addr);
	if (flags & PF_X)
	    memMPIWrt(addr, p, size);
	else
	    memMPWrt(addr, p, size);
    }
}

static BOOL elf64SegmentLoad(const Elf64_Phdr *phdr, ADDR addr, int fd,
			     const char *file_name)
{
    BYTE *segbuf;

    if (phdr->p_filesz > phdr->p_memsz) {
	fprintf(stderr, "%s - bad ELF header (file size > memory size)\n",
	    file_name);
	return NO;
    }

    if (!(segbuf = calloc(phdr->p_memsz, 1))) {
	(void)fprintf(stderr, "out of memory loading %s\n", file_name);
	return NO;
    }
    if (lseek(fd, phdr->p_offset, SEEK_SET) == -1) {
	perror(file_name);
	free(segbuf);
	return NO;
    }
    if (read(fd, segbuf, phdr->p_filesz) == -1) {
	perror(file_name);
	free(segbuf);
	return NO;
    }
    segload(segbuf, addr, phdr->p_memsz, phdr->p_flags);
    free(segbuf);
    return YES;
}

static BOOL elf32SegmentLoad(const Elf32_Phdr *phdr, ADDR addr, int fd,
			     const char *file_name)
{
    BYTE *segbuf;

    if (phdr->p_filesz > phdr->p_memsz) {
	(void)fprintf(stderr,
		      "%s - bad ELF header (file size > memory size)\n",
		      file_name);
	return NO;
    }
#if 0
/* NEW */
    vminfo("Before segload:");
    if (phdr->p_flags & PF_X) {
	void *hostadr;

	/*text_base = addr;*/
	text_end = addr + phdr->p_memsz;
	hostadr = mmap(0, phdr->p_filesz, PROT_READ|PROT_WRITE,
		       MAP_PRIVATE|MAP_VARIABLE|MAP_FILE, fd, phdr->p_offset);
	if (hostadr == (void *)-1) {
	    perror("mmap");
	    return NO;
	}
	memMPMap(addr, hostadr, phdr->p_filesz);
	return YES;
    } else {
	void *hostadr, *temp, *bssadr;
	int i;
	size_t datasz, bsssz;

	temp = mmap(0, phdr->p_memsz, PROT_READ|PROT_WRITE,
		    MAP_PRIVATE|MAP_VARIABLE|MAP_ANONYMOUS, 0, 0);
	i = munmap(temp, phdr->p_memsz);
	if (i == -1) {
	    perror("munmap");
	    return NO;
	}
	datasz = (phdr->p_filesz + 0xFFF) & ~0xFFF;
	bsssz =  phdr->p_memsz - datasz;
	hostadr = mmap(temp, datasz, PROT_READ|PROT_WRITE,
		       MAP_PRIVATE|MAP_FIXED|MAP_FILE, fd, phdr->p_offset);
	if (hostadr != temp) {
	    perror("mmap");
	    return NO;
	}
	bssadr = mmap((char *)temp+datasz, bsssz, PROT_READ|PROT_WRITE,
		      MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, 0, 0);
	if (bssadr != (char *)temp+datasz) {
	    perror("mmap");
	    return NO;
	}
	memMPMap(addr, hostadr, phdr->p_memsz);
	return YES;
    }
/* NEW */
#endif
    if (!(segbuf = calloc(phdr->p_memsz, 1))) {
	(void)fprintf(stderr, "out of memory loading %s\n", file_name);
	return NO;
    }
    if (lseek(fd, phdr->p_offset, SEEK_SET) == -1) {
	perror(file_name);
	free(segbuf);
	return NO;
    }
    if (read(fd, segbuf, phdr->p_filesz) == -1) {
	perror(file_name);
	free(segbuf);
	return NO;
    }
    vminfo("Before segload:");
    segload(segbuf, addr, phdr->p_memsz, phdr->p_flags);
    free(segbuf);
    return YES;
}

static BOOL interp(int fd, off_t offset, unsigned sz)
{
    char buffer[PATH_MAX];
    char *interpName;
    Elf *elfptr;
    ADDR rtld_text, rtld_data = 0, unwind_base = 0;
    unsigned char class;

    memset(buffer, 0, sizeof(buffer));
    if (sim_root)
    	strncpy(buffer, sim_root, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = 0;
    interpName = buffer + sim_root_len;

    if (numLms == MAX_LOAD_MODULES) {
	(void)fprintf(stderr, "Too many load modules\n");
	return NO;
    }
    lseek(fd, offset, SEEK_SET);
    if (read(fd, interpName, sz) != sz) {
	(void)fprintf(stderr, "Failed to load interpName\n");
	return NO;
    }
    DEBUG("interp: '%s'\n", interpName);

    /* skip interp info past first colon */
    (void)strtok(interpName, ":");
    interpName = buffer;
    elfptr = elfOpen(interpName, stderr, &fd, EO_DYN, &class);
    if (elfptr == NULL)
	return NO;

    if (class == ELFCLASS64) {
	ADDR bias = os_rtld64_bias();
	Elf64_Ehdr *ehdr;
	Elf64_Phdr *phdr;
	unsigned i;

	DEBUG("interp: ELF64\n");

	if (!(ehdr = elf64_getehdr(elfptr))) {
	    elfClose(elfptr, fd);
	    return NO;
	}
	X_PSR_BE(psrval) = ehdr->e_flags & EF_IA_64_BE ? YES : NO;

	if (!(phdr = elf64_getphdr(elfptr))) {
	    (void)fprintf(stderr, "%s - %s\n", interpName, elf_errmsg(0));
	    elfClose(elfptr, fd);
	    return NO;
	}

	rtld_text = bias + phdr->p_vaddr;
	DEBUG("interp: rtld_text: %#llx\n", rtld_text);
	DEBUG("interp: program_headers: %u\n", ehdr->e_phnum);

	for (i = 0; i < ehdr->e_phnum; i++) {
	    if (phdr[i].p_type == PT_IA_64_UNWIND) {
		addLM5("ld.so", phdr, ehdr->e_phnum, bias, 1);
		unwind_base = bias + phdr[i].p_vaddr;
		DEBUG("interp: PT_IA_64_UNWIND: %#llx\n", unwind_base);
	    }
	    if (phdr[i].p_type != PT_LOAD)
		continue;
	    if (phdr[i].p_flags & PF_X) {
		entry_ip = bias + ehdr->e_entry;
		DEBUG("interp: entry: %#llx\n", entry_ip);
		if (!elf64SegmentLoad(&phdr[i], bias + phdr[i].p_vaddr, fd, interpName)) {
		    elfClose(elfptr, fd);
		    return NO;
		}
		text_base = rtld_text;
		text_end  = text_base + phdr[i].p_memsz - 1;
		DEBUG("interp: text_base: %#llx - %#llx\n", text_base, text_end);
	    } else {
		rtld_data = bias + phdr[i].p_vaddr;
		DEBUG("interp: data_base: %#llx\n", rtld_data);
		if (!elf64SegmentLoad(&phdr[i], rtld_data, fd, interpName)) {
		    elfClose(elfptr, fd);
		    return NO;
		}
	    }
	}
	elf64_slurp_all_symbols(elfptr, ehdr, phdr, rtld_text);
    } else {	/* ELFCLASS32 */
	ADDR bias = os_rtld32_bias();
	Elf32_Ehdr *ehdr;
	Elf32_Phdr *phdr;
	unsigned i;

	if (!(ehdr = elf32_getehdr(elfptr))) {
	    elfClose(elfptr, fd);
	    return NO;
	}
	X_PSR_BE(psrval) = ehdr->e_flags & EF_IA_64_BE ? YES : NO;

	if (!(phdr = elf32_getphdr(elfptr))) {
	    (void)fprintf(stderr, "%s - %s\n", interpName, elf_errmsg(0));
	    elfClose(elfptr, fd);
	    return NO;
	}

	rtld_text = bias + phdr->p_vaddr;

	for (i = 0; i < ehdr->e_phnum; i++) {
	    if (phdr[i].p_type == PT_IA_64_UNWIND)
		unwind_base = bias + phdr[i].p_vaddr;
	    if (phdr[i].p_type != PT_LOAD)
		continue;
	    if (phdr[i].p_flags & PF_X) {
		entry_ip = bias + ehdr->e_entry;
		if (!elf32SegmentLoad(&phdr[i], rtld_text, fd, interpName)) {
		    elfClose(elfptr, fd);
		    return NO;
		}
		text_base = rtld_text;
		text_end  = text_base + phdr[i].p_memsz - 1;
	    } else {
		rtld_data = bias + phdr[i].p_vaddr;
		if (!elf32SegmentLoad(&phdr[i], rtld_data, fd, interpName)) {
		    elfClose(elfptr, fd);
		    return NO;
		}
	    }
	}
	elf32_slurp_all_symbols(elfptr, ehdr, phdr, rtld_text);
    }

    if (entry_ip == 0) {
	(void)fprintf(stderr, "%s - missing text segment\n", interpName);
	elfClose(elfptr, fd);
	return NO;
    }
    if (rtld_data == 0) {
	(void)fprintf(stderr, "%s - missing data segment\n", interpName);
	elfClose(elfptr, fd);
	return NO;
    }

    elfClose(elfptr, fd);
    lminfo[numLms].text_base = text_base;
    lminfo[numLms].text_end = text_end;
    lminfo[numLms].unwind_base = unwind_base;
    /* XXX - replace call to strdup */
    lminfo[numLms].name = strdup(interpName);
    numLms++;
    return YES;
}

/*--------------------------------------------------------------------------
 * elfSymbolLoad - This routine loads the symbol table of the file given
 *  as an argument.  This routine is very similar to elfLoad.  Make sure
 *  that changes in the sections that are the same in the two functions get
 *  reflected in both functions.
 *-------------------------------------------------------------------------*/
BOOL elfSymLoad(const char *file_name)
{
    Elf *elfptr;
    int fd;
    unsigned char class;
    ADDR unwind_base = 0;

    if (numLms == MAX_LOAD_MODULES)
	return NO;
    /* XXX - should 2nd arg be stderr?  What if not ELF? */
    elfptr = elfOpen(file_name, NULL, &fd, EO_SYMS, &class);
    if (elfptr == NULL)
	return NO;

    if (class == ELFCLASS64) {
	Elf64_Ehdr *ehdr;
	Elf64_Phdr *phdr;
	unsigned i;

	if (!(ehdr = elf64_getehdr(elfptr))) {
	    elfClose(elfptr, fd);
	    return NO;
	}
	if (ehdr->e_type != ET_EXEC) {
	    elfClose(elfptr, fd);
	    return NO;
	}
	if (!(phdr = elf64_getphdr(elfptr))) {
	    elfClose(elfptr, fd);
	    return NO;
	}

	for (i = 0; i < ehdr->e_phnum; i++)
	    if (phdr[i].p_type == PT_IA_64_UNWIND)
		unwind_base = phdr[i].p_vaddr;
	    else if (phdr[i].p_type == PT_LOAD && phdr[i].p_flags & PF_X) {
		text_base = phdr[i].p_vaddr;
		text_end = text_base + phdr[i].p_memsz - 1;
	    }

	addLM5(file_name, phdr, ehdr->e_phnum, 0, 1);
	elf64_slurp_all_symbols(elfptr, ehdr, phdr, 0);
    } else {	/* ELFCLASS32 */
	Elf32_Ehdr *ehdr;
	Elf32_Phdr *phdr;
	unsigned i;

	if (!(ehdr = elf32_getehdr(elfptr))) {
	    elfClose(elfptr, fd);
	    return NO;
	}
	if (ehdr->e_type != ET_EXEC) {
	    elfClose(elfptr, fd);
	    return NO;
	}
	if (!(phdr = elf32_getphdr(elfptr))) {
	    elfClose(elfptr, fd);
	    return NO;
	}

	for (i = 0; i < ehdr->e_phnum; i++)
	    if (phdr[i].p_type == PT_IA_64_UNWIND)
		unwind_base = phdr[i].p_vaddr;
	    else if (phdr[i].p_type == PT_LOAD && phdr[i].p_flags & PF_X) {
		text_base = phdr[i].p_vaddr;
		ADDPTR(text_base);	/* XXX - check lp64? */
		text_end = text_base + phdr[i].p_memsz - 1;
	    }
	elf32_slurp_all_symbols(elfptr, ehdr, phdr, 0);

    }
    if (interface != BATCH)
      initDwUnitTbl(fd);
    elfClose(elfptr, fd);
    lminfo[numLms].text_base = text_base;
    lminfo[numLms].text_end = text_end;
    lminfo[numLms].unwind_base = unwind_base;
    /* XXX - replace call to strdup */
    lminfo[numLms].name = strdup(file_name);
    numLms++;
    return YES;
}

/* XXX - from peload.c */
static BOOL peLoad(int fd, int s_argc, char *s_argv[]);
static int peChk(const char *name);

/* XXX - NonStop hack */
static ADDR getNSgp(Elf *elfptr)
{
    Elf_Scn *scn = NULL;
    Elf64_Shdr *shdr;
    Elf_Data *data;
    Elf64_Dyn *dyn;
    unsigned ndyn, i;

    while ((scn = elf_nextscn(elfptr, scn)))
	if ((shdr = elf64_getshdr(scn))->sh_type == SHT_DYNAMIC)
	    break;
    data = elf_getdata(scn, 0);
    dyn = data->d_buf;
    ndyn = data->d_size / sizeof (Elf64_Dyn);
    for (i = 0; i < ndyn; i++)
	if (dyn[i].d_tag == DT_PLTGOT)
	    return dyn[i].d_un.d_val;
    return 0;
}

/*--------------------------------------------------------------------------
 * elfLoad - This routine loads the ELF file text and data segments into
 *  memory.  It also reads the file symbol table.
 *
 *  elfSymLoad is based on this routine.  Make sure that changes in the
 *  sections that are the same in the two functions get reflected in both
 *  functions.
 *-------------------------------------------------------------------------*/
BOOL elfLoad(const char *file_name, int s_argc, char *s_argv[])
{
    struct os_proc proc = {};
    Elf *elfptr;
    int fd;
    unsigned char class;
    ADDR unwind_base = 0, end_addr = 0;
    REG rsc;
    BOOL abi;

    DEBUG("%s('%s')\n", __func__, file_name);

    if ((fd = peChk(file_name)) != -1)
	return peLoad(fd, s_argc, s_argv);
    elfptr = elfOpen(file_name, stderr, &fd, EO_EXEC, &class);
    if (elfptr == NULL)
	return NO;

    /*
     * XXX - setABI() clobbers unixABI. In fact, it clobbers dosABI as well.
     * Rather bogus. In anyway, save unixABI here so that we can recover
     * later.
     */
    abi = unixABI;

    /* XXX - for now so that memory pages are auto-allocated */
    setABI(YES);

    proc.has_rtld = 0;
    proc.aout_base = ~0ULL;
    proc.aout_phdr = ~0ULL;

    if (class == ELFCLASS64) {
	Elf64_Ehdr *ehdr;
	Elf64_Phdr *phdr;
	unsigned i;
	ADDR pstart, pend;

	DEBUG("ELF64\n");

	if (!(ehdr = elf64_getehdr(elfptr))) {
	    elfClose(elfptr, fd);
	    return NO;
	}

	/* XXX - NonStop hack */
	if (ehdr->e_type == 100) {
	    ehdr->e_type = ET_EXEC;
	    ehdr->e_flags |= EF_IA_64_BE;
	    (void)grSet(0, GP_ID, getNSgp(elfptr));
	    DEBUG("*** Enable Big Endian hack\n");
	}

	if (!(phdr = elf64_getphdr(elfptr))) {
	    (void)fprintf(stderr, "%s - %s\n", file_name, elf_errmsg(0));
	    elfClose(elfptr, fd);
	    return NO;
	}

	ADDR bias = 0;
	if (ehdr->e_type == ET_DYN) {
		/* PIE: a base that looks like ld.so but does not conflict
		  with it. TODO: make it random. */
		bias = 0x2000000100000000ULL;
		DEBUG("bias: %#llx\n", bias);
	}

	entry_ip = bias + ehdr->e_entry;
	DEBUG("entry: %#lx\n", ehdr->e_entry);
	proc.ehdr_flags = ehdr->e_flags;

	DEBUG("program headers: %u\n", ehdr->e_phnum);
	for (i = 0; i < ehdr->e_phnum; i++)
	    switch (phdr[i].p_type) {
#ifdef __hpux
		case PT_PHDR:
		    /* XXX - check alignment?  Must be 4K aligned? */
		    /* TODO: account for ELF bias? */
		    proc.aout_phdr = bias + phdr[i].p_vaddr;
		    break;
#endif
		case PT_IA_64_UNWIND:
		    /* TODO: account for ELF bias? */
		    addLM5(file_name, phdr, ehdr->e_phnum, bias, 1);
		    unwind_base = bias + phdr[i].p_vaddr;
		    /*unwind_end = bias + phdr[i].p_vaddr + phdr[i].p_filesz;*/
		    DEBUG("PT_IA_64_UNWIND: %#llx\n", unwind_base);
		    break;
		case PT_INTERP:
		    if (!abi) {
			(void)fprintf(stderr,
				"%s - PT_INTERP segment not allowed\n",
				file_name);
			elfClose(elfptr, fd);
			return NO;
		    }
		    if (!interp(fd, phdr[i].p_offset, phdr[i].p_filesz))
			return NO;
		    proc.has_rtld = 1;
		    proc.rtld_base = text_base;
		    DEBUG("PT_INTERP: rtld_base: %#llx\n", proc.rtld_base);
		    break;
		case PT_LOAD:
		    pstart = bias + phdr[i].p_vaddr;
		    pend = pstart + phdr[i].p_memsz - 1;
		    DEBUG("PT_LOAD: pstart: %#llx - %#llx\n", pstart, pend);
		    if (!elf64SegmentLoad(&phdr[i], pstart, fd, file_name)) {
			elfClose(elfptr, fd);
			return NO;
		    }
		    if (phdr[i].p_flags & PF_X) {
			/* XXX - shouldn't assume aout_base == text_base? */
			/* XXX - especially if aout_base needs swapping? */
			text_base = proc.aout_base = pstart;
			text_end = pend;
			DEBUG("*** PT_LOAD: text_base: %#llx - %#llx\n", text_base, text_end);
		    } else
			end_addr = pend + 1;
	    }
	elf64_slurp_all_symbols(elfptr, ehdr, phdr, bias);
	proc.phdr_count = ehdr->e_phnum;
	proc.phdr_addr = proc.aout_base + ehdr->e_phoff;
	proc.proc_entry = bias + ehdr->e_entry;
    } else {	/* ELFCLASS32 */
	Elf32_Ehdr *ehdr;
	Elf32_Phdr *phdr;
	unsigned i;
	ADDR pstart, pend;

	if (!(ehdr = elf32_getehdr(elfptr))) {
	    elfClose(elfptr, fd);
	    return NO;
	}

	if (!(phdr = elf32_getphdr(elfptr))) {
	    (void)fprintf(stderr, "%s - %s\n", file_name, elf_errmsg(0));
	    elfClose(elfptr, fd);
	    return NO;
	}

	entry_ip = ehdr->e_entry;
	ADDPTR(entry_ip);	/* XXX - check lp64? If so, not set yet! */
	proc.ehdr_flags = ehdr->e_flags;

	for (i = 0; i < ehdr->e_phnum; i++)
	    switch (phdr[i].p_type) {
#ifdef __hpux
		case PT_PHDR:
		    proc.aout_phdr = phdr[i].p_vaddr;
		    break;
#endif
		case PT_IA_64_UNWIND:
		    unwind_base = phdr[i].p_vaddr;
		    /*unwind_end = phdr[i].p_vaddr + phdr[i].p_filesz;*/
		    break;
		case PT_INTERP:
		    if (!abi) {
			(void)fprintf(stderr,
				"%s - PT_INTERP segment not allowed\n",
				file_name);
			elfClose(elfptr, fd);
			return NO;
		    }
		    if (!interp(fd, phdr[i].p_offset, phdr[i].p_filesz))
			return NO;
		    proc.has_rtld = 1;
		    proc.rtld_base = text_base;
		    break;
		case PT_LOAD:
		    pstart = phdr[i].p_vaddr;
		    ADDPTR(pstart);	/* XXX - check lp64? */
		    pend = pstart + phdr[i].p_memsz - 1;
		    if (!elf32SegmentLoad(&phdr[i], pstart, fd, file_name)) {
			elfClose(elfptr, fd);
			return NO;
		    }
		    if (phdr[i].p_flags & PF_X) {
			text_base = proc.aout_base = pstart;
			text_end = pend;
		    } else
			end_addr = pend + 1;
	    }
	elf32_slurp_all_symbols(elfptr, ehdr, phdr, 0);
	proc.phdr_count = ehdr->e_phnum;
	proc.phdr_addr = proc.aout_base + ehdr->e_phoff;
	proc.proc_entry = ehdr->e_entry;
    }

#ifdef __hpux
    if (proc.aout_phdr == ~0ULL) {
	(void)fprintf(stderr, "%s - missing PT_PHDR segment\n", file_name);
	elfClose(elfptr, fd);
	return NO;
    }

    if (proc.aout_base == ~0ULL) {
	(void)fprintf(stderr, "%s - missing text segment\n", file_name);
	elfClose(elfptr, fd);
	return NO;
    }
#endif

    lp64 = proc.ehdr_flags & EF_IA_64_ABI64 ? YES : NO;
    X_PSR_BE(psrval) = proc.ehdr_flags & EF_IA_64_BE ? YES : NO;
    /* XXX - this is OK for app mode, but if norecov is set for sys mode,
       we need to set itlb.ed in page table entries for code pages */
#if 0
    /* XXX - set norecov BOOL based on PF_IA_64_NORECOV */
    X_PSR_ED(psrval) = alwaysNat && norecov ? YES : NO;
#else
    X_PSR_ED(psrval) = alwaysNat ? YES : NO;
#endif

    lminfo[numLms].text_base = text_base;
    lminfo[numLms].text_end = text_end;
    lminfo[numLms].unwind_base = unwind_base;
    /* XXX - replace call to strdup */
    lminfo[numLms].name = strdup(file_name);
    numLms++;
    if (interface != BATCH)
	initDwUnitTbl(fd);
    elfClose(elfptr, fd);
    fileLoaded = YES;
    setABI(abi);

    (void)ipSet(0, entry_ip);
    setPrgwIP(entry_ip);
    rsc = arGet(0, RSC_ID);
    XRSC_BE(rsc) = X_PSR_BE(psrval);
    (void)arSet(0, RSC_ID, rsc);
    setLp64(0, lp64);
    (void)psrSet(0, psrval);

    /* initialize heap pointer */
    heapSet(0, 16 * ((end_addr + 15)/16));

    if (!unixABI)
	return YES;

    os_setup_process(file_name, s_argc, s_argv, &proc);

    X_PSR_CPL(psrval) = 3;	/* XXX - needed? */
    (void)psrSet(0, psrval);
    XRSC_PL(rsc) = 3;
    (void)arSet(0, RSC_ID, rsc);

    return YES;
}

static int peChk(const char *name)
{
    int fd;

    if ((fd = open(name, O_RDONLY)) == -1)
        (void)fprintf(stderr, "%s - %s\n", name, strerror(errno));
    else {
	char id[2];
	ssize_t ret;

	ret = read(fd, id, 2);
	if (ret != 2 || id[0] != 'M' || id[1] != 'Z') {
	    close(fd);
	    fd = -1;
	}
    }
    return fd;
}

#define IMAGE_FILE_MACHINE_IA64	0x200
#define IMAGE_SUBSYSTEM_EFI_APPLICATION 10
#define SYSTEM_TABLE_SIZE 0x78
#define RUNTIME_TABLE_SIZE 0x70
#define BOOT_TABLE_SIZE 0x170

static BOOL peLoad(int fd, int s_argc, char *s_argv[])
{
    int i;
    unsigned argv_sz, ptr_sz, aout_sz, str_sz, arg_sz;
    ADDR sp, base_sp, arg_p, str_p, aout_path;
    ADDR addr, end_addr = 0;
    REG psrval = 0, rsc;
    ADDR image_base, entry, entry_gp;
    WORD ep, code_base, z, symTblOfs, num_syms, *sec_addr, strTblBase;
    HWORD machine, opt_hdr_size, subsyst;
    unsigned char buf2[2], buf4[4], buf8[8];
    unsigned num_sections;
    ssize_t r;

    lseek(fd, 0x3C, SEEK_SET);
    r = read(fd, buf4, 4);
    (void)r;
    z = (WORD)buf4[3] << 24 | (WORD)buf4[2] << 16 |
	(WORD)buf4[1] << 8 | buf4[0];

    lseek(fd, z, SEEK_SET);
    r = read(fd, buf4, 4);
    (void)r;
    if (memcmp(buf4, "PE\0", 4)) {
	(void)fprintf(stderr, "Incorrect signature\n");
	return NO;
    }
    r = read(fd, buf2, 2);
    (void)r;
    machine = (HWORD)buf2[1] << 8 | buf2[0];
    if (machine != IMAGE_FILE_MACHINE_IA64) {
	(void)fprintf(stderr, "Incorrect machine type\n");
	return NO;
    }
    r = read(fd, buf2, 2);
    (void)r;
    num_sections = (HWORD)buf2[1] << 8 | buf2[0];

    lseek(fd, 4, SEEK_CUR);	/* skip TimeDateStamp */
    r = read(fd, buf4, 4);
    (void)r;
    symTblOfs = (WORD)buf4[3] << 24 | (WORD)buf4[2] << 16 |
		(WORD)buf4[1] << 8 | buf4[0];
    r = read(fd, buf4, 4);
    (void)r;
    num_syms = (WORD)buf4[3] << 24 | (WORD)buf4[2] << 16 |
	       (WORD)buf4[1] << 8 | buf4[0];
    strTblBase = symTblOfs+18*num_syms;	/* 18 is size of a symbol entry */

    r = read(fd, buf2, 2);
    (void)r;
    opt_hdr_size = (HWORD)buf2[1] << 8 | buf2[0];

    lseek(fd, z+0x28, SEEK_SET);
    r = read(fd, buf4, 4);
    (void)r;
    ep = (WORD)buf4[3] << 24 | (WORD)buf4[2] << 16 |
	 (WORD)buf4[1] << 8 | buf4[0];

    /* XXX - code_base not used */
    r = read(fd, buf4, 4);
    (void)r;
    code_base = (WORD)buf4[3] << 24 | (WORD)buf4[2] << 16 |
		(WORD)buf4[1] << 8 | buf4[0];

    r = read(fd, buf8, 8);
    (void)r;
    image_base = (ADDR)buf8[7] << 56 | (ADDR)buf8[6] << 48 |
		 (ADDR)buf8[5] << 40 | (ADDR)buf8[4] << 32 |
		 (ADDR)buf8[3] << 24 | (ADDR)buf8[2] << 16 |
		 (ADDR)buf8[1] <<  8 | (ADDR)buf8[0];

    lseek(fd, 36, SEEK_CUR);	/* Skip to subsystem */
    r = read(fd, buf2, 2);
    (void)r;
    subsyst = (HWORD)buf2[1] << 8 | buf2[0];
    if (subsyst != IMAGE_SUBSYSTEM_EFI_APPLICATION) {
	(void)fprintf(stderr, "Incorrect subsystem (%d)\n", subsyst);
	return NO;
    }

    lp64 = YES;
    X_PSR_BE(psrval) = NO;
    X_PSR_ED(psrval) = NO;
    X_PSR_IS(psrval) = NO;
    setABI(YES); /* XXX - for now so that memory pages are auto-allocated */
    unixABI = NO;

    lseek(fd, z+0x18+opt_hdr_size, SEEK_SET);

    sec_addr = malloc(num_sections * sizeof *sec_addr);
    for (i = 0; i < num_sections; i++) {
	WORD mem_size, mem_addr, file_size, file_ofs;
	off_t sav;
	char *buf;
	ssize_t r;

	lseek(fd, 8, SEEK_CUR);	/* name */
	r = read(fd, buf4, 4);
	(void)r;
	mem_size = (WORD)buf4[3] << 24 | (WORD)buf4[2] << 16 |
		   (WORD)buf4[1] << 8 | buf4[0];
	r = read(fd, buf4, 4);
	(void)r;
	mem_addr = (WORD)buf4[3] << 24 | (WORD)buf4[2] << 16 |
		   (WORD)buf4[1] << 8 | buf4[0];
	r = read(fd, buf4, 4);
	(void)r;
	file_size = (WORD)buf4[3] << 24 | (WORD)buf4[2] << 16 |
		    (WORD)buf4[1] << 8 | buf4[0];
	r = read(fd, buf4, 4);
	(void)r;
	file_ofs = (WORD)buf4[3] << 24 | (WORD)buf4[2] << 16 |
		   (WORD)buf4[1] << 8 | buf4[0];
	sav = lseek(fd, 16, SEEK_CUR);
	lseek(fd, file_ofs, SEEK_SET);
	buf = calloc(mem_size, 1);
	if (file_size > mem_size)
	    file_size = mem_size;
	r = read(fd, buf, file_size);
	(void)r;
	sec_addr[i] = image_base+mem_addr;
{
ADDR a, s, e;
s = image_base+mem_addr;
e = s + mem_size - 1;
    for (a = page_base(s); a <= page_base(e); a += page_size)
	memZerosPageSet(a);
}
	memBBWrt(image_base+mem_addr, (BYTE *)buf, mem_size);
	free(buf);
	lseek(fd, sav, SEEK_SET);
    }

    memMRd(image_base+ep, 8, &entry);
    memMRd(image_base+ep+8, 8, &entry_gp);
    (void)ipSet(0, entry);
    (void)grSet(0, GP_ID, entry_gp);
{
    lseek(fd, symTblOfs, SEEK_SET);
    for (i = 0; i < num_syms; i++) {
	char *sym_name;
	WORD sym_value;
	HWORD section_num, sym_type;
	BYTE sym_class, num_aux;
	ssize_t r;

	r = read(fd, buf8, 8);
	(void)r;
	if (buf8[0] || buf8[1] || buf8[2] || buf8[3]) {
	    sym_name = malloc(9);
	    memcpy(sym_name, buf8, 8);
	    sym_name[8] = '\0';
	} else {
	    off_t sav = lseek(fd, 0, SEEK_CUR);
	    WORD strTblOfs;
	    size_t sym_len = 0;
	    unsigned char ch;

	    strTblOfs = (WORD)buf8[7] << 24 | (WORD)buf8[6] << 16 |
			(WORD)buf8[5] << 8 | buf8[4];
	    lseek(fd, strTblBase+strTblOfs, SEEK_SET);
	    do {
		r = read(fd, &ch, 1);
		(void)r;
		sym_len++;
	    } while (ch);
	    lseek(fd, strTblBase+strTblOfs, SEEK_SET);
	    sym_name = malloc(sym_len);
	    r = read(fd, sym_name, sym_len);
	    (void)r;
	    lseek(fd, sav, SEEK_SET);
	}
	r = read(fd, buf4, 4);
	(void)r;
	sym_value = (WORD)buf4[3] << 24 | (WORD)buf4[2] << 16 |
		    (WORD)buf4[1] << 8 | buf4[0];
	r = read(fd, buf2, 2);
	(void)r;
	section_num = (HWORD)buf2[1] << 8 | buf2[0];
	r = read(fd, buf2, 2);
	(void)r;
	sym_type = (HWORD)buf2[1] << 8 | buf2[0];
	/* XXX - sym_type is always 0, add check? */
	r = read(fd, &sym_class, 1);
	(void)r; /* XXX - sym_class is always 2 or 3, add check? */
	r = read(fd, &num_aux, 1);
	(void)r; /* XXX - num_aux is always 0, add check? */

	if (section_num > 0)
	    symInsert(sym_name, sec_addr[section_num-1]+sym_value, 0);
	free(sym_name);
    }
    free(sec_addr);
}

    close(fd);
    fileLoaded = YES;
    setABI(unixABI);

    setPrgwIP(ipGet(0));
    rsc = arGet(0, RSC_ID);
    XRSC_BE(rsc) = X_PSR_BE(psrval);
    (void)arSet(0, RSC_ID, rsc);
    setLp64(0, lp64);
    (void)psrSet(0, psrval);

    /* initialize heap pointer */
#if 0
    heapSet(page_size * ((end_addr + (page_size-1))/page_size));
#else
    heapSet(0, 16 * ((end_addr + 15)/16));
#endif

    /* Runtime stack */
    for (argv_sz = i = 0; i < s_argc; i++)
	argv_sz += strlen(s_argv[i]) + 1;
    aout_sz = strlen(s_argv[0]) + 1;

    ptr_sz = lp64 ? sizeof (DWORD) : sizeof (WORD);
    if (ptr_sz == 8)
	sp = 0xFFFFFFULL;
    else
	sp = 0x200000007FFFFFFFULL;
    sp++;
    setMaxSP(sp);

    str_sz = argv_sz + aout_sz;
    arg_sz = (s_argc + 2) * ptr_sz;
    arg_p = base_sp = (sp - (str_sz + arg_sz + 0x800000)) & ~(ADDR)0xF;
    str_p = base_sp + arg_sz;
    aout_path = sp - aout_sz;

    /* Allocate all necessary pages */
    for (addr = page_base(base_sp); addr < sp; addr += page_size)
	memZerosPageSet(addr);

    (void)grSet(0, SP_ID, base_sp);
    (void)grSet(0, 32, 0);
    (void)grSet(0, 33, arg_p);
    for (i = 0; i < s_argc; i++) {
	memBBWrt(str_p, (BYTE *)s_argv[i], strlen(s_argv[i]) + 1);
	memMWrt(arg_p, ptr_sz, str_p);
	str_p += strlen(s_argv[i]) + 1;
	arg_p += ptr_sz;
    }
    memMWrt(arg_p, ptr_sz, 0);
    arg_p += ptr_sz;

    memBBWrt(aout_path, (BYTE *)s_argv[0], aout_sz);

    /* Backing Store */
    if (ptr_sz == 8) {
#ifdef __linux__
	arSet(0, BSP_ID,   0x9FFFFFFF80000000ULL);
	arSet(0, BSPST_ID, 0x9FFFFFFF80000000ULL);
#else
	(void)arSet(0, BSP_ID, 0x9FFFFFFF7F600000ULL);
	(void)arSet(0, BSPST_ID, 0x9FFFFFFF7F600000ULL);
#endif
    } else {
	(void)arSet(0, BSP_ID, 0x200000007A800000ULL);
	(void)arSet(0, BSPST_ID, 0x200000007A800000ULL);
    }
    return YES;
}
