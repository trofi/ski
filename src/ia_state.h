/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator iA State Header
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
#ifndef _SKI_IA_STATE_H
#define _SKI_IA_STATE_H

#include "ia_types.h"

/* mapping of iA state onto IA-64 GRs */
/* XXX - the following definitions work on a BIG-ENDIAN host only */

#define IA64_EAX		grs[EAX_ID].val
#define AL			BitfR(IA64_EAX, 56,  8)
#define AH			BitfR(IA64_EAX, 48,  8)
#define AX			BitfR(IA64_EAX, 48, 16)
#define EAX			BitfR(IA64_EAX, 32, 32)

#define IA64_ECX		grs[ECX_ID].val
#define CL			BitfR(IA64_ECX, 56,  8)
#define CH			BitfR(IA64_ECX, 48,  8)
#define CX			BitfR(IA64_ECX, 48, 16)
#define ECX			BitfR(IA64_ECX, 32, 32)

#define IA64_EDX		grs[EDX_ID].val
#define DL			BitfR(IA64_EDX, 56,  8)
#define DH			BitfR(IA64_EDX, 48,  8)
#define DX			BitfR(IA64_EDX, 48, 16)
#define EDX			BitfR(IA64_EDX, 32, 32)

#define IA64_EBX		grs[EBX_ID].val
#define BL			BitfR(IA64_EBX, 56,  8)
#define BH			BitfR(IA64_EBX, 48,  8)
#define BX			BitfR(IA64_EBX, 48, 16)
#define EBX			BitfR(IA64_EBX, 32, 32)

#define IA64_ESP		grs[ESP_ID].val
#define IASP			BitfR(IA64_ESP, 48, 16)
#define ESP			BitfR(IA64_ESP, 32, 32)

#define IA64_EBP		grs[EBP_ID].val
#define BP			BitfR(IA64_EBP, 48, 16)
#define EBP			BitfR(IA64_EBP, 32, 32)

#define IA64_ESI		grs[ESI_ID].val
#define SI			BitfR(IA64_ESI, 48, 16)
#define ESI			BitfR(IA64_ESI, 32, 32)

#define IA64_EDI		grs[EDI_ID].val
#define DI			BitfR(IA64_EDI, 48, 16)
#define EDI			BitfR(IA64_EDI, 32, 32)

#define SEGREG(id)		BitfR(grs[(id)].val,   48, 16)
#define ES			BitfR(grs[ES_ID].val,  48, 16)
#define CS			BitfR(grs[CS_ID].val,  48, 16)
#define SS			BitfR(grs[SS_ID].val,  48, 16)
#define DS			BitfR(grs[DS_ID].val,  48, 16)
#define FS			BitfR(grs[FS_ID].val,  48, 16)
#define GS			BitfR(grs[GS_ID].val,  48, 16)

#define LDT			BitfR(grs[LDT_ID].val, 48, 16)

#define SEGD(id)		(*(IASegDescPtr)&grs[id+8].val)
#define ESD			(*(IASegDescPtr)&grs[ESD_ID].val)
#define CSD			(*(IASegDescPtr)&grs[CSD_ID].val)
#define SSD			(*(IASegDescPtr)&grs[SSD_ID].val)
#define DSD			(*(IASegDescPtr)&grs[DSD_ID].val)
#define FSD			(*(IASegDescPtr)&grs[FSD_ID].val)
#define GSD			(*(IASegDescPtr)&grs[GSD_ID].val)

#define LDTD			(*(IASegDescPtr)&grs[LDTD_ID].val)
#define GDTD			(*(IASegDescPtr)&grs[GDTD_ID].val)

/* mapping of iA state onto IA-64 ARs */
#define IOBASE			ars[IOBASE_ID]
#define TSS			BitfR(ars[TSS_ID], 48, 16)
#define TSSD			(*(IASegDescPtr)&ars[TSSD_ID])
#define IDTD			(*(IASegDescPtr)&ars[IDTD_ID])
#define CR3_CR2			(*(IAcr3cr2Ptr)&ars[CR3_CR2_ID])
#define DR6_DR7			(*(IAdr6dr7Ptr)&ars[DR6_DR7_ID])
#define EFLAGS			(*(IAeflagsPtr)&ars[EFLAGS_ID])
#define CSD_AR			(*(IASegDescPtr)&ars[CSD_AR_ID])
#define SSD_AR			(*(IASegDescPtr)&ars[SSD_AR_ID])
#define CFLG			(*(IAcflgPtr)&ars[CFLG_ID])
#define FSR			ars[FSR_ID]
#define FIR			ars[FIR_ID]
#define FDR			ars[FDR_ID]

/* EIP */
extern ADDR4 EIP;

#define JMPE_RP			grs[1].val

extern BOOL dosABI;
extern ADDR ioportBase;

#endif 	/* _SKI_IA_STATE_H */
