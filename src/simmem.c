/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Memory Routines
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

#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include "std.h"
#include "bits.h"
#include "types.h"
#include "fields.h"
#include "state.h"
#include "sim.h"
#include "simmem.h"
#include "tlb.h"
#include "libcore.h"
#include "libtrace.h"
#include "trace.h"
#include "interruption.h"
#include "platform.h"
#include "ia_types.h"
#include "ia_state.h"
#include "ia_exec.h"
#include "ssDBT.h"
#include "syscall_api.h"

extern BOOL use_alat;
void alat_inval_multiple_entries(ADDR pa, unsigned size);

/*
 * FreeBSD uses the traditional MAP_ANON and does not have MAP_ANONYMOUS.
 * HP-UX has MAP_ANONYMOUS, but not MAP_ANON and Linux has both, but has
 * deprecated MAP_ANON. Use MAP_ANONYMOUS as it has the largest following
 * and deal with the cases when there's a MAP_ANON but no MAP_ANONYMOUS
 * below.
 */
#if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#define	MAP_ANONYMOUS	MAP_ANON
#endif

#define PMEMHSHSZ	(1024*1024)
#define PmemHash(a)	(((DWORD)(a) >> log2_page_size) & (PMEMHSHSZ-1))

#define allocOK(adr)	(autoAlloc || \
			 (SP-0x1000ULL <= adr && adr < max_sp) || \
			 (BSPST <= adr && adr < BSPST+0x1000ULL))

#define PIBSZ		0x200000		/* 2 MBytes */
#define INTA_OFS	0x1E0000
#define XTP_OFS		0x1E0008
/* XXX - since ibBase rarely if ever changes, perhaps we should pre-compute
 *       the comparison addresses (e.g., ibBase+XTP_OFS)
 */
/* ipiAccess used by memWrt8 and memMWrt8 */
#define ipiAccess(adr)	((adr) >= ibBase && (adr) < (ibBase + PIBSZ/2))
/* xtpAccess used by memWrt1 and memMWrt1 but currently commented out */
#define xtpAccess(adr)	((adr) == ibBase + XTP_OFS)
/* intaAccess not currently used */
#define intaAccess(adr)	((adr) == ibBase + INTA_OFS)

extern int numBptsToLoad;

unsigned long page_size;
unsigned int log2_page_size;
DWORD page_mask;		/* Why DWORD? Why not u_long? */

/*##################### Local Variables ####################################*/

typedef struct pmemStruct {
    ADDR padr;			/* real byte address of this page */
    struct pmemStruct *next;	/* pointer to next page in hash chain */
    char *pmem;			/* ptr to the actual page of memory */
    unsigned int flags;
#define	PMEM_FL_BREAKPOINT	1	/* Breakpoints exist in this page. */
} pmemStruct;

static pmemStruct *pmemHshTbl[PMEMHSHSZ] = {NULL};    /* pmem hash table */

static BOOL unallocMRd(ADDR adr, int size, DWORD *pval);
static void unallocPageRd(ADDR adr, int size, DWORD *pval);
static void unallocPageWrt1(ADDR adr, BYTE val);
static void unallocPageWrt2(ADDR adr, HWORD val, BOOL be);
static void unallocPageWrt4(ADDR adr, WORD val, BOOL be);
static void unallocPageWrt8(ADDR adr, DWORD val, BOOL be);
static void unallocPageWrt16(ADDR adr, DWORD val1, DWORD val2, BOOL be);

BOOL autoAlloc = NO;

/*##################### Functions and Subroutines ##########################*/

/*
 * Single-page mappings will yield a large amount of small VM objects that
 * may or may not be collided into larger VM object by the kernel. Having
 * a large number of small objects will generally cause a significant
 * slowdown in performance.
 * The pmem_new_page() function is therefore responsible to allocate a page
 * in the process address space (the address space of ski) in whatever way
 * is most efficient.
 * XXX the current implementation is non-optimized.
 */
inline static void *
pmem_new_page(void)
{
    char *pa;

    pa = mmap(0, page_size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE,
	-1, 0);
    if ((long)pa == -1)
	pa = 0;
    return (pa);
}

/*
 * This function deals with multi-process and multi-processor support.
 * Multi-process support has a hashtable per application, while multi-
 * processor support has a single hashtable shared between the CPUs.
 * In multi-process scenarios simulated addresses (called virtual
 * addresses in this module) are typically process virtual addresses.
 * In multi-processor scenarios simulated addresses are typically
 * simulated physical addresses.
 * XXX not implemented yet.
 */
inline static pmemStruct **
pmem_get_hashtable(void)
{
    return (pmemHshTbl);
}

/*
 * Given a virtual (=simulated) address and a physical (=process virtual)
 * address, insert the mapping into the hash table. All addresses must be
 * page aligned.
 */
inline static pmemStruct *
pmem_insert(ADDR va, void *pa)
{
    pmemStruct *ps, **tbl;
    unsigned int bucket;

    assert(page_offset(va) == 0);

    ps = malloc(sizeof *ps);
    if (ps == NULL)
	abort();
    ps->padr = va;
    ps->pmem = pa;
    ps->flags = 0;

    tbl = pmem_get_hashtable();
    bucket = PmemHash(va);

    /* XXX lock [tbl,bucket] */
    ps->next = tbl[bucket];
    tbl[bucket] = ps;
    /* XXX unlock [tbl,bucket] */

    return (ps);
}

inline static pmemStruct *
pmem_create(ADDR va)
{
    struct pmemStruct *ps;
    void *pa;

    pa = pmem_new_page();
    if (pa == NULL)
	abort();
    ps = pmem_insert(va, pa);

    return (ps);
}

inline static pmemStruct *
pmem_lookup(ADDR va)
{
    pmemStruct *ps, **tbl;
    unsigned int bucket;
    ADDR pg;

    pg = page_base(va);
    tbl = pmem_get_hashtable();
    bucket = PmemHash(pg);

    /* XXX lock [tbl,bucket] */
    ps = tbl[bucket];
    while (ps != NULL && ps->padr != pg)
	ps = ps->next;
    /* XXX unlock [tbl,bucket] */
    return (ps);
}

inline static pmemStruct *
pmem_lookup_create(ADDR va)
{
    pmemStruct *ps;

    ps = pmem_lookup(va);
    if (ps == NULL)
	ps = pmem_create(page_base(va));
    assert(ps != NULL);
    return (ps);
}

inline static int
pmem_update_flags(ADDR va, unsigned int set, unsigned int clear)
{
    pmemStruct *ps;

    assert((set & clear) == 0);

    ps = pmem_lookup(va);
    if (ps == NULL)
	return (0);
    ps->flags = (ps->flags | set) & ~clear;
    return (1);
}

/*
 * Compatibility cruft. Needs reviewing and either removed or formalized.
 */

#define	pmemLookup_q(va)	((DWORD*)pmemLookupX(va, 0xf, YES))
#define	pmemMLookup_q(va)	((DWORD*)pmemLookupX(va, 0xf, NO))
#define	pmemLookup_d(va)	((DWORD*)pmemLookupX(va, 0x7, YES))
#define	pmemMLookup_d(va)	((DWORD*)pmemLookupX(va, 0x7, NO))
#define	pmemLookup_w(va)	((WORD*)pmemLookupX(va, 0x3, YES))
#define	pmemMLookup_w(va)	((WORD*)pmemLookupX(va, 0x3, NO))
#define	pmemLookup_h(va)	((HWORD*)pmemLookupX(va, 0x1, YES))
#define	pmemMLookup_h(va)	((HWORD*)pmemLookupX(va, 0x1, NO))
#define	pmemLookup_b(va)	((BYTE*)pmemLookupX(va, 0x0, YES))
#define	pmemMLookup_b(va)	((BYTE*)pmemLookupX(va, 0x0, NO))

static inline void *
pmemLookupX(ADDR va, unsigned int mask, int chkbp)
{
    pmemStruct *ps;

    /*
     * XXX Misaligned addresses always have to fail the lookup. Why you
     * then don't check alignment before you do the lookup is beyond me.
     * In short: we shouldn't have to deal with this.
     */
    if ((va & mask) != 0)
	return NULL;

    ps = pmem_lookup(va);
    if (ps == NULL)
	return (NULL);

    /*
     * XXX handling of watchpoints is severely broken. You don't wanna know.
     * Well, maybe you do, but I don't wanna explain it :-)
     */
    if (chkbp && (ps->flags & PMEM_FL_BREAKPOINT))
	return (NULL);

    return (void*)(ps->pmem + page_offset(va));
}

DWORD *
pmemLookup(ADDR va)
{
    pmemStruct *ps;

    ps = pmem_lookup(va);
    if (ps == NULL)
	return (NULL);
    return (DWORD*)(ps->pmem + page_offset(va));
}

DWORD *
pmemLookup_p(ADDR va)
{
    pmemStruct *ps;

    ps = pmem_lookup_create(va);
    return (DWORD*)(ps->pmem + page_offset(va));
}

BOOL
pmemMap(ADDR va, void *pa)
{
    pmem_insert(va, pa);
    return YES;
}

void
memSetDbptBit(ADDR va)
{
    pmem_update_flags(va, PMEM_FL_BREAKPOINT, 0);
}

void
memClrDbptBit(ADDR va)
{
    pmem_update_flags(va, 0, PMEM_FL_BREAKPOINT);
}

/* XXX - Global problem: the ALAT is supposed to store and lookup based on
   physical addresses, not virtual as is done here.  Easiest fix is probably
   to pass physical address back from mem functions, as is done in the SDM
*/

/*******************************/
/* Simulated data memory reads */
/*******************************/

inline static long
segfault (ADDR addr)
{
#ifdef __linux__
    if (unixABI) {
	signal_queue_info (11, ip);
    } else
#endif
    progExit("Page at address 0x%016llx not allocated\n", addr);
    return 0;
}



BOOL memLPF(ADDR adr, Accesstype atype)
{
    DWORD d;

    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_LFETCH, DOFF_TRC_1_BYTE);
    if (atype == LFETCH_ACCESS)
	return YES;
    if (!abi && dtlbLookup(adr, 1, LFETCH_ACCESS, PSR_CPL, PSR_DT, &adr) == -1)
	return NO;
    if (!pmemMLookup_b(adr)) {
	unallocPageRd(adr, 1, &d);
    }
    return YES;
}

/* XXX - make return type an enum */
/* return 0 for success, 1 for defer, -1 for fault */
int memRd1(ADDR adr, Accesstype atype, BYTE *pval)
{
    BYTE *pb;
    int ret = 0;

    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_READ, DOFF_TRC_1_BYTE);
    if (!abi && (ret = dtlbLookup(adr, 1, atype, PSR_CPL, PSR_DT, &adr)))
	return ret;
    if ((pb = pmemLookup_b(adr)))
	*pval = *pb;
    else if (dbptCheck(adr, 1, 1))
	progStop("Data breakpoint encountered\n");
    else if ((pb = pmemMLookup_b(adr)))
	*pval = *pb;
    else {
	DWORD d;

	unallocPageRd(adr, 1, &d);
	*pval = d;
    }
    return 0;
}

int memRd2(ADDR adr, Accesstype atype, HWORD *pval)
{
    HWORD *ph;
    int ret = 0;

    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_READ, DOFF_TRC_2_BYTES);
    if (!abi && (ret = dtlbLookup(adr, 2, atype, PSR_CPL, PSR_DT, &adr)))
	return ret;
    if ((ph = pmemLookup_h(adr)))
	*pval = *ph;
    else if (dbptCheck(adr, 1, 2))
	progStop("Data breakpoint encountered\n");
    else if (adr & 0x1) {
	unalignedDataFault(MEM_LD_ACCESS);
	return -1;			/* never reached? */
    } else if ((ph = pmemMLookup_h(adr))) {
	*pval = *ph;
    } else {
	DWORD d;

	unallocPageRd(adr, 2, &d);
	*pval = d;
    }
    *pval = FIX_BYTEORDER_H(PSR_BE, *pval);
    return 0;
}

int memRd4(ADDR adr, Accesstype atype, WORD *pval)
{
    WORD *pw;
    int ret = 0;

    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_READ, DOFF_TRC_4_BYTES);
    if (!abi && (ret = dtlbLookup(adr, 4, atype, PSR_CPL, PSR_DT, &adr)))
	return ret;
    if ((pw = pmemLookup_w(adr)))
	*pval = *pw;
    else if (dbptCheck(adr, 1, 4))
	progStop("Data breakpoint encountered\n");
    else if (adr & 0x3) {
	unalignedDataFault(MEM_LD_ACCESS);
	return -1;			/* never reached? */
    } else if ((pw = pmemMLookup_w(adr))) {
	*pval = *pw;
    } else {
	DWORD d;

	unallocPageRd(adr, 4, &d);
	*pval = d;
    }
    *pval = FIX_BYTEORDER_W(PSR_BE, *pval);
    return 0;
}

int memRd8(ADDR adr, Accesstype atype, DWORD *pval)
{
    DWORD *pd;
    int ret = 0;

    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_READ, DOFF_TRC_8_BYTES);
    if (!abi && (ret = dtlbLookup(adr, 8, atype, PSR_CPL, PSR_DT, &adr)))
	return ret;
    if ((pd = pmemLookup_d(adr)))
	*pval = *pd;
    else if (dbptCheck(adr, 1, 8))
	progStop("Data breakpoint encountered\n");
    else if (adr & 0x7) {
	unalignedDataFault(MEM_LD_ACCESS);
	return -1;			/* never reached? */
    } else if ((pd = pmemMLookup_d(adr))) {
	*pval = *pd;
    } else {
	unallocPageRd(adr, 8, pval);
    }
    *pval = FIX_BYTEORDER_D(PSR_BE, *pval);
    return 0;
}

int memRd16(ADDR adr, Accesstype atype, QWORD *pval)
{
    DWORD *pd;
    int ret = 0;

    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_READ, DOFF_TRC_16_BYTES);
    if (!abi && (ret = dtlbLookup(adr, 16, atype, PSR_CPL, PSR_DT, &adr)))
	return ret;
    if ((pd = pmemLookup_q(adr))) {
	pval->left  = *pd;
	pval->right = *(pd+1);
    } else if (dbptCheck(adr, 1, 16))
	progStop("Data breakpoint encountered\n");
    else if (adr & 0xF) {
	unalignedDataFault(MEM_LD_ACCESS);
	return -1;			/* never reached? */
    } else if ((pd = pmemMLookup_q(adr))) {
	pval->left  = *pd;
	pval->right = *(pd+1);
    } else {
	unallocPageRd(adr, 16, (DWORD *)pval);
    }
    FIX_BYTEORDER_Q(PSR_BE, pval->left, pval->right, pval->left, pval->right);
    return 0;
}

BOOL rseRd(ADDR adr, DWORD *pval)
{
    DWORD *pd;

    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_RSEREAD, DOFF_TRC_8_BYTES);
    if (!abi && dtlbLookup(adr, 8, RSE_LD_ACCESS, RSC_PL, PSR_RT, &adr) == -1)
	return NO;
    if ((pd = pmemLookup_d(adr)))
	*pval = *pd;
    else if (dbptCheck(adr, 1, 16))
	progStop("Data breakpoint encountered\n");
    else if ((pd = pmemMLookup_d(adr)))
	*pval = *pd;
    else {
	unallocPageRd(adr, 8, pval);
    }
    *pval = FIX_BYTEORDER_D(RSC_BE, *pval);
    return YES;
}

#define PTA_VF		BitfR(PTA,55,1)
#define BAD_TPA	5
BOOL vhptRd(ADDR adr, DWORD *ptpa, DWORD *ptar, DWORD *ptag)
{
    int ret, long_format = (PTA_VF != 0);
    DWORD *pd;

    if ((ret = vhptLookup(adr, &adr)) == 0)
	return NO;
    if (ret == -1) {
	*ptpa = BAD_TPA;	/* force reservedInsert() to fail */
	return YES;
    }
    if ((pd = pmemLookup_d(adr))) {
	*ptpa = pd[0];
	if (long_format) {
	    *ptar = pd[1];
	    *ptag = pd[2];
	}
    } else if (dbptCheck(adr, 1, 32)) {
	progStop("Data breakpoint encountered\n");
    } else if ((pd = pmemMLookup_d(adr))) {
	*ptpa = pd[0];
	if (long_format) {
	    *ptar = pd[1];
	    *ptag = pd[2];
	}
    } else {	/* XXX - Is this correct? */
	unallocPageRd(adr, 8, ptpa);
	unallocPageRd(adr, 8, ptar);
	unallocPageRd(adr, 8, ptag);
    }
    *ptpa = FIX_BYTEORDER_D(DCR_BE, *ptpa);
    if (long_format) {
	*ptar = FIX_BYTEORDER_D(DCR_BE, *ptar);
	*ptag = FIX_BYTEORDER_D(DCR_BE, *ptag);
    }
    return YES;
}

/* XXX - make return type an enum */
/* return 0 for success, 1 for NaT, -1 for fault */
int specRd1(ADDR adr, Accesstype atype, BYTE *pval)
{
    BYTE *pb;
    int ret = 0;

    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_READ, DOFF_TRC_1_BYTE);
    if (!abi && (ret = specLookup(adr, 1, SPEC_LD_ACCESS, &adr)) == -1)
	return ret;
    if ((pb = pmemLookup_b(adr)))
	*pval = *pb;
    else if (!ret && dbptCheck(adr, 1, 1))
	progStop("Data breakpoint encountered\n");
    else if ((pb = pmemMLookup_b(adr)))
	*pval = *pb;
    else if (abi)
	return 1;
    else if (!ret) {
	DWORD d;

	unallocPageRd(adr, 1, &d);
	*pval = d;
#if 0
	progExit("Speculative read to unallocated address 0x%016llx\n", adr);
		/* Call unallocPageRd()? */
#endif
    }
    return ret;
}

int specRd2(ADDR adr, Accesstype atype, HWORD *pval)
{
    HWORD *ph;
    int ret = 0;

    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_READ, DOFF_TRC_2_BYTES);
    if (!abi && (ret = specLookup(adr, 2, SPEC_LD_ACCESS, &adr)) == -1)
	return ret;
    if ((ph = pmemLookup_h(adr)))
	*pval = *ph;
    else if (!ret && dbptCheck(adr, 1, 2))
	progStop("Data breakpoint encountered\n");
    else if (adr & 0x1) {
	if (!PSR_IC || ITLB_ED)
	    return 1;
	unalignedDataFault(SPEC_LD_ACCESS);
	return -1;
    } else if ((ph = pmemMLookup_h(adr))) {
	*pval = *ph;
    } else if (abi)
	return 1;
    else if (!ret) {
	DWORD d;

	unallocPageRd(adr, 2, &d);
	*pval = d;
#if 0
	progExit("Speculative read to unallocated address 0x%016llx\n", adr);
		/* Call unallocPageRd()? */
#endif
    }
    *pval = FIX_BYTEORDER_H(PSR_BE, *pval);
    return ret;
}

int specRd4(ADDR adr, Accesstype atype, WORD *pval)
{
    WORD *pw;
    int ret = 0;

    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_READ, DOFF_TRC_4_BYTES);
    if (!abi && (ret = specLookup(adr, 4, SPEC_LD_ACCESS, &adr)) == -1)
	return ret;
    if ((pw = pmemLookup_w(adr)))
	*pval = *pw;
    else if (!ret && dbptCheck(adr, 1, 4))
	progStop("Data breakpoint encountered\n");
    else if (adr & 0x3) {
	if (!PSR_IC || ITLB_ED)
	    return 1;
	unalignedDataFault(SPEC_LD_ACCESS);
	return -1;
    } else if ((pw = pmemMLookup_w(adr))) {
	*pval = *pw;
    } else if (abi)
	return 1;
    else if (!ret) {
	DWORD d;

	unallocPageRd(adr, 4, &d);
	*pval = d;
#if 0
	progExit("Speculative read to unallocated address 0x%016llx\n", adr);
		/* Call unallocPageRd()? */
#endif
    }
    *pval = FIX_BYTEORDER_W(PSR_BE, *pval);
    return ret;
}

int specRd8(ADDR adr, Accesstype atype, DWORD *pval)
{
    DWORD *pd;
    int ret = 0;

    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_READ, DOFF_TRC_8_BYTES);
    if (!abi && (ret = specLookup(adr, 8, SPEC_LD_ACCESS, &adr)) == -1)
	return ret;
    if ((pd = pmemLookup_d(adr)))
	*pval = *pd;
    else if (!ret && dbptCheck(adr, 1, 8))
	progStop("Data breakpoint encountered\n");
    else if (adr & 0x7) {
	if (!PSR_IC || ITLB_ED)
	    return 1;
	unalignedDataFault(SPEC_LD_ACCESS);
	return -1;
    } else if ((pd = pmemMLookup_d(adr))) {
	*pval = *pd;
    } else if (abi)
	return 1;
    else if (!ret) {
	unallocPageRd(adr, 8, pval);
#if 0
	progExit("Speculative read to unallocated address 0x%016llx\n", adr);
		/* Call unallocPageRd()? */
#endif
    }
	*pval = FIX_BYTEORDER_D(PSR_BE, *pval);
    return ret;
}

int specRd16(ADDR adr, Accesstype atype, QWORD *pval)
{
    DWORD *pd;
    int ret = 0;

    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_READ, DOFF_TRC_16_BYTES);
    if (!abi && (ret = specLookup(adr, 16, SPEC_LD_ACCESS, &adr)) == -1)
	return ret;
    if ((pd = pmemLookup_q(adr))) {
	pval->left  = *pd;
	pval->right = *(pd+1);
    } else if (!ret && dbptCheck(adr, 1, 16))
	progStop("Data breakpoint encountered\n");
    else if (adr & 0xF) {
	if (!PSR_IC || ITLB_ED)
	    return 1;
	unalignedDataFault(SPEC_LD_ACCESS);
	return -1;
    } else if ((pd = pmemMLookup_q(adr))) {
	pval->left  = *pd;
	pval->right = *(pd+1);
    } else if (abi)
	return 1;
    else if (!ret) {
	unallocPageRd(adr, 16, (DWORD *)pval);
#if 0
	progExit("Speculative read to unallocated address 0x%016llx\n", adr);
		/* Call unallocPageRd()? */
#endif
    }
    FIX_BYTEORDER_Q(PSR_BE, pval->left, pval->right, pval->left, pval->right);
    return ret;
}


/********************************/
/* Simulated data memory writes */
/********************************/

BOOL memWrt1(ADDR adr, BYTE val)
{
    BYTE *pb;

    if (use_alat)
	alat_inval_multiple_entries(adr, 1);
    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_WRITE, DOFF_TRC_1_BYTE);
    if (!abi && dtlbLookup(adr, 1, MEM_ST_ACCESS, PSR_CPL, PSR_DT, &adr) == -1)
	return NO;
    if (!abi && xtpAccess(adr))
	xtpCycle(val);
    if ((pb = pmemLookup_b(adr)))
	*pb = val;
    else if (dbptCheck(adr, 2, 1))
	progStop("Data breakpoint encountered\n");
    else if ((pb = pmemMLookup_b(adr)))
	*pb = val;
    else
	unallocPageWrt1(adr, val);
    return YES;
}

BOOL memWrt2(ADDR adr, HWORD val)
{
    HWORD *ph;

    if (use_alat)
	alat_inval_multiple_entries(adr, 2);
    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_WRITE, DOFF_TRC_2_BYTES);
    if (!abi && dtlbLookup(adr, 2, MEM_ST_ACCESS, PSR_CPL, PSR_DT, &adr) == -1)
	return NO;
    if ((ph = pmemLookup_h(adr)))
	*ph = FIX_BYTEORDER_H(PSR_BE, val);
    else if (dbptCheck(adr, 2, 2))
	progStop("Data breakpoint encountered\n");
    else if (adr & 0x1) {
	unalignedDataFault(MEM_ST_ACCESS);
	return NO;			/* never reached? */
    } else if ((ph = pmemMLookup_h(adr))) {
	*ph = FIX_BYTEORDER_H(PSR_BE, val);
    } else
	unallocPageWrt2(adr, val, PSR_BE);
    return YES;
}

BOOL memWrt4(ADDR adr, WORD val)
{
    WORD *pw;

    if (use_alat)
	alat_inval_multiple_entries(adr, 4);
    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_WRITE, DOFF_TRC_4_BYTES);
    if (!abi && dtlbLookup(adr, 4, MEM_ST_ACCESS, PSR_CPL, PSR_DT, &adr) == -1)
	return NO;
    if ((pw = pmemLookup_w(adr)))
	*pw = FIX_BYTEORDER_W(PSR_BE, val);
    else if (dbptCheck(adr, 2, 4))
	progStop("Data breakpoint encountered\n");
    else if (adr & 0x3) {
	unalignedDataFault(MEM_ST_ACCESS);
	return NO;			/* never reached? */
    } else if ((pw = pmemMLookup_w(adr))) {
	*pw = FIX_BYTEORDER_W(PSR_BE, val);
    } else
	unallocPageWrt4(adr, val, PSR_BE);
    return YES;
}

BOOL memWrt8(ADDR adr, DWORD val)
{
    DWORD *pd;

    if (use_alat)
	alat_inval_multiple_entries(adr, 8);

    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_WRITE, DOFF_TRC_8_BYTES);

    if (!abi) {
	if (dtlbLookup(adr, 8, MEM_ST_ACCESS, PSR_CPL, PSR_DT, &adr) == -1)
	    return NO;

#ifdef NEW_MP
	if (ipiAccess(adr)) {
	    sendIPI((adr >> 12) & 0xff, (adr >> 4) & 0xff, val & 0xff);
	    return YES;
	}
#endif
    }

    if ((pd = pmemLookup_d(adr)))
	*pd = FIX_BYTEORDER_D(PSR_BE, val);
    else if (dbptCheck(adr, 2, 8))
	progStop("Data breakpoint encountered\n");
    else if (adr & 0x7) {
	unalignedDataFault(MEM_ST_ACCESS);
	return NO;			/* never reached? */
    } else if ((pd = pmemMLookup_d(adr))) {
	*pd = FIX_BYTEORDER_D(PSR_BE, val);
    } else
	unallocPageWrt8(adr, val, PSR_BE);
    return YES;
}

BOOL memWrt10(ADDR adr, DWORD val1, DWORD val2)
{
    DWORD *pd;

    if (use_alat)
	alat_inval_multiple_entries(adr, 10);
    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_WRITE, DOFF_TRC_10_BYTES);
    if (!abi && dtlbLookup(adr, 16, MEM_ST_ACCESS, PSR_CPL, PSR_DT, &adr) == -1)
	return NO;
    if ((pd = pmemLookup_q(adr))) {
	HWORD *ph = (HWORD*)(pd + 1);

#if BYTE_ORDER == BIG_ENDIAN
    if (PSR_BE) {
	    *pd = val1;
	    *ph = HW0(val2);
	} else {
	    DWORD dtmp = BitfX(val1,16,48) << 16 | HW0(val2);
	    HWORD htmp = HW0(val1);

	    *pd = swapd(dtmp);
	    *ph = swaph(htmp);
	}
#else /* !BYTE_ORDER === BIG_ENDIAN */
	if (PSR_BE) {
	    *pd = swapd(val1);
	    *ph = swaph(HW0(val2));
	} else {
	    *pd = HW0(val2) | (BitfX(val1,16,48) << 16);
	    *ph = HW0(val1);
	}
#endif /* !BYTE_ORDER === BIG_ENDIAN */
    } else if (dbptCheck(adr, 2, 10))
	progStop("Data breakpoint encountered\n");
    else if (adr & 0xF) {
	unalignedDataFault(MEM_ST_ACCESS);
	return NO;			/* never reached? */
    } else if ((pd = pmemMLookup_q(adr))) {
	HWORD *ph = (HWORD*)(pd + 1);
#if BYTE_ORDER == BIG_ENDIAN
    if (PSR_BE) {
	    *pd = val1;
	    *ph = HW0(val2);
	} else {
	    DWORD dtmp = BitfX(val1,16,48) << 16 | HW0(val2);
	    HWORD htmp = HW0(val1);

	    *pd = swapd(dtmp);
	    *ph = swaph(htmp);
	}
#else /* !BYTE_ORDER === BIG_ENDIAN */
	if (PSR_BE) {
	    *pd = swapd(val1);
	    *ph = swaph(HW0(val2));
	} else {
	    *pd = HW0(val2) | (BitfX(val1,16,48) << 16);
	    *ph = HW0(val1);
	}
#endif /* !BYTE_ORDER === BIG_ENDIAN */
    } else
	unallocPageWrt16(adr, val1, val2, PSR_BE);
    return YES;
}

BOOL memWrt16(ADDR adr, DWORD val1, DWORD val2)
{
    DWORD *pd;

    if (use_alat)
	alat_inval_multiple_entries(adr, 16);
    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_WRITE, DOFF_TRC_16_BYTES);
    if (!abi && dtlbLookup(adr, 16, MEM_ST_ACCESS, PSR_CPL, PSR_DT, &adr) == -1)
	return NO;
    if ((pd = pmemLookup_q(adr))) {
	FIX_BYTEORDER_Q(PSR_BE, *pd, *(pd+1), val1, val2);
    } else if (dbptCheck(adr, 2, 16))
	progStop("Data breakpoint encountered\n");
    else if (adr & 0xF) {
	unalignedDataFault(MEM_ST_ACCESS);
	return NO;			/* never reached? */
    } else if ((pd = pmemMLookup_q(adr))) {
	FIX_BYTEORDER_Q(PSR_BE, *pd, *(pd+1), val1, val2);
    } else
	unallocPageWrt16(adr, val1, val2, PSR_BE);
    return YES;
}

BOOL rseWrt(ADDR adr, DWORD val)
{
    DWORD *pd;

    if (traceEnb)
	WRT_DOFFSET_TR(adr, DOFF_TRC_RSEWRITE, DOFF_TRC_8_BYTES);
    if (!abi && dtlbLookup(adr, 8, RSE_ST_ACCESS, RSC_PL, PSR_RT, &adr) == -1)
	return NO;
    if ((pd = pmemLookup_d(adr)))
	*pd = FIX_BYTEORDER_D(RSC_BE, val);
    else if (dbptCheck(adr, 2, 8))
	progStop("Data breakpoint encountered\n");
    else if ((pd = pmemMLookup_d(adr)))
	*pd = FIX_BYTEORDER_D(RSC_BE, val);
    else
	unallocPageWrt8(adr, val, RSC_BE);
    return YES;
}


/************************************/
/* Simulated data memory semaphores */
/************************************/

#if defined(__hppa) && defined(__hpux)
#include <machine/inline.h>
#endif

BOOL memXchg(ADDR adr, unsigned size, REG val, REG *pval)
{
    BYTE  *pb;
    HWORD *ph;
    WORD  *pw;
    DWORD *pd, d;

    if (use_alat)
	alat_inval_multiple_entries(adr, size);
    if (!abi &&
	dtlbLookup(adr, size, MEM_SEMA_ACCESS, PSR_CPL, PSR_DT, &adr) == -1)
	return NO;
    switch (size) {
	case 1:
	    if (traceEnb)
		WRT_DOFFSET_TR(adr, DOFF_TRC_SEMAPHORE, DOFF_TRC_1_BYTE);
	    if ((pb = pmemLookup_b(adr))) {
		*pval = *pb;
		*pb = val;
	    } else if (dbptCheck(adr, 3, 1)) {
		progStop("Data breakpoint encountered\n");
	    } else if ((pb = pmemMLookup_b(adr))) {
		*pval = *pb;
		*pb = val;
	    } else {
		/* XXX - what to do here? */
		unallocPageRd(adr, 1, &d);
		*pval = d;
	    }
	    break;
	case 2:
	    if (traceEnb)
		WRT_DOFFSET_TR(adr, DOFF_TRC_SEMAPHORE, DOFF_TRC_2_BYTES);
	    if ((ph = pmemLookup_h(adr))) {
		*pval = *ph;
		*ph = FIX_BYTEORDER_H(PSR_BE, val);
	    } else if (dbptCheck(adr, 3, 2))
		progStop("Data breakpoint encountered\n");
	    else if (adr & 0x1) {
		unalignedDataFault(MEM_SEMA_ACCESS);
		return NO;			/* never reached? */
	    } else if ((ph = pmemMLookup_h(adr))) {
		*pval = *ph;
		*ph = FIX_BYTEORDER_H(PSR_BE, val);
	    }
	    *pval = FIX_BYTEORDER_H(PSR_BE, *pval);
	    break;
	case 4:
	    if (traceEnb)
		WRT_DOFFSET_TR(adr, DOFF_TRC_SEMAPHORE, DOFF_TRC_4_BYTES);
	    if ((pw = pmemLookup_w(adr))) {
#ifndef __hppa
		*pval = *pw;
		*pw = FIX_BYTEORDER_W(PSR_BE, val);
#else
		if (!val) {
		    register unsigned *addr, value;
		    addr = pw;
#if defined __GNUC__
		    asm volatile ("ldcws 0(%1),%0" : "=r"(value) : "r"(addr));
#else
		    _LDCWS(0, 0, addr, value);
#endif
		    *pval = value;
		} else {
		    *pval = *pw;
		    *pw = FIX_BYTEORDER_W(PSR_BE, val);
		}
#endif
	    } else if (dbptCheck(adr, 3, 4))
		progStop("Data breakpoint encountered\n");
	    else if (adr & 0x3) {
		unalignedDataFault(MEM_SEMA_ACCESS);
		return NO;			/* never reached? */
	    } else if ((pw = pmemMLookup_w(adr))) {
#ifndef __hppa
		*pval = *pw;
		*pw = FIX_BYTEORDER_W(PSR_BE, val);
#else
		if (!val) {
		    register unsigned *addr, value;
		    addr = pw;
#if defined __GNUC__
		    asm volatile ("ldcws 0(%1),%0" : "=r"(value) : "r"(addr));
#else
		    _LDCWS(0, 0, addr, value);
#endif
		    *pval = value;
		} else {
		    *pval = *pw;
		    *pw = FIX_BYTEORDER_W(PSR_BE, val);
		}
#endif
	    }
	    *pval = FIX_BYTEORDER_W(PSR_BE, *pval);
	    break;
	case 8:
	    if (traceEnb)
		WRT_DOFFSET_TR(adr, DOFF_TRC_SEMAPHORE, DOFF_TRC_8_BYTES);
	    if ((pd = pmemLookup_d(adr))) {
		*pval = *pd;
		*pd = FIX_BYTEORDER_D(PSR_BE, val);
	    } else if (dbptCheck(adr, 3, 8))
		progStop("Data breakpoint encountered\n");
	    else if (adr & 0x7) {
		unalignedDataFault(MEM_SEMA_ACCESS);
		return NO;			/* never reached? */
	    } else if ((pd = pmemMLookup_d(adr))) {
		*pval = *pd;
		*pd = FIX_BYTEORDER_D(PSR_BE, val);
	    }
	    *pval = FIX_BYTEORDER_D(PSR_BE, *pval);
	    break;
    }
    return YES;
}

BOOL memXchgAdd(ADDR adr, unsigned size, int inc, REG *pval)
{
    WORD  *pw;
    DWORD *pd;

    if (use_alat)
	alat_inval_multiple_entries(adr, size);
    if (!abi &&
	dtlbLookup(adr, size, MEM_FETCHADD_ACCESS, PSR_CPL, PSR_DT, &adr) == -1)
	return NO;
    switch (size) {
	case 4:
	    if (traceEnb)
		WRT_DOFFSET_TR(adr, DOFF_TRC_SEMAPHORE, DOFF_TRC_4_BYTES);
	    if ((pw = pmemLookup_w(adr))) {
		*pval = FIX_BYTEORDER_W(PSR_BE, *pw);
		*pw = *pval + inc;
		*pw = FIX_BYTEORDER_W(PSR_BE, *pw);
	    } else if (dbptCheck(adr, 3, 4))
		progStop("Data breakpoint encountered\n");
	    else if (adr & 0x3) {
		unalignedDataFault(MEM_SEMA_ACCESS);
		return NO;			/* never reached? */
	    } else if ((pw = pmemMLookup_w(adr))) {
		*pval = FIX_BYTEORDER_W(PSR_BE, *pw);
		*pw = *pval + inc;
		*pw = FIX_BYTEORDER_W(PSR_BE, *pw);
	    } else {
		unallocPageRd(adr, 4, pval);
		unallocPageWrt4(adr, *pval, PSR_BE);
	    }
	    break;
	case 8:
	    if (traceEnb)
		WRT_DOFFSET_TR(adr, DOFF_TRC_SEMAPHORE, DOFF_TRC_8_BYTES);
	    if ((pd = pmemLookup_d(adr))) {
		*pval = FIX_BYTEORDER_D(PSR_BE, *pd);
		*pd = *pval + inc;
		*pd = FIX_BYTEORDER_D(PSR_BE, *pd);
	    } else if (dbptCheck(adr, 3, 8))
		progStop("Data breakpoint encountered\n");
	    else if (adr & 0x7) {
		unalignedDataFault(MEM_SEMA_ACCESS);
		return NO;			/* never reached? */
	    } else if ((pd = pmemMLookup_d(adr))) {
		*pval = FIX_BYTEORDER_D(PSR_BE, *pd);
		*pd = *pval + inc;
		*pd = FIX_BYTEORDER_D(PSR_BE, *pd);
	    } else {
		unallocPageRd(adr, 8, pval);
		unallocPageWrt8(adr, *pval, PSR_BE);
	    }
	    break;
    }
    return YES;
}

BOOL memXchgCond(ADDR adr, unsigned size, REG val, REG *pval)
{
    BYTE  *pb;
    HWORD *ph;
    WORD  *pw;
    DWORD *pd;

    /* XXX - Only invalidate ALAT if CCV matches! */
    if (use_alat)
	alat_inval_multiple_entries(adr, size);
    if (!abi &&
	dtlbLookup(adr, size, MEM_SEMA_ACCESS, PSR_CPL, PSR_DT, &adr) == -1)
	return NO;
    switch (size) {
	case 1:
	    if (traceEnb)
		WRT_DOFFSET_TR(adr, DOFF_TRC_SEMAPHORE, DOFF_TRC_1_BYTE);
	    if ((pb = pmemLookup_b(adr))) {
		*pval = *pb;
		if (*pb == CCV)
		    *pb = val;
	    } else if (dbptCheck(adr, 3, 1)) {
		progStop("Data breakpoint encountered\n");
	    } else if ((pb = pmemMLookup_b(adr))) {
		*pval = *pb;
		if (*pb == CCV)
		    *pb = val;
	    } else {
		/* XXX - what to do here? */
		unallocPageRd(adr, 1, pval);
		if (*pval == CCV)
		    unallocPageWrt1(adr, *pval);
	    }
	    break;
	case 2:
	    if (traceEnb)
		WRT_DOFFSET_TR(adr, DOFF_TRC_SEMAPHORE, DOFF_TRC_2_BYTES);
	    if ((ph = pmemLookup_h(adr))) {
		*pval = FIX_BYTEORDER_H(PSR_BE, *ph);
		if (*pval == CCV)
		    *ph = FIX_BYTEORDER_H(PSR_BE, val);
	    } else if (dbptCheck(adr, 3, 2))
		progStop("Data breakpoint encountered\n");
	    else if (adr & 0x1) {
		unalignedDataFault(MEM_SEMA_ACCESS);
		return NO;			/* never reached? */
	    } else if ((ph = pmemMLookup_h(adr))) {
		*pval = FIX_BYTEORDER_H(PSR_BE, *ph);
		if (*pval == CCV)
		    *ph = FIX_BYTEORDER_H(PSR_BE, val);
	    } else {
		unallocPageRd(adr, 2, pval);
 		if (*pval == CCV)
		    unallocPageWrt2(adr, *pval, PSR_BE);
	    }
	    break;
	case 4:
	    if (traceEnb)
		WRT_DOFFSET_TR(adr, DOFF_TRC_SEMAPHORE, DOFF_TRC_4_BYTES);
	    if ((pw = pmemLookup_w(adr))) {
		*pval = FIX_BYTEORDER_W(PSR_BE, *pw);
		if (*pval == CCV)
		    *pw = FIX_BYTEORDER_W(PSR_BE, val);
	    } else if (dbptCheck(adr, 3, 4))
		progStop("Data breakpoint encountered\n");
	    else if (adr & 0x3) {
		unalignedDataFault(MEM_SEMA_ACCESS);
		return NO;			/* never reached? */
	    } else if ((pw = pmemMLookup_w(adr))) {
		*pval = FIX_BYTEORDER_W(PSR_BE, *pw);
		if (*pval == CCV)
		    *pw = FIX_BYTEORDER_W(PSR_BE, val);
	    } else {
		unallocPageRd(adr, 4, pval);
		if (*pval == CCV)
		    unallocPageWrt4(adr, *pval, PSR_BE);
	    }
	    break;
	case 8:
	    if (traceEnb)
		WRT_DOFFSET_TR(adr, DOFF_TRC_SEMAPHORE, DOFF_TRC_8_BYTES);
	    if ((pd = pmemLookup_d(adr))) {
		*pval = FIX_BYTEORDER_D(PSR_BE, *pd);
		if (*pval == CCV)
		    *pd = FIX_BYTEORDER_D(PSR_BE, val);
	    } else if (dbptCheck(adr, 3, 8))
		progStop("Data breakpoint encountered\n");
	    else if (adr & 0x7) {
		unalignedDataFault(MEM_SEMA_ACCESS);
		return NO;			/* never reached? */
	    } else if ((pd = pmemMLookup_d(adr))) {
		*pval = FIX_BYTEORDER_D(PSR_BE, *pd);
		if (*pval == CCV)
		    *pd = FIX_BYTEORDER_D(PSR_BE, val);
	    } else {
		unallocPageRd(adr, 8, pval);
		if (*pval == CCV)
		    unallocPageWrt8(adr, *pval, PSR_BE);
	    }
	    break;
    }
    return YES;
}

BOOL memXchg16Cond(ADDR adr, REG val1, REG val2, REG *pval)
{
    DWORD *pd;
    ADDR adr16 = adr & ~(ADDR)0x8;		/* 16-byte align address */
    BOOL half = (adr & (ADDR)0x8) >> 3;		/* the half to read/compare */

    /* XXX - Only invalidate ALAT if CCV matches! */
    if (use_alat)
	alat_inval_multiple_entries(adr16, 16);
    if (!abi &&
	dtlbLookup(adr16, 16, MEM_SEMA_ACCESS, PSR_CPL, PSR_DT, &adr16) == -1)
	return NO;
    if (traceEnb)
	WRT_DOFFSET_TR(adr16, DOFF_TRC_SEMAPHORE, DOFF_TRC_16_BYTES);
    if ((pd = pmemLookup_q(adr16))) {
        *pval = FIX_BYTEORDER_D(PSR_BE, *(pd+half));
	if (*pval == CCV) {
	    *pd = FIX_BYTEORDER_D(PSR_BE, val1);
	    *(pd+1) = FIX_BYTEORDER_D(PSR_BE, val2);
	}
    } else if (dbptCheck(adr16, 3, 16))
	progStop("Data breakpoint encountered\n");
    else if (adr16 & 0xF) {
	unalignedDataFault(MEM_SEMA_ACCESS);
	return NO;			/* never reached? */
    } else if ((pd = pmemMLookup_q(adr16))) {
        *pval = FIX_BYTEORDER_D(PSR_BE, *(pd+half));
	if (*pval == CCV) {
	    *pd = FIX_BYTEORDER_D(PSR_BE, val1);
	    *(pd+1) = FIX_BYTEORDER_D(PSR_BE, val2);
	}
    } else {
	unallocPageRd(adr+half, 8, pval);
	if (*pval == CCV) {
        unallocPageWrt16(adr16, val1, val2, PSR_BE);
    }
    }
    return YES;
}


/*******************************/
/* Simulator data memory reads */
/*******************************/

/* XXX - Called by memBBRd and a few others */
static BOOL memMRd1(ADDR adr, BYTE *pval)
{
    BYTE *pb;

    if ((pb = pmemMLookup_b(adr))) {
	*pval = *pb;
	return YES;
    } else
	return NO;
}

BOOL memMRd(ADDR adr, unsigned size, DWORD *pval)
{
    BYTE *pb;
    HWORD *ph;
    WORD *pw;
    DWORD *pd;
    BOOL ret;

    if (!abi && !dtlbMLookup(adr, 0, PSR_CPL, PSR_DT, &adr))
	return NO;

    switch (size) {
	case 1:
	    if ((pb = pmemMLookup_b(adr))) {
		*(BYTE*)pval = *pb;
		ret = YES;
	    } else
		ret = NO;
	    break;
	case 2:	/* Used by elfLoad to swap ELF header values and by profCnt */
	    if ((ph = pmemMLookup_h(adr))) {
		*(HWORD*)pval = FIX_BYTEORDER_H(PSR_BE && !PSR_IS, *ph);
		ret = YES;
	    } else
		ret = NO;
	    break;
	case 4:	/* Used by elfLoad to swap ELF header values */
	    if ((pw = pmemMLookup_w(adr))) {
		*(WORD*)pval = FIX_BYTEORDER_W(PSR_BE && !PSR_IS, *pw);
		ret = YES;
	    } else
		ret = NO;
	    break;
	case 8:	/* Used by datwLine and for the '*<expr>' evaluation */
	    if ((pd = pmemMLookup_d(adr))) {
		*pval = FIX_BYTEORDER_D(PSR_BE && !PSR_IS, *pd);
		ret = YES;
	    } else if (!abi)
		ret = unallocMRd(adr, 8, pval);
	    else
		ret = NO;
	    break;
	default:
	    ret = NO;
    }

    return ret;
}

BOOL vhptMRd(ADDR adr, DWORD *ptpa, DWORD *ptar, DWORD *ptag)
{
    DWORD *pd;

    if (!dtlbMLookup(adr, 1, 0, YES, &adr))
	return NO;
    if ((pd = pmemMLookup_d(adr))) {
	*ptpa = FIX_BYTEORDER_D(DCR_BE, pd[0]);
	*ptar = FIX_BYTEORDER_D(DCR_BE, pd[1]);
	*ptag = FIX_BYTEORDER_D(DCR_BE, pd[2]);
	return YES;
    } else
	return NO;
}




/********************************/
/* Simulator data memory writes */
/********************************/

/* Called by "=1" command and memBBWrt */
static BOOL memMWrt1(ADDR adr, BYTE val)
{
    BYTE *pb;

    if (!abi && !dtlbMLookup(adr, 2, PSR_CPL, PSR_DT, &adr))
	return NO;

    if (!abi && xtpAccess(adr))
	xtpCycle(val);

    if ((pb = pmemMLookup_b(adr))) {
	*pb = val;
	clearPdecode(adr, 1);
	return YES;
    } else if (!abi) {
	IS_t x;

	x.addr = adr;
	x.data = val;
	x.size = 1;
	switch (iomStore(&x)) {
	    case 0:
		return NO;
	    case 1:
		*((BYTE *)pmemLookup_p(adr)) = val;
		clearPdecode(adr, 1);
		return YES;
	    case 2:
		clearPdecode(adr, 1);
		return YES;
	}
    }
	return NO;
}

/* Called by "=2" command */
static BOOL memMWrt2(ADDR adr, HWORD val)
{
    HWORD *ph;

    if (!abi && !dtlbMLookup(adr, 2, PSR_CPL, PSR_DT, &adr))
	return NO;
    val = FIX_BYTEORDER_H(PSR_BE, val);
    if ((ph = pmemMLookup_h(adr))) {
	*ph = val;
	clearPdecode(adr, 2);
	return YES;
    } else if (!abi) {
	IS_t x;

	x.addr = adr;
	x.data = val;
	x.size = 2;
	switch (iomStore(&x)) {
	    case 0:
		return NO;
	    case 1:
		*(HWORD *)((BYTE *)pmemLookup_p(adr)) = val;
		clearPdecode(adr, 2);
		return YES;
	    case 2:
		clearPdecode(adr, 2);
		return YES;
	}
    }
	return NO;
}

/* Called by "=4" command and by elfLoad to set up runtime stack */
static BOOL memMWrt4(ADDR adr, WORD val)
{
    WORD *pw;

    if (!abi && !dtlbMLookup(adr, 2, PSR_CPL, PSR_DT, &adr))
	return NO;
    if (!PSR_BE)
    val = FIX_BYTEORDER_W(PSR_BE, val);
    if ((pw = pmemMLookup_w(adr))) {
	*pw = val;
	clearPdecode(adr, 4);
	return YES;
    } else if (!abi) {
	IS_t x;

	x.addr = adr;
	x.data = val;
	x.size = 4;
	switch (iomStore(&x)) {
	    case 0:
		return NO;
	    case 1:
		*(WORD *)((BYTE *)pmemLookup_p(adr)) = val;
		clearPdecode(adr, 4);
		return YES;
	    case 2:
		clearPdecode(adr, 4);
		return YES;
	}
    }
	return NO;
}

/* Called by "=8" command and by elfLoad to set up runtime stack */
static BOOL memMWrt8(ADDR adr, DWORD val)
{
    DWORD *pd;

    if (!abi && !dtlbMLookup(adr, 2, PSR_CPL, PSR_DT, &adr))
	return NO;

    if (!PSR_BE)
	val = FIX_BYTEORDER_D(PSR_BE, val);

    if ((pd = pmemMLookup_d(adr))) {
	*pd = val;
	clearPdecode(adr, 8);
	return YES;
    } else if (!abi) {
	IS_t x;

	x.addr = adr;
	x.data = val;
	x.size = 8;
	switch (iomStore(&x)) {
	    case 0:
		return NO;
	    case 1:
		*(DWORD *)((BYTE *)pmemLookup_p(adr)) = val;
		clearPdecode(adr, 8);
		return YES;
	    case 2:
		clearPdecode(adr, 8);
		return YES;
	}
    }
	return NO;
}

BOOL memMWrt(ADDR adr, unsigned size, DWORD val)
{

    switch (size) {
	case 1:
	    return memMWrt1(adr, val);

	case 2:
	    return memMWrt2(adr, val);

	case 4:
	    return memMWrt4(adr, val);

	case 8:
	    return memMWrt8(adr, val);

	default:
	    return NO;
    }
}

/* Called by "pd" command, prgwLine, datwLine, ibptLoad, ibptUnload */
/* XXX - Should be named Bundle Read (memMBRd) */
/* Also called by the write bundle trace macros */
BOOL memMIRd(ADDR adr, DWORD *pval)
{
    DWORD *pdw;
    int i;

    if (!abi && !itlbMLookup(adr, PSR_IT, &adr))
	return NO;

    if (!(pdw = pmemMLookup_d(adr)))
	return NO;

    for (i = 0; i < 2; i++)
	pval[i] = *pdw++;

    return YES;
}

/* Called by ibptLoad(), ibptUnload() */
BOOL memMIWrt(ADDR adr, DWORD *pval)
{
    DWORD *pdw;
    int i;

    if (!abi && !itlbMLookup(adr, PSR_IT, &adr))
	return NO;

    if (!(pdw = pmemMLookup_d(adr)))
	return NO;

    for (i = 0; i < 2; i++)
	*pdw++ = pval[i];

    return YES;
}


/* Called for loading code pages */
void memMPIWrt(ADDR adr, const BYTE *src, unsigned size)
{
    BYTE *dst = (BYTE *)pmemLookup_p(adr);
    (void)memcpy(dst, src, size);
}

/* Called for loading data pages */
/* XXX - Can maybe merge w/ memMPIWrt if an access mode argument is passed */
void memMPWrt(ADDR adr, const BYTE *src, unsigned size)
{
    BYTE *dst = (BYTE *)pmemLookup_p(adr);
    (void)memcpy(dst, src, size);
}

/* XXX - Assumes start is page-aligned */
/* XXX - Ignores any failures from pmemMap */
void memMPMap(ADDR start, char *hostadr, size_t len)
{
    ADDR adr, end = start+len-1;

    for (adr = start; adr <= end; adr += page_size, hostadr += page_size)
	(void)pmemMap(adr, (void *)hostadr);
}

/* Called by various syscalls (app-mode only) */
BOOL memBBRd(ADDR adr, BYTE *buf, unsigned nbytes)
{
    if (nbytes) {
	unsigned i;

	if (dbptCheck(adr, 1, nbytes))
	    progStop("Data breakpoint encountered\n");
	/* XXX - had to clear the dbpt before I could continue simulation! */
	for (i = 0; i < nbytes; i++) {
	    if (!memMRd1(adr, buf))
		return NO;
	    adr++;
	    buf++;
	}
    } else /* XXX - checking for dbpts either done 1 at a time or need to
	    *       keep a count of the bytes and check afterwards
	    */
	for (;;) {
	    if (!memMRd1(adr, buf))
		return NO;
	    if (!*buf)
		break;
	    adr++;
	    buf++;
	}
    return YES;
}

/* Called by elfLoad (args on runtime stack), and various syscalls */
void memBBWrt(ADDR adr, const BYTE *buf, unsigned nbytes)
{
    unsigned i;

    /* XXX - this will cause a core dump if hit when used by elfLoad */
    if (dbptCheck(adr, 2, nbytes))
	progStop("Data breakpoint encountered\n");
    for (i = 0; i < nbytes; i++)
	memMWrt1(adr++, *buf++);
}

/*
 * Optimized version of the previous function
 * Mostly used by SSC_READ
 * 04/21/99 S.Eranian 
 */
extern void cmdwPrint(const char *, ...);
void memBBWrt_opt(ADDR adr, const BYTE *buf, unsigned nbytes)
{
    unsigned i, nwords;
    unsigned long long *p =  (unsigned long long *)buf;

    /*
     * sanity check: note that we assume that the destination buffer
     * is aligned correctly.
     */
    if ((unsigned long) buf & 0x7) {
	memBBWrt(adr, buf, nbytes);
	return;
    }

    /* let's be safe here */
    if ( adr & 0x7 ) {
        memBBWrt(adr, buf, nbytes);
        return;
    }

    nwords = nbytes >> 3;
    for (i = 0; i < nwords; i++) {
	memMWrt8(adr, *p++);
	adr += 8;
    }
    if (nbytes & 7) {
	memBBWrt(adr, (BYTE *) p, (nbytes & 7));
    }
}

/* 
 * Fixes a problem with system call emulation where
 * we would copy into a non yet mapped page. The called to pmemLookup_p()
 * ensures that we actually get a physical page at whatever address we're
 * passing (user-mode only, abi=YES).
 * As an optimization only call pmemLookup_p() when necessary and not
 * for every byte.
 * 06/23/99 S.Eranian 
 */
void memBBWrt_alloc(ADDR adr, const BYTE *buf, unsigned nbytes)
{
    unsigned i;

    /* touch and possibly allocate first page */
    pmemLookup_p(page_base(adr));

    for (i = 0; i < nbytes; i++) {

	/* only touch at page boundary */
    	if ( (adr & (page_size-1)) ==0 ) {
	   pmemLookup_p(page_base(adr));
	}
	memMWrt1(adr++, *buf++);
    }
}

/* Called by some SSCs */
BOOL memBBRdP(ADDR adr, BYTE *buf, unsigned nbytes)
{
    ADDR pg_start, pg_end, pa;
    BYTE *pb;
    unsigned start_bytes, end_bytes;

    adr = BitfR(adr,1,63);	/* XXX - is this necessary? */

    if (!nbytes)
	for (;;) {
	    if (!(pb = pmemMLookup_b(adr)))
		return NO;
	    if (!(*buf = *pb))
		return YES;
	    adr++;
	    buf++;
	}

    pg_start = page_base(adr);
    pg_end = page_base(adr+nbytes-1);

    /* Special case for reading from a single page */
    if (pg_start == pg_end) {
	pb = (BYTE *)pmemLookup_p(pg_start);
	(void)memcpy(buf, pb+page_offset(adr), nbytes);
	return YES;
    }

    start_bytes = page_size - page_offset(adr);
    end_bytes = adr + nbytes - pg_end;

    /* Copy partial first page */
    pb = (BYTE *)pmemLookup_p(pg_start);
    (void)memcpy(buf, pb+page_offset(adr), start_bytes);
    buf += start_bytes;

    /* Copy entire middle pages */
    for (pa = pg_start+page_size; pa < pg_end; pa += page_size) {
	pb = (BYTE *)pmemLookup_p(pa);
	(void)memcpy(buf, pb, page_size);
	buf += page_size;
    }

    /* Copy partial last page */
    pb = (BYTE *)pmemLookup_p(pg_end);
    (void)memcpy(buf, pb, end_bytes);
    return YES;
}

/* Called by some SSCs */
void memBBWrtP(ADDR adr, const BYTE *buf, unsigned nbytes)
{
    ADDR pg_start, pg_end, pa;
    BYTE *pb;
    unsigned start_bytes, end_bytes;

    adr = BitfR(adr,1,63);	/* XXX - is this necessary? */
    pg_start = page_base(adr);
    pg_end = page_base(adr+nbytes-1);

    /* Special case for writing to a single page */
    if (pg_start == pg_end) {
	pb = (BYTE *)pmemLookup_p(pg_start);
	(void)memcpy(pb+page_offset(adr), buf, nbytes);
	return;
    }

    start_bytes = page_size - page_offset(adr);
    end_bytes = adr + nbytes - pg_end;

    /* Copy partial first page */
    pb = (BYTE *)pmemLookup_p(pg_start);
    (void)memcpy(pb+page_offset(adr), buf, start_bytes);
    buf += start_bytes;

    /* Copy entire middle pages */
    for (pa = pg_start+page_size; pa < pg_end; pa += page_size) {
	pb = (BYTE *)pmemLookup_p(pa);
	(void)memcpy(pb, buf, page_size);
	buf += page_size;
    }

    /* Copy partial last page */
    pb = (BYTE *)pmemLookup_p(pg_end);
    (void)memcpy(pb, buf, end_bytes);
}

void memFree(ADDR adr)
{
    pmemStruct *p, *q;
    ADDR padr = page_base(adr);
    unsigned hash = PmemHash(adr);

    for (q = NULL, p = pmemHshTbl[hash]; p; q = p, p = p->next)
	if (p->padr == padr) {
	    if (q)
		q->next = p->next;
	    else
		pmemHshTbl[hash] = p->next;
	    free(p);
	    return;
	}
}

void memFreeAll(void)
{
    int i;
    pmemStruct *p, *q;

    for (i = 0; i < PMEMHSHSZ; i++) {
	for (p = pmemHshTbl[i]; p; p = q) {
	    free(p->pmem);
	    q = p->next;
	    free(p);
	}
	pmemHshTbl[i] = NULL;
    }
}

static BOOL unallocMRd(ADDR adr, int size, DWORD *pval)
{
    IS_t x;

    x.addr = adr;
    x.size = size;
    if (iomLoad(&x) == 2) {
	*pval = x.data;
	return YES;
    } else
	return NO;
}

static void unallocPageRd(ADDR adr, int size, DWORD *pval)
{
    IS_t x;

    if (abi) {
	if (allocOK(adr)) {
	    (void)pmemLookup_p(adr);
	    *pval = 0;
	    if (size == 16)
		*(pval + 1) = 0;
	    return;
	} else {
	    segfault(adr);
	    return;
	}
    }
    x.addr = adr;
    x.size = size;
    switch (ioLoad(&x)) {
	case 0:
	    progExit("Illegal address\n");
	case 1:
	    (void)pmemLookup_p(adr);
	    *pval = 0;
	    if (size == 16)
		*(pval + 1) = 0;
	    break;
	case 2:
	    *pval = x.data;
	    if (size == 16)
		*(pval + 1) = x.fpnt;
	    break;
    }
}

static void unallocPageWrt1(ADDR adr, BYTE val)
{
    IS_t x;

    if (abi) {
	if (allocOK(adr)) {
	    *((BYTE *)pmemLookup_p(adr)) = val;
	    return;
	} else {
	    segfault(adr);
	    return;
	}
    }
    x.addr = adr;
    x.data = val;
    x.size = 1;
    switch (ioStore(&x)) {
	case 0:
	    progExit("Illegal address\n");
	case 1:
	    *((BYTE *)pmemLookup_p(adr)) = val;
	    break;
	case 2:
	    break;
    }
}

static void unallocPageWrt2(ADDR adr, HWORD val, BOOL be)
{
    IS_t x;

    val = FIX_BYTEORDER_H(be, val);
    if (abi) {
	if (allocOK(adr)) {
	    (void)memcpy((BYTE *)pmemLookup_p(adr), &val, 2);
	    return;
	} else {
	    segfault(adr);
	    return;
	}
    }
    x.addr = adr;
    x.data = val;
    x.size = 2;
    switch (ioStore(&x)) {
	case 0:
	    progExit("Illegal address\n");
	case 1:
	    (void)memcpy((BYTE *)pmemLookup_p(adr), &val, 2);
	    break;
	case 2:
	    break;
    }
}

static void unallocPageWrt4(ADDR adr, WORD val, BOOL be)
{
    IS_t x;

    val = FIX_BYTEORDER_W(be, val);
    if (abi) {
	if (allocOK(adr)) {
	    (void)memcpy((BYTE *)pmemLookup_p(adr), &val, 4);
	    return;
	} else {
	    segfault(adr);
	    return;
	}
    }
    x.addr = adr;
    x.data = val;
    x.size = 4;
    switch (ioStore(&x)) {
	case 0:
	    progExit("Illegal address\n");
	case 1:
	    (void)memcpy((BYTE *)pmemLookup_p(adr), &val, 4);
	    break;
	case 2:
	    break;
    }
}

static void unallocPageWrt8(ADDR adr, DWORD val, BOOL be)
{
    IS_t x;

    val = FIX_BYTEORDER_D(be, val);
    if (abi) {
	if (allocOK(adr)) {
	    (void)memcpy((BYTE *)pmemLookup_p(adr), &val, 8);
	    return;
	} else {
	    segfault(adr);
	    return;
	}
    }
    x.addr = adr;
    x.data = val;
    x.size = 8;
    switch (ioStore(&x)) {
	case 0:
	    progExit("Illegal address\n");
	case 1:
	    (void)memcpy((BYTE *)pmemLookup_p(adr), &val, 8);
	    break;
	case 2:
	    break;
    }
}

static void unallocPageWrt16(ADDR adr, DWORD val1, DWORD val2, BOOL be)
{
    IS_t x;
    BYTE *p;

    FIX_BYTEORDER_Q(be, val1, val2, val1, val2);
    if (abi) {
	if (allocOK(adr)) {
	    p = (BYTE *)pmemLookup_p(adr);
	    (void)memcpy(p, &val1, 8);
	    (void)memcpy(p + 8, &val2, 8);
	    return;
	} else {
	    segfault(adr);
	    return;
	}
    }
    x.addr = adr;
    x.data = val1;
    x.fpnt = val2;
    x.size = 16;
    switch (ioStore(&x)) {
	case 0:
	    progExit("Illegal address\n");
	case 1:
	    p = (BYTE *)pmemLookup_p(adr);
	    (void)memcpy(p, &val1, 8);
	    (void)memcpy(p + 8, &val2, 8);
	    break;
	case 2:
	    break;
    }
}


/*
 * Save/Restore Routines
 */

BOOL memGet(Meminfo_p *list)
{
    int i;
    pmemStruct *p;
    Meminfo_p pm;

    *list = NULL;
    for (i = 0; i < PMEMHSHSZ; i++)
	for (p = pmemHshTbl[i]; p; p = p->next) {
	    if (!(pm = malloc(sizeof *pm)))
		/* XXX - free what has already been malloc'ed? */
		return NO;
	    pm->startadr = p->padr;
	    /* XXX - why always 1? -- page_size would at least be more
		abstract */
	    pm->numpgs	 = 1;
	    pm->mdata	 = (void*)p->pmem;
	    pm->next	 = *list;
	    *list = pm;
	}
    /* XXX - the memory malloc'ed here is never freed -- either add a function
	     to free it or have caller manage it
     */

    return YES;
}

void memZerosPageSet(ADDR adr)
{
    (void)pmemLookup_p(page_base(adr));
}

void memSet(ADDR adr, REG val)
{
    DWORD *pg = pmemLookup_p(adr);
    *pg = val;
}

/******************************/
/* iA instruction memory read */
/******************************/

BOOL memIAIRd(ADDR4 eip, BYTE *pval, unsigned nbytes)
{
    BYTE *pb;
    ADDR adr = IPfromEIP(eip);

    /* XXX - need to deal with CS limits/faults */
    if (!dosABI && !itlbLookup(adr, PSR_IT, &adr))
	return NO;
    for ( ; nbytes; nbytes--) {
	if (!(pb = pmemLookup_b(adr++)))
	    return NO;
	*pval++ = *pb;
    }
    return YES;
}

/**************************************/
/* monitor iA instruction memory read */
/**************************************/

/* called by ibptLoad() */
BOOL memMIAIRd(ADDR4 eip, BYTE *pval, unsigned nbytes)
{
    BYTE *pb;
    ADDR adr = IPfromEIP(eip);

    if (!dosABI && !itlbMLookup(adr, PSR_IT, &adr))
	return NO;
    for ( ; nbytes; adr++, nbytes--) {
	if (!(pb = pmemMLookup_b(adr)))
	    return NO;
	*pval++ = *pb;
    }
    return YES;
}

/***************************************/
/* monitor iA instruction memory write */
/***************************************/

/* called by cmdIALoad(), ibptLoad(), ibptUnload() */
BOOL memMIAIWrt(ADDR adr, BYTE *pval, unsigned nbytes)
{
    BYTE *pb;

    if (!dosABI && !itlbMLookup(adr, PSR_IT, &adr))
	return NO;
    for ( ; nbytes; adr++, nbytes--) {
	pb = pmemMLookup_b(adr);
	if (pb == NULL)
	    pb = (BYTE*)pmemLookup_p(adr);
	*pb = *pval++;
    }
    return YES;
}

/******************/
/* iA memory read */
/******************/

/* Data breakpoints and DBR matching are not supported in IA32 mode.	*/
/* That's why there are no calls to dbptCheck and the size passed to	*/
/* to dtlbLookup is 1 to avoid unaligned data faults.			*/
Status memIAadrRd(ADDR adr, BYTE size, Accesstype acc, ADDR *padr, ADDR *padr2,
	       IADWORD *pval)
{
    BYTE *pb, *pb1, *pb2, *pb3, *pb4, *pb5, p1size, p2size;
    unsigned i;
    DWORD d;
    ADDR eadr;

    switch (size) {
    case 1:
	if (!dosABI && dtlbLookup(adr, 1, acc, PSR_CPL, PSR_DT, &adr) == -1)
	    return 0;
	if (acc & READ_ACCESS) {
	    if ((pb = pmemMLookup_b(adr)))
		*pval = *pb;
	    else {
		unallocPageRd(adr, 1, &d);
		*pval = d;
	    }
    }
	break;
    case 2:
	if (page_base(eadr = adr + 1) == page_base(adr)) {    /* 1 page */
	    if (!dosABI && dtlbLookup(adr, 1, acc, PSR_CPL, PSR_DT, &adr) == -1)
		return 0;
	    *padr2 = adr + 2;
	    /* XXX - should we try to optimize aligned references? */
	    /* XXX - add unaligned fault */
	    if (acc & READ_ACCESS) {
		if ((pb = pmemMLookup_b(adr)))
		    *pval = *(pb+1) << 8 | *pb;
		else {
		    unallocPageRd(adr, 2, &d);
		    *pval = d;
		}
        }
	} else {	/* page crossing */
	    if (!dosABI &&
		(dtlbLookup(adr, 1, acc, PSR_CPL, PSR_DT, &adr) == -1 ||
		 dtlbLookup(eadr, 1, acc, PSR_CPL, PSR_DT, &eadr) == -1))
		return 0;
	    *padr2 = page_base(eadr);
	    if (acc & READ_ACCESS) {
		if ((pb = pmemMLookup_b(adr)) &&
		    (pb1 = pmemMLookup_b(eadr)))
		    *pval = *pb1 << 8 | *pb;
		else {
		    if (!pb) {
			unallocPageRd(adr, 1, &d);
			*pval = d;
		    } else
			*pval = *pb;
		    if (!pb1) {
			unallocPageRd(eadr, 1, &d);
			*pval |= d << 8;
		    } else
			*pval |= *pb1 << 8;
		}
        }
	}
	break;
    case 4:
	if (page_base(eadr = adr + 3) == page_base(adr)) {    /* 1 page */
	    if (!dosABI && dtlbLookup(adr, 1, acc, PSR_CPL, PSR_DT, &adr) == -1)
		return 0;
	    *padr2 = adr + 4;
	    /* XXX - should we try to optimize aligned references? */
	    /* XXX - add unaligned fault */
	    if (acc & READ_ACCESS) {
		if ((pb = pmemMLookup_b(adr)))
		    *pval = *(pb+3) << 24 | *(pb+2) << 16 | *(pb+1) << 8 | *pb;
		else {
		    unallocPageRd(adr, 4, &d);
		    *pval = d;
		}
        }
	} else {	/* page crossing */
	    if (!dosABI &&
		(dtlbLookup(adr, 1, acc, PSR_CPL, PSR_DT, &adr) == -1 ||
		 dtlbLookup(eadr, 1, acc, PSR_CPL, PSR_DT, &eadr) == -1))
		return 0;
	    *padr2 = page_base(eadr);
	    if (acc & READ_ACCESS) {
		if ((pb = pmemMLookup_b(adr)) &&
		    (pb3 = pmemMLookup_b(eadr))) {
		    switch ((BYTE)adr & 3) {
			/* case 0 is aligned and can't cross page */
		    case 1:
			pb1 = pb + 1;
			pb2 = pb + 2;
			break;
		    case 2:
			pb1 = pb + 1;
			pb2 = pb3 - 1;
			break;
		    case 3:
			pb1 = pb3 - 2;
			pb2 = pb3 - 1;
			break;
		    }
		    *pval = *pb3 << 24 | *pb2 << 16 | *pb1 << 8 | *pb;
		} else {
		    /* XXX - fix this - see size 2 above */
		    progExit("memIAadrRd, size 4, unalloc page crossing\n");
		}
        }
	}
	break;
    case 6:
	if (page_base(eadr = adr + 5) == page_base(adr)) {    /* 1 page */
	    if (!dosABI && dtlbLookup(adr, 1, acc, PSR_CPL, PSR_DT, &adr) == -1)
		return 0;
	    *padr2 = adr + 6;
	    /* XXX - should we try to optimize aligned references? */
	    /* XXX - add unaligned fault */
	    if (acc & READ_ACCESS) {
		if ((pb = pmemMLookup_b(adr))) {
		    *pval = *(pb+3) << 24 | *(pb+2) << 16 | *(pb+1) << 8 | *pb;
		    *(pval+1) = *(pb+5) << 8 | *(pb+4);
		} else {
		    unallocPageRd(adr, 6, &d);
		    *pval = (IADWORD)d;
		    *(pval+1) = (IADWORD)(d >> 32);
		}
        }
	} else {	/* page crossing */
	    if (!dosABI &&
		(dtlbLookup(adr, 1, acc, PSR_CPL, PSR_DT, &adr) == -1 ||
		 dtlbLookup(eadr, 1, acc, PSR_CPL, PSR_DT, &eadr) == -1))
		return 0;
	    *padr2 = page_base(eadr);
	    if (acc & READ_ACCESS) {
		if ((pb = pmemMLookup_b(adr)) &&
		    (pb5 = pmemMLookup_b(eadr))) {
		    switch ((BYTE)adr & 7) {
			/* cases 0 - 2 can't cross page */
		    case 3:
			pb1 = pb + 1;
			pb2 = pb + 2;
			pb3 = pb + 3;
			pb4 = pb + 4;
			break;
		    case 4:
			pb1 = pb + 1;
			pb2 = pb + 2;
			pb3 = pb + 3;
			pb4 = pb5 - 1;
			break;
		    case 5:
			pb1 = pb + 1;
			pb2 = pb + 2;
			pb3 = pb5 - 2;
			pb4 = pb5 - 1;
			break;
		    case 6:
			pb1 = pb + 1;
			pb2 = pb5 - 3;
			pb3 = pb5 - 2;
			pb4 = pb5 - 1;
			break;
		    case 7:
			pb1 = pb5 - 4;
			pb2 = pb5 - 3;
			pb3 = pb5 - 2;
			pb4 = pb5 - 1;
			break;
		    }
		    *pval = *pb3 << 24 | *pb2 << 16 | *pb1 << 8 | *pb;
		    *(pval+1) = *pb5 << 8 | *pb4;
		} else {
		    /* XXX - fix this - see size 2 above */
		    progExit("memIAadrRd, size 6, unalloc page crossing\n");
		}
        }
	}
	break;
    case 16:		/* for pusha/popa */
	if (page_base(eadr = adr + 15) == page_base(adr)) {    /* 1 page */
	    if (!dosABI && dtlbLookup(adr, 1, acc, PSR_CPL, PSR_DT, &adr) == -1)
		return 0;
	    *padr2 = adr + 16;
	    /* XXX - should we try to optimize aligned references? */
	    /* XXX - add unaligned fault */
	    if (acc & READ_ACCESS) {
		if ((pb = pmemMLookup_b(adr))) {
		    *pval     = *(pb+1)  << 8 | *pb;
		    *(pval+1) = *(pb+3)  << 8 | *(pb+2);
		    *(pval+2) = *(pb+5)  << 8 | *(pb+4);
		    *(pval+3) = *(pb+7)  << 8 | *(pb+6);
		    *(pval+4) = *(pb+9)  << 8 | *(pb+8);
		    *(pval+5) = *(pb+11) << 8 | *(pb+10);
		    *(pval+6) = *(pb+13) << 8 | *(pb+12);
		    *(pval+7) = *(pb+15) << 8 | *(pb+14);
		} else {
		    unallocPageRd(adr, 2, &d);
		    *pval = d;
		    unallocPageRd(adr+2, 2, &d);
		    *(pval+1) = d;
		    unallocPageRd(adr+4, 2, &d);
		    *(pval+2) = d;
		    unallocPageRd(adr+6, 2, &d);
		    *(pval+3) = d;
		    unallocPageRd(adr+8, 2, &d);
		    *(pval+4) = d;
		    unallocPageRd(adr+10, 2, &d);
		    *(pval+5) = d;
		    unallocPageRd(adr+12, 2, &d);
		    *(pval+6) = d;
		    unallocPageRd(adr+14, 2, &d);
		    *(pval+7) = d;
		}
        }
	} else {	/* page crossing */
	    p1size = page_base(eadr) - adr;
	    p2size = eadr - page_base(eadr) + 1;
	    if (!dosABI &&
		(dtlbLookup(adr, 1, acc, PSR_CPL, PSR_DT, &adr) == -1 ||
		 dtlbLookup(eadr, 1, acc, PSR_CPL, PSR_DT, &eadr) == -1))
		return 0;
	    *padr2 = page_base(eadr);
	    if (acc & READ_ACCESS) {
		if ((pb = pmemMLookup_b(adr)) &&
		    (pb2 = pmemMLookup_b(page_base(eadr)))) {
		    for (i = 0; p1size; --p1size) {
			if (i % 2 == 0)
			    *pval = 0;
			*pval |= *pb++ << ((i % 2) * 8);
			if (++i % 2 == 0)
			    pval++;
		    }
		    for ( ; p2size; --p2size) {
			if (i % 2 == 0)
			    *pval = 0;
			*pval |= *pb2++ << ((i % 2) * 8);
			if (++i % 2 == 0)
			    pval++;
		    }
		} else {
		    /* XXX - fix this - see above */
		    progExit("memIAadrRd, size 16, unalloc page crossing\n");
		}
        }
	}
	break;
    case 32:		/* for pushad/popad */
	if (page_base(eadr = adr + 31) == page_base(adr)) {    /* 1 page */
	    if (!dosABI && dtlbLookup(adr, 1, acc, PSR_CPL, PSR_DT, &adr) == -1)
		return 0;
	    *padr2 = adr + 32;
	    /* XXX - should we try to optimize aligned references? */
	    /* XXX - add unaligned fault */
	    if (acc & READ_ACCESS) {
		if ((pb = pmemMLookup_b(adr))) {
		    *pval =
			*(pb+3)  << 24 | *(pb+2) << 16 |
			*(pb+1)  <<  8 | *pb;
		    *(pval+1) =
			*(pb+7)  << 24 | *(pb+6) << 16 |
			*(pb+5)  <<  8 | *(pb+4);
		    *(pval+2) =
			*(pb+11) << 24 | *(pb+10) << 16 |
			*(pb+9)  <<  8 | *(pb+8);
		    *(pval+3) =
			*(pb+15) << 24 | *(pb+14) << 16 |
			*(pb+13) <<  8 | *(pb+12);
		    *(pval+4) =
			*(pb+19) << 24 | *(pb+18) << 16 |
			*(pb+17) <<  8 | *(pb+16);
		    *(pval+5) =
			*(pb+23) << 24 | *(pb+22) << 16 |
			*(pb+21) <<  8 | *(pb+20);
		    *(pval+6) =
			*(pb+27) << 24 | *(pb+26) << 16 |
			*(pb+25) <<  8 | *(pb+24);
		    *(pval+7) =
			*(pb+31) << 24 | *(pb+30) << 16 |
			*(pb+29) <<  8 | *(pb+28);
		} else {
		    unallocPageRd(adr, 4, &d);
		    *pval = d;
		    unallocPageRd(adr+4, 4, &d);
		    *(pval+1) = d;
		    unallocPageRd(adr+8, 4, &d);
		    *(pval+2) = d;
		    unallocPageRd(adr+12, 4, &d);
		    *(pval+3) = d;
		    unallocPageRd(adr+16, 4, &d);
		    *(pval+4) = d;
		    unallocPageRd(adr+20, 4, &d);
		    *(pval+5) = d;
		    unallocPageRd(adr+24, 4, &d);
		    *(pval+6) = d;
		    unallocPageRd(adr+28, 4, &d);
		    *(pval+7) = d;
		}
        }
	} else {	/* page crossing */
	    p1size = page_base(eadr) - adr;
	    p2size = eadr - page_base(eadr) + 1;
	    if (!dosABI &&
		(dtlbLookup(adr, 1, acc, PSR_CPL, PSR_DT, &adr) == -1 ||
		 dtlbLookup(eadr, 1, acc, PSR_CPL, PSR_DT, &eadr) == -1))
		return 0;
	    *padr2 = page_base(eadr);
	    if (acc & READ_ACCESS) {
		if ((pb = pmemMLookup_b(adr)) &&
		    (pb2 = pmemMLookup_b(page_base(eadr)))) {
		    for (i = 0; p1size; --p1size) {
			if (i % 4 == 0)
			    *pval = 0;
			*pval |= *pb++ << ((i % 4) * 8);
			if (++i % 4 == 0)
			    pval++;
		    }
		    for ( ; p2size; --p2size) {
			if (i % 4 == 0)
			    *pval = 0;
			*pval |= *pb2++ << ((i % 4) * 8);
			if (++i % 4 == 0)
			    pval++;
		    }
		} else {
		    /* XXX - fix this - see above */
		    progExit("memIAadrRd, size 32, unalloc page crossing\n");
		}
        }
	}
	break;
    default:
	progExit("memIAadrRd - unsupported size %d\n", size);
	break;
    }
    /* padr gets physical address corresponding to adr passed in.
     * padr2 gets padr + size if access is all on one page or
     * the physical address of the base of the second page if the access
     * crossed a 4K page boundary.
     */
    *padr = adr;
    return 1;
}

Status memIARd(BYTE seg, ADDR4 ofs, BYTE size, Accesstype acc, ADDR *padr,
	    ADDR *padr2, IADWORD *pval)
{
    ADDR adr;
    IASegDescPtr segd = &SEGD(seg);

    /* XXX - add segment limit check, null segment check, etc. */
    adr = segd->base + ofs;
    return memIAadrRd(adr, size, acc, padr, padr2, pval);
}

/*******************/
/* iA memory write */
/*******************/

/* Data breakpoints are not supported in IA32 mode */

void memIAWrt(BYTE size, ADDR adr, ADDR adr2, IADWORD val)
{
    BYTE *pb, *pb2;

    switch (size) {
    case 1:
	if ((pb = pmemMLookup_b(adr)))
	    *pb = val;
	else
	    unallocPageWrt1(adr, val);
	break;
    case 2:
	if (adr2 == adr + 2) {    /* 1 page */
	    /* XXX - should we try to optimize aligned references? */
	    if ((pb = pmemMLookup_b(adr))) {
		*pb = val;
		*(pb+1) = val >> 8;
	    } else
		unallocPageWrt2(adr, val, NO);
	} else {	/* page crossing */
	    if ((pb = pmemMLookup_b(adr)) &&
		(pb2 = pmemMLookup_b(adr2))) {
		*pb = val;
		*pb2 = val >> 8;
	    } else {
		unallocPageWrt1(adr, val);
		unallocPageWrt1(adr2, val >> 8);
	    }
	}
	break;
    case 4:
	if (adr2 == adr + 4) {    /* 1 page */
	    /* XXX - should we try to optimize aligned references? */
	    if ((pb = pmemMLookup_b(adr))) {
		*pb = val;
		*(pb+1) = val >> 8;
		*(pb+2) = val >> 16;
		*(pb+3) = val >> 24;
	    } else
		unallocPageWrt4(adr, val, NO);
	} else {	/* page crossing */
	    if ((pb = pmemMLookup_b(adr)) &&
		(pb2 = pmemMLookup_b(adr2))) {
		*pb = val;
		switch ((BYTE)adr & 3) {
		    /* case 0 is aligned and can't cross page */
		case 1:
		    *(pb+1) = val >> 8;
		    *(pb+2) = val >> 16;
		    *pb2 = val >> 24;
		    break;
		case 2:
		    *(pb+1) = val >> 8;
		    *pb2 = val >> 16;
		    *(pb2+1) = val >> 24;
		    break;
		case 3:
		    *pb2 = val >> 8;
		    *(pb2+1) = val >> 16;
		    *(pb2+2) = val >> 24;
		    break;
		}
	    } else {
		unallocPageWrt1(adr, val);
		switch ((BYTE)adr & 3) {
		case 1:
		    unallocPageWrt1(adr+1, val >> 8);
		    unallocPageWrt1(adr+2, val >> 16);
		    unallocPageWrt1(adr2,  val >> 24);
		    break;
		case 2:
		    unallocPageWrt1(adr+1,  val >> 8);
		    unallocPageWrt1(adr2,   val >> 16);
		    unallocPageWrt1(adr2+1, val >> 24);
		    break;
		case 3:
		    unallocPageWrt1(adr2,   val >> 8);
		    unallocPageWrt1(adr2+1, val >> 16);
		    unallocPageWrt1(adr2+2, val >> 24);
		    break;
		}
	    }
	}
	break;
    case 6:
	/* XXX - fix this */
	progExit("memIAWrt called with size 6\n");
	break;
    case 16:		/* for pusha */
	/* although size is 16, we only write 2 bytes - caller loops 8 times */
	if (page_base(adr) == page_base(adr+1)) {    /* 1 page */
	    /* XXX - should we try to optimize aligned references? */
	    if ((pb = pmemMLookup_b(adr))) {
		*pb = val;
		*(pb+1) = val >> 8;
	    } else
		unallocPageWrt2(adr, val, NO);
	} else {	/* page crossing */
	    if ((pb = pmemMLookup_b(adr)) &&
		(pb2 = pmemMLookup_b(adr2))) {
		*pb = val;
		*pb2 = val >> 8;
	    } else {
		unallocPageWrt1(adr, val);
		unallocPageWrt1(adr2, val >> 8);
	    }
	}
	break;
    case 32:		/* for pushad */
	/* although size is 32, we only write 4 bytes - caller loops 8 times */
	if (page_base(adr) == page_base(adr+3)) {    /* 1 page */
	    /* XXX - should we try to optimize aligned references? */
	    if ((pb = pmemMLookup_b(adr))) {
		*pb = val;
		*(pb+1) = val >> 8;
		*(pb+2) = val >> 16;
		*(pb+3) = val >> 24;
	    } else
		unallocPageWrt4(adr, val, NO);
	} else {	/* page crossing */
	    if ((pb = pmemMLookup_b(adr)) &&
		(pb2 = pmemMLookup_b(adr2))) {
		*pb = val;
		switch ((BYTE)adr & 3) {
		    /* case 0 is aligned and can't cross page */
		case 1:
		    *(pb+1) = val >> 8;
		    *(pb+2) = val >> 16;
		    *pb2 = val >> 24;
		    break;
		case 2:
		    *(pb+1) = val >> 8;
		    *pb2 = val >> 16;
		    *(pb2+1) = val >> 24;
		    break;
		case 3:
		    *pb2 = val >> 8;
		    *(pb2+1) = val >> 16;
		    *(pb2+2) = val >> 24;
		    break;
		}
	    } else {
		unallocPageWrt1(adr, val);
		switch ((BYTE)adr & 3) {
		case 1:
		    unallocPageWrt1(adr+1, val >> 8);
		    unallocPageWrt1(adr+2, val >> 16);
		    unallocPageWrt1(adr2,  val >> 24);
		    break;
		case 2:
		    unallocPageWrt1(adr+1,  val >> 8);
		    unallocPageWrt1(adr2,   val >> 16);
		    unallocPageWrt1(adr2+1, val >> 24);
		    break;
		case 3:
		    unallocPageWrt1(adr2,   val >> 8);
		    unallocPageWrt1(adr2+1, val >> 16);
		    unallocPageWrt1(adr2+2, val >> 24);
		    break;
		}
	    }
	}
	break;
    default:
	progExit("memIAWrt - unsupported size %d\n", size);
	break;
    }
}

void setABI(BOOL val)
{
    abi = dosABI = unixABI = val;
}


/*******************************/
/* Page Table Display routines */
/*******************************/

size_t pageTableDisplaySize(void)
{
    int i;
    pmemStruct *p;
    size_t size = 0;

    for (i = 0; i < PMEMHSHSZ; i++)
	for (p = pmemHshTbl[i]; p; p = p->next)
	    size += 40;
    return size;
}

void getPageTableInfo(char buf[])
{
    int i;
    pmemStruct *p;
    char *s = buf;

    for (i = 0; i < PMEMHSHSZ; i++)
	for (p = pmemHshTbl[i]; p; p = p->next)
	    s += sprintf(s, "%016llx %p\n", p->padr, (void *)p->pmem);
}
