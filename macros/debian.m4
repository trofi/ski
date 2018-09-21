dnl debian.m4
dnl
dnl Copyright (C) 1995-2007, Hewlett-Packard Development Company, L.P.
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or 
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful, 
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of 
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License along
dnl with this program; if not, write to the Free Software Foundation, Inc.,
dnl 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
dnl
dnl 
dnl This macro allows an option to enable building on a debian system
dnl or not

AC_ARG_ENABLE(debian,
[  --enable-debian    Build for a Debian environment],
[case "${enableval}" in
  yes) debian=true ;;
  no)  debian=false ;;
  *)   AC_MSG_ERROR(bad value ${enableval} for --enable-debian) ;;
esac],[debian=false])
AM_CONDITIONAL(DEBIAN, test x$debian = xtrue)

