#ifndef _SKI_SSGTK_H_
#define _SKI_SSGTK_H_

#include "std.h"

#ifdef HAVE_GTK

/* Callbacks exported from binary for glade. */
/* TODO: explicitly expose these symbols via attribute visibility */
/* TODO: validate if these prototypes have correct expected types. */
void SimBtnCmdGtk(void *data);
void SimComboCmdGtk(void *data);
void SimShowAbout(void *data);

#endif /* !HAVE_GTK */

#endif /* _SKI_SSGTK_H_ */
