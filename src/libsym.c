/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Symbol Table Library Routines
 *
 * Copyright (C) 1995-2007, Hewlett-Packard Development Company, L.P.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

/* The symbol table allows lookups based upon either the symbol's name or
   its address.  The name lookup uses a hash table and collision chains.
   The address lookup uses a Skip List.  The following description is from
   Pugh's example C Skip List implementation, with comments related to our
   symbol table use in square brackets [like this].

   Skip Lists are a probabilistic alternative to balanced trees, as
   described in the June 1990 issue of CACM and were invented by William
   Pugh in 1987.

    A couple of comments about this implementation:
      The routine randomLevel has been hard-coded to generate random
      levels using p=0.25. It can be easily changed [but don't].

      The insertion routine has been implemented so as to use the dirty
      hack described in the CACM paper: if a random level is generated that
      is more than the current maximum level, the current maximum level
      plus one is used instead.

      Levels start at zero and go up to MaxLevel, which is equal to
      (MaxNumberOfLevels-1).

      The compile flag allowDuplicates determines whether or not duplicates
      are allowed. If defined, duplicates are allowed and act in a FIFO
      manner.  If not defined, an insertion of a value already in the file
      updates the previously existing binding.  [This flag has been
      removed.  The symbol table always allows duplicates.]

      BitsInRandom is defined to be the number of bits returned by a call
      to random(). For most all machines with 32-bit integers, this is 31
      bits as currently set. [correct for HP-UX random(); could switch to
      drand48() and change to 48]
 */


/*##################### Header and Constant Definitions ####################*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "std.h"
#include "types.h"
#include "libsym.h"

#define SYMHTSIZ	64	/* hash table entries in symbol table */
				/*  (must be a power of 2!) */
#define BitsInRandom 31
#define MaxNumberOfLevels 16
#define MaxLevel (MaxNumberOfLevels-1)
#define newNodeOfLevel(l) malloc(sizeof (Node)+(l)*sizeof (NodePtr))

/*##################### Datatypes ##########################################*/

typedef ADDR keyType;
typedef Symbol valueType;
typedef struct nodeStructure *NodePtr;

/* symbol structure */
typedef struct nodeStructure {
    Symbol sym;		/* symbol information */
    NodePtr nextname;	/* next symbol linked in name hash table */
    NodePtr prevval;	/* prev link for value ordered linked list */
    NodePtr nextval[1];	/* variable sized array of next link pointers
			   for value ordered linked list */
} Node;

typedef struct symTableStructure {
    int level;		/* Maximum level of the list
			   (1 less than the number of levels in the list) */
    NodePtr header;	/* pointer to header */
    NodePtr nextSymPtr;	/* used to step through list in symNextSym() */
    NodePtr symht[SYMHTSIZ];	/* symbol hash table */
#ifdef DEBUG
    unsigned int count[MaxNumberOfLevels];
#endif /* DEBUG */
} SymTable;

/*##################### Local Variables ####################################*/

#ifdef OLD_WAY
static NodePtr symht[SYMHTSIZ] = {NULL};	     /* symbol hash table */
static NodePtr symlist = NULL;		     /* value ordered linked list */
static NodePtr recentSym = NULL;  /* most recent symbol looked up by addr */
static NodePtr nextSymPtr;
#endif /* OLD_WAY */
static NodePtr NIL;		/* ptr to the "NIL" node */
static long randomBits;		/* stash of random bits */
static unsigned randomsLeft;	/* pairs of bits left in randomBits */
SymTablePtr defaultSymTable;	/* the default symbol table */
#ifdef DEBUG
static unsigned insCount = 0, insLoops = 0, symCount = 0, symLoops = 0,
    cmpCount = 0, cmpLoops = 0;
#endif /* DEBUG */

/*##################### Functions and Subroutines ##########################*/

#define panic(s)	(void)fputs(s, stderr), exit(EXIT_FAILURE)

/*---------------------------------------------------------------------------
 * randomLevel - Returns a random level in the range 0..MaxLevel.
 *---------------------------------------------------------------------------*/
static int randomLevel(void)
{
    register int level = 0;
    register int b;

    do {
	b = randomBits & 3;
	if (!b) level++;
	randomBits >>= 2;
	if (--randomsLeft == 0) {
	    randomBits = random();
	    randomsLeft = BitsInRandom / 2;
	}
    } while (!b);

    return level > MaxLevel ? MaxLevel : level;
}

#ifdef DEBUG
void symStats(void)
{
    unsigned i;

    /* XXX - update this for skip list stats */
    fprintf(stderr, "insCount = %u\n"
	    "insLoops = %u\n"
	    "insLoops/Count = %3.1f\n"
	    "symCount = %u\n"
	    "symLoops = %u\n"
	    "symLoops/Count = %3.1f\n"
	    "cmpCount = %u\n"
	    "cmpLoops  = %u\n"
	    "cmpLoops/Count = %3.1f\n",
	    insCount, insLoops,
	    (float)insLoops / (float)insCount,
	    symCount, symLoops,
	    (float)symLoops / (float)symCount,
	    cmpCount, cmpLoops,
	    (float)cmpLoops / (float)cmpCount);
    for (i = 0; i < MaxNumberOfLevels; i++)
	fprintf(stderr, "count[%u] = %u (%3.1f%%)\n", i,
		defaultSymTable->count[i],
		(float)defaultSymTable->count[i] / insCount * 100.0);
}
#endif /* DEBUG */

/*---------------------------------------------------------------------------
 * symInit - Initialize the symbol table library.  Create the default
 * symbol table.
 *---------------------------------------------------------------------------*/
void symInit(void)
{
    NIL = newNodeOfLevel(0);
    NIL->sym.addr = 0xffffffffffffffffULL;	/* XXX - will this work? */
#ifdef DEBUG
    srandom(1);
    atexit(symStats);
#else /* !DEBUG */
    srandom((unsigned)time(0));
#endif /* DEBUG */
    randomBits = random();
    randomsLeft = BitsInRandom/2;
    defaultSymTable = symNewTable();
}

/*---------------------------------------------------------------------------
 * symNewTable - Create a new symbol table and return a pointer to it.
 * Returns NULL if the new table couldn't be created.
 *---------------------------------------------------------------------------*/
SymTablePtr symNewTable(void)
{
    SymTablePtr t;
    int i;

    if (!(t = malloc(sizeof *t)))
	return NULL;
    t->level = 0;
    /* XXX - doesn't this allocate one too many pointers? */
    t->header = newNodeOfLevel(MaxNumberOfLevels);
    for (i = 0; i < MaxNumberOfLevels; i++) {
	t->header->nextval[i] = NIL;
#ifdef DEBUG
	t->count[i] = 0;
#endif /* DEBUG */
    }
    t->nextSymPtr = 0;
    for (i = 0; i < SYMHTSIZ; i++)
	t->symht[i] = 0;
    return t;
}

/*---------------------------------------------------------------------------
 * symHash - Hash into the symbol hash table t and return the
 * address of the hash table entry corresponding to the passed string.
 *---------------------------------------------------------------------------*/
static NodePtr *symHash(SymTablePtr t, const char *s)
{
    int i;

    for (i = 0; *s; i += (int)*s++)
	;
    return &t->symht[i & (SYMHTSIZ-1)];
}

/*---------------------------------------------------------------------------
 * nodeNameSearch - Find and return a pointer to the node with the
 * passed name, or NULL if not found.
 *---------------------------------------------------------------------------*/
static NodePtr nodeNameSearch(SymTablePtr t, const char *sname)
{
    NodePtr s;

    for (s = *symHash(t, sname); s && strcmp(sname, s->sym.name);
	 s = s->nextname)
	;
    return s;
}

/*---------------------------------------------------------------------------
 * nodeAddrSearch - Find and return a pointer to the node with the largest
 * address less than or equal to the passed address and for which the cmpFn
 * (if not 0) returns nonzero, or NULL if there are none.
 *---------------------------------------------------------------------------*/
static NodePtr nodeAddrSearch(SymTablePtr t, ADDR adr,
			      PSCF cmpFn, const void *cmpArg)
{
    int k;
    NodePtr p, q;

#ifdef DEBUG
    symCount++;
#endif /* DEBUG */
    /* XXX - this search code sems to hang when searching for ~0; I hope
     *       we never have a symbol whose value is ~0.
     */
    if (adr == ~0ULL)
	return NULL;
    p = t->header;
    k = t->level;
    do {
	while ((q = p->nextval[k]) && q->sym.addr <= adr) {
	    p = q;
#ifdef DEBUG
	    symLoops++;
#endif /* DEBUG */
	}
    } while (--k >= 0);
    if (p == t->header)
	return NULL;
    if (cmpFn) {	/* search prevval list using cmpFn() */
#ifdef DEBUG
	cmpCount++;
#endif /* DEBUG */
	while (p != t->header && !cmpFn(&p->sym, cmpArg)) {
	    p = p->prevval;
#ifdef DEBUG
	    cmpLoops++;
#endif /* DEBUG */
	}
    }
    return p != t->header ? p : NULL;
}

/*---------------------------------------------------------------------------
 * nodeAddrSearchGT - Find and return a pointer to the symbol with the
 * smallest address greater than the passed address and for which the cmpFn
 * (if not 0) returns nonzero, or NULL if there are none.
 *---------------------------------------------------------------------------*/
static NodePtr nodeAddrSearchGT(SymTablePtr t, ADDR adr,
				PSCF cmpFn, const void *cmpArg)
{
    NodePtr s;

    /* XXX - change this to use the skip list like nodeAddrSearch() */
    if (!cmpFn) {
	for (s = t->header->nextval[0]; s != NIL; s = s->nextval[0])
	    if (s->sym.addr > adr)
		break;
    } else {	/* have cmpFn */
	for (s = t->header->nextval[0]; s != NIL; s = s->nextval[0])
	    if (s->sym.addr > adr && cmpFn(&s->sym, cmpArg))
		break;
    }
    return s != NIL ? s : 0;
}

/*---------------------------------------------------------------------------
 * symAddrtoAddrX - Find and return the address of the symbol with the
 * largest address less than or equal to (dist < 0) or greater than (dist
 * >= 0) the passed address and within "dist" of that address (unless dist
 * is 0, which means allow any distance), or the input adr if there are
 * none.  The cmpFn and cmpArg are used to restrict the set of eligible
 * symbols.
 *---------------------------------------------------------------------------*/
ADDR symAddrtoAddrX(SymTablePtr t, ADDR adr, int dist,
		    PSCF cmpFn, const void *cmpArg)
{
    NodePtr p;

    if (dist < 0) {
	p = nodeAddrSearch(t, adr, cmpFn, cmpArg);
	adr = p && (int)(adr - p->sym.addr) <= -dist ? p->sym.addr : adr;
    } else {
	p = nodeAddrSearchGT(t, adr, cmpFn, cmpArg);
	adr = p && (dist == 0 || (int)(p->sym.addr - adr) <= dist) ?
	    p->sym.addr : adr;
    }

    return adr;
}


/*---------------------------------------------------------------------------
 * symInsertX - Insert the passed symbol name into the symbol table t
 * (AND into the address ordered skip list) with the specified virtual
 * address as its value. If the name is already in the symbol table,
 * it is added again.
 *---------------------------------------------------------------------------*/
SymbolPtr symInsertX(SymTablePtr t, const char *name, ADDR addr, void *extra)
{
    NodePtr *h, p, q;
    NodePtr update[MaxNumberOfLevels];
    int k;

#ifdef DEBUG
    insCount++;
#endif /* DEBUG */
    p = t->header;
    k = t->level;
    do {
	while (q = p->nextval[k], q->sym.addr <= addr) {
	    p = q;
#ifdef DEBUG
	    insLoops++;
#endif /* DEBUG */
	}
	update[k] = p;
    } while (--k >= 0);

    k = randomLevel();
    if (k > t->level) {
	k = ++t->level;
	update[k] = t->header;
    };
#ifdef DEBUG
    t->count[k]++;
#endif /* DEBUG */
    if (!(q = newNodeOfLevel(k)) ||
	!(q->sym.name = strdup(name)))
	panic("Out of memory\n");
    /* set the new node's value */
    q->sym.addr = addr;
    q->sym.extra = extra;
    /* link node into skip list */
    do {
	p = update[k];
	q->nextval[k] = p->nextval[k];
	p->nextval[k] = q;
    } while (--k >= 0);
    /* link node into hash chain */
    h = symHash(t, name);
    q->nextname = *h;
    *h = q;
    /* link node into prevval list */
    if (q->nextval[0] != NIL)
	q->nextval[0]->prevval = q;
    q->prevval = p;
    return &q->sym;
}

static void symName(SymbolPtr s, unsigned long long addr, char sname[],
	     int rmdr, int add0x, int width)
{
    BOOL exact = NO;

    if (width < 0) {
	exact = YES;
	width = -width;
    }
    if (s) {
	ADDR distance = addr - s->addr;

	if (!distance) {
	    if (!width)
		(void)strcpy(sname, s->name);
	    else if (strlen(s->name) <= width)
		if (exact)
		    (void)strcpy(sname, s->name);
		else
		    (void)sprintf(sname, "%-*s", width, s->name);
	    else
		(void)sprintf(sname, "%.*s*", width-1, s->name);
	    return;
	}

	if (rmdr && distance < (1ULL<<4*rmdr)) {
	    if (width) {
		int symlen = width - (rmdr + 1 + (add0x ? 2 : 0));

		if (strlen(s->name) <= symlen)
		    if (exact)
			(void)sprintf(sname, "%s+%s%0*llx", s->name,
				      add0x ? "0x" : "", rmdr, distance);
		    else
			(void)sprintf(sname, "%s+%s%0*llx%*s", s->name,
				      add0x ? "0x" : "", rmdr, distance,
				      (int)(symlen - strlen(s->name)), "");
		else
		    (void)sprintf(sname, "%.*s*+%s%0*llx",
				  symlen-1, s->name,
				  add0x ? "0x" : "", rmdr, distance);
	    } else
		(void)sprintf(sname, "%s+%s%0*llx", s->name,
			      add0x ? "0x" : "", rmdr, distance);
	    return;
	}
    }
    (void)sprintf(sname, "%s%016llx", add0x ? "0x" : "", addr);
}

/*---------------------------------------------------------------------------
 * symAddrtoNameX - Find the name of the symbol with the greatest value
 * less than or equal to the passed virtual address and within 16**rmdr
 * bytes.  If cmpFn is not 0, it is called with the Symbol ptr and the
 * cmpArg.  A return of 0 indicates the symbol should not be considered; a
 * nonzero return indicates it is a candidate symbol.  If such a symbol
 * exists, fill in sname with its name using width to either pad or
 * truncate.  If such a symbol does not exist, fill in sname with the hex
 * representation of adr.  rmdr must be >= 0; add0x indicates whether to
 * add a leading 0x width must be 0 or its absolute value must be >= 16 (18
 * if add0x is YES).  width < 0 means that the resultant string should not
 * be padded.
 *---------------------------------------------------------------------------*/
static void symAddrtoNameX(SymTablePtr t, ADDR adr, char sname[], int rmdr,
		    int add0x, int width, PSCF cmpFn, const void *cmpArg)
{
    NodePtr p = nodeAddrSearch(t, adr, cmpFn, cmpArg);
    SymbolPtr s = p ? &p->sym : NULL;

    symName(s, adr, sname, rmdr, add0x, width);
}

/*---------------------------------------------------------------------------
 * Compatibility stub.  Calls symAddrtoNameX() with 0 for cmpFn and cmpArg,
 * and defaultSymTable as the symbol table.
 *---------------------------------------------------------------------------*/
void symAddrtoName(ADDR adr, char sname[], int rmdr, int add0x, int width)
{
    symAddrtoNameX(defaultSymTable, adr, sname, rmdr, add0x, width, 0, 0);
}

/*---------------------------------------------------------------------------
 * symNametoAddrX - Find the symbol with the passed name in symbol table t.
 * If found, place its value (addr) into paddr and return YES; otherwise,
 * return NO.
 *---------------------------------------------------------------------------*/
BOOL symNametoAddrX(SymTablePtr t, const char *sname, ADDR *paddr)
{
    NodePtr np;

    if ((np = nodeNameSearch(t, sname))) {
	*paddr = np->sym.addr;
	return YES;
    }
    return NO;
}


/*---------------------------------------------------------------------------
 * symNameLookupX - Find the symbol with the passed name in symbol table t.
 * If found, place its value into the passed variables and return YES;
 * otherwise, return NO.  Same as symNametoAddr() except that extra is
 * returned.
 *---------------------------------------------------------------------------*/
BOOL symNameLookupX(SymTablePtr t, const char *sname, ADDR *paddr,
		    void **extra)
{
    NodePtr np;

    if ((np = nodeNameSearch(t, sname))) {
	*paddr = np->sym.addr;
	*extra = np->sym.extra;
	return YES;
    }
    return NO;
}


/*---------------------------------------------------------------------------
 * symNextSymInitX - Initialize the nextSymPtr pointer to the top of the
 * symbol table t.
 *---------------------------------------------------------------------------*/
void symNextSymInitX(SymTablePtr t)
{
    t->nextSymPtr = t->header->nextval[0];
}


/*---------------------------------------------------------------------------
 * symNextSymX - If there is a next symbol in symbol table t, return YES
 * and the current symbol parameters, and move the current symbol pointer
 * to the next symbol; otherwise, return NO.
 *---------------------------------------------------------------------------*/
BOOL symNextSymX(SymTablePtr t, SymbolPtr sym)
{
    if (t->nextSymPtr == NIL)
	return NO;
    sym->addr = t->nextSymPtr->sym.addr;
    sym->name = t->nextSymPtr->sym.name;
    sym->extra = t->nextSymPtr->sym.extra;
    t->nextSymPtr = t->nextSymPtr->nextval[0];
    return YES;
}

/*---------------------------------------------------------------------------
 * symDeleteAddrX - Delete the symbols in the range saddr..eaddr.
 *---------------------------------------------------------------------------*/
void symDeleteAddrX(SymTablePtr t, unsigned long long saddr,
		    unsigned long long eaddr)
{
    register int k, m;
    NodePtr update[MaxNumberOfLevels];
    NodePtr p, q, r;

    /* skip nodes < saddr */
    p = t->header;
    k = m = t->level;
    do {
	while (q = p->nextval[k], q->sym.addr < saddr)
	    p = q;
	update[k] = p;
    } while (--k >= 0);

    if (q->sym.addr > eaddr)	/* no nodes in range */
	return;

    for ( ; q->sym.addr <= eaddr; q = r) {
	NodePtr *h, s;

	/* update nextval pointers */
	for (k = 0; k <= m && (r = update[k])->nextval[k] == q; k++)
	    r->nextval[k] = q->nextval[k];
	/* unlink from name list */
	h = symHash(t, q->sym.name);
	for (r = 0, s = *h; q != s; r = s, s = s->nextname)
	    ;
	if (r)
	    r->nextname = s->nextname;
	else
	    *h = s->nextname;
	r = q->nextval[0];
	free(q);
    }

    /* unlink nodes from prevval list */
    if (r != NIL)
	r->prevval = p;
    /* update the table's level */
    while (t->header->nextval[m] == NIL && m > 0)
	m--;
    t->level = m;
}
