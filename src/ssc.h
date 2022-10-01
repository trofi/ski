/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator Simulator System Calls Header
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
#ifndef _SKI_SSC_H
#define _SKI_SSC_H

#include "types.h"

/**************************************************************************
 * Simulator System Call (SSC) numbers
 **************************************************************************/

#define SSC_STOP			 0

#define SSC_CONSOLE_INIT		20
#define SSC_GETCHAR			21
#define SSC_PUTCHAR			31

#define SSC_OPEN			50
#define SSC_CLOSE			51
#define SSC_READ			52
#define SSC_WRITE			53
#define SSC_GET_COMPLETION		54
#define SSC_WAIT_COMPLETION		55

#define SSC_CONNECT_INTERRUPT		58
#define SSC_GENERATE_INTERRUPT		59
#define SSC_SET_PERIODIC_INTERRUPT	60
#define SSC_GET_RTC			65

#define SSC_EXIT			66

#define SSC_LOAD_SYMBOLS		69

#define SSC_GET_TOD			74
#define SSC_GET_ARGS			75

#define SSC_CTL_TRACE			76
#define SSC_GET_INITRAMFS		77

#define SSC_DUMP_CMD_SETUP		86
#define SSC_DUMP			87

#define SSC_NETDEV_PROBE                100
#define SSC_NETDEV_SEND                 101
#define SSC_NETDEV_RECV                 102
#define SSC_NETDEV_ATTACH		103
#define SSC_NETDEV_DETACH		104

#define	SSC_SAL_SET_VECTORS		120

/**************************************************************************
 * Interrupt types
 **************************************************************************/

#define DISK_IRPT		1
#define MOUSE_IRPT		2
#define KEYBOARD_IRPT		3
#define CLK_TIMER_IRPT		4
#define PROF_TIMER_IRPT		5
#define APC_IRPT		6
#define DPC_IRPT		7
#define NETWORK_IRPT		8
#define SIMSCSI_IRPT		9

#define IO_IRPT_CNT		500	/* Make configurable?		 */

/**************************************************************************
 * File access typs
 **************************************************************************/

#define SSC_READ_ACCESS		1
#define SSC_WRITE_ACCESS	2

/**************************************************************************
 * typedefs
 **************************************************************************/

struct sscReqNode {
    int irptcnt;
    unsigned type;
    unsigned fd;
    WORD count;
    struct sscReqNode *next;
};

typedef struct sscReqNode SscReqNode;


/**************************************************************************
 * externs
 **************************************************************************/

extern SscReqNode *sscPend;

void pendSscIrpt(unsigned type);
void doSSC(HWORD num, REG arg0, REG arg1, REG arg2, REG arg3, REG *ret);

extern char *initramfsFile;

#endif	/* _SKI_SSC_H */
