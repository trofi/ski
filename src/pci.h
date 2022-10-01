/*
 * Simulator PCI Header
 */
#ifndef _SKI_PCI_H
#define _SKI_PCI_H

#include "types.h"

extern WORD pciBar[6];

REG pciConfigRd(REG arg1, REG arg2);
void pciConfigWr(REG arg1, REG arg2, REG arg3);

#endif /* _SKI_PCI_H */
