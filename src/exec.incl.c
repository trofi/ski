/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Execution Head File
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
#include "exec.h"
#include "types.h"
#include "fields.h"
#include "state.h"
#include "float.h"
#include "sim.h"
#include "combfns.gen.h"
#include "simmem.h"
#include "tlb.h"
#include "instinfo.h"
#include "sign_ext.h"
#include "interruption.h"
#include "libsym.h"
#include "ia_types.h"
#include "ia_state.h"
#include "ia_exec.h"
#include "ssc.h"
#include "syscall_api.h"

#define HYPERLAZY		/* To support non-hyperlazy RSE, Ski must
				   implement a RNAT array instead of a single
				   reg */

#define NAT_BIT_SHIFT		3
#define NAT_BIT_MASK		0x3f

#define implemented_vm()	NO	/* not (yet) implemented */

/* XXX - use BitfR */
#define NAT_COLLECT_BIT(bsp)	(((bsp) >> NAT_BIT_SHIFT) & NAT_BIT_MASK)

#define NATS_GROW(bsp, nregs)	((NAT_COLLECT_BIT(bsp) + (nregs)) / 63)
#if 1
#define NATS_SHRINK(bsp, nregs)	((62 - NAT_COLLECT_BIT(bsp) + (nregs)) / 63)
#else
#define NATS_SHRINK(bsp, nregs)	\
		( ((NAT_COLLECT_BIT(bsp) - (nregs)) == 63) ? \
		    0 : ((62 - NAT_COLLECT_BIT(bsp) + (nregs)) / 63) )
#endif

#define ALU_1OP_EX(expr)	\
    do {			\
	DST1    = expr;		\
	DSTNAT1 = SRCNAT1;	\
    } while (0)

#define ALU_2OP_EX(expr)		\
    do {				\
	DST1    = expr;			\
	DSTNAT1 = SRCNAT1 || SRCNAT2;	\
    } while (0)

#define CMP_EX(cond)			\
    do {				\
	if (QUAL) {			\
	    BOOL res = cond;		\
					\
	    CMPRES1 = res;		\
	    CMPRES2 = !res;		\
	} else {			\
	    CMPRES1 = 0;		\
	    CMPRES2 = 0;		\
	}				\
	DSTNAT1 = SRCNAT1 || SRCNAT2;	\
    } while (0)

#define ALAT_WRITE_CHECK	use_alat
#define FM_mem_cache_update(a,b)
#define FM_rse_cache_update(a,b,c,d)

#define LD_EX(type,size,nat1)						\
    do {								\
	type val;							\
									\
	if (OUT_OF_FRAME(r1)) {						\
	    illegalOpFault();						\
	    return StFault;						\
	}								\
	if (SRCNAT1) {							\
	    regNatConsumptionFault(MEM_LD_ACCESS);			\
	    return StFault;						\
	}								\
	FM_mem_cache_update(SRC1, info);				\
	if (memRd##size(SRC1, MEM_LD_ACCESS, &val) == -1)		\
	    return StFault;						\
	DST1 = val;							\
	DSTNAT1 = nat1;							\
    } while (0)

#define LD16_EX(type,size,nat1)						\
    do {								\
	type val;							\
									\
	if (OUT_OF_FRAME(r1)) {						\
	    illegalOpFault();						\
	    return StFault;						\
	}								\
	if (SRCNAT1) {							\
	    regNatConsumptionFault(MEM_LD_ACCESS);			\
	    return StFault;						\
	}								\
	FM_mem_cache_update(SRC1, info);				\
	if (memRd##size(SRC1, MEM_LD_ACCESS, &val) == -1)		\
	    return StFault;						\
	DST1 = PSR_BE ? val.left : val.right;				\
	DSTNAT1 = nat1;							\
	DST2 = PSR_BE ? val.right : val.left;				\
    } while (0)

#define LD_A_EX(type,size)						\
    do {								\
	type val;							\
	int ret;							\
									\
	if (OUT_OF_FRAME(r1)) {						\
	    illegalOpFault();						\
	    return StFault;						\
	}								\
	if (SRCNAT1) {							\
	    regNatConsumptionFault(ADV_LD_ACCESS);			\
	    return StFault;						\
	}								\
	FM_mem_cache_update(SRC1, info);				\
	if ((ret = memRd##size(SRC1, ADV_LD_ACCESS, &val)) == -1)	\
	    return StFault;						\
	else if (ret == 1)						\
	    DST1 = 0;							\
	else /* ret == 0 */						\
	    DST1 = val;							\
	DSTNAT1 = 0;							\
	alat_inval_single_entry(NO, r1);				\
	if (ALAT_WRITE_CHECK && !ret)					\
	    alat_write(NO, r1, SRC1, size);				\
    } while (0)

#define LD_S_EX(type,size)						\
    do {								\
	type val;							\
	int ret;							\
									\
	if (OUT_OF_FRAME(r1)) {						\
	    illegalOpFault();						\
	    return StFault;						\
	}								\
	FM_mem_cache_update(SRC1, info);				\
	if (PSR_ED || SRCNAT1 ||					\
	    (ret = specRd##size(SRC1, SPEC_LD_ACCESS, &val)) == 1) {	\
	    DST1    = 0;						\
	    DSTNAT1 = 1;						\
	} else if (!ret) {						\
	    DST1    = val;						\
	    DSTNAT1 = 0;						\
	} else								\
	    return StFault;						\
    } while (0)

#define LD_SA_EX(type,size)						\
    do {								\
	type val;							\
	int ret = 1;							\
									\
	if (OUT_OF_FRAME(r1)) {						\
	    illegalOpFault();						\
	    return StFault;						\
	}								\
	FM_mem_cache_update(SRC1, info);				\
	if (PSR_ED || SRCNAT1 ||					\
	    (ret = specRd##size(SRC1, SPEC_ADV_LD_ACCESS, &val)) == 1) {\
	    DST1    = 0;						\
	    DSTNAT1 = 1;						\
	} else if (!ret) {						\
	    DST1    = val;						\
	    DSTNAT1 = 0;						\
	} else								\
	    return StFault;						\
	alat_inval_single_entry(NO, r1);				\
	if (ALAT_WRITE_CHECK && !ret)					\
	    alat_write(NO, r1, SRC1, size);				\
    } while (0)

#define LD_C_NC_EX(type,size)						\
    do {								\
	int miss;							\
									\
	if (OUT_OF_FRAME(r1)) {						\
	    illegalOpFault();						\
	    return StFault;						\
	}								\
	if (SRCNAT1) {							\
	    regNatConsumptionFault(MEM_LD_ACCESS);			\
	    return StFault;						\
	}								\
	miss = ALAT_WRITE_CHECK ? alat_cmp(NO, r1, NO) : 1;		\
	if (miss) {							\
	    LD_EX(type,size,NO);					\
	    if (ALAT_WRITE_CHECK)					\
		alat_write(NO, r1, SRC1, size);				\
	} else								\
	    GrRd(r1, DST1, DSTNAT1, NO);				\
    } while (0)

#define LD_C_CLR_EX(type,size)						\
    do {								\
	int miss;							\
									\
	if (OUT_OF_FRAME(r1)) {						\
	    illegalOpFault();						\
	    return StFault;						\
	}								\
	if (SRCNAT1) {							\
	    regNatConsumptionFault(MEM_LD_ACCESS);			\
	    return StFault;						\
	}								\
	miss = ALAT_WRITE_CHECK ? alat_cmp(NO, r1, YES) : 1;		\
	if (miss)							\
	    LD_EX(type,size,NO);					\
	else								\
	    GrRd(r1, DST1, DSTNAT1, NO);				\
    } while (0)

#define LDF_EX(type,size,fn)						\
    do {								\
	type val;							\
									\
	if (f1 <= 1) {							\
	    illegalOpFault();						\
	    return StFault;						\
	}								\
	if ((PSR_DFH && FPHI(f1)) || (PSR_DFL && FPLO(f1))) {		\
	    disabledFpregFault(FPHI(f1) << 1 | FPLO(f1), MEM_LD_ACCESS);\
	    return StFault;						\
	}								\
	if (SRCNAT1) {							\
	    regNatConsumptionFault(MEM_LD_ACCESS);			\
	    return StFault;						\
	}								\
	FM_mem_cache_update(SRC1, info);				\
	if (memRd##size(SRC1, MEM_LD_ACCESS, &val) == -1)		\
	    return StFault;						\
	FDST1 = fn##2freg(val);						\
    } while (0)

#define LDF_A_EX(type,size,fn,clrexp)					\
    do {								\
	type val;							\
	int ret;							\
									\
	if (f1 <= 1) {							\
	    illegalOpFault();						\
	    return StFault;						\
	}								\
	if ((PSR_DFH && FPHI(f1)) || (PSR_DFL && FPLO(f1))) {		\
	    disabledFpregFault(FPHI(f1) << 1 | FPLO(f1), ADV_LD_ACCESS);\
	    return StFault;						\
	}								\
	if (SRCNAT1) {							\
	    regNatConsumptionFault(ADV_LD_ACCESS);			\
	    return StFault;						\
	}								\
	FM_mem_cache_update(SRC1, info);				\
	if ((ret = memRd##size(SRC1, ADV_LD_ACCESS, &val)) == -1)	\
	    return StFault;						\
	else if (ret == 1)						\
	    clrexp = 0;							\
	alat_inval_single_entry(YES, f1);				\
	if (ALAT_WRITE_CHECK && !ret)					\
	    alat_write(YES, f1, SRC1, size);				\
	FDST1 = fn##2freg(val);						\
    } while (0)

#define LDF_S_EX(type,size,fn)						\
    do {								\
	BOOL fphi, fplo;						\
	type val;							\
	int ret;							\
									\
	if (f1 <= 1) {							\
	    illegalOpFault();						\
	    return StFault;						\
	}								\
	fphi = FPHI(f1);						\
	fplo = FPLO(f1);						\
	if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {			\
	    disabledFpregFault(fphi << 1 | fplo, SPEC_LD_ACCESS);	\
	    return StFault;						\
	}								\
	FM_mem_cache_update(SRC1, info);				\
	if (PSR_ED || SRCNAT1 ||					\
	    (ret = specRd##size(SRC1, SPEC_LD_ACCESS, &val)) == 1) {	\
	    FDST1.special = YES;					\
	    FDST1.class = CLASS_NAT;					\
	} else if (!ret)						\
	    FDST1 = fn##2freg(val);					\
	else								\
	    return StFault;						\
    } while (0)

#define LDF_SA_EX(type,size,fn)						\
    do {								\
	BOOL fphi, fplo;						\
	type val;							\
	int ret = 1;							\
									\
	if (f1 <= 1) {							\
	    illegalOpFault();						\
	    return StFault;						\
	}								\
	fphi = FPHI(f1);						\
	fplo = FPLO(f1);						\
	if ((PSR_DFH && fphi) || (PSR_DFL && fplo)) {			\
	    disabledFpregFault(fphi << 1 | fplo, SPEC_ADV_LD_ACCESS);	\
	    return StFault;						\
	}								\
	FM_mem_cache_update(SRC1, info);				\
	if (PSR_ED || SRCNAT1 ||					\
	    (ret = specRd##size(SRC1, SPEC_ADV_LD_ACCESS, &val)) == 1) {\
	    FDST1.special = YES;					\
	    FDST1.class = CLASS_NAT;					\
	} else if (!ret)						\
	    FDST1 = fn##2freg(val);					\
	else								\
	    return StFault;						\
	alat_inval_single_entry(YES, f1);				\
	if (ALAT_WRITE_CHECK && !ret)					\
	    alat_write(YES, f1, SRC1, size);				\
    } while (0)

#define LDF_C_NC_EX(type,size,fn)					\
    do {								\
	int miss;							\
									\
	if (f1 <= 1) {							\
	    illegalOpFault();						\
	    return StFault;						\
	}								\
	if ((PSR_DFH && FPHI(f1)) || (PSR_DFL && FPLO(f1))) {		\
	    disabledFpregFault(FPHI(f1) << 1 | FPLO(f1), MEM_LD_ACCESS);\
	    return StFault;						\
	}								\
	if (SRCNAT1) {							\
	    regNatConsumptionFault(MEM_LD_ACCESS);			\
	    return StFault;						\
	}								\
	miss = ALAT_WRITE_CHECK ? alat_cmp(YES, f1, NO) : 1;		\
	if (miss) {							\
	    LDF_EX(type,size,fn);					\
	    if (ALAT_WRITE_CHECK)					\
		alat_write(YES, f1, SRC1, size);			\
	} else								\
	    FDST1 = FrRd(f1);						\
    } while (0)

#define LDF_C_CLR_EX(type,size,fn)					\
    do {								\
	int miss;							\
									\
	if (f1 <= 1) {							\
	    illegalOpFault();						\
	    return StFault;						\
	}								\
	if ((PSR_DFH && FPHI(f1)) || (PSR_DFL && FPLO(f1))) {		\
	    disabledFpregFault(FPHI(f1) << 1 | FPLO(f1), MEM_LD_ACCESS);\
	    return StFault;						\
	}								\
	if (SRCNAT1) {							\
	    regNatConsumptionFault(MEM_LD_ACCESS);			\
	    return StFault;						\
	}								\
	miss = ALAT_WRITE_CHECK ? alat_cmp(YES, f1, YES) : 1;		\
	if (miss)							\
	    LDF_EX(type,size,fn);					\
	else								\
	    FDST1 = FrRd(f1);						\
    } while (0)

#define LDFP_MEM(size,var)						\
    do {								\
	FM_mem_cache_update(SRC1, info);				\
	if (memRd##size(SRC1, MEM_LD_ACCESS, var) == -1)		\
	    return StFault;						\
    } while (0)

#define LDFP_ADV(size,var)						\
    do {								\
	FM_mem_cache_update(SRC1, info);				\
	if ((ret = memRd##size(SRC1, ADV_LD_ACCESS, var)) == -1)	\
	    return StFault;						\
	alat_inval_single_entry(YES, f1);				\
	if (ALAT_WRITE_CHECK && !ret)					\
	    alat_write(YES, f1, SRC1, size);				\
    } while (0)

#define LDFP_DST_S()							\
    do {								\
	WORD val1, val2;						\
									\
	val1 = PSR_BE ? WD0(dval) : WD1(dval);				\
	val2 = PSR_BE ? WD1(dval) : WD0(dval);				\
	FDST1 = sgl2freg(val1);						\
	FDST2 = sgl2freg(val2);						\
    } while (0)

#define LDFP_DST_D()							\
    do {								\
	FDST1 = dbl2freg(PSR_BE ? val.left : val.right);		\
	FDST2 = dbl2freg(PSR_BE ? val.right : val.left);		\
    } while (0)

#define LDFP_DST_8()							\
    do {								\
	if (PSR_BE) {							\
	    FDST1 = dword2freg(mant.left);				\
	    FDST2 = dword2freg(mant.right);				\
	} else {							\
	    FDST1 = dword2freg(mant.right);				\
	    FDST2 = dword2freg(mant.left);				\
	}								\
    } while (0)

#define LDFP_SPEC(size,atype,var,type)					\
    do {								\
	FM_mem_cache_update(SRC1, info);				\
	if (PSR_ED || SRCNAT1 ||					\
	    (ret = specRd##size(SRC1, atype, var)) == 1) {		\
	    FDST1.special = YES;					\
	    FDST1.class = CLASS_NAT;					\
	    FDST2.special = YES;					\
	    FDST2.class = CLASS_NAT;					\
	} else if (!ret)						\
	    LDFP_DST_##type();						\
	else								\
	    return StFault;						\
    } while (0)

#define POST_INC()	(DST2 = SRC1 + SRC2, DSTNAT2 = SRCNAT2)
#define POST_INC_SPEC()	(DST2 = SRC1 + SRC2, DSTNAT2 = SRCNAT1 || SRCNAT2)

#define LDFP_POST_INC(inc)	(DST1 = SRC1 + inc, DSTNAT1 = 0)
#define LDFP_POST_INC_SPEC(inc)	(DST1 = SRC1 + inc, DSTNAT1 = SRCNAT1)

/* XXX - should be named ILLEGAL_GR_ACCESS? */
#define OUT_OF_FRAME(reg)	((reg) >= GR_STK_BASE+sof || !(reg))

#define FPHI(reg)	((reg) >= 32)
#define FPLO(reg)	((reg) >= 2 && (reg) <= 31)

extern BOOL use_alat;

void arithFlagsFromEflags(void);
void setIAmode(void);

void alat_inval_single_entry(BOOL fpreg, int rega);
void alat_write(BOOL fpreg, int rega, ADDR pa, unsigned size);
int alat_cmp(BOOL fpreg, int rega, BOOL clearit);
void alat_inval_multiple_entries(ADDR pa, unsigned size);
void alat_inval_all_entries(void);

/* Forward declarations */
int rse_load(void);
int rse_store(void);
FREG qword2freg(QWORD);
Status swizzleComb(INSTINFO*);

/* RSE Auxiliary Functions */

static int wrap(int reg_num)
{
    if (reg_num < 0)
	return reg_num + N_STACK_PHYS;
    if (reg_num > (N_STACK_PHYS - 1))
	return reg_num - N_STACK_PHYS;
    return reg_num;
}

static void rse_invalidate_non_current_regs(void)
{
    dirty = dirtyNat = 0;
    clean = cleanNat = 0;
    invalid = N_STACK_PHYS - sof;
}

/*
 * Returns the number of GRs stored in the RSE backing store or -1 on faults
 */
int rse_store(void)
{
    unsigned ncb;
    int status;

#ifdef RSE_DEBUG
rseDB("mandatory RSE store");
#endif
    if ((ncb = NAT_COLLECT_BIT(BSPST)) == 63) {
	if (!rseWrt(BSPST, RNAT))
	    return -1;
	SIGN(RNAT) = 0;
	FM_rse_cache_update(BSPST, UATAG_RSEWRT, RFILE_AR, RNAT_ID);
	BSPST += 8;
	dirtyNat--;
#ifndef HYPERLAZY
/* In hyperlazy implementations this is not needed */
	cleanNat++;
#endif
	status = 0;
    } else {
	unsigned RSEStoreRegNum;
#if 1
	RSEStoreRegNum = wrap((int)bol - dirty) + GR_STK_BASE;
#else
/* If we ever enable this code, change 128 to appropriate #define */
	RSEStoreRegNum = grmap[128 - (int)dirty];
#endif
	if (!rseWrt(BSPST, grs[RSEStoreRegNum].val))
	    return -1;
	if (grs[RSEStoreRegNum].nat)
	    RNAT |= (REG)1 << ncb;
	else
	    RNAT &= ~((REG)1 << ncb);
	FM_rse_cache_update(BSPST, UATAG_RSEWRT, RFILE_GR, RSEStoreRegNum);
	BSPST += 8;
	dirty--;
#ifndef HYPERLAZY
/* Hyperlazy implementations increment invalid instead of clean */
	clean++;
#else
	invalid++;
#endif
	status = 1;
    }
    /* XXX - can this be avoided in app. mode? */
    /* Setting these PSR bits is valid *only* for mandatory RSE stores.
	Change this if this function is to be used for eager RSE stores */
    PSR_DA = PSR_DD = NO;
    return status;
}

/*
 * Returns the number of GRs loaded from the RSE backing store or -1 on faults
 */
/*
 * This routine causes a segmentation violation when compiled w/ opt > 1
 * if grmap is used instead of the wrap call
 */
int rse_load(void)
{
    ADDR BspLoad = BSP - (clean + cleanNat + dirty + dirtyNat + 1) * 8;
    unsigned ncb;
    int status;

    if ((ncb = NAT_COLLECT_BIT(BspLoad)) == 63) {
	if (!rseRd(BspLoad, &RNAT))
	    return -1;
	FM_rse_cache_update(BspLoad, UATAG_RSERD, RFILE_AR, RNAT_ID);
	cleanNat++;
	status = 0;
    } else {
	DWORD val;
	unsigned RSELoadRegNum;

	if (!rseRd(BspLoad, &val))
	    return -1;
#if 1
	RSELoadRegNum = wrap((int)bol - (clean + dirty + 1)) + GR_STK_BASE;
#else
/* XXX If we ever enable this code, change 127 to appropriate #define */
	RSELoadRegNum = grmap[127 - (int)clean - (int)dirty];
#endif
	grs[RSELoadRegNum].val = val;
	grs[RSELoadRegNum].nat = (RNAT >> ncb) & 0x1;	/* XXX - use BitfX */
	FM_rse_cache_update(BspLoad, UATAG_RSERD, RFILE_GR, RSELoadRegNum);
	clean++;
	invalid--;
	status = 1;
    }
    PSR_DA = PSR_DD = NO;
    return status;
}

static BOOL rse_new_frame(int growth)
{
    if (growth <= (int)invalid)
	invalid -= growth;
    else {
	growth -= invalid;
	if (growth <= clean) {
	    clean -= growth;
	    cleanNat = NATS_SHRINK(BSP, clean + dirty + 1) - dirtyNat;
	    invalid = 0;
	} else {
	    int i, j;

	    growth -= clean;
	    for (i = growth; i > 0; i -= j) {
		if ((j = rse_store()) == -1)
		    return NO;
		/* The SDM includes call to... */
		/* deliver_unmasked_pending_external_interrupt(); */
	    }
	    clean = cleanNat = 0;
	    invalid = 0;
	}
    }
    return YES;
}

static void rse_restore_frame(int preserved, int growth, int old)
{
    int preservedNat = NATS_SHRINK(BSP, preserved);

    BSP -= (preserved + preservedNat) * 8;

    /* Growing into the dirty region in a return */
    if (growth > (invalid + clean)) {
	invalid  += preserved + old;
	dirty    -= preserved;
	dirtyNat -= preservedNat;
	    /* If there were eager stores, it's possible that dirty
	       and dirtyNat become negative, which is wrong */

	sof = soil = sor = rrbg = rrbf = rrbp = 0;
	return;
    }

    invalid -= growth;

    /* Growing into the clean region */
    if (invalid < 0) {
	clean   += invalid;
	cleanNat = NATS_SHRINK(BSP, clean + dirty + 1) - dirtyNat;
	invalid  = 0;
    }

    if (preserved <= (int)dirty) {
	dirty    -= preserved;
	dirtyNat -= preservedNat;
    } else {
	preserved    -= dirty;
	preservedNat -= dirtyNat;
	if (preserved <= (int)clean) {
	    clean    -= preserved;
	    cleanNat -= preservedNat;
	    dirty     = dirtyNat = 0;
	    BSPST     = BSP;
	} else {
	    preserved    -= clean;
	    preservedNat -= cleanNat;
	    clean         = cleanNat = 0;
	    dirty         = -preserved;
	    dirtyNat      = -preservedNat;
	    BSPST         = BSP - (dirty + dirtyNat) * 8;
	    cfle          = YES;
	    preInst      |= PRE_CFLE;
	}
    }
}

static void rotate_regs(void)
{
    if (sor)
	rrbg = rrbg ? (rrbg - 1) : (sor - 1);
    rrbf = rrbf ? (rrbf - 1) : (FR_ROT_SIZE - 1);
    rrbp = rrbp ? (rrbp - 1) : (PR_ROT_SIZE - 1);
}

/* Multimedia Auxiliary Functions */

static unsigned numOnes(DWORD n)
{
    unsigned cnt;

    for (cnt = 0; n; cnt++)
	n &= n - 1;	/* clear least significant set bit */
    return cnt;
}

static BYTE unsignedSat8(long long val)
{
    if (val < 0x00LL)
	return 0x00;

    if (val > 0xFFLL)
	return 0xFF;

    return val;
}

static BYTE signedSat8(long long val)
{
    if (val < 0xFFFFFFFFFFFFFF80LL)
	return 0x80;

    if (val > 0x7FLL)
	return 0x7F;

    return val;
}

static HWORD unsignedSat16(long long val)
{
    if (val < 0x0000LL)
	return 0x0000;

    if (val > 0xFFFFLL)
	return 0xFFFF;

    return val;
}

static HWORD signedSat16(long long val)
{
    if (val < 0xFFFFFFFFFFFF8000LL)
	return 0x8000;

    if (val > 0x7FFFLL)
	return 0x7FFF;

    return val;
}

static HWORD shaddSat16(long long val1, long long val2, int shamt)
{
    long long temp = val1 << shamt;

    if (temp < 0xFFFFFFFFFFFF8000LL)
	return 0x8000;

    if (temp > 0x7FFFLL)
	return 0x7FFF;

    return signedSat16(temp + val2);
}

/* FP Auxiliary Functions */

/*********************************************************
 * FP Value    special class  unorm  sign   exp   mant   *
 *  NaTVal       YES    NAT     -      -     -     -     *
 *  Infinity     YES    INF     -     0/1    -     -     *
 *  sNaN         YES    SNAN    -     0/1    -     m     *
 *  qNaN         YES    QNAN    -     0/1    -     m     *
 *  Unsupp       YES    UNSUPP  -?    0/1    -     m     *
 *  Normal       NO     NONE     0    0/1   !0     m     *
 *  Zero         NO     NONE    64    0/1    0     -     *
 *  Pseudo-Zero  NO     NONE    64    0/1   !0     -     *
 *  FR0          NO     NONE   255    0      0     -     *
 *  Exp0         NO     DE0    0..63  0/1   C040   m     *
 *  Unorm        NO     NONE   1..63  0/1   !0     m     *
 *********************************************************/

static int numLzeroes(DWORD *mant)
{
    int lz = 0;

    if (!*mant)
	return 64;

    if (!(*mant >> 32)) {
	*mant <<= 32;
	lz += 32;
    }
    if (!(*mant >> 48)) {
	*mant <<= 16;
	lz += 16;
    }
    if (!(*mant >> 56)) {
	*mant <<= 8;
	lz += 8;
    }
    if (!(*mant >> 60)) {
	*mant <<= 4;
	lz += 4;
    }
    if (!(*mant >> 62)) {
	*mant <<= 2;
	lz += 2;
    }
    if (!(*mant >> 63)) {
	*mant <<= 1;
	lz += 1;
    }
    return lz;
}

#ifdef HPUX9
#pragma OPTIMIZE OFF
#endif
FREG fill2freg(BYTE sign, WORD exp, DWORD mant)
{
    FREG fr;

    if (NATVAL(sign, exp, mant)) {
	fr.special = YES;
	fr.class = CLASS_NAT;
	return fr;
    }

    fr.sign = sign;
    if (exp == 0x1FFFF) {		/* special */
	fr.special = YES;
	if (mant == 0x8000000000000000ULL)
	    fr.class = CLASS_INF;
	else if (mant >= 0xC000000000000000ULL) {
	    fr.class = CLASS_QNAN;
	    fr.mant = mant;
	} else if (mant >= 0x8000000000000001ULL) {
	    fr.class = CLASS_SNAN;
	    fr.mant = mant;
	} else {
	    fr.class = CLASS_UNSUPP;
	    fr.mant = mant;
	}
	return fr;
    }

    if (mant) {
	fr.unorm = numLzeroes(&mant);
	if (exp)
	    fr.class = CLASS_NONE;
	else {
	    fr.class = CLASS_DE0;
	    exp = 0xC001;
	}
	fr.exp  = exp + EXTRA_BIAS - fr.unorm;
	fr.mant = mant;
    } else {		/* zero/pseudo-zero */
	fr.class = CLASS_NONE;
	fr.unorm = 64;
	fr.exp   = exp ? exp + EXTRA_BIAS : 0;
    }
    fr.special = NO;
    return fr;
}
#ifdef HPUX9
#pragma OPTIMIZE ON
#endif

static FREG sgl2freg(WORD val)
{
    BYTE sign;
    WORD exp;
    DWORD mant;

    sign = BitfX32(val,0,1);
    exp  = BitfX32(val,1,8);
    mant = BitfX32(val,9,23);

    if (!exp && mant) {		/* denormal */
	mant <<= 40;
	exp = 0xFF81;
    } else if (exp == 0xFF) {	/* inf/NaN */
	mant = (1ULL << 23 | mant) << 40;
	exp  = 0x1FFFF;
    } else if (exp > 0) {	/* normal */
	mant = (1ULL << 23 | mant) << 40;
	exp += 0xFF80;
    }
    return fill2freg(sign, exp, mant);
}

static FREG dbl2freg(DWORD val)
{
    BYTE sign;
    WORD exp;
    DWORD mant;

    sign = BitfX(val,0,1);
    exp  = BitfX(val,1,11);
    mant = BitfX(val,12,52);

    if (!exp && mant) {		/* denormal */
	mant <<= 11;
	exp = 0xFC01;
    } else if (exp == 0x7FF) {	/* inf/NaN */
	mant = (1ULL << 52 | mant) << 11;
	exp  = 0x1FFFF;
    } else if (exp > 0) {	/* normal */
	mant = (1ULL << 52 | mant) << 11;
	exp += 0xFC00;
    }
    return fill2freg(sign, exp, mant);
}

static FREG ext2freg(QWORD val)
{
    BYTE sign;
    WORD exp;
    DWORD mant;

    if (PSR_BE) {
	sign = BitfX(val.left,0,1);
	exp  = BitfX(val.left,1,15);
	mant = BitfX(val.left,16,48) << 16 | BitfX(val.right,0,16);
    } else {
	sign = BitfX(val.left,48,1);
	exp  = BitfX(val.left,49,15);
	mant = val.right;
    }

    if (exp == 0x7FFF)	/* inf/NaN */
	exp = 0x1FFFF;
    else if (exp > 0)	/* normal */
	exp += 0xC000;
    return fill2freg(sign, exp, mant);
}

static FREG dword2freg(DWORD mant)
{
    FREG fr;

    fr.sign = 0;
    fr.special = NO;
    if (mant) {
	fr.unorm = numLzeroes(&mant);
	fr.exp   = INTEGER_EXP - fr.unorm;
	fr.mant  = mant;
	fr.class = CLASS_NONE;
    } else {
	fr.unorm = 64;
	fr.exp   = INTEGER_EXP;
    }
    return fr;
}

FREG qword2freg(QWORD val)
{
    return fill2freg(BitfX(val.left,46,1), BitfX(val.left,47,17), val.right);
}

void freg2spill(FREG fr, BYTE *sign, WORD *exp, DWORD *mant)
{
    *sign = fr.sign;
    if (fr.special) {
	*exp = 0x1FFFF;
	switch (fr.class) {
	    case CLASS_QNAN:
		*mant = fr.mant;
		return;
	    case CLASS_SNAN:
		*mant = fr.mant;
		return;
	    case CLASS_NAT:
		*sign = 0;
		*exp = 0x1FFFE;
		*mant = 0x0ULL;
		return;
	    case CLASS_INF:
		*mant = 0x8000000000000000ULL;
		return;
	    case CLASS_UNSUPP:
		*mant = fr.mant;
		return;
	}
    }
    if (fr.unorm >= 64) {	/* zero/pseudo-zero */
	*exp  = fr.exp ? fr.exp - EXTRA_BIAS : 0;
	*mant = 0;
    } else {
	*exp  = fr.class == CLASS_DE0 ? 0 : fr.exp - EXTRA_BIAS + fr.unorm;
	*mant = fr.mant >> fr.unorm;
    }
}

static WORD freg2sgl(FREG fr)
{
    BYTE sign;
    WORD exp, val;
    DWORD mant;

    freg2spill(fr, &sign, &exp, &mant);
    BitfR32(val,0,1) = sign;
    if (SIGN(mant)) {
	BitfR32(val,1,1) = BitfX32(exp,15,1);
	BitfR32(val,2,7) = BitfX32(exp,25,7);
    } else
	BitfR32(val,1,8) = 0;
    BitfR32(val,9,23) = BitfX(mant,1,23);
    return val;
}

static DWORD freg2dbl(FREG fr)
{
    BYTE sign;
    WORD exp;
    DWORD mant, dval;

    freg2spill(fr, &sign, &exp, &mant);
    BitfR(dval,0,1) = sign;
    if (SIGN(mant)) {
	BitfR(dval,1,1)  = BitfX32(exp,15,1);
	BitfR(dval,2,10) = BitfX32(exp,22,10);
    } else
	BitfR(dval,1,11) = 0;
    BitfR(dval,12,52) = BitfX(mant,1,52);
    return dval;
}

static QWORD freg2ext(FREG fr)
{
    BYTE sign;
    WORD exp;
    DWORD mant;
    QWORD qval;

    freg2spill(fr, &sign, &exp, &mant);
    BitfR(qval.left,0,1)   = sign;
    BitfR(qval.left,1,1)   = BitfX32(exp,15,1);
    BitfR(qval.left,2,14)  = BitfX32(exp,18,14);
    BitfR(qval.left,16,48) = BitfX(mant,0,48);
    BitfR(qval.right,0,16) = BitfX(mant,48,16);
    return qval;
}

static DWORD freg2dword(FREG fr)
{
    BYTE sign;
    WORD exp;
    DWORD mant;

    freg2spill(fr, &sign, &exp, &mant);
    return mant;
}

static void fpSimdFault(unsigned ret0, unsigned ret1)
{
    int code = 0;

    if (ret0 & FP_FAULT)
	code |= ret0 & FP_FAULT;
    if (ret1 & FP_FAULT)
	code |= (ret1 & FP_FAULT) << 4;
    fpExceptionFault(code);
}

static void fpSimdTrap(unsigned delta, unsigned ret0, unsigned ret1)
{
    int code = 0;

    if (ret0 & FP_TRAP)
	code |= (ret0 >> 11) << 4;
    if (ret1 & FP_TRAP)
	code |= (ret1 >> 11);
    fpExceptionTrap(delta, code);
}


/* Reserved Register/Field Functions */

static BOOL reservedRscField(REG val)
{
    return BitfX(val,0,34) || BitfX(val,48,11);
}

static BOOL reservedFpsrField(REG val)
{
    return BitfX(val,0,6) != 0 || BitfX(val,51,1) != 0 ||
	BitfX(val,15,2) == 1 || BitfX(val,28,2) == 1 ||
	BitfX(val,41,2) == 1 || BitfX(val,54,2) == 1;
}

static BOOL reservedPfsField(REG val)
{
    BYTE psof, psoil, psor, prrbg, prrbf, prrbp;

    if (BitfX(val,2,4) || BitfX(val,12,14))
	return YES;
    psof  = XFM_SOF(val);
    psoil = XFM_SOIL(val);
    psor  = XFM_SOR(val) << 3;
    prrbg = XFM_RRBG(val);
    prrbf = XFM_RRBF(val);
    prrbp = XFM_RRBP(val);
    if (psof > N_STACK_VIRT || psoil > psof || psor > psof ||
	(psor && prrbg >= psor) || (!psor && prrbg) ||
	prrbf >= FR_ROT_SIZE || prrbp >= PR_ROT_SIZE)
	return YES;
    return NO;
}

static BOOL reservedUserMaskField(REG val)
{
    return BitfX(val,0,58) || BitfX(val,63,1);
}

static BOOL reservedSystemMaskField(REG val)
{
    return BitfX(val,0,19) || BitfX(val,32,4) || BitfX(val,47,1) ||
	   BitfX(val,51,7) || BitfX(val,63,1);
}

static BOOL reservedDcrField(REG val)
{
    return BitfX(val,0,49) || BitfX(val,56,5);
}

static BOOL reservedPtaField(REG val)
{
    return BitfX(val,49,6) || BitfX(val,56,6) < 15 || BitfX(val,62,1);
}

BOOL reservedIpsrField(REG val)
{
    return BitfX(val,0,18) || BitfX(val,21,2) == 3 || BitfX(val,32,4) ||
	   BitfX(val,47,1) || BitfX(val,51,7) || BitfX(val,63,1);
}

static BOOL reservedIsrField(REG val)
{
    return BitfX(val,0,20) || BitfX(val,21,2) == 3 ||
	   BitfX(val,32,8);
}

static BOOL reservedIfsField(REG val)
{
    BYTE psof, psoil, psor, prrbg, prrbf, prrbp;

    if (BitfX(val,1,25))
	return YES;
    if (!BitfX(val,0,1))	/* if (new_IFS.v == 0) */
	return NO;
    psof  = XFM_SOF(val);
    psoil = XFM_SOIL(val);
    psor  = XFM_SOR(val) << 3;
    prrbg = XFM_RRBG(val);
    prrbf = XFM_RRBF(val);
    prrbp = XFM_RRBP(val);
    if (psof > N_STACK_VIRT || psoil > psof || psor > psof ||
	(psor && prrbg >= psor) || (!psor && prrbg) ||
	prrbf >= FR_ROT_SIZE || prrbp >= PR_ROT_SIZE)
	return YES;
    return NO;
}

static BOOL reservedLidField(REG val)
{
    return BitfX(val,48,16) != 0;
}

static BOOL reservedIvrField(REG val)
{
    return BitfX(val,0,56) != 0;
}

static BOOL reservedTprField(REG val)
{
    return BitfX(val,48,8) != 0;
}

static BOOL reservedItvField(REG val)
{
    return BitfX(val,48,3) || BitfX(val,52,4);
}

static BOOL reservedLrrField(REG val)
{
    return BitfX(val,49,1) || BitfX(val,52,1) ||
	   BitfX(val,53,3) == 1 || BitfX(val,53,3) == 3 || BitfX(val,53,3) == 6;
}

BOOL reservedCrField(unsigned cr, REG *val)
{
    switch (cr) {
	case DCR_ID:
	    if (reservedDcrField(*val))
		return YES;
	    break;
	case IVA_ID:
	    *val &= ~ONES(15);
	    break;
	case PTA_ID:
	    if (reservedPtaField(*val))
		return YES;
	    break;
	case IPSR_ID:
	    if (reservedIpsrField(*val))
		return YES;
	    break;
	case ISR_ID:
	    if (reservedIsrField(*val))
		return YES;
	    break;
	case IFS_ID:
	    if (reservedIfsField(*val))
		return YES;
	    break;
	case IHA_ID:
	    *val &= ~ONES(2);
	    break;
	case LID_ID:
	    if (reservedLidField(*val))
		return YES;
	    *val = WD1(*val);
	    break;
	/* XXX - Is this needed since IVR is read-only? */
	case IVR_ID:
	    if (reservedIvrField(*val))
		return YES;
	    break;
	case TPR_ID:
	    if (reservedTprField(*val))
		return YES;
	    *val &= 0x100F0;
	    break;
	case EOI_ID:
	    *val = 0;
	    break;
	case ITV_ID:
	case PMV_ID:
	case CMCV_ID:
	    if (reservedItvField(*val))
		return YES;
	    *val &= ONES(17) & ~0x1000;
	    break;
	case LRR0_ID:
	case LRR1_ID:
	    if (reservedLrrField(*val))
		return YES;
	    *val &= ONES(17) & ~0x1000;
	    break;
    }
    return NO;
}

static BOOL reservedRrField(REG val)
{
    return BitfX(val,0,32) || BitfX(val,32,24) >> rid_len || BitfX(val,62,1) ||
	   unimplPageSize(BitfX(val,56,6));
}

static void ignoredDbrField(unsigned ind, REG *val)
{
    if (ind % 2)
	BitfR(*val,2,2) = 0;
}

static void ignoredIbrField(unsigned ind, REG *val)
{
    if (ind % 2)
	BitfR(*val,1,3) = 0;
}

static BOOL reservedPkrField(REG val)
{
    return BitfX(val,0,32) || BitfX(val,32,24) >> key_len || BitfX(val,56,4);
}

static BOOL reservedPmcField(REG val)
{
    /* Check for unimplemented events */
    /* return BitfX(val,0,48) || BitfX(val,56,1); */
    return NO;
}

static BOOL reservedARi(unsigned ar)
{
    return ar <= 47 || (ar >= 67 && ar <= 111);
}

static BOOL reservedARm(unsigned ar)
{
    return (ar >=   8 && ar <=  15) ||
        ar ==  20		  ||
	   (ar >=  22 && ar <=  23) ||
        ar ==  31		  ||
	   (ar >=  33 && ar <=  35) ||
	   (ar >=  37 && ar <=  39) ||
	   (ar >=  41 && ar <=  43) ||
	   (ar >=  45 && ar <=  47) ||
	   (ar >=  64 && ar <= 111);
}

BOOL reservedAR(unsigned ar)
{
    return (ar >=   8 && ar <=  15) ||
        ar ==  20		  ||
	   (ar >=  22 && ar <=  23) ||
        ar ==  31		  ||
	   (ar >=  33 && ar <=  35) ||
	   (ar >=  37 && ar <=  39) ||
	   (ar >=  41 && ar <=  43) ||
	   (ar >=  45 && ar <=  47) ||
	   (ar >=  67 && ar <= 111);
}

static BOOL ignoredAR(unsigned ar)
{
    return (ar >=  48 && ar <=  63) ||
	   (ar >= 112 && ar <= 127);
}

BOOL reservedCR(unsigned cr)
{
    return (cr >=   3 && cr <=   7) ||
	   (cr >=  10 && cr <=  15) ||
        cr ==  18              ||
	   (cr >=  26 && cr <=  63) ||
	   (cr >=  75 && cr <=  79) ||
	   (cr >=  82 && cr <= 127);
}

static BOOL readonlyCR(unsigned cr)
{
    return cr == IRR0_ID || cr == IRR1_ID || cr == IRR2_ID || cr == IRR3_ID ||
	   cr == IVR_ID;
}

static BOOL interruptionCR(unsigned cr)
{
    return cr >= IPSR_ID && cr <= IHA_ID;
}

Status illComb(INSTINFO *info)
{
    illegalOpFault();
    return StFault;
}

Status illQpComb(INSTINFO *info)
{
    if (PrRd(qp) != 1)
	return StSuccess;
    illegalOpFault();
    return StFault;
}

Status illBQpComb(INSTINFO *info)
{
    if (PrRd(qp) != 1)
	return StSuccess;
    illegalOpFault();
    return StFault;
}

Status ignComb(INSTINFO *info)
{
    return StSuccess;
}

Status movGrComb(INSTINFO *info)
{
    REG src1;
    BOOL srcnat1;

    if (PrRd(qp) != 1)
	return StSuccess;
#if 0
    if (OUT_OF_FRAME(r1)) {
	illegalOpFault();
	return StFault;
    }
#endif
    GrRd(r3, src1, srcnat1, NO);
    GrWrt(r1, src1, srcnat1, NO);
    return StSuccess;
}

Status swizzleComb(INSTINFO *info)
{
    REG src2, dst1;
    BOOL srcnat1, srcnat2, dstnat1;

    if (PrRd(qp) != 1)
	return StSuccess;
#if 0
    if (OUT_OF_FRAME(r1)) {
	illegalOpFault();
	return StFault;
    }
#endif
    SRCNAT1 = 0;
    GrRd(r3, SRC2, SRCNAT2, NO);
    ALU_2OP_EX(BitfX(SRC2,32,2) << 61 | (WORD)SRC2);
    GrWrt(r1, DST1, DSTNAT1, NO);
    return StSuccess;
}
