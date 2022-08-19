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

AC_PREREQ(2.70)

AC_INIT([ski],[1.4.0-pre],[slyich@gmail.com], [ski], [https://github.com/trofi/ski])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign subdir-objects dist-xz dist-lzip no-dist-gzip])
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

dnl Canonical host
AC_CANONICAL_HOST
host_alias=$host dnl For dejagnu

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

# Version number definitions
#
SKI_MAJOR_VERSION=1
SKI_MINOR_VERSION=3
SKI_MICRO_VERSION=2
SKI_INTERFACE_AGE=0
SKI_BINARY_AGE=0
SKI_RELEASE=gplv2

SKI_VERSION=$SKI_MAJOR_VERSION.$SKI_MINOR_VERSION.$SKI_MICRO_VERSION
AC_SUBST(SKI_MAJOR_VERSION)
AC_SUBST(SKI_MINOR_VERSION)
AC_SUBST(SKI_MICRO_VERSION)
AC_SUBST(SKI_VERSION)
AC_SUBST(SKI_INTERFACE_AGE)
AC_SUBST(SKI_BINARY_AGE)
AC_SUBST(SKI_RELEASE)
AC_DEFINE_UNQUOTED([SKI_RELEASE], ["$SKI_RELEASE"], [Ski release string.])

# libtool versioning
LT_RELEASE=$SKI_MAJOR_VERSION.$SKI_MINOR_VERSION
LT_CURRENT=`expr $SKI_MICRO_VERSION - $SKI_INTERFACE_AGE`
LT_REVISION=$SKI_INTERFACE_AGE
LT_AGE=`expr $SKI_BINARY_AGE - $SKI_INTERFACE_AGE`
AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

# Checks for programs.
AC_PROG_CC
AM_PROG_AS
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_AWK
AC_PROG_SED
AC_PROG_YACC
AC_PROG_LEX([noyywrap])
PKG_PROG_PKG_CONFIG

AC_C_INLINE
AC_C_CONST

dnl Check for required build tools
AC_CHECK_PROG(GPERF, gperf, gperf)
test -z "$GPERF" && GPERF='$(top_srcdir)/missing gperf'

dnl Check for the pager program
AC_CHECK_PROGS(PAGER, [less])
AC_DEFINE_UNQUOTED([PAGER], ["$PAGER"], [Define to the pager program.])

dnl Set yacc flags
YFLAGS=-d
AC_SUBST(YFLAGS)

dnl Initialize maintainer mode
AM_MAINTAINER_MODE

dnl See if we build X11
AC_ARG_WITH([x11],
  AS_HELP_STRING([--with-x11],
    [Enable the Motif based X11 interface default=no]),
    [with_x11=$withval])

AS_IF([test "x$with_x11" = "xyes"],[AC_WITH_MOTIF])

# either way we need to set these
AM_CONDITIONAL(WITH_X_INTERFACE, test "x$with_x11" = "xyes")
AC_SUBST(X_INTERFACE_LIBS)
  
dnl See if we build GTK
AC_ARG_WITH([gtk],
  AS_HELP_STRING([--with-gtk],
    [Enable building the GTK Ski interface default=no]),
    [with_gtk=$withval]
)

AS_IF([test "x$with_gtk" = "xyes"], [
 PKG_CHECK_MODULES([GSKI], [libglade-2.0 libgnomeui-2.0 >= 1.110.0])
 AC_DEFINE(HAVE_GTK, 1, [define if you have GTK support])
])
AC_SUBST(GSKI_CFLAGS)
AC_SUBST(GSKI_LIBS)
AM_CONDITIONAL(WITH_GTK_INTERFACE, test "x$with_gtk" = "xyes")

dnl Check for host endianness
AC_CACHE_CHECK([whether host byte ordering is defined in sys/param.h],
ski_cv_c_bigendian_compile,
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <sys/param.h>
]],[[
#if !BYTE_ORDER || !BIG_ENDIAN || !LITTLE_ENDIAN
#error bogus endian macros
#endif
]])],
[ski_cv_c_bigendian_compile=yes], [ski_cv_c_bigendian_compile=no])])

AS_IF([test "x$ski_cv_c_bigendian_compile" = xyes], [
AC_DEFINE(ENDIANESS_IN_SYS_PARAM_H, 1,
    [define if sys/param.h defines the endiness])
])
AC_C_BIGENDIAN

dnl Check for variables & functions

AC_FUNC_ALLOCA

AC_CHECK_FUNCS([strtoull __strtoull])

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
    with_netdev=yes
  ])
])

AM_CONDITIONAL(WITH_NETDEV, test "x$with_netdev" = xyes)

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

dnl Check if ski_elf.h is missing
AC_CHECK_HEADER(ski_elf.h,,ske_elf_h_is_missing=yes)
AS_IF([test "x$ske_elf_h_is_missing" = xyes], [
    AC_CONFIG_LINKS([src/ski_elf.h:src/missing/ski_elf.h])
])

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

encdir='$(top_srcdir)'/src/encodings
AC_SUBST(encdir)

dnl Links
#AC_CONFIG_LINKS([src/syscall-hpux.c:src/syscall.c])
#AC_CONFIG_LINKS([src/dwarf-hpux.c:src/dwarf.c])

dnl Configuration files
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile src/decoder/Makefile src/encoder/Makefile])
AC_CONFIG_FILES([src/fake-xterm/Makefile])
AC_CONFIG_FILES([doc/Makefile])

LT_INIT
AC_CONFIG_MACRO_DIRS([m4])

dnl Output the files
AC_OUTPUT
