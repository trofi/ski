/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 *
 * Simulator Platform-Specific Header
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
#ifndef _SKI_PLATFORM_H
#define _SKI_PLATFORM_H

/*--------------------------------------------*/
/*  Data structures defined in this file only */
/*--------------------------------------------*/
typedef struct {
    unsigned long long addr;
    unsigned long long data;
    unsigned long long fpnt;
    unsigned int       size;
} IS_t;

/*-------------------------------------------------------*/
/*  Function prototypes core code calls these functions  */
/*-------------------------------------------------------*/
int ioLoad(IS_t *argIn);
int ioStore(IS_t *argIn);

int iomLoad(IS_t *argIn);
int iomStore(IS_t *argIn);

void xtpCycle(BYTE val);

void implArgs(void);
void InitPlatform(void);
void implInit(void);

#endif	/* _SKI_PLATFORM_H */
