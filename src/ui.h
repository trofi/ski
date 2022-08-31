#ifndef _SKI_UI_H
#define _SKI_UI_H

#include <stddef.h>

#include "std.h"

BOOL datwMakeActivew(const char *tag);
/*
    char *tag		registered data window name
    (retval)		was the data window made active?
 */

unsigned datwSz(const char *tag);
/*
    char *tag		registered data window name
    (retval)		data window size
 */

BOOL regwFwd(unsigned argc, char *argv[]);
BOOL regwBkwd(unsigned argc, char *argv[]);
BOOL regwDump(unsigned argc, char *argv[]);
BOOL showUrs(unsigned argc, char *argv[]);
BOOL showSrs(unsigned argc, char *argv[]);
BOOL showGrs(unsigned argc, char *argv[]);
BOOL showFrs(unsigned argc, char *argv[]);
BOOL showIArs(unsigned argc, char *argv[]);

char *getSimID(void);
char *skiID(void);

ssize_t sysWrite(int fd, void *buf, size_t len);

#endif /* _SKI_UI_H */
