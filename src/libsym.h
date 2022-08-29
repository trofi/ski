/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Symbol Table Library Header
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
#ifndef _SKI_LIBSYM_H
#define _SKI_LIBSYM_H

#include "std.h"

typedef struct {
    char *name;			/* symbol name */
    void *extra;		/* pointer to extra symbol info */
    unsigned long long addr;	/* value (address) associated with symbol */
} Symbol, *SymbolPtr;

typedef struct symTableStructure *SymTablePtr;
typedef int (*PSCF)(const SymbolPtr sym, const void *cmpArg);

extern SymTablePtr defaultSymTable;

/* function prototypes */
#define symAddrtoAddr(addr, dist, cmpFn, cmpArg) \
	symAddrtoAddrX(defaultSymTable, addr, dist, cmpFn, cmpArg)
unsigned long long symAddrtoAddrX(SymTablePtr t, unsigned long long addr,
				  int dist, PSCF cmpFn, const void *cmpArg);
#define symInsert(name, addr, extra)	((void)symInsertX(defaultSymTable, name, addr, extra))
SymbolPtr symInsertX(SymTablePtr t, const char *name, unsigned long long addr,
		     void *extra);
#define symNametoAddr(name, paddr) symNametoAddrX(defaultSymTable, name, paddr)
BOOL symNametoAddrX(SymTablePtr t, const char *name, unsigned long long *paddr);
#define symNameLookup(name, paddr, extra) \
	symNameLookupX(defaultSymTable, name, paddr, extra)
BOOL symNameLookupX(SymTablePtr t, const char *name, unsigned long long *paddr,
		   void **extra);
void symAddrtoName(unsigned long long addr, char sname[], int rmdr, int add0x,
		   int width);
#define symNextSymInit() symNextSymInitX(defaultSymTable)
void symNextSymInitX(SymTablePtr t);
#define symNextSym(sym) symNextSymX(defaultSymTable, sym)
BOOL symNextSymX(SymTablePtr t, SymbolPtr sym);
#define symDeleteAddr(saddr, eaddr) \
	symDeleteAddrX(defaultSymTable, saddr, eaddr)
void symDeleteAddrX(SymTablePtr t, unsigned long long saddr,
		    unsigned long long eaddr);
void symInit(void);
SymTablePtr symNewTable(void);

#endif 	/* _SKI_LIBSYM_H */
