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

# AC_LANG_COMPILER_INTEL
# ----------------------
m4_define([_AC_LANG_COMPILER_INTEL],
[AC_CACHE_CHECK([whether we are using Intel _AC_LANG compiler],
                [ac_cv_[]_AC_LANG_ABBREV[]_compiler_intel],
[_AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [[
#ifndef __INTEL_COMPILER
       choke me
#endif
]])],
                   [ac_compiler_intel=yes],
                   [ac_compiler_intel=no])
ac_cv_[]_AC_LANG_ABBREV[]_compiler_intel=$ac_compiler_intel
])])# _AC_LANG_COMPILER_INTEL

# AC_LANG_COMPILER_HP
# -------------------
m4_define([_AC_LANG_COMPILER_HP],
[AC_CACHE_CHECK([whether we are using HP _AC_LANG compiler],
                [ac_cv_[]_AC_LANG_ABBREV[]_compiler_hp],
[_AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [[
#if ( !defined( __cplusplus ) && (!defined(__hpux) || defined(__GNUC__) ) ) \
    || ( defined( __cplusplus ) && !defined( __HP_aCC ) )
       choke me
#endif
]])],
                   [ac_compiler_hp=yes],
                   [ac_compiler_hp=no])
ac_cv_[]_AC_LANG_ABBREV[]_compiler_hp=$ac_compiler_hp
])])# _AC_LANG_COMPILER_HP

AC_DEFUN([AC_LANG_COMPILER_CHECK],[
# Check for Intel & HP's C compilers
AC_LANG_PUSH(C)dnl
_AC_LANG_COMPILER_INTEL
ICC=`test $ac_compiler_intel = yes && echo yes`
_AC_LANG_COMPILER_HP
HPCC=`test $ac_compiler_hp = yes && echo yes`
AC_LANG_POP(C)dnl
])# AC_LANG_COMPILER_CHECK
