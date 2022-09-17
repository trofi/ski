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

# Use --install to put put thirt-party .m4 files into m4/ instead of
# globbing all together into aclocal.m4.
ACLOCAL_AMFLAGS = -I m4 -I macros --install

SUBDIRS = src doc

SKI_BOOTLOADER_FILES = \
	ski-bootloader/include/ia64/setup.h \
	ski-bootloader/include/ia64/pal.h \
	ski-bootloader/include/ia64/sal.h \
	ski-bootloader/include/efi.h \
	ski-bootloader/include/elf.h \
	ski-bootloader/include/types.h \
	ski-bootloader/Makefile \
	ski-bootloader/README.md \
	ski-bootloader/boot_head.S \
	ski-bootloader/bootloader.c \
	ski-bootloader/bootloader.h \
	ski-bootloader/bootloader.lds \
	ski-bootloader/fw-emu.c \
	ski-bootloader/ssc.h

EXTRA_DIST = \
	misc/bskinc \
	misc/ia64fmt \
	ChangeLog.2007 \
	$(SKI_BOOTLOADER_FILES)

install-exec-hook:
	$(LN_S) -f ski $(DESTDIR)/$(bindir)/bski;
	$(LN_S) -f ski.1 $(DESTDIR)/$(mandir)/man1/bski.1;
	$(LN_S) -f ski $(DESTDIR)/$(bindir)/bskinc;
	$(LN_S) -f ski.1 $(DESTDIR)/$(mandir)/man1/bskinc.1;
if WITH_GTK_INTERFACE
	$(LN_S) -f ski $(DESTDIR)/$(bindir)/gski
	$(LN_S) -f ski.1 $(DESTDIR)/$(mandir)/man1/gski.1
endif
