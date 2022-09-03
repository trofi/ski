/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Multimedia Execution Routines
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

Status mix1lEx(INSTINFO *info)
{
    B0(DST1) = B0(SRC1);
    B1(DST1) = B0(SRC2);
    B2(DST1) = B2(SRC1);
    B3(DST1) = B2(SRC2);
    B4(DST1) = B4(SRC1);
    B5(DST1) = B4(SRC2);
    B6(DST1) = B6(SRC1);
    B7(DST1) = B6(SRC2);
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status mix1rEx(INSTINFO *info)
{
    B0(DST1) = B1(SRC1);
    B1(DST1) = B1(SRC2);
    B2(DST1) = B3(SRC1);
    B3(DST1) = B3(SRC2);
    B4(DST1) = B5(SRC1);
    B5(DST1) = B5(SRC2);
    B6(DST1) = B7(SRC1);
    B7(DST1) = B7(SRC2);
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status mix2lEx(INSTINFO *info)
{
    HW0(DST1) = HW0(SRC1);
    HW1(DST1) = HW0(SRC2);
    HW2(DST1) = HW2(SRC1);
    HW3(DST1) = HW2(SRC2);
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status mix2rEx(INSTINFO *info)
{
    HW0(DST1) = HW1(SRC1);
    HW1(DST1) = HW1(SRC2);
    HW2(DST1) = HW3(SRC1);
    HW3(DST1) = HW3(SRC2);
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status mix4lEx(INSTINFO *info)
{
    WD0(DST1) = WD0(SRC1);
    WD1(DST1) = WD0(SRC2);
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status mix4rEx(INSTINFO *info)
{
    WD0(DST1) = WD1(SRC1);
    WD1(DST1) = WD1(SRC2);
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status mux1Ex(INSTINFO *info)
{
    switch (MHT) {
	case _brcst:
	    B0(DST1) = B7(SRC1);
	    B1(DST1) = B7(SRC1);
	    B2(DST1) = B7(SRC1);
	    B3(DST1) = B7(SRC1);
	    B4(DST1) = B7(SRC1);
	    B5(DST1) = B7(SRC1);
	    B6(DST1) = B7(SRC1);
	    B7(DST1) = B7(SRC1);
	    break;
	case _mix:
	    B0(DST1) = B0(SRC1);
	    B1(DST1) = B4(SRC1);
	    B2(DST1) = B2(SRC1);
	    B3(DST1) = B6(SRC1);
	    B4(DST1) = B1(SRC1);
	    B5(DST1) = B5(SRC1);
	    B6(DST1) = B3(SRC1);
	    B7(DST1) = B7(SRC1);
	    break;
	case _shuf:
	    B0(DST1) = B0(SRC1);
	    B1(DST1) = B4(SRC1);
	    B2(DST1) = B1(SRC1);
	    B3(DST1) = B5(SRC1);
	    B4(DST1) = B2(SRC1);
	    B5(DST1) = B6(SRC1);
	    B6(DST1) = B3(SRC1);
	    B7(DST1) = B7(SRC1);
	    break;
	case _alt:
	    B0(DST1) = B0(SRC1);
	    B1(DST1) = B2(SRC1);
	    B2(DST1) = B4(SRC1);
	    B3(DST1) = B6(SRC1);
	    B4(DST1) = B1(SRC1);
	    B5(DST1) = B3(SRC1);
	    B6(DST1) = B5(SRC1);
	    B7(DST1) = B7(SRC1);
	    break;
	case _rev:
	    B0(DST1) = B7(SRC1);
	    B1(DST1) = B6(SRC1);
	    B2(DST1) = B5(SRC1);
	    B3(DST1) = B4(SRC1);
	    B4(DST1) = B3(SRC1);
	    B5(DST1) = B2(SRC1);
	    B6(DST1) = B1(SRC1);
	    B7(DST1) = B0(SRC1);
	    break;
    }
    DSTNAT1 = SRCNAT1;
    return 0;
}

Status mux2Ex(INSTINFO *info)
{
    HW0(DST1) = HWn(SRC1, BitfX8(MHT,0,2));
    HW1(DST1) = HWn(SRC1, BitfX8(MHT,2,2));
    HW2(DST1) = HWn(SRC1, BitfX8(MHT,4,2));
    HW3(DST1) = HWn(SRC1, BitfX8(MHT,6,2));
    DSTNAT1   = SRCNAT1;
    return 0;
}

Status pack2sssEx(INSTINFO *info)
{
    B0(DST1) = signedSat8((long long)(short)HW0(SRC2));
    B1(DST1) = signedSat8((long long)(short)HW1(SRC2));
    B2(DST1) = signedSat8((long long)(short)HW2(SRC2));
    B3(DST1) = signedSat8((long long)(short)HW3(SRC2));
    B4(DST1) = signedSat8((long long)(short)HW0(SRC1));
    B5(DST1) = signedSat8((long long)(short)HW1(SRC1));
    B6(DST1) = signedSat8((long long)(short)HW2(SRC1));
    B7(DST1) = signedSat8((long long)(short)HW3(SRC1));
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pack2ussEx(INSTINFO *info)
{
    B0(DST1) = unsignedSat8((long long)(short)HW0(SRC2));
    B1(DST1) = unsignedSat8((long long)(short)HW1(SRC2));
    B2(DST1) = unsignedSat8((long long)(short)HW2(SRC2));
    B3(DST1) = unsignedSat8((long long)(short)HW3(SRC2));
    B4(DST1) = unsignedSat8((long long)(short)HW0(SRC1));
    B5(DST1) = unsignedSat8((long long)(short)HW1(SRC1));
    B6(DST1) = unsignedSat8((long long)(short)HW2(SRC1));
    B7(DST1) = unsignedSat8((long long)(short)HW3(SRC1));
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pack4sssEx(INSTINFO *info)
{
    HW0(DST1) = signedSat16((long long)(int)WD0(SRC2));
    HW1(DST1) = signedSat16((long long)(int)WD1(SRC2));
    HW2(DST1) = signedSat16((long long)(int)WD0(SRC1));
    HW3(DST1) = signedSat16((long long)(int)WD1(SRC1));
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status padd1Ex(INSTINFO *info)
{
    B0(DST1) = B0(SRC1) + B0(SRC2);
    B1(DST1) = B1(SRC1) + B1(SRC2);
    B2(DST1) = B2(SRC1) + B2(SRC2);
    B3(DST1) = B3(SRC1) + B3(SRC2);
    B4(DST1) = B4(SRC1) + B4(SRC2);
    B5(DST1) = B5(SRC1) + B5(SRC2);
    B6(DST1) = B6(SRC1) + B6(SRC2);
    B7(DST1) = B7(SRC1) + B7(SRC2);
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status padd1sssEx(INSTINFO *info)
{
    B0(DST1) = signedSat8((long long)(signed char)B0(SRC1) +
			  (long long)(signed char)B0(SRC2));
    B1(DST1) = signedSat8((long long)(signed char)B1(SRC1) +
			  (long long)(signed char)B1(SRC2));
    B2(DST1) = signedSat8((long long)(signed char)B2(SRC1) +
			  (long long)(signed char)B2(SRC2));
    B3(DST1) = signedSat8((long long)(signed char)B3(SRC1) +
			  (long long)(signed char)B3(SRC2));
    B4(DST1) = signedSat8((long long)(signed char)B4(SRC1) +
			  (long long)(signed char)B4(SRC2));
    B5(DST1) = signedSat8((long long)(signed char)B5(SRC1) +
			  (long long)(signed char)B5(SRC2));
    B6(DST1) = signedSat8((long long)(signed char)B6(SRC1) +
			  (long long)(signed char)B6(SRC2));
    B7(DST1) = signedSat8((long long)(signed char)B7(SRC1) +
			  (long long)(signed char)B7(SRC2));
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status padd1uusEx(INSTINFO *info)
{
    B0(DST1) = unsignedSat8(B0(SRC1) + (long long)(signed char)B0(SRC2));
    B1(DST1) = unsignedSat8(B1(SRC1) + (long long)(signed char)B1(SRC2));
    B2(DST1) = unsignedSat8(B2(SRC1) + (long long)(signed char)B2(SRC2));
    B3(DST1) = unsignedSat8(B3(SRC1) + (long long)(signed char)B3(SRC2));
    B4(DST1) = unsignedSat8(B4(SRC1) + (long long)(signed char)B4(SRC2));
    B5(DST1) = unsignedSat8(B5(SRC1) + (long long)(signed char)B5(SRC2));
    B6(DST1) = unsignedSat8(B6(SRC1) + (long long)(signed char)B6(SRC2));
    B7(DST1) = unsignedSat8(B7(SRC1) + (long long)(signed char)B7(SRC2));
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status padd1uuuEx(INSTINFO *info)
{
    B0(DST1) = unsignedSat8(B0(SRC1) + B0(SRC2));
    B1(DST1) = unsignedSat8(B1(SRC1) + B1(SRC2));
    B2(DST1) = unsignedSat8(B2(SRC1) + B2(SRC2));
    B3(DST1) = unsignedSat8(B3(SRC1) + B3(SRC2));
    B4(DST1) = unsignedSat8(B4(SRC1) + B4(SRC2));
    B5(DST1) = unsignedSat8(B5(SRC1) + B5(SRC2));
    B6(DST1) = unsignedSat8(B6(SRC1) + B6(SRC2));
    B7(DST1) = unsignedSat8(B7(SRC1) + B7(SRC2));
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status padd2Ex(INSTINFO *info)
{
    HW0(DST1) = HW0(SRC1) + HW0(SRC2);
    HW1(DST1) = HW1(SRC1) + HW1(SRC2);
    HW2(DST1) = HW2(SRC1) + HW2(SRC2);
    HW3(DST1) = HW3(SRC1) + HW3(SRC2);
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status padd2sssEx(INSTINFO *info)
{
    HW0(DST1) = signedSat16((long long)(short)HW0(SRC1) +
			    (long long)(short)HW0(SRC2));
    HW1(DST1) = signedSat16((long long)(short)HW1(SRC1) +
			    (long long)(short)HW1(SRC2));
    HW2(DST1) = signedSat16((long long)(short)HW2(SRC1) +
			    (long long)(short)HW2(SRC2));
    HW3(DST1) = signedSat16((long long)(short)HW3(SRC1) +
			    (long long)(short)HW3(SRC2));
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status padd2uusEx(INSTINFO *info)
{
    HW0(DST1) = unsignedSat16(HW0(SRC1) + (long long)(short)HW0(SRC2));
    HW1(DST1) = unsignedSat16(HW1(SRC1) + (long long)(short)HW1(SRC2));
    HW2(DST1) = unsignedSat16(HW2(SRC1) + (long long)(short)HW2(SRC2));
    HW3(DST1) = unsignedSat16(HW3(SRC1) + (long long)(short)HW3(SRC2));
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status padd2uuuEx(INSTINFO *info)
{
    HW0(DST1) = unsignedSat16(HW0(SRC1) + HW0(SRC2));
    HW1(DST1) = unsignedSat16(HW1(SRC1) + HW1(SRC2));
    HW2(DST1) = unsignedSat16(HW2(SRC1) + HW2(SRC2));
    HW3(DST1) = unsignedSat16(HW3(SRC1) + HW3(SRC2));
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status padd4Ex(INSTINFO *info)
{
    WD0(DST1) = WD0(SRC1) + WD0(SRC2);
    WD1(DST1) = WD1(SRC1) + WD1(SRC2);
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pavg1Ex(INSTINFO *info)
{
    B0(DST1) = AVG(B0(SRC1) + B0(SRC2));
    B1(DST1) = AVG(B1(SRC1) + B1(SRC2));
    B2(DST1) = AVG(B2(SRC1) + B2(SRC2));
    B3(DST1) = AVG(B3(SRC1) + B3(SRC2));
    B4(DST1) = AVG(B4(SRC1) + B4(SRC2));
    B5(DST1) = AVG(B5(SRC1) + B5(SRC2));
    B6(DST1) = AVG(B6(SRC1) + B6(SRC2));
    B7(DST1) = AVG(B7(SRC1) + B7(SRC2));
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pavg2Ex(INSTINFO *info)
{
    HW0(DST1) = AVG(HW0(SRC1) + HW0(SRC2));
    HW1(DST1) = AVG(HW1(SRC1) + HW1(SRC2));
    HW2(DST1) = AVG(HW2(SRC1) + HW2(SRC2));
    HW3(DST1) = AVG(HW3(SRC1) + HW3(SRC2));
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pavg1razEx(INSTINFO *info)
{
    B0(DST1) = AVGRAZ(B0(SRC1) + B0(SRC2));
    B1(DST1) = AVGRAZ(B1(SRC1) + B1(SRC2));
    B2(DST1) = AVGRAZ(B2(SRC1) + B2(SRC2));
    B3(DST1) = AVGRAZ(B3(SRC1) + B3(SRC2));
    B4(DST1) = AVGRAZ(B4(SRC1) + B4(SRC2));
    B5(DST1) = AVGRAZ(B5(SRC1) + B5(SRC2));
    B6(DST1) = AVGRAZ(B6(SRC1) + B6(SRC2));
    B7(DST1) = AVGRAZ(B7(SRC1) + B7(SRC2));
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pavg2razEx(INSTINFO *info)
{
    HW0(DST1) = AVGRAZ(HW0(SRC1) + HW0(SRC2));
    HW1(DST1) = AVGRAZ(HW1(SRC1) + HW1(SRC2));
    HW2(DST1) = AVGRAZ(HW2(SRC1) + HW2(SRC2));
    HW3(DST1) = AVGRAZ(HW3(SRC1) + HW3(SRC2));
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pavgsub1Ex(INSTINFO *info)
{
    B0(DST1) = AVG(B0(SRC1) - B0(SRC2));
    B1(DST1) = AVG(B1(SRC1) - B1(SRC2));
    B2(DST1) = AVG(B2(SRC1) - B2(SRC2));
    B3(DST1) = AVG(B3(SRC1) - B3(SRC2));
    B4(DST1) = AVG(B4(SRC1) - B4(SRC2));
    B5(DST1) = AVG(B5(SRC1) - B5(SRC2));
    B6(DST1) = AVG(B6(SRC1) - B6(SRC2));
    B7(DST1) = AVG(B7(SRC1) - B7(SRC2));
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pavgsub2Ex(INSTINFO *info)
{
    HW0(DST1) = AVG(HW0(SRC1) - HW0(SRC2));
    HW1(DST1) = AVG(HW1(SRC1) - HW1(SRC2));
    HW2(DST1) = AVG(HW2(SRC1) - HW2(SRC2));
    HW3(DST1) = AVG(HW3(SRC1) - HW3(SRC2));
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pcmp1eqEx(INSTINFO *info)
{
    B0(DST1) = B0(SRC1) == B0(SRC2) ? 0xFF : 0;
    B1(DST1) = B1(SRC1) == B1(SRC2) ? 0xFF : 0;
    B2(DST1) = B2(SRC1) == B2(SRC2) ? 0xFF : 0;
    B3(DST1) = B3(SRC1) == B3(SRC2) ? 0xFF : 0;
    B4(DST1) = B4(SRC1) == B4(SRC2) ? 0xFF : 0;
    B5(DST1) = B5(SRC1) == B5(SRC2) ? 0xFF : 0;
    B6(DST1) = B6(SRC1) == B6(SRC2) ? 0xFF : 0;
    B7(DST1) = B7(SRC1) == B7(SRC2) ? 0xFF : 0;
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pcmp1gtEx(INSTINFO *info)
{
    B0(DST1) = (signed char)B0(SRC1) > (signed char)B0(SRC2) ? 0xFF : 0;
    B1(DST1) = (signed char)B1(SRC1) > (signed char)B1(SRC2) ? 0xFF : 0;
    B2(DST1) = (signed char)B2(SRC1) > (signed char)B2(SRC2) ? 0xFF : 0;
    B3(DST1) = (signed char)B3(SRC1) > (signed char)B3(SRC2) ? 0xFF : 0;
    B4(DST1) = (signed char)B4(SRC1) > (signed char)B4(SRC2) ? 0xFF : 0;
    B5(DST1) = (signed char)B5(SRC1) > (signed char)B5(SRC2) ? 0xFF : 0;
    B6(DST1) = (signed char)B6(SRC1) > (signed char)B6(SRC2) ? 0xFF : 0;
    B7(DST1) = (signed char)B7(SRC1) > (signed char)B7(SRC2) ? 0xFF : 0;
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pcmp2eqEx(INSTINFO *info)
{
    HW0(DST1) = HW0(SRC1) == HW0(SRC2) ? 0xFFFF : 0;
    HW1(DST1) = HW1(SRC1) == HW1(SRC2) ? 0xFFFF : 0;
    HW2(DST1) = HW2(SRC1) == HW2(SRC2) ? 0xFFFF : 0;
    HW3(DST1) = HW3(SRC1) == HW3(SRC2) ? 0xFFFF : 0;
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pcmp2gtEx(INSTINFO *info)
{
    HW0(DST1) = (short)HW0(SRC1) > (short)HW0(SRC2) ? 0xFFFF : 0;
    HW1(DST1) = (short)HW1(SRC1) > (short)HW1(SRC2) ? 0xFFFF : 0;
    HW2(DST1) = (short)HW2(SRC1) > (short)HW2(SRC2) ? 0xFFFF : 0;
    HW3(DST1) = (short)HW3(SRC1) > (short)HW3(SRC2) ? 0xFFFF : 0;
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pcmp4eqEx(INSTINFO *info)
{
    WD0(DST1) = WD0(SRC1) == WD0(SRC2) ? 0xFFFFFFFF : 0;
    WD1(DST1) = WD1(SRC1) == WD1(SRC2) ? 0xFFFFFFFF : 0;
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pcmp4gtEx(INSTINFO *info)
{
    WD0(DST1) = (int)WD0(SRC1) > (int)WD0(SRC2) ? 0xFFFFFFFF : 0;
    WD1(DST1) = (int)WD1(SRC1) > (int)WD1(SRC2) ? 0xFFFFFFFF : 0;
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pmax1uEx(INSTINFO *info)
{
    B0(DST1) = MAX(B0(SRC1), B0(SRC2));
    B1(DST1) = MAX(B1(SRC1), B1(SRC2));
    B2(DST1) = MAX(B2(SRC1), B2(SRC2));
    B3(DST1) = MAX(B3(SRC1), B3(SRC2));
    B4(DST1) = MAX(B4(SRC1), B4(SRC2));
    B5(DST1) = MAX(B5(SRC1), B5(SRC2));
    B6(DST1) = MAX(B6(SRC1), B6(SRC2));
    B7(DST1) = MAX(B7(SRC1), B7(SRC2));
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pmax2Ex(INSTINFO *info)
{
    HW0(DST1) = MAX((short)HW0(SRC1), (short)HW0(SRC2));
    HW1(DST1) = MAX((short)HW1(SRC1), (short)HW1(SRC2));
    HW2(DST1) = MAX((short)HW2(SRC1), (short)HW2(SRC2));
    HW3(DST1) = MAX((short)HW3(SRC1), (short)HW3(SRC2));
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pmin1uEx(INSTINFO *info)
{
    B0(DST1) = MIN(B0(SRC1), B0(SRC2));
    B1(DST1) = MIN(B1(SRC1), B1(SRC2));
    B2(DST1) = MIN(B2(SRC1), B2(SRC2));
    B3(DST1) = MIN(B3(SRC1), B3(SRC2));
    B4(DST1) = MIN(B4(SRC1), B4(SRC2));
    B5(DST1) = MIN(B5(SRC1), B5(SRC2));
    B6(DST1) = MIN(B6(SRC1), B6(SRC2));
    B7(DST1) = MIN(B7(SRC1), B7(SRC2));
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pmin2Ex(INSTINFO *info)
{
    HW0(DST1) = MIN((short)HW0(SRC1), (short)HW0(SRC2));
    HW1(DST1) = MIN((short)HW1(SRC1), (short)HW1(SRC2));
    HW2(DST1) = MIN((short)HW2(SRC1), (short)HW2(SRC2));
    HW3(DST1) = MIN((short)HW3(SRC1), (short)HW3(SRC2));
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pmpy2lEx(INSTINFO *info)
{
    WD0(DST1) = (short)HW0(SRC1) * (short)HW0(SRC2);
    WD1(DST1) = (short)HW2(SRC1) * (short)HW2(SRC2);
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pmpy2rEx(INSTINFO *info)
{
    WD0(DST1) = (short)HW1(SRC1) * (short)HW1(SRC2);
    WD1(DST1) = (short)HW3(SRC1) * (short)HW3(SRC2);
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pmpyshr2Ex(INSTINFO *info)
{
    HW0(DST1) = ((short)HW0(SRC1) * (short)HW0(SRC2)) >> CNT;
    HW1(DST1) = ((short)HW1(SRC1) * (short)HW1(SRC2)) >> CNT;
    HW2(DST1) = ((short)HW2(SRC1) * (short)HW2(SRC2)) >> CNT;
    HW3(DST1) = ((short)HW3(SRC1) * (short)HW3(SRC2)) >> CNT;
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pmpyshr2uEx(INSTINFO *info)
{
    HW0(DST1) = (HW0(SRC1) * HW0(SRC2)) >> CNT;
    HW1(DST1) = (HW1(SRC1) * HW1(SRC2)) >> CNT;
    HW2(DST1) = (HW2(SRC1) * HW2(SRC2)) >> CNT;
    HW3(DST1) = (HW3(SRC1) * HW3(SRC2)) >> CNT;
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status popcntEx(INSTINFO *info)
{
    DST1 = numOnes(SRC1);
    DSTNAT1 = SRCNAT1;
    return 0;
}

Status psad1Ex(INSTINFO *info)
{
    DST1    = SIM_ABS((long long)(B0(SRC1) - B0(SRC2)));
    DST1   += SIM_ABS((long long)(B1(SRC1) - B1(SRC2)));
    DST1   += SIM_ABS((long long)(B2(SRC1) - B2(SRC2)));
    DST1   += SIM_ABS((long long)(B3(SRC1) - B3(SRC2)));
    DST1   += SIM_ABS((long long)(B4(SRC1) - B4(SRC2)));
    DST1   += SIM_ABS((long long)(B5(SRC1) - B5(SRC2)));
    DST1   += SIM_ABS((long long)(B6(SRC1) - B6(SRC2)));
    DST1   += SIM_ABS((long long)(B7(SRC1) - B7(SRC2)));
    DSTNAT1 = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pshl2Ex(INSTINFO *info)
{
    if (CNT >= 16)
	DST1 = 0;
    else {
	HW0(DST1) = HW0(SRC1) << CNT;
	HW1(DST1) = HW1(SRC1) << CNT;
	HW2(DST1) = HW2(SRC1) << CNT;
	HW3(DST1) = HW3(SRC1) << CNT;
    }
    DSTNAT1 = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pshl4Ex(INSTINFO *info)
{
    if (CNT >= 32)
	DST1 = 0;
    else {
	WD0(DST1) = WD0(SRC1) << CNT;
	WD1(DST1) = WD1(SRC1) << CNT;
    }
    DSTNAT1 = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pshladd2Ex(INSTINFO *info)
{
    HW0(DST1) = shaddSat16((long long)(short)HW0(SRC1),
			   (long long)(short)HW0(SRC2),CNT);
    HW1(DST1) = shaddSat16((long long)(short)HW1(SRC1),
			   (long long)(short)HW1(SRC2),CNT);
    HW2(DST1) = shaddSat16((long long)(short)HW2(SRC1),
			   (long long)(short)HW2(SRC2),CNT);
    HW3(DST1) = shaddSat16((long long)(short)HW3(SRC1),
			   (long long)(short)HW3(SRC2),CNT);
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pshr2Ex(INSTINFO *info)
{
    if (CNT >= 16) {
	HW0(DST1) = (long long)(short)HW0(SRC1) >> 16;
	HW1(DST1) = (long long)(short)HW1(SRC1) >> 16;
	HW2(DST1) = (long long)(short)HW2(SRC1) >> 16;
	HW3(DST1) = (long long)(short)HW3(SRC1) >> 16;
    } else {
	HW0(DST1) = (short)HW0(SRC1) >> CNT;
	HW1(DST1) = (short)HW1(SRC1) >> CNT;
	HW2(DST1) = (short)HW2(SRC1) >> CNT;
	HW3(DST1) = (short)HW3(SRC1) >> CNT;
    }
    DSTNAT1 = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pshr2uEx(INSTINFO *info)
{
    if (CNT >= 16)
	DST1 = 0;
    else {
	HW0(DST1) = HW0(SRC1) >> CNT;
	HW1(DST1) = HW1(SRC1) >> CNT;
	HW2(DST1) = HW2(SRC1) >> CNT;
	HW3(DST1) = HW3(SRC1) >> CNT;
    }
    DSTNAT1 = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pshr4Ex(INSTINFO *info)
{
    if (CNT >= 32) {
	WD0(DST1) = (long long)(int)WD0(SRC1) >> 32;
	WD1(DST1) = (long long)(int)WD1(SRC1) >> 32;
    } else {
	WD0(DST1) = (int)WD0(SRC1) >> CNT;
	WD1(DST1) = (int)WD1(SRC1) >> CNT;
    }
    DSTNAT1 = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pshr4uEx(INSTINFO *info)
{
    if (CNT >= 32)
	DST1 = 0;
    else {
	WD0(DST1) = WD0(SRC1) >> CNT;
	WD1(DST1) = WD1(SRC1) >> CNT;
    }
    DSTNAT1 = SRCNAT1 | SRCNAT2;
    return 0;
}

Status pshradd2Ex(INSTINFO *info)
{
    HW0(DST1) = signedSat16(((long long)(short)HW0(SRC1) >> CNT) +
			    (short)HW0(SRC2));
    HW1(DST1) = signedSat16(((long long)(short)HW1(SRC1) >> CNT) +
			    (short)HW1(SRC2));
    HW2(DST1) = signedSat16(((long long)(short)HW2(SRC1) >> CNT) +
			    (short)HW2(SRC2));
    HW3(DST1) = signedSat16(((long long)(short)HW3(SRC1) >> CNT) +
			    (short)HW3(SRC2));
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status psub1Ex(INSTINFO *info)
{
    B0(DST1) = B0(SRC1) - B0(SRC2);
    B1(DST1) = B1(SRC1) - B1(SRC2);
    B2(DST1) = B2(SRC1) - B2(SRC2);
    B3(DST1) = B3(SRC1) - B3(SRC2);
    B4(DST1) = B4(SRC1) - B4(SRC2);
    B5(DST1) = B5(SRC1) - B5(SRC2);
    B6(DST1) = B6(SRC1) - B6(SRC2);
    B7(DST1) = B7(SRC1) - B7(SRC2);
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status psub1sssEx(INSTINFO *info)
{
    B0(DST1) = signedSat8((long long)(signed char)B0(SRC1) -
			  (long long)(signed char)B0(SRC2));
    B1(DST1) = signedSat8((long long)(signed char)B1(SRC1) -
			  (long long)(signed char)B1(SRC2));
    B2(DST1) = signedSat8((long long)(signed char)B2(SRC1) -
			  (long long)(signed char)B2(SRC2));
    B3(DST1) = signedSat8((long long)(signed char)B3(SRC1) -
			  (long long)(signed char)B3(SRC2));
    B4(DST1) = signedSat8((long long)(signed char)B4(SRC1) -
			  (long long)(signed char)B4(SRC2));
    B5(DST1) = signedSat8((long long)(signed char)B5(SRC1) -
			  (long long)(signed char)B5(SRC2));
    B6(DST1) = signedSat8((long long)(signed char)B6(SRC1) -
			  (long long)(signed char)B6(SRC2));
    B7(DST1) = signedSat8((long long)(signed char)B7(SRC1) -
			  (long long)(signed char)B7(SRC2));
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status psub1uusEx(INSTINFO *info)
{
    B0(DST1) = unsignedSat8(B0(SRC1) - (long long)(signed char)B0(SRC2));
    B1(DST1) = unsignedSat8(B1(SRC1) - (long long)(signed char)B1(SRC2));
    B2(DST1) = unsignedSat8(B2(SRC1) - (long long)(signed char)B2(SRC2));
    B3(DST1) = unsignedSat8(B3(SRC1) - (long long)(signed char)B3(SRC2));
    B4(DST1) = unsignedSat8(B4(SRC1) - (long long)(signed char)B4(SRC2));
    B5(DST1) = unsignedSat8(B5(SRC1) - (long long)(signed char)B5(SRC2));
    B6(DST1) = unsignedSat8(B6(SRC1) - (long long)(signed char)B6(SRC2));
    B7(DST1) = unsignedSat8(B7(SRC1) - (long long)(signed char)B7(SRC2));
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status psub1uuuEx(INSTINFO *info)
{
    B0(DST1) = unsignedSat8(B0(SRC1) - B0(SRC2));
    B1(DST1) = unsignedSat8(B1(SRC1) - B1(SRC2));
    B2(DST1) = unsignedSat8(B2(SRC1) - B2(SRC2));
    B3(DST1) = unsignedSat8(B3(SRC1) - B3(SRC2));
    B4(DST1) = unsignedSat8(B4(SRC1) - B4(SRC2));
    B5(DST1) = unsignedSat8(B5(SRC1) - B5(SRC2));
    B6(DST1) = unsignedSat8(B6(SRC1) - B6(SRC2));
    B7(DST1) = unsignedSat8(B7(SRC1) - B7(SRC2));
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status psub2Ex(INSTINFO *info)
{
    HW0(DST1) = HW0(SRC1) - HW0(SRC2);
    HW1(DST1) = HW1(SRC1) - HW1(SRC2);
    HW2(DST1) = HW2(SRC1) - HW2(SRC2);
    HW3(DST1) = HW3(SRC1) - HW3(SRC2);
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status psub2sssEx(INSTINFO *info)
{
    HW0(DST1) = signedSat16((long long)(short)HW0(SRC1) -
			    (long long)(short)HW0(SRC2));
    HW1(DST1) = signedSat16((long long)(short)HW1(SRC1) -
			    (long long)(short)HW1(SRC2));
    HW2(DST1) = signedSat16((long long)(short)HW2(SRC1) -
			    (long long)(short)HW2(SRC2));
    HW3(DST1) = signedSat16((long long)(short)HW3(SRC1) -
			    (long long)(short)HW3(SRC2));
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status psub2uusEx(INSTINFO *info)
{
    HW0(DST1) = unsignedSat16(HW0(SRC1) - (long long)(short)HW0(SRC2));
    HW1(DST1) = unsignedSat16(HW1(SRC1) - (long long)(short)HW1(SRC2));
    HW2(DST1) = unsignedSat16(HW2(SRC1) - (long long)(short)HW2(SRC2));
    HW3(DST1) = unsignedSat16(HW3(SRC1) - (long long)(short)HW3(SRC2));
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status psub2uuuEx(INSTINFO *info)
{
    HW0(DST1) = unsignedSat16(HW0(SRC1) - HW0(SRC2));
    HW1(DST1) = unsignedSat16(HW1(SRC1) - HW1(SRC2));
    HW2(DST1) = unsignedSat16(HW2(SRC1) - HW2(SRC2));
    HW3(DST1) = unsignedSat16(HW3(SRC1) - HW3(SRC2));
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status psub4Ex(INSTINFO *info)
{
    WD0(DST1) = WD0(SRC1) - WD0(SRC2);
    WD1(DST1) = WD1(SRC1) - WD1(SRC2);
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status shlEx(INSTINFO *info)
{
    if (CNT >= 64)
	DST1 = 0;
    else
	DST1 = SRC1 << CNT;
    DSTNAT1 = SRCNAT1 | SRCNAT2;
    return 0;
}

Status shrEx(INSTINFO *info)
{
    if (CNT >= 64)
	DST1 = (long long)SRC1 >> 63;
    else
	DST1 = (long long)SRC1 >> CNT;
    DSTNAT1 = SRCNAT1 | SRCNAT2;
    return 0;
}

Status shruEx(INSTINFO *info)
{
    if (CNT >= 64)
	DST1 = 0;
    else
	DST1 = SRC1 >> CNT;
    DSTNAT1 = SRCNAT1 | SRCNAT2;
    return 0;
}

Status unpack1hEx(INSTINFO *info)
{
    B0(DST1) = B0(SRC1);
    B1(DST1) = B0(SRC2);
    B2(DST1) = B1(SRC1);
    B3(DST1) = B1(SRC2);
    B4(DST1) = B2(SRC1);
    B5(DST1) = B2(SRC2);
    B6(DST1) = B3(SRC1);
    B7(DST1) = B3(SRC2);
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status unpack1lEx(INSTINFO *info)
{
    B0(DST1) = B4(SRC1);
    B1(DST1) = B4(SRC2);
    B2(DST1) = B5(SRC1);
    B3(DST1) = B5(SRC2);
    B4(DST1) = B6(SRC1);
    B5(DST1) = B6(SRC2);
    B6(DST1) = B7(SRC1);
    B7(DST1) = B7(SRC2);
    DSTNAT1  = SRCNAT1 | SRCNAT2;
    return 0;
}

Status unpack2hEx(INSTINFO *info)
{
    HW0(DST1) = HW0(SRC1);
    HW1(DST1) = HW0(SRC2);
    HW2(DST1) = HW1(SRC1);
    HW3(DST1) = HW1(SRC2);
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}

Status unpack2lEx(INSTINFO *info)
{
    HW0(DST1) = HW2(SRC1);
    HW1(DST1) = HW2(SRC2);
    HW2(DST1) = HW3(SRC1);
    HW3(DST1) = HW3(SRC2);
    DSTNAT1   = SRCNAT1 | SRCNAT2;
    return 0;
}
