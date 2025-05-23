## Process this file with autoconf to produce a configure script.
##
##
## Copyright (C) 1995-2007, Hewlett-Packard Development Company, L.P.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or 
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful, 
## but WITHOUT ANY WARRANTY; without even the implied warranty of 
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License along
## with this program; if not, write to the Free Software Foundation, Inc.,
## 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
##
##

AC_PREREQ([2.29])

AC_INIT([ski],[1.5.0],[slyich@gmail.com],[ski],[https://github.com/trofi/ski])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign subdir-objects dist-xz dist-lzip no-dist-gzip])
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_HOST

case $host_os in
    linux*)	HOST_OS=linux;;
    hpux*)	HOST_OS=hpux;;
    freebsd*)	HOST_OS=freebsd;;
    *)		HOST_OS=unknown;;
esac
AC_SUBST(HOST_OS)
AM_CONDITIONAL(LINUX, test "$HOST_OS" = linux)
AM_CONDITIONAL(HPUX,  test "$HOST_OS" = hpux)
AM_CONDITIONAL(FREEBSD, test "$HOST_OS" = freebsd)

# Checks for programs.
AC_PROG_CC
AM_PROG_AS
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_AWK
AC_PROG_SED
PKG_PROG_PKG_CONFIG

AC_C_INLINE
AC_C_CONST

dnl Check for required build tools
AC_CHECK_PROGS([GPERF], [gperf], [$MISSING gperf])
AC_CHECK_PROGS([FLEX], [flex], [$MISSING flex])
AC_CHECK_PROGS([BISON], [bison], [$MISSING bison])

dnl Check for the pager program
AC_CHECK_PROGS([PAGER], [less])
AC_DEFINE_UNQUOTED([PAGER], ["$PAGER"], [Define to the pager program.])

dnl Initialize maintainer mode
AM_MAINTAINER_MODE

dnl Find libelf.h
AC_CHECK_HEADER(libelf.h,,
    [AC_CHECK_HEADER(libelf/libelf.h, have_libelf_libelf_h=yes,
        [AC_MSG_ERROR(Required libelf.h header not found.)])])
AS_IF([test "x$have_libelf_libelf_h" = xyes], [
    AC_DEFINE(HAVE_LIBELF_LIBELF_H, 1, [define if you have libelf/libelf.h])
])

dnl Check for netdev support
AS_IF([test "$HOST_OS" = linux], [
  AC_ARG_ENABLE([netdev],
    AS_HELP_STRING([--disable-netdev], [disable netdev support]))
  AS_IF([test "x$enable_netdev" != xno], [
    AC_DEFINE(SKINET_ENABLE, 1, [define if you want netdev support])
  ])
])

AS_IF([test "$HOST_OS" = freebsd], [
    LDFLAGS="$LDFLAGS -lutil"
])

dnl Check for optional libs
AC_CHECK_LIB(unwind-ia64, _Uia64_get_proc_name, )

# Test for IA-64 libbfd

AC_ARG_WITH([bfd-includes],
  AS_HELP_STRING([--with-bfd-includes],[specify location of bfd headers]),[
    BFD_CFLAGS="-I$withval"
])

AC_ARG_WITH([bfd-libs],
  AS_HELP_STRING([--with-bfd-libs],[specify location of bfd libraries]),[
    BFD_LDFLAGS="-L$withval"
])

AC_MSG_CHECKING([for bfd support])
AC_ARG_WITH([bfd], AS_HELP_STRING([--with-bfd],[use the bfd library]))

AS_IF([test "x$with_bfd" = xno], [have_bfd=disabled], [
    save_CFLAGS=$CFLAGS
    save_LDFLAGS=$LDFLAGS
    save_LIBS=$LIBS
    CFLAGS="$CFLAGS $BFD_CFLAGS"
    LDFLAGS="$LDFLAGS $BFD_LDFLAGS"
    LIBS="$LIBS -lbfd -liberty"
    AC_RUN_IFELSE([AC_LANG_PROGRAM([[#include <bfd.h>]],[[
            bfd_init ();
            return bfd_set_default_target ("elf64-ia64-little") == 0;
        ]])],
        [have_bfd=yes], [have_bfd=no],[have_bfd=no])
    CFLAGS=$save_CFLAGS
    LDFLAGS=$save_LDFLAGS
    LIBS=$save_LIBS
])

AS_IF([test "x$have_bfd" = xyes], [
    DWARF_CFLAGS="$BFD_CFLAGS"
    DWARF_LIBS="$BFD_LDFLAGS -lbfd -liberty"
])
AC_MSG_RESULT([$have_bfd])

AS_IF([test "x$have_bfd" = xyes], [
    AC_DEFINE(HAVE_DWARF_SUPPORT, 1, [define if you have dwarf support])
])

AC_SUBST(DWARF_CFLAGS)
AC_SUBST(DWARF_LIBS)

dnl Check for required libs
AC_CHECK_LIB(m, ldexp, [],
    [AC_MSG_ERROR(Required library libm not found.)])

AS_IF([test "$HOST_OS" = hpux], [
    # HPUX prefers libHcurses.
    AC_CHECK_LIB(Hcurses, tgetent,[
        AC_DEFINE(__HP_CURSES, 1, [define if you want to use HP curses])
        LIBS="-lHcurses $LIBS"
        check_curses=no], [])
])

AS_IF([test "x$check_curses" != xno], [
    PKG_CHECK_MODULES([NCURSES], [ncurses])
    CFLAGS="$CFLAGS $NCURSES_CFLAGS"
    LIBS="$LIBS $NCURSES_LIBS"
])

AC_CHECK_LIB(elf, elf_begin, [],
    [AC_MSG_ERROR(Required library libelf not found.)])

dnl Add extra flags to CFLAGS depending on os
case $host_os in
    linux*)	CFLAGS="$CFLAGS -D_GNU_SOURCE";;
    hpux9*)	CFLAGS="$CFLAGS -DHPUX9";;
    hpux10*)	CFLAGS="$CFLAGS -DHPUX1020";;
    hpux11*)	CFLAGS="$CFLAGS -DHPUX1100";;
    # __linux__ on freebsd looks scary. Does it work at all?
    freebsd*)	CFLAGS="$CFLAGS -D__linux__";;
esac

# CPU instructions modify arbitrary memory.
# No chance to maintain aliasing guarantees.
CFLAGS="$CFLAGS -fno-strict-aliasing"

# Macro from autoconf-archive.
# Pull in -Werror= flags to catch obviously wrong code
AX_CHECK_COMPILE_FLAG([-Werror=implicit-int], [CFLAGS="$CFLAGS -Werror=implicit-int"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Werror=implicit-function-declaration], [CFLAGS="$CFLAGS -Werror=implicit-function-declaration"], [], [-Werror])
# Useful to catch missing 'static' declarations and missing header
# inclusion to keep header and .c file prototypes in sync.
AX_CHECK_COMPILE_FLAG([-Wmissing-declarations], [CFLAGS="$CFLAGS -Wmissing-declarations"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wunused-function], [CFLAGS="$CFLAGS -Wunused-function"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wunused-variable], [CFLAGS="$CFLAGS -Wunused-variable"], [], [-Werror])
# Useful to catch non-standard constructs.
AX_CHECK_COMPILE_FLAG([-Wpedantic], [CFLAGS="$CFLAGS -Wpedantic"], [], [-Werror])

dnl Treat warnings as errors
AC_ARG_ENABLE([werror],
  AS_HELP_STRING([--enable-werror],
    [Fail on encountered compiler warnings]))
AS_IF([test "x$enable_werror" = "xyes"],
  [CFLAGS="$CFLAGS -Werror"])

dnl Configuration files
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/fake-xterm/Makefile])
AC_CONFIG_FILES([doc/Makefile])

AC_CONFIG_MACRO_DIRS([m4])

dnl Output the files
AC_OUTPUT
