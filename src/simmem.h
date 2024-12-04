/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Memory Header
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
#ifndef _SKI_MEMORY_H_
#define _SKI_MEMORY_H_

#include <endian.h>
#include <stddef.h>

#include "types.h"
#include "sim.h"

extern unsigned long page_size;
extern unsigned int log2_page_size;
extern DWORD page_mask;

#define page_base(addr)		((addr) & page_mask)
#define page_align(addr)	(((addr) + ~page_mask) & page_mask)
#define page_offset(addr)	((addr) & ~page_mask)

/* Access Types */

typedef enum {
    READ_ACCESS		= 1,
    WRITE_ACCESS	= 1 <<  1,
    MEM_ACCESS		= 1 <<  2,
    SPEC_ACCESS		= 1 <<  3,
    ADV_ACCESS		= 1 <<  4,
    RSE_ACCESS		= 1 <<  5,
    VHPT_ACCESS		= 1 <<  6,
    NA_ACCESS		= 1 <<  7,
    PROBE_ACCESS	= 1 <<  8,
    FAULT_ACCESS	= 1 <<  9,
    TPA_MASK		= 1 << 10,
    FC_MASK		= 1 << 11,
    TAK_MASK		= 1 << 12,
    FETCHADD_ACCESS	= 1 << 13
} Accesstypemask;

typedef enum {
    EXECUTE_ACCESS,
    MEM_LD_ACCESS = MEM_ACCESS | READ_ACCESS,
    MEM_ST_ACCESS = MEM_ACCESS | WRITE_ACCESS,
    MEM_SEMA_ACCESS = MEM_ACCESS | READ_ACCESS | WRITE_ACCESS,
    MEM_FETCHADD_ACCESS = MEM_SEMA_ACCESS | FETCHADD_ACCESS,
    MEM_LDST_ACCESS = MEM_ACCESS | READ_ACCESS | WRITE_ACCESS,
    SPEC_LD_ACCESS = MEM_ACCESS | READ_ACCESS | SPEC_ACCESS,
    ADV_LD_ACCESS = MEM_ACCESS | READ_ACCESS | ADV_ACCESS,
    SPEC_ADV_LD_ACCESS = MEM_ACCESS | READ_ACCESS | SPEC_ACCESS | ADV_ACCESS,
    RSE_LD_ACCESS = RSE_ACCESS | READ_ACCESS,
    RSE_ST_ACCESS = RSE_ACCESS | WRITE_ACCESS,
    VHPT_RD_ACCESS = VHPT_ACCESS | READ_ACCESS,
    LFETCH_ACCESS = MEM_ACCESS | NA_ACCESS | READ_ACCESS,
    LFETCH_FAULT_ACCESS = MEM_ACCESS | NA_ACCESS | READ_ACCESS | FAULT_ACCESS,
	/* LFETCH_FAULT_ACCESS is not currently used.  LFETCH_ACCESS is
	   used by lfetch.fault */
    PROBER_ACCESS = PROBE_ACCESS | NA_ACCESS | READ_ACCESS,
    PROBEW_ACCESS = PROBE_ACCESS | NA_ACCESS | WRITE_ACCESS,
    PROBERF_ACCESS = PROBER_ACCESS | FAULT_ACCESS,
    PROBEWF_ACCESS = PROBEW_ACCESS | FAULT_ACCESS,
    PROBERW_ACCESS = PROBER_ACCESS | PROBEW_ACCESS | FAULT_ACCESS,
    TPA_ACCESS = TPA_MASK | NA_ACCESS,
    FC_ACCESS = FC_MASK | NA_ACCESS | READ_ACCESS,
    TPA_FC_ACCESS = TPA_MASK | FC_MASK | NA_ACCESS,
    TAK_ACCESS = TAK_MASK | NA_ACCESS
} Accesstype;

typedef union {
    WORD w[2];
    DWORD d;
} CDT;

#if !BYTE_ORDER || !BIG_ENDIAN || !LITTLE_ENDIAN
# error "Endianness is unknown!"
#endif

#define swaph(src)	((((src) >> 8) & 0xff) | (((src) & 0xff) << 8))
#define swapw(src)	(((src) >> 24) | (((src) >> 8) & 0xff00) | \
			 (((src) << 8) & 0xff0000) | ((src) << 24))
#if BYTE_ORDER == BIG_ENDIAN
# define swapd(src)	(((REG)swapw(((CDT *)&src)->w[1]) << 32) | \
 			 swapw(((CDT *)&src)->w[0]))
#else /* BYTE_ORDER == LITTLE_ENDIAN */
# define swapd(src)	(((REG)swapw(((CDT *)&src)->w[0]) << 32) | \
 			 swapw(((CDT *)&src)->w[1]))
#endif /* BYTE_ORDER == LITTLE_ENDIAN */

/* Prototypes */

#if defined __cplusplus
extern "C" {
#endif /* defined __cplusplus */

BOOL memLPF(ADDR adr, Accesstype atype);
int memRd1(ADDR adr, Accesstype atype, BYTE *pval);
int memRd2(ADDR adr, Accesstype atype, HWORD *pval);
int memRd4(ADDR adr, Accesstype atype, WORD *pval);
int memRd8(ADDR adr, Accesstype atype, DWORD *pval);
int memRd16(ADDR adr, Accesstype atype, QWORD *pval);
int specRd1(ADDR adr, Accesstype atype, BYTE *pval);
int specRd2(ADDR adr, Accesstype atype, HWORD *pval);
int specRd4(ADDR adr, Accesstype atype, WORD *pval);
int specRd8(ADDR adr, Accesstype atype, DWORD *pval);
int specRd16(ADDR adr, Accesstype atype, QWORD *pval);
BOOL rseRd(ADDR adr, DWORD *pval);
BOOL vhptRd(ADDR adr, DWORD *ptpa, DWORD *ptar, DWORD *ptag);

BOOL memWrt1(ADDR adr, BYTE val);
BOOL memWrt2(ADDR adr, HWORD val);
BOOL memWrt4(ADDR adr, WORD val);
BOOL memWrt8(ADDR adr, DWORD val);
BOOL memWrt10(ADDR adr, DWORD val1, DWORD val2);
BOOL memWrt16(ADDR adr, DWORD val1, DWORD val2);
BOOL rseWrt(ADDR adr, DWORD val);

BOOL memXchg(ADDR adr, unsigned size, REG val, REG *pval);
BOOL memXchgAdd(ADDR adr, unsigned size, int inc, REG *pval);
BOOL memXchgCond(ADDR adr, unsigned size, REG val, REG *pval);
BOOL memXchg16Cond(ADDR adr, REG val1, REG val2, REG *pval);

BOOL memMRd(ADDR adr, unsigned size, DWORD *pval);
BOOL vhptMRd(ADDR adr, DWORD *ptpa, DWORD *ptar, DWORD *ptag);


BOOL memMWrt(ADDR adr, unsigned size, DWORD val);

BOOL memMIRd(ADDR adr, DWORD *pval);
void memMPRd(ADDR adr, DWORD *pval);
BOOL memBBRd(ADDR adr, BYTE *buf, unsigned nbytes);
BOOL memBBRdP(ADDR adr, BYTE *buf, unsigned nbytes);
void memBBWrt(ADDR adr, const BYTE *buf, unsigned nbytes);
void memBBWrtP(ADDR adr, const BYTE *buf, unsigned nbytes);
void memMPMap(ADDR start, char *hostadr, size_t len);
void memBBWrt_opt(ADDR adr, const BYTE *buf, unsigned nbytes);
void memBBWrt_alloc(ADDR adr, const BYTE *buf, unsigned nbytes);

DWORD *pmemLookup(ADDR adr);
DWORD *pmemLookup_p(ADDR adr);
BOOL pmemMap(ADDR adr, void *hostadr);

BOOL memIAIRd(WORD eip, BYTE *pval, unsigned nbytes);
Status memIAadrRd(ADDR adr, BYTE size, Accesstype acc,
		  ADDR *padr, ADDR *padr2, IADWORD *pval);
Status memIARd(BYTE seg, WORD ofs, BYTE size, Accesstype acc, ADDR *padr,
	       ADDR *padr2, IADWORD *pval);
void memIAWrt(BYTE size, ADDR adr, ADDR adr2, WORD val);

void memFree(ADDR adr);
void memFreeAll(void);

/* Sets the ABI to App-mode or Sys-mode */
void setABI(BOOL val);

/* Displays the Page Table */
size_t pageTableDisplaySize(void);
void getPageTableInfo(char buf[]);

/* Writes a bundle to memory for cproc */
BOOL memMIWrt(ADDR adr, DWORD *pval);

/* Writes up to a page of memory for cproc (data) */
void memMPWrt(ADDR adr, const BYTE *src, unsigned size);

/* Writes up to a page of memory for cproc (instruction) */
void memMPIWrt(ADDR adr, const BYTE *src, unsigned size);

/* Set/clear the data breakpoint bit corresponding to the address for cproc */
void memSetDbptBit(ADDR adr);
void memClrDbptBit(ADDR adr);

/* Reads nbytes of IA memory */
BOOL memMIAIRd(WORD eip, BYTE *pval, unsigned nbytes);

/* Writes nbytes of IA memory */
BOOL memMIAIWrt(ADDR adr, BYTE *pval, unsigned nbytes);

/* Returns the memory contents for cproc in the Meminfo linked list */
BOOL memGet(Meminfo_p *list);

#if !BYTE_ORDER || !BIG_ENDIAN || !LITTLE_ENDIAN
# error "Endianness is unknown!"
#endif

#if BYTE_ORDER == BIG_ENDIAN
# define FIX_BYTEORDER_H(be,x)	(!(be) ? swaph(x) : (x))
# define FIX_BYTEORDER_W(be,x)	(!(be) ? swapw(x) : (x))
# define FIX_BYTEORDER_D(be,x)	(!(be) ? swapd(x) : (x))
# define FIX_BYTEORDER_Q(be,msb,lsb,lo,hi)	\
    do {					\
	DWORD _lo = (lo), _hi = (hi);		\
						\
	if (be) {				\
	    (msb) = _lo;			\
	    (lsb) = _hi;			\
	} else {				\
	    (lsb) = swapd(_lo);			\
	    (msb) = swapd(_hi);			\
	}					\
    } while (0)
#else
# define FIX_BYTEORDER_H(be,x)	((be) ? swaph(x) : (x))
# define FIX_BYTEORDER_W(be,x)	((be) ? swapw(x) : (x))
# define FIX_BYTEORDER_D(be,x)	((be) ? swapd(x) : (x))
# define FIX_BYTEORDER_Q(be,msb,lsb,lo,hi)	\
    do {					\
	DWORD _lo = (lo), _hi = (hi);		\
						\
	if (be) {				\
	    (msb) = swapd(_lo);			\
	    (lsb) = swapd(_hi);			\
	} else {				\
	    (lsb) = _lo;			\
	    (msb) = _hi;			\
	}					\
    } while (0)
#endif

#if defined __cplusplus
}
#endif /* defined __cplusplus */

#endif /* _SKI_MEMORY_H_ */
