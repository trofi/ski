dnl sed.m4
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
dnl This macro verifies that a copy of sed exists

AC_DEFUN([AC_PROG_SED],
[
   AC_CHECK_PROG(SED, sed, sed, /bin/sed)
])
