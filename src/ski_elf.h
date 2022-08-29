/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*============================================================================

   File: elf_em.h
        Include file for ELF IA_64 file. Defines both 32-/64-bit files. 
============================================================================*/
/*
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
#ifndef _SKI_ELF_IA_64_INCLUDED
#define _SKI_ELF_IA_64_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define DT_HP_DLD_FLAGS			0
#define DT_HP_DEBUG_PRIVATE		0
#define DT_HP_DEBUG_CALLBACK		0
#define DT_HP_DEBUG_CALLBACK_BOR	0

/*============================================================================
   The ELF_* macros are the allowed values of e_machine
============================================================================*/

#define ELF_IA_64     50

/*============================================================================
   The EF_* macros define bits in the e_flags field of the ElfXX_Ehdr
   structure.
============================================================================*/

#define EF_IA_64_TRAPNIL         0x00000001  /* Trap nil pointer dereferences */
#define EF_IA_64_LAZYSWAP        0x00000002  /* Lazy Swap algorithm */
#define EF_IA_64_BE              0x00000008  /* PSR BE bit set (big-endian) */
#define EF_IA_64_ABI64           0x00000010  /* 64-bit ABI */

#define EF_IA_64_REDUCEDFP       0x00000020  
                                          /* Reserved for kernel conventions */
#define EF_IA_64_CONSGP          0x00000040 
                                          /* Reserved for kernel conventions */ 
#define EF_IA_64_NOFUNCDESC_CONS_GP  0x00000080
                                          /* Reserved for kernel conventions */
#define EF_IA_64_ABSOLUTE        0x00000100
                                          /* Reserved for kernel conventions */

#ifndef EF_IA_64_ARCH
#define EF_IA_64_ARCH            0xFF000000  /* architecture version mask */
#endif

#ifndef EF_IA_64_MASKOS
#define EF_IA_64_MASKOS          0x00FF000F  /* Bits reserved for OS-specific flags*/
#endif
#define EFA_IA_64                0x00000000

/*============================================================================
   The PT_* macros are the values of p_type in ElfXX_Phdr.
============================================================================*/

#define PT_IA_64_ARCHEXT   (PT_LOPROC + 0) /* arch. extension bits */
#define PT_IA_64_UNWIND    (PT_LOPROC + 1) /* IA64 unwind bits */

/*============================================================================
   The PF_* macros are the segment flag bits in p_flags of ElfXX_Phdr.
============================================================================*/

#define PF_IA_64_NORECOV   0x80000000  /* segment contains code that uses
                                          speculative instructions w/o 
                                          recovery code. */

/*============================================================================
   The ELF_STRING_xxx macros are names of common sections
============================================================================*/

#define ELF_STRING_IA_64_archext           ".IA_64.archext"
#define ELF_STRING_IA_64_milli             ".IA_64.milli"
#define ELF_STRING_IA_64_unwind_hdr        ".IA_64.unwind_hdr"
#define ELF_STRING_IA_64_unwind            ".IA_64.unwind"
#define ELF_STRING_IA_64_unwind_info       ".IA_64.unwind_info"
#define ELF_STRING_HP_opt_annot            ".HP.opt_annot"  // JAGab77569 -lmtran


/*============================================================================
   The SHF_* macros are the allowed values of the sh_flags field of 
   ElfXX_Shdr.  These 1-bit flags define attributes of a section.
============================================================================*/

#define SHF_IA_64_SHORT    0x10000000 /* section near gp */
#define SHF_IA_64_NORECOV  0x20000000 /* use speculative instr. w/o
					 recovery code */

/*============================================================================
   SHN_* macros are reserved section header indexes.  An object file will
   not have sections for these special indexes.
============================================================================*/

#define SHN_IA_64_ANSI_COMMON      (SHN_LOPROC + 0)


/*============================================================================
   SHNX_* macros are reserved section header indexes for use with 24-bit
   section tables.  An object file will not have sections for these 
   special indexes.
============================================================================*/

#define SHNX_IA_64_ANSI_COMMON  (SHNX_LOPROC + 0)


/*============================================================================
   SHT_* macros are the values of sh_type in ElfXX_Shdr
============================================================================*/

#define SHT_IA_64_EXT    (SHT_LOPROC + 0) /* product-specific extension bits */
#define SHT_IA_64_UNWIND (SHT_LOPROC + 1) /* unwind bits */

/*============================================================================
   DT_* macros are the values of the dynamic table
============================================================================*/

#ifndef DT_IA_64_PLT_RESERVE
#define DT_IA_64_PLT_RESERVE    0x70000000
#endif

/*============================================================================
   The R_* macros are the IA_64 relocation types
============================================================================*/
	/*
	** These are "real" relocations.  The offset in a relocation
	** applied to a data location is the actual byte address of the
	** 32-/64-bit field to relocate.  The value of (offset & ~3) in
	** an instruction relocation is the byte offset of the bundle
	** the instruction lives in; the value of (offset & 3) signifies:
	**   0: first  instruction slot in bundle
	**   1: second instruction slot in bundle
	**   2: third  instruction slot in bundle
	**
	** Little piece of info: the first (hex) digit specifies the
	** expression type, while the second specifies the format of
	** the data word being relocated.
	*/

#define R_IA_64_NONE		0x00    /* none	*/

#define R_IA_64_IMM14		0x21    /* symbol + add, add imm14	*/
#define R_IA_64_IMM22		0x22    /* symbol + add, add imm22	*/
#define R_IA_64_IMM64		0x23    /* symbol + add, mov imm64	*/

#define R_IA_64_DIR32MSB	0x24    /* symbol + add, data4MSB	*/
#define R_IA_64_DIR32LSB	0x25    /* symbol + add, data4LSB	*/
#define R_IA_64_DIR64MSB	0x26    /* symbol + add, data8MSB	*/
#define R_IA_64_DIR64LSB	0x27    /* symbol + add, data8LSB	*/

#define R_IA_64_GPREL22		0x2a	/* @gprel(sym + add), add imm22 */
#define R_IA_64_GPREL64I	0x2b	/* @gprel(sym + add), mov imm64 */
#define R_IA_64_GPREL32MSB	0x2c	/* @gprel(sym + add), data4MSB  */
#define R_IA_64_GPREL32LSB	0x2d   	/* @gprel(sym + add), data4LSB  */
#define R_IA_64_GPREL64MSB	0x2e   	/* @gprel(sym + add), data8MSB  */
#define R_IA_64_GPREL64LSB	0x2f   	/* @gprel(sym + add), data8LSB  */

#define R_IA_64_LTOFF22		0x32	/* @ltoff(sym + add), add imm22 */
#define R_IA_64_LTOFF64I	0x33	/* @ltoff(sym + add), mov imm64 */
 
#define R_IA_64_PLTOFF22	0x3a	/* @pltoff(sym) + add, add imm22 */
#define R_IA_64_PLTOFF64I	0x3b	/* @pltoff(sym) + add, mov imm64 */
#define R_IA_64_PLTOFF64MSB	0x3e	/* ditto, data8 MSB */
#define R_IA_64_PLTOFF64LSB	0x3f	/* ditto, data8 LSB */

#define R_IA_64_FPTR64I		0x43	/* @fptr(sym) + add, mov imm64 */
#define R_IA_64_FPTR32MSB	0x44	/* ditto, data4 MSB */
#define R_IA_64_FPTR32LSB	0x45	/* ditto, data4 LSB */
#define R_IA_64_FPTR64MSB	0x46	/* ditto, data8 MSB */
#define R_IA_64_FPTR64LSB	0x47	/* ditto, data8 LSB */

#define R_IA_64_PCREL60B        0x48    /* sym + add - IP */
#define R_IA_64_PCREL21B	0x49	/* sym + add - IP, ptb.call */
#define R_IA_64_PCREL21M	0x4a	/* sym + add - IP, chk.s */
#define R_IA_64_PCREL21F	0x4b	/* sym + add - IP, fchkf */

#define R_IA_64_PCREL32MSB	0x4c	/* @pcrel(sym + add), data4 MSB */
#define R_IA_64_PCREL32LSB	0x4d	/* @pcrel(sym + add), data4 LSB */
#define R_IA_64_PCREL64MSB	0x4e	/* @pcrel(sym + add), data8 MSB */
#define R_IA_64_PCREL64LSB	0x4f	/* @pcrel(sym + add), data8 LSB */

#define R_IA_64_LTOFF_FPTR22	0x52   /* @ltoff(@fptr(sym) + add), add imm22 */
#define R_IA_64_LTOFF_FPTR64I	0x53   /* @ltoff(@fptr(sym) + add), mov imm64 */
#define R_IA_64_LTOFF_FPTR32MSB	0x54   /* @ltoff(@fptr(sym) + add), data 4 MSB*/
#define R_IA_64_LTOFF_FPTR32LSB	0x55   /* @ltoff(@fptr(sym) + add), data 4 LSB*/
#define R_IA_64_LTOFF_FPTR64MSB	0x56   /* @ltoff(@fptr(sym) + add), data 8 MSB*/
#define R_IA_64_LTOFF_FPTR64LSB	0x57   /* @ltoff(@fptr(sym) + add), data 8 LSB*/

#define R_IA_64_SEGREL32MSB	0x5c	/* @segrel(sym + add), data4 MSB */
#define R_IA_64_SEGREL32LSB	0x5d	/* @segrel(sym + add), data4 LSB */
#define R_IA_64_SEGREL64MSB	0x5e	/* @segrel(sym + add), data8 MSB */
#define R_IA_64_SEGREL64LSB	0x5f	/* @segrel(sym + add), data8 LSB */

#define R_IA_64_SECREL32MSB	0x64	/* @secrel(sym + add), data4 MSB */
#define R_IA_64_SECREL32LSB	0x65	/* @secrel(sym + add), data4 LSB */
#define R_IA_64_SECREL64MSB	0x66	/* @secrel(sym + add), data8 MSB */
#define R_IA_64_SECREL64LSB	0x67	/* @secrel(sym + add), data8 LSB */

#define R_IA_64_REL32MSB	0x6c	/* RF + C, data4 MSB */
#define R_IA_64_REL32LSB	0x6d	/* RF + C, data4 LSB */
#define R_IA_64_REL64MSB	0x6e	/* RF + C, data8 MSB */
#define R_IA_64_REL64LSB	0x6f	/* RF + C, data8 LSB */

#define R_IA_64_LTV32MSB	0x74	/* sym + add, data4 MSB */
#define R_IA_64_LTV32LSB	0x75	/* sym + add, data4 LSB */
#define R_IA_64_LTV64MSB	0x76	/* sym + add, data8 MSB */
#define R_IA_64_LTV64LSB	0x77	/* sym + add, data8 LSB */

#define R_IA_64_PCREL21BI	0x79	/* sym + add - IP */
#define R_IA_64_PCREL22		0x7a    /* sym + add - IP */
#define R_IA_64_PCREL64I        0x7b    /* sym + add - IP */

#define R_IA_64_IPLTMSB		0x80	/* dyn reloc, imported PLT, MSB */
#define R_IA_64_IPLTLSB		0x81	/* dyn reloc, imported PLT, LSB */
#define R_IA_64_EPLTMSB		0x82	/* dyn reloc, exported PLT, MSB */
#define R_IA_64_EPLTLSB		0x83	/* dyn reloc, exported PLT, LSB */
#define R_IA_64_COPY		0x84	/* dyn reloc, data copy         */

#define R_IA_64_SUB             0x85    /* add - sym, add imm22 (A5) */
#define R_IA_64_LTOFF22X        0x86    /* @ltoff(sym + add), add imm22 (A5) */
#define R_IA_64_LDXMOV          0x87    /* */ 

    /* see IA-64 Runtime Architecture Supplement: Thread-Local Storage */
#define R_IA_64_TPREL14		0x91	/* @tprel(sym + add) */
#define R_IA_64_TPREL22		0x92	/* sym - TP + add, add imm22 */
#define R_IA_64_TPREL64I	0x93
#define R_IA_64_TPREL64MSB	0x96	/* sym - TP + add, data8 MSB */
#define R_IA_64_TPREL64LSB	0x97	/* sym - TP + add, data8 LSB */

#define R_IA_64_LTOFF_TPREL22	0x9a	/* @ltoff(@tprel(sym + add)) */

#define R_IA_64_DTPMOD64MSB	0xa6	/* @dtpmod(sym + add) */
#define R_IA_64_DTPMOD64LSB	0xa7	
#define R_IA_64_LTOFF_DTPMOD22	0xaa	/* @ltoff(@dtpmod(sym + add)) */

#define R_IA_64_DTPREL14	0xb1	/* @dtprel(sym + add) */
#define R_IA_64_DTPREL22	0xb2	/* @dtprel(sym + add) */
#define R_IA_64_DTPREL64I	0xb3	/* @dtprel(sym + add) */
#define R_IA_64_DTPREL32MSB	0xb4	/* @dtprel(sym + add) */
#define R_IA_64_DTPREL32LSB	0xb5	/* @dtprel(sym + add) */
#define R_IA_64_DTPREL64MSB	0xb6	/* @dtprel(sym + add) */
#define R_IA_64_DTPREL64LSB	0xb7	/* @dtprel(sym + add) */

#define R_IA_64_LTOFF_DTPREL22	0xba	/* @ltoff(@dtprel(sym + add)) */

#ifdef __cplusplus
}
#endif

#endif /* _SKI_ELF_IA_64_INCLUDED */
