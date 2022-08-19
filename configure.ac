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

# Requires autoconf 2.61
AC_PREREQ(2.61)

# Initialize autoconf (and check for presence of src/ski.h)
AC_INIT(src/ski.h)

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

# Package information
PACKAGE=ski
VERSION=$SKI_VERSION

# Specify the default install prefix
AC_PREFIX_DEFAULT(/usr/local)

# Initialize automake
AM_INIT_AUTOMAKE($PACKAGE,$VERSION)

# Specify a configuration file
AM_CONFIG_HEADER(config.h)
AM_ACLOCAL_INCLUDE(macros)

# Checks for programs.
AC_PROG_CC( icc ecc gcc3 cc gcc )
AM_PROG_CC_STDC
AM_PROG_AS
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_AWK
AC_PROG_SED
AC_PROG_YACC
AM_PROG_LEX
PKG_PROG_PKG_CONFIG

AC_C_INLINE
AC_C_CONST

dnl Disable shared for default (enable with --enable-shared) 
AM_DISABLE_SHARED

dnl Check for compiler kind (HP cc or Intel icc)
AC_LANG_COMPILER_CHECK

dnl Check for required build tools
AC_CHECK_PROG(GPERF, gperf, gperf)
test -z "$GPERF" && GPERF='$(top_srcdir)/missing gperf'

dnl Check for the pager program
AC_CHECK_PROGS(PAGER, [less])
AC_DEFINE_UNQUOTED([PAGER], ["$PAGER"], [Define to the pager program.])

dnl Set yacc flags
YFLAGS=-d
AC_SUBST(YFLAGS)

dnl Initialize libtool
AC_LIBTOOL_DLOPEN

AM_PROG_LIBTOOL

# build ltdl.o in src
AC_LIB_LTDL

dnl Initialize maintainer mode
AM_MAINTAINER_MODE

dnl See if we build X11
AC_ARG_WITH(x11,
AC_HELP_STRING([--with-x11],
    [Enable the Motif based X11 interface default=no]),
    [with_x11=$withval])
  
if test "x$with_x11" = "xyes"; then
  dnl Check for motif, error out if not found
  AC_WITH_MOTIF
fi
# either way we need to set these
AM_CONDITIONAL(WITH_X_INTERFACE, test "x$with_x11" = "xyes")
AC_SUBST(X_INTERFACE_LIBS)
  
dnl See if we build GTK
AC_ARG_WITH(gtk,
AC_HELP_STRING([--with-gtk],
    [Enable building the GTK Ski interface default=no]),
    [with_gtk=$withval]
)

if test "x$with_gtk" = "xyes"; then
 PKG_CHECK_MODULES([GSKI], [libglade-2.0 libgnomeui-2.0 >= 1.110.0])
 AC_DEFINE(HAVE_GTK, 1, [define if you have GTK support])
fi
AC_SUBST(GSKI_CFLAGS)
AC_SUBST(GSKI_LIBS)
AM_CONDITIONAL(WITH_GTK_INTERFACE, test "x$with_gtk" = "xyes")

dnl Check for host endianness
AC_CACHE_CHECK([whether host byte ordering is defined in sys/param.h],
ski_cv_c_bigendian_compile,
[AC_TRY_COMPILE([
#include <sys/types.h>
#include <sys/param.h>
],[
#if !BYTE_ORDER || !BIG_ENDIAN || !LITTLE_ENDIAN
#error bogus endian macros
#endif
],
ski_cv_c_bigendian_compile=yes, ski_cv_c_bigendian_compile=no)])

if test "x$ski_cv_c_bigendian_compile" = xyes; then
AC_DEFINE(ENDIANESS_IN_SYS_PARAM_H, 1,
    [define if sys/param.h defines the endiness])
fi
AC_C_BIGENDIAN

dnl Check for variables & functions

AC_FUNC_ALLOCA

vars=""
funcs="strtoull __strtoull"

# These are neither executed nor required, but they help keep
# autoheader happy.
if test "yes" = "no"; then
    AC_CHECK_FUNCS(strtoull __strtoull)
    AC_DEFINE(HAVE_SYS_SIGIST, 1,
              [Define if you have the sys_siglist variable.] )
fi
# Check for $vars
for var in $vars; do
    AC_MSG_CHECKING([for $var])
    AC_CACHE_VAL(ski_cv_var_$var,
    [AC_TRY_LINK([int *p;], [extern int $var; p = &$var;],
                 [eval "ski_cv_var_$var=yes"],
                 [eval "ski_cv_var_$var=no"])])
    if eval "test \"`echo '$ski_cv_var_'$var`\" = yes"; then
        AC_MSG_RESULT(yes)
        n=HAVE_`echo $var | tr 'abcdefghijklmnopqrstuvwxyz' \
                'ABCDEFGHIJKLMNOPQRSTUVWXYZ'`
        AC_DEFINE_UNQUOTED($n)
    else
        AC_MSG_RESULT(no)
    fi
done
# Check for $funcs
AC_CHECK_FUNCS($funcs)

dnl Find libelf.h
AC_CHECK_HEADER(libelf.h,,
    [AC_CHECK_HEADER(libelf/libelf.h, have_libelf_libelf_h=yes,
        [AC_MSG_ERROR(Required libelf.h header not found.)])])
if test "x$have_libelf_libelf_h" = xyes; then
    AC_DEFINE(HAVE_LIBELF_LIBELF_H, 1, [define if you have libelf/libelf.h])
fi

dnl Check for netdev support
if test "$HOST_OS" = linux; then
AC_ARG_ENABLE(netdev,
    AC_HELP_STRING([--disable-netdev], [disable netdev support]))
if test "x$enable_netdev" != xno; then
    AC_DEFINE(SKINET_ENABLE, 1, [define if you want netdev support])
    with_netdev=yes
fi
fi

AM_CONDITIONAL(WITH_NETDEV, test "x$with_netdev" = xyes)

if test "$HOST_OS" = freebsd; then
	LDFLAGS="$LDFLAGS -lutil"
fi

dnl Check for optional libs
AC_CHECK_LIB(unwind-ia64, _Uia64_get_proc_name, )

# Test for IA-64 libbfd

AC_ARG_WITH(bfd-includes,
AC_HELP_STRING([--with-bfd-includes],[specify location of bfd headers]),[
    BFD_CFLAGS="-I$withval"
])

AC_ARG_WITH(bfd-libs,
AC_HELP_STRING([--with-bfd-libs],[specify location of bfd libraries]),[
    BFD_LDFLAGS="-L$withval"
])

AC_MSG_CHECKING([for bfd support])
AC_ARG_WITH(bfd, AC_HELP_STRING([--with-bfd],[use the bfd library]))

if test "x$with_bfd" = xno; then
    have_bfd=disabled
else
    save_CFLAGS=$CFLAGS
    save_LDFLAGS=$LDFLAGS
    save_LIBS=$LIBS
    CFLAGS="$CFLAGS $BFD_CFLAGS"
    LDFLAGS="$LDFLAGS $BFD_LDFLAGS"
    LIBS="$LIBS -lbfd -liberty"
    AC_TRY_RUN([
        #include <bfd.h>
        int main() {
            bfd_init ();
            return bfd_set_default_target ("elf64-ia64-little") == 0;
        }],
        have_bfd=yes, have_bfd=no)
    CFLAGS=$save_CFLAGS
    LDFLAGS=$save_LDFLAGS
    LIBS=$save_LIBS
fi

if test "x$have_bfd" = xyes; then
    DWARF_CFLAGS="$BFD_CFLAGS"
    DWARF_LIBS="$BFD_LDFLAGS -lbfd -liberty"
fi
AC_MSG_RESULT([$have_bfd])

if test "x$have_bfd" = xyes; then
    AC_DEFINE(HAVE_DWARF_SUPPORT, 1, [define if you have dwarf support])
fi


AC_SUBST(DWARF_CFLAGS)
AC_SUBST(DWARF_LIBS)

dnl Check if ski_elf.h is missing
AC_CHECK_HEADER(ski_elf.h,,ske_elf_h_is_missing=yes)
if test "x$ske_elf_h_is_missing" = xyes; then
    AC_CONFIG_LINKS([src/ski_elf.h:src/missing/ski_elf.h])
fi

dnl Check for required libs

AC_CHECK_LIB(m, ldexp, [],
    [AC_MSG_ERROR(Required library libm not found.)])

if test "$HOST_OS" = hpux; then
    # HPUX prefers libHcurses.
    AC_CHECK_LIB(Hcurses, tgetent,[
        AC_DEFINE(__HP_CURSES, 1, [define if you want to use HP curses])
        LIBS="-lHcurses $LIBS"
        check_curses=no], [])
fi    

if test "x$check_curses" != xno; then
    PKG_CHECK_MODULES([NCURSES], [ncurses])
    CFLAGS="$CFLAGS $NCURSES_CFLAGS"
    LIBS="$LIBS $NCURSES_LIBS"
fi

AC_CHECK_LIB(elf, elf_begin, [],
    [AC_MSG_ERROR(Required library libelf not found.)])

dnl Add extra flags to CFLAGS depending on os
case $host_os in
    linux*)	CFLAGS="$CFLAGS -DLINUX -D_GNU_SOURCE";;
    hpux9*)	CFLAGS="$CFLAGS -DHPUX9";;
    hpux10*)	CFLAGS="$CFLAGS -DHPUX1020";;
    hpux11*)	CFLAGS="$CFLAGS -DHPUX1100";;
    freebsd*)	CFLAGS="$CFLAGS -DFREEBSD -D__linux__";;
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
AC_CONFIG_FILES([ski-config], [chmod +x ski-config])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([macros/Makefile])
AC_CONFIG_FILES([src/Makefile src/decoder/Makefile src/encoder/Makefile])
AC_CONFIG_FILES([src/fake-xterm/Makefile])
AC_CONFIG_FILES([doc/Makefile])

dnl Output the files
AC_OUTPUT
