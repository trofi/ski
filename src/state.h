/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Architected State Header
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
#ifndef _SKI_STATE_H_
#define	_SKI_STATE_H_

#include "tlb.h"

#define MAX_NGRS	2048
#define NGRS	(GR_STK_BASE + N_STACK_PHYS)
#define NBGRS	 16
#define NPRS	 64
#define NFRS	128
#define NBRS	  8
#define NARS	128

#define NCRS	128
#define NRRS	  8
#define NPKRS	 16
#define NDBRS	 16
#define NIBRS	 16
#define NPMS	 64
#define NMSRS	4096
#define NCPUIDS	  5

#define BGR_BASE	16
#define GR_STK_BASE	32
#define N_STACK_PHYS	n_stack_phys
#define N_STACK_VIRT	96
#define GR_ROT_SIZE	N_STACK_VIRT
#define PR_ROT_BASE	16
#define PR_ROT_SIZE	(NPRS-PR_ROT_BASE)
#define FR_ROT_BASE	32
#define FR_ROT_SIZE	(NFRS-FR_ROT_BASE)

typedef struct {
    REG val;
    BOOL nat;
} GREG;

typedef struct {
    BYTE  special;
#if defined __cplusplus
    BYTE  clazz;	/* used when special is 1 */
#else /* !defined __cplusplus */
    BYTE  class;	/* used when special is 1 */
#endif /* !defined __cplusplus */
    BYTE  unorm;	/* used when special is 0 */
    BYTE  sign;
    WORD  exp;
    DWORD mant;
} FREG;

extern const FREG ZERO_FREG;

typedef struct IC *ICPtr;

#ifdef NEW_MP
/* XXX - split into common, app-mode only, and sys-mode only state? */
/* XXX - when these become malloc'ed, can avoid unneeded storage */
typedef struct {
    GREG grs_[MAX_NGRS];
    unsigned grmap_[GR_STK_BASE+N_STACK_VIRT];
    GREG bankedGrs_[NBGRS];
    BOOL prs_[NPRS];
    FREG frs_[NFRS];
    unsigned frmap_[NFRS + FR_ROT_SIZE];
    REG brs_[NBRS];
    REG ars_[NARS];

    REG psr_;
    REG crs_[NCRS];
    REG rrs_[NRRS];
    REG pkrs_[NPKRS];
    REG dbrs_[NDBRS];
    REG ibrs_[NIBRS];
    REG pmcs_[NPMS];
    REG pmds_[NPMS];
    REG msrs_[NMSRS];
    REG cpuids_[NCPUIDS];
    BYTE sof_, soil_, sor_;
    BYTE rrbg_, rrbf_, rrbp_;
    unsigned bol_;
    int dirty_, dirtyNat_, clean_, cleanNat_;
    int invalid_;
    BOOL cfle_;

    ADDR ip_, heap_;

    BOOL lp64_;
    ICPtr icp_;
    unsigned nextPid_;	/* index of next runnable process */
    unsigned parentPid_;
    unsigned status_;

    TlbEntry dtcs_[NDTCS];
    TlbEntry dtrs_[NDTRS];
    TlbEntry itcs_[NITCS];
    TlbEntry itrs_[NITRS];
} mpstate;

extern unsigned curPid, lastPid;
extern mpstate mpState[];
#define grs	mpState[curPid].grs_
#define grmap	mpState[curPid].grmap_
#define bankedGrs	mpState[curPid].bankedGrs_
#define prs	mpState[curPid].prs_
#define frs	mpState[curPid].frs_
#define frmap	mpState[curPid].frmap_
#define brs	mpState[curPid].brs_
#define ars	mpState[curPid].ars_
#define psr	mpState[curPid].psr_
#define crs	mpState[curPid].crs_
#define rrs	mpState[curPid].rrs_
#define pkrs	mpState[curPid].pkrs_
#define dbrs	mpState[curPid].dbrs_
#define ibrs	mpState[curPid].ibrs_
#define pmcs	mpState[curPid].pmcs_
#define pmds	mpState[curPid].pmds_
#define msrs	mpState[curPid].msrs_
#define cpuids	mpState[curPid].cpuids_
#define sof	mpState[curPid].sof_
#define soil	mpState[curPid].soil_
#define sor	mpState[curPid].sor_
#define rrbg	mpState[curPid].rrbg_
#define rrbf	mpState[curPid].rrbf_
#define rrbp	mpState[curPid].rrbp_
#define bol	mpState[curPid].bol_
#define dirty	mpState[curPid].dirty_
#define dirtyNat	mpState[curPid].dirtyNat_
#define clean	mpState[curPid].clean_
#define cleanNat	mpState[curPid].cleanNat_
#define invalid	mpState[curPid].invalid_
#define cfle	mpState[curPid].cfle_
#define ip	mpState[curPid].ip_
#define heap	mpState[curPid].heap_
#define lp64	mpState[curPid].lp64_
#define icp	mpState[curPid].icp_
#define nextPid	mpState[curPid].nextPid_
#define parentPid	mpState[curPid].parentPid_
#define	itcs	mpState[curPid].itcs_
#define	itrs	mpState[curPid].itrs_
#define dtcs	mpState[curPid].dtcs_
#define dtrs	mpState[curPid].dtrs_
#else
#define	curPid		0
extern INSTINFO *icp;
#endif

#define PHYS_GR(r)	(r < GR_STK_BASE ? r : r >= GR_STK_BASE+sor \
			    ? grmap[r] \
			    : r + rrbg < GR_STK_BASE + sor \
				? grmap[r + rrbg] \
				: grmap[r + rrbg - sor])

#define PHYS_FR(r)	(r < FR_ROT_BASE ? r : frmap[r + rrbf])

#define PHYS_PR(r)	(r < PR_ROT_BASE \
			    ? r \
			    : r + rrbp < NPRS \
				? r + rrbp \
				: r + rrbp - PR_ROT_SIZE)

#define GrRd0(reg, val, nat)	(reg < GR_STK_BASE ?	\
				    (val = grs[reg],	\
				     nat = grNaTs[reg])	\
				: reg >= GR_STK_BASE+sor ? \
				    (val = grs[grmap[reg]],	\
				     nat = grNaTs[grmap[reg]])	\
				    : reg+rrbg < GR_STK_BASE+sor ?	\
					(val = grs[grmap[reg+rrbg]],	\
					 nat = grNaTs[grmap[reg+rrbg]]) \
					: (val = grs[grmap[reg+rrbg-sor]], \
					   nat = grNaTs[grmap[reg+rrbg-sor]]))

#define GrRd2(reg, val, nat)	(reg < GR_STK_BASE || reg >= GR_STK_BASE+sor ? \
				    (val = grs[grmap[reg]],	\
				     nat = grNaTs[grmap[reg]])	\
				    : reg+rrbg < GR_STK_BASE+sor ?	\
					(val = grs[grmap[reg+rrbg]],	\
					 nat = grNaTs[grmap[reg+rrbg]]) \
					: (val = grs[grmap[reg+rrbg-sor]], \
					   nat = grNaTs[grmap[reg+rrbg-sor]]))

#define GrRd(reg, gval, gnat, st)		\
    do {					\
	GREG *pgr;				\
						\
	if (st)				 	\
	    pgr = (GREG *)(st);			\
	else if (reg < GR_STK_BASE)		\
	    pgr = &grs[reg];			\
	else if (reg >= GR_STK_BASE+sor) 	\
	    pgr = &grs[grmap[reg]];		\
	else if (reg+rrbg < GR_STK_BASE+sor)	\
	    pgr = &grs[grmap[reg+rrbg]];	\
	else					\
	    pgr = &grs[grmap[reg+rrbg-sor]];	\
	gval = pgr->val;			\
	gnat = pgr->nat;			\
    } while (0)

#if 0
	else if (reg >= GR_STK_BASE+sof) {	x
	    illegalOpFault();			x
	    return StFault;			x
	}
#endif

#define GrWrt(reg, gval, gnat, st)		\
    do {					\
	GREG *pgr;				\
						\
	if (st)					\
	    pgr = (GREG *)(st);			\
	else if (reg < GR_STK_BASE)		\
	    pgr = &grs[reg];			\
	else if (reg >= GR_STK_BASE+sor) 	\
	    pgr = &grs[grmap[reg]];		\
	else if (reg+rrbg < GR_STK_BASE+sor)	\
	    pgr = &grs[grmap[reg+rrbg]];	\
	else					\
	    pgr = &grs[grmap[reg+rrbg-sor]];	\
	pgr->val = gval;			\
	pgr->nat = gnat;			\
    } while (0)

#define GrWrtx(reg, gval, gnat)			\
    do {					\
	GREG *pgr;				\
						\
	if (reg < GR_STK_BASE)			\
	    pgr = &grs[reg];			\
	else if (reg >= GR_STK_BASE+sor) 	\
	    pgr = &grs[grmap[reg]];		\
	else if (reg+rrbg < GR_STK_BASE+sor)	\
	    pgr = &grs[grmap[reg+rrbg]];	\
	else					\
	    pgr = &grs[grmap[reg+rrbg-sor]];	\
	pgr->val = gval;			\
	pgr->nat = gnat;			\
    } while (0)

#define GrWrt0(reg, val, nat)	(reg < GR_STK_BASE	\
				   ? (grs[reg] = val,	 \
				      grNaTs[reg] = nat) \
				: reg >= GR_STK_BASE+sor \
				   ? (grs[grmap[reg]] = val, \
				      grNaTs[grmap[reg]] = nat)	\
				   : reg+rrbg < GR_STK_BASE+sor \
					? (grs[grmap[reg+rrbg]] = val,	\
					   grNaTs[grmap[reg+rrbg]] = nat) \
					: (grs[grmap[reg+rrbg-sor]] = val, \
					   grNaTs[grmap[reg+rrbg-sor]] = nat))

#define GrWrt2(reg, val, nat)	((reg < GR_STK_BASE || reg >= GR_STK_BASE+sor) \
				   ? (grs[grmap[reg]] = val, \
				      grNaTs[grmap[reg]] = nat)	\
				   : reg+rrbg < GR_STK_BASE+sor \
					? (grs[grmap[reg+rrbg]] = val,	\
					   grNaTs[grmap[reg+rrbg]] = nat) \
					: (grs[grmap[reg+rrbg-sor]] = val, \
					   grNaTs[grmap[reg+rrbg-sor]] = nat))

#define PrRd(reg)		(!reg ? 1	\
				    : reg < PR_ROT_BASE ? prs[reg]	\
				    : reg + rrbp < NPRS ? prs[reg+rrbp]  \
					: prs[reg+rrbp-PR_ROT_SIZE])

#define PrWrt(reg, val)		(!reg ? 0	\
				    : reg < PR_ROT_BASE ? (prs[reg] = val)  \
					: reg + rrbp < NPRS ?	\
					    (prs[reg+rrbp] = val)	\
					    : (prs[reg+rrbp-PR_ROT_SIZE] = val))

#define FrRd(reg)		(reg < FR_ROT_BASE	\
				    ? frs[reg]	\
				    : frs[frmap[reg+rrbf]])

#define FrWrt(reg, val)		(reg <= 1 ? ZERO_FREG	    \
				    : reg < FR_ROT_BASE ? (frs[reg] = val)  \
					: (frs[frmap[reg+rrbf]] = val))

#define BrRd(reg)		brs[reg]
#define BrWrt(reg, val)		brs[reg] = val

#define ArRd(reg)		ars[reg]
#define ArWrt(reg, val)		ars[reg] = val

#define CrRd(reg)		crs[reg]
#define CrWrt(reg, val)		crs[reg] = val

#define RrRd(reg)		rrs[reg]
#define RrWrt(reg, val)		rrs[reg] = val

#define PkrRd(reg)		pkrs[reg]
#define PkrWrt(reg, val)	pkrs[reg] = val

#define DbrRd(reg)		dbrs[reg]
#define DbrWrt(reg, val)	dbrs[reg] = val

#define IbrRd(reg)		ibrs[reg]
#define IbrWrt(reg, val)	ibrs[reg] = val

#define PmcRd(reg)		pmcs[reg]
#define PmcWrt(reg, val)	pmcs[reg] = val

#define PmdRd(reg)		pmds[reg]
#define PmdWrt(reg, val)	pmds[reg] = val

#define MsrRd(reg)		msrs[reg]
#define MsrWrt(reg, val)	msrs[reg] = val

#define CpuidRd(reg)		cpuids[reg]

/* GRs */
#define SP	grs[SP_ID].val
#define SPNAT	grs[SP_ID].nat
#define TP	grs[TP_ID].val
#define TPNAT	grs[TP_ID].nat

/* BRs */
#define RP	brs[RP_ID]

/* ARs */
#define FPSR	ars[FPSR_ID]
#define FPSR_SF3	BitfR(FPSR,6,13)
#define FPSR_SF3_FLAGS	X_FPSR_SF3_FLAGS(FPSR)
#define FPSR_SF3_CTRLS	X_FPSR_SF3_CTRLS(FPSR)
#define FPSR_SF2	BitfR(FPSR,19,13)
#define FPSR_SF2_FLAGS	X_FPSR_SF2_FLAGS(FPSR)
#define FPSR_SF2_CTRLS	X_FPSR_SF2_CTRLS(FPSR)
#define FPSR_SF1	BitfR(FPSR,32,13)
#define FPSR_SF1_FLAGS	X_FPSR_SF1_FLAGS(FPSR)
#define FPSR_SF1_CTRLS	X_FPSR_SF1_CTRLS(FPSR)
#define FPSR_SF0	BitfR(FPSR,45,13)
#define FPSR_SF0_FLAGS	X_FPSR_SF0_FLAGS(FPSR)
#define FPSR_SF0_CTRLS	X_FPSR_SF0_CTRLS(FPSR)
#define FPSR_TRAPS	BitfR(FPSR,58,6)
#define X_FPSR_SF0_FLAGS(x)	BitfR(x,45,6)
#define X_FPSR_SF0_CTRLS(x)	BitfR(x,51,7)
#define X_FPSR_SF0_TD(x)	BitfR(x,51,1)
#define X_FPSR_SF0_PC(x)	BitfR(x,54,2)
#define X_FPSR_SF1_FLAGS(x)	BitfR(x,32,6)
#define X_FPSR_SF1_CTRLS(x)	BitfR(x,38,7)
#define X_FPSR_SF1_PC(x)	BitfR(x,41,2)
#define X_FPSR_SF2_FLAGS(x)	BitfR(x,19,6)
#define X_FPSR_SF2_CTRLS(x)	BitfR(x,25,7)
#define X_FPSR_SF2_PC(x)	BitfR(x,28,2)
#define X_FPSR_SF3_FLAGS(x)	BitfR(x,6,6)
#define X_FPSR_SF3_CTRLS(x)	BitfR(x,12,7)
#define X_FPSR_SF3_PC(x)	BitfR(x,15,2)

#define ITC	ars[ITC_ID]
#define RSC	ars[RSC_ID]
#define RSC_LOADRS	XRSC_LOADRS(RSC)
#define RSC_BE	XRSC_BE(RSC)
#define RSC_PL	XRSC_PL(RSC)
#define RSC_MODE	XRSC_MODE(RSC)

#define BSP	ars[BSP_ID]
#define BSPST	ars[BSPST_ID]
#define RNAT	ars[RNAT_ID]
#define UNAT	ars[UNAT_ID]
#define CCV	ars[CCV_ID]

#define K0	ars[K0_ID]
#define K1	ars[K1_ID]
#define K2	ars[K2_ID]
#define K3	ars[K3_ID]
#define K4	ars[K4_ID]
#define K5	ars[K5_ID]
#define K6	ars[K6_ID]
#define K7	ars[K7_ID]

#define LC	ars[LC_ID]
#define EC	ars[EC_ID]
#define EC_CNT	BitfR(EC,58,6)
#define PFS	ars[PFS_ID]
#define PFS_PPL	XFS_PPL(PFS)
#define PFS_PEC	XFS_PEC(PFS)
#define PFM_RRBP	XFM_RRBP(PFS)
#define PFM_RRBF	XFM_RRBF(PFS)
#define PFM_RRBG	XFM_RRBG(PFS)
#define PFM_SOR		XFM_SOR(PFS)
#define PFM_SOIL	XFM_SOIL(PFS)
#define PFM_SOF		XFM_SOF(PFS)

/* CRs */
#define DCR	crs[DCR_ID]
#define DCR_DD	BitfR(DCR,49,1)
#define DCR_DA	BitfR(DCR,50,1)
#define DCR_DR	BitfR(DCR,51,1)
#define DCR_DX	BitfR(DCR,52,1)
#define DCR_DK	BitfR(DCR,53,1)
#define DCR_DP	BitfR(DCR,54,1)
#define DCR_DM	BitfR(DCR,55,1)
#define DCR_LC	BitfR(DCR,61,1)
#define DCR_BE	BitfR(DCR,62,1)
#define DCR_PP	BitfR(DCR,63,1)
#define ITM	crs[ITM_ID]
#define IVA	crs[IVA_ID]
#define PTA	crs[PTA_ID]
#define IPSR	crs[IPSR_ID]
#define ISR	crs[ISR_ID]
#define IIP	crs[IIP_ID]
#define IFA	crs[IFA_ID]
#define ITIR	crs[ITIR_ID]
#define IIPA	crs[IIPA_ID]
#define IFS	crs[IFS_ID]
#define IFS_V	BitfR(IFS,0,1)
#define IIM	crs[IIM_ID]
#define IHA	crs[IHA_ID]
#define LID	crs[LID_ID]
#define IVR	crs[IVR_ID]
#define TPR	crs[TPR_ID]
#define TPR_MMI	BitfR(TPR,47,1)
#define TPR_MIC	BitfR(TPR,56,4)
#define EOI	crs[EOI_ID]
#define IRR0	crs[IRR0_ID]
#define IRR1	crs[IRR1_ID]
#define IRR2	crs[IRR2_ID]
#define IRR3	crs[IRR3_ID]
#define ITV	crs[ITV_ID]
#define PMV	crs[PMV_ID]
#define LRR0	crs[LRR0_ID]
#define LRR1	crs[LRR1_ID]
#define CMCV	crs[CMCV_ID]

#define IMASK(x)	BitfR(x,47,1)
#define DM(x)		BitfR(x,53,3)
#define VECTOR(x)	BitfR(x,56,8)

#define IFM_RRBP	XFM_RRBP(IFS)
#define IFM_RRBF	XFM_RRBF(IFS)
#define IFM_RRBG	XFM_RRBG(IFS)
#define IFM_SOR		XFM_SOR(IFS)
#define IFM_SOIL	XFM_SOIL(IFS)
#define IFM_SOF		XFM_SOF(IFS)

/* Debug Registers */
#define DBR_R(i)	BitfR(dbrs[i],0,1)
#define DBR_W(i)	BitfR(dbrs[i],1,1)
#define DBR_PLM(i)	BitfR(dbrs[i],4,4)
#define DBR_MASK(i)	BitfR(dbrs[i],8,56)

#define IBR_X(i)	BitfR(ibrs[i],0,1)
#define IBR_PLM(i)	BitfR(ibrs[i],4,4)
#define IBR_MASK(i)	BitfR(ibrs[i],8,56)

/* Performance Monitor Registers */
#define PMC_PM(x)	BitfR(x,57,1)

/* PSR fields */
#define PSR_VM	X_PSR_VM(psr)
#define PSR_IA	X_PSR_IA(psr)
#define PSR_BN	X_PSR_BN(psr)
#define PSR_ED	X_PSR_ED(psr)
#define PSR_RI	X_PSR_RI(psr)
#define PSR_SS	BitfR(psr,23,1)
#define PSR_DD	X_PSR_DD(psr)
#define PSR_DA	X_PSR_DA(psr)
#define PSR_ID	BitfR(psr,26,1)
#define PSR_IT	X_PSR_IT(psr)
#define PSR_MC	X_PSR_MC(psr)
#define PSR_IS	X_PSR_IS(psr)
#define PSR_CPL	X_PSR_CPL(psr)
#define PSR_RT	X_PSR_RT(psr)
#define PSR_TB	BitfR(psr,37,1)
#define PSR_LP	BitfR(psr,38,1)
#define PSR_DB	BitfR(psr,39,1)
#define PSR_SI	BitfR(psr,40,1)
#define PSR_DI	BitfR(psr,41,1)
#define PSR_PP	X_PSR_PP(psr)
#define PSR_SP	X_PSR_SP(psr)
#define PSR_DFH	BitfR(psr,44,1)
#define PSR_DFL	BitfR(psr,45,1)
#define PSR_DT	X_PSR_DT(psr)
#define PSR_PK	X_PSR_PK(psr)
#define PSR_I	BitfR(psr,49,1)
#define PSR_IC	X_PSR_IC(psr)
#define PSR_MFH	X_PSR_MFH(psr)
#define PSR_MFL	X_PSR_MFL(psr)
#define PSR_AC	X_PSR_AC(psr)
#define PSR_UP	X_PSR_UP(psr)
#define PSR_BE	X_PSR_BE(psr)
#define PSR_UM	X_PSR_UM(psr)

/* TLB */
#define ITLB_ED		(itlbInfo >> 7)

/* register arrays */
#ifndef NEW_MP
extern GREG grs[];
extern unsigned grmap[];
extern GREG bankedGrs[];
extern BOOL prs[];
extern FREG frs[];
extern unsigned frmap[];
extern REG brs[];
extern REG ars[];

extern REG psr;

extern REG crs[];
extern REG rrs[];
extern REG pkrs[];
extern REG dbrs[];
extern REG ibrs[];
extern REG pmcs[];
extern REG pmds[];
extern REG msrs[];
extern const REG cpuids[];
#endif

/* Contains first byte of information from ITLB, including ed, ar, pl */
extern BYTE itlbInfo;

extern unsigned n_stack_phys;
#ifndef NEW_MP
extern BYTE sof, soil, sor, rrbg, rrbf, rrbp;
extern unsigned bol;
extern int dirty, clean, invalid, dirtyNat, cleanNat;
extern BOOL cfle;

extern ADDR ip, heap;
#endif
extern ADDR ibBase;

extern BOOL fileLoaded, running, exited;
#ifdef NEW_MP
extern BOOL abi, unixABI;
#else
extern BOOL abi, unixABI, lp64;
#endif
extern ADDR max_sp;

#define updateGrMap(old) { int i; \
			  for (i = GR_STK_BASE+old; i < GR_STK_BASE+sof; i++) \
			      if (i + bol >= NGRS) \
				  grmap[i] = i + bol - N_STACK_PHYS; \
			      else \
				  grmap[i] = i + bol; \
			}

extern void freg2spill(FREG fr, BYTE *sign, WORD *exp, DWORD *mant);

#endif /* _SKI_STATE_H_ */
