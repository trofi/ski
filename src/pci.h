/*
 * Simulator PCI Header
 */
#ifndef _SKI_PCI_H
#define _SKI_PCI_H

#include "types.h"

extern WORD pciBar[6];

REG pciConfigRd(REG pci_address, REG size);
void pciConfigWr(REG pci_address, REG size, REG value);

#endif /* _SKI_PCI_H */
