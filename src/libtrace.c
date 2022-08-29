/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Trace Library Routines
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
#include <time.h>
#include <unistd.h>
#if defined __linux__
# include <sys/time.h>
#endif /* !defined __linux__ */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/param.h>		/* for MIN */
#include "std.h"		/* remove if get rid of BOOL dependence */
#include "libtrace.h"
#include "package.h"
#include "types.h"
#include "state.h"
#include "exportui.h"
#include "coreui.h"

#define HEADER_VERSION		0x10001
#define TRACE_VERSION		0x10000
    /* Change version numbers because header and trace records changed?
	for at least branch prediction.  Why are these defines here and
	not in libtrace.h? */

#define MAXTRCBUF		(BUFSIZ * 100)


extern FILE *tracef;

/*##################### Local Variables ####################################*/

/* Trace Output */
static unsigned short	out_treclen[MAXTREC];
static unsigned char	out_trcbuf[MAXTRCBUF];
static unsigned char	*outbufp = out_trcbuf;


/*##################### Functions and Subroutines ##########################*/

/*************************/
/* Trace Output Routines */
/*************************/

/*-------------------------------------------------------------------------
 * trace$OutputFile$Initialize - Open the trace output file, and write the trace
 *  header structure to it.  Some of the fields in the header structure are
 *  uninitialized.  They will be subsequently filled up by the record
 *  conversion tool.
 *------------------------------------------------------------------------*/
BOOL traceOutInit(char *trfile, char *mode, double virtcycle, char *toolname)
{
    Trace_Header trhdr;
    char infobuf[BUFSIZ];
#ifndef __ia64
    char buf[100];
    struct hostent *hostinfo;
    struct in_addr hostaddr;
#endif
    char *p = infobuf, *pbuf;
    struct timeval t;
    struct timezone tz;
    unsigned i;

    if (!(tracef = fopen(trfile, mode))) {
	cmdErr("Could not open trace output file %s\n", trfile);
	return NO;
    }

    trhdr.header_version.version  = HEADER_VERSION >> 16;
    trhdr.header_version.revision = (HEADER_VERSION & 0xFFFF);
    trhdr.trace_version.version   = TRACE_VERSION >> 16;
    trhdr.trace_version.revision  = (TRACE_VERSION & 0xFFFF);

    trhdr.trace_rec_offset	= 0;
	/* Final value is initialized when the rest of the header information
	   has been initialized, and again by the conversion tool */

    trhdr.number_trace_rec	= 0;
	/* Zero to allow piping output.  Final value is initialized when file
	   is closed (do I need another function for this?), and again by the
	   conversion tool */

    trhdr.virtual_cycle		= virtcycle;
    trhdr.compression_ptr	= 0;		/* No compression */

    trhdr.valid_header_fields	= 0;
	/* Final value are initialized by the conversion tool */

    trhdr.valid_record_fields	= IREGION_VALID | SLOTS_VALID |
				  VCYCLE_INCR_VALID | IOFFSET_VALID |
				  BUNDLE_VALID | DOFFSET_0_VALID |
				  DOFFSET_1_VALID | DOFFSET_2_VALID;

    trhdr.valid_record_flags	= DISCONTINUITY_VALID;
	/* Since the first release is to run on application mode, there is
	   no need to support CONTEXT_SWITCH_VALID or INTERRUPT_VALID */

    trhdr.valid_slot_flags	= BR_TAKEN_VALID | QPRED_VALID | DREGION_VALID;
    trhdr.doffset_usage		= DOFFSET_MEMORY | DOFFSET_PFS |
				  DOFFSET_BRPRED | DOFFSET_ARTGT;
    trhdr.res_0_mbz		= 0;

    trhdr.Elf64_Ehdr_e_machine	= 0;
    trhdr.Elf64_Ehdr_e_flags	= 0;
	/* These two fields are initialized by the conversion tool */

    if (!gettimeofday(&t, &tz))
	trhdr.sim_time		= t.tv_sec;
    else
	trhdr.sim_time		= 0;	/* Set to Jan. 1, 1970 */

#ifndef __ia64
    if (!gethostname(buf, sizeof buf))
	if ((hostinfo = gethostbyname(buf))) {
	    hostaddr.s_addr = *((unsigned long *)(hostinfo->h_addr_list[0]));
	    if ((pbuf = inet_ntoa(hostaddr))) {
		trhdr.ip_ptr	= sizeof(Trace_Header) + (p - infobuf);
		p += sprintf(p, "%s", pbuf) + 1;
	    } else
		trhdr.ip_ptr	= 0;	/* No IP address */
	} else
	    trhdr.ip_ptr	= 0;	/* No IP address */
    else
#endif
	trhdr.ip_ptr		= 0;	/* No IP address */

    if ((pbuf = getlogin())) {
	trhdr.user_ptr		= sizeof (Trace_Header) + (p - infobuf);
	p += sprintf(p, "%s", pbuf) + 1;
    } else
	trhdr.user_ptr		= 0;	/* No user name */

    if (toolname && strlen(toolname)) {
	trhdr.tool_name_ptr	= sizeof (Trace_Header) + (p - infobuf);
	p += sprintf(p, "%s", toolname) + 1;
    } else
	trhdr.tool_name_ptr	= 0;	/* No tool name */

    trhdr.uarch_ptr		= 0;
    trhdr.workload_ptr		= 0;
    trhdr.trace_name_ptr	= 0;
    trhdr.comments_ptr		= 0;
	/* These four fields are initialized by the conversion tool */

    trhdr.trace_rec_offset      = sizeof (Trace_Header) + (p - infobuf);
	/* We now know where the first trace record will be stored */

    out_treclen[CANONICAL_TREC]	= 0;
	/* Not directly generated by the library, but by the conversion tool */

    out_treclen[STALL_LATENCY_TREC]	= sizeof (Stall_Latency_Trec);
    out_treclen[STALL_TREC]		= sizeof (Stall_Trec);

    out_treclen[CAPSULE_TREC]		= sizeof (Capsule_Trec);

    out_treclen[CONTEXT_SWITCH_TREC]	= 0;
    out_treclen[INTERRUPTION_TREC]	= sizeof (Interruption_Trec);
	/* Since the first release is to run on application mode, these two
	   trace records are not generated */

    out_treclen[DISCONTINUITY_TREC]	= sizeof (Discontinuity_Trec);
    out_treclen[VIRT_CYCLE_TREC]	= sizeof (Virt_Cycle_Trec);
    out_treclen[IOFFSET_TREC]		= sizeof (Ioffset_Trec);
    out_treclen[BUNDLE_TREC]		= sizeof (Bundle_Trec);
    out_treclen[BRANCH_TREC]		= sizeof (Branch_Trec);
    out_treclen[DOFFSET_TREC]		= sizeof (Doffset_Trec);
    out_treclen[PREV_PFS_TREC]		= sizeof (Prev_Pfs_Trec);
    out_treclen[BRPRED_TREC]		= sizeof (Br_Pred_Trec);
    out_treclen[ARTGT_TREC]		= sizeof (Ar_Tgt_Trec);
    out_treclen[END_OF_TRACE_TREC]	= sizeof (End_of_Trace_Trec);

    /* XXX - use memcpy */
    for (i = 0; i < MAXTREC; i++)
	trhdr.record_lengths[i] = out_treclen[i];

    if (!fwrite(&trhdr, sizeof trhdr, 1, tracef)) {
	cmdErr("Could not write trace header to file %s\n", trfile);
	(void)fclose(tracef);
	tracef = NULL;
	return NO;
    }

    if (p > infobuf && fwrite(infobuf, 1, (unsigned)(p - infobuf), tracef)
	< (unsigned)(p - infobuf)) {
	cmdErr("Could not write information block to file %s\n", trfile);
	(void)fclose(tracef);
	tracef = NULL;
	return NO;
    }
    return YES;
}

/*-------------------------------------------------------------------------
 * traceBuffer$Flush - Flush the trace buffer to the output file.  Reset
 *  the trace buffer.
 *------------------------------------------------------------------------*/
int traceFlush(FILE *fp)
{
    unsigned cursz = outbufp - out_trcbuf;
    int status = 0;

    if (fwrite(out_trcbuf, 1, cursz, fp) < cursz || fflush(fp))
	status = -1;
    outbufp = out_trcbuf;
    return status;
}

/*-------------------------------------------------------------------------
 * trace$OutputFile$Close - Close trace output file.
 *------------------------------------------------------------------------*/
int traceOutClose(void)
{
    static End_of_Trace_Trec	eot_trec = { END_OF_TRACE_TREC };

    if (!tracef)
	return 0;
    /* TBD: update the header's trace record count */
    if (!traceWrite(tracef, &eot_trec) && !traceFlush(tracef))
	return fclose(tracef);
    else {
	(void)fclose(tracef);
	return EOF;
    }
}

/*-------------------------------------------------------------------------
 * traceRecord$Write - Send a trace record to the trace output file.  The
 *  library buffers several trace records before actually writing out the
 *  trace records.
 *------------------------------------------------------------------------*/
int traceWrite(FILE *fp, void *trcrec)
{
    unsigned trclen = out_treclen[*(unsigned char *)trcrec];
    unsigned cursz = outbufp - out_trcbuf;
    int status = 0;
#if 1
    unsigned i;
    char *p = (char *)trcrec;
#endif

    if ((cursz + trclen) > MAXTRCBUF) {
	if (fwrite(out_trcbuf, 1, cursz, fp) < cursz)
	    status = -1;
	outbufp = out_trcbuf;
    }
#if 1
/* Which implementation has better performance? */
    for (i = 0; i < trclen; i++)
	*outbufp++ = *p++;
#else
    (void)memcpy(outbufp, trcrec, trclen);
    outbufp += trclen;
#endif

    return status;
}

