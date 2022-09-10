#ifndef _INCLUDE_ELF_H_
#define _INCLUDE_ELF_H_

#include <types.h>

#define PT_LOAD   1
#define ET_EXEC   2
#define EM_IA_64 50

struct elf64_hdr {
    unsigned char e_ident[16]; /* "\x7fELF" */
    u16 e_type;
    u16 e_machine;
    u32 e_version;
    u64 e_entry;
    u64 e_phoff;
    u64 e_shoff;
    u32 e_flags;
    u16 e_ehsize;
    u16 e_phentsize;
    u16 e_phnum;
    u16 e_shentsize;
    u16 e_shnum;
    u16 e_shstrndx;
};

struct elf64_phdr {
    u32 p_type;
    u32 p_flags;
    u64 p_offset;
    u64 p_vaddr;
    u64 p_paddr;
    u64 p_filesz;
    u64 p_memsz;
    u64 p_align;
};
#endif /* _INCLUDE_ELF_H_ */
