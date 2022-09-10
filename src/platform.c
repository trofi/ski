/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 *
 * Simulator Platform-Specific Code
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "std.h"
#include "bits.h"
#include "types.h"
#include "platform.h"
#include "sim.h"
#include "ski.h"
#include "state.h"
#include "syscall_api.h"

extern BOOL autoAlloc, noConsole;
extern char *consLog;
extern unsigned va_len, pa_len, rid_len, key_len;
extern unsigned mips;
extern unsigned alatAssoc;
extern BOOL use_alat;
static unsigned grfile = 128;

#define VGA_START_ADDR 0xB8000
#define VGA_CRTC_INDEX 0xF53D4
#define VGA_CRTC_DATA  0xF53D5
#define VGA_CURSOR_START 0xA
#define VGA_CURSOR_HI 0xE
#define VGA_CURSOR_LO 0xF
#define VGA_MAXX 80
#define VGA_MAXY 25

#define CONS_FILE

static BYTE vga_framebuffer[VGA_MAXX*VGA_MAXY*2];
static ADDR vga_end = VGA_START_ADDR + sizeof vga_framebuffer;
#ifdef CONS_FILE
static FILE *consfile = NULL;
#endif
#ifdef CONS_LOG
static FILE *conslog = NULL;
#endif
#ifdef VGALOG
static FILE *vgalog;
#endif
#ifdef CONS_HPTERM
static BOOL vga_ready = NO;
#endif

#define SBA_ADDR 0xFED00000ULL
#define SBA_END  (SBA_ADDR+0xFF00ULL)
/* XXX - PCI or LBA? */
#define PCI_ADDR 0xFED20000ULL
#define PCI_END  (PCI_ADDR+0xFF00ULL)
#define IOBLK_PORT_ADDR 0xFFFFC000000ULL
#define UART1_ADDR 0xFF5E0000ULL
#define UART1_END  (UART1_ADDR+0xFF0ULL)
#define UART2_ADDR 0xFF5E2000ULL
#define UART2_END  (UART2_ADDR+0xFF0ULL)

static unsigned pci_io_reg_addr = 0;
#ifdef CONS_HPTERM
static unsigned cursor_state = 0, newpos = 0;
#endif

extern WORD pciBar[];

static int sbaRead(unsigned offset, REG *data)
{
    if (!offset) {	/* vendor and device id */
	*data = 0x3C10291200000000ULL;	/* PLUTO */
	return 2;
    }
    if (offset == 0x1008ULL) {	/* func_class */
	*data = 0x0300000000000000ULL;
	return 2;
    }
    return 0;
}

static int sbaWrite(unsigned offset, REG data)
{
    if (offset >= 0x1200ULL && offset <= 0x1238ULL)	/* ropen_cntrl */
	return 2;
    if (offset == 0x1318ULL || offset == 0x1320ULL)	/* tcng, pdir_base */
	return 2;
    if (offset == 0x1040ULL)				/* rope_config */
	return 2;
    if (offset == 0x1300ULL || offset == 0x1308ULL)	/* ibase, imask */
	return 2;
    return 0;
}

static WORD redirEntry[16] = {
    0x00000100,
    0x00000000,
    0x00000100,
    0x00000000,
    0x00000100,
    0x00000000,
    0x00000100,
    0x00000000,
    0x00000100,
    0x00000000,
    0x00000100,
    0x00000000,
    0x00000100,
    0x00000000,
    0x00000100,
    0x00000000
};

static BYTE scsimem[0x100] = {0};

static int pciRead(unsigned offset, REG *data)
{
    if (!offset) {	/* vendor and device id */
	*data = 0x3C10541000000000ULL;	
	return 2;
    }
    if (offset == 0x008ULL) {	/* func_class */
	*data = 0x05000000ULL;
	return 2;
    }
    if (offset == 0x108ULL) {	/* status/control */
	*data = 0;
	return 2;
    }
    if (offset == 0x680ULL) {	/* error config */
	*data = 0;
	return 2;
    }
    if (offset == 0x810ULL) {	/* I/O window */
	switch (pci_io_reg_addr) {
	    case 0x01000000:	/* I/O SAPIC Version */
		*data = 0x20001F00ULL;
		break;
	    case 0x10000000:
	    case 0x11000000:
	    case 0x12000000:
	    case 0x13000000:
	    case 0x14000000:
	    case 0x15000000:
	    case 0x16000000:
	    case 0x17000000:
	    case 0x18000000:
	    case 0x19000000:
	    case 0x1A000000:
	    case 0x1B000000:
	    case 0x1C000000:
	    case 0x1D000000:
	    case 0x1E000000:
	    case 0x1F000000:
		*data = redirEntry[(pci_io_reg_addr-0x10000000)>>24];
		break;
	    default:
		return 0;
	}
	pci_io_reg_addr = 0;
	return 2;
    }
    return 0;
}

static int pciWrite(unsigned offset, REG data)
{
    if (offset == 0x108ULL)	/* status/control */
	return 2;
    if (offset == 0x800ULL) {	/* I/O register select */
	pci_io_reg_addr = data;	/* 4-byte little-endian */
	return 2;
    }
    if (offset == 0x810ULL) {	/* I/O window */
	switch (pci_io_reg_addr) {
	    case 0x10000000:
	    case 0x11000000:
	    case 0x12000000:
	    case 0x13000000:
	    case 0x14000000:
	    case 0x15000000:
	    case 0x16000000:
	    case 0x17000000:
	    case 0x18000000:
	    case 0x19000000:
	    case 0x1A000000:
	    case 0x1B000000:
	    case 0x1C000000:
	    case 0x1D000000:
	    case 0x1E000000:
	    case 0x1F000000:
		redirEntry[(pci_io_reg_addr-0x10000000)>>24] = data;
		break;
	    default:
		return 0;
	}
	pci_io_reg_addr = 0;
	return 2;
    }
    return 0;
}

/*
 * Good overview of an UART8250 chip:
 *   https://en.wikibooks.org/wiki/Serial_Programming/8250_UART_Programming
 * Ski implements minimal byte-at-a-time I/O to make serial port work.
 */

/* 1-byte UART registers */
#define UART8250_RBR 0x00         /* R:   data rx; DLAB == 0 */
#define UART8250_THR UART8250_RBR /* W:   data tx; DLAB == 0 */
#define UART8250_IER 0x01         /* W:   interrupt enable; DLAB == 0 */
#define UART8250_IRR UART8250_IER /* R:   interrupt identification; DLAB == 0 */
#define UART8250_FCR 0x02         /* W:   FIFO control */
#define UART8250_LCR 0x03         /* R/W: Line control */
#define UART8250_MCR 0x04         /* R/W: Modem control */
#define UART8250_LSR 0x05         /* R:   Line status */
#define UART8250_MSR 0x06         /* R:   Modem status */
#define UART8250_SR  0x07         /* R/W: Scratch status */

/*
 * IIR bits
 * [ 7 6 5 4 3 2 1 0 ]
 * - 7-6: 00 - no, FIFO
 *        01 - Reserved
 *        10 - FIF enabled, not functioning
 *        11 - FIFO enabled
 * - 5: 64-bit FIFO enabled
 * - 4: Reserved
 * - 3-1: 000 - Modem status interrupt, to reset: read MSR
 *        001 - THR empty, to reset: read IIR or write THR
 *        010 - Received data available, to reset: read RBR
 *        011 - Received line status interrupt, to reset: read LSR
 *        100 - reserved
 *        101 - reserved
 *        110 - Time-out Interrupt pending, to reset: read RBR
 *        111 - reserved
 * - 0: Interrupt pending: '0' - yes, '1' - no (surprisingly).
 */

/* LSR bits */
#define UART_LSR_BIT_TEMT  0x40 /* Transmitter empty */
#define UART_LSR_BIT_THRE  0x20 /* Transmit-hold-register empty */

static void uartRead(int uartno, int regno, char * pc) {
    switch (regno) {
	case UART8250_RBR:
	    readConsole(pc);
	    break;
	case UART8250_IRR:
	    *pc = 1; /* no pending interrupts, no support for anything */
	    break;
	case UART8250_LSR:
	    /* always ready */
	    *pc = UART_LSR_BIT_TEMT | UART_LSR_BIT_THRE;
	    break;
	default:
	    fprintf(stderr, "WARNING: unknown read regno=%u val=%#02X\n", regno, 0);
	    break;
    }
}

static void uartWrite(int uartno, int regno, char c) {
    switch (regno) {
	case UART8250_THR:
	    writeConsole(&c, 1);
	    break;
	case UART8250_IER:
	    if (c != 1) { /* not just interrupts enabled? */
		fprintf(stderr, "WARNING: unknown IER value regno=%u val=%#02X\n", regno, (unsigned)c);
	    }
	    break;
	default:
	    fprintf(stderr, "WARNING: unknown write regno=%u val=%#02X\n", regno, (unsigned)c);
	    break;
    }
}

/* 1-byte SCSI registers */
#define DSTAT  0x0C
#define ISTAT  0x14
#define CTEST2 0x1A
#define CTEST3 0x1B
#define SIST0  0x42
#define SIST1  0x43
#define STEST1 0x4D
#define STEST3 0x4F
#define STEST4 0x52

/* 2-byte SCSI registers */
/* SIDL, SODL, SBDL */

/* 3-byte SCSI registers */
/* DBC */

/* 4-byte SCSI registers */
#define DSA      0x10
#define TEMP     0x1C
/* DNAD, DSPS, ADDER */
#define DSP      0x2C
#define SCRATCHA 0x34
#define SCRATCHB 0x5C
#define SCRATCHC 0x60
#define SCRATCHD 0x64
#define SCRATCHE 0x68
#define SCRATCHF 0x6C
#define SCRATCHG 0x70
#define SCRATCHH 0x74
#define SCRATCHI 0x78
#define SCRATCHJ 0x7C

#define SIM_KERNEL
int ioLoad(IS_t *argIn)
{
#ifdef SIM_KERNEL
    if (argIn->size == 8 && argIn->addr == 0xA000010000ULL)
	return 1;
    if (argIn->size == 1 && argIn->addr >= 0xE0000ULL && argIn->addr < 0x100000ULL)
	return 1;
#endif
    if (argIn->size == 1 && argIn->addr >= VGA_START_ADDR && argIn->addr < vga_end) {
	unsigned offset = argIn->addr-VGA_START_ADDR;

	argIn->data = vga_framebuffer[offset];
	return 2;
    }
    if (argIn->addr >= SBA_ADDR && argIn->addr <= SBA_END)
	return sbaRead(argIn->addr - SBA_ADDR, &argIn->data);
    if (argIn->addr >= PCI_ADDR && argIn->addr <= PCI_END)
	return pciRead(argIn->addr - PCI_ADDR, &argIn->data);

    /* UART devices: */
    if (argIn->addr >= UART1_ADDR && argIn->addr <= UART1_END) {
	unsigned int offset = argIn->addr - UART1_ADDR;
	if (argIn->size != 1) {
		fprintf(stderr, "WARNING: UART1: non-byte read: offset=%u val=%llx size=%u\n",
			offset, argIn->data, argIn->size);
	}
	unsigned char c = '\0';
	uartRead(1, offset, &c);
	argIn->data = c;
	return 2;
    }
    if (argIn->addr >= UART2_ADDR && argIn->addr <= UART2_END) {
	unsigned int offset = argIn->addr - UART2_ADDR;
	if (argIn->size != 1) {
		fprintf(stderr, "WARNING: UART2: non-byte read: offset=%u val=%llx size=%u\n",
			offset, argIn->data, argIn->size);
	}
	unsigned char c = '\0';
	uartRead(2, offset, &c);
	argIn->data = c;
	return 2;
    }

    if (argIn->addr >= IOBLK_PORT_ADDR) {
	argIn->data = 0;
	return 2;
    }
    if (argIn->addr >= pciBar[1] && argIn->addr < pciBar[1]+0x1000ULL) {
	unsigned offset = argIn->addr - pciBar[1];

	if (argIn->size != 1)
	    return 0;
	argIn->data = scsimem[offset];
	if (offset == DSTAT && (argIn->data & 0x4)) {
	    scsimem[offset] &= ~0x4;	/* SIR, SCRIPTS Interrupt received */
	    scsimem[ISTAT] &= ~0x1;	/* DIP, DMA Interrupt pending */
	}
	if (offset == CTEST2)
	    scsimem[ISTAT] &= ~0x20;	/* SIGP, Signal process */
	if (offset == SIST0 && (argIn->data & 0x80)) {
	    scsimem[offset] &= ~0x80;	/* MA, Phase mismatch interrupt */
	    scsimem[ISTAT] &= ~0x2;	/* SIP, SCSI Interrupt pending */
	}
	if (offset == SIST1 && (argIn->data & 0x4)) {
	    scsimem[offset] &= ~0x4;	/* STO, Selection timeout */
	    scsimem[ISTAT] &= ~0x2;	/* SIP, SCSI Interrupt pending */
	}
	return 2;
    }
    if (argIn->addr >= pciBar[2] && argIn->addr < pciBar[2]+0x1000ULL)
	return 1;
    return 1;
}

int ioStore(IS_t *argIn)
{
    if (argIn->size == 1 && argIn->addr >= VGA_START_ADDR && argIn->addr < vga_end) {
	unsigned offset = argIn->addr-VGA_START_ADDR;

#ifdef CONS_FILE
	if (!consfile)
	    consfile = fopen("console", "w");
#endif
#ifdef CONS_LOG
	if (!conslog)
	    conslog = fopen("console.log", "w");
#endif
	vga_framebuffer[offset] = argIn->data;
	if (!(argIn->addr & 1)) {
#ifdef CONS_FILE
	    fseek(consfile, offset>>1, SEEK_SET);
	    fputc(argIn->data & 0xFF, consfile);
	    fflush(consfile);
#endif
#ifdef CONS_LOG
	    fputc(argIn->data & 0xFF, conslog);
	    fflush(conslog);
#endif
#ifdef CONS_HPTERM
	    if (vga_ready)
		doSSC(31, argIn->data);
#endif
#ifdef VGALOG
	    fprintf(vgalog, "write to addr %llx (%u,%u)\n", argIn->addr,
		    (offset>>1)/VGA_MAXX, (offset>>1)%VGA_MAXX);
#endif
	}
	return 2;
    }
    if (argIn->addr >= SBA_ADDR && argIn->addr <= SBA_END)
	return sbaWrite(argIn->addr - SBA_ADDR, argIn->data);
    if (argIn->addr >= PCI_ADDR && argIn->addr <= PCI_END)
	return pciWrite(argIn->addr - PCI_ADDR, argIn->data);

    /* UART devices */
    if (argIn->addr >= UART1_ADDR && argIn->addr <= UART1_END) {
	unsigned int offset = argIn->addr - UART1_ADDR;
	if (argIn->size != 1) {
		fprintf(stderr, "WARNING: UART1: non-byte write: offset=%u val=%llx size=%u\n",
			offset, argIn->data, argIn->size);
	}
	uartWrite(1, offset, argIn->data);
	return 2;
    }
    if (argIn->addr >= UART2_ADDR && argIn->addr <= UART2_END) {
	unsigned int offset = argIn->addr - UART2_ADDR;
	if (argIn->size != 1) {
		fprintf(stderr, "WARNING: UART2: non-byte write: offset=%u val=%llx size=%u\n",
			offset, argIn->data, argIn->size);
	}
	uartWrite(2, offset, argIn->data);
	return 2;
    }

    if (argIn->addr >= IOBLK_PORT_ADDR) {
#ifdef CONS_HPTERM
	unsigned offset = argIn->addr - IOBLK_PORT_ADDR;

	if (offset == VGA_CRTC_INDEX && argIn->data == VGA_CURSOR_START)
	    vga_ready = YES;
	switch (cursor_state) {
	    case 0:
		if (offset == VGA_CRTC_INDEX && argIn->data == VGA_CURSOR_HI)
		    cursor_state = 1;
		break;
	    case 1:
		if (offset == VGA_CRTC_INDEX && argIn->data == VGA_CURSOR_LO)
		    cursor_state = 2;
		if (offset == VGA_CRTC_DATA)
		    newpos = argIn->data << 8;
		break;
	    case 2:
		if (offset == VGA_CRTC_DATA) {
		    unsigned newxpos, newypos;
		    static unsigned lastypos = 0;

		    newpos |= argIn->data;
		    newypos = newpos / VGA_MAXX;
		    newxpos = newpos % VGA_MAXX;
#ifdef VGALOG
		    fprintf(vgalog, "set cursor to (%u,%u)\n",
			    newypos, newxpos);
		    fflush(vgalog);
#endif
		    if (newypos && !newxpos && newypos > lastypos) {
			doSSC(31, (REG)'\r');
			doSSC(31, (REG)'\n');
			lastypos = newypos;
		    }
		}
		cursor_state = 0;
		break;
	}
#endif
	return 2;
    }
    if (argIn->addr >= pciBar[1] && argIn->addr < pciBar[1]+0x1000ULL) {
	unsigned offset = argIn->addr - pciBar[1];

	if (argIn->size == 4)
	    switch (offset) {
		case DSA:
		case DSP:
		case TEMP:
		case SCRATCHA:
		case SCRATCHB:
		case SCRATCHC:
		case SCRATCHD:
		case SCRATCHE:
		case SCRATCHF:
		case SCRATCHG:
		case SCRATCHH:
		case SCRATCHI:
		case SCRATCHJ:
		    scsimem[offset+0] = BitfR(argIn->data,56,8);
		    scsimem[offset+1] = BitfR(argIn->data,48,8);
		    scsimem[offset+2] = BitfR(argIn->data,40,8);
		    scsimem[offset+3] = BitfR(argIn->data,32,8);
		    return 2;
		default:
		    return 0;
	    }
	if (argIn->size != 1)
	    return 0;
	if (offset == DSTAT)	/* read-only register */
	    return 0;
	scsimem[offset] = argIn->data;
	if (offset == ISTAT && (argIn->data & 0x20))	/* SIGP */
	    /*runScript()*/;
	if (offset == DSP)
	    /*runScript()*/;
	if (offset == STEST1 && (argIn->data & 0x8))	/* QEN */
	    scsimem[STEST4] |= 0x20;	/* LOCK */
	if (offset == STEST3) {
	    scsimem[offset] &= ~0x2;	/* CSF, Clear SCSI FIFO */
	    /* XXX - actually clear the FIFO? */
	}
	return 2;
    }
    if (argIn->addr >= pciBar[2] && argIn->addr < pciBar[2]+0x1000ULL)
	return 1;
    return 1;
}

/* XXX - this might be better as a BOOL function since caller only checks
 *	 for the value of 2 vs. not-2
 */
int iomLoad(IS_t *argIn)
{
    if (argIn->addr >= VGA_START_ADDR && argIn->addr < vga_end) {
	unsigned offset = argIn->addr-VGA_START_ADDR;

	(void)memcpy(&argIn->data, vga_framebuffer+offset, 8);
	return 2;
    }
    return 1;
}

/* Any page requested using =1/2/4/8/s/i/t is created */
int iomStore(IS_t *argIn)
{
    return 1;
}

void xtpCycle(BYTE val)
{
}

void implArgs(void)
{
    argIns("-alat", &use_alat, ARG_BOOL, "bcx", "");
    argIns("-alatassoc", &alatAssoc, ARG_INT4, "bcx", "");
    argIns("-autoalloc", &autoAlloc, ARG_BOOL, "bcx", "");
    argIns("-mips", &mips, ARG_INT4, "bcx", "");
    argIns("-noconsole", &noConsole, ARG_BOOL, "bx",
	   "-noconsole      Use stdout for console output");
    argIns("-conslog", &consLog, ARG_STRING, "bcx",
	   "-conslog <file> Log the console output to the specified file");
    argIns("-palen", &pa_len, ARG_INT4, "bcx",
	   "-palen <n>      Implemented physical address bits.  Default: 63");
    argIns("-valen", &va_len, ARG_INT4, "bcx",
	   "-valen <n>      Implemented virtual address bits.  Default: 61");
    argIns("-ridlen", &rid_len, ARG_INT4, "bcx",
	   "-ridlen <n>     Implemented RR.rid bits.  Default: 24");
    argIns("-keylen", &key_len, ARG_INT4, "bcx",
	   "-keylen <n>     Implemented PKR.key bits.  Default: 24");
    argIns("-grfile", &grfile, ARG_INT4, "bcx",
	   "-grfile <n>     GR file size.  Default: 128");
}

void InitPlatform(void)
{
    /* Validate some of the customization values, for a lack of a better
	place to do it for now */
    if (pa_len < 32 || pa_len > 63) {
	(void)fprintf(stderr, "Illegal -palen argument: %u"
		      " (must be in the range [32-63])\n", pa_len);
	exit(EXIT_FAILURE);
    }
    if (va_len < 51 || va_len > 61) {
	(void)fprintf(stderr, "Illegal -valen argument: %u"
		      " (must be in the range [51-61])\n", va_len);
	exit(EXIT_FAILURE);
    }
    if (rid_len < 18 || rid_len > key_len || key_len > 24) {
	(void)fprintf(stderr,
		"Illegal -ridlen(%u)/-keylen(%u)"
		" (must have 18 <= rid <= key <= 24)\n",
		rid_len, key_len);
	exit(EXIT_FAILURE);
    }
    if (grfile < 128 || grfile > MAX_NGRS || grfile % 8) {
	(void)fprintf(stderr,
		"Illegal -grfile argument: %u"
		" (must be a multiple of 8 in the range [128-%d])\n",
		grfile, MAX_NGRS);
	exit(EXIT_FAILURE);
    } else {
	n_stack_phys = grfile - GR_STK_BASE;
	invalid = n_stack_phys - N_STACK_VIRT;
    }
    if (!alatAssoc || alatAssoc > 32 || (alatAssoc & (alatAssoc-1))) {
	(void)fprintf(stderr, "Illegal -alatassoc argument: %u"
		      " (must be in 1, 2, 4, 8, 16, or 32)\n", alatAssoc);
	exit(EXIT_FAILURE);
    }
#ifdef VGALOG
    vgalog = fopen("vgalog", "w");
#endif
    scsimem[DSTAT]  = 0x80;	/* DSTAT_DFE, DMA FIFO empty */
    scsimem[CTEST3] = 0xA0;	/* Chip rev. level matches PCI rev. ID */
    scsimem[STEST4] = 0xC0;
}

void implInit(void)
{
    alatInit();
}
