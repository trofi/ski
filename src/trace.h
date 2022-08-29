/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Internal Trace Constants and Prototypes
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
#ifndef _SKI_TRACE_H
#define _SKI_TRACE_H

#include <stdio.h>

#include "libtrace.h"
#include "types.h"

/* Constants */

/* trc_flags values */
#define FIRST_TREC	BitfR32(trc_flags, 31, 1)
#define BR_FLAG		BitfR32(trc_flags, 30, 1)
#define CSWITCH_FLAG	BitfR32(trc_flags, 29, 1)
#define IRPT_FLAG	BitfR32(trc_flags, 28, 1)
#define DISC_FLAG	BitfR32(trc_flags, 27, 1)
#define WRT_IOFFSET	BitfR32(trc_flags, 27, 5)

#define TREC_SIZE	32	/* Make larger than any atomic trace record */

/* Typedefs */


/* Globals */

extern unsigned traceEnb;
extern FILE *tracef;
extern unsigned trc_flags;

extern Capsule_Trec		capsule_trec;
extern Interruption_Trec	irpt_trec;
extern Discontinuity_Trec	disc_trec;
extern Virt_Cycle_Trec		vcycle_trec;
extern Ioffset_Trec		ioffset_trec;
extern Bundle_Trec		bundle_trec;
extern Branch_Trec		branch_trec;
extern Doffset_Trec		doffset_trec;
extern Prev_Pfs_Trec		prev_pfs_trec;
extern Br_Pred_Trec		br_pred_trec;

/* Functions */

void traceArTgt(REG ar);
void traceBrPred(REG br);
void tracePrevPFS(REG prevPfs);

/* Macros */

/* Used in sim.c */
#define WRT_CAPSULE_TR()						\
do {									\
    capsule_trec.pred    = PrRd(qp);					\
    capsule_trec.slot     = SLOT(ip);					\
    (void)traceWrite(tracef, &capsule_trec);				\
} while (0)

/* Used in trace.c */
#define WRT_DISC_TR()							\
do {									\
    DISC_FLAG = 1;							\
    (void)traceWrite(tracef, &disc_trec);				\
} while (0)

/* Used in trace.c */
#define WRT_VCYCLE_TR()							\
do {									\
    vcycle_trec.cycle_incr = total_insts - traceVcy;			\
    (void)traceWrite(tracef, &vcycle_trec);				\
} while (0)

/* Used in sim.c */
#define WRT_BUNDLE_TR()							\
do {									\
    DWORD bndl[2];							\
									\
    if (!SLOT(ip) || WRT_IOFFSET) {					\
	(void)memMIRd(ip & ~(ADDR)0xF, bndl);				\
	bundle_trec.bundle[0] = WD0(bndl[0]);				\
	bundle_trec.bundle[1] = WD1(bndl[0]);				\
	bundle_trec.bundle[2] = WD0(bndl[1]);				\
	bundle_trec.bundle[3] = WD1(bndl[1]);				\
	(void)traceWrite(tracef, &bundle_trec);				\
	/* WRT_IOFFSET is cleared in WRT_IOFFSET_TR() */		\
    }									\
} while (0)

/* Used in sim.c */
#define WRT_IOFFSET_TR()						\
do {									\
    if (WRT_IOFFSET) {							\
	ioffset_trec.ioffset = ip;					\
	BitfR(ioffset_trec.ioffset,60,4) = 0;				\
	(void)traceWrite(tracef, &ioffset_trec);			\
	WRT_IOFFSET = 0;						\
    }									\
} while (0)

/* Used in sim.c */
#define WRT_TKNBR_TR()							\
do {									\
    branch_trec.taken = BR_FLAG = 1;					\
    branch_trec.target_addr	= ip;					\
    (void)traceWrite(tracef, &branch_trec);				\
} while (0)

/* Used in interruption.c */
#define WRT_IRPT_TR()							\
do {									\
    irpt_trec.psr_	= psr;						\
    irpt_trec.ipsr_	= IPSR;						\
    irpt_trec.isr_	= ISR;						\
    irpt_trec.iip_	= IIP;						\
    irpt_trec.iipa_	= IIPA;						\
    (void)traceWrite(tracef, &irpt_trec);				\
} while (0)

/* Used in simmem.c */
#define WRT_DOFFSET_TR(adr,typ,sz)					\
do {									\
    doffset_trec.doffset = adr;						\
    doffset_trec.type = typ;						\
    doffset_trec.size = sz;						\
    (void)traceWrite(tracef, &doffset_trec);				\
} while (0)

/* Used in trace.c */
#define WRT_PREV_PFS_TR(pfs)						\
do {									\
    prev_pfs_trec.prev_pfs = pfs;					\
    (void)traceWrite(tracef, &prev_pfs_trec);				\
} while (0)

/* Used in trace.c */
#define WRT_BRPRED_TR(br)						\
do {									\
    br_pred_trec.br_pred = br;						\
    (void)traceWrite(tracef, &br_pred_trec);				\
} while (0)

/* Used in trace.c */
#define WRT_ARTGT_TR(ar)						\
do {									\
    ar_tgt_trec.ar_tgt = ar;						\
    (void)traceWrite(tracef, &ar_tgt_trec);				\
} while (0)


#ifdef DEBUG
/* For testing implementation-dependent trace record generation */

#define WRT_STALL_TR()							\
do {									\
    Stall_Trec trec = { STALL_TREC };					\
    unsigned i;								\
									\
    for (i = 0; i < 3; i++)						\
	trec.stall_vec[i] = total_insts << i;				\
    (void)traceWrite(tracef, &trec);					\
} while (0)

#define WRT_STALL_LATENCY_TR()						\
do {									\
    Stall_Latency_Trec trec = { STALL_LATENCY_TREC };			\
									\
    trec.stall_event	= 27;						\
    trec.slot		= 1;						\
    trec.latency	= total_insts;					\
    (void)traceWrite(tracef, &trec);					\
} while (0)
#endif

#endif	/* _SKI_TRACE_H */
