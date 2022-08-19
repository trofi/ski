#!/bin/sh
#
# Copyright (C) 1995-2007, Hewlett-Packard Development Company, L.P.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or 
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, 
# but WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
#
# Run this to generate all the initial makefiles, etc.

DIE=0

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: You must have \`autoconf' installed to compile ski."
  echo "Download the appropriate package for your distribution,"
  echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
  DIE=1
}

(grep "^AM_PROG_LIBTOOL" $srcdir/configure.ac >/dev/null) && {
  (libtool --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "**Error**: You must have \`libtool' installed to compile ski."
    echo "Get ftp://ftp.gnu.org/pub/gnu/libtool-1.2d.tar.gz"
    echo "(or a newer version if it is available)"
    DIE=1
  }
}

grep "^AM_GNU_GETTEXT" $srcdir/configure.ac >/dev/null && {
  grep "sed.*POTFILES" $srcdir/configure.ac >/dev/null || \
  (gettext --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "**Error**: You must have \`gettext' installed to compile ski."
    echo "Get ftp://alpha.gnu.org/gnu/gettext-0.16.1.tar.gz"
    echo "(or a newer version if it is available)"
    DIE=1
  }
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: You must have \`automake' installed to compile ski."
  echo "Get ftp://ftp.gnu.org/pub/gnu/automake-1.10.tar.gz"
  echo "(or a newer version if it is available)"
  DIE=1
  NO_AUTOMAKE=yes
}

# not all distros have an 'aclocal'.  debian, in particular, uses
# 'aclocal-X.Y'.  so, set variables properly.
ACLOCAL=aclocal
test -n "$NO_AUTOMAKE" || test -f "/etc/debian_version" && {
  version=`automake --version | head -1 | cut -d' ' -f4 | cut -d. -f1-2 | \
           cut -d- -f1`
  ACLOCAL=aclocal-${version}
  ACLOCAL_FLAGS="-I /usr/share/aclocal-${version}"
}

# if no automake, don't bother testing for aclocal
test -n "$NO_AUTOMAKE" || ($ACLOCAL --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: Missing \`$ACLOCAL'.  The version of \`automake'"
  echo "installed doesn't appear recent enough."
  echo "Get ftp://ftp.gnu.org/pub/gnu/automake-1.10.tar.gz"
  echo "(or a newer version if it is available)"
  DIE=1
}

if test "$DIE" -eq 1; then
  exit 1
fi

if test -z "$*"; then
  echo "**Warning**: I am going to run \`configure' with no arguments."
  echo "If you wish to pass any to it, please specify them on the"
  echo \`$0\'" command line."
  echo
fi

case $CC in
xlc )
  am_opt=--include-deps;;
esac

for coin in `find $srcdir -name configure.ac -print`
do 
  echo "=> working on $coin"
  dr=`dirname $coin`
  cf=`basename $coin`
  if test -f $dr/NO-AUTO-GEN; then
    echo skipping $dr -- flagged as no auto-gen
  else
    echo processing $dr
    macrodirs=`sed -n -e 's,AM_ACLOCAL_INCLUDE(\(.*\)),\1,gp' < $coin`
    ( cd $dr
      aclocalinclude="$ACLOCAL_FLAGS"
      for k in $macrodirs; do
        if test -d $k; then
          aclocalinclude="$aclocalinclude -I $k"
        fi
      done
      if grep "^AM_GNU_GETTEXT" $cf >/dev/null; then
        if grep "sed.*POTFILES" $cf >/dev/null; then
          : do nothing -- we still have an old unmodified configure.ac
        else
          echo "Creating $dr/aclocal.m4 ..."
          test -r $dr/aclocal.m4 || touch $dr/aclocal.m4
          echo "Running gettextize...  Ignore non-fatal messages."
          echo "no" | gettextize --force --copy
          echo "Making $dr/aclocal.m4 writable ..."
          test -r $dr/aclocal.m4 && chmod u+w $dr/aclocal.m4
        fi
      fi
      if grep "^AM_PROG_LIBTOOL" $cf >/dev/null; then
	    echo "Running libtoolize..."
	    libtoolize --force --copy $lt_opt
      fi
      echo "Running $ACLOCAL $aclocalinclude ..."
      $ACLOCAL $aclocalinclude || {
        echo
        echo "**Error**: $ACLOCAL failed. This may mean that you have not"
        echo "installed all of the packages you need, or you may need to"
        echo "set ACLOCAL_FLAGS to include \"-I \$prefix/share/aclocal\""
        echo "for the prefix where you installed the packages whose"
        echo "macros were not found"
        exit 1
      }
      if grep "^AM_CONFIG_HEADER" $cf >/dev/null; then
	    echo "Running autoheader..."
	    autoheader || { echo "**Error**: autoheader failed."; exit 1; }
      fi
      echo "Running automake --add-missing --gnu $am_opt ..."
      automake --add-missing --gnu $am_opt ||
        { echo "**Error**: automake failed."; exit 1; }
      echo "Running autoconf ..."
      autoconf || { echo "**Error**: autoconf failed."; exit 1; }
    ) || exit 1
  fi
done

conf_flags=""
conf_flags="$conf_flags --enable-maintainer-mode"
conf_flags="$conf_flags --enable-dependency-tracking"
#conf_flags="$conf_flags --enable-iso-c"

if test x$NOCONFIGURE = x; then
  # NOTE: srcdir must be already set or passed to each configure via --srcdir
  echo Running $srcdir/configure $conf_flags "$@" ...
  $srcdir/configure $conf_flags "$@" \
  && echo Now type \`make\' to compile $PKG_NAME || exit 1
else
  echo Skipping configure process.
fi
