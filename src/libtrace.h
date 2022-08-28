/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Trace Library Header
 *
 * Trace format version number: 1.0.
 * Trace header version number: 1.1.
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
#ifndef _SKI_LIBTRACE_H
#define _SKI_LIBTRACE_H

#include "std.h"

/* XXX - should this library have any dependence on types such as BOOL
	 and ADDR? */
/* XXX - Add documentation/comments to describe how atomic records get
   converted into canonical records */

/**************************************************************************
 * File header constants
 **************************************************************************/

/* Defines for valid_header_fields, these are bit masks */
#define E_MACHINE_VALID		0x1	/* Elf64_Ehdr.e_machine is valid */
#define E_FLAGS_VALID		0x2	/* Elf64_Ehdr.e_flags is valid */

#define MAXTREC			256	/* Record sizes table length */

/**************************************************************************
 * Record types
 **************************************************************************/

typedef enum {		/* Must be 8-bit constants */
    CANONICAL_TREC	= 0x20,
    STALL_LATENCY_TREC	= 0x21,
    STALL_TREC		= 0x22,

    CAPSULE_TREC	= 0x80,
    CONTEXT_SWITCH_TREC	= 0x81,
    INTERRUPTION_TREC	= 0x82,
    DISCONTINUITY_TREC	= 0x83,
    VIRT_CYCLE_TREC	= 0x84,
    IOFFSET_TREC	= 0x85,
    BUNDLE_TREC		= 0x86,
    BRANCH_TREC		= 0x87,
    DOFFSET_TREC	= 0x88,
    REGION_CHNG_TREC	= 0x89,
    PREV_PFS_TREC	= 0x8a,
    BRPRED_TREC		= 0x8b,
    ARTGT_TREC		= 0x8c,

    END_OF_TRACE_TREC	= 0xf0
} Record_Type;


/**************************************************************************
 * Canonical record constants
 **************************************************************************/

/* Defines for valid_record_fields, these are bit masks */
#define IREGION_VALID		0x1	/* iregion valid */
#define SLOTS_VALID		0x2	/* slot[0-2] valid */
#define VCYCLE_INCR_VALID  	0x4	/* virtual_cycle_incr valid */
#define IOFFSET_VALID		0x8	/* ioffset valid */
#define BUNDLE_VALID		0x10	/* bundle[0-3] valid */
#define DOFFSET_0_VALID		0x20	/* doffset[0] valid */
#define DOFFSET_1_VALID		0x40	/* doffset[1] valid */
#define DOFFSET_2_VALID		0x80	/* doffset[2] valid */

/* Defines for valid_record_flags, these are bit masks */
#define CONTEXT_SWITCH_VALID	0x4	/* The context switch flag is valid */
#define DISCONTINUITY_VALID	0x2	/* The discontinuity flag is valid */
#define INTERRUPT_VALID		0x1	/* The interrupt flag is valid */

/* Defines for valid_slot_flags, these are bit masks */
#define BR_TAKEN_VALID	0x2000000	/* br_taken is valid */
#define QPRED_VALID	0x1000000	/* qpred is valid */
#define DREGION_VALID	0x0000001	/* dregion is valid */

/* Defines for doffset_usage */
#define DOFFSET_MEMORY		(1<<0)	/* doffset & dregion for memory op
					   addresses */
#define DOFFSET_PFS		(1<<1)	/* return branches (and rfi?) save the
					   Previous Function State (AR[64])
					   in the doffset field */
#define DOFFSET_BRPRED		(1<<2)	/* mov-to-br and brp save the branch
					   register in the doffset field */
#define DOFFSET_ARTGT		(1<<3)	/* mov-to-ar save the target value
					   in the doffset field */


/**************************************************************************
 * Atomic record constants
 **************************************************************************/

/* Doffset_Trec enums */

typedef enum {		/* Must be 8-bit constants */
    DOFF_TRC_READ	= 0,
    DOFF_TRC_WRITE	= 1,
    DOFF_TRC_SEMAPHORE	= 2,
    DOFF_TRC_LFETCH	= 3,
    DOFF_TRC_RSEREAD	= 4,
    DOFF_TRC_RSEWRITE	= 5,
    DOFF_TRC_FLUSH	= 6,
    DOFF_TRC_TY_UNKNOWN	= 0xff
} Doff_Trc_Type;

typedef enum {		/* Must be 8-bit constants */
    DOFF_TRC_1_BYTE	=  1,
    DOFF_TRC_2_BYTES	=  2,
    DOFF_TRC_4_BYTES	=  4,
    DOFF_TRC_8_BYTES	=  8,
    DOFF_TRC_10_BYTES	= 10,
    DOFF_TRC_16_BYTES	= 16,
    DOFF_TRC_SZ_UNKNOWN	= 0xff
} Doff_Trc_Size;

/* Branch_Trec enums and constants */

typedef enum {		/* Must be 8-bit constants */
    BR_TRC_TYPE_UNKNOWN	= 0xff
} Br_Trc_Type;

typedef enum {		/* Must be 8-bit constants */
    BR_TRC_HINT_UNKNOWN	= 0xff
} Br_Trc_Hint;

#define BR_TRC_BREG_UNKNOWN	0x7f	/* Must be a 7-bit constant */

/**************************************************************************
 * Auxiliary structs
 **************************************************************************/

typedef struct {
    unsigned short	version;	/* major version number */
    unsigned short	revision;	/* minor revision number */
} Version_Type;

/**************************************************************************
 * Trace file header
 **************************************************************************/

typedef struct {
   Version_Type header_version;    /* version of this header */
   Version_Type trace_version;     /* version of the trace records */
   unsigned trace_rec_offset;      /* pointer to 1st trace record */
   unsigned number_trace_rec;      /* trace records in package */

   double   virtual_cycle;         /* virtual cycle of 1st trace rec */
   unsigned compression_ptr;       /* pointer to compression struct */


   unsigned valid_header_fields;   /* which header fields are valid */
   unsigned valid_record_fields;   /* which record fields are valid */
   unsigned valid_record_flags;	   /* which record flags are valid */
   unsigned valid_slot_flags;	   /* which slot flags are valid */
   unsigned doffset_usage;	   /* how the doffset field is used */

   unsigned short res_0_mbz;	   /* reserved, should be zero */

   unsigned short Elf64_Ehdr_e_machine;  /* identifies the target architecture*/
   unsigned Elf64_Ehdr_e_flags;	         /* processor specific flag */

   unsigned sim_time;		   /* gettimeofday time trace was made */
   unsigned ip_ptr;		   /* pointer to host machine's ip address */
   unsigned user_ptr;		   /* pointer to user name */
   unsigned tool_name_ptr;	   /* pointer to name of tool & revision level*/
   unsigned uarch_ptr;	   	   /* pointer to assumed implementation */
   unsigned workload_ptr;	   /* pointer to name of workload description */
   unsigned trace_name_ptr;	   /* pointer to trace name */
   unsigned comments_ptr;	   /* pointer to comments about trace */

   unsigned short record_lengths[MAXTREC];	/* length of each record type */
} Trace_Header;

/**************************************************************************
 * Trace records
 **************************************************************************/

/* Slot struct for canonical record */
typedef struct {	/* status bits about the instruction */
    unsigned res1_mbz	: 5;	/* reserved, must be zero */
    unsigned not_valid	: 1;	/* this is not a valid instruction */
    unsigned br_taken	: 1;	/* this instruction is a taken branch */
    unsigned pred	: 1;	/* qualifying pred of the instruction */
    unsigned dregion	: 24;	/* region bits for data reference */
} Trace_Slot;

/* Canonical record */
typedef struct {
    Record_Type record_type	: 8;	/* CANONICAL_TREC */
    unsigned virtual_cycle_incr	: 24;	/* virt cycles since last record */

    unsigned processor		: 8;	/* processor number */
    unsigned iregion		: 24;	/* region bits of instruction address */

    unsigned long long ioffset;		/* region offset bits of instruction
					   address */

    unsigned bundle[4];			/* instruction bundle */

    unsigned res2_mbz		: 29;	/* reserved, must be zero */
    unsigned context_switch	: 1;	/* 1 if this is the first instruction
					   of a new context */
    unsigned discontinuity	: 1;	/* 1 if this is the first instruction
					   after a discontinuity in trace */
    unsigned interrupt		: 1;	/* 1 if instruction is the first
					   executed after an interrupt */

    Trace_Slot slot[3];			/* information on each instruction in
					   bundle, in sequential order */

    unsigned long long doffset[3];	/* region offset address bits for
					   data reference */
} Canonical_Trec;

/* Snow-specific records */

typedef struct {
    Record_Type record_type	: 8;	/* STALL_TREC */
    unsigned res0_mbz		: 24;	/* reserved, must be 0 */
    unsigned stall_vec[3];		/* stall vector for each slot */
} Stall_Trec;

typedef struct {
    Record_Type record_type	: 8;	/* STALL_LATENCY_TREC */
    unsigned stall_event	: 5;	/* bit position (big endian) of
					   associated stall */
    unsigned slot		: 3;	/* which slot */
    unsigned res0_mbz		: 4;	/* unused, must be 0 */
    unsigned latency		: 12;	/* the cycle latency */
} Stall_Latency_Trec;


/* Atomic records */

typedef struct {
    Record_Type		record_type	: 8;	/* CAPSULE_TREC */
    unsigned char	proc;
    unsigned char	br_taken	: 1;
    unsigned char	pred		: 1;
    unsigned char	rv		: 6;
    unsigned char	slot;
} Capsule_Trec;

typedef struct {
    Record_Type		record_type	: 8;	/* CONTEXT_SWITCH_TREC */
    unsigned char	rv[3];
} Context_Switch_Trec;

typedef struct {
    Record_Type		record_type	: 8;	/* INTERRUPTION_TREC */
    unsigned char	rv[3];
    unsigned long long	psr_;
    unsigned long long	ipsr_;
    unsigned long long	isr_;
    unsigned long long	iip_;
    unsigned long long	iipa_;
} Interruption_Trec;

typedef struct {
    Record_Type		record_type	: 8;	/* DISCONTINUITY_TREC */
    unsigned char	rv[3];
} Discontinuity_Trec;

typedef struct {		/* If not present, assume an increment of 1 */
    Record_Type		record_type	: 8;	/* VIRT_CYCLE_TREC */
    unsigned char	rv[3];
    unsigned int	cycle_incr;
} Virt_Cycle_Trec;

typedef struct {		/* Generated only if ioffset is not sequential*/
    Record_Type		record_type	: 8;	/* IOFFSET_TREC */
    unsigned char	rv[7];
    unsigned long long	ioffset;
} Ioffset_Trec;

typedef struct {
    Record_Type		record_type	: 8;	/* BUNDLE_TREC */
    unsigned char	rv[3];
    unsigned int	bundle[4];
} Bundle_Trec;

typedef struct {
    Record_Type		record_type	: 8;	/* BRANCH_TREC */
    Br_Trc_Type		br_type		: 8;
    Br_Trc_Hint		br_hint		: 8;
    unsigned char	br_reg		: 7;
    unsigned char	taken		: 1;
    unsigned char	rv[4];
    unsigned long long	target_addr;
} Branch_Trec;

typedef struct {
    Record_Type		record_type	: 8;	/* DOFFSET_TREC */
    Doff_Trc_Type	type		: 8;
    Doff_Trc_Size	size		: 8;
    unsigned char	rv[5];
    unsigned long long	doffset;
} Doffset_Trec;

typedef struct {		/* Generated only if region changes */
    Record_Type		record_type	: 8;	/* REGION_CHNG_TREC */
    unsigned char	reg_no;
    unsigned char	rv[6];
    unsigned long long	region_id;
} Region_Chng_Trec;

typedef struct {
    Record_Type		record_type	: 8;	/* PREV_PFS_TREC */
    unsigned char	rv[7];
    unsigned long long	prev_pfs;
} Prev_Pfs_Trec;

typedef struct {
    Record_Type		record_type	: 8;	/* BRPRED_TREC */
    unsigned char	rv[7];
    unsigned long long	br_pred;
} Br_Pred_Trec;

typedef struct {
    Record_Type		record_type	: 8;	/* ARTGT_TREC */
    unsigned char	rv[7];
    unsigned long long	ar_tgt;
} Ar_Tgt_Trec;

/* End of Trace record */

typedef struct {
    Record_Type		record_type	: 8;	/* END_OF_TRACE_TREC */
    unsigned char	rv[3];
} End_of_Trace_Trec;


/**************************************************************************
 * External functions
 **************************************************************************/

BOOL traceOutInit(char *trfile, char *mode, double virtcycle, char *toolname);
/*
    This function opens the trace output file, and writes the trace header
    structure to it.  Some of the fields in the header structure are
    uninitialized.  They will be subsequently filled up by the record
    conversion tool.

    trfile	Trace output file.
    mode	Create ("w") vs. append to ("a") trace file.
    virtcycle	Initial virtual cycle.  The fractional part represents
		MP-ordering.
    toolname	Generating tool name and version.
    (retval)	Success/Failure
 */


int traceInInit(FILE *trcf, char errmsg[], unsigned long long *firstip);
/*
    This function reads the trace input file and performs header and trace
    version number consistency.  The trace file can be prefixed with a
    package header which if present and valid is skipped.  The routine
    passes the initial ip to the caller.

    trcf	Trace input file pointer.
    errmsg	Buffer for any error message.
    (retval)	Status indication: 0 (pass) or -1 (fail).
 */


int traceOutClose(void);
/*
    This function closes the trace output file.  The function also does
    whatever action is necessary before closing the file (e.g., writing
    the End_of_Trace trace record, and writing the trace record type).

    (retval)	Status indication: 0 (pass) or -1 (fail).
 */


int traceWrite(FILE *fp, void *trcrec);
/*
    This function conceptually sends a trace record to the trace output file
    (actually the record is buffered before writing several records out).
    The trace file format is the same as the the trace records defined in
    this header.

    fp		Trace file pointer.
    trcrec	Pointer to the trace record to be written.  The pointer points
		to the record type.
    (retval)	Status indication: 0 (pass) or -1 (fail).
 */

int traceRead(unsigned trcfno, unsigned char *trec);
/*
    This function returns the trace record at the current location in the
    trace input buffer.  The file pointer is not advanced (see traceAdvance()).

    fp		Trace input file pointer.
    trec	Pointer to a buffer where the trace record is read.
    (retval)	Status indication: 0 (pass) or -1 (fail).
 */


int traceAdvance(unsigned trcfno);
/*
    This function advances the trace input file pointer to the next trace
    record.

    fp		Trace input file pointer.
    (retval)	Status indication: 0 (pass) or -1 (fail).
 */


int traceLookAhead(FILE *fp, unsigned char *trec);
/*
    This function reads the trace record following the one at the current
    position.  The file pointer is not advanced.

    fp		Trace input file pointer.
    trec	Pointer to a buffer where the trace record is read.
    (retval)	Status indication: 0 (pass) or -1 (fail).
 */


int traceBacktrack(unsigned trcfno, int n);
/*
    This function backtracks the trace input file pointer by a given number
    of trace records.

    n       	Number of trace records to backtrack.
    (retval)	Status indication: 0 (pass) or -1 (fail).
 */

int traceFlush(FILE *fp);

#endif	/* _SKI_LIBTRACE_H */
