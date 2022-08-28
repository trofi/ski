/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Interruption Handling Header
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
#ifndef _SKI_INTERRUPTION_H
#define _SKI_INTERRUPTION_H

#include "simmem.h"

/* IA-64 Interruption Vector Offsets */
#define VHPT_VECTOR				0x0000
#define PAGE_NOT_PRESENT_VECTOR			0x5000
#define KEY_PERMISSION_VECTOR			0x5100
#define GENERAL_EXCEPTION_VECTOR		0x5400
#define NAT_CONSUMPTION_VECTOR			0x5600
#define DEBUG_VECTOR				0x5900

#define EXTERNAL_INTERRUPT_VECTOR		0x3000
#define ALTERNATE_INST_TLB_VECTOR		0x0C00
#define VHPT_INST_VECTOR			VHPT_VECTOR
#define INST_TLB_VECTOR				0x0400
#define INST_PAGE_NOT_PRESENT_VECTOR		PAGE_NOT_PRESENT_VECTOR
#define INST_NAT_PAGE_CONSUMPTION_VECTOR	NAT_CONSUMPTION_VECTOR
#define INST_KEY_MISS_VECTOR			0x1800
#define INST_KEY_PERMISSION_VECTOR		KEY_PERMISSION_VECTOR
#define INST_ACCESS_RIGHTS_VECTOR		0x5200
#define INST_ACCESS_BIT_VECTOR			0x2400
#define INST_DEBUG_VECTOR			DEBUG_VECTOR
#define IA32_EXCEPTION_VECTOR			0x6900
#define ILLEGAL_OP_VECTOR			GENERAL_EXCEPTION_VECTOR
#define BREAK_INST_VECTOR			0x2C00
#define PRIV_OP_VECTOR				GENERAL_EXCEPTION_VECTOR
#define DISABLED_FPREG_VECTOR			0x5500
#define DISABLED_INST_SET_TRANSITION_VECTOR	GENERAL_EXCEPTION_VECTOR
#define REG_NAT_CONSUMPTION_VECTOR		NAT_CONSUMPTION_VECTOR
#define RESERVED_REG_FIELD_VECTOR		GENERAL_EXCEPTION_VECTOR
#define PRIV_REG_VECTOR				GENERAL_EXCEPTION_VECTOR
#define SPECULATIVE_OP_VECTOR			0x5700
#define DATA_NESTED_TLB_VECTOR			0x1400
#define ALTERNATE_DATA_TLB_VECTOR		0x1000
#define VHPT_DATA_VECTOR			VHPT_VECTOR
#define DATA_TLB_VECTOR				0x0800
#define DATA_PAGE_NOT_PRESENT_VECTOR		PAGE_NOT_PRESENT_VECTOR
#define DATA_NAT_PAGE_CONSUMPTION_VECTOR	NAT_CONSUMPTION_VECTOR
#define DATA_KEY_MISS_VECTOR			0x1C00
#define DATA_KEY_PERMISSION_VECTOR		KEY_PERMISSION_VECTOR
#define DATA_ACCESS_RIGHTS_VECTOR		0x5300
#define DATA_DIRTY_BIT_VECTOR			0x2000
#define DATA_ACCESS_BIT_VECTOR			0x2800
#define DATA_DEBUG_VECTOR			DEBUG_VECTOR
#define UNALIGNED_DATA_REFERENCE_VECTOR		0x5A00
#define UNSUPPORTED_DATA_REFERENCE_VECTOR	0x5B00
#define FP_EXCEPTION_FAULT_VECTOR		0x5C00
#define FP_EXCEPTION_TRAP_VECTOR		0x5D00
#define IA32_INTERCEPT_VECTOR			0x6A00
#define IA32_INTERRUPT_VECTOR			0x6B00
#define LOWER_PRIVILEGE_VECTOR			0x5E00
#define UNIMPL_INSTRUCTION_ADDRESS_VECTOR	LOWER_PRIVILEGE_VECTOR
#define TAKEN_BRANCH_VECTOR			0x5F00
#define SINGLE_STEP_VECTOR			0x6000
#define VIRTUALIZATION_VECTOR			0x6100

/* IA-32 Exception Vectors */
#define IA32_DIVIDE_EXCEPTION			 0
#define IA32_DEBUG_EXCEPTION			 1
#define IA32_BREAK_EXCEPTION			 3
#define IA32_OVERFLOW_EXCEPTION			 4
#define IA32_BOUNDS_EXCEPTION			 5
#define IA32_DEVICE_NOT_AVAILABLE_EXCEPTION	 7
#define IA32_SEGMENT_NOT_PRESENT_EXCEPTION	11
#define IA32_STACK_EXCEPTION			12
#define IA32_GENERAL_PROTECTION_EXCEPTION	13
#define IA32_FLOATING_POINT_EXCEPTION		16
#define IA32_ALIGNMENT_CHECK_EXCEPTION		17

/* IA-32 Intercept Vectors */
#define IA32_INSTRUCTION_INTERCEPT		 0
#define IA32_GATE_INTERCEPT			 1
#define IA32_SYSTEM_FLAG_INTERCEPT		 2
#define IA32_LOCK_INTERCEPT			 4

void externalInterrupt(void);
void alternateInstTlbFault(void);
void vhptInstFault(void);
void instTlbFault(void);
void instPageNotPresentFault(void);
void instNatPageConsumptionFault(void);
void instKeyMissFault(void);
void instKeyPermissionFault(void);
void instAccessRightsFault(void);
void instAccessBitFault(void);
void instDebugFault(void);
void ia32ExceptionFault(unsigned vector, unsigned error_code);
void illegalOpFault(void);
void breakInstFault(int val);
void privOpFault(Accesstype atype);
void disabledFpregFault(int code, Accesstype atype);
void disabledInstSetTransitionFault(void);
void regNatConsumptionFault(Accesstype atype);
void reservedRegFieldFault(Accesstype atype);
void privRegFault(void);
void specOpFault(int val, int code);
void alternateDataTlbFault(Accesstype atype);
void vhptDataFault(Accesstype atype);
void dataTlbFault(Accesstype atype);
void dataPageNotPresentFault(Accesstype atype);
void dataNatPageConsumptionFault(Accesstype atype);
void dataKeyMissFault(Accesstype atype);
void dataKeyPermissionFault(Accesstype atype);
void dataAccessRightsFault(Accesstype atype);
void dataDirtyBitFault(Accesstype atype);
void dataAccessBitFault(Accesstype atype);
void dataDebugFault(Accesstype atype);
void unalignedDataFault(Accesstype atype);
void unsupportedDataReferenceFault(void);
void fpExceptionFault(int code);
void fpExceptionTrap(unsigned delta, int code);
void ia32ExceptionTrap(unsigned vector, unsigned trap_code, unsigned delta);
void ia32InterceptTrap(void);
void ia32InterruptTrap(unsigned vector, unsigned trap_code, unsigned delta);
void unimplInstructionAddressTrap(unsigned slot, Status st);
void lowerPrivilegeTransferTrap(unsigned slot);
void takenBranchTrap(unsigned slot);
void singleStepTrap(unsigned slot);
void virtualizationFault(void);

BOOL irptMsg(ADDR adr);
void pendIrpt(REG irptData);
BOOL acceptIrpt(void);
void serviceIrpt(void);
void completeIrpt(void);

extern BOOL intrsim, extint;

#endif	/* _SKI_INTERRUPTION_H */
