#include <stddef.h>
#include <stdio.h>

#include "syscall-print.h"

#include "syscall-linux.h"
#include "ui.h"

const char * syscallName (HWORD num)
{
  switch (num)
  {
  #define CASE_VAL_TO_STRING(sys_name) \
    case LIA64_##sys_name: return #sys_name;

  FOR_EACH_SYSCALL(CASE_VAL_TO_STRING)

  #undef CASE_VAL_TO_STRING
  }
  return NULL;
}

void traceSyscallEnter (HWORD num, REG arg0, REG arg1, REG arg2,
			REG arg3, REG arg4, REG arg5)
{
  const char * sn = syscallName (num);
  char snNum[16];

  if (!sn)
  {
    snprintf (snNum, sizeof (snNum), "sc%d", num);
    sn = snNum;
  }

  cmdwPrint ("<%s(%d)>(0x%llx,0x%llx,0x%llx,0x%llx,0x%llx,0x%llx) -> ",
	     sn, num, arg0, arg1, arg2, arg3, arg4, arg5);
}

void traceSyscallExit (REG ret, REG status)
{
  cmdwPrint ("0x%llx/%lld\n", ret, status);
}
