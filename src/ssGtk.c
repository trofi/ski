/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Gtk Interface to SKI
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
#include "ssGtk.h"

#include "std.h"

#ifdef HAVE_GTK

#include <gtk/gtk.h>
#include <glade/glade.h>

#include <string.h>
#include <assert.h>

#include "types.h"
#include "coreui.h"
#include "ssDCmd.h"
#include "libcore.h"
#include "state.h"
#include "program.h"
#include "data.h"
#include "load.h"
#include "ssDPrs.h"

/* cycles to do in one background loop */
#define GTK_REFRESH_RATE 100000

/* elements kept in command history combo */
#define GTK_CMD_HISTORY 10

/* file globals */
static GladeXML *xml;

/* both these from ssX.c, can't define again if we build it in */
#if HAVE_MOTIF
extern unsigned nproc;
extern unsigned cproc;
#else
unsigned nproc;
unsigned cproc;
#endif

/* program window */
static GtkTextView *prgw_view;
static GtkTextBuffer *prgw_buffer;
static GtkLabel *prgw_label;

/* data window */
static GtkTextView *datw_view;
static GtkTextBuffer *datw_buffer;

/* output list view */
static GtkTreeView *output_view;
static GtkListStore *output_store;
static GtkTreeIter output_iter;


/* for updating the status bar */
static GtkStatusbar* status_bar_widget;
static guint status_bar_context;

/* for running */
static BOOL stopPressed;
static CTR execcnt;

static GtkLabel* instcount_label;
/* sits in a background loop */
static gboolean runItGtk(gpointer data)
{
	char label[200];
	if (!stopPressed && runIt_loopX(GTK_REFRESH_RATE)) {
		execcnt += GTK_REFRESH_RATE;
		sprintf(label, "%llu cycles\n", execcnt);
		gtk_label_set_text(instcount_label, label);
		// run again
		return TRUE;
	}
	cleanup_execLoop(YES);
	return FALSE;
}


void runIt_setupGtk(void)
{
	g_idle_add(runItGtk, NULL);
	stopPressed = NO;
	execcnt = 0;
	/* draw the window */
	gtk_widget_show_all(glade_xml_get_widget(xml, "running_window"));
}

static CTR tostep;

static int stepItGtk(void *data)
{
	char label[200];
	if (tostep <= GTK_REFRESH_RATE)
	{
		stepIt_loop(tostep);
		goto out;
	}
	if (!stopPressed && !stepIt_loop(GTK_REFRESH_RATE)) {
		execcnt += GTK_REFRESH_RATE;
		tostep -= GTK_REFRESH_RATE;
		sprintf(label, "%llu cycles\n", execcnt);
		gtk_label_set_text(instcount_label, label);
		// run again
		return TRUE;
	}
 out:
	cleanup_execLoop(YES);
	return FALSE;
}

void stepIt_setupGtk(CTR cnt)
{
	printf("*** stepIt_setupGtk -> step %llu\n", cnt);
	tostep = cnt;
	g_idle_add(stepItGtk, NULL);
	stopPressed = NO;
	execcnt = 0;
	/* only show the window if we are going to go for a while */
	if (tostep > GTK_REFRESH_RATE)
		gtk_widget_show_all(glade_xml_get_widget(xml, "running_window"));
}

static void stop_simulator(void)
{
	stopPressed = YES;
	gtk_widget_hide(glade_xml_get_widget(xml, "running_window"));
}

/* commands to keep in history */
static GtkComboBox *cmd_combo_box;
static char *cmd_history[GTK_CMD_HISTORY];
static int cmd_history_len;
static void add_to_cmd_history(const char *s)
{
	char *new_str = malloc(strlen(s) + 1);
	if (!new_str) return;
	strcpy(new_str, s);
	if (cmd_history_len < GTK_CMD_HISTORY) {
		/* To make glade make a combo box suitable for using
		   the [add|remove]_text functions on you need to put
		   in a dummy item.  This removes the dummy item on the
		   first command. */
		if (cmd_history_len == 0)
			gtk_combo_box_remove_text(cmd_combo_box, 0);

		cmd_history[cmd_history_len] = new_str;
		gtk_combo_box_insert_text(cmd_combo_box, cmd_history_len,
					  new_str);
		cmd_history_len++;
		return;
	}
	/* free last, shuffle down others, add new */
	int i;
	free(cmd_history[GTK_CMD_HISTORY-1]);
	for (i = GTK_CMD_HISTORY-1; i > 0; i--) {
		cmd_history[i] = cmd_history[i-1];
		gtk_combo_box_remove_text(cmd_combo_box, i);
		gtk_combo_box_insert_text(cmd_combo_box,
					  i, cmd_history[i]);
	}
	cmd_history[0] = new_str;
	gtk_combo_box_remove_text(cmd_combo_box, 0);
	gtk_combo_box_insert_text(cmd_combo_box,
				  0, cmd_history[0]);
}

/* Little helper to fix the font of a newly written to buffer.
 * Assumes tag has already been setup */
static void fixed_font_buffer(GtkTextBuffer *buf)
{
	GtkTextIter start, end;
	gtk_text_buffer_get_bounds(buf, &start, &end);
	gtk_text_buffer_apply_tag_by_name(buf, "font", &start, &end);
}

/* Handle a command entry from the combo box */
void SimComboCmdGtk(void *data)
{
	const char *cmd = gtk_entry_get_text((GtkEntry*)data);
	printf("*** SimComboCmdGtk()\n");
	printf("    -> cmd %s\n", cmd);
	cmdExLin(cmd);
	add_to_cmd_history(cmd);
	gtk_entry_set_text((GtkEntry*)data, "");
}


/* Handle a button click.  The name of the button is the command to
 * run. */
void SimBtnCmdGtk(void *data)
{
	const gchar *btn = gtk_widget_get_name(GTK_WIDGET(data));

	// cmd to pass to simulator
	const char *cmd = NULL;
	printf("*** SimCmdGtk() \n");
	printf("    -> %s\n", btn);

	if (!strcmp(btn, "btnRun"))
		cmd = "run";
	else if (!strcmp(btn, "btnStep"))
		cmd = "step";
	else if (!strcmp(btn, "btnRunningStop")) {
		stop_simulator();
		return;
	}
	else
		printf("    -> bad command %s???\n", btn);

	add_to_cmd_history(cmd);
	if (!cmdExLin(cmd)) {
		printf("    -> bad command!\n");
	}

}

/* Show the about box */
void SimShowAbout(void *data)
{
	printf("*** SimShowAbout()\n");
	GtkWidget *about = glade_xml_get_widget(xml, "about_dialog");
	assert(about);

	gtk_widget_show_all(about);
}

static void update_command_status(const char *s)
{
	/* remove whatever was there */
	gtk_statusbar_pop(status_bar_widget, status_bar_context);
	gtk_statusbar_push(status_bar_widget, status_bar_context, s);
}

void msgwPrintGtk(const char *s)
{
	printf("*** msgwPrintGtk (%s)\n", s);
	/* this sucks, but we really don't want the newline on the end */
	char *buf;
	int len = strlen(s);
	buf = malloc(len+1);
	assert(buf);
	strncpy(buf, s, len);
	buf[len-1] = '\0';
	gtk_list_store_append(output_store, &output_iter);
	gtk_list_store_set(output_store, &output_iter, 0, buf, -1);
	gtk_tree_view_scroll_to_cell(output_view,
				     gtk_tree_model_get_path(GTK_TREE_MODEL(output_store), &output_iter),
				     NULL, TRUE, 1, 0);
	free(buf);
}

static void datwInit(void)
{
    unsigned i, save = cproc;

    for (cproc = 0; cproc < nproc; cproc++) {
        datwCVA = dataStart;
        datwLVA = 0;
        for (i = 0; i < WINTBLSIZ; i++)
            datInfo.perProc[cproc].form[i] = 0;
    }

    cproc = save;
    datInfo.numDataWds = 4;
    datInfo.columns = 67;       /* 67 to include ASCII */
    datInfo.expr[0] = '\0';

    datw_view = GTK_TEXT_VIEW(glade_xml_get_widget(xml, "textviewDatw"));
    assert(datw_view);
    datw_buffer = gtk_text_view_get_buffer(datw_view);
    assert(datw_buffer);
    gtk_text_buffer_create_tag(datw_buffer, "font", "font", "fixed", NULL);
}


static void prgwInit(void)
{
	/* program window stuff */
	for (cproc = 0; cproc < 1; cproc++) {
		prgwTop = ipGet(0);
		prgwIS = 0;
		prgwPrevTop = 0;
		prgCmd = PrgUseTop;
		prgInfo.perProc[cproc].form = 0;
	}
	prgInfo.iofsHex = YES;
	prgInfo.iofsSymWidth = 13;
	prgInfo.instHex = YES;
	prgInfo.instSymWidth = 45;
	prgInfo.columns = 88;


	prgw_label = GTK_LABEL(glade_xml_get_widget(xml, "labelPrgw"));
	assert(prgw_label);
	prgw_view = GTK_TEXT_VIEW(glade_xml_get_widget(xml, "textviewPrgw"));
	assert(prgw_view);
	prgw_buffer = gtk_text_view_get_buffer(prgw_view);
	assert(prgw_buffer);
	gtk_text_buffer_create_tag(prgw_buffer, "font", "font", "fixed", NULL);
}

void prgwUpdateGtk(void)
{
	printf("*** prgwUpdateGtk()\n");
	ADDR adr;
	unsigned i, j, srcrows, asmrows, nbytes;
	char *p, *text;

	char *output = malloc(80*20*2);
	int output_len = 0;
	assert(output);

	prgRows = 20;
	switch (prgCmd) {
	case PrgUseTop:
		adr = prgwTop;
		break;
	case PrgUseBot:
		adr = prgwBot;
		break;
	default:
		adr = prgwCVA;
		if (adr >= prgwTop && adr <= prgwBot)	/* adr is in view */
			adr = prgwTop;
		else if (prgwIS)	/* iA */
			adr = iAinstAddr(adr, prgwIAmode, -prgOverlap);
		else			/* EM */
			adr -= prgOverlap*16;
		break;
	}

	if (prgwIS) {		/* iA */
		for (prgwTop = adr, i = 0; i < prgRows; i++, adr += nbytes) {
			text = prgwLineIA(adr, prgwIAmode, YES, &nbytes);
			output_len += sprintf(output+output_len, "%s", text);
		}
		prgBndl = prgRows;
		prgwBot = adr - nbytes;
	} else {		/* EM */
		if (prgCmd != PrgUseBot) {
			for (prgwTop = adr, i = prgRows, prgBndl = 0; i > 0; ) {
				text = prgwLine(adr, &srcrows, &asmrows);
				if (i >= (srcrows + asmrows)) {
					i -= srcrows + asmrows;
					output_len += sprintf(output+output_len, "%s", text);
					adr += 16;
					prgBndl++;
				} else {	/* fill in remaining lines with source */
					for (p = text, j = 0; j < i && j < srcrows; j++)
						p = strchr(p, '\n') + 1;
					*p = '\0';
					output_len += sprintf(output+output_len, "%s", text);
					i = 0;
				}
			}
			prgwBot = adr - 16;
		} else {		/* PrgUseBot */
			for (i = prgRows, prgBndl = 0; i > 0; ) {
				text = prgwLine(adr, &srcrows, &asmrows);
				if (i >= (srcrows + asmrows)) {
					i -= srcrows + asmrows;
					sprintf(output+output_len, "%s", text);
					adr -= 16;
					prgBndl++;
				} else {
					if (i >= asmrows) {
						for (p = text, j = 0; j < (srcrows+asmrows) - i; j++)
							p = strchr(p, '\n') + 1;
						output_len += sprintf(output+output_len, "%s", text);
						i = 0;
					} else
						for (; i > 0; i--)
								  ; // XXX
				}
			}
			prgwTop = adr + 16;
		}
	}

	if (strlen(p = getSrcName(prgwTop))) {
		char buf[200];
		snprintf(buf, 200, "Program : %s", p);
		gtk_label_set_text(prgw_label, buf);
	} else
		gtk_label_set_text(prgw_label, "Program Window");

	gtk_text_buffer_set_text(prgw_buffer, output, -1);
	fixed_font_buffer(prgw_buffer);

	prgCmd = PrgUseTop;
}

static void fake_tlb_update(void);

static void regwDrawGtk(void)
{
	int i;
	REG size;
	static BOOL first_run = YES;

	char name[] = "textXrs";
	for (i = 0; i < topregw; i++)
	{
		evalExpr(regwtbl[i].size, DECEXP, &size);

		/* this might be a bit fragile, it relies on
		   regwtbl[i].tag being a certain thing and nobody
		   changing the textbox names via glade. */
		name[4] = regwtbl[i].tag[0];
		GtkTextView *tvw =
			(GtkTextView*)glade_xml_get_widget(xml, name);
		assert(tvw);

		GtkTextBuffer* tb = gtk_text_view_get_buffer(tvw);
		assert(tb);

		/* size is lines, lines shouldn't be longer than this */
		char *buf = malloc(size * 100);
		assert(buf);

		int j,len=0;
		for (j = 0; j < size; j++) {
			len += sprintf(buf+len, "%s\n", regwtbl[i].fcn(j));
		}
		buf[len+1] = '\0';
		gtk_text_buffer_set_text(tb, buf, -1);
		free(buf);
		/* only need to set this tag first time around */
		if (first_run)
			gtk_text_buffer_create_tag(tb, "font", "font", "fixed", NULL);
		fixed_font_buffer(tb);
	}
	first_run = NO;

	/* "fake" updates of TLB so we always see fresh windows */
	fake_tlb_update();
}

void regwUpdateGtk(void)
{
	printf("*** regwUpdateGtk()\n");
	regwDrawGtk();
}

void cmdwPrintGtk(const char *s)
{
	printf("*** cmdwPrintGtk() (%s)\n", s);
}

void cmdwUpdateGtk(void)
{
	printf("*** cmdwUpdateGtk()\n");
}

void cmdwSetStatusGtk(const char *s)
{
	printf("*** cmdwSetStatusGtk() (%s)\n", s);
	update_command_status(s);
}

void prgwDrawGtk(void)
{
	printf("*** prgwDraw()\n");
	prgwUpdateGtk();
}

static void tlbwUpdate(char win, const char *hdr, const char *buf);

/* hack derived from "sit/sdt" in menu.c */
static void fake_tlb_update(void)
{
	printf("*** fake_tlb_update()\n");
	size_t bufsz;
	char *buf, hdr[81];

	bufsz = instTlbDisplaySize();
	buf = malloc(bufsz+1);
	assert(buf);
	getInstTlbInfo(hdr, sizeof (hdr), buf, bufsz+1);
	tlbwUpdate('I', hdr, buf);
	free(buf);

	bufsz = dataTlbDisplaySize();
	buf = malloc(bufsz+1);
	assert(buf);
	getDataTlbInfo(hdr, sizeof (hdr), buf, bufsz+1);
	tlbwUpdate('D', hdr, buf);
	free(buf);
}

static void tlbwUpdate(char win, const char *hdr, const char *buf)
{
	char name[] = "textviewXtlbw";
	name[8] = win;

	printf("%s\n", name);
	GtkTextView *tvw =
		(GtkTextView*)glade_xml_get_widget(xml, name);
	assert(tvw);

	GtkTextBuffer* tb = gtk_text_view_get_buffer(tvw);
	assert(tb);

	gtk_text_buffer_set_text(tb, hdr, -1);
	gtk_text_buffer_insert_at_cursor(tb, buf, -1);
	fixed_font_buffer(tb);
}

void cmdOutGtk(const char *name, const char *hdr, const char *buf)
{
	if (!strcmp(name, "dtlblist")) {
		tlbwUpdate('D', hdr, buf);
		return;
	}
	if (!strcmp(name, "itlblist")) {
		tlbwUpdate('I', hdr, buf);
		return;
	}
	printf("*** cmdOutGtk (%s %s %s)\n", name, hdr, buf);
}

/* data lines to show */
#define DATW_LINES 15
void datwUpdateGtk(void)
{
	printf("*** datwUpdateGtk\n");
	char output[80 * DATW_LINES]; /* not actually 80 columns wide, so this should do */
	sprintf(output, "%s", datwtbl[0].bdfcn(DATW_LINES));

	gtk_text_buffer_set_text(datw_buffer, output, -1);
	fixed_font_buffer(datw_buffer);
}

void scrnEndGtk(void)
{
	printf("*** scrnEndGtk()\n");
}

static void tlbwInit()
{
	GtkTextView *w;

	w = GTK_TEXT_VIEW(glade_xml_get_widget(xml, "textviewDtlbw"));
	assert(w);
	gtk_text_buffer_create_tag(gtk_text_view_get_buffer(w),
				   "font", "font", "fixed", NULL);

	w = GTK_TEXT_VIEW(glade_xml_get_widget(xml, "textviewItlbw"));
	assert(w);
	gtk_text_buffer_create_tag(gtk_text_view_get_buffer(w),
				   "font", "font", "fixed", NULL);
}

void scrnInitGtk(void)
{
    GtkWidget *window;

    printf("*** srcninitGtk()\n");

    gtk_init(NULL, NULL);

    xml = glade_xml_new(SKI_DATA_DIR "/gski.glade", NULL, NULL);

    /* connect signal handlers */
    glade_xml_signal_autoconnect(xml);

    /* Set default line in status bar */
    status_bar_widget =
	    (GtkStatusbar*)glade_xml_get_widget(xml, "statusCmdOutput");
    status_bar_context = gtk_statusbar_get_context_id(status_bar_widget,
						      "ski");
    gtk_statusbar_push(status_bar_widget, status_bar_context, "Ready");

    /* setup treeview for command output */
    output_view = GTK_TREE_VIEW(glade_xml_get_widget(xml, "treeviewCmd"));

    output_store = gtk_list_store_new(1, G_TYPE_STRING);
    assert(output_store);

    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(output_view, -1, "Command Output", renderer,
						"text", 0, NULL);
    gtk_tree_view_set_model(output_view, GTK_TREE_MODEL(output_store));

    /* setup combo box pointer */
    cmd_combo_box = GTK_COMBO_BOX(glade_xml_get_widget(xml, "comboCmd"));
    assert(cmd_combo_box);

    /* instruction count pointer */
    instcount_label = GTK_LABEL(glade_xml_get_widget(xml, "labelInstCount"));
    assert(instcount_label);

    /* draw the window */
    window = glade_xml_get_widget(xml, "main_window");
    assert(window);

    gtk_widget_show_all(window);

    tlbwInit();
    datwInit();
    prgwInit();

    /* get the registers filled out */
    regwDrawGtk();

    return;
}


void cmdLoopGtk(void)
{
	printf("*** cmdLoopGtk\n");
	gtk_main();
}

#endif /* !HAVE_GTK */

