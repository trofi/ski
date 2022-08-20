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


AM_CPPFLAGS= -I$(top_srcdir)/src/decoder -I$(top_builddir)/src/decoder \
			 -I$(top_srcdir)/src

BUILT_SOURCES= EMformats.h encode_table.c formats.c

noinst_LTLIBRARIES=libencoder.la

libencoder_la_SOURCES= \
	EMInst.h \
	encoder.h encoder.c \
	$(BUILT_SOURCES)

noinst_DATA= Copyright formats.c_template

noinst_SCRIPTS= formats.awk

EMformats.h formats.c: formats.awk formats.c_template \
					   $(encdir)/encoding.format Copyright
	rm -f $@;
	$(AWK) -f $(srcdir)/formats.awk \
		-v HFILE=EMformats.h \
		-v CFILE=formats.c \
		-v TEMPLATE=$(srcdir)/formats.c_template \
		-v COPYRIGHT=$(srcdir)/Copyright \
		$(encdir)/encoding.format

encode_table.c:
	@$(LN_S) $(encdir)/encoding.encode $@

EXTRA_DIST= $(noinst_DATA) $(noinst_SCRIPTS)

distclean-local:
	-rm -rf $(BUILT_SOURCES)
