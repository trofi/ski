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
#

srcdir=`dirname $0`
test -z "${srcdir}" && srcdir=.

PKG_NAME="ski"

(test -f ${srcdir}/configure.ac) || {
    echo "**Error**: Directory "\`${srcdir}\'" does not look like the"\
         "top-level ${PKG_NAME} directory"
    exit 1
}

# Deal with the GNUism or Linuxism of always including /usr/local.
# This is not done on FreeBSD, because /usr/local is not dictated
# or enforced by FreeBSD and system administrators are free to
# pick a different tree like /opt for there add-on bits. As such,
# they are responsible for telling the compiler where the add-on
# bits are. By default things are put under /usr/local though, so
# that's our default as well...
if [ `uname` = FreeBSD ]; then
    _BASE=${LOCALBASE:=/usr/local}
    export CPPFLAGS=-I${_BASE}/include
    export LDFLAGS=-L${_BASE}/lib
fi

. ${srcdir}/macros/autogen.sh
