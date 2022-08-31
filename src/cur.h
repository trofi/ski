#ifndef _SKI_CUR_H
#define _SKI_CUR_H

#include "std.h"

BOOL regwFwdCur(unsigned argc, char *argv[]);
BOOL regwBkwdCur(unsigned argc, char *argv[]);
void regwUpdateCur(void);

void datwUpdateCur(void);

void prgwDrawCur(void);
void prgwUpdateCur(void);

void cmdwUpdateCur(void);
void cmdwSetStatusCur(const char *);
void cmdwPrintCur(const char *);

void scrnInitCur(void);
void scrnEndCur(void);

void cmdOutCur(const char *hdr, const char *buf);
void cmdLoopCur(void);

#endif /* _SKI_CUR_H */
