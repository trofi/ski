#include "syscall-print.h"

void traceSyscallEnter (HWORD num, REG arg0, REG arg1, REG arg2,
			REG arg3, REG arg4, REG arg5)
{
  cmdwPrint ("<sc%d>(0x%llx,0x%llx,0x%llx,0x%llx,0x%llx,0x%llx) -> ",
	     num, arg0, arg1, arg2, arg3, arg4, arg5);
}

void traceSyscallExit (REG ret, REG status)
{
  cmdwPrint ("0x%llx/%lld\n", ret, status);
}
