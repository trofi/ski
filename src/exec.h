#ifndef _SKI_EXEC_H
#define _SKI_EXEC_H

#include "std.h"
#include "state.h"
#include "types.h"

FREG fill2freg(BYTE sign, WORD exp, DWORD mant);
void freg2spill(FREG fr, BYTE *sign, WORD *exp, DWORD *mant);

BOOL reservedAR(unsigned ar);
BOOL reservedCR(unsigned cr);
BOOL reservedCrField(unsigned cr, REG *val);
BOOL reservedIpsrField(REG val);

int rse_store(void);
int rse_load(void);

#endif /* _SKI_EXEC_H */
