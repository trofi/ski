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


SUBDIRS = fake-xterm

AM_CPPFLAGS = \
	$(X_CFLAGS) \
	$(DWARF_CFLAGS) \
	-I$(srcdir)/decoder \
	-I$(srcdir)/encoder \
	-I$(top_builddir)/src/encoder \
	-I$(top_builddir)/src/decoder \
	$(GSKI_CFLAGS) \
	-DSKI_DATA_DIR=\"$(pkgdatadir)\"

bin_PROGRAMS = ski

ENCODING_FILES = \
	encodings/encoding.decode \
	encodings/encoding.encode \
	encodings/encoding.format \
	encodings/encoding.imm \
	encodings/encoding.opcode \
	encodings/encoding.unusedop

noinst_SCRIPTS = \
	asm.awk \
	das.awk \
	mkfns.awk \
	opcode.awk \
	combfns.awk \
	fmtimm.awk \
	mkicnt.awk \
	predecode.awk \
	iaexec_dec.sed \
	iadas_dec.sed \
	ia_types.sed \
	make_asm.sh \
	\
	decoder/instID.awk \
	decoder/operands.awk \
	\
	encoder/formats.awk

noinst_DATA = \
	execTbl \
	$(ENCODING_FILES) \
	Copyright \
	\
	decoder/operands.h_template \
	\
	encoder/formats.c_template

COMBFNS_FILES = \
	exec.tmpl.c \
	fp_exec.tmpl.c \
	mem_exec.tmpl.c \
	mm_exec.tmpl.c \
	read.tmpl.c \
	sys_exec.tmpl.c \
	write.tmpl.c

COMBFNS_DERIVED = \
	combfns.gen.c \
	combfns.gen.h

PREDECODE_DERIVED = \
	predecode.gen.c \
	predecode.gen.h \
	immed.gen.h

IAEXEC_DECODER_DERIVED = \
	iaexec_decoder.gen.c

IADAS_DECODER_DERIVED = \
	iadas_decoder.gen.c

IADAS_TYPES_DERIVED = \
	iadas_types.gen.h

DAS_INSTR_DERIVED = \
	das_instr.gen.c

ASM_HASH_DERIVED = \
	asm_hash.gen.c

ICNT_DERIVED = \
	icnt_core.gen.h \
	icnt_ui.gen.h

DECODER_DERIVED = \
	decoder/decode_tree.gen.c \
	decoder/operands.gen.h \
	decoder/instID.gen.h

ENCODER_DERIVED = \
	encoder/EMformats.gen.h \
	encoder/encode_table.gen.c \
	encoder/formats.gen.c

EXPR_SCANNER_FILES = escan.l

EXPR_SCANNER_DERIVED = \
	escan.gen.h escan.gen.c

EXPR_PARSER_FILES = eparse.y

EXPR_PARSER_DERIVED = \
	eparse.gen.h eparse.gen.c

BUILT_SOURCES = \
	$(COMBFNS_DERIVED) \
	$(PREDECODE_DERIVED) \
	$(IAEXEC_DECODER_DERIVED) \
	$(IADAS_DECODER_DERIVED) \
	$(IADAS_TYPES_DERIVED) \
	$(DAS_INSTR_DERIVED) \
	$(ASM_HASH_DERIVED) \
	$(ICNT_DERIVED) \
	$(DECODER_DERIVED) \
	$(ENCODER_DERIVED) \
	$(EXPR_SCANNER_DERIVED) \
	$(EXPR_PARSER_DERIVED)
	instr.gen.c

noinst_HEADERS= \
	asm.h \
	bits.h \
	fields.h \
	libcore.h \
	libsym.h \
	simmem.h \
	os_support.h \
	platform.h \
	sim.h \
	ski.h \
	state.h \
	std.h \
	types.h \
	\
	tlb.h \
	instinfo.h \
	sign_ext.h \
	interruption.h \
	ia_types.h \
	ia_state.h \
	ia_exec.h \
	ssc.h \
	float.h \
	ia_read.h \
	ia_write.h \
	libtrace.h \
	trace.h \
	instr.h \
	ia_decoder.h \
	package.h \
	libsrs.h \
	lp64struct.h \
	memui.h \
	ia_das.h \
	das_instr.h \
	ssDCmd.h \
	lm.h \
	data.h \
	dos.h \
	program.h \
	libdas.h \
	netdev.h \
	osload.h

EXTRA_linux_FILES = \
	linux/dwarf-linux.c \
	linux/osload-linux.c \
	linux/syscall-linux.c \
	linux/syscall-linux.h \
	linux/syscall-print.h \
	linux/syscall-print.c

if LINUX
linux_FILES = $(EXTRA_linux_FILES)
else
linux_FILES =
endif

EXTRA_hpux_FILES = \
	hpux/dwarf-hpux.c \
	hpux/osload-hpux.c \
	hpux/syscall-hpux.c \
	hpux/syscall-hpux.h

if HPUX
hpux_FILES = $(EXTRA_hpux_FILES)
else
hpux_FILES =
endif

EXTRA_freebsd_FILES = \
	freebsd/convert.c \
	freebsd/dwarf.c \
	freebsd/firmware.c \
	freebsd/machdep.h \
	freebsd/misc.c \
	freebsd/osload.c \
	freebsd/signal.c \
	freebsd/ssc.c \
	freebsd/syscall.c \
	freebsd/syscall.h

if FREEBSD
freebsd_FILES = $(EXTRA_freebsd_FILES)
else
freebsd_FILES =
endif

ski_SOURCES = \
	main.c \
	\
	$(COMBFNS_DERIVED) \
	$(PREDECODE_DERIVED) \
	$(IAEXEC_DECODER_DERIVED) \
	$(ICNT_DERIVED) \
	state.c \
	sim.c \
	simmem.c \
	ski_elf.h \
	float.c \
	interruption.c \
	tlb.c \
	ia_read.c \
	ia_exec.c \
	ia_write.c \
	dos.c \
	memui.c \
	trace.c \
	libtrace.c \
	impl.c \
	impl.h \
	os_support.c \
	instr.gen.c \
	netdev.c \
	$(linux_FILES) \
	$(hpux_FILES) \
	$(freebsd_FILES) \
	libsym.c \
	$(ASM_HASH_DERIVED) \
	$(IADAS_DECODER_DERIVED) \
	$(IADAS_TYPES_DERIVED) \
	$(DAS_INSTR_DERIVED) \
	ski.c \
	menu.c \
	ssDBT.c \
	ssDBT.h \
	ssDDM.c \
	ssDDM.h \
	ssDCmd.c \
	program.c \
	data.c \
	exec.h \
	ssDSym.c \
	ssDSym.h \
	ssDPrs.c \
	ssDPrs.h \
	traceui.c \
	traceui.h \
	ia_das.c \
	ui.c \
	ui.h \
	batch.c \
	batch.h \
	cur.c \
	cur.h \
	ssX.c \
	ssX.h \
	ssXDSD.c \
	ssXDSD.h \
	ssGtk.c \
	ssGtk.h \
	syscall_api.h \
	asm.c \
	$(EXPR_SCANNER_DERIVED) \
	$(EXPR_PARSER_DERIVED) \
	load.c \
	load.h \
	platform.c \
	libdas.c \
	libsrs.c \
	\
	decoder/_decoder.h \
	decoder/decoder.c \
	decoder/decoder.h \
	decoder/parts.c \
	$(DECODER_DERIVED) \
	\
	encoder/EMInst.h \
	encoder/encoder.c \
	encoder/encoder.h \
	$(ENCODER_DERIVED)

if WITH_GTK_INTERFACE
# gski exports some callbacks to gski.glade for
# dynamic discovery.
ski_LDFLAGS=-Wl,--export-dynamic
endif

ski_LDADD = \
	$(DWARF_LIBS) \
	$(X_INTERFACE_LIBS) \
	$(GSKI_LIBS)

EXTRA_DIST = \
	exec.incl.c \
	ia_decoder.tmpl.c \
	$(COMBFNS_FILES) \
	$(gski_DATA) \
	$(noinst_DATA) \
	$(noinst_SCRIPTS) \
	$(EXPR_SCANNER_FILES) \
	$(EXPR_PARSER_FILES)

EXTRA_ski_SOURCES = \
	$(EXTRA_linux_FILES) \
	$(EXTRA_hpux_FILES) \
	$(EXTRA_freebsd_FILES)

if WITH_X_INTERFACE
XSKI= xski
else
XSKI=
endif

if WITH_GTK_INTERFACE
gskidir= $(datadir)/ski
gski_DATA= \
	gski.glade
GSKI= gski
else
GSKI=
endif

all-local: bski $(XSKI) $(GSKI)

bski $(XSKI) $(GSKI) : ski
	@rm -f $@; ln ski $@

## DERIVED sources rules:

das_instr.gen.c: das.awk $(srcdir)/encodings/encoding.opcode \
			 $(srcdir)/encodings/encoding.unusedop \
			 $(srcdir)/libdas.c $(srcdir)/Copyright
	@rm -f $(DAS_INSTR_DERIVED)
	$(AWK) -f $(srcdir)/das.awk \
		-v COPYRIGHT=$(srcdir)/Copyright \
		-v LIBDAS_C=$(srcdir)/libdas.c \
		-v CFILE=das_instr.gen.c \
		$(srcdir)/encodings/encoding.opcode \
		unused=1 $(srcdir)/encodings/encoding.unusedop

icnt_ui.gen.h: icnt_core.gen.h
icnt_core.gen.h: mkicnt.awk $(srcdir)/encodings/encoding.opcode \
				 $(srcdir)/encodings/encoding.unusedop \
				 $(srcdir)/Copyright
	@rm -f $(ICNT_DERIVED)
	$(AWK) -f $(srcdir)/mkicnt.awk \
		-v COPYRIGHT=$(srcdir)/Copyright \
		-v HFILE_UI=icnt_ui.gen.h \
		-v HFILE_CORE=icnt_core.gen.h \
		$(srcdir)/encodings/encoding.opcode \
		unused=1 $(srcdir)/encodings/encoding.unusedop
	touch icnt_ui.gen.h

asm_hash.gen.c: $(srcdir)/encodings/encoding.opcode \
			asm.awk make_asm.sh $(srcdir)/Copyright
	@rm -f $(ASM_HASH_DERIVED)
	@rm -rf asm_hash.gen
	sh $(srcdir)/make_asm.sh \
		-f $(srcdir)/asm.awk \
		-o $(srcdir)/encodings/encoding.opcode \
		-c asm_hash.gen.c \
		-e asm_hash.gen.err \
		-C $(srcdir)/Copyright \
		-T asm_hash.gen

iaexec_decoder.gen.c: $(srcdir)/ia_decoder.tmpl.c $(srcdir)/iaexec_dec.sed
	@rm -f $(IAEXEC_DECODER_DERIVED)
	sed -f $(srcdir)/iaexec_dec.sed $(srcdir)/ia_decoder.tmpl.c > iaexec_decoder.gen.c

iadas_decoder.gen.c: $(srcdir)/ia_decoder.tmpl.c $(srcdir)/iadas_dec.sed
	@rm -f $(IADAS_DECODER_DERIVED)
	sed -f $(srcdir)/iadas_dec.sed $(srcdir)/ia_decoder.tmpl.c > iadas_decoder.gen.c

iadas_types.gen.h: $(srcdir)/ia_types.h $(srcdir)/ia_types.sed
	@rm -f $(IADAS_TYPES_DERIVED)
	sed -f $(srcdir)/ia_types.sed $(srcdir)/ia_types.h > iadas_types.gen.h

combfns.gen.h: combfns.gen.c
combfns.gen.c: execTbl $(COMBFNS_FILES) mkfns.awk combfns.awk
	@rm -rf ./combfns.gen
	@mkdir combfns.gen
	@rm -f $(COMBFNS_DERIVED)
	$(AWK) -f $(srcdir)/mkfns.awk \
		-v TEMP=combfns.gen \
		$(srcdir)/exec.tmpl.c \
		$(srcdir)/fp_exec.tmpl.c \
		$(srcdir)/mem_exec.tmpl.c \
		$(srcdir)/mm_exec.tmpl.c \
		$(srcdir)/read.tmpl.c \
		$(srcdir)/sys_exec.tmpl.c \
		$(srcdir)/write.tmpl.c
	$(AWK) -f $(srcdir)/combfns.awk \
		-v TEMP=combfns.gen \
		-v CFILE=combfns.gen.c \
		-v HFILE=combfns.gen.h \
		-v COPYRIGHT=$(srcdir)/Copyright \
		$(srcdir)/execTbl
	@touch combfns.gen.h

predecode.gen.h: predecode.gen.c
immed.gen.h: predecode.gen.c
predecode.gen.c: $(srcdir)/encodings/encoding.format $(srcdir)/encodings/encoding.imm \
			 fmtimm.awk predecode.awk $(srcdir)/Copyright
	@rm -rf $(PREDECODE_DERIVED)
	$(AWK) -f $(srcdir)/fmtimm.awk \
		-v ENCODING_IMM=$(srcdir)/encodings/encoding.imm \
		$(srcdir)/encodings/encoding.format | \
		$(AWK) -f $(srcdir)/predecode.awk \
			-v CFILE=predecode.gen.c \
			-v PHFILE=predecode.gen.h \
			-v IHFILE=immed.gen.h \
			-v COPYRIGHT=$(srcdir)/Copyright \
			-
	@touch predecode.gen.h immed.gen.h

instr.gen.c: $(srcdir)/encodings/encoding.opcode \
		$(srcdir)/encodings/encoding.unusedop \
		opcode.awk execTbl $(srcdir)/Copyright
	@rm -f instr.gen.c
	$(AWK) -f $(srcdir)/opcode.awk \
		-v CFILE=instr.gen.c \
		-v COPYRIGHT=$(srcdir)/Copyright \
		-v EXECTBL=execTbl \
		$(srcdir)/encodings/encoding.opcode \
		unused=1 $(srcdir)/encodings/encoding.unusedop

## DERIVED decoder sources rules:

decoder/instID.gen.h: decoder/instID.awk $(srcdir)/encodings/encoding.opcode $(srcdir)/encodings/encoding.unusedop
	$(AWK) -f $(srcdir)/decoder/instID.awk \
		-v IDFILE=decoder/instID.gen.h \
		-v COPYRIGHT=$(srcdir)/Copyright \
		$(srcdir)/encodings/encoding.opcode \
		$(srcdir)/encodings/encoding.unusedop

decoder/operands.gen.h: decoder/operands.awk $(srcdir)/encodings/encoding.opcode decoder/operands.h_template
	$(AWK) -f $(srcdir)/decoder/operands.awk \
		-v OPFILE=decoder/operands.gen.h \
		-v TEMPLATE=$(srcdir)/decoder/operands.h_template \
		$(srcdir)/encodings/encoding.opcode

decoder/decode_tree.gen.c: $(srcdir)/encodings/encoding.decode
	cp $(srcdir)/encodings/encoding.decode $@

## DERIVED encoder sources rules:

encoder/EMformats.gen.h encoder/formats.gen.c: encoder/formats.awk encoder/formats.c_template \
					   $(srcdir)/encodings/encoding.format $(srcdir)/Copyright
	rm -f $@;
	$(AWK) -f $(srcdir)/encoder/formats.awk \
		-v HFILE=encoder/EMformats.gen.h \
		-v CFILE=encoder/formats.gen.c \
		-v TEMPLATE=$(srcdir)/encoder/formats.c_template \
		-v COPYRIGHT=$(srcdir)/Copyright \
		$(srcdir)/encodings/encoding.format

encoder/encode_table.gen.c: $(srcdir)/encodings/encoding.encode
	cp $(srcdir)/encodings/encoding.encode $@

## DERIVED lexers and parsers

escan.gen.h: escan.gen.c

escan.gen.c escan.gen.h: $(srcdir)/escan.l
	$(FLEX) $(LFLAGS) --outfile=escan.gen.c --header-file=escan.gen.h $(srcdir)/escan.l
	touch escan.gen.h

eparse.gen.h: eparse.gen.c

eparse.gen.c eparse.gen.h: eparse.y
	$(BISON) --warnings $(YFLAGS) --output=eparse.gen.c --header=eparse.gen.h $(srcdir)/eparse.y
	touch eparse.gen.h


distclean-local:
	-rm -f $(XSKI) $(GSKI) bski $(BUILT_SOURCES)
