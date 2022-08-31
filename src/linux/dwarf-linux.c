/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- 
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#include "std.h"
#include "bits.h"
#include "types.h"
#include "fields.h"
#include "ski.h"
#include "program.h"
#include "libsym.h"
#include "libsrs.h"
#include "libcore.h"
#include "syscall_api.h"
#include "ui.h"


char *src_root;		/* hold the prefix for all filenames */

#define FNAME_LEN	54
#define EMPTY_FILENAME	"???"
#define NOT_A_LINE	0xffffffff

#if defined HAVE_DWARF_SUPPORT

#include "bfd.h"

#include <sys/param.h>

#undef DEBUG_DWARF		/* define this for debug printf into a file */

extern PrgwMode prgwm;

struct line_info
  {
    bfd_vma address;
    bfd_vma limit;
    const char *filename;
    const char *function;
    unsigned int line;		/* first line number */
    unsigned int num_lines;	/* number of lines to display */
    int valid;			/* is this info valid? */
  };

static int bfd_init_done;
static int found;	/* used when iterating over sections */

static asymbol **syms;	/* our local symbol table */
static int src_root_len;	/* how long is the prefix ? */
static bfd *cur_bfd;	/* holds the BFD state for a given executable */
static struct line_info cache;

#ifdef DEBUG_DWARF
static FILE *dump_fd;
#endif

/*
 * Bail out with an error message. We cannot really use cmdwPrint()
 * because it may not be initialized before we get an error
 */
static void fatal_error (char *msg)
{
  fprintf (stderr, "simulator-dwarf: %s", msg);
  exit (1);
}

/*
 * This function is called automatically by the BDF library
 * when looking for a match
 *
 * The last "opaque" argument contains the address we're looking for
 */
static void find_in_section (bfd * bfd, asection * sect, void * obj)
{
  struct line_info *info = obj;
  unsigned int last_line = 0, line;
  struct line_info first;
  bfd_vma vma, offset, i;
  const char *filename;
  const char *function;
  bfd_size_type size;

  /*
   * We want to stop at the first match, not the last one, if any
   */
  if (info->valid)
    return;

  /*
   * we're looking at loadable code only
   */
  if ((bfd_section_flags (sect) & (SEC_CODE | SEC_ALLOC)) == 0)
    return;

  /*
   * check if address belongs to this section
   */
  vma = bfd_section_vma (sect);
  size = bfd_section_size (sect);

  offset = info->address - vma;
  if (offset >= size)
    return;

  /* Check each instruction in the bundle and determine the first and
     last source line.  If the same bundle contains source code from
     multiple files or functions, we're in trouble and return the info
     for the first file/function only.  */
  first.valid = 0;
  for (i = 0; i <= 8; i += 4, offset += 4)
    {
      found = bfd_find_nearest_line (bfd, sect, syms, offset,
				     &filename, &function, &line);
#ifdef DEBUG_DWARF
      fprintf (dump_fd,
	       "%llx -> sect=%s found=%d filename=%s function=%s line=%d\n",
	       vma + offset, sect->name, found, filename ? filename : "<none>",
	       function ? function : "<none>", line);
#endif
      if (found)
	{
	  if (!first.valid)
	    {
	      /* stash away line info for textually first instruction: */
    	      first.valid = 1;
	      first.address = info->address;
	      first.filename = filename;
	      first.function = function;
	      first.line = line;
	      first.num_lines = 1;
	      first.limit = sect->size;
	      last_line = line;
	    }
	  if (line > last_line
	      && filename == first.filename && function == first.function)
	    last_line = line;
	}
    }

  if (!first.valid)
    return;

  /*
   * Determine the line number of the next instruction in the code.
   * If that instruction comes from the same file and function, then
   * we use it to determine how many lines to display.
   */
  while (offset < first.limit)
    {
      offset += 4;
      if ((offset & 0xf) > 8)
	offset += 16 - (offset & 0xf);	/* switch to next bundle */
      found = bfd_find_nearest_line (bfd, sect, syms, offset,
				     &filename, &function, &line);

      if (!found || filename != first.filename || function != first.function)
	break;

      if (line != last_line)
	{
	  if (line > last_line)
	    {
#ifdef DEBUG_DWARF
	      fprintf (dump_fd, "line range = %u..%u\n", first.line, line - 1);
#endif
	      first.num_lines = line - first.line;
	    }
	  break;
	}
    }
  *info = first;
}

/*
 * Extremely dumb/simple line_load routine. We don't cache anything and rely
 * on fgets() to figure out the \n
 */
static char * read_source_lines (struct line_info *info, unsigned *count)
{
  static char *line = NULL;
  static size_t line_size = 0;
  static char *src_lines = 0;
  static size_t size = 0;
  unsigned int line_num = 0;
  const char *filename;
  ssize_t line_len = 0;
  size_t off;
  FILE *fp;
  int i;

  if (!info->filename)
    return "";

#ifdef DEBUG_DWARF
  fprintf (dump_fd, "displaying %s:%u-%u\n",
	   info->filename, info->line, info->line + info->num_lines - 1);
#endif

  filename = info->filename;
  if (src_root_len > 0)
    {
      size_t filename_len = strlen (info->filename);
      char *buf;

      /* append filename to prefix and use full path as filename: */

      buf = alloca (src_root_len + filename_len + 1);
      if (!buf)
	fatal_error ("Out of memory");

      memcpy (buf, src_root, src_root_len);
      memcpy (buf + src_root_len, info->filename, filename_len + 1);
      filename = buf;
    }
  fp = fopen (filename, "r");
  if (fp == NULL)
    return "";

  while (line_num < info->line
	 && (line_len = getline (&line, &line_size, fp)) >= 0)
    ++line_num;

  if (line_num != info->line)
    {
      /*
       * Oops, can't find it!  The file may have changed in between:
       * please don't do that!
       */
      cmdwPrint ("WARNING: Can't find line %d in %s\n", line, filename);
      return "";
    }

  off = 0;
  for (i = 0; i < info->num_lines; ++i)
    {
      if (*count >= 3)
	{
	  strcpy (line, "...\n");
	  line_len = 4;
	  i = info->num_lines;
	}
      if (off + line_len + 4 > size)
	{
	  /* increase size of src_lines */
	  size += 1024;
	  src_lines = realloc (src_lines, size);
	  if (!src_lines)
	    fatal_error ("Out of memory");
	}
      sprintf (src_lines + off, "%03u ", line_num + i);
      memcpy (src_lines + off + 4, line, line_len + 1);
      off += line_len + 4;
      ++*count;
      line_len = getline (&line, &line_size, fp);
      if (line_len < 0)
	break;
    }
  fclose (fp);
  return src_lines;
}

static int
get_source_info (ADDR addr, struct line_info *info)
{
  if (cur_bfd == NULL)
    return -1;

  if (cache.valid && (cache.address >> 4) == (addr >> 4))
    {
      *info = cache;
      return 0;
    }

  memset (info, 0, sizeof (*info));
  info->address = addr;
  bfd_map_over_sections (cur_bfd, find_in_section, info);

  if (info->valid)
    {
      cache = *info;
      return 0;
    }
  return -1;
}

/*
 * This routine returns the source lines that correspond to the bundle
 * of instructions at address OFS.  The source lines are returned in a
 * single string, separated by a newline character.  This function is
 * NOT reentrant as the source line string is stored in a single
 * buffer.
 *
 * Possibilities to improve:
 *    - The search algorithms are linear.  The performance of these
 *	algorithms can be improved by making them binary searches.
 *
 *    -	The line table is always accessed from the DWARF data structures.
 *	The performance can be improved by caching the line table.
 */
char *
getSrcLines (ADDR ofs, unsigned *count)
{
  static struct line_info prev;
  struct line_info info, disp;
  int ret;

  *count = 0;

  if (prgwm == ASM)
    return "";

  cmdwSetStatus ("Reading debug info...");
  ret = get_source_info (ofs, &info);
  cmdwSetStatus ("");

  if (ret < 0)
    return "";

  disp = info;
  if (prev.valid && ofs > prev.address)
    {
      /*
       * It's okay to check for filenames as pointers because the
       * variable point to the same data structure in BFD if it's the
       * same file. Otherwise we'd have to use strcmp.
       */
      if (info.filename == prev.filename)
	{
	  if (info.line + info.num_lines <= prev.line + prev.num_lines)
	    return "";

	  /* compute the new lines of source code: */
	  disp.line = MAX (prev.line + prev.num_lines, info.line);
	  disp.num_lines = info.line + info.num_lines - disp.line;
	}
    }
  prev = info;
  return read_source_lines (&disp, count);
}

/*
 * Get the filename for the top level bar of the display
 *
 * must match whatever is in cur.c prgwUpdateCur() for buf
 * looks like it cannot really be different from 80
 * that's not so much related to the size of the screen
 * even though cur.c:prgwUpdateCur() has been adjusted to fit 
 * the minimal screen size allowed.
 */
char *
getSrcName (ADDR ofs)
{
  static char fname[FNAME_LEN];
  struct line_info info;
  int l, offs;

  if (get_source_info (ofs, &info) < 0 || !info.filename)
    {
      strcpy (fname, EMPTY_FILENAME);
      return fname;
    }

  /*
   * In case the name is too long to fit, we cut the least significant
   * part of it (ie closer from the root)
   */
  l = src_root_len + strlen (info.filename);
  if (l > FNAME_LEN - 1)
    {
      offs = l - (FNAME_LEN - 1 - 3);
      l = FNAME_LEN - 1 - 3;
      strcpy (fname + 3, info.filename + offs);
      fname[0] = fname[1] = fname[2] = '.';
    }
  else
    {
      if (src_root_len > 0)
	strncpy (fname, src_root, src_root_len);
      strcpy (fname + src_root_len, info.filename);
    }
  return fname;
}

/*
 * inspired by slurp_symtab from addr2line/objdump
 */
static void
load_symbols (bfd * abfd)
{
  long storage;
  long symcount;

  if ((bfd_get_file_flags (abfd) & HAS_SYMS) == 0)
    return;

  storage = bfd_get_symtab_upper_bound (abfd);
  if (storage < 0)
    fatal_error ("can't get sym table size\n");
  syms = (asymbol **) malloc (storage);
  if (!syms)
    fatal_error ("malloc\n");

  symcount = bfd_canonicalize_symtab (abfd, syms);
  if (symcount < 0)
    fatal_error ("canonicalize\n");
#ifdef DEBUG_DWARF
  cmdwPrint ("symcount=%d\n", symcount);
#endif
}

#ifdef DEBUG_DWARF
/*
 * Init DWARF tables for the file referenced by file descriptor FD.
 */
static void
close_dump_fd (void)
{
  fclose (dump_fd);
}
#endif

static void
do_bfd_init (void)
{

  /*
   * sanity check, make sure we're using the right bfd.h
   * can't use progExit() because subsystem not initialized yet
   */
  if (sizeof (bfd_vma) < sizeof (ADDR))
    {
      fprintf (stderr, "invalid bfd_vma size: check your bfd.h\n");
      exit (1);
    }

  bfd_init ();

  if (!bfd_set_default_target ("elf64-ia64-little"))
    fatal_error ("set_default_target");

  bfd_init_done = 1;

#ifdef DEBUG_DWARF
  dump_fd = fopen ("ski.log", "w");
  atexit (close_dump_fd);
#endif
  /*
   * where to add suffix, if any (avoid sprintf in line_load)
   */
  src_root_len = 0;
  if (src_root)
    src_root_len = strlen (src_root);
}

/*
 * This function is called when a file is loaded to establish a BFD
 * context for it.
 */
void
initDwUnitTbl (int fd)
{
  char **matching;
  bfd *bfd;
  int fd2;

  /*
   * that's the one time initialization code for BFD
   */
  if (bfd_init_done == 0)
    do_bfd_init ();

  /*
   * we need to be isolated from the rest of the simulator
   * and we need to rewind to beginning of file to read headers
   *
   */
  fd2 = dup (fd);
  if (fd2 == -1)
    fatal_error ("dup failed\n");
  lseek (fd2, 0, SEEK_SET);

  /*
   * if we're loading a subsequent file (like for the kernel)
   * cleanup previous state before anything else
   *
   * Note that bfd_close() takes care of closing the file as well
   * (the one we dupped from fd)
   */
  if (cur_bfd)
    {
      bfd_close (cur_bfd);
      cur_bfd = NULL;

      memset (&cache, 0, sizeof (cache));

      if (syms)
	free (syms);
      syms = NULL;
    }

  /*
   * given that we have fdopen() the first argument 
   * is not really used. That's good because this way, we don't
   * need to change the API of this function.
   */
  bfd = bfd_fdopenr ("", "default", fd2);
  if (bfd == NULL)
    fatal_error ("fdopenr\n");

  if (bfd_check_format (bfd, bfd_archive))
    fatal_error ("check_format");

  if (!bfd_check_format_matches (bfd, bfd_object, &matching))
    fatal_error ("check_format_matches");

  load_symbols (bfd);

  cur_bfd = bfd;
}

#else /* !defined HAVE_DWARF_SUPPORT */

char *
getSrcLines (ADDR ofs, unsigned *count)
{
  *count = 0;
  return "";
}

char *
getSrcName (ADDR ofs)
{
  static char fname[FNAME_LEN];
  strcpy (fname, EMPTY_FILENAME);
  return fname;
}

void
initDwUnitTbl (int fd)
{
}

#endif /* !defined HAVE_DWARF_SUPPORT */
