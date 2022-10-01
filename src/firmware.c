#include "firmware.h"

#include "pci.h"
#include "sim.h"
#include "state.h"
#include "std.h"

static void doEFI_GetTime(REG arg0, REG *ret0)
{
    *ret0 = 0;
    (void)memMWrt(arg0, 8, 0);
    (void)memMWrt(arg0+8, 8, 0);
}

static void doPAL(REG *ret0, REG *ret1, REG *ret2, REG *ret3)
{
    REG arg0, arg1, arg2, arg3;
    BOOL junk;

    GrRd(28, arg0, junk, NO);
    GrRd(29, arg1, junk, NO);
    GrRd(30, arg2, junk, NO);
    GrRd(31, arg3, junk, NO);

    *ret0 = *ret1 = *ret2 = *ret3 = 0;
    if (arg0 == 0x4ULL) { /* PAL_CACHE_SUMMARY */
	*ret1 = 3;	/* CACHE_LEVELS_MIN */
    } else if (arg0 == 0x2ULL) { /* PAL_CACHE_INFO */
	/* XXX - are these cache parameters OK? */
	switch ((int)arg1) {
	    case 0:
		*ret1 = 4<<8;	/* L1I_INST_DAT_CACHE_ASSOC */
		*ret2 = 0x4000;	/* L1I_INST_DAT_CACHE_SZ */
		break;
	    case 1:
		*ret1 = arg2 == 1 ? 0 : 6<<8;	/* L2_UNIF_CACHE_ASSOC */
		*ret2 = arg2 == 1 ? 0 : 0x18000;/* L2_UNIF_CACHE_SZ */
		break;
	    case 2:
		/* L3 Unified Cache w/ line size = 32, assoc = 4 */
		*ret1 = arg2 == 1 ? 0 : 5<<16|4<<8;
		*ret2 = arg2 == 1 ? 0 : 0x200000;/* L3_UNIF_CACHE_SZ */
		break;
	}
    } else if (arg0 == 0x8ULL) { /* PAL_VM_SUMMARY */
	*ret1 = 7ULL<<40 | 7ULL<<32 | 15ULL<<16 | 18ULL<<8;
	*ret2 = 18ULL<<8 | 50;
    } else if (arg0 == 0x9ULL) { /* PAL_BUS_GET_FEATURES */
	*ret1 = 0;	/* avail */
	*ret2 = 0;	/* status */
	*ret3 = 0;	/* control */
    } else if (arg0 == 0xBULL) { /* PAL_DEBUG_INFO */
	*ret1 = 4;
	*ret2 = 4;
    } else if (arg0 == 0xEULL) { /* PAL_FREQ_RATIOS */
	*ret1 = 0x800000002ULL;
	*ret2 = 0x100000001ULL;
	*ret3 = 0x800000002ULL;
    } else if (arg0 == 0xFULL) { /* PAL_PERF_MON_INFO */
	*ret1 = 0ULL<<24 | 0ULL<<16 | 0ULL<<8 | 4;
    } else if (arg0 == 0x11ULL) { /* PAL_PROC_GET_FEATURES */
	*ret1 = 0;	/* avail */
	*ret2 = 0;	/* status */
	*ret3 = 0;	/* control */
    } else if (arg0 == 0x14ULL) { /* PAL_VERSION */
	*ret1 = 0;	/* min_pal_ver */
	// *ret2 = 0xFFULL<<24; /* cur_pal_ver (vendor == INTEL) */
	*ret2 = 0;	/* cur_pal_ver */
    } else if (arg0 == 0x22ULL) { /* PAL_VM_PAGE_SIZE */
	*ret1 = 15557ULL << 12;
    } else
	progStop("bad PAL call\n");
}

REG doFW(REG imm, REG arg0, REG arg1, REG arg2, REG arg3)
{
    REG ret0 = 0, ret1 = 0, ret2 = 0, ret3 = 0;

    if (ip == 0x12FFFFB0ULL)
	doEFI_GetTime(arg0, &ret0);
    else if (arg0 < 0x100ULL)	/* XXX - hack that should detect PAL calls */
	doPAL(&ret0, &ret1, &ret2, &ret3);
    else if (arg0 == 0x1000000ULL)	/* SAL_SET_VECTORS */
	;
    else if (arg0 == 0x1000001ULL)	/* SAL_GET_STATE_INFO */
	ret0 = -5ULL;
    else if (arg0 == 0x1000002ULL)	/* SAL_GET_STATE_INFO_SIZE */
	ret1 = 1;
    else if (arg0 == 0x1000003ULL)	/* SAL_CLEAR_STATE_INFO */
	;
    else if (arg0 == 0x1000005ULL)	/* SAL_MC_SET_PARAMS */
	;
    else if (arg0 == 0x1000010ULL)	/* SAL_PCI_CONFIG_READ */
	ret1 = pciConfigRd(arg1, arg2);
    else if (arg0 == 0x1000011ULL)	/* SAL_PCI_CONFIG_WRITE */
	pciConfigWr(arg1, arg2, arg3);
    else if (arg0 == 0x1000012ULL)	/* SAL_FREQ_BASE */
	ret1 = 10000000ULL/*140000000ULL*/,
	ret2 = 100ULL;
    else
	progStop("bad FW call\n");
    GrWrtx(9,  ret1, 0);
    GrWrtx(10, ret2, 0);
    GrWrtx(11, ret3, 0);
    return ret0;
}
