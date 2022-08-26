/*
 * Provide helpers to print system call parameters and return values to
 * support tracing (-strace option).
 */
#ifndef _SKI_SYSCALL_PRINT_H
#define _SKI_SYSCALL_PRINT_H

#include "types.h"

void traceSyscallEnter (HWORD num, REG arg0, REG arg1, REG arg2,
			REG arg3, REG arg4, REG arg5);
void traceSyscallExit (REG ret, REG status);

#endif /* _SKI_SYSCALL_LINUX_H */
