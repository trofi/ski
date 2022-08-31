#ifndef _SKI_SSX_H_
#define _SKI_SSX_H_

#include "std.h"

#ifdef HAVE_MOTIF

#include "types.h"

void regwDrawX(void);
void regwUpdateX(void);

void datwDrawX(void);
void datwUpdateX(void);

void prgwDrawX(void);
void prgwUpdateX(void);

void cmdwUpdateX(void);
void cmdwSetStatusX(const char *);
void cmdwPrintX(const char *);
void msgwPrintX(const char *);

void stepUntil_setupX(const char *expr);
BOOL stepUntil_loopX(CTR delay, REG val, const char *expr);
void stepIt_setupX(CTR cnt);
void runIt_setupX(void);

void scrnInitX(void);
void scrnEndX(void);

void cmdOutX(const char *name, const char *hdr, const char *buf);
void cmdLoopX(void);

#endif /* !HAVE_MOTIF */

#endif /* _SKI_SSX_H_ */
