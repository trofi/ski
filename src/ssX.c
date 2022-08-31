/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator X11/Motif Interface
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

#include "ssX.h"

#include <stdint.h>
#include <Xm/Xm.h>
#include <Xm/CascadeB.h>
#include <Xm/Command.h>
#include <Xm/FileSB.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/MessageB.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#ifdef ICON_DEFINED
#include "icon.h"
#endif
#include "types.h"
#include "ssDCmd.h"
#include "libcore.h"
#include "ski.h"
#include "ssXDSD.h"
#include "ui.h"

#define SKI_NPROC 1 /* TODO: use program.h definition */

#define FIXED_RSRCS	4
#define FIXED_OPTIONS	6
#define REFRESH_RATE	100000		/* ~1 sec wait */
#define CALLBACK_ARGS	Widget w, XtPointer client_data, XtPointer call_data


/*##################### Globals - Imports ##################################*/

extern BOOL noscreen;

static void cmdHelpCallback(CALLBACK_ARGS);
static void prodInfoCallback(CALLBACK_ARGS);

/*##################### Globals - Exports ##################################*/

unsigned nproc = 1, cproc = 0;

/*##################### Local Variables ####################################*/

static XtAppContext app_context;
static Widget stopsim;
static XtWorkProcId stopsimWP;
static BOOL firstTime, stopPressed;
static CTR stepcnt, tmpcnt, execcnt;
static char stepexpr[80];
static REG stepval;

typedef struct {
    unsigned numButtons, refreshRate;
    Boolean viewRegs, viewProg;
    Boolean viewDw[WINTBLSIZ];
    Boolean showRs[WINTBLSIZ];
} AppData;

static AppData app_data;

static XtResource resources[FIXED_RSRCS + 2*WINTBLSIZ] = {
    { "numButtons", "NumButtons", XmRInt, sizeof(unsigned),
      XtOffsetOf(AppData, numButtons), XmRImmediate, (XtPointer)8 },
    { "refreshRate", "RefreshRate", XmRInt, sizeof(unsigned),
      XtOffsetOf(AppData, refreshRate), XmRImmediate, (XtPointer)REFRESH_RATE },
    { "viewRegisters", "ViewRegisters", XmRBoolean, sizeof(Boolean),
      XtOffsetOf(AppData, viewRegs), XmRImmediate, (XtPointer)False },
    { "viewProgram", "ViewProgram", XmRBoolean, sizeof(Boolean),
      XtOffsetOf(AppData, viewProg), XmRImmediate, (XtPointer)False }
};
static unsigned toprsrc = FIXED_RSRCS;

static XrmOptionDescRec options[FIXED_OPTIONS + 2*2*WINTBLSIZ] = {
    { "-numButtons",	".numButtons",	XrmoptionSepArg,	NULL },
    { "-refreshRate",	".refreshRate",	XrmoptionSepArg,	NULL },
    { "-viewRegisters",	".viewRegisters",	XrmoptionNoArg,	"True" },
    { "+viewRegisters",	".viewRegisters",	XrmoptionNoArg,	"False" },
    { "-viewProgram",	".viewProgram",	XrmoptionNoArg,		"True" },
    { "+viewProgram",	".viewProgram",	XrmoptionNoArg,		"False" }
};
static unsigned topopts = FIXED_OPTIONS;

static Display *dpy;
Widget cmd, msgs;
static Widget cmdHist;

static void addRegwRsrcOpts(void);
static void addDatwRsrcOpts(void);
static Widget createMBitem(Widget parent, char *name);
static void createSubmenuItem(Widget parent, char *name, XtCallbackProc p);
static void changeCprocCallback(CALLBACK_ARGS);
static void simCmdCallback(CALLBACK_ARGS);
static void programCallback(CALLBACK_ARGS);
static void dataCallback(CALLBACK_ARGS);
static void regCallback(CALLBACK_ARGS);
static void sdtCallback(CALLBACK_ARGS);
static void cstCallback(CALLBACK_ARGS);
static void blistCallback(CALLBACK_ARGS);
static void symlistCallback(CALLBACK_ARGS);
static void SimCmd(Widget w, XEvent *event,
		   String *params, Cardinal *num_params);

static Boolean stepUntilX(XtPointer client_data);
static Boolean stepItX(XtPointer client_data);
static Boolean runItX(XtPointer client_data);
static void stop_execLoopXCB(CALLBACK_ARGS);

static XtActionsRec actions[] = {
    { "SimCmd", SimCmd },
    { "dwResize", dwResize },
    { "pwResize", pwResize }
};
static char prevCmdStr[100] = {0};

/*##################### Functions and Subroutines ##########################*/

void scrnInitX(void)
{
    Widget toplevel, control;
    Widget mb, fileMenu, viewMenu, confMenu, helpMenu;
    Widget procFrame = NULL, bboxFrame = NULL;
#ifdef ICON_DEFINED
    Pixmap icon;
#endif
    XmString s;
    Arg args[10];
    unsigned i, n;
    int argc = 0;

    addRegwRsrcOpts();
    addDatwRsrcOpts();
    toplevel = XtAppInitialize(&app_context, "XSki",
			       options, topopts, &argc, NULL,
			       NULL, NULL, 0);
    XtGetApplicationResources(toplevel, &app_data,
			      resources, toprsrc, NULL, 0);
    XtAppAddActions(app_context, actions, XtNumber(actions));
    dpy = XtDisplay(toplevel);

#ifdef ICON_DEFINED
    /* set default icon */
    icon = XCreateBitmapFromData(dpy, RootWindowOfScreen(XtScreen(toplevel)),
				 icon_bits, icon_width, icon_height);
    XtSetArg(args[0], XmNiconPixmap, icon);
    XtSetValues(toplevel, args, 1);
#endif

    /* create control window manager */
    control = XmCreateForm(toplevel, "control", NULL, 0);
    XtManageChild(control);

    /* Control window menubar */
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
    mb = XmCreateMenuBar(control, "mb", args, n);
    XtManageChild(mb);
    fileMenu = createMBitem(mb, "File");
    viewMenu = createMBitem(mb, "View");
    confMenu = createMBitem(mb, "Configure");
    helpMenu = createMBitem(mb, "Help");

    createSubmenuItem(fileMenu, "load", 0);
    createSubmenuItem(fileMenu, "save", 0);
    createSubmenuItem(fileMenu, "trace", 0);
    createSubmenuItem(fileMenu, "traceOn", 0);
    createSubmenuItem(fileMenu, "traceOff", 0);
    createSubmenuItem(fileMenu, "read", 0);
    XtManageChild(XmCreateSeparatorGadget(fileMenu, "", NULL, 0));
    createSubmenuItem(fileMenu, "quit", simCmdCallback);

    createSubmenuItem(viewMenu, "program", programCallback);
    createSubmenuItem(viewMenu, "data", dataCallback);
    createSubmenuItem(viewMenu, "registers", regCallback);
    createSubmenuItem(viewMenu, "cache", 0);
    createSubmenuItem(viewMenu, "tlb", sdtCallback);
    XtManageChild(XmCreateSeparatorGadget(viewMenu, "", NULL, 0));
    createSubmenuItem(viewMenu, "callstack", cstCallback);
    createSubmenuItem(viewMenu, "statistics", 0);
    createSubmenuItem(viewMenu, "breakpoints", blistCallback);
    createSubmenuItem(viewMenu, "branches", 0);
    createSubmenuItem(viewMenu, "symbols", symlistCallback);
    createSubmenuItem(viewMenu, "macros", 0);

    createSubmenuItem(confMenu, "program", 0);

    createSubmenuItem(confMenu, "data", 0);

    createSubmenuItem(confMenu, "registers", 0);

    createSubmenuItem(helpMenu, "context", 0);
    createSubmenuItem(helpMenu, "overview", 0);
    createSubmenuItem(helpMenu, "commands", cmdHelpCallback);
    createSubmenuItem(helpMenu, "product", prodInfoCallback);

    /* Control window processor option menu */
    if (nproc > 1) {
	Widget procMenu, proc[SKI_NPROC];

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
	XtSetArg(args[n], XmNtopWidget, mb);			n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
	procFrame = XmCreateFrame(control, "", args, n);
	XtManageChild(procFrame);

	procMenu = XmCreatePulldownMenu(procFrame, "", NULL, 0);
	for (i = 0; i < nproc; i++) {
	    char name[4];

	    (void)sprintf(name, "p%d", i);
	    proc[i] = XmCreatePushButton(procMenu, name, NULL, 0);
	    XtAddCallback(proc[i], XmNactivateCallback,
			  changeCprocCallback, (XtPointer)(uintptr_t)i);
	}
	XtManageChildren(proc, nproc);
	n = 0;
	XtSetArg(args[n], XmNsubMenuId, procMenu);		n++;
	XtManageChild(XmCreateOptionMenu(procFrame, "procOption", args, n));
    }

    /* Control window button box */
    if (app_data.numButtons) {
	Widget bbox, btns[20];

	/* create button box manager */
	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
	XtSetArg(args[n], XmNtopWidget, procFrame ? procFrame : mb);	n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
	bboxFrame = XmCreateFrame(control, "", args, n);
	XtManageChild(bboxFrame);

	/* create button box window */
	bbox = XmCreateRowColumn(bboxFrame, "bbox", NULL, 0);
	XtManageChild(bbox);
	/* create each button */
	for (i = 0; i < app_data.numButtons; i++) {
	    char name[8];

	    (void)sprintf(name, "bb%d", i);
	    btns[i] = XmCreatePushButton(bbox, name, NULL, 0);
	}
	XtManageChildren(btns, app_data.numButtons);
    }

    /* Control window command area */
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
    XtSetArg(args[n], XmNtopWidget,
	     bboxFrame ? bboxFrame : procFrame ? procFrame : mb);	n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
    cmd = XmCreateCommand(control, "cmd", args, n);
    XtManageChild(cmd);
    XtAddCallback(cmd, XmNcommandEnteredCallback, simCmdCallback, 0);

    /* command history box */
    n = 0;
    XtSetArg(args[0], XmNinitialFocus, cmd);	n++;
    XtSetValues(control, args, n);
    cmdHist = XmCommandGetChild(cmd, XmDIALOG_HISTORY_LIST);

    /* message window */
    msgs = XmCreateScrolledText(control, "msgs", NULL, 0);
    XtManageChild(msgs);
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
    XtSetArg(args[n], XmNtopWidget, cmd);				n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
    XtSetValues(XtParent(msgs), args, n);

    /* create stop simulation dialog */
	/* Consider changing this to a button.  This would allow other
	   functionality to still be active when simulation is going on.
	   For example, Help, Quit, opening and closing windows, etc. */
    stopsim = XmCreateWorkingDialog(control, "stopsim", NULL, 0);
    n = 0;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);	n++;
    s = XmStringCreateLocalized("Running...");
    XtSetArg(args[n], XmNdialogTitle, s);				n++;
    s = XmStringCreateLocalized(" icnt: 0 ");
    XtSetArg(args[n], XmNmessageString, s);				n++;
    s = XmStringCreateLocalized("Stop");
    XtSetArg(args[n], XmNcancelLabelString, s);				n++;
    XtSetValues(stopsim, args, n);
    XtAddCallback(stopsim, XmNcancelCallback, stop_execLoopXCB, NULL);
    XtUnmanageChild(XmMessageBoxGetChild(stopsim, XmDIALOG_OK_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(stopsim, XmDIALOG_HELP_BUTTON));
    XmStringFree(s);

    /* XXX - This should probably be inside regwInit */
    for (i = 0; i < topregw; i++)
	regwtbl[i].show = app_data.showRs[i];

    /* XXX - This should probably be inside datwInit */
    for (i = 0; i < topdatw; i++)
	datwtbl[i].show = app_data.viewDw[i];

    prgwInit();
    datwInit();
    regwInit();
    if (app_data.viewProg)
	prgwDrawX();
    datwDrawX();
    if (app_data.viewRegs)
	regwDrawX();

    noscreen = NO;
    XtRealizeWidget(toplevel);
}

static void addRegwRsrcOpts(void)
{
    static char rsrc[EXPRSIZ+4][WINTBLSIZ];
    static char opt[EXPRSIZ+5][2*WINTBLSIZ];
    static char spec[EXPRSIZ+5][WINTBLSIZ];
    unsigned i;

    for (i = 0; i < topregw; i++) {
	sprintf(rsrc[i],    "show%s",  regwtbl[i].tag);
	sprintf(opt[2*i],   "-show%s", regwtbl[i].tag);
	sprintf(opt[2*i+1], "+show%s", regwtbl[i].tag);
	sprintf(spec[i],    ".show%s", regwtbl[i].tag);

	resources[toprsrc].resource_name   = rsrc[i];
	resources[toprsrc].resource_class  = "ShowRegs";
	resources[toprsrc].resource_type   = XmRBoolean;
	resources[toprsrc].resource_size   = sizeof(Boolean);
	resources[toprsrc].resource_offset = XtOffsetOf(AppData, showRs[0]) + i*sizeof(Boolean);
	resources[toprsrc].default_type    = XmRImmediate;
	resources[toprsrc].default_addr    = (XtPointer)True;
	toprsrc++;

	options[topopts].option    = opt[2*i];
	options[topopts].specifier = spec[i];
	options[topopts].argKind   = XrmoptionNoArg;
	options[topopts].value     = "True";
	topopts++;

	options[topopts].option    = opt[2*i+1];
	options[topopts].specifier = spec[i];
	options[topopts].argKind   = XrmoptionNoArg;
	options[topopts].value     = "False";
	topopts++;
    }
}

static void addDatwRsrcOpts(void)
{
    static char rsrc[EXPRSIZ+4][WINTBLSIZ];
    static char opt[EXPRSIZ+5][2*WINTBLSIZ];
    static char spec[EXPRSIZ+5][WINTBLSIZ];
    unsigned i;

    for (i = 0; i < topdatw; i++) {
	sprintf(rsrc[i],    "view%s",  datwtbl[i].tag);
	sprintf(opt[2*i],   "-view%s", datwtbl[i].tag);
	sprintf(opt[2*i+1], "+view%s", datwtbl[i].tag);
	sprintf(spec[i],    ".view%s", datwtbl[i].tag);

	resources[toprsrc].resource_name   = rsrc[i];
	resources[toprsrc].resource_class  = "ViewData";
	resources[toprsrc].resource_type   = XmRBoolean;
	resources[toprsrc].resource_size   = sizeof(Boolean);
	resources[toprsrc].resource_offset = XtOffsetOf(AppData, viewDw[0]) + i*sizeof(Boolean);
	resources[toprsrc].default_type    = XmRImmediate;
	resources[toprsrc].default_addr    = (XtPointer)True;
	toprsrc++;

	options[topopts].option    = opt[2*i];
	options[topopts].specifier = spec[i];
	options[topopts].argKind   = XrmoptionNoArg;
	options[topopts].value     = "True";
	topopts++;

	options[topopts].option    = opt[2*i+1];
	options[topopts].specifier = spec[i];
	options[topopts].argKind   = XrmoptionNoArg;
	options[topopts].value     = "False";
	topopts++;
    }
}
static Widget createMBitem(Widget parent, char *name)
{
    Widget menu, btn;
    Arg arg;

    menu = XmCreatePulldownMenu(parent, "", NULL, 0);
    XtSetArg(arg, XmNsubMenuId, menu);
    btn = XmCreateCascadeButton(parent, name, &arg, 1);
    XtManageChild(btn);
    if (!strcmp(name, "Help")) {
	XtSetArg(arg, XmNmenuHelpWidget, btn);
	XtSetValues(parent, &arg, 1);
    }
    return menu;
}

static void createSubmenuItem(Widget parent, char *name, XtCallbackProc p)
{
    Widget btn;

    btn = XmCreatePushButton(parent, name, NULL, 0);
    XtManageChild(btn);
    if (p)
	XtAddCallback(btn, XmNactivateCallback, p, (XtPointer)name);
    else
	XtSetSensitive(btn, False);
}

/*ARGSUSED*/
static void changeCprocCallback(CALLBACK_ARGS)
{
    cproc = (unsigned)(uintptr_t)client_data;
    (void)XmProcessTraversal(cmd, XmTRAVERSE_CURRENT);
}

/*ARGSUSED*/
static void simCmdCallback(CALLBACK_ARGS)
{
    char s[100];

    if (client_data) {
	XmString xms = XmStringCreateLocalized((char *)client_data);

	XmListAddItemUnselected(cmdHist, xms, 0);
	XmListSetBottomPos(cmdHist, 0);
	XmStringFree(xms);
	(void)strcpy(prevCmdStr, (char*)client_data);
	(void)sprintf(s, "%s\n", (char *)client_data);
    } else {
	XmCommandCallbackStruct *cbs = (XmCommandCallbackStruct *)call_data;
	char *cmdStr;

	(void)XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &cmdStr);
	if (cmdStr[0] == 'X') {
	    XmString msg = XmStringCreateLocalized("Bad command");

	    XmCommandError(cmd, msg);
	    XmStringFree(msg);
	}
	if (cmdStr[0] == '\0' && prevCmdStr != '\0') {
	    (void)strcpy(cmdStr, prevCmdStr);
	    XmStringFree(cbs->value);
	    cbs->value = XmStringCreateLocalized(cmdStr);
	}
	(void)strcpy(prevCmdStr, cmdStr);
	(void)sprintf(s, "%s\n", cmdStr);
    }
    if (!cmdExLin(s)) {
	XmString msg = XmStringCreateLocalized("Bad command");

	XmCommandError(cmd, msg);
	XmStringFree(msg);
    }
    (void)XmProcessTraversal(cmd, XmTRAVERSE_CURRENT);
}

static void programCallback(CALLBACK_ARGS)
{
    prgwDrawX();
}

static void dataCallback(CALLBACK_ARGS)
{
    (void)datwMakeActivew("Data");
    datwDrawX();
}

static void regCallback(CALLBACK_ARGS)
{
    regwDrawX();
}

static void sdtCallback(CALLBACK_ARGS)
{
    (void)cmdExLin("sdt");
}

static void cstCallback(CALLBACK_ARGS)
{
    (void)cmdExLin("cst");
}

static void blistCallback(CALLBACK_ARGS)
{
    (void)cmdExLin("bl");
}

static void symlistCallback(CALLBACK_ARGS)
{
    (void)cmdExLin("symlist");
}

void cmdHelpCallback(CALLBACK_ARGS)
{
    (void)cmdExLin("help");
}

void prodInfoCallback(CALLBACK_ARGS)
{
    static Widget dialog = NULL;

    if (!dialog) {
	XmString xms;
	Arg arg;

	xms = XmStringCreateLtoR(productInfo(), XmFONTLIST_DEFAULT_TAG);
	XtSetArg(arg, XmNmessageString, xms);
	dialog = XmCreateInformationDialog(cmd, "prodInfo", &arg, 1);
	XmStringFree(xms);
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
    }
    XtManageChild(dialog);
}

static void displayICnt(CTR delta, BOOL *flag)
{
    char msg[80];
    XmString s;

    execcnt += delta;
    (void)sprintf(msg, " icnt: %llu ", execcnt);
    s = XmStringCreateLocalized(msg);
    XtVaSetValues(stopsim, XmNmessageString, s, NULL);
    if (*flag) {
	XtManageChild(stopsim);
	*flag = NO;
    }
}

void stepUntil_setupX(const char *expr)
{
    (void)strcpy(stepexpr, expr);
    firstTime = YES;
    stopsimWP = XtAppAddWorkProc(app_context, stepUntilX, (XtPointer)NULL);
    stopPressed = NO;
    execcnt = 0;
}

static Boolean stepUntilX(XtPointer client_data)
{
    if (!stopPressed && stepUntil_loopX(app_data.refreshRate/100, stepval, stepexpr)) {
	displayICnt(app_data.refreshRate/100, &firstTime);
	return False;               /* call work procedure again */
    }
    cleanup_execLoop(tmpcnt != 1);
    XtUnmanageChild(stopsim);
    XtRemoveWorkProc(stopsimWP);
    return True;                        /* don't call work proc again */
}

void stepIt_setupX(CTR cnt)
{
    stepcnt = tmpcnt = cnt;
    firstTime = YES;
    stopsimWP = XtAppAddWorkProc(app_context, stepItX, (XtPointer)NULL);
    stopPressed = NO;
    execcnt = 0;
}

static Boolean stepItX(XtPointer client_data)
{
    if (!stopPressed && stepcnt > app_data.refreshRate) {
	if (stepIt_loop(app_data.refreshRate)) {
	    stepcnt -= app_data.refreshRate;
	    displayICnt(app_data.refreshRate, &firstTime);
	    return False;		/* call work procedure again */
	}
    } else if (!stopPressed && !stepIt_loop(stepcnt))
	return False;
    cleanup_execLoop(tmpcnt != 1);
    XtUnmanageChild(stopsim);
    XtRemoveWorkProc(stopsimWP);
    return True;                        /* don't call work proc again */
}

void runIt_setupX(void)
{
    firstTime = YES;
    stopsimWP = XtAppAddWorkProc(app_context, runItX, (XtPointer)NULL);
    stopPressed = NO;
    execcnt = 0;
}

static Boolean runItX(XtPointer client_data)
{
    if (!stopPressed && runIt_loopX(app_data.refreshRate)) {
	displayICnt(app_data.refreshRate, &firstTime);
	return False;			/* call work procedure again */
    }
    cleanup_execLoop(YES);
    XtUnmanageChild(stopsim);
    XtRemoveWorkProc(stopsimWP);
    return True;			/* don't call work proc again */
}

static void stop_execLoopXCB(CALLBACK_ARGS)
{
    stopPressed = YES;
}

static void SimCmd(Widget w, XEvent *event,
		   String *params, Cardinal *num_params)
{
    XmString xms;
    char s[100];

    xms = XmStringCreateLocalized(*params);
    XmListAddItemUnselected(cmdHist, xms, 0);
    XmListSetBottomPos(cmdHist, 0);
    XmStringFree(xms);
    (void)strcpy(prevCmdStr, *params);
    (void)sprintf(s, "%s\n", *params);
    (void)cmdExLin(s);
}

BOOL keepXLoopAlive = NO;

void cmdLoopX(void)
{
    /* eclipse - replace XtAppMainLoop so that we can temporarily    */
    /* disable event processing (when we return from emulated code), */
    /* and reenable the ui when we enter emulated code again         */
    /* XtAppMainLoop(app_context); */

    XEvent Event;
    while( keepXLoopAlive || !getExited() ) {
	// Get event
	XtAppNextEvent(app_context, &Event);
	// Send non-intercepted events to their respective widgets.
	XtDispatchEvent(&Event);
    }
}

#else /* !HAVE_MOTIF */

unsigned cproc = 0;

#endif /* !HAVE_MOTIF */
