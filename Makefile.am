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

SUBDIRS=libltdl macros src doc

appdefaultsdir=$(libdir)/X11/app-defaults
appdefaults_DATA=XSki

bin_SCRIPTS=ski-config

BUILT_SOURCES=ski-config

EXTRA_DIST=ski.spec.in XSki.freebsd XSki.hpux XSki.linux \
	misc/bskinc misc/ia64fmt

XSki:
	@$(LN_S) $(srcdir)/XSki.$(HOST_OS) $@

.PHONY: files release sanity snapshot pubdist

glib-config: glib-config.in

files:
	@files=`ls $(DISTFILES) 2> /dev/null `; for p in $$files; do \
		echo $$p; \
	done

release:
	$(MAKE) distcheck

snapshot:
	$(MAKE) dist distdir=$(PACKAGE)-`date +"%Y%m%d"`

distclean-hook:
	rm -f XSki

dist-hook: ski.spec
	cp ski.spec $(distdir)

rpm: $(PACKAGE).spec dist
	build_dir=`cd .; pwd` ; \
	rpm --define "_topdir $$build_dir"   --define "_sourcedir $$build_dir" \
	   --define "_srcrpmdir $$build_dir" --define "_builddir $$build_dir"  \
	   --define "_rpmdir $$build_dir"    --define "_target_cpu @host_cpu@" \
	   -ba --clean $(PACKAGE).spec

pubdist: src/ski
	mkdir -p usr/bin usr/man/man1 usr/X11R6/lib/X11/app-defaults;	     \
	cp ../../XSki usr/X11R6/lib/X11/app-defaults;			     \
	ln -s ski.1 usr/man/man1/xski.1;				     \
	ln -s ski.1 usr/man/man1/bskinc.1;			     \
	objcopy --strip-all ../../src/ski usr/bin/ski;		     \
	ln -s ski usr/bin/xski;					     \
	ln -s ski usr/bin/bski;					     \
	ln -s ski usr/bin/bskinc;					     \
	cd ..;								     \
	tar czf								     \
	  ../$(PACKAGE)-$(SKI_VERSION)-$(SKI_RELEASE)-`uname -m`.tar.gz .
