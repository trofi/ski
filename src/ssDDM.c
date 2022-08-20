/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Debugger Data Manipulation Routines
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

#include "config.h"

#ifdef HAVE_LIBUNWIND_IA64
# define HAVE_UNWIND_SUPPORT
#endif

#include <elf.h>
#if defined __linux
#include <stdint.h>
#endif
#include <string.h>
#ifdef HAVE_UNWIND_SUPPORT
# ifdef HAVE_LIBUNWIND_IA64
#   include <libunwind-ia64.h>
# else
#   if 1
#    include <unwind.h>
#   else
#    include "phaseIunwind/unwind.h"
#    include "phaseIunwind/crt0.h"
#   endif
#   define _Unwind_StepResult UnwindStepResult
#   define _Unwind UnwindContext
#   define _Unwind_SetIP	UnwindContext_putIPval
#   define _Unwind_ctor3	UnwindContext_ctor3
#   define _Unwind_SetCFM	UnwindContext_putCFMval
#   define _Unwind_SetBR	UnwindContext_putBRval
#   define _Unwind_dtor	UnwindContext_dtor
#   define _Unwind_GetIP	UnwindContext_getIPval
#   define _Unwind_step	UnwindContext_step
#   define _Unwind_SetUM	UnwindContext_putUMval
#   define _Unwind_SetGR	UnwindContext_putGRval
#   define _Unwind_SetAR	UnwindContext_putARval
#   define _UW_GR_SP	GR_SP
#   define _UW_AR_PFS	AR_PFS
#   define _UW_AR_RNAT	AR_RNAT
#   define _UW_AR_BSP	AR_BSP
#   define _UW_AR_BSPSTORE	AR_BSPSTORE
#   define _UW_AR_RSC	AR_RSC
#   define _UW_BR_RP	BR_RP
#   define _UW_STEP_OK	UNWIND_STEP_OK
#   define _UW_STEP_BOTTOM UNWIND_STEP_BOTTOM
#    ifdef __ia64
#     include <dlfcn.h>
#    endif
# endif /* !HAVE_LIBUNWIND_IA64 */
#endif /* HAVE_UNWIND_SUPPORT */
#include "std.h"
#include "bits.h"
#include "types.h"
#include "fields.h"
#include "ssDCmd.h"
#include "ski.h"
#include "sim.h"
#include "exportui.h"
#include "coreui.h"
#include "libsym.h"
#include "decoder.h"
#include "libcore.h"
#include "libdas.h"
#include "asm.h"
#include "lm.h"
#include "state.h"
#include "simmem.h"

#if 0
#include "ssDefs.h"
#include "mp.h"
#include "ssReg.h"
#endif

#define CSTKSZ	50
#if 0
#define UNWIND_DEBUG
#endif

#if 0
/*##################### Globals - Exports ##################################*/

BOOL	symPsw = YES;

#endif

/*##################### Globals - Imports ##################################*/

extern LMINFO lminfo[];

/*##################### Local Variables   ##################################*/

/*##################### Functions and Subroutines ##########################*/

/**********************************/
/* Register Modification Routines */
/**********************************/

/*---------------------------------------------------------------------------
 * value$Assign - Assign the user specified value to the user-specifed
 *  internal symbol.
 *---------------------------------------------------------------------------*/
BOOL valAssign(unsigned argc, char *argv[])
{
    struct isym *psym, sym;
    REG val;

    if (!(psym = isymVLkp(argv[0]))) {
	cmdErr("Unrecognized symbol name: %s\n", argv[0]);
	return NO;
    }
    sym = *psym;
    if (!evalExpr(argv[1], psym->dbase, &val))
	return NO;

    switch (sym.type) {
	case SINGLE_SYM:
	    if (sym.setFn) {
		if (!sym.setFn(0, val)) {
		    cmdErr("Attempt to modify symbol failed\n");
		    return NO;
		}
	    } else {
		cmdWarn("Ignored attempt to write a Read-Only symbol\n");
		return NO;
	    }
	    break;
	case REGSET_SYM:
	    if (sym.setFn) {
		if (!sym.setFn(0, sym.ndx, val)) {
		    cmdErr("Attempt to modify symbol failed\n");
		    return NO;
		}
	    } else {
		cmdWarn("Ignored attempt to write a Read-Only symbol\n");
		return NO;
	    }
	    break;
	case BITF_SYM:
	    if (sym.setFn) {
		REG oldval = sym.getFn(0), mask;

		mask = ONES(sym.len) << (sym.start - (sym.len - 1));
		val <<= sym.start - (sym.len - 1);
		val = (oldval & ~mask) | (val & mask);
		if (!sym.setFn(0, val)) {
		    cmdErr("Attempt to modify symbol failed\n");
		    return NO;
		}
	    } else {
		cmdWarn("Ignored attempt to write a Read-Only symbol\n");
		return NO;
	    }
	    break;
	case RS_BITF_SYM:
	    if (sym.setFn) {
		REG oldval = sym.getFn(0, sym.ndx), mask;

		mask = ONES(sym.len) << (sym.start - (sym.len - 1));
		val <<= sym.start - (sym.len - 1);
		val = (oldval & ~mask) | (val & mask);
		if (!sym.setFn(0, sym.ndx, val)) {
		    cmdErr("Attempt to modify symbol failed\n");
		    return NO;
		}
	    } else {
		cmdWarn("Ignored attempt to write a Read-Only symbol\n");
		return NO;
	    }
	    break;
	default:
	    break;
    }
    setDerivedState(1);
    prgwPCRedraw();
    datwUpdate();
    regwUpdate();
    return YES;
}

/*---------------------------------------------------------------------------
 * memory$Assign - Assign specified data items of specified 'size' to memory
 *  starting with the given address.
 *---------------------------------------------------------------------------*/
static BOOL memAssign(unsigned argc, char *argv[], unsigned size)
{
    ADDR adr, tadr;
    REG val, tval;
    REG adrmask  = size - 1;
    REG datamask = ONES(size*8);
    unsigned i;
    BOOL ret = YES;
    static const char sizestr[][12] = {
	"0 ?", "byte", "halfword",
	"3 ?", "word", "5 ?",
	"6 ?", "7 ?", "doubleword"
    };

    if (!evalExpr(argv[0], HEXEXP, &adr))
	return NO;
    if (adr != (tadr = adr & ~adrmask)) {
	adr = tadr;
	cmdWarn("Non %s-aligned address.  Aligned to 0x%llx\n",
		sizestr[size], adr);
    }

    for (i = 1; i < argc; i++, adr += size) {
	if (!evalExpr(argv[i], HEXEXP, &val))
	    break;
	if (val != (tval = val & datamask)) {
	    val = tval;
	    cmdWarn("Data larger than %d byte%s.  Truncated to 0x%llx\n",
		    size, size > 1 ? "s" : "", val);
	}
	if (!memMWrt(adr, size, val))
	    cmdWarn("Assignment to address %llx failed\n", adr);
    }
    if (i != argc) {
	cmdErr("Some values could not be assigned\n");
	ret = NO;
    }
    prgwUpdate();
    datwUpdate();
    return ret;
}

/*---------------------------------------------------------------------------
 * byte$Assign - Assign specified byte(s) to memory starting with the given
 *  address.
 *---------------------------------------------------------------------------*/
BOOL byteAssign(unsigned argc, char *argv[])
{
    return memAssign(argc, argv, 1);
}

/*---------------------------------------------------------------------------
 * halfword$Assign - Assign specified halfword(s) to memory starting with
 *  the given address.
 *---------------------------------------------------------------------------*/
BOOL hwAssign(unsigned argc, char *argv[])
{
    return memAssign(argc, argv, 2);
}

/*---------------------------------------------------------------------------
 * word$Assign - Assign specified word(s) to memory starting with the given
 *  address.
 *---------------------------------------------------------------------------*/
BOOL wordAssign(unsigned argc, char *argv[])
{
    return memAssign(argc, argv, 4);
}

/*---------------------------------------------------------------------------
 * doubleword$Assign - Assign specified doubleword(s) to memory starting
 *  with the given address.
 *---------------------------------------------------------------------------*/
BOOL dwordAssign(unsigned argc, char *argv[])
{
    return memAssign(argc, argv, 8);
}

static ADDR badr;

ADDR encoderIP(void)
{
    return badr;
}

BOOL instrAssign(unsigned argc, char *argv[])
{
    ADDR adr, tadr;
    REG val, tval;
    REG datamask = ONES(41);
    unsigned i, slot;
    BOOL ret = YES;
    Bundle bndl;
    InstID instID;
    Operands ops;
    EncodedInst inst;

    if (!evalExpr(argv[0], HEXEXP, &adr))
	return NO;
    if ((slot = SLOT(adr)) > 2) {
	cmdErr("Illegal slot field in instruction address\n");
	return NO;
    }
    if (adr != (tadr = adr & ~(ADDR)0x3)) {
	adr = tadr;
	cmdWarn("Non slot-aligned address.  Aligned to 0x%llx\n", adr);
    }
    badr = adr & ~(ADDR)0xf;	/* bundle-align address */

    for (i = 1; i < argc; i++, slot++) {
	if (slot >= SLOTS_PER_BUNDLE) {
	    slot = 0;
	    badr += 16;
	}
	if (asm_inst(argv[i], &instID, &ops, &inst) != AsmOK) {
	    if (evalExpr(argv[i], HEXEXP, &val)) {
		if (val != (tval = val & datamask)) {
		    val = tval;
		    cmdWarn("Data larger than 41 bits.  Truncated to 0x%llx\n",
			    val);
		}
		inst.bits = val;
		inst.unit = No_Unit;
		inst.valid_extra_bits = 0;
	    } else
		break;
    }
	if (!memMIRd(badr, (DWORD *)&bndl)) {
	    cmdWarn("Assignment failed\n");
	    continue;
	}
	if (!instrReplace(&bndl, badr, slot, &inst)) {
	    cmdWarn("That instruction not allowed in that slot\n");
	    continue;
	}
	(void)memMIWrt(badr, (DWORD *)&bndl);
	if (inst.valid_extra_bits)
	    slot++;
    }
    if (i != argc) {
	cmdErr("Some values could not be assigned\n");
	ret = NO;
    }
    prgwUpdate();
    datwUpdate();
    return ret;
}

BOOL stringAssign(unsigned argc, char *argv[])
{
    ADDR adr;
    char *p;
    unsigned i;
    BOOL ret = YES;

    if (!evalExpr(argv[0], HEXEXP, &adr))
	return NO;
    for (p = argv[1]; *p; p++, adr++)
	if (!memMWrt(adr, 1, *p))
	    cmdWarn("Assignment failed\n");
    for (i = 2; i < argc; i++) {
	if (!memMWrt(adr++, 1, ' '))
	    cmdWarn("Assignment failed\n");
	for (p = argv[i]; *p; p++, adr++)
	    if (!memMWrt(adr, 1, *p))
		cmdWarn("Assignment failed\n");
    }
    if (!memMWrt(adr, 1, '\0'))
	cmdWarn("Assignment failed\n");
    if (i != argc) {
	cmdErr("Some values could not be assigned\n");
	ret = NO;
    }
    prgwUpdate();
    datwUpdate();
    return ret;
}

BOOL templAssign(unsigned argc, char *argv[])
{
    ADDR adr, tadr;
    REG val, tval;
    Bundle bndl;
    BYTE templ;
    REG datamask = ONES(5);
    unsigned i;
    BOOL ret = YES;

    if (!evalExpr(argv[0], HEXEXP, &adr))
	return NO;
    if (adr != (tadr = adr & ~(ADDR)0xf)) {
	adr = tadr;
	cmdWarn("Non bundle-aligned address.  Aligned to 0x%llx\n", adr);
    }

    for (i = 1; i < argc; i++, adr += 16) {
	if (asm_templ(argv[i], &templ) == AsmOK)
	    val = templ;
	else if (!evalExpr(argv[i], HEXEXP, &val))
	    continue;
	if (val != (tval = val & datamask)) {
	    val = tval;
	    cmdWarn("Data larger than 0x1f.  Truncated to 0x%llx\n", val);
	}
	templ = (BYTE)val;
	if (!memMIRd(adr, (DWORD *)&bndl)) {
	    cmdWarn("Assignment failed\n");
	    continue;
	}
	templReplace(&bndl, adr, templ);
	(void)memMIWrt(adr, (DWORD *)&bndl);
    }
    if (i != argc) {
	cmdErr("Some values could not be assigned\n");
	ret = NO;
    }
    prgwUpdate();
    datwUpdate();
    return ret;
}

#if 0

/*--------------------------------------------------------------------------
 *  generalRegister$Clear - Clear all general registers.
 *--------------------------------------------------------------------------*/
BOOL grClear(unsigned argc, char *argv[])
{
    int i;

    for (i = 0; i < NGRS; i++)
	grMWrt(i,0);
    grwUpdate();
}


/*************************************/
/* Main Memory Modification Routines */
/*************************************/
#endif

#if 0

/*******************************/
/* Space Manipulation Routines */
/*******************************/

/*--------------------------------------------------------------------------
 * space$GetBoundaries - Prompt for a space id, and required boundaries
 *  for pages to be allocated automatically in the space. The boundaries
 *  default to 1 for the lower bounds and 0 for the upper bounds.
 *--------------------------------------------------------------------------*/
BOOL spcGBds(unsigned argc, char *argv[])
{
    PGID lbd = 0, ubd = MAXPGID;
    REG accr = ~0, accid = ~0;

    if (Proclvl == level0) {
	cmdErr("No spaces in Level 0 systems\n");
	return;
    }

    if (getNum("Space ID: ", &sid, 16)) {
	getNumToken("Access rights: ", &accr, 16);
	getNumToken("Access ID: ", &accid, 16);
	getNumToken("Lower bound (page id #): ", &lbd, 16);
	getNumToken("Upper bound (page id #): ", &ubd, 16);
	if (!spcRestrict(sid, lbd, ubd, accr, accid))
	    cmdwPrint("Space created with given protection\n");
	else
	    cmdwPrint("Space protection boundaries changed\n");
    }
}


/*********************/
/* Page Manipulation */
/*********************/

/*--------------------------------------------------------------------------
 * page$GetProtectionFlags - Prompt for address and access flags, then change
 *  the protection on the page containing the specified address to
 *  the specified access(es), creating the page if necessary.
 *--------------------------------------------------------------------------*/
BOOL pagGProt(unsigned argc, char *argv[])
{
    ADDR adr;
    REG accr = ~0, accid = ~0;
    BOOL ok;

    if (Proclvl == level0) {
	cmdErr("No page protection in Level 0 systems\n");
	return;
    }

    if (getVA(&sid, &adr) > 0) {
	getNumToken("Access rights: ", &accr, 16);
	getNumToken("Access ID: ", &accid, 16);
	if ((ok = pdeCreate(sid, adr, accr, accid)) == -1)
	    cmdErr("Can't create page -- out of memory\n");
	else if (!ok)
	    cmdwPrint("Page created with specified protection\n");
	else {
	    dtlbPurge(sid, adr);
	    itlbPurge(sid, adr);
	    cmdwPrint("Access protection changed\n");
	}
    }
}

#endif


struct load_module {
    struct load_module *next;
    const char *name;
    ADDR load_base;
    Elf64_Phdr *phdr;
    int phnum;
    int discontiguous;	/* is unwind table/info discontinguous in hostmem? */
} *load_module_list;

void
addLM5 (const char *name, Elf64_Phdr *phdr, int phnum, ADDR load_base,
       int discontiguous)
{
    struct load_module *lm;

    lm = malloc (sizeof (struct load_module));
    memset (lm, 0, sizeof (*lm));

    lm->name = strdup (name);
    lm->phnum = phnum;
    lm->discontiguous = discontiguous;
    lm->load_base = load_base;
    lm->phdr = malloc (phnum * sizeof (Elf64_Phdr));
    memcpy (lm->phdr, phdr, phnum * sizeof (Elf64_Phdr));

    lm->next = load_module_list;
    load_module_list = lm;
}

#ifdef HAVE_LIBUNWIND_IA64

static int
get_unwind_table (unw_dyn_info_t *di, unw_addr_space_t as,
		  struct load_module *lm)
{
    ADDR table_addr, table_size, text_addr = 0, text_size = 0, gp;
    Elf64_Phdr *ph, *pdyn = NULL, *punw = NULL;
    ADDR dyn_addr, tag, addr, *p, min_text_addr, max_text_addr;
    extern DWORD *pmemLookup_p (ADDR);
    char *mem;

    for (ph = lm->phdr; ph < lm->phdr + lm->phnum; ++ph) {
	switch (ph->p_type) {
	      case PT_IA_64_UNWIND:	punw = ph; break;
	      case PT_DYNAMIC:		pdyn = ph; break;
	      default: break;
	}
    }
    if (!punw)
	return -1;

    table_addr = lm->load_base + punw->p_vaddr;
    table_size = punw->p_memsz;

    for (ph = lm->phdr; ph < lm->phdr + lm->phnum; ++ph) {
	if (ph->p_type == PT_LOAD) {
	    text_addr = ph->p_vaddr + lm->load_base;
	    text_size = ph->p_memsz;
	    if (table_addr >= text_addr && table_addr < text_addr + text_size)
		break;
	}
    }

    /*
     * The Linux kernel's unwind table covers multiple text segments:
     * the one for the main kernel and the one for the init text
     * section.
     */
    min_text_addr = ~(ADDR) 0;
    max_text_addr = 0;
    for (ph = lm->phdr; ph < lm->phdr + lm->phnum; ++ph) {
	if (ph->p_type == PT_LOAD && (ph->p_flags & PF_X)) {
	    if (ph->p_vaddr + lm->load_base < min_text_addr)
		min_text_addr = ph->p_vaddr + lm->load_base;
	    if (ph->p_vaddr + ph->p_memsz + lm->load_base > max_text_addr)
		max_text_addr = ph->p_vaddr + ph->p_memsz + lm->load_base;
	}
    }

    if (pdyn) {
	dyn_addr = pdyn->p_vaddr + lm->load_base;
	for (;
	     memMRd(dyn_addr, 8, &tag), tag != DT_NULL;
	     dyn_addr += 8)
	    if (tag == DT_PLTGOT) {
		/* On IA-64, _DYNAMIC is writable and GLIBC has
		   relocate it.  */
		memMRd(dyn_addr + 8, 8, &gp);
		break;
	    }
    } else
	/* there is a single global pointer for the entire program */
	gp = grGet(0, GP_ID);

    di->start_ip = min_text_addr;
    di->end_ip = max_text_addr;
    di->gp = gp;
    di->format = UNW_INFO_FORMAT_TABLE;
    di->u.ti.name_ptr = 0;
    di->u.ti.segbase = text_addr;
    di->u.ti.table_len = punw->p_memsz / sizeof (unw_word_t);

    table_addr = punw->p_vaddr + lm->load_base;

    mem = malloc(punw->p_memsz + sizeof (unw_word_t));
    if (!mem)
	return -UNW_ENOMEM;

    mem += sizeof(unw_word_t) - 1;
    mem = (char *) ((uintptr_t) mem & -sizeof (unw_word_t));

    addr = punw->p_vaddr + lm->load_base;
#ifdef UNWIND_DEBUG
    fprintf(stderr, "\tget_unwind_table: copying %llu bytes from %llx to %p\n",
	    punw->p_memsz, addr, mem);
#endif
    for (p = (ADDR *) mem; (char *) p < mem + punw->p_memsz; ++p) {
	memMRd(addr, 8, p);
	addr += 8;
    }
    di->u.ti.table_data = (unw_word_t *) mem;
    return 0;
}

static int
get_unwind_info (unw_dyn_info_t *di, unw_addr_space_t as, unw_word_t ip)
{
    ADDR text_addr, text_size;
    Elf64_Phdr *ph;
    struct load_module *lm;

    for (lm = load_module_list; lm; lm = lm->next) {
	for (ph = lm->phdr; ph < lm->phdr + lm->phnum; ++ph) {
	    if (ph->p_type == PT_LOAD) {
		text_addr = ph->p_vaddr + lm->load_base;
		text_size = ph->p_memsz;
		if (ip >= text_addr && ip < text_addr + text_size)
		    return get_unwind_table(di, as, lm);
	    }
	}
    }
    return -1;
}

static void
put_unwind_info (unw_addr_space_t as, unw_proc_info_t *pi, void *arg)
{
    if (!pi->unwind_info)
	return;
    free (pi->unwind_info);
    pi->unwind_info = NULL;
}

static int
get_dyn_info_list_addr (unw_addr_space_t as, unw_word_t *dil_addr, void *arg)
{
  struct load_module *lm;
  unw_dyn_info_t di;
  unw_word_t res;
  int count = 0;

  for (lm = load_module_list; lm; lm = lm->next) {
      if (get_unwind_table(&di, as, lm) >= 0) {
	  res = _Uia64_find_dyn_list(as, &di, arg);
	  if (res && ++count == 0)
	      *dil_addr = res;
	  free(di.u.ti.table_data);
      }
  }
  return (count > 0) ? 0 : -UNW_ENOINFO;
}

static int
find_proc_info (unw_addr_space_t as, unw_word_t ip, unw_proc_info_t *pi,
		int need_unwind_info, void *arg)
{
    unw_dyn_info_t di;
    int ret;

    if (get_unwind_info (&di, as, ip) < 0)
	return -UNW_ENOINFO;

#ifdef UNWIND_DEBUG
    fprintf(stderr, "find_proc_info()\n\tip=%llx, [0x%llx-0x%llx)\n"
	    "\tgp=0x%llx segbase=0x%llx\n"
	    "\ttable=%p, table_len=%llu\n",
	    (long long) ip,
	    (long long) di.start_ip, (long long) di.end_ip,
	    (long long) di.gp, (long long) di.u.ti.segbase,
	    di.u.ti.table_data, (long long) di.u.ti.table_len);
#endif

    ret = _Uia64_search_unwind_table (as, ip, &di, pi, need_unwind_info, arg);
#ifdef UNWIND_DEBUG
    fprintf (stderr, " %llx-%llx gp=%llx info=%p sz=%d ret=%d\n",
	     pi->start_ip, pi->end_ip, pi->gp, pi->unwind_info,
	     pi->unwind_info_size, ret);
#endif
    return ret;
}

static __inline__ uint64_t
ia64_rse_slot_num (uint64_t addr)
{
	return (addr >> 3) & 0x3f;
}

static __inline__ uint64_t
ia64_rse_num_regs (uint64_t bspstore, uint64_t bsp)
{
	uint64_t slots = (bsp - bspstore) >> 3;

	return slots - (ia64_rse_slot_num(bspstore) + slots)/0x40;
}

static __inline__ uint64_t
ia64_rse_skip_regs (uint64_t addr, long num_regs)
{
	long delta = ia64_rse_slot_num(addr) + num_regs;

	if (num_regs < 0)
		delta -= 0x3e;
	return addr + ((num_regs + delta/0x3f) << 3);
}

static REG rbs_overflow_area[MAX_NGRS + (MAX_NGRS / 63) + 1];

static inline int
wrap (int reg_num)
{
    if (reg_num < 0)
	return reg_num + N_STACK_PHYS;
    if (reg_num > (N_STACK_PHYS - 1))
	return reg_num - N_STACK_PHYS;
    return reg_num;
}

/*
 * Flush the dirty partition into rbs_overflow_area[].  We do this to
 * ensure that the unwinder has an easy way to get to these bits,
 * without affecting the visible machine state.  Moreover, if the
 * backing store pointer were invalid (e.g., pointing to an as-of-yet
 * unmapped page), then there would be no way to flush the dirty
 * partition at any rate.
 */
static void
unwind_flush (void)
{
    REG bspstore = BSPST;
    REG rnat = RNAT;
    REG *wp = rbs_overflow_area;
    unsigned int ncb;		/* NaT collection bit # */
    int i, regnum, d = dirty;

    for (i = 0; i < dirty + dirtyNat; ++i) {
	ncb = (bspstore >> 3) & 0x3f;
	if (ncb == 63) {
	    *wp++ = rnat;
	    rnat = 0;
	} else {
	    regnum = wrap((int) bol - d) + GR_STK_BASE;
	    *wp++ = grs[regnum].val;
	    if (grs[regnum].nat)
		rnat |= (REG)1 << ncb;
	    else
		rnat &= ~((REG)1 << ncb);
	    --d;
	}
	bspstore += 8;
    }
}

static int
access_mem (unw_addr_space_t as, unw_word_t addr, unw_word_t *val,
	    int write, void *arg)
{
    unw_word_t sof = cfmGet(0) & 0x7f;
    REG bsp = arGet(0, BSP_ID);
    REG bspstore = arGet(0, BSPST_ID);
    unw_word_t regnum = ia64_rse_num_regs(arGet(0, BSP_ID), addr);

    if (write)
	return -UNW_EINVAL;

    if (regnum < sof)
	*val = grGet(0, 32 + regnum);
    else if (addr >= bspstore && addr < bsp)
	*val = rbs_overflow_area[(addr - bspstore) / sizeof(REG)];
    else
	memMRd(addr, 8, (DWORD *) val);
#ifdef UNWIND_DEBUG
    fprintf(stderr, "  access_mem[%llx] -> 0x%016llx (bsp=%llx)\n",
	    addr, *val, arGet(0, BSP_ID));
#endif
    return 0;
}

static int
access_reg (unw_addr_space_t as, unw_regnum_t reg, unw_word_t *val, int write,
	    void *arg)
{
    unw_word_t ip, slot;

    if (write)
	return -UNW_EINVAL;

    switch (reg) {
	  case UNW_IA64_IP:
	    /* convert from Ski slot numbering convention (slot0=0,
	       slot1=4, slot2=8) to libunwind numbering convention
	       (slot0=0, slot1=1, slot2=2): */
	    ip = ipGet(0);
	    slot = (ip & 0xf)/4;
	    *val = (ip & ~ (unw_word_t) 0xf) + slot;
	    break;

	  case UNW_IA64_SP:
	    *val = grGet(0, SP_ID);
	    break;

	  case UNW_IA64_CFM:
	    *val = cfmGet(0);
	    break;

	  case UNW_IA64_AR + 0 ... UNW_IA64_AR + 127:
	    if (reg == UNW_IA64_AR_BSPSTORE) {
		unw_word_t sof = cfmGet(0) & 0x7f;
		*val = ia64_rse_skip_regs (arGet(0, BSP_ID), sof);
	    } else {
		*val = arGet(0, reg - UNW_IA64_AR);
	    }
	    break;

	  case UNW_IA64_PR:
	  {
	      unw_word_t pr = 0;
	      int i;

	      for (i = 63; i >= 0; i--)
		  pr = (pr << 1) | (prGet(0, i) & 0x1);
	      *val = pr;
	      break;
	  }

	  case UNW_IA64_GR + 0 ... UNW_IA64_GR + 31:
	    *val = grGet(0, reg - UNW_IA64_GR);
	    break;

	  case UNW_IA64_NAT + 0 ... UNW_IA64_NAT + 31:
	    *val = grNatGet(0, reg - UNW_IA64_NAT);
	    break;

	  case UNW_IA64_BR + 0 ... UNW_IA64_BR + 7:
	    *val = brGet(0, reg - UNW_IA64_BR);
	    break;

	  default:
	    return -UNW_EBADREG;
    }
#ifdef UNWIND_DEBUG
    fprintf(stderr, "  access_reg(%s)\t-> 0x%016llx\n",
	    unw_regname (reg), *val);
#endif
    return 0;
}

static int
access_fpreg (unw_addr_space_t as, unw_regnum_t reg, unw_fpreg_t *val,
	      int write, void *arg)
{
    WORD sign, exp;

    if (write)
	return -UNW_EINVAL;

    sign = frSignGet(0, reg - UNW_IA64_FR);
    exp = frExpGet(0, reg - UNW_IA64_FR);
    val->raw.bits[0]= frMantGet(0, reg - UNW_IA64_FR);
    val->raw.bits[1] = sign << 17 | exp;
    return 0;
}

BOOL
cstkDisplay (unsigned argc, char *argv[])
{
    FILE *f = NULL;
    static char cstklist[CSTKSZ * 80];
    static int initialized = 0;
    static unw_addr_space_t addr_space;
    char buf[40], *p = cstklist;
    unw_cursor_t uc;
    unw_accessors_t acc;
    unsigned level = 0;
    int result;

    if (initialized <= 0) {
	if (initialized < 0)
	    return NO;

	initialized = 1;

	acc.find_proc_info = find_proc_info;
	acc.put_unwind_info = put_unwind_info;
	acc.get_dyn_info_list_addr = get_dyn_info_list_addr;
	acc.access_mem = access_mem;
	acc.access_reg = access_reg;
	acc.access_fpreg = access_fpreg;
	acc.resume = 0;
	addr_space = unw_create_addr_space(&acc, 0);
	if (!addr_space) {
	    cmdErr("Failed to initialize unwinder (unw_create_addr_space()\n");
	    initialized = -1;
	    return NO;
	}
    }

    result = unw_init_remote(&uc, addr_space, NULL);
    if (result < 0) {
	if (result == -UNW_ENOINFO)
	    cmdErr("No unwind info for current IP.\n");
	else
	    cmdErr("unw_init_remote() failed with error %d\n", result);
	return NO;
    }

    unwind_flush();	/* flush RSE to ensure backing store is up-to-date */

    do {
	unw_word_t unw_ip;

	if (unw_get_reg(&uc, UNW_REG_IP, &unw_ip) < 0)
	    return NO;

	if ((unw_ip & 0xF) == 1)
	    unw_ip += 3;
	if ((unw_ip & 0xF) == 2)
	    unw_ip += 6;
	symAddrtoName(unw_ip, buf, 4, NO, -35);
	p += sprintf(p, "%2u) %s\n", level++, buf);
	if (level == CSTKSZ)
	    break;
	result = unw_step(&uc);
	if (result < 0) {
	    p += sprintf(p, "Error %d while unwinding\n", result);
	    break;
	}
    } while (result > 0);

    if (argc == 1 && !(f = cmdOpenFile(argv[0], "w")))
	return NO;
    cmdOut("callstack", "Call Stack:\n", cstklist, f);
    return YES;
}

#else /* !HAVE_LIBUNWIND_IA64 */

#ifdef HAVE_UNWIND_SUPPORT
static void *read_tgt_mem(void *dst, const uint64_t src, size_t nbytes,
			  uint32_t ident)
{
    BYTE *buf = (BYTE *)dst;
    DWORD val;
    ADDR adr = src;
    size_t i;

#ifdef UNWIND_DEBUG
fprintf(stderr, "read_tgt_mem called: %llx %x\n", src, (unsigned)nbytes);
#endif
    for (i = 0; i < nbytes; i++) {
	if (!memMRd(adr, 1, &val))
	    break;
	adr++;
	*buf++ = val;
    }
    return dst;
}
#endif /* !HAVE_UNWIND_SUPPORT */

#define ADDPTR(addr)    (addr = (BitfX(addr,32,32) | BitfX(addr,32,2) << 61))

void addLM2(ADDR adr, size_t len)
{
#ifdef HAVE_UNWIND_SUPPORT
    struct load_module_desc lmd;

    if (len > sizeof lmd)
	/*progExit("Bad size for load_module_desc\n");*/
	return;
    if (!getLp64(0))
	ADDPTR(adr);
    (void)read_tgt_mem(&lmd, adr, len, 0);
    lminfo[numLms].text_base = lmd.text_base;
    if (!getLp64(0))
	ADDPTR(lminfo[numLms].text_base);
    lminfo[numLms].text_end = lminfo[numLms].text_base + lmd.text_size - 1;
#if 0
    lminfo[numLms].unwind_base = lmd.unwind_base;
#else
    if (lmd.unwind_base)
	lminfo[numLms].unwind_base = lmd.unwind_base;
    else
	lminfo[numLms].unwind_base = lmd.text_base+0x1f88;
#endif
    if (!getLp64(0))
	ADDPTR(lminfo[numLms].unwind_base);
    lminfo[numLms].name = "<unknown>";
    numLms++;
#endif /* HAVE_UNWIND_SUPPORT */
}

#ifdef HAVE_UNWIND_SUPPORT
static void load_map_from_ip(void * const new_load_map, uint64_t adr)
{
    struct load_module_desc *lmd = new_load_map;

    /* XXX - How much needs to be set? */
    /* XXX - Keep a cache of these and memcpy the proper one? */
#ifdef UNWIND_DEBUG
fprintf(stderr, "load_map_from_ip called\n");
#endif
    lmd->text_base = 0ULL;
    lmd->text_size = 0;
    lmd->data_base = 0ULL;
    lmd->data_size = 0;
    lmd->unwind_base = 0ULL;
    lmd->linkage_ptr = 0ULL;
    lmd->phdr_base = 0ULL;
    {
	unsigned i;

	for (i = 0; i < MAX_LOAD_MODULES; i++)
	    if (adr >= lminfo[i].text_base && adr <= lminfo[i].text_end) {
		lmd->text_base = lminfo[i].text_base;
		lmd->unwind_base = lminfo[i].unwind_base;
		break;
	    }
    }
}
#endif /* HAVE_UNWIND_SUPPORT */

BOOL cstkDisplay(unsigned argc, char *argv[])
{
#ifdef HAVE_UNWIND_SUPPORT
    FILE *f = NULL;
    static char cstklist[CSTKSZ * 80];
    char buf[40], *p = cstklist;
    enum _Unwind_StepResult result;
    _Unwind *uc;
    unsigned level = 0, i;
    REG psrval = psrGet(0), curr_fm = cfmGet(0),
	curr_ip = ipGet(0);

    /* XXX - should call this only once? */
#ifdef UNWIND_DEBUG
#if 0
    uc = _Unwind_ctor3(UNWIND_FLAG_TRACE_CALLS|UNWIND_FLAG_DUMP_CONTEXT,
		read_tgt_mem, 0, load_map_from_ip);
#else
    uc = _Unwind_ctor3(pmdGet(0, 0),
		read_tgt_mem, 0, load_map_from_ip);
#endif
#else
    uc = _Unwind_ctor3(0, read_tgt_mem, 0, load_map_from_ip);
#endif

    /* Flush RSE - XXX - why needed? */
    unwindFlush();

    curr_ip -= 3 * ((curr_ip & 0xF) >> 2);
    _Unwind_SetIP(uc, curr_ip);
    _Unwind_SetGR(uc, _UW_GR_SP, grGet(0, SP_ID));
    _Unwind_SetAR(uc, _UW_AR_PFS, arGet(0, PFS_ID));
    _Unwind_SetAR(uc, _UW_AR_RNAT, arGet(0, RNAT_ID));
    _Unwind_SetAR(uc, _UW_AR_BSP, arGet(0, BSP_ID));
    _Unwind_SetAR(uc, _UW_AR_BSPSTORE, arGet(0, BSPST_ID));
    _Unwind_SetAR(uc, _UW_AR_RSC, arGet(0, RSC_ID));
    _Unwind_SetBR(uc, _UW_BR_RP, brGet(0, RP_ID));
    _Unwind_SetUM(uc, X_PSR_UM(psrval));
    _Unwind_SetCFM(uc, curr_fm);
    for (i = GR_STK_BASE; i < GR_STK_BASE+XFM_SOF(curr_fm); i++)
	_Unwind_SetGR(uc, i, grGet(0, i));
    /* maybe repeat above for the rest of the static GRs, and maybe the
       FRs, BRs, PRs, & ARs
     */

    do {
	ADDR unw_ip = _Unwind_GetIP(uc);

	if ((unw_ip & 0xF) == 1)
	    unw_ip += 3;
	if ((unw_ip & 0xF) == 2)
	    unw_ip += 6;
	symAddrtoName(unw_ip, buf, 4, NO, -35);
	p += sprintf(p, "%2u) %s\n", level++, buf);
	if (level == CSTKSZ)
	    break;
	if ((result = _Unwind_step(uc)) > _UW_STEP_OK) {
	    p += sprintf(p, "Error %d while unwinding\n", result);
	    break;
	}
    } while (result != _UW_STEP_BOTTOM);

    _Unwind_dtor(uc);

    if (argc == 1 && !(f = cmdOpenFile(argv[0], "w")))
	return NO;
    cmdOut("callstack", "Call Stack:\n", cstklist, f);
#endif /* HAVE_UNWIND_SUPPORT */
    return YES;
}

#endif /* !HAVE_LIBUNWIND_IA64 */

BOOL showLoadModules(unsigned argc, char *argv[])
{
    size_t bufsz = 8192;
    FILE *f = NULL;
    char *buf, *p;
    unsigned i;

    if (!numLms) {
	cmdWarn("No load modules\n");
	return YES;
    }
    if (!(buf = malloc(bufsz+1))) {
	cmdErr("Could not malloc space for load module list\n");
	return NO;
    }
    if (argc == 1 && !(f = cmdOpenFile(argv[0], "w"))) {
	free(buf);
	return NO;
    }
    p = buf;
    for (i = 0; i < numLms; i++)
	p += sprintf(p, "%2u %016llx %8llx    %s\n",
	      i, lminfo[i].text_base, lminfo[i].text_end-lminfo[i].text_base+1,
	      lminfo[i].name);
    cmdOut("loadmodules", " #    Text Base     Text Size   Name\n", buf, f);
    free(buf);
    return YES;
}

