## Process this file with automake to produce makefile.in
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

SUBDIRS=macros src doc

if WITH_X_INTERFACE
appdefaultsdir=/etc/X11/app-defaults
appdefaults_DATA=XSki
# This is a bit of a hack.  appdefaults_DATA doesn't create a target
# for each argument, hence this doesn't get built.  We therefore add it
# to BUILT_SOURCES conditionally -ianw
BUILD_XSKI=XSki
XSki:
	@$(LN_S) $(srcdir)/XSki.$(HOST_OS) $@
else
BUILD_XSKI=
endif

BUILT_SOURCES=$(BUILD_XSKI)

EXTRA_DIST=XSki.freebsd XSki.hpux XSki.linux \
	misc/bskinc misc/ia64fmt ChangeLog.2007

distclean-hook:
	rm -f XSki

install-exec-hook:
	$(LN_S) -f ski $(DESTDIR)/$(bindir)/bski;
	$(LN_S) -f ski.1 $(DESTDIR)/$(mandir)/man1/bski.1;
	$(LN_S) -f ski $(DESTDIR)/$(bindir)/bskinc;
	$(LN_S) -f ski.1 $(DESTDIR)/$(mandir)/man1/bskinc.1;
if WITH_X_INTERFACE
	$(LN_S) -f ski $(DESTDIR)/$(bindir)/xski;
	$(LN_S) -f ski.1 $(DESTDIR)/$(mandir)/man1/xski.1;
endif
if WITH_GTK_INTERFACE
	$(LN_S) -f ski $(DESTDIR)/$(bindir)/gski
	$(LN_S) -f ski.1 $(DESTDIR)/$(mandir)/man1/gski.1
endif	
