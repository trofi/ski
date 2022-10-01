#include "pci.h"

#include "bits.h"
#include "sim.h"

WORD pciBar[6] = {0xFFFFE001, 0xFFFFE000, 0xFFFFE000};

static HWORD pciCmd;
static BYTE pciCacheLineSz, pciLatTmr;

REG pciConfigRd(REG arg1, REG arg2)
{
    BYTE segno, busno, devno, fnno, regno;

    segno = BitfR(arg1,32,8);
    busno = BitfR(arg1,40,8);
    devno = BitfR(arg1,48,5);
    fnno  = BitfR(arg1,53,3);
    regno = BitfR(arg1,56,8);
    if (arg1 == 0x7800)	/* Azusa platform */
	return arg2 == 2 ? 0x1000 : 0x000c1000;
    if (arg1 == 0x7804 && arg2 == 2)	/* command */
	return pciCmd;
    if (arg1 == 0x7804 && arg2 == 4)	/* command/status */
	return pciCmd;
    if (arg1 == 0x7806 && arg2 == 2)	/* status */
	return 0x0000;
    if (arg1 == 0x7808 && arg2 == 4)	/* revision ID and class code */
	return 0x0100000A;
    if (arg1 == 0x780A && arg2 == 2)	/* class */
	return 0x0100;	/* SCSI bus controller */
    if (arg1 == 0x780C && arg2 == 2)	/* cache line size & latency timer */
	return (HWORD)pciLatTmr << 8 | pciCacheLineSz;
    if (arg1 == 0x780E && arg2 == 1)	/* header type */
	return 0x00;
    if (arg1 >= 0x7810 && arg1 <= 0x7824 && arg2 == 4)	/* BARs */
	return pciBar[(arg1-0x7810)>>2];
    if (arg1 == 0x7830 && arg2 == 4)	/* Expansion ROM */
	return 0x00000000;
    if (arg1 == 0x783D && arg2 == 1)	/* Interrupt Pin */
	return 0x01;
    if (!segno && !busno && !fnno && !regno && arg2 == 4)
	return 0xFFFFFFFF;
    progStop("bad FW call\n");
    return 0;
}

void pciConfigWr(REG arg1, REG arg2, REG arg3)
{
    BYTE segno, busno, devno, fnno, regno;

    segno = BitfR(arg1,32,8);
    busno = BitfR(arg1,40,8);
    devno = BitfR(arg1,48,5);
    fnno  = BitfR(arg1,53,3);
    regno = BitfR(arg1,56,8);
    if (arg1 == 0x7804 && arg2 == 2) {	/* command */
	pciCmd = arg3;
	return;
    }
/* XXX - this case seems to only be currently used in a situation that
	 probably should never occur, so perhaps move this to the end?
 */
    if (arg1 == 0x780C && arg2 == 1) {	/* cache line size */
	pciCacheLineSz = BitfR(arg3,56,8);
	return;
    }
    if (arg1 == 0x780C && arg2 == 2) {	/* cache line size & latency timer */
	pciCacheLineSz = BitfR(arg3,56,8);
	pciLatTmr = BitfR(arg3,48,8);
	return;
    }
    if (arg1 == 0x7810 && arg2 == 4) {	/* I/O BAR */
	pciBar[(arg1-0x7810)>>2] = (arg3 & ~0x1FFFULL) | 1;
	return;
    }
    if (arg1 >= 0x7814 && arg1 <= 0x7818 && arg2 == 4) { /* Memory BARs */
	pciBar[(arg1-0x7810)>>2] = arg3 & ~0x1FFFULL;
	return;
    }
    if (arg1 >= 0x781C && arg1 <= 0x7824 && arg2 == 4)	/* unimpl. BARs */
	return;
    if (arg1 == 0x7830 && arg2 == 4)	/* Expansion ROM */
	return;
    progStop("bad FW call\n");
}
