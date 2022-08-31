#ifndef _SKI_SSGTK_H_
#define _SKI_SSGTK_H_

#include "std.h"

#ifdef HAVE_GTK

#include "types.h"

/* Callbacks exported from binary for glade. */
/* TODO: explicitly expose these symbols via attribute visibility */
/* TODO: validate if these prototypes have correct expected types. */
void SimBtnCmdGtk(void *data);
void SimComboCmdGtk(void *data);
void SimShowAbout(void *data);

void regwUpdateGtk(void);
void datwUpdateGtk(void);

void prgwDrawGtk(void);
void prgwUpdateGtk(void);

void cmdwUpdateGtk(void);
void cmdwSetStatusGtk(const char *);
void cmdwPrintGtk(const char *);

void msgwPrintGtk(const char *);

void runIt_setupGtk(void);

void stepIt_setupGtk(CTR cnt);

void scrnInitGtk(void);
void scrnEndGtk(void);

void cmdOutGtk(const char *name, const char *hdr, const char *buf);
void cmdLoopGtk(void);

#endif /* !HAVE_GTK */

#endif /* _SKI_SSGTK_H_ */
