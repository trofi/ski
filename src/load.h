#ifndef _SKI_LOAD_H
#define _SKI_LOAD_H

#include "std.h" /* BOOL */
#include "types.h" /* ADDR */

extern ADDR dataStart; /* used to set initial location of data window */

void dynBlock(ADDR start, ADDR len);
BOOL elfLoad(const char *file_name, int s_argc, char *s_argv[]);
BOOL elfSymLoad(const char *file_name);
void mmapSyms(int fd, ADDR start, ADDR len, ADDR offset);
void munmapSyms(ADDR low, ADDR high);

#endif /* _SKI_LOAD_H */
