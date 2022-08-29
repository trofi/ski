/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator X11/Motif Interface (Screen Drawing)
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


/*##################### Header and Constant Definitions ####################*/

#include "std.h"

#ifdef HAVE_MOTIF

#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <Xm/Xm.h>
#include <Xm/Command.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/PanedW.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleBG.h>

#include "ssXDSD.h"

#include "types.h"
#include "ssDCmd.h"
#include "exportui.h"
#include "coreui.h"
#include "libcore.h"	/* Just for ipGet */
#define XSIM
#include "program.h"
#include "data.h"

/*##################### Globals - Imports ##################################*/

extern Widget	cmd, msgs;

extern ADDR	dataStart;
ADDR iAinstAddr(ADDR baseAddr, BYTE iAmode, int delta);

BOOL noscreen = YES;

static ADDR progStart = 0;

typedef struct listWidget {
    char name[40];
    Widget w, hdr, txt;
    struct listWidget *next;
} listWidget;

/*##################### Local Variables ####################################*/

static struct {
    short columns;
    struct {
	Widget form;
	Widget rw[WINTBLSIZ];
    } perProc[NPROC];
    Widget toggles;
} regInfo;

static listWidget *listWidgets = NULL;

/*##################### Functions and Subroutines ##########################*/

static void regwUpd(void);

#define textAdd(w,s)	XmTextInsert(w, XmTextGetLastPosition(w), s)
#define Mapped(w)	(w && XtIsManaged(w))

/*ARGSUSED*/
static void gotoCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    printf("In gotoCB\n");
}

/*ARGSUSED*/
static void datwCloseCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    unsigned i;

    for (i = 0; i < topdatw; i++)
	if (datInfo.perProc[cproc].form[i] == (Widget)client_data)
	    datwtbl[i].show = NO;
    XtUnmanageChild((Widget)client_data);
    /* also unmanage associated windows like Goto? => re-manage later? */
}

/*ARGSUSED*/
static void closeCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XtUnmanageChild((Widget)client_data);
    /* also unmanage associated windows like Goto? => re-manage later? */
}

/*ARGSUSED*/
static void helpCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    (void)XmProcessTraversal((Widget)client_data, XmTRAVERSE_CURRENT);
}

/****************************/
/* Register Window Routines */
/****************************/

void regwInit(void)
{
    unsigned proc;

    regInfo.columns = 79;

    for (proc = 0; proc < nproc; proc++)
	regInfo.perProc[proc].form = NULL;
}

/*--------------------------------------------------------------------------
 * registerWindow$Upd - Update the contents of the passed register window.
 *--------------------------------------------------------------------------*/
static void regwUpd(void)
{
    Widget rwr;
    unsigned i, j;
    REG size;
    XmTextPosition current_position;

    for (i = 0; i < topregw; i++) {
	rwr = regInfo.perProc[cproc].rw[i];
	if (regwtbl[i].show) {
	    XmTextDisableRedisplay(rwr);
	    current_position = XmTextGetTopCharacter(rwr);
	    XmTextSetString(rwr, "");
	    (void)evalExpr(regwtbl[i].size, DECEXP, &size);
	    for (j = 0; j < size; j++) {
		if (j)
		    textAdd(rwr, "\n");
		textAdd(rwr, regwtbl[i].fcn(j));
	    }
	    XmTextSetTopCharacter(rwr, current_position);
	    XmTextEnableRedisplay(rwr);
	    XtManageChild(XtParent(rwr));
	    XtManageChild(rwr);
	} else
	    XtUnmanageChild(XtParent(rwr));
    }
}

/*--------------------------------------------------------------------------
 * registerWindow$Draw - Creates a register window for processor "cproc" if
 *  not already created, initializes its contents, and manages it.
 *--------------------------------------------------------------------------*/
void regwDrawX(void)
{
    Widget rw = regInfo.perProc[cproc].form;

    if (!rw) {
	Widget pane, rc, closeW, helpW;
	char tmpstr[80];
	Arg args[12];
	unsigned i, n;
	REG size;
	const int PXPLN = 20;		/* pixels per line */

	rw = XmCreateFormDialog(cmd, "regw", NULL, 0);
	XtManageChild(rc = XmCreateRowColumn(rw, "regwRC", NULL, 0));
	XtManageChild(XmCreateSeparatorGadget(rw, "regwS", NULL, 0));
	XtManageChild(pane = XmCreatePanedWindow(rw, "regwP", NULL, 0));
	for (i = 0; i < topregw; i++) {
	    Widget rwr;

	    (void)sprintf(tmpstr, "regw%s", regwtbl[i].tag);
	    (void)evalExpr(regwtbl[i].size, DECEXP, &size);
	    n = 0;
	    XtSetArg(args[n], XmNcolumns, regInfo.columns);		n++;
	    XtSetArg(args[n], XmNscrollHorizontal, False);             	n++;
	    XtSetArg(args[n], XmNpaneMaximum, size * PXPLN);		n++;
	    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);		n++;
	    XtSetArg(args[n], XmNeditable, False);			n++;
	    XtSetArg(args[n], XmNautoShowCursorPosition, False);	n++;
	    XtSetArg(args[n], XmNcursorPositionVisible, False);		n++;
	    rwr = XmCreateScrolledText(pane, tmpstr, args, n);
	    regInfo.perProc[cproc].rw[i] = rwr;
	}
	XtManageChild(closeW = XmCreatePushButtonGadget(rc, "close", NULL, 0));
	XtManageChild(helpW = XmCreatePushButtonGadget(rc, "help", NULL, 0));
	XtAddCallback(closeW, XmNactivateCallback, closeCB, rw);
	XtAddCallback(helpW, XmNactivateCallback, helpCB, rw);
	regInfo.perProc[cproc].form = rw;
    }
    regwUpd();
    XtManageChild(rw);
}

/*--------------------------------------------------------------------------
 * registerWindow$Update - Update the contents of all register windows which
 *  have been created.  It does not remap the windows.
 *--------------------------------------------------------------------------*/
void regwUpdateX(void)
{
    unsigned proc;

    for (proc = 0; proc < nproc; proc++)
	if (Mapped(regInfo.perProc[proc].form))
	    regwUpd();
}


/***************************/
/* Program Window Routines */
/***************************/

void prgwInit(void)
{
    unsigned save = cproc;

    progStart = ipGet(0);
    for (cproc = 0; cproc < nproc; cproc++) {
	prgwTop = progStart;
	prgwIS = 0;
	prgwPrevTop = 0;
	prgCmd = PrgUseTop;
	prgInfo.perProc[cproc].form = NULL;
    }
    cproc = save;
    prgInfo.iofsHex = YES;
    prgInfo.iofsSymWidth = 13;
    prgInfo.instHex = YES;
    prgInfo.instSymWidth = 45;
    prgInfo.columns = 88;
}

/*ARGSUSED*/
void pwResize(Widget w, XEvent *event,
	      String *params, Cardinal *num_params)
{
    Arg arg;
    short rows, cols;

    XtSetArg(arg, XmNrows, &rows);
    XtGetValues(w, &arg, 1);
    if (prgInfo.perProc[0].rows != rows) {
	prgInfo.perProc[0].rows = rows;
	prgwDrawX();
    }
    XtSetArg(arg, XmNcolumns, &cols);
    XtGetValues(w, &arg, 1);
    prgColumns = cols;
    prgwDrawX();
}

static void prgwUpd(unsigned proc, ADDR adr)
{
    char hdr[80];
    unsigned i, j;
    char *text, *p;
    unsigned srcrows, asmrows, nbytes;
    Arg arg;
    XmString s;
    Widget pwh = prgInfo.perProc[proc].hdr;
    Widget pwt = prgInfo.perProc[proc].text;

    switch (prgCmd) {
	case PrgUseTop:
	    adr = prgwTop;
	    break;
	case PrgUseBot:
	    adr = prgwBot;
	    break;
	default:
	    if (adr >= prgwTop && adr <= prgwBot)	/* adr is in view */
		adr = prgwTop;
	    else if (prgwIS)	/* iA */
		adr = iAinstAddr(adr, prgwIAmode, -prgOverlap);
	    else			/* EM */
		adr -= prgOverlap*16;
	    break;
    }
    XmTextDisableRedisplay(pwt);
    /* I thought the Disable call above would have effectively set the
       resizeWidth to False, but it didn't seem to do anything.  Should I
       try to get the current value and restore it? */
    XmTextSetString(pwt, "");
    if (prgwIS) {	/* iA */
	for (prgwTop = adr, i = 0; i < prgRows; i++, adr += nbytes) {
	    text = prgwLineIA(adr, prgwIAmode, YES, &nbytes);
	    textAdd(pwt, text);
	}
	prgBndl = prgRows;
	prgwBot = adr - nbytes;
    } else {		/* EM */
	if (prgCmd != PrgUseBot) {
	    for (prgwTop = adr, i = prgRows, prgBndl = 0; i > 0; ) {
		text = prgwLine(adr, &srcrows, &asmrows);
		if (i >= (srcrows + asmrows)) {
		    i -= srcrows + asmrows;
		    textAdd(pwt, text);
		    adr += 16;
		    prgBndl++;
		} else {	/* fill in remaining lines with source */
		    for (p = text, j = 0; j < i && j < srcrows; j++)
			p = strchr(p, '\n') + 1;
		    *p = '\0';
		    textAdd(pwt, text);
		    i = 0;
		}
	    }
	    prgwBot = adr - 16;
	} else {		/* PrgUseBot */
	    for (i = prgRows, prgBndl = 0; i > 0; ) {
		text = prgwLine(adr, &srcrows, &asmrows);
		if (i >= (srcrows + asmrows)) {
		    i -= srcrows + asmrows;
		    XmTextInsert(pwt, 0, text);
		    adr -= 16;
		    prgBndl++;
		} else {
		    if (i >= asmrows) {
			for (p = text, j = 0; j < (srcrows+asmrows) - i; j++)
			    p = strchr(p, '\n') + 1;
			XmTextInsert(pwt, 0, p);
		    }
		    i = 0;
		}
	    }
	    prgwTop = adr + 16;
	}
    }
    if (strlen(p = getSrcName(prgwTop)))
	(void)sprintf(hdr, "Program          (file: %s)", p);
    else
	(void)strcpy(hdr, "Program");
    s = XmStringCreateLocalized(hdr);
    XtSetArg(arg, XmNlabelString, s);
    XtSetValues(pwh, &arg, 1);
    XmStringFree(s);
    prgCmd = PrgUseTop;
    XmTextEnableRedisplay(pwt);
}

void prgwDrawX(void)
{
    Widget pw = prgInfo.perProc[cproc].form;

    if (!pw) {
	short rows;
	Widget pwh, pwt, rc, closeW, jump, helpW;
	Arg arg;

	pw = XmCreateFormDialog(cmd, "prgw", NULL, 0);
	XtManageChild(rc = XmCreateRowColumn(pw, "prgwRC", NULL, 0));
	XtManageChild(XmCreateSeparatorGadget(pw, "prgwS", NULL, 0));
	XtManageChild(pwh = XmCreateLabel(pw, "prgwH", NULL, 0));
	XtManageChild(pwt = XmCreateText(pw, "prgwT", NULL, 0));
	XtManageChild(closeW = XmCreatePushButtonGadget(rc, "close", NULL, 0));
	XtManageChild(jump = XmCreatePushButtonGadget(rc, "goto", NULL, 0));
	XtManageChild(helpW = XmCreatePushButtonGadget(rc, "help", NULL, 0));
	XtAddCallback(closeW, XmNactivateCallback, closeCB, pw);
	XtAddCallback(jump, XmNactivateCallback, gotoCB, (XtPointer)(uintptr_t)cproc);
	XtAddCallback(helpW, XmNactivateCallback, helpCB, pwt);
	XtSetArg(arg, XmNinitialFocus, pwt);
	XtSetValues(pw, &arg, 1);

	XtSetArg(arg, XmNrows, &rows);
	XtGetValues(pwt, &arg, 1);
	prgRows = rows;
	XtSetArg(arg, XmNcolumns, prgColumns);
	XtSetValues(pwt, &arg, 1);
	prgInfo.perProc[cproc].form = pw;
	prgInfo.perProc[cproc].hdr  = pwh;
	prgInfo.perProc[cproc].text = pwt;
    }
    prgwUpd(cproc, prgwCVA);
    XtManageChild(pw);
}

/*--------------------------------------------------------------------------
 * programWindow$Update - Update the contents of all program windows which
 *  have been created.  It does not remap the windows.
 *--------------------------------------------------------------------------*/
void prgwUpdateX(void)
{
    unsigned proc;

    for (proc = 0; proc < nproc; proc++)
	if (Mapped(prgInfo.perProc[proc].form))
	    prgwUpd(proc, prgwCVA);
}

/************************/
/* Data Window Routines */
/************************/

void datwInit(void)
{
    unsigned i, save = cproc;

    for (cproc = 0; cproc < nproc; cproc++) {
	datwCVA = dataStart;
	datwLVA = 0;
	for (i = 0; i < WINTBLSIZ; i++)
	    datInfo.perProc[cproc].form[i] = NULL;
    }
    cproc = save;
    datInfo.numDataWds = 4;
#ifdef __linux__
    datInfo.columns = 67;	/* 67 to include ASCII */
#else
     datInfo.columns = 54;	/* 67 to include ASCII */
#endif
    datInfo.expr[0] = '\0';
}

/*ARGSUSED*/
void dwResize(Widget w, XEvent *event,
	      String *params, Cardinal *num_params)
{
    Arg arg;
    short rows;

    XtSetArg(arg, XmNrows, &rows);
    XtGetValues(w, &arg, 1);
    if (datInfo.perProc[0].rows != rows) {
	datInfo.perProc[0].rows = rows;
	/* XXX - which data window was resized? */
	datwtbl[0].size = rows;
	datwUpdateX();
    }
}

static void datwUpd(unsigned proc, unsigned inx)
{
    char hdr[80];
    short lines;
    Arg arg;
    XmString s;
    Widget dw = datInfo.perProc[proc].form[inx];
    Widget dwh = datInfo.perProc[proc].hdr[inx];
    Widget dwt = datInfo.perProc[proc].text[inx];

    if (!datwtbl[inx].show)
	return;

    (void)strcpy(hdr, "Data");
    s = XmStringCreateLocalized(hdr);
    XtSetArg(arg, XmNlabelString, s);
    XtSetValues(dwh, &arg, 1);
    XmStringFree(s);
    XmTextDisableRedisplay(dwt);
    XtSetArg(arg, XmNresizeWidth, False);
    XtSetValues(dwt, &arg, 1);
    XmTextSetString(dwt, "");
    lines = datwtbl[inx].size;
    textAdd(dwt, datwtbl[inx].bdfcn(lines));
    XtSetArg(arg, XmNresizeWidth, True);
    XtSetValues(dwt, &arg, 1);
    XmTextEnableRedisplay(dwt);
    XtManageChild(dw);
}

void datwDrawX(void)
{
    Widget dw;
    unsigned i;

    for (i = 0; i < topdatw; i++) {
	dw = datInfo.perProc[cproc].form[i];
	if (!dw) {
	    Widget dwh, dwt, rc, closeW, jump, helpW;
	    short rows;
	    Arg arg;
	    char tag[80], tmpstr[80];

	    if (!strcmp(datwtbl[i].tag, "Data"))
		(void)strcpy(tag, "datw");
	    else
		(void)sprintf(tag, "%s", datwtbl[i].tag);

	    (void)sprintf(tmpstr, "%s", tag);
	    dw = XmCreateFormDialog(cmd, tmpstr, NULL, 0);

	    (void)sprintf(tmpstr, "%sRC", tag);
	    XtManageChild(rc = XmCreateRowColumn(dw, tmpstr, NULL, 0));

	    (void)sprintf(tmpstr, "%sS", tag);
	    XtManageChild(XmCreateSeparatorGadget(dw, tmpstr, NULL, 0));

	    (void)sprintf(tmpstr, "%sH", tag);
	    XtManageChild(dwh = XmCreateLabel(dw, tmpstr, NULL, 0));

	    (void)sprintf(tmpstr, "%sT", tag);
	    XtManageChild(dwt = XmCreateText(dw, tmpstr, NULL, 0));

	    XtManageChild(closeW = XmCreatePushButtonGadget(rc, "close",
							    NULL, 0));
	    XtManageChild(jump = XmCreatePushButtonGadget(rc, "goto", NULL, 0));
	    XtManageChild(helpW = XmCreatePushButtonGadget(rc, "help",
							   NULL, 0));
	    XtAddCallback(closeW, XmNactivateCallback, datwCloseCB, dw);
	    XtAddCallback(jump, XmNactivateCallback, gotoCB, (XtPointer)(uintptr_t)cproc);
	    XtAddCallback(helpW, XmNactivateCallback, helpCB, dwt);
	    XtSetArg(arg, XmNinitialFocus, dwt);
	    XtSetValues(dw, &arg, 1);
	    XtSetArg(arg, XmNrows, &rows);
	    XtGetValues(dwt, &arg, 1);
	    datwtbl[i].size = rows;
	    XtSetArg(arg, XmNcolumns, i ? 80 : datColumns);
	    XtSetValues(dwt, &arg, 1);
	    datInfo.perProc[cproc].form[i] = dw;
	    datInfo.perProc[cproc].hdr[i]  = dwh;
	    datInfo.perProc[cproc].text[i] = dwt;
	}
	datwUpd(cproc, i);
    }
}

/*--------------------------------------------------------------------------
 * dataWindow$Update - Update the contents of all data windows which have
 *  been created.  It does not remap the windows.
 *--------------------------------------------------------------------------*/
void datwUpdateX(void)
{
    unsigned proc, i;

    for (proc = 0; proc < nproc; proc++)
	for (i = 0; i < topdatw; i++)
	    datwUpd(proc, i);
}


/***************************/
/* Command Window Routines */
/***************************/

void cmdwPrintX(const char *s)
{
    (void)fprintf(stderr, "%s", s);
}

void cmdwUpdateX(void)
{
    /* In tty version, sets cproc, implementation, and version info */
    /* Called only by cmdwInit and valAssign (= cproc) */
}

void msgwPrintX(const char *s)
{
    textAdd(msgs, (char *)s);
}

void cmdwSetStatusX(const char *msg)
{
    static BOOL getmsg = YES;
    static XmString defaultmsg;
    XmString cmdmsg, s;

    if (getmsg) {
	XtVaGetValues(cmd,
	    XmNpromptString, &defaultmsg,
	    NULL);
	s = XmStringCreateLocalized("  ");
	defaultmsg = XmStringConcat(defaultmsg, s);
	XmStringFree(s);
	getmsg = NO;
    }
    s = XmStringCreateLocalized((char *)msg);
    cmdmsg = XmStringConcat(defaultmsg, s);
    XtVaSetValues(cmd,
	XmNpromptString, cmdmsg,
	NULL);
    XmUpdateDisplay(cmd);
    XmStringFree(s);
    XmStringFree(cmdmsg);
}

void scrnEndX(void)
{
    /* Used by ssDCmd.c(cmdExit), ssDSD.c(ctrlz), ssErr.c(panic) */
}

static void listUpd(listWidget *wp, const char *hdr, const char *buf)
{
    if (wp->hdr) {
	XmTextSetString(wp->hdr, (char *)hdr);
	/* Delete final newline */
	XmTextReplace(wp->hdr, XmTextGetLastPosition(wp->hdr)-1,
		      XmTextGetLastPosition(wp->hdr), "");
    }
    XmTextSetString(wp->txt, (char *)buf);
    /* Delete final newline */
    XmTextReplace(wp->txt, XmTextGetLastPosition(wp->txt)-1,
		  XmTextGetLastPosition(wp->txt), "");
}

void cmdOutX(const char *name, const char *hdr, const char *buf)
{
    char rcname[20], sepname[20], hdrname[20], txtname[20];
    Widget sep, rc, closeW, helpW;
    Arg arg, args[15];
    int n;
    listWidget *wp;

    for (wp = listWidgets; wp; wp = wp->next)
	if (!strcmp(wp->name,name)) {
	    listUpd(wp, hdr, buf);
	    XtManageChild(wp->w);
	    return;
	}
    if (!(wp = malloc(sizeof *wp)))
	/* XXX - print something? */
	return;

    n = 0;
    XtSetArg(args[n], XmNdefaultPosition, False);		n++;
    wp->w = XmCreateFormDialog(cmd, (char *)name, args, n);

    (void)sprintf(rcname, "%sRC", name);
    n = 0;
    XtSetArg(args[n], XmNadjustLast, False);			n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);		n++;
    XtSetArg(args[n], XmNspacing, 10);				n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
    XtManageChild(rc = XmCreateRowColumn(wp->w, rcname, args, n));

    (void)sprintf(sepname, "%sS", name);
    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNbottomWidget, rc);			n++;
    XtManageChild(sep = XmCreateSeparatorGadget(wp->w, sepname, args, n));

    if (*hdr) {
	(void)sprintf(hdrname, "%sH", name);
	n = 0;
	XtSetArg(args[n], XmNeditable, False);			n++;
	XtSetArg(args[n], XmNautoShowCursorPosition, False);	n++;
	XtSetArg(args[n], XmNcursorPositionVisible, False);	n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);	n++;
	XtManageChild(wp->hdr = XmCreateTextField(wp->w, hdrname, args, n));
    } else
	wp->hdr = NULL;

    (void)sprintf(txtname, "%sT", name);
    n = 0;
    XtSetArg(args[n], XmNrows, 14);				n++;
    XtSetArg(args[n], XmNcolumns, 40);				n++;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);		n++;
    XtSetArg(args[n], XmNeditable, False);			n++;
    XtSetArg(args[n], XmNautoShowCursorPosition, False);	n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False);		n++;
    XtSetArg(args[n], XmNresizeWidth, False);			n++;
    XtSetArg(args[n], XmNscrollHorizontal, False);		n++;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED);	n++;
    XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC);		n++;
    XtManageChild(wp->txt = XmCreateScrolledText(wp->w, txtname, args, n));

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
    if (*hdr) {
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
	XtSetArg(args[n], XmNtopWidget, wp->hdr);		n++;
    } else {
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);	n++;
    }
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNbottomWidget, sep);			n++;
    XtSetValues(XtParent(wp->txt), args, n);

    XtManageChild(closeW = XmCreatePushButtonGadget(rc, "close", NULL, 0));
    XtManageChild(helpW = XmCreatePushButtonGadget(rc, "help", NULL, 0));
    XtAddCallback(closeW, XmNactivateCallback, closeCB, wp->w);
    XtAddCallback(helpW, XmNactivateCallback, helpCB, wp->txt);
    XtSetArg(arg, XmNinitialFocus, XtParent(wp->txt));
    XtSetValues(wp->w, &arg, 1);

    (void)strcpy(wp->name,name);
    wp->next = listWidgets;
    listWidgets = wp;
    listUpd(wp, hdr, buf);
    XtManageChild(wp->w);
}

#endif /* HAVE_MOTIF */
