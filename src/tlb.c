/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator TLB Routines
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

#include <string.h>
#include "std.h"
#include "bits.h"
#include "types.h"
#include "fields.h"
#include "state.h"
#include "sim.h"
#include "simmem.h"
#include "tlb.h"
#include "interruption.h"

#define PKR_KEY_MASK	(ONES(key_len) << 8 | 1)
#define TPA_MASK	0x0003FFFFFFFFF000ULL
#define VRN_MASK	0xE000000000000000ULL

#define ITLB_AR		BitfX8(itlbInfo,1,3)
#define ITLB_PL		BitfX8(itlbInfo,4,2)

enum {
    WB      = 0x0,
    UC      = 0x4,
    UCE     = 0x5,
    WC      = 0x6,
    NATPAGE = 0x7
};

/*##################### Globals - Exports ##################################*/

BYTE itlbInfo;

unsigned va_len = 61;	/* Range: 51 - 61.  Merced: 51 */
unsigned pa_len = 63;	/* Range: 32 - 63.  Merced: 44 */

unsigned rid_len = 24;	/* Merced: 18 */
unsigned key_len = 24;	/* Merced: 21 */
	/* Range: 18 <= rid <= key <= 24 */

/*##################### Local Variables ####################################*/

#if 0
static unsigned dtcSMsk = NDTCS - 1;
static unsigned itcSMsk = NITCS - 1;
#endif

#ifndef NEW_MP
/* XXX - Non-static for random code generators */
static TlbEntry dtrs[NDTRS], itrs[NITRS];
static TlbEntry dtcs[NDTCS], itcs[NITCS];
#endif

static REG pkrKeyMask;

/*##################### Macros #############################################*/

#define PTA_BASE	BitfR(PTA,3,46)
#define PTA_VRN		BitfX(PTA,0,3)
#define PTA_VF		BitfR(PTA,55,1)
#define PTA_SIZE	BitfR(PTA,56,6)
#define PTA_VE		BitfR(PTA,63,1)

#define RR_RID(x)	BitfX(RrRd(x),32,24)
#define RR_PS(x)	BitfR(RrRd(x),56,6)
#define RR_VE(x)	BitfR(RrRd(x),63,1)

#define TPA_ED(x)	BitfR(x,11,1)
#define TPA_AR(x)	BitfR(x,52,3)
#define TPA_PL(x)	BitfR(x,55,2)
#define TPA_D(x)	BitfR(x,57,1)
#define TPA_A(x)	BitfR(x,58,1)
#define TPA_MA(x)	BitfR(x,59,3)
#define TPA_P(x)	BitfR(x,63,1)

#define TAR_KEY(x)	BitfR(x,32,24)
#define TAR_PS(x)	BitfR(x,56,6)

#define RR(x)		BitfR(x,0,3)

#define VAmatch(e,va,rid)	((e)->vpn == ((va) & (e)->psm) && \
				 (e)->rid == rid)
#define physAddr(e,va)	((e)->ppn | ((va) & ~(e)->psm))
#define PurgeMatch(e,vam,mask,rid)	\
	(((e)->vpn & (mask)) == ((vam) & (e)->psm) && (e)->rid == rid)

/*##################### Functions ##########################################*/

static TlbEntry *tcInsert(TlbEntry tc[], unsigned tccnt,
			  TlbEntry tr[], unsigned trcnt,
			  ADDR tpa, ADDR tar, ADDR tva);
static BOOL accessRights(unsigned ar, unsigned pl, unsigned cpl,
			 Accesstype atype);
#ifdef DBGREGS
static BOOL search_DBR(ADDR adr, unsigned size, Accesstype atype, unsigned plm);
static BOOL search_IBR(ADDR adr, unsigned size, unsigned plm);
#endif

static TlbEntry *dtcs_head = NULL;
static TlbEntry *itcs_head = NULL;

static void init(TlbEntry tlb[], unsigned cnt)
{
    unsigned i;

    for (i = 0; i < cnt; i++) {
	tlb[i].psm = 0;
	tlb[i].vpn = 1;
	tlb[i].next = &tlb[i+1];
    }
    tlb[cnt-1].next = 0;

    if (tlb == itcs)
	itcs_head = &itcs[0];
    if (tlb == dtcs)
	dtcs_head = &dtcs[0];

}

void tlbInit(void)
{
    init(dtrs, NDTRS);
    init(itrs, NITRS);
    init(dtcs, NDTCS);
    init(itcs, NITCS);
    pkrKeyMask = PKR_KEY_MASK;
}

static void setQuickChk(TlbEntry *e)
{
    unsigned pl;

    e->quickchk = 0;
    if (!e->p || e->ma == NATPAGE || !e->a || !e->d)
	return;
    for (pl = 0; pl <= 3; pl++) {
	e->quickchk |=
	    (accessRights(e->ar, e->pl, pl, READ_ACCESS|WRITE_ACCESS) << 12 |
	     accessRights(e->ar, e->pl, pl, (Accesstype)WRITE_ACCESS) << 8 |
	     accessRights(e->ar, e->pl, pl, (Accesstype)READ_ACCESS) << 4) << pl;
    }
}

static void tlbInsert(TlbEntry *e, ADDR tva, ADDR tpa, ADDR tar)
{
    extern void bptLoad (void);
    extern int numBptsToLoad;
    int num_bpts = numBptsToLoad;
    e->ed  = TPA_ED(tpa);
    e->ar  = TPA_AR(tpa);
    e->pl  = TPA_PL(tpa);
    e->d   = TPA_D(tpa);
    e->a   = TPA_A(tpa);
    e->ma  = TPA_MA(tpa);
    e->p   = TPA_P(tpa);
    e->key = TAR_KEY(tar);
    e->psm = ~ONES(TAR_PS(tar));
    e->ppn = (tpa & TPA_MASK) & e->psm;
	/* The SDM seems to indicate that at insertion time only bits
	   ppn[11:0] get cleared, and that during lookup the remaining
	   bits are cleared based on the page size.  However, there
	   doesn't seem to be a reason not to clear all the relevant
	   bits at insertion time */
    e->vpn = tva & e->psm;
    e->rid = RR_RID(RR(tva));
#ifdef DBGREGS
    e->dbg = search_IBR(e->vpn, 1 << TAR_PS(tar), 0xF) ||
	     search_DBR(e->vpn, 1 << TAR_PS(tar), MEM_SEMA_ACCESS, 0xF);
#endif
    setQuickChk(e);

    if (num_bpts)
       bptLoad();
}

void uiDtrInsert(unsigned slot, ADDR tva, ADDR tpa, ADDR tar)
{
    tlbInsert(&dtrs[slot], tva, tpa, tar);
}

void uiItrInsert(unsigned slot, ADDR tva, ADDR tpa, ADDR tar)
{
    tlbInsert(&itrs[slot], tva, tpa, tar);
}

BOOL unimplPageSize(unsigned ps)
{
    return ps < 12;
}

static BOOL unimplKeyBits(unsigned key)
{
    return (key >> key_len) != 0;
}

static BOOL reservedMemAttr(unsigned ma)
{
    return ma != WB && ma != WC && ma != UC && ma != UCE && ma != NATPAGE;
}

static BOOL reservedInsert(ADDR tpa, ADDR tar)
{
    if (BitfX(tar,62,2) || unimplPageSize(TAR_PS(tar)))
	return YES;
    if (TPA_P(tpa) &&
	(BitfX(tpa,12,2) || BitfX(tpa,62,1) || BitfX(tar,0,32) ||
	 reservedMemAttr(TPA_MA(tpa)) ||
	 unimplKeyBits(TAR_KEY(tar)) ||
	 unimplPhysAddr((ADDR)BitfX(tpa,14,38) << 12)))
	return YES;
    return NO;
}

static TlbEntry *searchDTLB(ADDR va)
{
    unsigned i, rid = RR_RID(RR(va));
    static TlbEntry *cache1 = 0;
    static TlbEntry *cache2 = 0;
    TlbEntry *e, *prev;

    if(cache1 != NULL && VAmatch(cache1, va, rid))
	return cache1;

    if(cache2 != NULL && VAmatch(cache2, va, rid)) {
	TlbEntry *tmp = cache1;
	cache1 = cache2;
	cache2 = tmp;
	return cache1;
    }

    e = prev = dtcs_head;
    while (e) {
	if (VAmatch(e, va, rid)) {
	    if (e != dtcs_head) {
		prev->next = e->next;
		e->next = dtcs_head;
		dtcs_head = e;
	    }
	    cache2 = cache1;
	    return cache1 = e;
	}
	prev = e;
	e = e->next;
    }

    for (i = 0; i < NDTRS; i++)
	if (VAmatch(&dtrs[i], va, rid)) {
	    cache2 = cache1;
	    return cache1 = &dtrs[i];
	}

    return NULL;
}

static TlbEntry *searchITLB(ADDR va)
{
    unsigned i, rid = RR_RID(RR(va));
    static TlbEntry *cache1 = 0;
    static TlbEntry *cache2 = 0;
    TlbEntry *e, *prev;

    if(cache1 != NULL && VAmatch(cache1, va, rid))
	return cache1;

    if(cache2 != NULL && VAmatch(cache2, va, rid)) {
	TlbEntry *tmp = cache1;
	cache1 = cache2;
	cache2 = tmp;
	return cache1;
    }

    e = prev = itcs_head;
    while (e) {
	if (VAmatch(e, va, rid)) {
	    if (e != itcs_head) {
		prev->next = e->next;
		e->next = itcs_head;
		itcs_head = e;
	    }
	    cache2 = cache1;
	    return cache1 = e;
	}
	prev = e;
	e = e->next;
    }

    for (i = 0; i < NITRS; i++)
	if (VAmatch(&itrs[i], va, rid)) {
	    cache2 = cache1;
	    return cache1 = &itrs[i];
	}

    return NULL;
}

#ifdef DBGREGS
static BOOL search_DBR(ADDR adr, unsigned size, Accesstype atype, unsigned plm)
{
    unsigned slot;
    ADDR amask;

    for (slot = 0; slot < NDBRS; slot += 2) {
	if (!DBR_R(slot + 1) && !DBR_W(slot + 1) ||
	    !(DBR_PLM(slot + 1) & plm))
	    continue;
	amask = (~0ULL << 56) | DBR_MASK(slot + 1);
	if (adr & (size - 1))
	    amask &= ~(ADDR)(2 * size - 1);	/* impl-dependent behavior */
	else
	    amask &= ~(ADDR)(size - 1);

	if ((DBR_R(slot + 1) && (atype & READ_ACCESS) ||
	     DBR_W(slot + 1) && (atype & WRITE_ACCESS)) &&
	    (DbrRd(slot) & amask) == (adr & amask))
	    return YES;
    }
    return NO;
}

static BOOL search_IBR(ADDR adr, unsigned size, unsigned plm)
{
    unsigned slot;
    ADDR amask;

    for (slot = 0; slot < NIBRS; slot += 2) {
	if (!IBR_X(slot + 1) || !(IBR_PLM(slot + 1) & plm))
	    continue;
	amask = ((~0ULL << 56) | IBR_MASK(slot + 1)) & ~(ADDR)(size - 1);
	if ((IbrRd(slot) & amask) == (adr & amask))
	    return YES;
    }
    return NO;
}
#endif

/*---------------------------------------------------------------------------
 * Looks up the passed virtual address in the data TLB and VHPT.  If found,
 *   returns a pointer to the TLB entry; otherwise, faults accordingly and
 *   returns NULL.
 *---------------------------------------------------------------------------*/
static TlbEntry *dataLookup(ADDR va, Accesstype atype)
{
    unsigned rr;
    TlbEntry *ep;
    ADDR vhptaddr, tpa, tar, tag;
    static TlbEntry e;

    rr = RR(va);
    if (PSR_IC) {
	ITIR = RR_RID(rr) << 8 | RR_PS(rr) << 2;
	IFA = va;
    }

    if ((ep = searchDTLB(va)))
	return ep;

    if (unimplVirtAddr(va)) {
	if (atype & SPEC_ACCESS) {
	    e.vpn = 2;	/* XXX - replace constant w/ #define */
	    return &e;
	}
	reservedRegFieldFault(atype);
	return NULL;
    }

    if (!(PTA_VE && RR_VE(rr) && PSR_DT)) {
	if ((atype & SPEC_ACCESS) && (!PSR_IC || (ITLB_ED && DCR_DM))) {
	    e.vpn = 1;	/* XXX - replace constant w/ #define */
	    return &e;
	}
	alternateDataTlbFault(atype);
	return NULL;
    }

    /* look in VHPT */

    vhptaddr = thash(va);
    if (PSR_IC)
	IHA = vhptaddr;

    if (!unimplVirtAddr(vhptaddr)) {
	if (!vhptRd(vhptaddr, &tpa, &tar, &tag)) {
	    if ((atype & SPEC_ACCESS) && (!PSR_IC || (ITLB_ED && DCR_DM))) {
		e.vpn = 1;	/* XXX - replace constant w/ #define */
		return &e;
	    }
	    if (PSR_IC) {
		rr = RR(vhptaddr);
		ITIR = RR_RID(rr) << 8 | RR_PS(rr) << 2;
	    }
	    vhptDataFault(atype);
	    return NULL;
	}
#ifdef DBGREGS
	if (!PSR_DB ||
	    !search_DBR(vhptaddr, PTA_VF ? 32 : 8, READ_ACCESS, 1 << 0)) {
#endif

	    if (!PTA_VF)	/* short format */
		tar = RR_RID(rr) << 8 | RR_PS(rr) << 2;

	    if ((tag == ttag(va) || !PTA_VF) && !reservedInsert(tpa,tar))
		return tcInsert(dtcs, NDTCS, dtrs, NDTRS, tpa, tar, va);
#ifdef DBGREGS
	}
#endif
    }

    if ((atype & SPEC_ACCESS) && (!PSR_IC || (ITLB_ED && DCR_DM))) {
	e.vpn = 1;	/* XXX - replace constant w/ #define */
	return &e;
    }
    dataTlbFault(atype);
    return NULL;
}

/*---------------------------------------------------------------------------
 * dataMLookup - Looks up the passed virtual address in the data TLB.
 *  If found, returns a pointer to the TLB entry; otherwise, returns NULL.
 *---------------------------------------------------------------------------*/
static TlbEntry *dataMLookup(ADDR va, Accesstype atype)
{
    TlbEntry *ep;
    unsigned rr;
    ADDR vhptaddr, tpa, tar, tag;
    static TlbEntry e;

    if ((ep = searchDTLB(va)))
	return ep;

    if (atype == VHPT_RD_ACCESS)
	return NULL;

    if (unimplVirtAddr(va))
	return NULL;

    rr = RR(va);
    if (!(PTA_VE && RR_VE(rr)))
	return NULL;

    /* look in VHPT */

    vhptaddr = thash(va);
    if (unimplVirtAddr(vhptaddr) || !vhptMRd(vhptaddr, &tpa, &tar, &tag))
	return NULL;
    if (!PTA_VF)	/* short format */
	tar = RR_RID(rr) << 8 | RR_PS(rr) << 2;

    if ((tag == ttag(va) || !PTA_VF) && !reservedInsert(tpa,tar)) {
	tlbInsert(&e, 0, tpa, tar);
	return &e;
    }
    return NULL;
}

/*---------------------------------------------------------------------------
 * Looks up the passed virtual address in the instruction TLB.  If found,
 *   returns a pointer to the TLB entry; otherwise, faults accordingly.
 *---------------------------------------------------------------------------*/
static TlbEntry *instLookup(ADDR va)
{
    unsigned rr;
    TlbEntry *ep;
    ADDR vhptaddr, tpa, tar, tag;

    rr = RR(va);
    if (PSR_IC) {
	ITIR = RR_RID(rr) << 8 | RR_PS(rr) << 2;
	IFA = PSR_IS ? ip : ip & ~(REG)0xF;
    }

    if ((ep = searchITLB(va)))
	return ep;

    if (!(PTA_VE && RR_VE(rr) && PSR_DT && PSR_IC)) {
	alternateInstTlbFault();
	return NULL;
    }

    /* look in VHPT */

    IHA = vhptaddr = thash(va);

    if (!unimplVirtAddr(vhptaddr)) {
	if (!vhptRd(vhptaddr, &tpa, &tar, &tag)) {
	    rr = RR(vhptaddr);
	    ITIR = RR_RID(rr) << 8 | RR_PS(rr) << 2;
	    vhptInstFault();
	    return NULL;
	}
#ifdef DBGREGS
	if (!PSR_DB ||
	    !search_DBR(vhptaddr, PTA_VF ? 32 : 8, READ_ACCESS, 1 << 0)) {
#endif

	    if (!PTA_VF)	/* short format */
		tar = RR_RID(rr) << 8 | RR_PS(rr) << 2;

	    if ((tag == ttag(va) || !PTA_VF) && !reservedInsert(tpa,tar))
		return tcInsert(itcs, NITCS, itrs, NITRS, tpa, tar, va);
#ifdef DBGREGS
	}
#endif
    }

    instTlbFault();
    return NULL;
}

/*---------------------------------------------------------------------------
 * instMLookup - Looks up the passed virtual address in the instruction TLB.
 *  If found, returns a pointer to the TLB entry; otherwise, returns NULL.
 *---------------------------------------------------------------------------*/
static TlbEntry *instMLookup(ADDR va)
{
    TlbEntry *ep;
    unsigned rr;
    ADDR vhptaddr, tpa, tar, tag;
    static TlbEntry e;

    if ((ep = searchITLB(va)))
	return ep;

    rr = RR(va);
    if (!(PTA_VE && RR_VE(rr)))
	return NULL;

    /* look in VHPT */

    vhptaddr = thash(va);
    if (unimplVirtAddr(vhptaddr) || !vhptMRd(vhptaddr, &tpa, &tar, &tag))
	return NULL;
    if (!PTA_VF)	/* short format */
	tar = RR_RID(rr) << 8 | RR_PS(rr) << 2;

    if ((tag == ttag(va) || !PTA_VF) && !reservedInsert(tpa,tar)) {
	tlbInsert(&e, 0, tpa, tar);
	return &e;
    }
    return NULL;
}

/* return value:
    0 failed access, no fault
    1 successful access
   -1 failed access, fault
 */
#define OLDWAY
static int keyCheck(Accesstype atype, unsigned key)
{
    int i;
    REG keyval = (key << 8) | 1;

#ifdef OLDWAY
    for (i = 0; i < NPKRS; i++) {
	REG pkr = PkrRd(i);

	if ((pkr & pkrKeyMask) == keyval) {
	    switch (atype & (READ_ACCESS | WRITE_ACCESS)) {
		case EXECUTE_ACCESS:
		    if (pkr & 8) {
			instKeyPermissionFault();
			return -1;
		    }
		    break;
		case READ_ACCESS:
		    if (pkr & 4) {
			if (((atype & SPEC_ACCESS) &&
				(!PSR_IC || (ITLB_ED && DCR_DX))) ||
			    (atype == PROBER_ACCESS) || (atype & VHPT_ACCESS))
			    return 0;
			dataKeyPermissionFault(atype);
			return -1;
		    }
		    break;
		case WRITE_ACCESS:
		    if (pkr & 2) {
			if (atype == PROBEW_ACCESS)
			    return 0;
			dataKeyPermissionFault(atype);
			return -1;
		    }
		    break;
		case READ_ACCESS | WRITE_ACCESS:
		    if (pkr & 6) {
			dataKeyPermissionFault(atype);
			return -1;
		    }
		    break;
	    }
	    return 1;
	}
    }
    if (((atype & SPEC_ACCESS) && (!PSR_IC || (ITLB_ED && DCR_DK))) ||
	(atype & VHPT_ACCESS))
	return 0;
    if (atype == EXECUTE_ACCESS)
	instKeyMissFault();
    else
	dataKeyMissFault(atype);
    return -1;
#else
    switch (atype & (READ_ACCESS | WRITE_ACCESS)) {
	case EXECUTE_ACCESS:
	    for (i = 0; i < NPKRS; i++) {
		REG pkr = PkrRd(i);

		if ((pkr & pkrKeyMask) == keyval) {
		    if (pkr & 8) {
			instKeyPermissionFault();
			return -1;
		    }
		    return 1;
		}
	    }
	    instKeyMissFault();
	    return -1;
	case READ_ACCESS:
	    for (i = 0; i < NPKRS; i++) {
		REG pkr = PkrRd(i);

		if ((pkr & pkrKeyMask) == keyval) {
		    if (pkr & 4) {
			if ((atype & SPEC_ACCESS) &&
				(!PSR_IC || ITLB_ED && DCR_DX) ||
			    (atype == PROBER_ACCESS) || (atype & VHPT_ACCESS))
			    return 0;
			dataKeyPermissionFault(atype);
			return -1;
		    }
		    return 1;
		}
	    }
	    if ((atype & SPEC_ACCESS) && (!PSR_IC || ITLB_ED && DCR_DK) ||
		(atype & VHPT_ACCESS))
		return 0;
	    dataKeyMissFault(atype);
	    return -1;
	case WRITE_ACCESS:
	    for (i = 0; i < NPKRS; i++) {
		REG pkr = PkrRd(i);

		if ((pkr & pkrKeyMask) == keyval) {
		    if (pkr & 2) {
			if (atype == PROBEW_ACCESS)
			    return 0;
			dataKeyPermissionFault(atype);
			return -1;
		    }
		    return 1;
		}
	    }
	    dataKeyMissFault(atype);
	    return -1;
	case READ_ACCESS | WRITE_ACCESS:
	    for (i = 0; i < NPKRS; i++) {
		REG pkr = PkrRd(i);

		if ((pkr & pkrKeyMask) == keyval) {
		    if (pkr & 6) {
			dataKeyPermissionFault(atype);
			return -1;
		    }
		    return 1;
		}
	    }
	    dataKeyMissFault(atype);
	    return -1;
    }
#endif
}

static BOOL accessRights(unsigned ar, unsigned pl, unsigned cpl,
			 Accesstype atype)
{
    atype &= READ_ACCESS | WRITE_ACCESS;

    switch (ar) {
	case 0:
	    if (atype != READ_ACCESS || cpl > pl)
		return NO;
	    break;
	case 1:
	    if (atype & WRITE_ACCESS || cpl > pl)
		return NO;
	    break;
	case 2:
	    if (atype == EXECUTE_ACCESS || cpl > pl)
		return NO;
	    break;
	case 3:
	    if (cpl > pl)
		return NO;
	    break;
	case 4:
	    if (atype == EXECUTE_ACCESS || cpl > pl)
		return NO;
	    if ((atype & WRITE_ACCESS) && cpl && cpl == pl)
		return NO;
	    break;
	case 5:
	    if (cpl > pl)
		return NO;
	    if ((atype & WRITE_ACCESS) && cpl)
		return NO;
	    break;
	case 6:
	    if (cpl > pl)
		return NO;
	    if (atype == EXECUTE_ACCESS && (!cpl || cpl < pl))
		return NO;
	    break;
	case 7:
	    if (atype & WRITE_ACCESS)
		return NO;
	    if (atype == READ_ACCESS && cpl)
		return NO;
	    break;
    }
    return YES;
}

/***********************/
/* TLB Lookup routines */
/***********************/

/*---------------------------------------------------------------------------
 * dataTLBLookup - Translate the passed virtual address into a real address
 *  by looking in the data TLB.  Checks for presence of page and checks all
 *  privileges and traps accordingly if anything is wrong.
 *---------------------------------------------------------------------------*/
/* return 0 for success, 1 for defer, -1 for fault */
int dtlbLookup(ADDR va, unsigned size, Accesstype atype, unsigned pl,
	       BOOL virtual, ADDR *pa)
{
    TlbEntry *e;

    if (!virtual) {
	if (PSR_IC)
	    IFA = va;
	*pa = BitfX(va,1,63);
	if (unimplPhysAddr(va)) {
	    reservedRegFieldFault(atype);
	    return -1;
	}
#ifdef DBGREGS
	if (PSR_DB && !PSR_DD && atype != FC_ACCESS &&
	    search_DBR(va, size, atype, 1<<pl)) {
	    dataDebugFault(atype);
	    return -1;
	}
#endif
	if (va & (size - 1)) {
	    unalignedDataFault(atype);
	    return -1;
	}
	if ((atype == MEM_SEMA_ACCESS || atype == MEM_FETCHADD_ACCESS)
	      && SIGN(va)) {
	    unsupportedDataReferenceFault();
	    return -1;
	}
	if (atype & ADV_ACCESS && SIGN(va))
	    return 1;
	return 0;
    }

    if (!(e = dataLookup(va, atype)))
	return -1;

if (e->quickchk & (1 << (4*(atype & 3)+pl))) {
    if (atype == TPA_ACCESS) {
	*pa = physAddr(e, va);
	return 0;
    }

    if (atype != FC_ACCESS && PSR_PK && keyCheck(atype, e->key) != 1)
	return -1;

    if (atype == FC_ACCESS) {
	*pa = physAddr(e, va);
	return 0;
    }

    if (va & (size - 1)) {
	unalignedDataFault(atype);
	return -1;
    }

    if ((atype == MEM_SEMA_ACCESS &&
	    (e->ma == UC || e->ma == UCE || e->ma == WC)) ||
	(atype == MEM_FETCHADD_ACCESS &&		/* exported fetchadd behavior */
	    (e->ma == UC || e->ma == WC))) {
	unsupportedDataReferenceFault();
	return -1;
    }

    if (atype & ADV_ACCESS && (e->ma == UC || e->ma == UCE))
	return 1;

    *pa = physAddr(e, va);
    return 0;
}

    if (!e->p) {
	dataPageNotPresentFault(atype);
	return -1;
    }

    if (e->ma == NATPAGE) {
	dataNatPageConsumptionFault(atype);
	return -1;
    }

    if (atype == TPA_ACCESS) {
	*pa = physAddr(e, va);
	return 0;
    }

    if (atype != FC_ACCESS && PSR_PK && keyCheck(atype, e->key) != 1)
	return -1;

    if (!accessRights(e->ar, e->pl, pl, atype)) {
	dataAccessRightsFault(atype);
	return -1;
    }

    if (atype == FC_ACCESS) {
	*pa = physAddr(e, va);
	return 0;
    }

    if (!PSR_DA) {	/* Check dirty/access bits */
	if ((atype & WRITE_ACCESS) && !e->d) {
	    dataDirtyBitFault(atype);
	    return -1;
	}
	if (!e->a) {
	    dataAccessBitFault(atype);
	    return -1;
	}
    }

#ifdef DBGREGS
    if (PSR_DB && !PSR_DD && search_DBR(va, size, atype, 1 << pl)) {
	dataDebugFault(atype);
	return -1;
    }
#endif

    if (va & (size - 1)) {
	unalignedDataFault(atype);
	return -1;
    }

    if ((atype == MEM_SEMA_ACCESS &&
	    (e->ma == UC || e->ma == UCE || e->ma == WC)) ||
	(atype == MEM_FETCHADD_ACCESS &&		/* exported fetchadd behavior */
	    (e->ma == UC || e->ma == WC))) {
	unsupportedDataReferenceFault();
	return -1;
    }

    if (atype & ADV_ACCESS && (e->ma == UC || e->ma == UCE))
	return 1;

    *pa = physAddr(e, va);
    return 0;
}

/* return 0 for success, 1 for NaT, -1 for fault */
int specLookup(ADDR va, unsigned size, Accesstype atype, ADDR *pa)
{
    TlbEntry *e;
    unsigned pl = PSR_CPL;
    int defer = 0;
    BOOL virtual = PSR_DT;

    if (!virtual) {
	if (PSR_IC)
	    IFA = va;
	*pa = BitfX(va,1,63);
	if (unimplPhysAddr(va)) {
	    reservedRegFieldFault(atype);
	    return -1;
	}
#ifdef DBGREGS
	if (PSR_DB && !PSR_DD && search_DBR(va, size, atype, 1<<pl))
	    if (!PSR_IC || ITLB_ED && DCR_DD)
		return 1;
	    else {
		dataDebugFault(atype);
		return -1;
	    }
#endif
	if (va & (size - 1)) {
	    if (!PSR_IC || ITLB_ED)
		return 1;
	    else {
		unalignedDataFault(atype);
		return -1;
	    }
    }
	return 1;
    }

    /* Unimpl address faults preclude all faults below.  Data TLB Miss faults
       (Alternate Data TLB, VHPT Data, and Data TLB faults) preclude all
       faults below except for Data Debug faults */

    if (!(e = dataLookup(va, atype)))
	return -1;

    if (e->vpn == 1)		/* Data TLB Miss faults */
	defer = 1;
    else if (e->vpn == 2)	/* Unimpl address faults */
	return 1;

    else if (!e->p) {		/* Data Page Not Present faults preclude all
				   faults below except for Data Debug faults */
	if (!PSR_IC || (ITLB_ED && DCR_DP))
	    defer = 1;
	else {
	    dataPageNotPresentFault(atype);
	    return -1;
	}
    } else {
	if (PSR_PK) {
	    int ret = keyCheck(atype, e->key);

	    if (!ret)
		defer = 1;	/* Data Key Miss and Data Key Permission faults
				   do NOT preclude any of the faults below */
	    else if (ret == -1)
		return -1;
	}

	if (!accessRights(e->ar, e->pl, pl, atype)) {
	    if (!PSR_IC || (ITLB_ED && DCR_DR))
		defer = 1;	/* Data Access Rights faults do NOT preclude
				   any of the faults below */
	    else {
		dataAccessRightsFault(atype);
		return -1;
	    }
	}

	if (!PSR_DA && !e->a) {
	    if (!PSR_IC || (ITLB_ED && DCR_DA))
		defer = 1;	/* Data Access Bit faults do NOT preclude any
				   of the faults below */
	    else {
		dataAccessBitFault(atype);
		return -1;
	    }
	}
    }

    *pa = physAddr(e, va);
#ifdef DBGREGS
    if (PSR_DB && !PSR_DD && search_DBR(va, size, atype, 1 << pl)) {
        if (!PSR_IC || (ITLB_ED && DCR_DD))
	    return 1;
	else {
	    dataDebugFault(atype);
	    return -1;
	}
    }
#endif
    if (va & (size - 1)) {
	if (!PSR_IC || ITLB_ED)
	    return 1;
	else {
	    unalignedDataFault(atype);
	    return -1;
	}
    }

    if (e->ma == NATPAGE || e->ma == UC || e->ma == UCE)
	return 1;

    return defer;
}

/* return 0 for failure, 1 for success, -1 for fault */
int probeLookup(ADDR va, Accesstype atype, unsigned pl)
{
    TlbEntry *e;
    int ret;

    if (unixABI)
	return 1;

    if (unimplVirtAddr(va))
	return 0;

    if (!(e = dataLookup(va, atype)))
	return -1;

    if (!e->p) {
	dataPageNotPresentFault(atype);
	return -1;
    }

    if (e->ma == NATPAGE) {
	dataNatPageConsumptionFault(atype);
	return -1;
    }

    if (PSR_PK && (ret = keyCheck(atype, e->key)) != 1)
	return ret;

    if (!accessRights(e->ar, e->pl, pl, atype))
	return 0;

    return 1;
}

REG takLookup(ADDR va)
{
    TlbEntry *e;

    if ((e = dataLookup(va, TAK_ACCESS)) && e->p)
	return (REG)e->key << 8;
    return 1;
}

/* returns 1 for success, 0 for miss, -1 for failed search */
int vhptLookup(ADDR va, ADDR *pa)
{
    TlbEntry *e;

    if (!(e = searchDTLB(va)))
	return 0;

    if (!e->p)
	return -1;

    if (e->ma == NATPAGE)
	return -1;

    if (PSR_PK && keyCheck(VHPT_RD_ACCESS, e->key) != 1)
	return -1;

    if (!e->a)
	return -1;

    if (e->ma == UC || e->ma == UCE || e->ma == WC)
	return -1;

    *pa = physAddr(e, va);
    return 1;
}

/* Look up the DTLB without triggering any side-effects.  Returns 1 if
   a valid translation was found and -1 otherwise.  */
int dbptLookup(ADDR va, ADDR *pa)
{
    TlbEntry *e;

    if (!(e = searchDTLB(va)))
	return -1;

    if (!e->p)
	return -1;

    if (e->ma == NATPAGE)
	return -1;

    *pa = physAddr(e, va);
    return 1;
}

/*---------------------------------------------------------------------------
 * instTLBLookup - Translate the passed virtual address into a real address
 *  by looking in the instruction TLB.  Checks for presence of page and checks
 *  all privileges and traps accordingly if anything is wrong.
 *---------------------------------------------------------------------------*/
BOOL itlbLookup(ADDR va, BOOL virtual, ADDR *pa)
{
    TlbEntry *e;

    itlbInfo = 0;
    if (!virtual) {
	if (PSR_IC)
	    IFA = va;
	*pa = BitfX(va,1,63);
    } else {
	if (!(e = instLookup(va)))
	    return NO;

	if (!e->p) {
	    instPageNotPresentFault();
	    return NO;
	}

	if (e->ma == NATPAGE) {
	    instNatPageConsumptionFault();
	    return NO;
	}

	if (PSR_PK && keyCheck(EXECUTE_ACCESS, e->key) != 1)
	    return NO;

	if (!accessRights(e->ar, e->pl, PSR_CPL, EXECUTE_ACCESS)) {
	    instAccessRightsFault();
	    return NO;
	}

	if (!PSR_IA && !e->a) {
	    instAccessBitFault();
	    return NO;
	}

	*pa = physAddr(e, va);
	itlbInfo = e->ed << 7 | e->ar << 4 | e->pl << 2;
    }

#ifdef DBGREGS
    if (PSR_DB && !PSR_ID && search_IBR(va, 16, 1 << PSR_CPL)) {
	instDebugFault();
	return NO;
    }
#endif

    return YES;
}

/*---------------------------------------------------------------------------
 * dataTLBMLookup - Translate the passed virtual address into a real address
 *  by looking in the data TLB.  Checks for presence of page and checks all
 *  privileges and returns NO if anything is wrong.
 *---------------------------------------------------------------------------*/
BOOL dtlbMLookup(ADDR va, int at, unsigned pl, BOOL virtual, ADDR *pa)
{
    Accesstype atype;
    TlbEntry *e;

    if (!virtual) {
	*pa = BitfX(va,1,63);
	return YES;
    }

    switch (at) {
	case 0:
	    atype = MEM_LD_ACCESS;
	    break;
	case 1:
	    atype = VHPT_RD_ACCESS;
	    break;
	case 2:
	    atype = MEM_ST_ACCESS;
	    break;
    }
    if (!(e = dataMLookup(va, atype)))
	return NO;

    if (!e->p)
	return NO;

    if (e->ma == NATPAGE)
	return NO;

#if 0
/* XXX - these can currently cause a fault! */
    if (PSR_PK && keyCheck(atype, e->key) != 1)
	return NO;

    if (!accessRights(e->ar, e->pl, pl, atype))
	return NO;
#endif

    if (!PSR_DA) {	/* Check dirty/access bits */
	if ((atype & WRITE_ACCESS) && !e->d)
	    return NO;
	if (!e->a)
	    return NO;
    }

    *pa = physAddr(e, va);
    return YES;
}


/*---------------------------------------------------------------------------
 * instTLBMLookup - Translate the passed virtual address into a real address
 *  by looking in the instruction TLB.  Checks for presence of page and checks
 *  all privileges and returns NO if anything is wrong.
 *---------------------------------------------------------------------------*/
BOOL itlbMLookup(ADDR va, BOOL virtual, ADDR *pa)
{
    TlbEntry *e;

    if (!virtual) {
	*pa = BitfX(va,1,63);
	return YES;
    }

    if (!(e = instMLookup(va)))
	return NO;

    if (!e->p)
	return NO;

    if (e->ma == NATPAGE)
	return NO;

#if 0
/* XXX - these can currently cause a fault! */
    if (PSR_PK && keyCheck(EXECUTE_ACCESS, e->key) != 1)
	return NO;

    if (!accessRights(e->ar, e->pl, PSR_CPL, EXECUTE_ACCESS))
	return NO;
#endif

    if (!PSR_IA && !e->a)
	return NO;

    *pa = physAddr(e, va);
    return YES;
}


/*********************************/
/* TLB Insert and Purge Routines */
/*********************************/

static BOOL trInsert(TlbEntry tr[], unsigned trcnt, unsigned slot,
		     TlbEntry tc[], unsigned tccnt, ADDR tpa)
{
    ADDR rangeMask = (~ONES(BitfX(ITIR,56,6)) & ~VRN_MASK) | 1;
    ADDR vam = IFA & rangeMask;
    unsigned i, rid = RR_RID(RR(IFA));

    /* check for reserved fields and unimpl address bits */
    if (reservedInsert(tpa,ITIR) || unimplVirtAddr(IFA)) {
	reservedRegFieldFault(0);
	return NO;
    }

    /* machine check if overlapping TR entries exist */
    for (i = 0; i < trcnt; i++)
	if (PurgeMatch(&tr[i], vam, rangeMask, rid))
	    progStop("Machine check at IP = %016llx.  Overlapping TR entries\n",
		     ip);

    /* invalidate matching TC entries */
    for (i = 0; i < tccnt; i++)
	if (PurgeMatch(&tc[i], vam, rangeMask, rid)) {
	    tc[i].psm = 0;
	    tc[i].vpn = 1;
	}

    /* Fill in TR entry */
    tlbInsert(&tr[slot], IFA, tpa, ITIR);
    return YES;
}

static void trPurge(ADDR va, unsigned range, TlbEntry tr[], unsigned cnt)
{
    unsigned ps = BitfX32(range,24,6);
    ADDR rangeMask = (~ONES(ps) & ~VRN_MASK) | 1;
    ADDR vam = va & rangeMask;
    unsigned i, rid = RR_RID(RR(va));

    if (unimplPageSize(ps))
	progStop("Machine check at IP = %016llx.  Unimplemented page size"
		 " (%d)\n", ip, ps);
    for (i = 0; i < cnt; i++)
	if (PurgeMatch(&tr[i], vam, rangeMask, rid)) {
	    tr[i].psm = 0;
	    tr[i].vpn = 1;
	}

    /* Depending on the implementation, purge overlapping TC entries, too */
    /* tcPurge(va, range); */
}

static TlbEntry *tcInsert(TlbEntry tc[], unsigned tccnt,
			  TlbEntry tr[], unsigned trcnt,
			  ADDR tpa, ADDR tar, ADDR tva)
{
    ADDR rangeMask = (~ONES(BitfX(tar,56,6)) & ~VRN_MASK) | 1;
    ADDR vam = tva & rangeMask;
    unsigned i, ind, first_when, rid = RR_RID(RR(tva));
    static unsigned when = 0;
    BOOL found = NO;
    TlbEntry *tce;

    /* machine check if overlapping TR entries exist */
    for (i = 0; i < trcnt; i++)
	if (PurgeMatch(&tr[i], vam, rangeMask, rid))
	    progStop("Machine check at IP = %016llx.  Overlapping TR entries\n",
		     ip);

    /* Use existing matching entry if found, invalidate other matching entries*/
    for (i = 0; i < tccnt; i++)
        if (PurgeMatch(&tc[i], vam, rangeMask, rid)) {
	    if (!found) {
		tlbInsert(&tc[i], tva, tpa, tar);
		tc[i].when = when++;
		tce = &tc[i];
		found = YES;
	    } else {
		tc[i].psm = 0;
		tc[i].vpn = 1;
	    }
        }
    if (found)
	return tce;

#if 0
    /* Use existing invalid entry if found */
    for (i = 0; i < tccnt; i++)
	if (tc[i].vpn == 1) {
	    tlbInsert(&tc[i], tva, tpa, tar);
	    tc[i].when = when++;
	    return &tc[i];
	}

    /* Use fifo */
    first_when = tc[0].when;
    ind = 0;
    for (i = 1; i < tccnt; i++)
	if (tc[i].when < first_when) {
	    first_when = tc[i].when;
	    ind = i;
	}
    tlbInsert(&tc[ind], tva, tpa, tar);
    tc[ind].when = when++;
    return &tc[ind];
#else
    /* Use LRU */
    {
	TlbEntry **head, *prev, *e;

	head = (tc == dtcs) ? &dtcs_head : &itcs_head;
	e = *head;
	while (e->next) {
	    if (e->vpn == 1) {
		/* found an existing invalid enty */
		break;
	    }
	    prev = e;
	    e = e->next;
	}
	if (e != *head) {
	    prev->next = e->next;
	    e->next = *head;
	    *head = e;
	}
	tlbInsert(e, tva, tpa, tar);
	return e;
    }
#endif
}

BOOL dtrInsert(unsigned slot, ADDR tpa)
{
    return trInsert(dtrs, NDTRS, slot, dtcs, NDTCS, tpa);
}

BOOL itrInsert(unsigned slot, ADDR tpa)
{
    return trInsert(itrs, NITRS, slot, itcs, NITCS, tpa);
}

void dtrPurge(ADDR va, unsigned range)
{
    trPurge(va, range, dtrs, NDTRS);
}

void itrPurge(ADDR va, unsigned range)
{
    trPurge(va, range, itrs, NITRS);
}

BOOL dtcInsert(unsigned when, ADDR tpa)
{
    if (reservedInsert(tpa,ITIR) || unimplVirtAddr(IFA)) {
	reservedRegFieldFault(0);
	return NO;
    }
    (void)tcInsert(dtcs, NDTCS, dtrs, NDTRS, tpa, ITIR, IFA);
    return YES;
}

BOOL itcInsert(unsigned when, ADDR tpa)
{
    if (reservedInsert(tpa,ITIR) || unimplVirtAddr(IFA)) {
	reservedRegFieldFault(0);
	return NO;
    }
    (void)tcInsert(itcs, NITCS, itrs, NITRS, tpa, ITIR, IFA);
    return YES;
}

void tcPurge(ADDR va, unsigned range)
{
    unsigned ps = BitfX32(range,24,6), rid = RR_RID(RR(va));
    ADDR rangeMask = (~ONES(ps) & ~VRN_MASK) | 1;
    ADDR vam = va & rangeMask;
    int i;

    if (unimplPageSize(ps))
	progStop("Machine check at IP = %016llx.  Unimplemented page size"
		 " (%d)\n", ip, ps);
    for (i = 0; i < NITRS; i++)
	if (PurgeMatch(&itrs[i], vam, rangeMask, rid))
	    progStop("Machine check at IP = %016llx.  Overlapping TR entries\n",
		     ip);

    for (i = 0; i < NDTRS; i++)
	if (PurgeMatch(&dtrs[i], vam, rangeMask, rid))
	    progStop("Machine check at IP = %016llx.  Overlapping TR entries\n",
		     ip);

    for (i = 0; i < NITCS; i++)
	if (PurgeMatch(&itcs[i], vam, rangeMask, rid)) {
	    itcs[i].psm = 0;
	    itcs[i].vpn = 1;
	}
    for (i = 0; i < NDTCS; i++)
	if (PurgeMatch(&dtcs[i], vam, rangeMask, rid)) {
	    dtcs[i].psm = 0;
	    dtcs[i].vpn = 1;
	}
}

void tcEntryPurge(ADDR va)
{
    init(itcs, NITCS);
    init(dtcs, NDTCS);
}

/* hash_long is implementation-specific.  This is the oldest implementation */
static ADDR hash_long(ADDR hpn, ADDR rid) {
    return hpn ^ rid;
}

ADDR thash(ADDR va)
{
    ADDR mask, hpn, offset, addr;
    unsigned rr	= RR(va);

    mask = (1ULL << PTA_SIZE) - 1;
    mask = BitfX(mask,3,46);
    hpn  = BitfX(va,BE_VA_MSB,va_len) >> RR_PS(rr);
    if (PTA_VF) {
	offset	= hash_long(hpn, RR_RID(rr)) << 5;
	addr	= (ADDR)PTA_VRN << 61;
    } else {
	offset	= hpn << 3;
	addr	= (ADDR)rr << 61;
    }
    addr |= ((PTA_BASE & ~mask) | (BitfX(offset,3,46) & mask)) << 15
	    | BitfR(offset,49,15);
    return addr;
}

/* ttag is implementation-specific.  This is the oldest implementation */
ADDR ttag(ADDR va)
{
    unsigned rr = RR(va);

    return BitfX(va,BE_VA_MSB,va_len) >> RR_PS(rr) ^ RR_RID(rr) << 39;
}

void epcCheck(BYTE *cpl)
{
    unsigned ar = ITLB_AR;
    unsigned pl = ITLB_PL;

    if (ar == 7 && pl < *cpl)
	*cpl = pl;
}

BOOL vmswCheck(BOOL virtual)
{
    unsigned ar = ITLB_AR;

    return (virtual && ar == 7);
}


/************************/
/* TLB Display routines */
/************************/

#define TLB_HDR \
    "V RID    Virtual Page  Physical Page PgSz ED AR PL D A MA  P KEY\n"
#define TLB_DATA \
    "0 000000 0000000000000 0000000000000  16E 0  0  0  0 0 WB  0 000000\n"

static const char *getPagesize(ADDR psm)
{
    int cnt = 0;
    static char psStr[] = "    ";

    psm = ~psm;
    while (psm) {
	psm >>= 1;
	cnt++;
    }
    (void)sprintf(psStr, "%3d", 1 << (cnt % 10));
    psStr[3] = " KMGTPE"[cnt / 10];
    return psStr;
}

/*
 * tlbEntryLine - Return a string describing the given TLB entry
 */
static char *tlbEntryLine(TlbEntry *e)
{
    static char buf[81];
    static const char maStr[][4] = {
	"WB ", "?1 ", "?2 ", "?3 ",
	"UC ", "UCE", "WC ", "NaT",
    };

    (void)snprintf(buf, sizeof (buf),
		  "%d %06x %013llx %013llx %s %-2d %-2d %d  %d %d %3s %d %06x",
		  (int)~e->vpn & 1,
		  e->rid, e->vpn >> 12, e->ppn >> 12,
		  getPagesize(e->psm),
		  e->ed, e->ar, e->pl, e->d, e->a, maStr[e->ma], e->p, e->key);
    return buf;
}

size_t instTlbDisplaySize(void)
{
    return unixABI ? 0 : sizeof TLB_DATA * (NITRS+NITCS) + 1;
}

size_t dataTlbDisplaySize(void)
{
    return unixABI ? 0 : sizeof TLB_DATA * (NDTRS+NDTCS) + 1;
}

void getInstTlbInfo(char * hdr, size_t hdr_sz, char * buf, size_t buf_sz)
{
    int i;
    char *p = buf;
    size_t p_left = buf_sz;
    size_t w;

    (void)snprintf(hdr, hdr_sz, "%s", TLB_HDR);
    for (i = 0; i < NITRS; i++)
    {
	w = snprintf(p, p_left, "%s\n", tlbEntryLine(&itrs[i]));
	if (w >= p_left) return; /* Output truncated */
	p_left -= w;
    }

    w = snprintf(p, p_left, "\n");
    if (w >= p_left) return; /* Output truncated */
    p_left -= w;

    for (i = 0; i < NITCS; i++)
    {
	w = snprintf(p, p_left, "%s\n", tlbEntryLine(&itcs[i]));
	if (w >= p_left) return; /* Output truncated */
	p_left -= w;
    }
}

void getDataTlbInfo(char * hdr, size_t hdr_sz, char * buf, size_t buf_sz)
{
    int i;
    char *p = buf;
    size_t p_left = buf_sz;
    size_t w;

    (void)snprintf(hdr, hdr_sz, "%s", TLB_HDR);
    for (i = 0; i < NDTRS; i++)
    {
	w = snprintf(p, p_left, "%s\n", tlbEntryLine(&dtrs[i]));
	if (w >= p_left) return; /* Output truncated */
	p_left -= w;
    }

    w = snprintf(p, p_left, "\n");
    if (w >= p_left) return; /* Output truncated */
    p_left -= w;

    for (i = 0; i < NDTCS; i++)
    {
	w = snprintf(p, p_left, "%s\n", tlbEntryLine(&dtcs[i]));
	if (w >= p_left) return; /* Output truncated */
	p_left -= w;
    }
}
