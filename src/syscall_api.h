#ifndef _SKI_SYSCALL_API_H
#define _SKI_SYSCALL_API_H

/*
 * These APIs have to be implemented by OS backends.
 * Usually in src/<OS>/syscall-<OS>.c files.
 */

extern void saveOpenFiles(FILE *);
extern void restoreOpenFile(char *, unsigned, unsigned, unsigned);

#endif /* _SKI_SYSCALL_API_H */
