/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Field Macros
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
#ifndef _SKI_FIELDS_H
#define _SKI_FIELDS_H

typedef DWORD	INST;

#define B0(x)	BitfR(x,0,8)
#define B1(x)	BitfR(x,8,8)
#define B2(x)	BitfR(x,16,8)
#define B3(x)	BitfR(x,24,8)
#define B4(x)	BitfR(x,32,8)
#define B5(x)	BitfR(x,40,8)
#define B6(x)	BitfR(x,48,8)
#define B7(x)	BitfR(x,56,8)
#define Bn(x,n)	BitfX(x,8*n,8)

#define HW0(x)	BitfR(x,0,16)
#define HW1(x)	BitfR(x,16,16)
#define HW2(x)	BitfR(x,32,16)
#define HW3(x)	BitfR(x,48,16)
#define HWn(x,n)	BitfX(x,16*n,16)

#define WD0(x)	BitfR(x,0,32)
#define WD1(x)	BitfR(x,32,32)

#define SIGN(x)	BitfR(x,0,1)
#define EXP(x)	BitfR(x,1,15)

/* Instruction Field Macros */

#define SLOT(x)	BitfR(x,60,2)

/* Register Field Macros */

/* GRs */
#define GP_ID	 1
#define SP_ID	12
#define TP_ID	13

/* BRs */
#define RP_ID	0

/* ARs */
#define K0_ID	 0
#define K1_ID	 1
#define K2_ID	 2
#define K3_ID	 3
#define K4_ID	 4
#define K5_ID	 5
#define K6_ID	 6
#define K7_ID	 7
#define RSC_ID	 16
#define BSP_ID	 17
#define BSPST_ID 18
#define RNAT_ID	 19
#define CCV_ID	 32
#define UNAT_ID	 36
#define FPSR_ID	 40
#define ITC_ID	 44

#define PFS_ID	64
#define LC_ID	65
#define EC_ID	66

/* CRs */
#define DCR_ID	  0
#define ITM_ID	  1
#define IVA_ID	  2
#define PTA_ID	  8
#define GPTA_ID	  9
#define IPSR_ID	 16
#define ISR_ID	 17
#define IIP_ID	 19
#define IFA_ID	 20
#define ITIR_ID	 21
#define IIPA_ID	 22
#define IFS_ID	 23
#define IIM_ID	 24
#define IHA_ID	 25
#define LID_ID	 64
#define IVR_ID	 65
#define TPR_ID	 66
#define EOI_ID	 67
#define IRR0_ID	 68
#define IRR1_ID	 69
#define IRR2_ID	 70
#define IRR3_ID	 71
#define ITV_ID	 72
#define PMV_ID	 73
#define CMCV_ID	 74
#define LRR0_ID	 80
#define LRR1_ID	 81

/* RSC */
#define XRSC_LOADRS(x)	BitfR(x,34,14)
#define XRSC_BE(x)	BitfR(x,59,1)
#define XRSC_PL(x)	BitfR(x,60,2)
#define XRSC_MODE(x)	BitfR(x,62,2)

/* PFS */
#define XFS_PPL(x)	BitfR(x,0,2)
#define XFS_PEC(x)	BitfR(x,6,6)

/* CFM */
#define XFM_RRBP(x)	BitfR(x,26,6)
#define XFM_RRBF(x)	BitfR(x,32,7)
#define XFM_RRBG(x)	BitfR(x,39,7)
#define XFM_SOR(x)	BitfR(x,46,4)
#define XFM_SOIL(x)	BitfR(x,50,7)
#define XFM_SOF(x)	BitfR(x,57,7)

/* PSR */
#define X_PSR_VM(x)	BitfR(x,17,1)
#define X_PSR_IA(x)	BitfR(x,18,1)
#define X_PSR_BN(x)	BitfR(x,19,1)
#define X_PSR_ED(x)	BitfR(x,20,1)
#define X_PSR_RI(x)	BitfR(x,21,2)
#define X_PSR_DD(x)	BitfR(x,24,1)
#define X_PSR_DA(x)	BitfR(x,25,1)
#define X_PSR_IT(x)	BitfR(x,27,1)
#define X_PSR_MC(x)	BitfR(x,28,1)
#define X_PSR_IS(x)	BitfR(x,29,1)
#define X_PSR_CPL(x)	BitfR(x,30,2)
#define X_PSR_RT(x)	BitfR(x,36,1)
#define X_PSR_PP(x)	BitfR(x,42,1)
#define X_PSR_SP(x)	BitfR(x,43,1)
#define X_PSR_DT(x)	BitfR(x,46,1)
#define X_PSR_PK(x)	BitfR(x,48,1)
#define X_PSR_IC(x)	BitfR(x,50,1)
#define X_PSR_MFH(x)	BitfR(x,58,1)
#define X_PSR_MFL(x)	BitfR(x,59,1)
#define X_PSR_AC(x)	BitfR(x,60,1)
#define X_PSR_UP(x)	BitfR(x,61,1)
#define X_PSR_BE(x)	BitfR(x,62,1)
#define X_PSR_UM(x)	BitfR(x,58,6)

/* iA State */
/* GR mappings */
#define EAX_ID		 8
#define ECX_ID		 9
#define EDX_ID		10
#define EBX_ID		11
#define ESP_ID		12
#define EBP_ID		13
#define ESI_ID		14
#define EDI_ID		15
#define ES_ID		16
#define CS_ID		17
#define SS_ID		18
#define DS_ID		19
#define FS_ID		20
#define GS_ID		21
#define LDT_ID		22
#define ESD_ID		24
#define CSD_GR_ID	25
#define SSD_GR_ID	26
#define DSD_ID		27
#define FSD_ID		28
#define GSD_ID		29
#define LDTD_ID		30
#define GDTD_ID		31

/* AR mappings */
#define IOBASE_ID	 0
#define TSS_ID		 1
#define TSSD_ID		 2
#define IDTD_ID		 3
#define CR3_CR2_ID	 4
#define DR6_DR7_ID	 5
#define FCR_ID		21
#define EFLAGS_ID	24
#define CSD_ID		25
#define SSD_ID		26
#define CFLG_ID		27
#define FSR_ID		28
#define FIR_ID		29
#define FDR_ID		30
/* XXX - Temporary */
#define CSD_AR_ID	CSD_ID
#define SSD_AR_ID	SSD_ID

#define EIP_ID		 1	/* use one of the scratch GRs */

#if !BYTE_ORDER || !BIG_ENDIAN || !LITTLE_ENDIAN
# error "Endianness is unknown!"
#endif

typedef struct {
#if BYTE_ORDER == BIG_ENDIAN
    DWORD g:1;
    DWORD d:1;
    DWORD rv:2;
    DWORD p:1;
    DWORD dpl:2;
    DWORD s:1;
    DWORD type:4;
    DWORD lim:20;
    DWORD base:32;
#else /* BYTE_ORDER == LITTLE_ENDIAN */
    DWORD base:32;
    DWORD lim:20;
    DWORD type:4;
    DWORD s:1;
    DWORD dpl:2;
    DWORD p:1;
    DWORD rv:2;
    DWORD d:1;
    DWORD g:1;
#endif /* BYTE_ORDER == LITTLE_ENDIAN */
} IASegDesc, *IASegDescPtr;

typedef struct {
#if BYTE_ORDER == BIG_ENDIAN
    DWORD rv63_26:38;
    DWORD le:1;
    DWORD be:1;
    DWORD lt:1;
    DWORD rv22:1;
    DWORD id:1;
    DWORD vip:1;
    DWORD vif:1;
    DWORD ac:1;
    DWORD vm:1;
    DWORD rf:1;
    DWORD rv15:1;
    DWORD nt:1;
    DWORD iopl:2;
    DWORD of:1;
    DWORD df:1;
    DWORD if_:1;
    DWORD tf:1;
    DWORD sf:1;
    DWORD zf:1;
    DWORD rv5:1;
    DWORD af:1;
    DWORD rv3:1;
    DWORD pf:1;
    DWORD rv1:1;
    DWORD cf:1;
#else /* BYTE_ORDER == LITTLE_ENDIAN */
    DWORD cf:1;
    DWORD rv1:1;
    DWORD pf:1;
    DWORD rv3:1;
    DWORD af:1;
    DWORD rv5:1;
    DWORD zf:1;
    DWORD sf:1;
    DWORD tf:1;
    DWORD if_:1;
    DWORD df:1;
    DWORD of:1;
    DWORD iopl:2;
    DWORD nt:1;
    DWORD rv15:1;
    DWORD rf:1;
    DWORD vm:1;
    DWORD ac:1;
    DWORD vif:1;
    DWORD vip:1;
    DWORD id:1;
    DWORD rv22:1;
    DWORD lt:1;
    DWORD be:1;
    DWORD le:1;
    DWORD rv63_26:38;
#endif /* BYTE_ORDER == LITTLE_ENDIAN */
} IAeflags, *IAeflagsPtr;

typedef struct {
#if BYTE_ORDER == BIG_ENDIAN
    DWORD ig63_41:23;		/* CR4 */
    DWORD pce:1;
    DWORD pge:1;
    DWORD mce:1;
    DWORD pae:1;
    DWORD pse:1;
    DWORD de:1;
    DWORD tsd:1;
    DWORD pvi:1;
    DWORD vme:1;
    DWORD pg:1;			/* CR0 */
    DWORD cd:1;
    DWORD nw:1;
    DWORD ig28_19:10;
    DWORD am:1;
    DWORD rv17:1;
    DWORD wp:1;
    DWORD ig15_10:6;
    DWORD nm:1;
    DWORD ii:1;
    DWORD if_:1;
    DWORD io:1;
    DWORD ne:1;
    DWORD et:1;
    DWORD ts:1;
    DWORD em:1;
    DWORD mp:1;
    DWORD pe:1;
#else /* BYTE_ORDER == LITTLE_ENDIAN */
    DWORD pe:1;
    DWORD mp:1;
    DWORD em:1;
    DWORD ts:1;
    DWORD et:1;
    DWORD ne:1;
    DWORD io:1;
    DWORD if_:1;
    DWORD ii:1;
    DWORD nm:1;
    DWORD ig15_10:6;
    DWORD wp:1;
    DWORD rv17:1;
    DWORD am:1;
    DWORD ig28_19:10;
    DWORD nw:1;
    DWORD cd:1;
    DWORD pg:1;			/* CR0 */
    DWORD vme:1;
    DWORD pvi:1;
    DWORD tsd:1;
    DWORD de:1;
    DWORD pse:1;
    DWORD pae:1;
    DWORD mce:1;
    DWORD pge:1;
    DWORD pce:1;
    DWORD ig63_41:23;		/* CR4 */
#endif /* BYTE_ORDER == LITTLE_ENDIAN */
} IAcflg, *IAcflgPtr;

typedef struct {
#if BYTE_ORDER == BIG_ENDIAN
    DWORD pfla:32;		/* CR2 */
    DWORD pdb:20;		/* CR3 */
    DWORD rv11_5:7;
    DWORD pcd:1;
    DWORD pwt:1;
    DWORD rv2_0:3;
#else /* BYTE_ORDER == LITTLE_ENDIAN */
    DWORD rv2_0:3;
    DWORD pwt:1;
    DWORD pcd:1;
    DWORD rv11_5:7;
    DWORD pdb:20;		/* CR3 */
    DWORD pfla:32;		/* CR2 */
#endif /* BYTE_ORDER == LITTLE_ENDIAN */
} IAcr3cr2, *IAcr3cr2Ptr;

typedef struct {
#if BYTE_ORDER == BIG_ENDIAN
    DWORD len3:2;		/* DR7 */
    DWORD rw3:2;
    DWORD len2:2;
    DWORD rw2:2;
    DWORD len1:2;
    DWORD rw1:2;
    DWORD len0:2;
    DWORD rw0:2;
    DWORD rv15_14:2;
    DWORD gd:1;
    DWORD rv12_10:3;
    DWORD ge:1;
    DWORD le:1;
    DWORD g3:1;
    DWORD l3:1;
    DWORD g2:1;
    DWORD l2:1;
    DWORD g1:1;
    DWORD l1:1;
    DWORD g0:1;
    DWORD l0:1;
    DWORD rv31_16:16;		/* DR6 */
    DWORD bt:1;
    DWORD bs:1;
    DWORD bd:1;
    DWORD rv12_4:9;
    DWORD b3_:1;
    DWORD b2_:1;
    DWORD b1_:1;
    DWORD b0_:1;
#else /* BYTE_ORDER == LITTLE_ENDIAN */
    DWORD b0_:1;
    DWORD b1_:1;
    DWORD b2_:1;
    DWORD b3_:1;
    DWORD rv12_4:9;
    DWORD bd:1;
    DWORD bs:1;
    DWORD bt:1;
    DWORD rv31_16:16;		/* DR6 */
    DWORD l0:1;
    DWORD g0:1;
    DWORD l1:1;
    DWORD g1:1;
    DWORD l2:1;
    DWORD g2:1;
    DWORD l3:1;
    DWORD g3:1;
    DWORD le:1;
    DWORD ge:1;
    DWORD rv12_10:3;
    DWORD gd:1;
    DWORD rv15_14:2;
    DWORD rw0:2;
    DWORD len0:2;
    DWORD rw1:2;
    DWORD len1:2;
    DWORD rw2:2;
    DWORD len2:2;
    DWORD rw3:2;
    DWORD len3:2;		/* DR7 */
#endif /* BYTE_ORDER == LITTLE_ENDIAN */
} IAdr6dr7, *IAdr6dr7Ptr;

#endif	/* _SKI_FIELDS_H */
