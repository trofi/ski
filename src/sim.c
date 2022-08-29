/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Main Simulation Routines
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

#include <errno.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>

/* XXX should list systems that need the header */
#if !defined(linux) && !defined HPUX9 && !defined(__FreeBSD__)
#include <dl.h>
#endif
#ifdef NEW_MP
#include "decoder.h"
#include "encoder.h"
#endif
#include "std.h"
#include "bits.h"
#include "types.h"
#include "fields.h"
#include "state.h"
#include "sim.h"
#include "simmem.h"
#include "tlb.h"
#include "libtrace.h"
#include "trace.h"
#include "interruption.h"
#include "instr.h"
#ifndef NEW_MP
#include "decoder.h"
#include "encoder.h"
#endif
#include "libcore.h"
#include "ssc.h"
#include "ia_types.h"
#include "ia_state.h"
#include "ia_decoder.h"
#include "ia_exec.h"
#include "EMInst.h"
#include "instinfo.h"
#include "icnt_core.gen.h"
#include "os_support.h"

#define ALARM

#ifdef __linux__
extern void signal_invoke_handler(int in_syscall);
extern void signal_return(void);
extern int  signal_pending(void);
#endif

/* eclipse - added emulation bridge flag */
BOOL emul_bridge_signaled = NO;

BOOL intrsim = NO, extint = NO, kybdint = NO;
#ifdef NEW_MP
#define TIMESLICE	1
extern unsigned mips;
static CTR ipts;	/* instructions per time slice */
static CTR instsLeft;	/* max. instructions left in current time slice */
prempstate preMPstate[4];
REG os_boot_rendez_ip;
REG os_boot_rendez_gp;
#else
unsigned preInst;
#endif

static BOOL cntlC = NO;

#if 0
#define SKIP_NOPS
#define USE_PSEUDOS
#define FD_CNT
#define COUNT_GR_ACCESSES
#endif

CTR total_insts = 0, total_cycles = 0, total_faults = 0;
unsigned int mips = 500;
ExecutionMode executionMode;
#ifndef __linux__
static ADDR alarm_rp = ~0ULL;
#endif
extern CTR alarm_insts, alarm_interval;
extern CTR prof_insts;
extern ADDR sigHdlr;

/* XXX - move these elsewhere? */
WORD maxNats;
BOOL natStats;

#ifdef CALCINFO
unsigned calcInfo = 0;
#endif
#ifdef COUNT_GR_ACCESSES
CTR numgrrds[NGRS] = {0}, numgrwrts[NGRS] = {0};
#endif

#ifdef TRACE_DAVIDM
int davidm_trace = 1, wr = 0;
static struct {
	REG ip;
	REG pfs;
} tracebuf[1024*1024];
#endif /* TRACE_DAVIDM */

static BOOL icntEnb = NO;

#if !defined __linux__
static unsigned traceSve;	/* For timer interrupt support */
#endif /* !defined __linux__ */

#define IC_LEN 9
#define IC_START (52-IC_LEN)
#define NUM_INSTCACHES (1<<IC_LEN)
#define CTTAGMASK	(~(ADDR)0xFFF)

extern PCF instFetchDecodeFP;
extern PCF illCombFP, illQpCombFP;

extern void profCnt (void);

extern void alat_inval_single_entry(BOOL fpreg, int rega);
extern void alat_write(BOOL fpreg, int rega, ADDR pa, unsigned size);
extern int alat_cmp(BOOL fpreg, int rega, BOOL clearit);
extern void alat_inval_multiple_entries(ADDR pa, unsigned size);
extern void alat_inval_all_entries(void);


static void iCycleAppLoop(INSTINFO* icp);
static void iCycleSysLoop(void);

/*##################### Local Variables ####################################*/

static char execMsg[100];
static jmp_buf jmpenv;

#ifdef NEW_MP
static struct {
    CT CacheTbl[NUM_INSTCACHES];
} ctMPstate[4];
#define CacheTbl ctMPstate[curPid].CacheTbl
#else
static CT CacheTbl[NUM_INSTCACHES];
#endif
#ifdef GET_INFO
static INSTINFO ict[NUM_INSTCACHES*1024];
#endif

#ifndef NEW_MP
INSTINFO* icp = NULL;
#endif
extern BYTE iAmode;
extern IAWORD pspSeg;

int rse_store(void);
int rse_load(void);

/*#####################  Macros  ###########################################*/

#define COPY_TO_INFO(from, to) \
    (info##to->combFn = instrs[instr[from].instID].combFn, \
     ICNT(info##to) = &instrs[instr[from].instID].cnt, \
     info##to->stop = t->slot[from].stop)

/*##################### Functions ##########################################*/

#ifdef NEW_MP
int active(void)
{
    return (ip != NULL);
}

void sendIPI(int id, int eid, int vector)
{
    int cpu = ((id & 1) << 1) | (eid & 1);

#ifdef DEBUG
    printf("Sending IPI to cpu %d (LAPIC=%d:%d), vector=%d\n", cpu, id, eid,
	vector);
#endif

    /* XXX gratuitous hardcoding of 255 as the AP wakeup vector. */
    if (vector == 255) {
	if (mpState[cpu].ip_ == 0) {
	    mpState[cpu].ip_ = os_boot_rendez_ip;
	    mpState[cpu].grs_[1].val = os_boot_rendez_gp;
	}
    }
    pendIrpt(vector);
#error
    /* pendIrpt ill not be seend by SysLoopLite. Need to
       propagate ST_CHECK back to the combFn */
}
#endif

#ifdef EXTRA_CHECKS
BOOL checkStack(void)
{
    BOOL error = NO;
    int oknat;

    if (invalid < 0 || clean < 0 || cleanNat < 0 || dirty < 0 || dirtyNat < 0)
	error = YES;
    if (invalid > N_STACK_PHYS || clean > N_STACK_PHYS || dirty > N_STACK_PHYS)
	error = YES;
    oknat = dirty/63;
    if (dirtyNat != oknat && dirtyNat != oknat+1)
	error = YES;
    oknat = clean/63;
    if (cleanNat != oknat && cleanNat != oknat+1)
	error = YES;
    if (invalid + sof + dirty + clean != N_STACK_PHYS)
	error = YES;
    if ((dirty + dirtyNat) * 8 != (BSP - BSPST))
	error = YES;
    if (BSP < 0x1000ULL || BSP > 0xFFF0000000000000ULL)
	error = YES;
    if (error)
	progExit("RSE error @ %016llx (%llu): f=%d, i=%d, c=%d/%d, d=%d/%d\n",
		 ip, total_insts, sof,
		 invalid, clean, cleanNat, dirty, dirtyNat);
    return !error;
}
#endif

#ifdef RSE_DEBUG
static FILE *rsedb;
rseDB(char *s)
{
    static int first = 1;

    if (first) {
	rsedb = fopen("rsedb", "w");
	first = 0;
    }
    if (total_faults > 20000)
	return;

    fprintf(rsedb, "%s @ %llu\n", s, total_insts);
}
#endif

static void updateTmrs(void)
{
    if (++ITC == ITM && !IMASK(ITV))
	pendIrpt(ITV);
    if (sscPend && --sscPend->irptcnt <= 0) {
	pendSscIrpt(sscPend->type);
	sscPend = sscPend->next;
    }
}

void switchBanks(void)
{
    int i;

    for (i = 0; i < NBGRS; i++) {
	GREG tmp;

	tmp = bankedGrs[i];
	bankedGrs[i] = grs[BGR_BASE+i];
	grs[BGR_BASE+i] = tmp;
    }
}

#ifdef FD_CNT
static int fdCnt = 0;
#endif

#include "combfns.gen.h"

#ifdef USE_PSEUDOS
/* XXX - add this prototype to combfns.h via combfns.awk */
Status swizzleComb(INSTINFO *);

static void chkPseudo(InstID id, INSTINFO *info)
{
    if (info->combFn == &adds_r1_imm14_r3Comb && r3 == 0)
	info->combFn = movl_r1_imm64Comb;
    else if (info->combFn == &adds_r1_imm14_r3Comb && !imm64)
	info->combFn = movGrComb;
    else if (id == EM_ADDP4_R1_IMM14_R3 && !imm64)
	info->combFn = swizzleComb;
}
#endif

static BOOL instDecode(ADDR adr)
{
    ADDR pa;
    INSTINFO* iCp;
    unsigned i;
    DWORD *pagebuf;
    TemplateInfoPtr t;
    DecodedInstr instr[SLOTS_PER_BUNDLE];
    INSTINFO *info0, *info1, *info2;
#ifdef SKIP_NOPS
    static InstID nopInsts[No_Unit] = {
	EM_NOP_I_IMM21,
	EM_NOP_M_IMM21,
	EM_NOP_F_IMM21,
	EM_NOP_B_IMM21
    };
#endif

    pa = adr & ~(ADDR)0xF;

    i = BitfX(pa,52,10);
    /* XXX - What if page is not mapped? */
    if (!(pagebuf = pmemLookup_p(pa & CTTAGMASK))) {
	return NO;
    }
    t = bundle_decode((BundlePtr)&pagebuf[i>>1], instr, 0);
    iCp = &CacheTbl[BitfX(pa,IC_START,IC_LEN)].instCache[i];
    info0 = &iCp[0];
    info1 = &iCp[1];
    info2 = &iCp[2];

    if (instrs[instr[0].instID].flags & EM_FLAG_SLOT2_ONLY)
	instr[0].instID = EM_ILLEGALOP;
    if (instrs[instr[0].instID].flags & EM_FLAG_LAST_IN_INSTRUCTION_GROUP
	&& !t->slot[0].stop)
	instr[0].instID = EM_ILLEGALOP;
    instrs[instr[0].instID].pdecFn(instr[0].instrBits, info0);
    COPY_TO_INFO(0, 0);
    if (instrs[instr[0].instID].flags & EM_FLAG_CHECK_BASE_EQ_DST &&
	info0->extrainfo[1] == info0->extrainfo[3])
	info0->combFn = illQpCombFP;
    if (instrs[instr[0].instID].flags & EM_FLAG_CHECK_SAME_DSTS &&
        info0->extrainfo[1] == info0->extrainfo[4]) {
	if (instrs[instr[0].instID].flags & EM_FLAG_CTYPE_UNC)
	    info0->combFn = illCombFP;
	else
	    info0->combFn = illQpCombFP;
    }
#ifdef USE_PSEUDOS
    chkPseudo(instr[0].instID, info0);
#endif
    info0->delta = 1;
    info0->next = &iCp[1];

    if (t->slot[1].unit == L_Unit) {	/* MLX template */
	instrs[instr[1].instID].pdecFn(instr[1].instrBits, info1);
	instrs[instr[2].instID].pdecFn(instr[2].instrBits, info1);
	COPY_TO_INFO(2, 1);
	info1->delta = 3;
	info1->next = i == 1020 ? NULL : &iCp[4];

	/* To raise illegal op fault if rfi'ing to the middle of an MLX */
	instr[2].instID = EM_ILLEGALOP;
    } else {				/* not MLX */
	if (instrs[instr[1].instID].flags & EM_FLAG_SLOT2_ONLY)
	    instr[1].instID = EM_ILLEGALOP;
	if (instrs[instr[1].instID].flags & EM_FLAG_LAST_IN_INSTRUCTION_GROUP
	    && !t->slot[1].stop)
	    instr[1].instID = EM_ILLEGALOP;
	if (instrs[instr[2].instID].flags & EM_FLAG_LAST_IN_INSTRUCTION_GROUP
	    && !t->slot[2].stop)
	    instr[2].instID = EM_ILLEGALOP;
	instrs[instr[1].instID].pdecFn(instr[1].instrBits, info1);
	COPY_TO_INFO(1, 1);
	if (instrs[instr[1].instID].flags & EM_FLAG_CHECK_BASE_EQ_DST &&
	    info1->extrainfo[1] == info1->extrainfo[3])
	    info1->combFn = illQpCombFP;
	if (instrs[instr[1].instID].flags & EM_FLAG_CHECK_SAME_DSTS &&
	    info1->extrainfo[1] == info1->extrainfo[4]) {
	    if (instrs[instr[1].instID].flags & EM_FLAG_CTYPE_UNC)
		info1->combFn = illCombFP;
	    else
		info1->combFn = illQpCombFP;
    }
#ifdef USE_PSEUDOS
	chkPseudo(instr[1].instID, info1);
#endif
	info1->delta = 1;
	info1->next = &iCp[2];
#ifdef SKIP_NOPS
	if (instr[1].instID == nopInsts[t->slot[1].unit])
{
	    info0->delta = 2;
	info0->next = &iCp[2];
}
#endif
    }
    instrs[instr[2].instID].pdecFn(instr[2].instrBits, info2);
    COPY_TO_INFO(2, 2);
    if (instrs[instr[2].instID].flags & EM_FLAG_CHECK_SAME_DSTS &&
        info2->extrainfo[1] == info2->extrainfo[4]) {
	if (instrs[instr[2].instID].flags & EM_FLAG_CTYPE_UNC)
	    info2->combFn = illCombFP;
	else
	    info2->combFn = illQpCombFP;
    }
#ifdef USE_PSEUDOS
    chkPseudo(instr[2].instID, info2);
#endif
    info2->delta = 2;
    info2->next = i == 1020 ? NULL : &iCp[4];

#ifdef SKIP_NOPS
    if (instr[2].instID == nopInsts[t->slot[2].unit]) {
	if (info0->delta == 2)
{
	    info0->delta = (i == 1020 ? 3 : 4);
	    info0->next = i == 1020 ? &iCp[3] : &iCp[4];
}
	info1->delta = (i == 1020 ? 2 : 3);
	info1->next = i == 1020 ? &iCp[3] : &iCp[4];
    }
#endif
    return YES;
}

static void incrInstCnts(INSTINFO *info)
{
    ICNTS *cnt = ICNT(info);
    (cnt->cnt)++;
    if (info->qpred)
	if (PrRd(info->qpred))
	    (cnt->ptcnt)++;
	else
	    (cnt->pfcnt)++;
    else
	(cnt->p0cnt)++;
}

ICNTS getICnts(unsigned i)
{
    return *icnt[i].cnt;
}

Status instFetchDecode(INSTINFO *info)
{
#ifdef FD_CNT
    fdCnt++;
    fprintf(stderr, "instFetchDecode call #%-5d (%-7llu): %016llx\n",
	    fdCnt, total_insts, imm64);
#endif
    if (!instDecode(info->ct->tag + imm64)) {
	return StFault;
    }
    if (icntEnb)
	incrInstCnts(info);
    if (traceEnb) {
	WRT_CAPSULE_TR();
	WRT_BUNDLE_TR();
	WRT_IOFFSET_TR();
    }
    return info->combFn(info);
}

/* adr must be the address used to get to this page */
static void fillinDecodePage(CT *ct, ADDR adr)
{
    unsigned i;

    for (i = 0; i < 1024; i++) {
	ct->instCache[i].combFn = instFetchDecodeFP;
	ct->instCache[i].immed64 = i << 2;
#ifdef USE_IPOS
	ct->instCache[i].ipos = i << 2;
#endif /* USE_IPOS */
	ct->instCache[i].ct = ct;
    }
}

Status iAinstFetchDecode(IAinstInfoPtr info)
{
    if (!ia_decode(EIP, info))
	return StFault;

    return info->execFn(info);
}

static BOOL mandatoryRSEloads(void)
{
    int i;
#ifdef EXTRA_CHECKS
    int xdirty = -1, xdirtynat = -1;
#endif

    do {
#ifdef EXTRA_CHECKS
	if (dirty > 0 || dirtyNat > 0)
	    progExit("dirty or dirtyNat is > 0\n");
	xdirty = dirty;
	xdirtynat = dirtyNat;
#endif
#ifdef RSE_DEBUG
rseDB("mandatory RSE load");
#endif
	if ((i = rse_load()) == -1) {
	    cfle = NO;
	    preInst &= ~PRE_CFLE;
	    return NO;
	}
	if (i) {
	    clean--;
	    dirty++;
	} else {
	    cleanNat--;
	    dirtyNat++;
	}
	BSPST -= 8;
	if (!dirty && !dirtyNat) {
	    cfle = NO;
	    preInst &= ~PRE_CFLE;
	}
    } while (cfle);
    return YES;
}

#if !defined __linux__

static struct {
    GREG allgrs[MAX_NGRS];
    REG allbrs[NBRS];
    BOOL allprs[NPRS];
    FREG allfrs[NFRS];
    REG pfs;
    unsigned slot;
} sigstate;

void gotoAlarm(void)
{
    int i;
    INSTINFO info;

    for (i = 1; i < NPRS; i++)
	sigstate.allprs[i] = PrRd(i);
    for (i = 0; i < NBRS; i++)
	sigstate.allbrs[i] = BrRd(i);
    for (i = 0; i < NFRS; i++)
	sigstate.allfrs[i] = FrRd(i);
    for (i = 0; i < NGRS; i++) {
	sigstate.allgrs[i] = grs[i];
    }
    sigstate.pfs = PFS;

    /* For now no trace is generated in the timer interrupt handler.
       Generate a discontinuity before returning.  See retAlarm() */
    traceSve = traceEnb;
    traceVcyEnb();
    setTraceEnb(NO);

    sigstate.slot = SLOT(ip);
    ip &= ~(ADDR)0xF;
    alarm_rp = ip;
    ip -= 16;
    icp = NULL;
#ifdef ALARM
    alarm_insts = total_insts + alarm_interval;
#endif
    BrWrt(6, sigHdlr);
    info.qpred = 0;
    info.extrainfo[1] = 0;
    info.extrainfo[2] = 6;
    (void)br_call_spnt_few_b1_b2Comb(&info);
    total_insts--;
}

void retAlarm(void)
{
    int i;

    for (i = 1; i < NPRS; i++)
	PrWrt(i, sigstate.allprs[i]);
    for (i = 0; i < NBRS; i++)
	BrWrt(i, sigstate.allbrs[i]);
    for (i = 2; i < NFRS; i++)
	FrWrt(i, sigstate.allfrs[i]);
    for (i = 1; i < NGRS; i++) {
	grs[i] = sigstate.allgrs[i];
    }
    PFS = sigstate.pfs;

    /* Since no trace is generated in the irpt handler, generate a
       discontinuity if tracing is enabled */
    setTraceEnb(traceSve);
    traceDisc();

    alarm_rp = ~0ULL;
    SLOT(ip) = sigstate.slot;
    icp = NULL;
    total_insts--;
}

#endif /* ! defined __linux__ */

INSTINFO* setIcp(void)
{
    CT *ct;

#ifdef CALCINFO
calcInfo++;
#endif
    ct = &CacheTbl[BitfX(ip,IC_START,IC_LEN)];
    ct->ipn = ip & CTTAGMASK;
    if ((ip & CTTAGMASK) != ct->tag) {
	ct->tag = ip & CTTAGMASK;
	fillinDecodePage(ct, ip);
    }
    return &ct->instCache[BitfX(ip,52,10)];
}

Status iCycleApp(void)
{
    INSTINFO* info = icp;
    Status st;

    if (preInst) {
#ifdef __linux__
	if (signal_pending()) {
	    signal_invoke_handler(0);
	    return StTakenBr;
	}
	if (ip == 0xa000000000002000ULL) {
	    signal_return();
	    return StTakenBr;
	}
#endif
	if (cfle && !mandatoryRSEloads())
	    return StFault;
#if !defined __linux__
	if (alarm_insts <= total_insts) {
	    gotoAlarm();
	    return StTakenBr;
	}
	if (ip == alarm_rp) {
	    retAlarm();
	    return StTakenBr;
	}
#endif /* !defined __linux__ */
	if (prof_insts == total_insts)
	    profCnt();
	if (icntEnb && info->combFn != instFetchDecodeFP)
	    incrInstCnts(info);
	if (traceEnb && info->combFn != instFetchDecodeFP) {
	    WRT_CAPSULE_TR();
	    WRT_BUNDLE_TR();
	    WRT_IOFFSET_TR();
	}
	if (cntlC) {		/* user interrupt */
	    intrsim = NO;
	    cntlC   = NO;
	    preInst &= ~PRE_USERINT;
	    progStop("Interrupting simulation\n");
	}
    }
#ifdef EXTRA_CHECKS
    if (!checkStack())
	longjmp(jmpenv, 0);
#endif
    st = info->combFn(info) & ~ST_CHECK;
    if (st & ST_IP_INC) {
	icp = info->next;
	ip += info->delta << 2;
	if (END_OF_PAGE(icp)) {
	    icp = setIcp();
	}
    } else
	icp = setIcp();
    ip = IPTR(icp);
    if (traceEnb && st == StTakenBr)
	WRT_TKNBR_TR();
#ifdef DEBUG
/* This is where an implementation might generate its specific trace records */
    if (traceEnb) {
	WRT_STALL_TR();
	WRT_STALL_LATENCY_TR();
    }
#endif
    return st | info->stop;
}

static BOOL setSysIcp(void)
{
    ADDR pip;
    CT *ct;

    if (!itlbLookup(ip, PSR_IT, &pip))
	return NO;
    ct = &CacheTbl[BitfX(pip,IC_START,IC_LEN)];
    ct->ipn = ip & CTTAGMASK;
    if ((pip & CTTAGMASK) != ct->tag) {
	ct->tag = pip & CTTAGMASK;
	fillinDecodePage(ct, ip);
    }
    icp = &ct->instCache[BitfX(pip,52,10)];
#if defined(__ia64__) && defined(__GNUC__)
    asm volatile ("lfetch.fault [%0]" :: "r"(&icp->next));
#endif
    return YES;
}

#ifdef TRACE_DAVIDM
static void
save_trace (void)
{
	FILE *tfp = fopen("trace", "w");
	int i = wr + 1;

	if (i > sizeof(tracebuf)/sizeof(tracebuf[0]))
		i = 0;
	do {
		fwrite(tracebuf + i, sizeof(tracebuf[0]), 1, tfp);
		++i;
		if (i >= sizeof(tracebuf)/sizeof(tracebuf[0]))
			i = 0;
	} while (i != wr);
	fclose(tfp);
}
#endif /* TRACE_DAVIDM */

Status iCycleSys(void)
{
    INSTINFO* info;
    Status st;
    BOOL ss = PSR_SS;	/* save "pre-execution" SS bit */
    unsigned slot;

    if (icp)
	ip = IPTR(icp);/* so IIP can get assigned on faults */
    if (cfle && !mandatoryRSEloads())
	return StFault;
    if (!icp && !setSysIcp())
	return StFault;
    info = icp;
#ifdef EXTRA_CHECKS
    if (!checkStack())
	longjmp(jmpenv, 0);
#endif
    if (icntEnb && info->combFn != instFetchDecodeFP)
	incrInstCnts(info);
    if (traceEnb && info->combFn != instFetchDecodeFP) {
	WRT_CAPSULE_TR();
	WRT_BUNDLE_TR();
	WRT_IOFFSET_TR();
    }
    slot = SLOT(ip);		/* save "pre-execution" slot */
    st = info->combFn(info) & ~ST_CHECK;
    if (st != StFault) {
	/* st = StSuccess, StTakenBr, StTrap (FP), or StRFI */
	if (st != StRFI)
	    PSR_IA = PSR_ID = PSR_DA = PSR_DD = PSR_ED = NO;
	if (st & ST_IP_INC) {		/* StSuccess, Fall through */
	    if (PSR_IC)
		IIPA = IPTR(info);
	    icp = icp->next;
	    if (END_OF_PAGE(icp)) {	/* End of page */
		ip = NEXT_PAGE_IPTR(info);
		icp = NULL;
	    } else
		ip = IPTR(icp); /* XXX - only needed for UI? */
	    /* unimplAddr(ip) check not done.  Too expensive for its worth */
	} else if (info->samepg) {	/* Branch to same page */
	    ADDR curr_ip = IPTR(icp);
	    icp += (int)(ip - curr_ip) / 4;
	} else {			/* Branch to different page, trap */
	    icp = NULL;
	    if (unimplAddr(ip)) {
		unimplInstructionAddressTrap(slot, st);
		st = StTrap;
	    }
	}
	if (st == StTakenBr && PSR_TB) {
	    takenBranchTrap(slot);
	    st = StTrap;
	} else if (st != StTrap && st != StRFI && ss) {
	    /* XXX - doublecheck if SKIP_NOPS defined */
	    singleStepTrap(info->delta == 3 ? 2 : slot);
	    st = StTrap;
	}
    } else		/* Fault */
	icp = NULL;
    if (++ITC == ITM && !IMASK(ITV))
	pendIrpt(ITV);
    if (sscPend && --sscPend->irptcnt <= 0)
	pendSscIrpt(sscPend->type);
    if (intrsim) {
	if (kybdint) {		/* keyboard interrupt */
	    intrsim = NO;
	    kybdint = NO;
	    pendSscIrpt(KEYBOARD_IRPT);
	} else if (cntlC) {	/* user interrupt */
	    intrsim = NO;
	    cntlC   = NO;
	    progStop("Interrupting simulation\n");
	} else if (extint) {	/* external interrupt */
	    intrsim = NO;
	    extint  = NO;
	    cfle    = NO;
	    externalInterrupt();
	    icp = NULL;
	}
    }
    if (traceEnb && (st == StTakenBr || st == StRFI))
	WRT_TKNBR_TR();
#ifdef DEBUG
/* This is where an implementation might generate its specific trace records */
    if (traceEnb) {
	WRT_STALL_TR();
	WRT_STALL_LATENCY_TR();
    }
#endif
    return st | info->stop;
}

static Status iAexec(IAinstInfoPtr info)
{
    return info->execFn(info);
}

Status iAiCycle(void)
{
    IAinstInfoPtr info = 0;
    Status st;
    ADDR pip;

    if (dosABI)
        pip = ip;
    else if (!itlbLookup(ip, PSR_IT, &pip))
        return StFault;
    /* XXX - add code-segment limit checks */
    if (!ia_decode(EIP, info))
        return StFault;
    if ((st = iAexec(info)) & ST_IP_INC) {
       EIP += info->delta;
       ip = IPfromEIP(EIP);
    }
    if (!dosABI)
        updateTmrs();
    if (intrsim) {
        if (cntlC) {            /* user interrupt */
            intrsim = NO;
            cntlC   = NO;
            progStop("Interrupting simulation\n");
        } else if (extint) {    /* external interrupt */
            intrsim = NO;
            extint  = NO;
            externalInterrupt();
        }
    }
    return st;
}

static IAinstInfoPtr addrToIAcacheInfo(ADDR adr)
{
    ADDR pa;

    if (dosABI)
       pa = adr;
    else if (!itlbMLookup(adr, PSR_IT, &pa))
       return 0;
    return 0;
}

void initSysState(void)
{
   
#ifdef NEW_MP
    instsLeft = ipts = mips * TIMESLICE;
#endif
    /* This value is only changed by a call to PAL_PLATFORM_ADDR */
    ibBase  = 0x00000000FEE00000ULL;
    preInst = icntEnb ? PRE_ICNT : 0;
}

void resetState(int proc)
{
    int i;

#ifdef NEW_MP
int save = curPid;
curPid = proc;
#endif
    for (i = 0; i < GR_STK_BASE+N_STACK_VIRT; i++)
	grmap[i] = i;
    for (i = FR_ROT_BASE; i < NFRS; i++)
	frmap[i] = i;
    for (i = 0; i < FR_ROT_SIZE; i++)
	frmap[i + NFRS] = i + FR_ROT_BASE;
    for (i = 2; i < NFRS; i++) {
	frs[i].special = NO;
	frs[i].unorm = 64;
#ifdef NEW_MP
	frs[i].sign = 0;
	frs[i].exp = 0;
	frs[i].mant = 0;
	frs[i].class = 0/*CLASS_NONE*/;
#endif
    }
#ifdef NEW_MP
    sof = N_STACK_VIRT;
    soil = sor = rrbg = rrbf = rrbp = bol = 0;
    dirty = dirtyNat = clean = cleanNat = invalid = 0;
    cfle = NO;
    for (i = 1; i < NPRS; i++)
	prs[i] = NO;
    for (i = 0; i < NBRS; i++)
	brs[i] = 0;
    for (i = 0; i < NARS; i++)
	ars[i] = 0;
    for (i = 0; i < MAX_NGRS; i++)
	grs[i].val = 0, grs[i].nat = NO;
#endif
    PTA = 0x3CULL;	/* size == 15 (define PTA_SIZE?) */
    (void)acceptIrpt();	/* set IVR to "no interrupts pending" value */
    tlbInit();
    /* reset all the tags to impossible values */
    for (i = 0; i < NUM_INSTCACHES; i++) {
	CacheTbl[i].tag = ~(ADDR)0;
    }
#ifdef NEW_MP
curPid = save;
#endif
}

void initState(int proc)
{
#ifdef NEW_MP
int save = curPid;
curPid = proc;
#endif
    prs[0] = YES;
    frs[0].special = NO;
    frs[0].unorm = 0xFF;
    frs[1].special = NO;
    frs[1].sign = 0;
    frs[1].exp  = 0x2007B;	/* 0xFFFF + EXTRA_BIAS */
    frs[1].mant = 0x8000000000000000ULL;
#ifdef NEW_MP
    cpuids[0] = 0x5320696b53205048ULL;
    cpuids[1] = 0x726f74616c756d69ULL;
    cpuids[2] = 0;
    cpuids[3] = 0x000000001f000004ULL;
    cpuids[4] = 5;
#endif
    resetState(proc);
#ifdef NEW_MP
curPid = save;
#endif
}

void initAppState(int proc)
{
#ifdef NEW_MP
    ADDR adr = mpState[proc].ip_ & CTTAGMASK;
#else
    ADDR adr = ip & CTTAGMASK;
#endif
    CT *ct;
    int i;

#ifdef NEW_MP
int save = curPid;
curPid = proc;
#endif
    for (i = 0; i < NUM_INSTCACHES; i++) {
	/* XXX - do we need itlbMLookup(adr) here? */
	ct = &CacheTbl[BitfX(adr,IC_START,IC_LEN)];
	ct->ipn = ct->tag = adr & CTTAGMASK;
	fillinDecodePage(ct, adr);
	adr += 0x1000ULL;
    }
#ifdef NEW_MP
curPid = save;
#endif
    pspSeg = DS;	/* XXX - move to separate initDosEnv function? */
			/* XXX - and what about similar code in bin_load? */
    setInitialTime(0);
}

#ifdef GET_INFO
INSTINFO *getInfo(void)
{
    return &ict[BitfX(ip,IC_START,IC_LEN+10)];
}
#endif

void unwindFlush(void)
{
    while (dirty + dirtyNat > 0)
	rse_store();
}

CTR getTotalInsts(void)
{
    return total_insts;
}

void setTotalInsts(CTR val)
{
    total_insts = val;
}

CTR getTotalFaults(void)
{
    return abi ? ~(CTR)0 : total_faults;
}

void setTotalFaults(CTR val)
{
    total_faults = val;
}

CTR getTotalCycles(void)
{
    return total_cycles;
}

void setTotalCycles(CTR val)
{
    total_cycles = val;
}

REG getMaxSP(void)
{
    return max_sp;
}

void setMaxSP(REG val)
{
    max_sp = val;
}

#ifdef NEW_MP
CTR getCurPid(void)
{
    return curPid+2;
}
#endif

IAinstInfoPtr addrToIAcacheInfo(ADDR adr);
Status iAinstFetchDecode(IAinstInfoPtr info);

/* adr is the physical address */
void clearPdecode(ADDR adr, unsigned size)
{
    CT *ct;

    /* XXX - Should all instructions in bundle be set to instFetchDecode
	in case the bundle isn't entered from the beginning? */
    ct = &CacheTbl[BitfX(adr,IC_START,IC_LEN)];
    if (ct->tag == (adr & CTTAGMASK)) {
	unsigned i = BitfX(adr & ~(ADDR)0xF,52,10);
	INSTINFO *info0, *info1, *info2;

	info0 = &ct->instCache[i + 0];
	info1 = &ct->instCache[i + 1];
	info2 = &ct->instCache[i + 2];
	info0->combFn = instFetchDecodeFP;
	info1->combFn = instFetchDecodeFP;
	info2->combFn = instFetchDecodeFP;
	info0->immed64 = i << 2;
	info1->immed64 = (i << 2) + 4;
	info2->immed64 = (i << 2) + 8;
    }
}

void bparts(Bundle *bptr, BYTE *templSB, DWORD slot[])
{
    BundleParts parts;

    parts = bundle_parts(bptr);
    *templSB = parts.templSB;
    slot[0] = parts.slot[0];
    slot[1] = parts.slot[1];
    slot[2] = parts.slot[2];
}

typedef union {
    struct {
	DWORD slot;
	INSTINFO info;
	TemplateInfoPtr t;
    } em;
    struct {
	BYTE op;
	IAinstInfo info;
    } ia;
} bpt_info;

/* XXX - use encoder? */
#define EMBRKIMM        0x0ull
#define EMBRK           (((EMBRKIMM & 0x100000ull) << 16) | \
			 ((EMBRKIMM & 0xfffffull) << 6))

void bptReplace(Bundle *bptr, ADDR *adrptr, DWORD *orig)
{
    BundleParts parts;
    TemplateInfoPtr t;
    ADDR adr = *adrptr;
    unsigned s = SLOT(adr);

    parts = bundle_parts(bptr);
    t = template_info(parts.templSB);
    if (s > 0 && t->slot[1].unit == L_Unit) {
	s = 2;
	SLOT(adr) = 1;
	*adrptr = adr;
    }
    *orig = parts.slot[s];
    parts.slot[s] = EMBRK;
    *bptr = bundle_from_parts(&parts);
    if (!abi)
	(void)itlbMLookup(adr, PSR_IT, &adr);
    clearPdecode(adr, 16);
}

void bptRestore(Bundle *bptr, ADDR adr, DWORD orig)
{
    BundleParts parts;
    TemplateInfoPtr t;
    unsigned slot = SLOT(adr);

    parts = bundle_parts(bptr);
    t = template_info(parts.templSB);
    if (slot > 0 && t->slot[1].unit == L_Unit)
	slot = 2;
    parts.slot[slot] = orig;
    *bptr = bundle_from_parts(&parts);
    if (!abi)
	(void)itlbMLookup(adr, PSR_IT, &adr);
    clearPdecode(adr, 16);
}

void iabptReplace(ADDR adr)
{
    IAinstInfoPtr info;

    if ((info = addrToIAcacheInfo(adr)))
	info->execFn = iAinstFetchDecode;
}

void iabptRestore(ADDR adr)
{
    IAinstInfoPtr info;

    if ((info = addrToIAcacheInfo(adr)))
	(void)ia_decode(EIPfromIP(adr), info);
}

BOOL instrReplace(BundlePtr bptr, ADDR badr, unsigned slot,
		  EncodedInstPtr inst)
{
    BundleParts parts;
    TemplateInfoPtr t;

    parts = bundle_parts(bptr);
    t = template_info(parts.templSB);
    if (inst->valid_extra_bits) {
	if (slot == 0 || t->slot[1].unit != L_Unit)
	    return NO;
	parts.slot[1] = inst->extra_bits;
	slot = 2;
    }
    if (!(inst->unit == No_Unit || inst->unit == t->slot[slot].unit ||
	  (inst->unit == A_Unit && (t->slot[slot].unit == M_Unit ||
				    t->slot[slot].unit == I_Unit))))
	return NO;
    parts.slot[slot] = inst->bits;
    *bptr = bundle_from_parts(&parts);
    clearPdecode(badr, 16);
    return YES;
}

void templReplace(BundlePtr bptr, ADDR adr, BYTE new)
{
    BundleParts parts;

    parts = bundle_parts(bptr);
    parts.templSB = new;
    *bptr = bundle_from_parts(&parts);

    itlbLookup(adr, PSR_IT, &adr);

    itlbLookup(adr, PSR_IT, &adr);
    clearPdecode(adr, 16);
}

void setDerivedState(BOOL setEIP)
{
    updateGrMap(0);
    setIAmode();
    arithFlagsFromEflags();
    if (setEIP)
	setEIPfromIP();
    (void)acceptIrpt();	/* to make IVR consistent with IRRs */
}

/* return to iCycle code, but do not stop simulation */
void progPause(void)
{
    running = NO;
    longjmp(jmpenv, 0);
}

/* return to iCycle code, then stop simulation */
void progStop(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    (void)vsprintf(execMsg, fmt, ap);
    va_end(ap);
    icp = abi ? setIcp() : NULL;
    longjmp(jmpenv, 0);
}

void progExit(const char *fmt, ...)
{
    va_list ap;

    exited = YES;
    va_start(ap, fmt);
    (void)vsprintf(execMsg, fmt, ap);
    va_end(ap);
    longjmp(jmpenv, 0);
}

BOOL stepIt_loop(CTR cnt)
{
    Status st;
#ifdef NEW_MP
    static CTR ti0, cnt0;
#endif

#ifdef NEW_MP
    ti0 = total_insts; cnt0 = cnt;
#endif
    execMsg[0] = '\0';
    if (cnt == 1) {
	executionMode = STEP;
#ifdef NEW_MP
	if (curPid == ~0U) {
	    (void)strcpy(execMsg, "No run\n");
	    return NO;
	}
#endif
    }
    if (!setjmp(jmpenv)) {
	while (cnt) {
	    while (PSR_IS && cnt) {		/* iA */
		cnt--;
		st = iAiCycle();
		total_cycles++;
		total_insts++;
	    }
	    while (!PSR_IS && cnt) {		/* EM */
		cnt--;
		st = unixABI ? iCycleApp() : iCycleSys();
		if (st & ST_STOP)
		    total_cycles++;
		total_insts++;
#ifdef NEW_MP
		if (!--instsLeft) {
		    curPid = nextPid;
		    icp = unixABI ? setIcp() : NULL;
		    instsLeft = ipts;
		}
#endif
	    }
	}
	return YES;
    }
    if (exited)
	total_insts++, total_cycles++;
    return NO;
}

BOOL runIt_loopX(CTR cnt)
{
    CTR i;
    Status st;

    execMsg[0] = '\0';
    if (!setjmp(jmpenv)) {
	if (PSR_IS)			/* iA */
	    for (i = 0; i < cnt; i++) {
		st = iAiCycle();
		total_cycles++;
		total_insts++;
	    }
	else if (unixABI)		/* EM - app mode */
	    for (i = 0; i < cnt; i++) {
		st = iCycleApp();
		if (st & ST_STOP)
		    total_cycles++;
		total_insts++;
	    }
	else				/* EM - sys mode */
	    for (i = 0; i < cnt; i++) {
#ifdef NEW_MP
		while (!active())
			curPid = (curPid + 1) & 3;
#endif
		st = iCycleSys();
		if (st & ST_STOP)
		    total_cycles++;
		total_insts++;
#ifdef NEW_MP
		curPid = (curPid + 1) & 3;
#endif
	    }
	return YES;
    }
    return NO;
}

#ifdef NEW_MP
void runIt_loop(void)
{
    Status st;

    execMsg[0] = '\0';
    if (!setjmp(jmpenv)) {
	if (PSR_IS) {			/* iA */
	    executionMode = RUN_IA;
	    for (;;) {
		st = iAiCycle();
		total_cycles++;
		total_insts++;
	    }
	} else {
	    executionMode = RUN_EM;
	    while (1) {
		while (!active())
		    curPid = (curPid + 1) & 3;
		st = (unixABI) ? iCycleApp() : iCycleSys();
		if (st & ST_STOP)
		    total_cycles++;
		total_insts++;
		curPid = (curPid + 1) & 3;
	    }
	}
    }
}

#else

void runIt_loop(void)
{
again:
    execMsg[0] = '\0';
    if (!setjmp(jmpenv)) {
	executionMode = RUN_EM;
	while (1) {
	    if (!unixABI) {
#ifdef NEW_MP
		for (curPid = 0; curPid < 4; curPid++) {
		    if (active()) {
			icp = NULL;
			iCycleSysLoop();
		    }
		}
#else
		iCycleSysLoop();
#endif
	    } else
		iCycleAppLoop(icp);
	}
    }
    if (exited)
	total_insts++, total_cycles++;
    if (!running) {
	running = YES;
	icp = setIcp();
	goto again;
    }
#ifdef COUNT_GR_ACCESSES
    print_gr_accesses();
#endif
}
#endif


static BOOL preInstStuff(INSTINFO *info)
{
#ifdef __linux__
    if (signal_pending()) {
	signal_invoke_handler(0);
	info = setIcp();
	return YES;
    }
    if (ip == 0xa000000000002000ULL) {
	signal_return();
	info = setIcp();
	return YES;
    }
#endif
    if (cfle && !mandatoryRSEloads())
	return YES;
#ifndef __linux__
    if (alarm_insts <= total_insts) {
	gotoAlarm();
	return YES;
    }
    if (ip == alarm_rp) {
	retAlarm();
	return YES;
    }
#endif
    if (prof_insts == total_insts)
	profCnt();
    if (icntEnb && info->combFn != instFetchDecodeFP)
	incrInstCnts(info);
    if (traceEnb && info->combFn != instFetchDecodeFP) {
	WRT_CAPSULE_TR();
	WRT_BUNDLE_TR();
	WRT_IOFFSET_TR();
    }
    return NO;
}

#ifdef NEW_MP
static void iCycleAppLoop(INSTINFO *zicp)
{
    INSTINFO *info;
    info = &zicp->info;
    /* set ip in case combFn hits a bpt */
    ip = zicp->ct->ipn + IPTR(&zicp->info);
    if (preInst && preInstStuff(info)) {
	fprintf(stderr, "preInstStuff failed at inst %lld, ip %llx\n",
	    total_insts, ip);
	fflush(stderr);
	return;
    }
    if (info->combFn(info) & ST_IP_INC) {
	zicp = zicp->next;
	if (info->stop)
	    total_cycles++;
    } else if (info->samepg) {
	ADDR curr_ip = IPTR(&zicp->info);
	zicp += (int)(ip - curr_ip) / 4;
	if (traceEnb)
	    WRT_TKNBR_TR();
	total_cycles++;
    } else {
	zicp = setIcp();
	if (traceEnb)
	    WRT_TKNBR_TR();
	total_cycles++;
    }
    total_insts++;
    ip = zicp->ct->ipn + IPTR(&zicp->info);
}
#else
static void iCycleAppLoop(INSTINFO *zicp)
{
    ip = IPTR(zicp);

    for (;;) {
	INSTINFO *info;

	if (!zicp) {
	    zicp = setIcp();
	}
	info = zicp;

	if (preInst && preInstStuff(info))
	    continue;
	if (info->combFn(info) & ST_IP_INC) {
	    zicp = zicp->next;
	    ip += info->delta << 2;
	    if (info->stop)
		total_cycles++;
	} else if (zicp && info->samepg) {
	    ADDR curr_ip = IPTR(zicp);
	    zicp += (int)(ip - curr_ip) / 4;
	    if (traceEnb)
		WRT_TKNBR_TR();
	    total_cycles++;
	} else {
	    zicp = setIcp();
	    if (traceEnb)
		WRT_TKNBR_TR();
	    total_cycles++;
	}
	total_insts++;
    }
}
#endif


#if 0
/* FIXME: remove when other iCycleSysLoop is ready */
static void iCycleSysLoop(void)
{
    INSTINFO *info;
    Status st = 0;
    BOOL ss = NO;
    unsigned slot;

#ifndef NEW_MP
    while (1) {
#endif
	if (icp) {
	    /* so IIP can get assigned on faults */
	    ip = IPTR(icp);
#if defined(__ia64__) && defined(__GNUC__)
	    asm volatile ("lfetch [%0]" :: "r"(icp->info.next));
#endif
	}
	if (cfle && !mandatoryRSEloads())
	    goto lbl;
	if (!icp && !setSysIcp())
	    goto lbl;
	info = icp;
	if (icntEnb && info->combFn != instFetchDecodeFP)
	    incrInstCnts(info);

	if (traceEnb && info->combFn != instFetchDecodeFP) {
	    WRT_CAPSULE_TR();
	    WRT_BUNDLE_TR();
WRT_IOFFSET = 1;
	    WRT_IOFFSET_TR();
	}

	ss = PSR_SS;		/* save "pre-execution" SS bit */
	slot = SLOT(ip);	/* save "pre-execution" slot */

	st = info->combFn(info) & ~ST_CHECK;
	if (st != StFault) {
	    if (st != StRFI) {
		/* PSR_IA = PSR_ID = PSR_DA = PSR_DD = PSR_ED = NO: */
		psr &= 0xFFFFD71FFFFFFFFFULL;
	    }

	    if (st & ST_IP_INC) {
		if (PSR_IC)
		    IIPA = IPTR(icp);
		icp = icp->next;
		if (END_OF_PAGE(icp)) {
		    ip = NEXT_PAGE_IPTR(info);
		    icp = NULL;
		} else {
		    /* XXX - mainly used for UI, but kernel boot needs? */
		    ip = IPTR(icp);
#if defined(__ia64__) && defined(__GNUC__)
                    asm volatile ("lfetch.fault [%0]" :: "r"(&icp->info.next));
#endif
		}
		if (info->stop)
		    total_cycles++;
	    } else if (info->samepg) {
		ADDR curr_ip = IPTR(icp);
		icp += (int)(ip - curr_ip) / 4;
#if defined(__ia64__) && defined(__GNUC__)
		asm volatile ("lfetch.fault [%0]" :: "r"(&icp->info.next));
#endif
		total_cycles++;
	    } else
		icp = NULL, total_cycles++;

	    /* XXX - Use a status of StTrap tp cover both "if"s? */
	    if (st == StTakenBr && PSR_TB) {
		takenBranchTrap(slot);
		st = StTrap;
	    } else if (st != StTrap && st != StRFI && ss) {
		/* XXX - doublecheck if SKIP_NOPS defined */
		singleStepTrap(info->delta == 3 ? 2 : slot);
		st = StTrap;
	    }
	    if (traceEnb && (st == StTakenBr || st == StRFI))
		WRT_TKNBR_TR();
	} else
	    icp = NULL, total_cycles++;
lbl:
	if (traceEnb && (st == StTakenBr || st == StRFI))
	    WRT_TKNBR_TR();

	if ((st != StFault && ss) || (st == StTakenBr && PSR_TB)) {
	    if (icp)
		ip = IPTR(icp);

	    slot = SLOT(ip);
	    if (st == StTakenBr)
		takenBranchTrap(slot);
	    else
		singleStepTrap(slot);
	    st = StTrap;
	    icp = NULL;
	}

	total_insts++;
	if (++ITC == ITM && !IMASK(ITV))
	    pendIrpt(ITV);
	if (sscPend && --sscPend->irptcnt <= 0)
	    pendSscIrpt(sscPend->type);
	if (intrsim) {
	    if (kybdint) {		/* keyboard interrupt */
		intrsim = NO;
		kybdint = NO;
		pendSscIrpt(KEYBOARD_IRPT);
	    } else if (cntlC) {		/* user interrupt */
		intrsim = NO;
		cntlC   = NO;
		progStop("Interrupting simulation\n");
	    } else if (extint) {	/* external interrupt */
		intrsim = NO;
		extint  = NO;
		cfle    = NO;
		externalInterrupt();
		icp = NULL;
	    }
	}
#ifndef NEW_MP
    }
#endif
}

#else

static Status iCycleSysLoopLite(unsigned long count)
{
    unsigned long insn_count = count;
    unsigned long cycle_count = 0L;
    BOOL psr_ic = PSR_IC;
    Status st;

    do {
	INSTINFO *info = icp;
	ADDR curr_ip = ip;

	st = info->combFn(info);

	if (st & ST_IP_INC) {
	    /* simple increment case:
	     *   both ip & icp are pointing at the current
	     *   instrution. Increment icp & ip by delta to
	     *   point at the next one. 
	     */
	    if (psr_ic) {
		IIPA = curr_ip;
	    }

	    icp = info->next;
	    ip += info->delta << 2;

	    if (info->stop) {
		++cycle_count;
	    }
	}
	else {
	    /* taken branch & other special cases (trap,fault,...):
	     *   ip is already pointing the new instruction.
	     *   Update icp to the next info or NULL if it is
	     *   out of reach.
	     */
	    if (info->samepg && (st & ~ST_CHECK) != StFault) {
		icp += (ip - curr_ip) >> 2;
	    }
	    else {
		icp = NULL;
	    }
	    ++cycle_count;
	}
	--insn_count;

	/* should we break the loop? */
	if (++ITC == ITM || !icp || st & ST_CHECK || intrsim) {
	    break;
	}

    } while(insn_count);

    
    /* ITC will get incremented upon return, restore value */
    --ITC;

    /* update the counters */
    total_insts += count - insn_count;
    total_cycles += cycle_count;

    /* ST_CHECK is only used in the lite loop, clear it! */
    return st & ~ST_CHECK;
}

static void iCycleSysLoop(void)
{
    INSTINFO *info;
    Status st;

    while (1) {
	unsigned int slot;
	BOOL psr_ss;

	if (icp) {
	    /* so IIP can get assigned on faults */
	    ip = IPTR(icp);
	}

	if ((!cfle || mandatoryRSEloads()) && (icp || setSysIcp())) {
	    /* we can execute! */

	    info = icp;
	    if (icntEnb && info->combFn != instFetchDecodeFP) {
		incrInstCnts(info);
	    }
	    if (traceEnb && info->combFn != instFetchDecodeFP) {
		WRT_CAPSULE_TR();
		WRT_BUNDLE_TR();
		WRT_IOFFSET = 1; /* FIXME_laurentm: why? */
		WRT_IOFFSET_TR();
	    }

	    psr_ss = PSR_SS;
	    slot = SLOT(ip);

	    /* if any of PSR_IA,PSR_ID,PSR_DA,PSR_DD,PSR_ED,PSR_TB,PSR_SS
	     * sscPend,icntEnb,traceEnb then only execute 1
	     * instruction in LoopLite
	     */
	    st = iCycleSysLoopLite(
		((psr & 0x000029E004000000ULL) != 0ULL
		 || sscPend || icntEnb || traceEnb) ? 1L : ~0L);

	    if (st != StRFI) {
		/* PSR_IA = PSR_ID = PSR_DA = PSR_DD = PSR_ED = NO: */
		psr &= 0xFFFFD71FFFFFFFFFULL;
	    }

	    if (traceEnb && (st == StTakenBr || st == StRFI)) {
		WRT_TKNBR_TR();
	    }

	    if (st == StTakenBr && PSR_TB) {
		takenBranchTrap(slot);
		st = StTrap;
	    }
	    else if (st != StFault && psr_ss) {
		singleStepTrap(slot);
		st = StTrap;
	    }
	}

	if (++ITC == ITM && !IMASK(ITV))
	    pendIrpt(ITV);

	if (sscPend && --sscPend->irptcnt <= 0)
	    pendSscIrpt(sscPend->type);

	if (intrsim) {
	    if (kybdint) {		/* keyboard interrupt */
		intrsim = NO;
		kybdint = NO;
		pendSscIrpt(KEYBOARD_IRPT);
	    } else if (cntlC) {		/* user interrupt */
		intrsim = NO;
		cntlC   = NO;
		progStop("Interrupting simulation\n");
	    } else if (extint) {	/* external interrupt */
		intrsim = NO;
		extint  = NO;
		cfle    = NO;
		externalInterrupt();
		icp = NULL;
	    }
	}
    }
}

#endif

#ifdef COUNT_GR_ACCESSES
static FILE *grstats;

static void print_stat(char *s, CTR rd, CTR trd, CTR wrt, CTR twrt)
{
    fprintf(grstats, "%-10s %9llu (%5.2f)   %9llu (%5.2f)\n",
	    s, rd, 100.0*rd/trd, wrt, 100.0*wrt/twrt);
}

static void print_gr_accesses(void)
{
    CTR scratchrd = numgrrds[2] + numgrrds[3];
    CTR scratchwrt = numgrwrts[2] + numgrwrts[3];
    CTR preservedrd = 0;
    CTR preservedwrt = 0;
    CTR stackedrd = 0;
    CTR stackedwrt = 0;
    CTR totalrd = 0;
    CTR totalwrt = 0;
    int i;

    if (!(grstats = fopen("grstats", "w"))) {
	fprintf(stderr, "can't open grstats\n");
	return;
    }
    for (i = 0; i <= 127; i++) {
	totalrd += numgrrds[i];
	totalwrt += numgrwrts[i];
    }
    print_stat("r0:", numgrrds[0],  totalrd, numgrwrts[0],  totalwrt);
    print_stat("gp:", numgrrds[1],  totalrd, numgrwrts[1],  totalwrt);
    print_stat("sp:", numgrrds[12], totalrd, numgrwrts[12], totalwrt);
    print_stat("tp:", numgrrds[13], totalrd, numgrwrts[13], totalwrt);
    for (i = 4; i <= 7; i++) {
	preservedrd += numgrrds[i];
	preservedwrt += numgrwrts[i];
    }
    print_stat("preserved:", preservedrd, totalrd, preservedwrt, totalwrt);
    for (i = 8; i <= 11; i++) {
	scratchrd += numgrrds[i];
	scratchwrt += numgrwrts[i];
    }
    for (i = 14; i <= 31; i++) {
	scratchrd += numgrrds[i];
	scratchwrt += numgrwrts[i];
    }
    print_stat("scratch:", scratchrd, totalrd, scratchwrt, totalwrt);
    for (i = 32; i <= 127; i++) {
	stackedrd += numgrrds[i];
	stackedwrt += numgrwrts[i];
    }
    print_stat("stacked:", stackedrd, totalrd, stackedwrt, totalwrt);
    fprintf(grstats, "total:     %9llu           %9llu\n", totalrd, totalwrt);
    fclose(grstats);
}
#endif


void setIcntEnb(void)
{
    icntEnb = YES;
}

void setIntrsim(BOOL val)
{
    intrsim = val;
    cntlC = val;
    if (val)
	preInst |= PRE_USERINT;
    else
	preInst &= ~PRE_USERINT;
}

char *getExecMsg(void)
{
    return execMsg[0] ? execMsg : NULL;
}

/* ALAT Code */

BOOL use_alat = NO;

#ifdef FIXED_ALAT
#define N_ALAT_SETS_LOG_2 4
#define N_ALAT_SETS (1<<N_ALAT_SETS_LOG_2)
#define N_ALAT_ASSOC 2
#define REG_MASK ONES(N_ALAT_SETS_LOG_2)
#else
unsigned alatAssoc = 2;
static unsigned alatSets, alatSetsLog2;
static unsigned REG_MASK, N_ALAT_ASSOC, N_ALAT_SETS, N_ALAT_SETS_LOG_2;
#endif

typedef struct {
    ADDR paddr;
    unsigned valid : 1;
    unsigned fpreg : 1;
#ifdef FIXED_ALAT
    unsigned rega  : 7-N_ALAT_SETS_LOG_2;
#else
    unsigned rega  : 7;
#endif
    unsigned size  : 5;
} alatEntry;
#ifdef FIXED_ALAT
static alatEntry alat[N_ALAT_SETS][N_ALAT_ASSOC] = {0};
#else
static alatEntry *alat;
#endif
#ifdef ALAT_DEBUG
FILE *alat_fp;
#endif

void alatInit(void)
{
#ifndef FIXED_ALAT
#ifdef ALAT_DEBUG
alat_fp = fopen("alat_info", "w");
#endif
    alatSets = 32 / alatAssoc;
    switch (alatSets) {
	case 1:
	    alatSetsLog2 = 0;
	    break;
	case 2:
	    alatSetsLog2 = 1;
	    break;
	case 4:
	    alatSetsLog2 = 2;
	    break;
	case 8:
	    alatSetsLog2 = 3;
	    break;
	case 16:
	    alatSetsLog2 = 4;
	    break;
	case 32:
	    alatSetsLog2 = 5;
	    break;
    }
    alat = calloc(alatSets * alatAssoc, sizeof *alat);
    REG_MASK = alatSetsLog2 ? ONES(alatSetsLog2) : 0;
    N_ALAT_SETS = alatSets;
    N_ALAT_SETS_LOG_2 = alatSetsLog2;
    N_ALAT_ASSOC = alatAssoc;
#endif
}

#ifdef ALAT_DEBUG
static void alat_display_entries(void)
{
    unsigned i, j, valid, fpreg, rega, size;
    ADDR paddr;

    fprintf(alat_fp, "V Reg  Addr            Size\n");
    for (i = 0; i < N_ALAT_SETS; i++)
	for (j = 0; j < N_ALAT_ASSOC; j++) {
#ifdef FIXED_ALAT
	    valid = alat[i][j].valid;
	    fpreg = alat[i][j].fpreg;
	    rega  = alat[i][j].rega;
	    paddr = alat[i][j].paddr;
	    size  = alat[i][j].size;
#else
	    valid = (alat + (i * alatAssoc + j))->valid;
	    fpreg = (alat + (i * alatAssoc + j))->fpreg;
	    rega  = (alat + (i * alatAssoc + j))->rega;
	    paddr = (alat + (i * alatAssoc + j))->paddr;
	    size  = (alat + (i * alatAssoc + j))->size;
#endif
	    fprintf(alat_fp, "%u %c%-3u %016llx %u\n",
		    valid, fpreg ? 'f' : 'r', (rega << N_ALAT_SETS_LOG_2) + i,
		    paddr, size);
	}
}
#endif

static alatEntry *alat_lookup(BOOL fpreg, int rega)
{
    alatEntry *e;
    unsigned i;

    rega = fpreg ? PHYS_FR(rega) : PHYS_GR(rega);
#ifdef FIXED_ALAT
    e = &alat[rega & REG_MASK][0];
#else
    e = alat + (rega & REG_MASK) * alatAssoc;
#endif
    for (i = 0; i < N_ALAT_ASSOC; i++, e++)
	if (e->valid &&
	    e->rega == rega >> N_ALAT_SETS_LOG_2 &&
	    e->fpreg == fpreg)
	    return e;
    return NULL;
}

void alat_inval_single_entry(BOOL fpreg, int rega)
{
    alatEntry *e;

#ifdef ALAT_DEBUG
    if (total_insts <= 1600ULL)
fprintf(alat_fp, "invala.e @ %lld: %c%d (%c%d)\n", total_insts,
	fpreg ? 'f' : 'r', rega,
	fpreg ? 'f' : 'r', fpreg ? PHYS_FR(rega) : PHYS_GR(rega));
#endif
    if (!(e = alat_lookup(fpreg, rega)))
	return;
    e->valid = NO;
}

static void alat_insert(alatEntry *e, BOOL fpreg, int rega, ADDR pa,
			unsigned size)
{
    e->valid = YES;
    e->rega = rega >> N_ALAT_SETS_LOG_2;
    e->fpreg = fpreg;
    e->paddr = pa;
    e->size = size;
}

void alat_write(BOOL fpreg, int rega, ADDR pa, unsigned size)
{
    alatEntry *e;
    unsigned i;

#ifndef FIXED_ALAT
    if (!N_ALAT_ASSOC)
	return;
#endif
#ifdef ALAT_DEBUG
alat_display_entries();
    if (total_insts <= 1600ULL)
fprintf(alat_fp, "ld%u.a @ %lld: %c%d (%c%d), %llx\n", size, total_insts,
	fpreg ? 'f' : 'r', rega,
	fpreg ? 'f' : 'r', fpreg ? PHYS_FR(rega) : PHYS_GR(rega), pa);
#endif
    /* Use existing entry if possible */
    if ((e = alat_lookup(fpreg, rega))) {
	rega = fpreg ? PHYS_FR(rega) : PHYS_GR(rega);
	alat_insert(e, fpreg, rega, pa, size);
#ifdef ALAT_DEBUG
alat_display_entries();
#endif
	return;
    }
    rega = fpreg ? PHYS_FR(rega) : PHYS_GR(rega);
#ifdef FIXED_ALAT
    e = &alat[rega & REG_MASK][0];
#else
    e = alat + (rega & REG_MASK) * alatAssoc;
#endif
    for (i = 0; i < N_ALAT_ASSOC; i++, e++)
	if (!e->valid) {
	    alat_insert(e, fpreg, rega, pa, size);
#ifdef ALAT_DEBUG
alat_display_entries();
#endif
	    return;
	}
    /* If all in use, use random entry */
    i = random() & (N_ALAT_ASSOC - 1);
#ifdef ALAT_DEBUG
cmdWarn("ALAT entry replaced.  Row: %d  Entry: %d\n", rega & REG_MASK, i);
#endif
#ifdef FIXED_ALAT
    e = &alat[rega & REG_MASK][i];
#else
    e = alat + ((rega & REG_MASK) * alatAssoc + i);
#endif
    alat_insert(e, fpreg, rega, pa, size);
#ifdef ALAT_DEBUG
alat_display_entries();
#endif
}

/* Return 1 if not found, 0 if found */
int alat_cmp(BOOL fpreg, int rega, BOOL clearit)
{
    alatEntry *e;

#ifdef ALAT_DEBUG
alat_display_entries();
    if (total_insts <= 1600ULL)
fprintf(alat_fp, "chk.a.%s @ %lld: %c%d (%c%d), %s\n",
	clearit ? "clr" : "nc", total_insts,
	fpreg ? 'f' : 'r', rega,
	fpreg ? 'f' : 'r', fpreg ? PHYS_FR(rega) : PHYS_GR(rega),
	alat_lookup(fpreg, rega) ? "found" : "not found");
#endif
    if (!(e = alat_lookup(fpreg, rega)))
	return 1;
    if (clearit)
	e->valid = NO;
    return 0;
}

void alat_inval_multiple_entries(ADDR pa, unsigned size)
{
    ADDR end = pa + size - 1;
#ifdef FIXED_ALAT
    alatEntry *e = &alat[0][0];
#else
    alatEntry *e = alat;
#endif
    unsigned i, j;

#ifdef ALAT_DEBUG
    if (total_insts <= 1600ULL)
fprintf(alat_fp, "st%u @ %lld: %llx\n", size, total_insts, pa);
#endif
    for (i = 0; i < N_ALAT_SETS; i++)
	for (j = 0; j < N_ALAT_ASSOC; j++, e++) {
	    ADDR alat_end = e->paddr + e->size - 1;

	    if (e->valid && alat_end >= pa && e->paddr <= end)
		e->valid = NO;
	}
#ifdef ALAT_DEBUG
alat_display_entries();
#endif
}

void alat_inval_all_entries(void)
{
    unsigned i, j;

#ifdef ALAT_DEBUG
    if (total_insts <= 1600ULL)
fprintf(alat_fp, "invala @ %lld\n", total_insts);
#endif
    for (i = 0; i < N_ALAT_SETS; i++)
	for (j = 0; j < N_ALAT_ASSOC; j++)
#ifdef FIXED_ALAT
	    alat[i][j].valid = NO;
#else
	    (alat + (i * alatAssoc + j))->valid = NO;
#endif
}

static BundlePtr pxx(ADDR a)
{
    unsigned i;
    DWORD *pagebuf;

    i = BitfX(a,52,10);
    pagebuf = pmemLookup(a & CTTAGMASK);
    return (pagebuf) ? (BundlePtr)&pagebuf[i>>1] : NULL;
}

/* XXX - move this to the top of the file? */
#include "predecode.gen.h"
/* XXX - copied from libcore_types.h!! */
#define BADDR(ip)       ((ip) & ~(ADDR)0xF)

BOOL brCall(ADDR a)
{
    BundlePtr b;
    unsigned s;
    TemplateInfoPtr t;
    DecodedInstr instr[SLOTS_PER_BUNDLE];

    if (!(b = pxx(BADDR(a))))
	return 0;
    s = SLOT(a);
    t = bundle_decode(b, instr, 0);
    {BOOL ret;
    ret = t->slot[s].unit == B_Unit &&
	  instr[s].instID >= EM_BR_CALL_SPTK_FEW_B1_TARGET25 &&
	  instr[s].instID <= EM_BR_CALL_DPNT_MANY_CLR_B1_TARGET25;
    return ret;}
}
