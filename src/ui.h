#ifndef _SKI_UI_H
#define _SKI_UI_H

#include <stddef.h>

#include "std.h"
#include "types.h"

#define EXPRSIZ		20
#define TITLESIZ	80
#define WINTBLSIZ	20
#define NAMLEN		32      /* symbol name max length (including NULL!) */

#define RFT "016llx"

typedef enum { ASM, MIXED, SRC } PrgwMode;
typedef enum {
    BATCH = 0,
    CURSES_INTERFACE,
} Interface;


/*--------------------------------------------------------------------------
 *  Internal symbol interface
 *-------------------------------------------------------------------------*/

typedef REG (*PGetFn1)(int);
typedef REG (*PGetFn2)(int, int);
typedef BOOL (*PSetFn2)(int, REG);
typedef BOOL (*PSetFn3)(int, int, REG);

typedef enum {
    SINGLE_SYM,
    REGSET_SYM,
    BITF_SYM,
    RS_BITF_SYM,
    REG_SYM,			/* generic 64-bit data items */
    BYTE_SYM,			/* generic 8-bit data items */
    BOOL_SYM,			/* boolean data items */
    STR_SYM,			/* string data items */
    IP_SYM,			/* IP */
    PSR_SYM,			/* PSR */
    GR_SYM,			/* GRs */
    GRNAT_SYM,			/* GR Nats */
    PR_SYM,			/* PRs */
#if 0
    FR_SYM,			/* FRs */
#endif
    FRMANT_SYM,			/* FR mantissas */
    FREXP_SYM,			/* FR exponents */
    FRSGN_SYM,			/* FR signs */
    RRBGF_SYM,			/* rrbg and rrbf */
    RRBP_SYM			/* rrbp */
} Symtyp;

struct isym {
    char name[NAMLEN];          /* internal symbol name */
    Symtyp type;                /* internal symbol type */

    PGetFn1 getFn1;
    PGetFn2 getFn2;

    PSetFn2 setFn2;
    PSetFn3 setFn3;

    void *pval;
    BOOL readonly;		/* is symbol read-only */
    unsigned len;		/* subfield length (len == 0 => not a subfld) */
    unsigned start;		/* subfield start bit position */
    unsigned ndx;		/* index in register set */
    REG align;			/* data item alignment */
    int dbase;			/* default base (hex or dec) */
};

void isymInit(void);
struct isym *isymVLkp(const char *sname);


/*--------------------------------------------------------------------------
 *  Command interface
 *-------------------------------------------------------------------------*/

#define KEYWDSIZ	20
#define DESCRSIZ	200

typedef BOOL	(*PFV)(unsigned argc, char **argv);
/*
    argc		command argument count
    argv		command arguments
    (retval)		error detected when running the command?
 */

typedef struct {
    char keywd[KEYWDSIZ];	/* menu lookup keyword */
    unsigned minargs, maxargs;	/* min and max number of arguments */
    char descrip[DESCRSIZ];	/* description (used in help) */
    PFV  fcn;			/* dispatch function */
    char format[DESCRSIZ];	/* command usage (used in help) */
} MENU;

void menuInit(void);
MENU *menuLkp(char *s);


/*--------------------------------------------------------------------------
 *  Register window interface
 *-------------------------------------------------------------------------*/

typedef char *	(*PLF)(unsigned line);
/*
    line		register window line to display
    (retval)		pointer to a buffer containing the line to display
 */

typedef struct {
    char tag[EXPRSIZ];		/* register window tag */
    char title[TITLESIZ];	/* register window title */
    PLF  fcn;			/* line display function */
    unsigned curln;		/* current line */
    char size[EXPRSIZ];		/* register window size.  It is a string to
				   allow use of evalExpr() for GR display */
    BOOL show;			/* show the register set in GUI */
} Regw;

extern Regw regwtbl[], *actRegw;
extern unsigned topregw;

void regwUpdate(void);

/*--------------------------------------------------------------------------
 *  Data window interface
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 *  Interface to data window registration and use
 *-------------------------------------------------------------------------*/

typedef char *	(*PBF)(unsigned datwSz);	/* ptr to buffer display fcn */
/*
    datwSz		available data window size
    (retval)		pointer to the display buffer
 */

typedef struct {
    char tag[EXPRSIZ];		/* data window tag */
    char title[TITLESIZ];	/* data window title */
    unsigned size;		/* data window size */
    PBF  bdfcn;			/* buffer display function */
    BOOL show;			/* show data window in GUI */
} Datw;

extern Datw datwtbl[], *actDatw;
extern unsigned topdatw;

char *dmembuf(unsigned datwSz);
void datwUpdate(void);


/*--------------------------------------------------------------------------
 *  Program window interface
 *-------------------------------------------------------------------------*/

void prgwDraw(void);
void prgwUpdate(void);
void prgwPCRedraw(void);


/*--------------------------------------------------------------------------
 *  Command/Main window interface
 *-------------------------------------------------------------------------*/

extern Interface interface;

void cmdErr(const char *, ...);
void cmdWarn(const char *, ...);
void cmdwSetStatus(const char *);
void cmdwPrint(const char *, ...);
void msgwPrint(const char *, ...);

void cleanup_execLoop(BOOL showIrate);

/*--------------------------------------------------------------------------
 *  Entire screen interface
 *-------------------------------------------------------------------------*/

void uiInit(void);

void scrnInit(void);
void scrnUpdate(void);
void scrnEnd(void);


BOOL datwMakeActivew(const char *tag);
/*
    char *tag		registered data window name
    (retval)		was the data window made active?
 */

unsigned datwSz(const char *tag);
/*
    char *tag		registered data window name
    (retval)		data window size
 */

BOOL regwFwd(unsigned argc, char *argv[]);
BOOL regwBkwd(unsigned argc, char *argv[]);
BOOL regwDump(unsigned argc, char *argv[]);
BOOL showUrs(unsigned argc, char *argv[]);
BOOL showSrs(unsigned argc, char *argv[]);
BOOL showGrs(unsigned argc, char *argv[]);
BOOL showFrs(unsigned argc, char *argv[]);
BOOL showIArs(unsigned argc, char *argv[]);

char *getSimID(void);
char *productInfo(void);
char *skiID(void);

ssize_t sysWrite(int fd, void *buf, size_t len);

void cmdLoop(void);

#endif /* _SKI_UI_H */
