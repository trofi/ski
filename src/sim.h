/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Simulator Header
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
#ifndef _SKI_SIM_H_
#define _SKI_SIM_H_

#include "libdas.h"
#include "std.h"
#include "types.h"

/* the bits of Status encode the following: */
#define ST_STOP		0x01	/* must be low-order bit */
#define ST_IP_INC	0x02
#define ST_PSR_CLR	0x04
#define ST_INST_COUNT	0x08
#define ST_TRAP		0x10
#define ST_CHECK        0x20
typedef enum {
    StSuccess=     ST_INST_COUNT|ST_PSR_CLR|ST_IP_INC,
    StSuccessStop= ST_INST_COUNT|ST_PSR_CLR|ST_IP_INC|ST_STOP,
    StTakenBr=     ST_INST_COUNT|ST_PSR_CLR          |ST_STOP,
    StTrap=ST_TRAP|ST_INST_COUNT|ST_PSR_CLR          |ST_STOP,
    StFault=                                          ST_STOP,
    StRFI=         ST_INST_COUNT                     |ST_STOP,
    StCheck=ST_CHECK
} Status;

#ifdef NEW_MP
typedef struct {
    unsigned preInst_;
} prempstate;
extern prempstate preMPstate[];
#define preInst preMPstate[curPid].preInst_
#else
extern unsigned preInst;
#endif
/* the bits of preInst encode the following: */
#define PRE_CFLE	1
#define PRE_ICNT	2
#define PRE_TRACE	4
#define PRE_USERINT	8
#define PRE_SIGNAL  16

#define ILL_INST 2


/* Generate "n" 1s for 1 <= n <= 64 */
#define ONES(n)		(~0ULL >> (64-(n)))

#define SIM_ABS(n)	((n) >= 0 ? (n) : -(n))

#if !defined MIN
# define MIN(n,m)	((n) <= (m) ? (n) : (m))
#endif /* !defined MIN */
#if !defined MAX
# define MAX(n,m)	((n) >= (m) ? (n) : (m))
#endif /* !defined MAX */

#define AVG(n)		((n) >> 1 | ((n) & 1))
#define AVGRAZ(n)	(((n) + 1) >> 1)

struct instinfo;

typedef Status		(*PCF)(struct instinfo *);

typedef struct {
    WORD cnt, ptcnt, pfcnt, p0cnt;
} ICNTS;

typedef struct CT_t CT;
typedef struct instinfo INSTINFO;

#ifdef __LP64
/* If sizeof(INSTINFO) is not a power of 2, use info->ipos */
# define USE_IPOS 1
#endif


#ifdef USE_IPOS
# define IPOS(info) ((info)->ipos)
#else
# define IPOS(info) (((info)-&(info)->ct->instCache[0])<<2)
#endif

#define IPTR(info) ((info)->ct->ipn + (IPOS(info)))
#define ICNT(info) (info->ct->cntCache[IPOS(info)>>2])

#define NEXT_PAGE_IPTR(info) ((info)->ct->ipn + 0x1000ULL)
#define END_OF_PAGE(info) (info == NULL)

struct instinfo {
    DWORD immed64;
    BYTE extrainfo[8];

    PCF combFn;
    INSTINFO *next;
    CT *ct;

    BYTE pgrtgt, pgrr2, pgrr3;
    BYTE delta  : 4;
    BYTE stop   : 1;
    BYTE samepg : 1;
    BYTE unused : 2;

#ifdef USE_IPOS
    WORD ipos;
#endif

};

struct CT_t {
    INSTINFO instCache[1024];
    ICNTS* cntCache[1024];
    ADDR ipn, tag, iaTag;
};


typedef Status		(*PEF)(INSTINFO *);
typedef Status		(*PRF)(INSTINFO *);
typedef void		(*PWF)(INSTINFO *);

#define SRC1	src1
#define SRC2	src2
#define SRC3	src3
#define SRCNAT1	srcnat1
#define SRCNAT2	srcnat2
#define SRCNAT3	srcnat3
#define FSRC1	fsrc1
#define FSRC2	fsrc2
#define FSRC3	fsrc3
#define DST1	dst1
#define DST2	dst2
#define DSTNAT1	dstnat1
#define DSTNAT2	dstnat2
#define FDST1	fdst1
#define FDST2	fdst2
#define BDST1	bdst1
#define PDST1	pdst1
#define CMPRES1	cmpres1
#define CMPRES2	cmpres2
#define QUAL	qual
#define POS	posx
#define LEN	lenx
#define CNT	cntx
#define MHT	mhtypex
#define SOF	sofx
#define SOIL	soilx
#define SOR	sorx
#define ARG0	arg0
#define ARG1	arg1
#define ARG2	arg2
#define ARG3	arg3
#define ARG4	arg4
#define ARG5	arg5
#define ARG6	arg6
#define ARG7	arg7
#define SC_NUM	syscallno
#define SC_RET	scRetval
#define SC_STAT	scStatus
#define CPL	cpl
#define TRAPS	traps
#define CTRLS	ctrls
#define ATYPE	atype

/* eclipse - define a unique value for use in break instructions to
 * indicate a transition from emulated back to native code.  The range
 * for application breakpoints is 0x040000 - 0x07FFFF. Added a flag to
 * signal when one of these break instructions have been reached.
 * Test the flag while catching SIGILL. */
#define BREAK_EMUL_BRIDGE 0x42319
extern BOOL emul_bridge_signaled;

typedef enum {RUN_EM, RUN_IA, STEP} ExecutionMode;

extern ExecutionMode executionMode;
extern CTR total_insts, total_cycles, total_faults;
extern unsigned int mips;

/* Enables the instruction bucket counting */
void setIcntEnb(void);
/* Initializes basic machine state for cproc */
void initState(int cproc);
/* Initializes application-specific state for cproc */
void initAppState(int cproc);
void initSysState(void);

void alatInit(void);
int alat_cmp(BOOL fpreg, int rega, BOOL clearit);
void alat_inval_all_entries(void);
void alat_inval_multiple_entries(ADDR pa, unsigned size);
void alat_inval_single_entry(BOOL fpreg, int rega);
void alat_write(BOOL fpreg, int rega, ADDR pa, unsigned size);
BOOL brCall(ADDR a);
void clearPdecode(ADDR adr, unsigned size);
ICNTS getICnts(unsigned i);
void gotoAlarm(void);
Status iCycleApp(void);
Status iCycleSys(void);
Status iAiCycle(void);
void progStop(const char *, ...);
void progExit(const char *, ...);
void progPause(void);
void resetState(int proc);

void sendIPI(int, int, int);
extern INSTINFO *setIcp(void);
void switchBanks(void);

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

/* Sets the intrsim flag */
void setIntrsim(BOOL val);

/* Simulate for up to cnt instructions */
BOOL stepIt_loop(CTR cnt);
BOOL runIt_loopX(CTR cnt);

/* Simulate until stopped */
void runIt_loop(void);

/* Gets the execution message */
char *getExecMsg(void);

/* Sets internal state that is derived from other architected state */
void setDerivedState(BOOL setEIP);

/* Replaces an instruction with a BREAK instruction, returning the original */
void bptReplace(Bundle *bptr, ADDR *adrptr, DWORD *orig);

/* Restores the original instruction */
void bptRestore(Bundle *bptr, ADDR adr, DWORD orig);

/* Replaces an IA instruction with a int 3 instruction */
void iabptReplace(ADDR adr);

/* Restores an IA instruction */
void iabptRestore(ADDR adr);

/* Splits a bundle into 3 instructions and the template/SB field */
void bparts(Bundle *bptr, BYTE *templSB, DWORD slot[]);

/* Replaces an instruction with a new value */
struct encoded_inst;
BOOL instrReplace(Bundle *bptr, ADDR badr, unsigned slot,
		  struct encoded_inst *inst);

/* Replaces a bundle's template with a new value */
void templReplace(Bundle *bptr, ADDR adr, BYTE _new);

#endif /* _SKI_SIM_H_ */
