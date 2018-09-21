/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator System Execution Routines
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
#include "types.h"
#include "fields.h"
#include "state.h"
#include "sim.h"
#include "simmem.h"

Status bsw0Ex(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (X_PSR_BN(SRC1))
	switchBanks();
    DST1 = SRC1;
    X_PSR_BN(DST1) = 0;
    return 0;
}

Status bsw1Ex(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (!X_PSR_BN(SRC1))
	switchBanks();
    DST1 = SRC1;
    X_PSR_BN(DST1) = 1;
    return 0;
}

Status epcEx(INSTINFO *info)
{
    /* SRC1 has PFS.ppl, SRC2 has PSR.it */
    if (SRC1 < CPL) {
	illegalOpFault();
	return StFault;
    }
    if (SRC2)
	epcCheck(&CPL);
    else
	CPL = 0;
    return 0;
}

Status fcEx(INSTINFO *info)
{
    if (SRCNAT1) {
	regNatConsumptionFault(FC_ACCESS);
	return StFault;
    }
    if (!unixABI &&
	dtlbLookup(SRC1, 1, FC_ACCESS, PSR_CPL, PSR_DT, &SRC1) == -1)
	return StFault;
    FM_mem_cache_update(SRC1, info);
    if (!unixABI)
	/* flush SRC1 */;
    clearPdecode(SRC1 & ~(ADDR)0x1F, 16);
    clearPdecode((SRC1 & ~(ADDR)0x1F)+16, 16);
    return 0;
}

Status itcdEx(INSTINFO *info)
{
    if (PSR_IC) {
	illegalOpFault();
	return StFault;
    }
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (!dtcInsert(0, SRC1))
	return StFault;
    return 0;
}

Status itciEx(INSTINFO *info)
{
    if (PSR_IC) {
	illegalOpFault();
	return StFault;
    }
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (!itcInsert(0, SRC1))
	return StFault;
    return 0;
}

Status itrdEx(INSTINFO *info)
{
    if (PSR_IC) {
	illegalOpFault();
	return StFault;
    }
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (B7(SRC2) >= NDTRS) {
	reservedRegFieldFault(0);
	return StFault;
    }
    if (!dtrInsert(B7(SRC2), SRC1))
	return StFault;
    return 0;
}

Status itriEx(INSTINFO *info)
{
    if (PSR_IC) {
	illegalOpFault();
	return StFault;
    }
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (B7(SRC2) >= NITRS) {
	reservedRegFieldFault(0);
	return StFault;
    }
    if (!itrInsert(B7(SRC2), SRC1))
	return StFault;
    return 0;
}

Status movtopsrlEx(INSTINFO *info)
{
    /* SRC2 is psr */
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (reservedSystemMaskField(SRC1)) {
	reservedRegFieldFault(0);
	return StFault;
    }
    WD0(DST1) = WD0(SRC2);
    WD1(DST1) = WD1(SRC1);
    return 0;
}

Status movfrompsrEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r1)) {	/* remove redundant check in write phase? */
	illegalOpFault();
	return StFault;
    }
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    DST1 = WD1(SRC1);
    X_PSR_MC(DST1) = PSR_MC;
    X_PSR_IT(DST1) = PSR_IT;
    DSTNAT1 = 0;
    return 0;
}

Status movtocrEx(INSTINFO *info)
{
    if (reservedCR(cr3) || readonlyCR(cr3) || (PSR_IC && interruptionCR(cr3))){
	illegalOpFault();
	return StFault;
    }
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (reservedCrField(cr3, &SRC1)) {
	reservedRegFieldFault(0);
	return StFault;
    }
    DST1 = SRC1;
    return 0;
}

Status movfromcrEx(INSTINFO *info)
{
    if (OUT_OF_FRAME(r1) || reservedCR(cr3) || (PSR_IC &&interruptionCR(cr3))){
	    /* remove redundant OUT_OF_FRAME check in write phase? */
	illegalOpFault();
	return StFault;
    }
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    DST1 = SRC1;
    DSTNAT1 = 0;
    if (cr3 == IVR_ID)
	serviceIrpt();
    else if (cr3 == ITIR_ID)
	DST1 &= 0xFFFFFFFCULL;
    return 0;
}

Status movtorrEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (reservedRrField(SRC1)) {
	reservedRegFieldFault(0);
	return StFault;
    }
    DST1 = SRC1;
    return 0;
}

Status movfromrrEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    DST1 = RrRd(BitfX(SRC1,0,3));
    DSTNAT1 = 0;
    return 0;
}

Status movtodbrEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (B7(SRC2) >= NDBRS) {
	reservedRegFieldFault(0);
	return StFault;
    }
    ignoredDbrField(B7(SRC2), &SRC1);
    DST1 = SRC1;
    return 0;
}

Status movfromdbrEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (B7(SRC1) >= NDBRS) {
	reservedRegFieldFault(0);
	return StFault;
    }
    DST1 = DbrRd(B7(SRC1));
    DSTNAT1 = 0;
    return 0;
}

Status movtoibrEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (B7(SRC2) >= NIBRS) {
	reservedRegFieldFault(0);
	return StFault;
    }
    ignoredIbrField(B7(SRC2), &SRC1);
    DST1 = SRC1;
    return 0;
}

Status movfromibrEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (B7(SRC1) >= NIBRS) {
	reservedRegFieldFault(0);
	return StFault;
    }
    DST1 = IbrRd(B7(SRC1));
    DSTNAT1 = 0;
    return 0;
}

Status movtomsrEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(0);
	return StFault;
    }
    progStop("mov to msr not implemented\n");
}

Status movfrommsrEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    progStop("mov from msr not implemented\n");
}

Status movtopkrEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (B7(SRC2) >= NPKRS || reservedPkrField(SRC1)) {
	reservedRegFieldFault(0);
	return StFault;
    }
    if (BitfX(SRC1,63,1)) {
	unsigned int i, key;

	key = BitfX(SRC1,32,24);
	for (i = 0; i < NPKRS; i++) {
	    REG pkr = PkrRd(i);

	    if (BitfX(pkr,32,24) == key && BitfX(pkr,63,1))
		PkrWrt(i, pkr & ~(REG)1);
	}
    }
    DST1 = SRC1;
    return 0;
}

Status movfrompkrEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (B7(SRC1) >= NPKRS) {
	reservedRegFieldFault(0);
	return StFault;
    }
    DST1 = PkrRd(B7(SRC1));
    DSTNAT1 = 0;
    return 0;
}

Status movtopmcEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (B7(SRC2) >= NPMS || reservedPmcField(SRC1)) {
	reservedRegFieldFault(0);
	return StFault;
    }
    DST1 = SRC1;
    return 0;
}

Status movfrompmcEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (B7(SRC1) >= NPMS) {
	reservedRegFieldFault(0);
	return StFault;
    }
    DST1 = PmcRd(B7(SRC1));
    DSTNAT1 = 0;
    return 0;
}

Status movtopmdEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (B7(SRC2) >= NPMS) {
	reservedRegFieldFault(0);
	return StFault;
    }
    DST1 = SRC1;
    return 0;
}

Status movfrompmdEx(INSTINFO *info)
{
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (B7(SRC1) >= NPMS) {
	reservedRegFieldFault(0);
	return StFault;
    }
    if (PMC_PM(PmcRd(B7(SRC1))) && CPL) {
	privRegFault();
	return StFault;
    }
    /* SRC2 has PSR.sp */
    DST1 = SRC2 ? 0 : PmdRd(B7(SRC1));
    DSTNAT1 = 0;
    return 0;
}

Status ptcEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(0);
	return StFault;
    }
#if 0
/* Processor may overpurge or generate a Machine Check */
    if (unimplPageSize(BitfX(SRC1,56,6))) {
	reservedRegFieldFault(0);
	return StFault;
    }
#endif
    if (unimplVirtAddr(SRC2)) {
	reservedRegFieldFault(0);
	return StFault;
    }
    tcPurge(SRC2, SRC1);
    return 0;
}

Status ptceEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1) {
	regNatConsumptionFault(0);
	return StFault;
    }
    tcEntryPurge(SRC1);
    return 0;
}

Status ptrdEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (unimplVirtAddr(SRC2)) {
	reservedRegFieldFault(0);
	return StFault;
    }
    dtrPurge(SRC2, SRC1);
    return 0;
}

Status ptriEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (SRCNAT1 || SRCNAT2) {
	regNatConsumptionFault(0);
	return StFault;
    }
    if (unimplVirtAddr(SRC2)) {
	reservedRegFieldFault(0);
	return StFault;
    }
    itrPurge(SRC2, SRC1);
    return 0;
}

Status rfiEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    DST1 = IPSR;

    if (X_PSR_IS(DST1)) {
	DST2 = WD1(IIP);
	arithFlagsFromEflags();
	setSegGRsFromARs();
	setIAmode();
	setEIPfromIP();
	rse_restore_frame(0, 0, sof);
	sof = soil = sor = rrbg = rrbf = rrbp = 0;
	rse_invalidate_non_current_regs();
	X_PSR_DD(DST1) = X_PSR_DA(DST1) = X_PSR_IA(DST1) = X_PSR_ED(DST1) = NO;
    } else {
	DST2 = (IIP & ~0xFULL) + (X_PSR_RI(DST1) << 2);

	/* Check for return to unimpl address */

	if (X_PSR_BN(SRC1) != X_PSR_BN(DST1))
	    switchBanks();
	if (IFS_V) {
	    int growth = -sof;
	    int old_frame = sof;

#if 0
	    mod_Tof(-(int)sof);
#endif
	    rrbp = IFM_RRBP;
	    rrbf = IFM_RRBF;
	    rrbg = IFM_RRBG;
	    sor  = IFM_SOR << 3;
	    soil = IFM_SOIL;
	    sof  = IFM_SOF;
	    if (bol < sof) {
		bol += N_STACK_PHYS - sof;
	    } else
		bol -= sof;
	    updateGrMap(0);
	    rse_restore_frame(IFM_SOF, growth, old_frame);
	} else
	    rse_restore_frame(0, 0, 0);	/* is this a nop?  Delete if so */
    }
    if (cfle) st |= ST_CHECK;
    return 0;
}

/* the next 5 are implementation-dependent firmware-only instructions */

Status rfixEx(INSTINFO *info)
{
    progStop("rfi.x not implemented\n");
}

Status haltEx(INSTINFO *info)
{
    progStop("halt not implemented\n");
}

Status haltmfEx(INSTINFO *info)
{
    progStop("halt.mf not implemented\n");
}

Status ccEx(INSTINFO *info)
{
    progStop("cc not implemented\n");
}

Status probexFEx(INSTINFO *info)
{
    progStop("probe.x.fault not implemented\n");
}

Status rsmEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (reservedSystemMaskField(imm64)) {
	reservedRegFieldFault(0);
	return StFault;
    }
    DST1 = SRC1 & ~imm64;
    return 0;
}

Status ssmEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (reservedSystemMaskField(imm64)) {
	reservedRegFieldFault(0);
	return StFault;
    }
    DST1 = SRC1 | imm64;
    return 0;
}

Status takEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(TAK_ACCESS);
	return StFault;
    }
    if (SRCNAT1) {
	regNatConsumptionFault(TAK_ACCESS);
	return StFault;
    }
    DST1 = takLookup(SRC1);
    DSTNAT1 = 0;
    return 0;
}

Status thashEx(INSTINFO *info)
{
    DST1 = thash(SRC1);
    DSTNAT1 = SRCNAT1 || unimplVirtAddr(SRC1);
    return 0;
}

Status tpaEx(INSTINFO *info)
{
    if (CPL) {
	privOpFault(TPA_ACCESS);
	return StFault;
    }
    if (SRCNAT1) {
	regNatConsumptionFault(TPA_ACCESS);
	return StFault;
    }
    if (dtlbLookup(SRC1, 1, TPA_ACCESS, 0, YES, &DST1) == -1)
	return StFault;
    DSTNAT1 = 0;
    return 0;
}

Status ttagEx(INSTINFO *info)
{
    DST1 = ttag(SRC1);
    DSTNAT1 = SRCNAT1 || unimplVirtAddr(SRC1);
    return 0;
}

Status vmsw0Ex(INSTINFO *info)
{
    /* SRC1 is PSR */
    if (!implemented_vm()) {
	illegalOpFault();
	return StFault;
    }
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (!vmswCheck(X_PSR_IT(SRC1))) {
	virtualizationFault();
	return StFault;
    }
    DST1 = SRC1;
    X_PSR_VM(DST1) = 0;
    return 0;
}

Status vmsw1Ex(INSTINFO *info)
{
    /* SRC1 is PSR */
    if (!implemented_vm()) {
	illegalOpFault();
	return StFault;
    }
    if (CPL) {
	privOpFault(0);
	return StFault;
    }
    if (!vmswCheck(X_PSR_IT(SRC1))) {
	virtualizationFault();
	return StFault;
    }
    DST1 = SRC1;
    X_PSR_VM(DST1) = 1;
    return 0;
}
