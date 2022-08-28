/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Core Header File
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
#ifndef _SKI_LIBCORE_H
#define _SKI_LIBCORE_H

#include "std.h"
#include "types.h"

/* This requires std.h and types.h */

#ifdef NEW_MP
#define MAX_NGRS	2048
#define NGRS	(GR_STK_BASE + N_STACK_PHYS)
#define NBGRS	 16
#define NPRS	 64
#define NFRS	128
#define NBRS	  8
#define NARS	128

#define NCRS	128
#define NRRS	  8
#define NPKRS	 16
#define NDBRS	 16
#define NIBRS	 16
#define NPMS	 64
#define NMSRS	4096
#define NCPUIDS	  5

#define BGR_BASE	16
#define GR_STK_BASE	32
#define N_STACK_PHYS	n_stack_phys
#define N_STACK_VIRT	96
#define GR_ROT_SIZE	N_STACK_VIRT
#define PR_ROT_BASE	16
#define PR_ROT_SIZE	(NPRS-PR_ROT_BASE)
#define FR_ROT_BASE	32
#define FR_ROT_SIZE	(NFRS-FR_ROT_BASE)
#endif

/****************************************/
/* These functions return machine state */
/****************************************/

#if defined __cplusplus
extern "C" {
#endif /* defined __cplusplus */

/* Returns the ip for cproc */
REG ipGet(int cproc);

/* Returns the psr for cproc */
REG psrGet(int cproc);

/* Returns the virtual GR[i] for cproc */
REG grGet(int cproc, int i);

/* Returns the virtual GR[i].nat for cproc */
REG grNatGet(int cproc, int i);

/* Returns the physical GR[i] for cproc */
REG phyGrGet(int cproc, int i);

/* Returns the physical GR[i].nat for cproc */
REG phyGrNatGet(int cproc, int i);

/* Returns the double value of virtual FR[i] for cproc */
double frGet(int cproc, int i);

/* Returns the sign bit of virtual FR[i] for cproc */
REG frSignGet(int cproc, int i);

/* Returns the exponent field of virtual FR[i] for cproc */
REG frExpGet(int cproc, int i);

/* Returns the mantissa field of virtual FR[i] for cproc */
REG frMantGet(int cproc, int i);

/* Returns the sign bit of physical FR[i] for cproc */
REG phyFrSignGet(int cproc, int i);

/* Returns the exponent field of physical FR[i] for cproc */
REG phyFrExpGet(int cproc, int i);

/* Returns the mantissa field of physical FR[i] for cproc */
REG phyFrMantGet(int cproc, int i);

/* Returns the virtual PR[i] for cproc */
REG prGet(int cproc, int i);

/* Returns the physical PR[i] for cproc */
BOOL phyPrGet(int cproc, int i);

/* Returns BR[i] for cproc */
REG brGet(int cproc, int i);

/* Returns AR[i] for cproc */
REG arGet(int cproc, int i);

/* Returns the CFM for cproc */
REG cfmGet(int cproc);

/* Returns the SOF for cproc */
REG sofGet(int cproc);

/* Returns the SOL for cproc */
REG solGet(int cproc);

/* Returns the SOR for cproc */
REG sorGet(int cproc);

/* Returns the RRB.gr for cproc */
REG rrbgGet(int cproc);

/* Returns the RRB.fr for cproc */
REG rrbfGet(int cproc);

/* Returns the RRB.pr for cproc */
REG rrbpGet(int cproc);

/* Returns the BOL for cproc */
REG bolGet(int cproc);

/* Returns the number of dirty registers for cproc */
REG dirtyGet(int cproc);

/* Returns the number of dirty NaT registers for cproc */
REG dirtyNatGet(int cproc);

/* Returns the number of clean registers for cproc */
REG cleanGet(int cproc);

/* Returns the number of clean NaT registers for cproc */
REG cleanNatGet(int cproc);

/* Returns the number of invalid registers for cproc */
REG invalidGet(int cproc);

/* Returns the current frame load enable for cproc */
BOOL cfleGet(int cproc);

/* Returns CR[i] for cproc */
REG crGet(int cproc, int i);

/* Returns the banked GR[i] for cproc */
REG bkrGet(int cproc, int i);

/* Returns the banked GR[i].nat for cproc */
REG bkrNatGet(int cproc, int i);

/* Returns RR[i] for cproc */
REG rrGet(int cproc, int i);

/* Returns PKR[i] for cproc */
REG pkrGet(int cproc, int i);

/* Returns DBR[i] for cproc */
REG dbrGet(int cproc, int i);

/* Returns IBR[i] for cproc */
REG ibrGet(int cproc, int i);

/* Returns PMC[i] for cproc */
REG pmcGet(int cproc, int i);

/* Returns PMD[i] for cproc */
REG pmdGet(int cproc, int i);

/* Returns CPUID[i] for cproc */
REG cpuidGet(int cproc, int i);

/* Returns the memory contents for cproc in the Meminfo linked list */
BOOL memGet(Meminfo_p *list);


/***************************************/
/* These functions set machine state   */
/* (the return value indicates whether */
/*  the setting was successful)        */
/***************************************/

/* Sets the ip for cproc */
BOOL ipSet(int cproc, REG val);

/* Sets the psr for cproc */
BOOL psrSet(int cproc, REG val);

/* Sets the virtual GR[i] for cproc */
BOOL grSet(int cproc, int i, REG val);

/* Sets the virtual GR[i].nat for cproc */
BOOL grNatSet(int cproc, int i, BOOL nat);

/* Sets the physical GR[i] for cproc */
BOOL phyGrSet(int cproc, int i, REG val);

/* Sets the physical GR[i].nat for cproc */
BOOL phyGrNatSet(int cproc, int i, BOOL nat);

/* Set the value of virtual FR[i] for cproc */
BOOL frSet(int cproc, int i, double val);

/* Sets the sign bit of virtual FR[i] for cproc */
BOOL frSignSet(int cproc, int i, REG val);

/* Sets the exponent field of virtual FR[i] for cproc */
BOOL frExpSet(int cproc, int i, WORD exp);

/* Sets the mantissa field of virtual FR[i] for cproc */
BOOL frMantSet(int cproc, int i, DWORD mant);

/* Sets the sign bit of physical FR[i] for cproc */
BOOL phyFrSignSet(int cproc, int i, BYTE sign);

/* Sets the exponent field of physical FR[i] for cproc */
BOOL phyFrExpSet(int cproc, int i, WORD exp);

/* Sets the mantissa field of physical FR[i] for cproc */
BOOL phyFrMantSet(int cproc, int i, DWORD mant);

/* Sets the virtual PR[i] for cproc */
BOOL prSet(int cproc, int i, BOOL val);

/* Sets the physical PR[i] for cproc */
BOOL phyPrSet(int cproc, int i, BOOL val);

/* Sets BR[i] for cproc */
BOOL brSet(int cproc, int i, REG val);

/* Sets AR[i] for cproc */
BOOL arSet(int cproc, int i, REG val);

/* Sets the CFM for cproc */
void cfmSet(int cproc, REG val);

/* Sets the RRB.gr for cproc */
BOOL rrbgSet(int cproc, REG val);

/* Sets the RRB.fr for cproc */
BOOL rrbfSet(int cproc, REG val);

/* Sets the RRB.pr for cproc */
BOOL rrbpSet(int cproc, REG val);

/* Sets the BOL for cproc */
void bolSet(int cproc, unsigned val);

/* Sets the number of dirty registers for cproc */
void dirtySet(int cproc, int val);

/* Sets the number of dirty NaT registers for cproc */
void dirtyNatSet(int cproc, int val);

/* Sets the number of clean registers for cproc */
void cleanSet(int cproc, int val);

/* Sets the number of clean NaT registers for cproc */
void cleanNatSet(int cproc, int val);

/* Sets the number of invalid registers for cproc */
void invalidSet(int cproc, int val);

/* Sets the current frame load enable for cproc */
void cfleSet(int cproc, BOOL val);

/* Sets CR[i] for cproc */
BOOL crSet(int cproc, int i, REG val);

/* Sets the banked GR[i] for cproc */
BOOL bkrSet(int cproc, int i, REG val);

/* Sets the banked GR[i].nat for cproc */
BOOL bkrNatSet(int cproc, int i, BOOL val);

/* Sets RR[i] for cproc */
BOOL rrSet(int cproc, int i, REG val);

/* Sets RR[i] for cproc */
BOOL pkrSet(int cproc, int i, REG val);

/* Sets DBR[i] for cproc */
BOOL dbrSet(int cproc, int i, REG val);

/* Sets IBR[i] for cproc */
BOOL ibrSet(int cproc, int i, REG val);

/* Sets PMC[i] for cproc */
BOOL pmcSet(int cproc, int i, REG val);

/* Sets PMD[i] for cproc */
BOOL pmdSet(int cproc, int i, REG val);

/* Sets 8 bytes of memory for cproc starting at adr */
void memSet(ADDR adr, REG val);

/* Sets a page of memory for cproc to all zeroes */
void memZerosPageSet(ADDR adr);

/*************************************/
/* These are miscellaneous functions */
/*************************************/

/* Flush the RSE prior to stack unwinding */
void unwindFlush(void);

/* Returns the total number of instructions simulated so far */
CTR getTotalInsts(void);

/* Sets the total number of instructions simulated so far */
void setTotalInsts(CTR val);

/* Returns the total number of faults simulated so far */
CTR getTotalFaults(void);

/* Sets the total number of faults simulated so far */
void setTotalFaults(CTR val);

/* Returns the total number of virtual cycles simulated so far */
CTR getTotalCycles(void);

/* Sets the total number of virtual cycles simulated so far */
void setTotalCycles(CTR val);

/* Returns the maximum SP value */
REG getMaxSP(void);

/* Sets the maximum SP value */
void setMaxSP(REG val);

#ifndef _SKI_DECODER_H
#ifndef MAX_DAS_LEN
typedef struct {
    DWORD left, right;
} Bundle;
#endif
#endif

/* Splits a bundle into 3 instructions and the template/SB field */
void bparts(Bundle *bptr, BYTE *templSB, DWORD slot[]);

/* Replaces an instruction with a BREAK instruction, returning the original */
void bptReplace(Bundle *bptr, ADDR *adrptr, DWORD *orig);

/* Restores the original instruction */
void bptRestore(Bundle *bptr, ADDR adr, DWORD orig);

/* Replaces an IA instruction with a int 3 instruction */
void iabptReplace(ADDR adr);

/* Restores an IA instruction */
void iabptRestore(ADDR adr);

/* Replaces an instruction with a new value */
#ifndef _ENCODER_H
struct encoded_inst;
#endif
BOOL instrReplace(Bundle *bptr, ADDR badr, unsigned slot,
		  struct encoded_inst *inst);

/* Replaces a bundle's template with a new value */
void templReplace(Bundle *bptr, ADDR adr, BYTE _new);

/* Sets internal state that is derived from other architected state */
void setDerivedState(BOOL setEIP);

/* Sets the intrsim flag */
void setIntrsim(BOOL val);

/* Returns the lp64 flag for cproc (indicating the executable's data model) */
BOOL getLp64(int cproc);

/* Sets the lp64 flag for cproc */
void setLp64(int cproc, BOOL val);

/* Sets up a file descriptor mapping from fd1 to fd2 */
void setFdmap(int fd1, int fd2);

/* Simulate for up to cnt instructions */
BOOL stepIt_loop(CTR cnt);
BOOL runIt_loopX(CTR cnt);

/* Simulate until stopped */
void runIt_loop(void);

/* Enable/Disable tracing */
BOOL setTraceEnb(BOOL enb);

/* Initialize the virtual cycle counter for tracing */
void initTraceVcy(void);

/* Initialize trace flags */
void initTraceFlags(void);

/* Generate a trace discontinuity */
void traceDisc(void);

/* Sets the virtual cycle counter for tracing (only if tracing enabled) */
void traceVcyEnb(void);

/* Currently unused */
void setUserMode(BOOL user_mode);

/* Gets the execution message */
char *getExecMsg(void);

extern unsigned long page_size;
extern unsigned int log2_page_size;
extern DWORD page_mask;

#define page_base(addr)		((addr) & page_mask)
#define page_align(addr)	(((addr) + ~page_mask) & page_mask)
#define page_offset(addr)	((addr) & ~page_mask)

/* Prototypes */

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

/* Returns the exited flag */
REG getExited(void);

/* Sets the exited flag */
void setExited(BOOL val);

/* Returns the heap address for cproc */
ADDR heapGet(int cproc);

/* Sets the heap address for cproc */
void heapSet(int cproc, ADDR val);

/* Displays the GR map */
void getGrMapInfo(char buf[]);

/* Displays the Page Table */
size_t pageTableDisplaySize(void);
void getPageTableInfo(char buf[]);

/* Translates the virtual address using the DTLB */
BOOL dataTranslate(ADDR va, BOOL rd, ADDR *pa);

/* Translates the virtual address using the ITLB */
BOOL instTranslate(ADDR va, ADDR *pa);

/* Initializes basic machine state for cproc */
void initState(int cproc);

/* Enables the instruction bucket counting */
void setIcntEnb(void);

/* Initializes application-specific state for cproc */
void initAppState(int cproc);

/* Sets the ABI to App-mode or Sys-mode */
void setABI(BOOL val);

/* Returns a string indicating the implementation */
char *getImplStr(void);

/* Returns the next static ip after the specified address */
ADDR nextIp(ADDR a);

/* Returns the number of B units in the template for the specified bundle */
int brCnt(ADDR a);

/* Returns the type of branch instruction if any at the specified address */
BRTYPE brType(ADDR a);

/* Returns the taken and fallthru addresses for an IP-relative branch at the
   specified address */
void brTargets(ADDR a, ADDR *taken, ADDR *fallthru);

/* Returns whether the branch at the specified address will be taken and
   the address of the next dynamic instruction */
BOOL brTaken(ADDR a, ADDR *next);

void initSysState(void);

#if defined __cplusplus
}
#endif /* defined __cplusplus */

#endif	/* _SKI_LIBCORE_H */
