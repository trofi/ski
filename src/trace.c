/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Trace Routines
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

#include <stdio.h>
#include <string.h>

#include "std.h"
#include "bits.h"
#include "types.h"
#include "state.h"
#include "sim.h"
#include "libcore.h"
#include "libtrace.h"
#include "trace.h"

/*##################### Globals - Imports ##################################*/

/*##################### Globals - Exports ##################################*/

unsigned traceEnb = NO;	/* trace output enable/disable flag */
FILE *tracef = NULL;	/* trace output file pointer */

unsigned trc_flags = 1;	/* indicates which records to write and whether to
			   write the IP.  Set to 1 for first trace capsule */

/* Initialize trace record globals with record type (and in some cases
   initialize other fields with "unknown").  All other fields are initialized
   to 0 */

Capsule_Trec		capsule_trec	= { CAPSULE_TREC };
Interruption_Trec	irpt_trec	= { INTERRUPTION_TREC };
Discontinuity_Trec	disc_trec	= { DISCONTINUITY_TREC };
Virt_Cycle_Trec		vcycle_trec	= { VIRT_CYCLE_TREC };
Ioffset_Trec		ioffset_trec	= { IOFFSET_TREC };
Bundle_Trec		bundle_trec	= { BUNDLE_TREC };
Branch_Trec		branch_trec	= { BRANCH_TREC,
					    BR_TRC_TYPE_UNKNOWN,
					    BR_TRC_HINT_UNKNOWN,
					    BR_TRC_BREG_UNKNOWN };
Doffset_Trec		doffset_trec	= { DOFFSET_TREC,
					    DOFF_TRC_TY_UNKNOWN,
					    DOFF_TRC_SZ_UNKNOWN };
Prev_Pfs_Trec		prev_pfs_trec	= { PREV_PFS_TREC };
Br_Pred_Trec		br_pred_trec	= { BRPRED_TREC };
Ar_Tgt_Trec		ar_tgt_trec	= { ARTGT_TREC };

/*####################### Local Variables ##################################*/

static CTR traceVcy;	/* trace virtual cycle, to determine discontinuities */

/*##################### Functions and Subroutines ##########################*/

/*--------------------------------------------------------------------------
 * traceWrite$Previous$PFS - Routine to output the 'Previous PFS' trace
 *  record.  (Initially this routine is added so that combfns.c doesn't
 *  depend on trace.h and libtrace.h to speed up compile time.  Later it
 *  can be decided to include the macro directly in combfns.c.)
 *--------------------------------------------------------------------------*/
void tracePrevPFS(REG prevPfs)
{
    if (traceEnb)
	WRT_PREV_PFS_TR(prevPfs);
}

/*--------------------------------------------------------------------------
 * traceWrite$Branch$Prediction - Routine to output the 'Branch Prediction'
 *  trace record.  (See comments in tracePrevPFS for future implementation).
 *--------------------------------------------------------------------------*/
void traceBrPred(REG br)
{
    if (traceEnb)
	WRT_BRPRED_TR(br);
}

/*--------------------------------------------------------------------------
 * traceWrite$AR$Target - Routine to output the 'AR Target Value'
 *  trace record.  (See comments in tracePrevPFS for future implementation).
 *--------------------------------------------------------------------------*/
void traceArTgt(REG ar)
{
    if (traceEnb)
	WRT_ARTGT_TR(ar);
}

/* returns NO if enabling tracing before setting the trace file */
BOOL setTraceEnb(BOOL enb)
{
    if (enb) {
	if (!tracef)
	    return NO;
	preInst |= PRE_TRACE;
	traceEnb = YES;
    } else {
	preInst &= ~PRE_TRACE;
	traceEnb = NO;
	traceFlush(tracef);
    }
    return YES;
}

void initTraceVcy(void)
{
    traceVcy = total_insts;
}

void initTraceFlags(void)
{
    FIRST_TREC = 1;
}

void traceDisc(void)
{
    if (traceEnb && total_insts - traceVcy > 1) {
	WRT_DISC_TR();
	WRT_VCYCLE_TR();
    }
}

void traceVcyEnb(void)
{
    if (traceEnb)
	traceVcy = total_insts;
}
