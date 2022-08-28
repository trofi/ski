/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator IA Execution Header
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
#ifndef _SKI_IA_EXEC_H
#define _SKI_IA_EXEC_H

#include "ia_types.h"
#include "sim.h"
#include "types.h"

#define IPfromEIP(eip)		((ADDR)((ADDR4)(eip) + (ADDR4)CSD.base))
#define EIPfromIP(ip)		((ADDR4)(ip) - (ADDR4)CSD.base)

#define OpAddrSizeMask		0x1
#define StackAddrSizeMask	0x2

#define OpAddrSize(mode)	(((mode) & OpAddrSizeMask) * 2 + 2)
#define StackAddrSize(mode)	(((mode) & StackAddrSizeMask) + 2)

/* Separate byte-per-flag for frequently accessed parts of EFLAGS.
 * Copied in to/out of real EFLAGS on transitions to/from EM mode by
 * eflagsFromArithFlags() & arithFlagsFromEflags().
 */

typedef struct {
    BYTE of;
    BYTE sf;
    BYTE zf;
    BYTE af;
    BYTE pf;
    BYTE cf;
} ArithFlags, *ArithFlagsPtr;

/* IA Execution Utility Function Prototypes */

void setIAmode(void);
BYTE getIAmode(void);
void eflagsFromArithFlags(void);
void arithFlagsFromEflags(void);
void setEIPfromIP(void);
void setSegGRsFromARs(void);
void setSegARsFromGRs(void);

/* IA Execution Function Prototypes */

/* These are in alphabetical order with one function corresponding to each
 * "instruction page" in the Pentium Pro Programmer's Reference Manual.
 * The functions use the names in the manual, in lower-case, with "IAEx"
 * appended.  Some "special" functions (like reserved & unimp) are at the
 * end of the list.
 */

Status aaaIAEx(IAinstInfoPtr info);
Status aadIAEx(IAinstInfoPtr info);
Status aamIAEx(IAinstInfoPtr info);
Status aasIAEx(IAinstInfoPtr info);
Status adcIAEx(IAinstInfoPtr info);
Status addIAEx(IAinstInfoPtr info);
Status andIAEx(IAinstInfoPtr info);
Status arplIAEx(IAinstInfoPtr info);
Status boundIAEx(IAinstInfoPtr info);
Status bsfIAEx(IAinstInfoPtr info);
Status bsrIAEx(IAinstInfoPtr info);
Status bswapIAEx(IAinstInfoPtr info);
Status btIAEx(IAinstInfoPtr info);
Status btcIAEx(IAinstInfoPtr info);
Status btrIAEx(IAinstInfoPtr info);
Status btsIAEx(IAinstInfoPtr info);
/* call has been split into 4 functions */
Status call_near_relIAEx(IAinstInfoPtr info);
Status call_near_indIAEx(IAinstInfoPtr info);
Status call_far_absIAEx(IAinstInfoPtr info);
Status call_far_indIAEx(IAinstInfoPtr info);
Status cbwIAEx(IAinstInfoPtr info);
Status clcIAEx(IAinstInfoPtr info);
Status cldIAEx(IAinstInfoPtr info);
Status cliIAEx(IAinstInfoPtr info);
Status cltsIAEx(IAinstInfoPtr info);
Status cmcIAEx(IAinstInfoPtr info);
Status cmovccIAEx(IAinstInfoPtr info);
Status cmpIAEx(IAinstInfoPtr info);
Status cmpsIAEx(IAinstInfoPtr info);
Status cmpxchgIAEx(IAinstInfoPtr info);
Status cmpxchg8bIAEx(IAinstInfoPtr info);
Status cpuidIAEx(IAinstInfoPtr info);
Status cwdIAEx(IAinstInfoPtr info);
Status daaIAEx(IAinstInfoPtr info);
Status dasIAEx(IAinstInfoPtr info);
Status decIAEx(IAinstInfoPtr info);
Status divIAEx(IAinstInfoPtr info);
Status enterIAEx(IAinstInfoPtr info);
/* XXX - add FP ops */
Status hltIAEx(IAinstInfoPtr info);
Status idivIAEx(IAinstInfoPtr info);
Status imulIAEx(IAinstInfoPtr info);
Status inIAEx(IAinstInfoPtr info);
Status incIAEx(IAinstInfoPtr info);
Status insIAEx(IAinstInfoPtr info);
Status intIAEx(IAinstInfoPtr info);
Status intoIAEx(IAinstInfoPtr info);
Status invdIAEx(IAinstInfoPtr info);
Status invlpgIAEx(IAinstInfoPtr info);
Status iretIAEx(IAinstInfoPtr info);
Status jccIAEx(IAinstInfoPtr info);
/* jmp has been split into 4 functions */
Status jmp_near_relIAEx(IAinstInfoPtr info);
Status jmp_near_indIAEx(IAinstInfoPtr info);
Status jmp_far_absIAEx(IAinstInfoPtr info);
Status jmp_far_indIAEx(IAinstInfoPtr info);
/* jmpe has been split into 2 functions */
Status jmpe_relIAEx(IAinstInfoPtr info);
Status jmpe_indIAEx(IAinstInfoPtr info);
Status lahfIAEx(IAinstInfoPtr info);
Status larIAEx(IAinstInfoPtr info);
Status leaIAEx(IAinstInfoPtr info);
Status leaveIAEx(IAinstInfoPtr info);
Status lgdtIAEx(IAinstInfoPtr info);
Status lidtIAEx(IAinstInfoPtr info);
Status lldtIAEx(IAinstInfoPtr info);
Status lmswIAEx(IAinstInfoPtr info);
Status load_far_ptrIAEx(IAinstInfoPtr info);
Status lockIAEx(IAinstInfoPtr info);
Status lodsIAEx(IAinstInfoPtr info);
Status loopIAEx(IAinstInfoPtr info);
Status lslIAEx(IAinstInfoPtr info);
Status ltrIAEx(IAinstInfoPtr info);
Status movIAEx(IAinstInfoPtr info);
Status movCRIAEx(IAinstInfoPtr info);
Status movDRIAEx(IAinstInfoPtr info);
Status movsIAEx(IAinstInfoPtr info);
/* movsx has been split into 2 functions */
Status movsx8IAEx(IAinstInfoPtr info);
Status movsx16IAEx(IAinstInfoPtr info);
/* movzx has been split into 2 functions */
Status movzx8IAEx(IAinstInfoPtr info);
Status movzx16IAEx(IAinstInfoPtr info);
Status mulIAEx(IAinstInfoPtr info);
Status negIAEx(IAinstInfoPtr info);
Status nopIAEx(IAinstInfoPtr info);
Status notIAEx(IAinstInfoPtr info);
Status orIAEx(IAinstInfoPtr info);
Status outIAEx(IAinstInfoPtr info);
Status outsIAEx(IAinstInfoPtr info);
Status popIAEx(IAinstInfoPtr info);
Status popaIAEx(IAinstInfoPtr info);
Status popfIAEx(IAinstInfoPtr info);
Status pushIAEx(IAinstInfoPtr info);
Status pushaIAEx(IAinstInfoPtr info);
Status pushfIAEx(IAinstInfoPtr info);
Status rclIAEx(IAinstInfoPtr info);
Status rcrIAEx(IAinstInfoPtr info);
Status rdmsrIAEx(IAinstInfoPtr info);
Status rdpmcIAEx(IAinstInfoPtr info);
Status rdtscIAEx(IAinstInfoPtr info);
/* rep has one function per string instruction */
Status rep_insIAEx(IAinstInfoPtr info);
Status rep_lodsIAEx(IAinstInfoPtr info);
Status rep_movsIAEx(IAinstInfoPtr info);
Status rep_outsIAEx(IAinstInfoPtr info);
Status rep_stosIAEx(IAinstInfoPtr info);
Status repe_cmpsIAEx(IAinstInfoPtr info);
Status repe_scasIAEx(IAinstInfoPtr info);
Status repne_cmpsIAEx(IAinstInfoPtr info);
Status repne_scasIAEx(IAinstInfoPtr info);
Status ret_nearIAEx(IAinstInfoPtr info);
Status ret_farIAEx(IAinstInfoPtr info);
Status rolIAEx(IAinstInfoPtr info);
Status rorIAEx(IAinstInfoPtr info);
Status rsmIAEx(IAinstInfoPtr info);
Status sahfIAEx(IAinstInfoPtr info);
Status salIAEx(IAinstInfoPtr info);
Status sarIAEx(IAinstInfoPtr info);
Status sbbIAEx(IAinstInfoPtr info);
Status scasIAEx(IAinstInfoPtr info);
Status setccIAEx(IAinstInfoPtr info);
Status sgdtIAEx(IAinstInfoPtr info);
Status shlIAEx(IAinstInfoPtr info);
Status shld_immIAEx(IAinstInfoPtr info);
Status shld_CLIAEx(IAinstInfoPtr info);
Status shrIAEx(IAinstInfoPtr info);
Status shrd_immIAEx(IAinstInfoPtr info);
Status shrd_CLIAEx(IAinstInfoPtr info);
Status sidtIAEx(IAinstInfoPtr info);
Status sldtIAEx(IAinstInfoPtr info);
Status smswIAEx(IAinstInfoPtr info);
Status stcIAEx(IAinstInfoPtr info);
Status stdIAEx(IAinstInfoPtr info);
Status stiIAEx(IAinstInfoPtr info);
Status stosIAEx(IAinstInfoPtr info);
Status strIAEx(IAinstInfoPtr info);
Status subIAEx(IAinstInfoPtr info);
Status testIAEx(IAinstInfoPtr info);
Status ud2IAEx(IAinstInfoPtr info);
Status verrIAEx(IAinstInfoPtr info);
Status verwIAEx(IAinstInfoPtr info);
Status waitIAEx(IAinstInfoPtr info);
Status wbinvdIAEx(IAinstInfoPtr info);
Status wrmsrIAEx(IAinstInfoPtr info);
Status xaddIAEx(IAinstInfoPtr info);
Status xchgIAEx(IAinstInfoPtr info);
Status xlatIAEx(IAinstInfoPtr info);
Status xorIAEx(IAinstInfoPtr info);

/* "special" functions */
Status unimpIAEx(IAinstInfoPtr info);
Status reservedIAEx(IAinstInfoPtr info);

#endif	/* _SKI_IA_EXEC_H */
