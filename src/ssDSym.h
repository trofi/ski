#ifndef _SKI_SSDSYM_H_
#define _SKI_SSDSYM_H_

#include "std.h"

BOOL cmdISymLst(unsigned argc, char *argv[]);
void symGetInit(void);
BOOL symGet(char **symnam, ADDR *adr);
void symSet(char *strval, ADDR adr);
void symAddrtoDefFunc(ADDR adr, char sname[], int rmdr, int add0x, int width);

#endif /* _SKI_SSDSYM_H_ */
