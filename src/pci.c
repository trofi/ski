#include "pci.h"

#include "bits.h"
#include "sim.h"

WORD pciBar[6] = {0xFFFFE001, 0xFFFFE000, 0xFFFFE000};

static HWORD pciCmd;
static BYTE pciCacheLineSz, pciLatTmr;

REG pciConfigRd(REG pci_address, REG size)
{
    BYTE segno, busno, devno, fnno, regno;

    segno = BitfR(pci_address,32,8);
    busno = BitfR(pci_address,40,8);
    devno = BitfR(pci_address,48,5);
    fnno  = BitfR(pci_address,53,3);
    regno = BitfR(pci_address,56,8);
    if (pci_address == 0x7800)	/* Azusa platform */
	return size == 2 ? 0x1000 : 0x000c1000;
    if (pci_address == 0x7804 && size == 2)	/* command */
	return pciCmd;
    if (pci_address == 0x7804 && size == 4)	/* command/status */
	return pciCmd;
    if (pci_address == 0x7806 && size == 2)	/* status */
	return 0x0000;
    if (pci_address == 0x7808 && size == 4)	/* revision ID and class code */
	return 0x0100000A;
    if (pci_address == 0x780A && size == 2)	/* class */
	return 0x0100;	/* SCSI bus controller */
    if (pci_address == 0x780C && size == 2)	/* cache line size & latency timer */
	return (HWORD)pciLatTmr << 8 | pciCacheLineSz;
    if (pci_address == 0x780E && size == 1)	/* header type */
	return 0x00;
    if (pci_address >= 0x7810 && pci_address <= 0x7824 && size == 4)	/* BARs */
	return pciBar[(pci_address-0x7810)>>2];
    if (pci_address == 0x7830 && size == 4)	/* Expansion ROM */
	return 0x00000000;
    if (pci_address == 0x783D && size == 1)	/* Interrupt Pin */
	return 0x01;
    if (!segno && !busno && !fnno && !regno && size == 4)
	return 0xFFFFFFFF;
    progStop("bad FW call\n");
    return 0;
}

void pciConfigWr(REG pci_address, REG size, REG value)
{
    BYTE segno, busno, devno, fnno, regno;

    segno = BitfR(pci_address,32,8);
    busno = BitfR(pci_address,40,8);
    devno = BitfR(pci_address,48,5);
    fnno  = BitfR(pci_address,53,3);
    regno = BitfR(pci_address,56,8);
    if (pci_address == 0x7804 && size == 2) {	/* command */
	pciCmd = value;
	return;
    }
/* XXX - this case seems to only be currently used in a situation that
	 probably should never occur, so perhaps move this to the end?
 */
    if (pci_address == 0x780C && size == 1) {	/* cache line size */
	pciCacheLineSz = BitfR(value,56,8);
	return;
    }
    if (pci_address == 0x780C && size == 2) {	/* cache line size & latency timer */
	pciCacheLineSz = BitfR(value,56,8);
	pciLatTmr = BitfR(value,48,8);
	return;
    }
    if (pci_address == 0x7810 && size == 4) {	/* I/O BAR */
	pciBar[(pci_address-0x7810)>>2] = (value & ~0x1FFFULL) | 1;
	return;
    }
    if (pci_address >= 0x7814 && pci_address <= 0x7818 && size == 4) { /* Memory BARs */
	pciBar[(pci_address-0x7810)>>2] = value & ~0x1FFFULL;
	return;
    }
    if (pci_address >= 0x781C && pci_address <= 0x7824 && size == 4)	/* unimpl. BARs */
	return;
    if (pci_address == 0x7830 && size == 4)	/* Expansion ROM */
	return;
    progStop("bad FW call\n");
}
