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


AM_CPPFLAGS= -I$(top_srcdir)/src/encoder -I$(top_builddir)/src/encoder \
			 -I$(top_srcdir)/src

BUILT_SOURCES= \
	decode_tree.c \
	operands.h \
	instID.h \
	../encoder/EMformats.h

noinst_LTLIBRARIES=libdecoder.la

libdecoder_la_SOURCES= \
	_decoder.h \
	decoder.h decoder.c \
	parts.c \
	$(BUILT_SOURCES)

noinst_DATA= operands.h_template

noinst_SCRIPTS= \
	instID.awk \
	operands.awk

instID.h: instID.awk $(encdir)/encoding.opcode $(encdir)/encoding.unusedop
	$(AWK) -f $(srcdir)/instID.awk \
		-v IDFILE=instID.h \
		-v COPYRIGHT=$(srcdir)/../Copyright \
		$(encdir)/encoding.opcode \
		$(encdir)/encoding.unusedop

operands.h: operands.awk $(encdir)/encoding.opcode operands.h_template
	$(AWK) -f $(srcdir)/operands.awk \
		-v OPFILE=operands.h \
		-v TEMPLATE=$(srcdir)/operands.h_template \
		$(encdir)/encoding.opcode

decode_tree.c:
	@$(LN_S) $(encdir)/encoding.decode $@

encode_table.c:
	@$(LN_S) $(encdir)/encoding.encode $@

../encoder/EMformats.h: $(srcdir)/../encoder/formats.awk \
					    $(srcdir)/../encoder/formats.c_template \
					    $(encdir)/encoding.format \
					    $(srcdir)/../Copyright
	(cd ../encoder; $(MAKE) EMformats.h)

EXTRA_DIST= $(noinst_DATA) $(noinst_SCRIPTS)

distclean-local:
	-rm -rf $(BUILT_SOURCES)
