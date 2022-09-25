#include <ia64/sal.h>
#include <ia64/setup.h>
#include <elf.h>

typedef union ia64_va {
	struct {
		unsigned long off : 61;		/* intra-region offset */
		unsigned long reg :  3;		/* region number */
	} f;
	unsigned long l;
	void *p;
} ia64_va;

/*
 * I think in firmware code we can directly use identity mapping.
 * as we did not enable virtual addressing yet.
 */
#define __pa(x)		({ia64_va _v; _v.l = (long) (x); _v.f.reg = 0; _v.l;})
