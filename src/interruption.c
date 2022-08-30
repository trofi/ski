/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Interruption Handling
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
#include "bits.h"
#include "types.h"
#include "fields.h"
#include "state.h"
#include "sim.h"
#include "simmem.h"
#include "tlb.h"
#include "interruption.h"
#include "libtrace.h"
#include "syscall_api.h"
#include "trace.h"

/* #define RSE_DEBUG */

#define ILLEGAL_OP_CODE			0x0
#define PRIV_OP_CODE			0x10
#define PRIV_REG_CODE			0x20
#define RESERVED_REG_FIELD_CODE		0x30
#define DISABLED_INST_SET_CODE		0x40

#define TPA_CODE			0
#define FC_CODE				1
#define PROBE_CODE			2
#define TAK_CODE			3
#define LFETCH_CODE			4
#define PROBE_FAULT_CODE		5

#define NAT_REG_CODE			0x10
#define NAT_PAGE_CODE			0x20

#define FP_TRAP_CODE			0x1
#define LOWER_PRIVILEGE_TRAP_CODE	0x2
#define TAKEN_BRANCH_TRAP_CODE		0x4
#define SINGLE_STEP_TRAP_CODE		0x8
#define UNIMPL_ADDRESS_TRAP_CODE	0x10

/* ISR bit fields */
/* If these bit fields are used by other source files, the most logical place
   to put them is state.h */

#define ISR_X		BitfR(ISR,31,1)
#define ISR_W		BitfR(ISR,30,1)
#define ISR_R		BitfR(ISR,29,1)
#define ISR_NA		BitfR(ISR,28,1)
#define ISR_SP		BitfR(ISR,27,1)
#define ISR_RS		BitfR(ISR,26,1)
#define ISR_IR		BitfR(ISR,25,1)
#define ISR_NI		BitfR(ISR,24,1)
#define ISR_EI		BitfR(ISR,21,2)
#define ISR_ED		BitfR(ISR,20,1)
#define ISR_NACODE	BitfR(ISR,60,4)

/* Interrupt Delivery Modes */

#define IDM_INT		0
#define IDM_PMI		2
#define IDM_NMI		4
#define IDM_INIT	5
#define IDM_EXTINT	7

#define EXTINT_IRPT	0x00
#define NMI_IRPT	0x02
#define SPURIOUS_IRPT	0x0F

/*##################### Globals - Imports ##################################*/

extern ADDR sigHdlr;
extern void gotoAlarm(void);

/*##################### Functions ##########################################*/

/* Set the PSR on an interruption */
static REG interruptionPSR(void)
{
    REG ipsr = 0;

    X_PSR_RT(ipsr)  = PSR_RT;
    X_PSR_UP(ipsr)  = PSR_UP;
    X_PSR_MFL(ipsr) = PSR_MFL;
    X_PSR_MFH(ipsr) = PSR_MFH;
    X_PSR_PK(ipsr)  = PSR_PK;
    X_PSR_IT(ipsr)  = PSR_IT;
    X_PSR_DT(ipsr)  = PSR_DT;
    X_PSR_MC(ipsr)  = PSR_MC;
    X_PSR_BE(ipsr)  = DCR_BE;
    X_PSR_PP(ipsr)  = DCR_PP;
    return ipsr;
}

/* Set the Interruption Registers for Faults */
/* For ISR, ei and ni are filled in here with the remaining bits passed in */
/* Note:
       if this function were to be merged with setTlbIRs, it must
       be taken into consideration that setFaultIRs do NOT consider atype == 0
       as EXECUTE_ACCESS.  The callers of setFaultIRs expecting a 0 to be
       ignored should use a different value in the new merged routine */
static void setFaultIRs(REG code, Accesstype atype)
{
    if (traceEnb)
	WRT_IRPT_TR();
    total_faults++;
    if (PSR_BN)
	switchBanks();
    if (PSR_IC) {
	IPSR = psr;
	X_PSR_RI(IPSR) = SLOT(ip);
	IIP = PSR_IS ? ip : ip & ~(REG)0xF;
	IFS = 0;
	IIPA &= ~0xF;
    }
    ISR    = code;
    ISR_EI = SLOT(ip);
    ISR_NI = !PSR_IC;
    ISR_IR = cfle;
    ISR_RS = (atype & RSE_ACCESS) != 0;
    ISR_SP = (atype & SPEC_ACCESS) != 0;
    if (atype & NA_ACCESS) {
	ISR_NA = 1;
	switch (atype) {
	    case TPA_ACCESS:
		ISR_NACODE = TPA_CODE;
		break;
	    case FC_ACCESS:
		ISR_NACODE = FC_CODE;
		break;
	    case PROBER_ACCESS:
	    case PROBEW_ACCESS:
		ISR_NACODE = PROBE_CODE;
		break;
	    case TAK_ACCESS:
		ISR_NACODE = TAK_CODE;
		break;
	    case LFETCH_ACCESS:
		ISR_NACODE = LFETCH_CODE;
		break;
	    case PROBERF_ACCESS:
	    case PROBEWF_ACCESS:
	    case PROBERW_ACCESS:
		ISR_NACODE = PROBE_FAULT_CODE;
		break;
	    default:
		break;
	}
    }
    ISR_R  = (atype & READ_ACCESS) != 0;
    ISR_W  = (atype & WRITE_ACCESS) != 0;
    psr = interruptionPSR();
    if (traceEnb)
	WRT_IRPT_TR();
    abi = unixABI;
}

/* Set the Interruption Registers for TLB Faults */
static void setTlbIRs(Accesstype atype)
{
    if (traceEnb)
	WRT_IRPT_TR();
    total_faults++;
    if (PSR_BN)
	switchBanks();
    if (PSR_IC) {
	IPSR = psr;
	X_PSR_RI(IPSR) = SLOT(ip);
	IIP = PSR_IS ? ip : ip & ~(REG)0xF;
	IFS = 0;
	IIPA &= ~0xF;
    }
    ISR    = 0;
    ISR_ED = (atype == EXECUTE_ACCESS || atype & RSE_ACCESS) ? 0 : ITLB_ED;
    ISR_EI = SLOT(ip);
    ISR_NI = !PSR_IC;
    ISR_IR = cfle;
    ISR_RS = (atype & RSE_ACCESS) != 0;
    ISR_SP = (atype & SPEC_ACCESS) != 0;
    if (atype & NA_ACCESS) {
	ISR_NA = 1;
	switch (atype) {
	    case TPA_ACCESS:
		ISR_NACODE = TPA_CODE;
		break;
	    case FC_ACCESS:
		ISR_NACODE = FC_CODE;
		break;
	    case PROBER_ACCESS:
	    case PROBEW_ACCESS:
		ISR_NACODE = PROBE_CODE;
		break;
	    case TAK_ACCESS:
		ISR_NACODE = TAK_CODE;
		break;
	    case LFETCH_ACCESS:
		ISR_NACODE = LFETCH_CODE;
		break;
	    case PROBERF_ACCESS:
	    case PROBEWF_ACCESS:
	    case PROBERW_ACCESS:
		ISR_NACODE = PROBE_FAULT_CODE;
		break;
	    default:
		break;
	}
    }
    ISR_R  = (atype & READ_ACCESS) != 0;
    ISR_W  = (atype & WRITE_ACCESS) != 0;
    ISR_X  = atype == EXECUTE_ACCESS;
    psr = interruptionPSR();
    if (traceEnb)
	WRT_IRPT_TR();
    abi = unixABI;
}

/* Set the Interruption Registers for Traps */
/* For ISR, ei, ni & ir are filled in here with the remaining bits passed in */
static void setTrapIRs(unsigned slot, REG val)
{
    if (traceEnb)
	WRT_IRPT_TR();
    if (PSR_BN)
	switchBanks();
    if (PSR_IC) {
	IPSR = psr;
	X_PSR_RI(IPSR) = PSR_IS ? 0 : SLOT(ip);
	IIP = PSR_IS ? ip : ip & ~(REG)0xF;
	IFS = 0;
	IIPA &= ~0xF;
    }
    ISR    = val;
    ISR_EI = slot;
    ISR_NI = !PSR_IC;
    ISR_IR = cfle;
    if (PSR_SS)
	ISR |= SINGLE_STEP_TRAP_CODE;
    psr = interruptionPSR();
    if (traceEnb)
	WRT_IRPT_TR();
    abi = unixABI;
}

/* Set the Interruption Registers for IA-32 Faults */
/* For ISR, ni is filled in here with the remaining bits passed in */
static void setIA32FaultIRs(REG val)
{
    if (PSR_BN)
	switchBanks();
    if (PSR_IC) {
	IPSR = psr;
	X_PSR_RI(IPSR) = 0;	/* XXX - unspecified in Seamless 2.0 */
	IIPA = IIP = ip;	/* both IIPA and IIP point to the fault instr */
	IFS = 0;
    }
    ISR    = val;
    ISR_NI = !PSR_IC;
    psr = interruptionPSR();
    abi = unixABI;
}

/* Set the Interruption Registers for IA-32 Traps */
/* For ISR, ni is filled in here with the remaining bits passed in */
static void setIA32TrapIRs(REG val, unsigned delta)
{
    if (PSR_BN)
	switchBanks();
    if (PSR_IC) {
	IPSR = psr;
	X_PSR_RI(IPSR) = 0;	/* XXX - unspecified in Seamless 2.0 */
	IIPA = ip;
	IIP = ip + delta;
	IFS = 0;
    }
    ISR    = val;
    ISR_NI = !PSR_IC;
    psr = interruptionPSR();
    abi = unixABI;
}

static void nestedFault(void)
{
    if (traceEnb)
	WRT_IRPT_TR();
    total_faults++;
    if (PSR_BN)
	switchBanks();
    psr = interruptionPSR();
    if (traceEnb)
	WRT_IRPT_TR();
    abi = unixABI;
    ip = IVA + DATA_NESTED_TLB_VECTOR;
}


/**********************/
/* Interrupt Routines */
/**********************/

void externalInterrupt(void)
{
#ifdef RSE_DEBUG
rseDB("External Interrupt");
#endif
    setFaultIRs(0, 0);
    ip = IVA + EXTERNAL_INTERRUPT_VECTOR;
}


/******************/
/* Fault Routines */
/******************/

void alternateInstTlbFault(void)
{
#ifdef RSE_DEBUG
rseDB("Alt. Inst");
#endif
    setTlbIRs(EXECUTE_ACCESS);
    ip = IVA + ALTERNATE_INST_TLB_VECTOR;
}

void vhptInstFault(void)
{
#ifdef RSE_DEBUG
rseDB("VHPT Inst");
#endif
    setTlbIRs(EXECUTE_ACCESS);
    ip = IVA + VHPT_INST_VECTOR;
}

void instTlbFault(void)
{
#ifdef RSE_DEBUG
rseDB("Inst TLB");
#endif
    setTlbIRs(EXECUTE_ACCESS);
    ip = IVA + INST_TLB_VECTOR;
}

void instPageNotPresentFault(void)
{
#ifdef RSE_DEBUG
rseDB("Inst Not Present");
#endif
    setTlbIRs(EXECUTE_ACCESS);
    ip = IVA + INST_PAGE_NOT_PRESENT_VECTOR;
}

void instNatPageConsumptionFault(void)
{
#ifdef RSE_DEBUG
rseDB("Inst NaT Page");
#endif
    setTlbIRs(EXECUTE_ACCESS);
    ISR |= NAT_PAGE_CODE;
    ip = IVA + INST_NAT_PAGE_CONSUMPTION_VECTOR;
}

void instKeyMissFault(void)
{
#ifdef RSE_DEBUG
rseDB("Inst Key Miss");
#endif
    setTlbIRs(EXECUTE_ACCESS);
    ip = IVA + INST_KEY_MISS_VECTOR;
}

void instKeyPermissionFault(void)
{
#ifdef RSE_DEBUG
rseDB("Inst Key Perm");
#endif
    setTlbIRs(EXECUTE_ACCESS);
    ip = IVA + INST_KEY_PERMISSION_VECTOR;
}

void instAccessRightsFault(void)
{
#ifdef RSE_DEBUG
rseDB("Inst Access Rights");
#endif
    setTlbIRs(EXECUTE_ACCESS);
    ip = IVA + INST_ACCESS_RIGHTS_VECTOR;
}

void instAccessBitFault(void)
{
#ifdef RSE_DEBUG
rseDB("Inst Access Bit");
#endif
    setTlbIRs(EXECUTE_ACCESS);
    ip = IVA + INST_ACCESS_BIT_VECTOR;
}

void instDebugFault(void)
{
#ifdef RSE_DEBUG
rseDB("Inst Debug");
#endif
    setTlbIRs(EXECUTE_ACCESS);
    ip = IVA + INST_DEBUG_VECTOR;
}

void ia32ExceptionFault(unsigned vector, unsigned error_code)
{
    if (abi)
#if defined __linux__
    {
	signal_queue_info(4, ip);
	return;
    }
#else /* !defined __linux__ */
	progExit("IA-32 Exception fault - vector %d\n", vector);
#endif /* !defined __linux__ */
    setIA32FaultIRs(((vector == 1) ? 1ULL << 32 : 0) |
		    vector << 16 | error_code);
    ip = IVA + IA32_EXCEPTION_VECTOR;
}

void ia32ExceptionTrap(unsigned vector, unsigned trap_code, unsigned delta)
{
    if (abi)
#if defined __linux__
    {
	signal_queue_info(4, ip);
	return;
    }
#else /* !defined __linux__ */
	progExit("IA-32 Exception trap - vector %d\n", vector);
#endif /* !defined __linux__ */
    setIA32TrapIRs(vector << 16 | trap_code, delta);
    ip = IVA + IA32_EXCEPTION_VECTOR;
}

void ia32InterruptTrap(unsigned vector, unsigned trap_code, unsigned delta)
{
    if (abi)
#if defined __linux__
    {
	signal_queue_info(4, ip);
	return;
    }
#else /* !defined __linux__ */
	progExit("IA-32 Interrupt trap - vector %d\n", vector);
#endif /* !defined __linux__ */
    setIA32TrapIRs(vector << 16 | trap_code, delta);
    ip = IVA + IA32_INTERRUPT_VECTOR;
}

void illegalOpFault(void)
{
#ifdef RSE_DEBUG
rseDB("Illegal Op");
#endif
    if (unixABI) {
#if defined __linux__
    {
	signal_queue_info(4, ip);
	return;
    }
#else /* !defined __linux__ */
	progExit("Illegal operation fault\n");
#endif /* !defined __linux__ */
    }
    setFaultIRs(ILLEGAL_OP_CODE, 0);
    ip = IVA + ILLEGAL_OP_VECTOR;
}

void breakInstFault(int val)
{
    /* eclipse - check for the emulation bridge value */
    if (val == BREAK_EMUL_BRIDGE) {
	emul_bridge_signaled = YES;
    }
    
#ifdef RSE_DEBUG
rseDB("Break Inst");
#endif
    if (unixABI) {
#if defined __linux__
	signal_queue_info(4, ip);
	return;
#else /* !defined __linux__ */
	switch (val) {
	    case 1:
		progExit("Break instruction fault (integer divide by zero)\n");
	    case 2:
		progExit("Break instruction fault (integer overflow)\n");
	    case 4:
		progExit("Break instruction fault (Nil pointer dereference)\n");
	    case 5:
		progExit("Break instruction fault (Misaligned data)\n");
	    default:
		progExit("Break instruction fault\n");
	}
#endif /* !defined __linux__ */
    }
    if (PSR_IC)
	IIM = val;
    setFaultIRs(0, 0);
    ip = IVA + BREAK_INST_VECTOR;
}

void privOpFault(Accesstype atype)
{
#ifdef RSE_DEBUG
rseDB("Priv Op");
#endif
    if (unixABI)
#if defined __linux__
    {
	signal_queue_info(4, ip);
	return;
    }
#else /* !defined __linux__ */
	progExit("Privileged operation fault\n");
#endif /* !defined __linux__ */
    setFaultIRs(PRIV_OP_CODE, atype);
    ip = IVA + PRIV_OP_VECTOR;
}

void disabledFpregFault(int code, Accesstype atype)
{
#ifdef RSE_DEBUG
rseDB("Disabled FP");
#endif
    code &= PSR_DFH << 1 | PSR_DFL;
    setFaultIRs(code, atype);
    ip = IVA + DISABLED_FPREG_VECTOR;
}

void disabledInstSetTransitionFault(void)
{
    setFaultIRs(DISABLED_INST_SET_CODE, 0);
    ip = IVA + DISABLED_INST_SET_TRANSITION_VECTOR;
}

void regNatConsumptionFault(Accesstype atype)
{
#ifdef RSE_DEBUG
rseDB("Reg NaT");
#endif
    if (unixABI) {
#if defined __linux__
    {
	signal_queue_info(4, ip);
	return;
    }
#else /* !defined __linux__ */
	progExit("Register NaT Consumption fault\n");
#endif /* !defined __linux__ */
    }
    setFaultIRs(NAT_REG_CODE, atype);
    ip = IVA + REG_NAT_CONSUMPTION_VECTOR;
}

void reservedRegFieldFault(Accesstype atype)
{
#ifdef RSE_DEBUG
rseDB("Reserved Reg/Field");
#endif
    if (unixABI) {
#if defined __linux__
    {
	signal_queue_info(4, ip);
	return;
    }
#else /* !defined __linux__ */
	progExit("Reserved register/field fault\n");
#endif /* !defined __linux__ */
    }
    setFaultIRs(RESERVED_REG_FIELD_CODE, atype);
    ip = IVA + RESERVED_REG_FIELD_VECTOR;
}

void privRegFault(void)
{
#ifdef RSE_DEBUG
rseDB("Priv Reg");
#endif
    if (unixABI)
#if defined __linux__
    {
	signal_queue_info(4, ip);
	return;
    }
#else /* !defined __linux__ */
	progExit("Privileged register fault\n");
#endif /* !defined __linux__ */
    setFaultIRs(PRIV_REG_CODE, 0);
    ip = IVA + PRIV_REG_VECTOR;
}

void specOpFault(int val, int code)
{
#ifdef RSE_DEBUG
rseDB("Spec Op");
#endif
    if (PSR_IC)
	IIM = val;
    setFaultIRs(code, 0);
    ip = IVA + SPECULATIVE_OP_VECTOR;
}

void alternateDataTlbFault(Accesstype atype)
{
#ifdef RSE_DEBUG
rseDB("Alt. Data");
#endif
    if (atype == TAK_ACCESS)
	return;
    if (PSR_IC) {
	setTlbIRs(atype);
	ip = IVA + ALTERNATE_DATA_TLB_VECTOR;
    } else
	nestedFault();
}

void vhptDataFault(Accesstype atype)
{
#ifdef RSE_DEBUG
rseDB("VHPT Data");
#endif
    if (atype == TAK_ACCESS)
	return;
    if (PSR_IC) {
	setTlbIRs(atype);
	ip = IVA + VHPT_DATA_VECTOR;
    } else
	nestedFault();
}

void dataTlbFault(Accesstype atype)
{
#ifdef RSE_DEBUG
rseDB("Data TLB");
#endif
    if (atype == TAK_ACCESS)
	return;
    if (PSR_IC) {
	setTlbIRs(atype);
	ip = IVA + DATA_TLB_VECTOR;
    } else
	nestedFault();
}

void dataPageNotPresentFault(Accesstype atype)
{
#ifdef RSE_DEBUG
rseDB("Data Not Present");
#endif
    setTlbIRs(atype);
    ip = IVA + DATA_PAGE_NOT_PRESENT_VECTOR;
}

void dataNatPageConsumptionFault(Accesstype atype)
{
#ifdef RSE_DEBUG
rseDB("Data NaT Page");
#endif
    setTlbIRs(atype);
    ISR_ED     = NO;	/* ISR.ed = 0 for this kind of data TLB faults */
    ISR |= NAT_PAGE_CODE;
    ip = IVA + DATA_NAT_PAGE_CONSUMPTION_VECTOR;
}

void dataKeyMissFault(Accesstype atype)
{
#ifdef RSE_DEBUG
rseDB("Data Key Miss");
#endif
    setTlbIRs(atype);
    ip = IVA + DATA_KEY_MISS_VECTOR;
}

void dataKeyPermissionFault(Accesstype atype)
{
#ifdef RSE_DEBUG
rseDB("Data Key Perm");
#endif
    setTlbIRs(atype);
    ip = IVA + DATA_KEY_PERMISSION_VECTOR;
}

void dataAccessRightsFault(Accesstype atype)
{
#ifdef RSE_DEBUG
rseDB("Data Access Rights");
#endif
    setTlbIRs(atype);
    ip = IVA + DATA_ACCESS_RIGHTS_VECTOR;
}

void dataDirtyBitFault(Accesstype atype)
{
#ifdef RSE_DEBUG
rseDB("Data Dirty Bit");
#endif
    setTlbIRs(atype);
    ip = IVA + DATA_DIRTY_BIT_VECTOR;
}

void dataAccessBitFault(Accesstype atype)
{
#ifdef RSE_DEBUG
rseDB("Data Access Bit");
#endif
    setTlbIRs(atype);
    ip = IVA + DATA_ACCESS_BIT_VECTOR;
}

void dataDebugFault(Accesstype atype)
{
#ifdef RSE_DEBUG
rseDB("Data Debug");
#endif
    setTlbIRs(atype);
    ip = IVA + DATA_DEBUG_VECTOR;
}

void unalignedDataFault(Accesstype atype)
{
#ifdef RSE_DEBUG
rseDB("Unaligned");
#endif
    if (unixABI) {
#if defined __linux__
    {
	signal_queue_info(4, ip);
	return;
    }
#else /* !defined __linux__ */
	progExit("Unaligned Data fault\n");
#endif /* !defined __linux__ */
    }
    setTlbIRs(atype);
    ip = IVA + UNALIGNED_DATA_REFERENCE_VECTOR;
}

void unsupportedDataReferenceFault(void)
{
    setTlbIRs(MEM_SEMA_ACCESS);
    ip = IVA + UNSUPPORTED_DATA_REFERENCE_VECTOR;
}

void fpExceptionFault(int code)
{
#ifdef RSE_DEBUG
rseDB("FP Fault");
#endif
    if (unixABI) {
#if defined __linux__
    {
	signal_queue_info(4, ip);
	return;
    }
#else /* !defined __linux__ */
	progExit("FP exception fault\n");
#endif /* !defined __linux__ */
    }
    setFaultIRs(code >> 6, 0);
    ip = IVA + FP_EXCEPTION_FAULT_VECTOR;
}

/* XXX -- should setTrapIRs handle clearing icp, extint, and intrsim? */

void fpExceptionTrap(unsigned delta, int code)
{
    unsigned slot = SLOT(ip);

#ifdef RSE_DEBUG
rseDB("FP Trap");
#endif
    if (unixABI)
#if defined __linux__
    {
	signal_queue_info(4, ip);
	return;
    }
#else /* !defined __linux__ */
	{ if (sigHdlr != ~0ULL) {
	    gotoAlarm();
	    ip -= 4;
	    icp = NULL;
	    return;
	} else
	    progExit("FP exception trap (SIGFPE)\n"); }
#endif /* !defined __linux__ */
    PSR_IA = PSR_ID = PSR_DA = PSR_DD = PSR_ED = NO;
    if (PSR_IC)
	IIPA = ip;
    ip += delta * 4;
    setTrapIRs(slot, code << 7 | FP_TRAP_CODE);
    ip = IVA + FP_EXCEPTION_TRAP_VECTOR;
}

void unimplInstructionAddressTrap(unsigned slot, Status st)
{
    REG isrcode = UNIMPL_ADDRESS_TRAP_CODE;

    if (st != StRFI)
	PSR_IA = PSR_ID = PSR_DA = PSR_DD = PSR_ED = NO;
    if (st == StTrap) {		/* can't be an FP trap if IP fall-through
				   unimpl check is not done in iCycleSys */
	isrcode |= LOWER_PRIVILEGE_TRAP_CODE;
	isrcode |= PSR_TB ? TAKEN_BRANCH_TRAP_CODE : 0;
    }

    if (st == StTakenBr && PSR_TB)
	isrcode |= TAKEN_BRANCH_TRAP_CODE;

    ip = PSR_IT ? sxtUnimplVA(ip) : zxtUnimplPA(ip);
    setTrapIRs(slot, isrcode);
    ip = IVA + UNIMPL_INSTRUCTION_ADDRESS_VECTOR;
}

void lowerPrivilegeTransferTrap(unsigned slot)
{
    PSR_IA = PSR_ID = PSR_DA = PSR_DD = PSR_ED = NO;
    if (PSR_TB)
	setTrapIRs(slot, LOWER_PRIVILEGE_TRAP_CODE|TAKEN_BRANCH_TRAP_CODE);
    else
	setTrapIRs(slot, LOWER_PRIVILEGE_TRAP_CODE);
    ip = IVA + LOWER_PRIVILEGE_VECTOR;
    icp = NULL;
/* XXX -- is this needed?
    intrsim = extint = NO;
 */
}

void takenBranchTrap(unsigned slot)
{
    setTrapIRs(slot, TAKEN_BRANCH_TRAP_CODE);
    ip = IVA + TAKEN_BRANCH_VECTOR;
    icp = NULL;
/* XXX -- is this needed?
    intrsim = extint = NO;
 */
}

void singleStepTrap(unsigned slot)
{
    setTrapIRs(slot, 0);
    ip = IVA + SINGLE_STEP_VECTOR;
    icp = NULL;
    intrsim = extint = NO;
}

void virtualizationFault(void)
{
#ifdef RSE_DEBUG
rseDB("Virtualization");
#endif
    if (unixABI)
	progExit("Virtualization fault\n");
    setFaultIRs(0, 0);
    ip = IVA + VIRTUALIZATION_VECTOR;
}


/*******************************/
/* External interrupt routines */
/*******************************/

static REG inserv[4] = { 0 };

static BOOL higherInserv(unsigned vector)
{
    int i, j;
    BOOL found = YES;
    unsigned hiInserv;

    if (vector < 16)
	vector += 256;		/* bias to put in proper priority order */

    if (BitfX(inserv[0],61,1))			/* NMI */
	hiInserv = NMI_IRPT + 256;
    else if (BitfX(inserv[0],63,1))		/* ExtInt */
	hiInserv = EXTINT_IRPT + 256;
    else {
	found = NO;
	for (j = 3; !found && j >= 0; j--)	/* vectored irpts */
	    for (i = 0; i < 64; i++)
		if (BitfX(inserv[j],i,1)) {
		    hiInserv = j << 6 | (63 - i);
		    found = YES;
		    break;
		}
    }

    return found && hiInserv >= vector;
}

static BOOL maskedIrpt(unsigned vector)
{
    return TPR_MMI || higherInserv(vector) ||
	   (vector != EXTINT_IRPT && (vector >> 4) <= TPR_MIC);
}

BOOL irptMsg(ADDR adr)
/* XXX - this function needs to be MP'd.  Maybe it should return the number
	of the processor receiving the interrupt instead of YES/NO */
{
    return BitfX(adr,44,16) == BitfX(LID,32,16);
}

/*--------------------------------------------------------------------------
 *  pendInterrupt - This routine sets the proper IRR bit and calls the
 *  acceptIrpt function to set IVR and perhaps trigger an interrupt.
 *--------------------------------------------------------------------------*/
void pendIrpt(REG irptData)
{
    unsigned vector = VECTOR(irptData), reg, bit;
    REG *p = &IRR0;		/* assume IRR0-3 are contiguous */

    if (DM(irptData) == IDM_NMI)
	vector = NMI_IRPT;
    else if (DM(irptData) == IDM_EXTINT)
	vector = EXTINT_IRPT;

    if (vector < 16 && vector != NMI_IRPT && vector != EXTINT_IRPT)
	return;			/* ignore vectors 1 and 3 - 15 */

    reg = vector >> 6 & 0x3;
    bit = vector & 0x3F;
    *(p + reg) |= (REG)1 << bit;

    if (acceptIrpt())
	intrsim = extint = YES;
}

/*--------------------------------------------------------------------------
 * acceptInterrupt - Determine if the highest priority, pending, unmasked
 *  external interrupt should cause an interupt.  The IVR is also set.
 *--------------------------------------------------------------------------*/
BOOL acceptIrpt(void)
{
    REG *p = &IRR3;		/* assume IRR0-3 are contiguous */
    unsigned vector;
    int i, j;
    BOOL found = NO;

    IVR = SPURIOUS_IRPT;
    if (BitfX(IRR0,61,1))				/* NMI */
	IVR = NMI_IRPT;
    else if (BitfX(IRR0,63,1) && !maskedIrpt(0))	/* ExtInt */
	IVR = EXTINT_IRPT;
    else
	for (j = 3; !found && j >= 0; j--, p--) {	/* vectored irpts */
	    if (!*p)
		continue;
	    for (i = 0; i < 64; i++)
		if (BitfX(*p,i,1)) {
		    vector = j << 6 | (63 - i);
		    if (!maskedIrpt(vector))
			IVR = vector;
		    found = YES;
		    break;
		}
	}

    return PSR_I && (IVR != SPURIOUS_IRPT);
}

/*--------------------------------------------------------------------------
 *  serviceInterrupt - This routine clears the proper IRR bit, sets the
 *  proper inserv bit and calls the acceptIrpt function to set IVR.
 *--------------------------------------------------------------------------*/
void serviceIrpt(void)
{
    unsigned reg = IVR >> 6 & 0x3;
    unsigned bit = IVR & 0x3f;
    REG *p = &IRR0;		/* assume IRR0-3 are contiguous */

    if (IVR >= 16 || IVR == NMI_IRPT || IVR == EXTINT_IRPT) {
	*(p + reg) &= ~((REG)1 << bit);
	*(inserv + reg) |= (REG)1 << bit;
    }

    (void)acceptIrpt();	       /* to compute new IVR value */
}

/*--------------------------------------------------------------------------
 *  completeInterrupt - This routine clears the proper inserv bit and calls
 *  the acceptIrpt function to set IVR and perhaps trigger an interrupt.
 *--------------------------------------------------------------------------*/
void completeIrpt(void)
{
    int i, j;
    BOOL found = NO;

    if (BitfX(inserv[0],61,1))			/* NMI */
	BitfR(inserv[0],61,1) = 0;
    else if (BitfX(inserv[0],63,1))		/* ExtInt */
	BitfR(inserv[0],63,1) = 0;
    else
	for (j = 3; !found && j >= 0; j--) {	/* vectored irpts */
	    for (i = 0; i < 64; i++)
		if (BitfX(inserv[j],i,1)) {
		    inserv[j] &= ~((REG)1 << (63 - i));
		    found = YES;
		    break;
		}
	}

    if (acceptIrpt())
	intrsim = extint = YES;
}
