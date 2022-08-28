#ifndef _SKI_SSXDSD_H_
#define _SKI_SSXDSD_H_

#include "std.h"

#ifdef HAVE_MOTIF

void datwInit(void);
void dwResize(Widget w, XEvent *event,
	      String *params, Cardinal *num_params);
void prgwInit(void);
void pwResize(Widget w, XEvent *event, String *params,
	      Cardinal *num_params);
void regwInit(void);

#endif /* HAVE_MOTIF */

#endif /* _SKI_SSXDSD_H_ */
