#ifndef _SKI_SSDBT_H
#define _SKI_SSDBT_H

#include "std.h"
#include "types.h"

BOOL dbptCheck(ADDR adr, unsigned acctyp, unsigned dlen);
BOOL bptDisplay(unsigned argc, char *argv[]);
BOOL bptSet(unsigned argc, char *argv[]);
BOOL bptDelete(unsigned argc, char *argv[]);
BOOL bptDAll(unsigned argc, char *argv[]);
BOOL iAbptSet(unsigned argc, char *argv[]);
int isbpt(ADDR adr);
BOOL dbptDisplay(unsigned argc, char *argv[]);
BOOL dbptSet(unsigned argc, char *argv[]);
BOOL dbptDelete(unsigned argc, char *argv[]);
BOOL dbptDAll(unsigned argc, char *argv[]);
void bptLoad(void);
void bptUnload(void);
BOOL tbDisplay(unsigned argc, char *argv[]);

#endif /* _SKI_SSDBT_H */
