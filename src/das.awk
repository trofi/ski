#!/usr/bin/awk -f
# -*- Mode: AWK; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-

function printEmacs(file)
{
    Emacs="/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t;" \
      " c-basic-offset: 4 -*- */"
    printf "%s\n", Emacs > file
}

function printWarn(file)
{
  printf("/* DO NOT EDIT - Automatically generated using:\n   ") > file;
  for (i = 0; i < ARGC; i++)
    printf(" %s", ARGV[i]) > file;
  printf("\n */\n\n") > file;
}

BEGIN {
    unused = 0
    printEmacs(CFILE)
    printWarn(CFILE)
    while (getline ln < COPYRIGHT == 1) {
	print ln > CFILE
    }
    close(COPYRIGHT);

    while (getline ln < LIBDAS_C == 1) {
      if (ln ~ "^\t{EM_") {
	sub(/,.*$/, "", ln);
	instID = substr(ln, 3);
	if (!(instID in pseudo))
	  pseudo[instID] = pnum;
	pnum++;
      }
    }
    close(LIBDAS_C);

    print "" > CFILE
    print "#include \"das_instr.h\"" > CFILE
    print "" > CFILE
    print "DasInstr dasBaseInstrs[] = {" > CFILE
    print "  { \"illegalOp\", -1, -1, 0 }," > CFILE
    print "  { \"illegalOp\", -1, -1, 0 }," > CFILE
    print "  { \"illegalOp\", -1, -1, 0 }," > CFILE
    print "  { \"ignoredOp\", -1, -1, 0 }," > CFILE
    print "  { \"imm41\", -1, -1, 0 }," > CFILE
}

/^#/ {
    next
}

{
    format = $1
    mnem = $2
    semi = index($0, ";");
    pound = index($0, "#");
    flags = 0;
    sub(/:.*$/, "")
    inst = substr($0,5)
    gsub(/-/, "m", inst)
    gsub(/[^a-zA-Z0-9.]/, "_", inst)
    gsub(/__+/, "_", inst)
    sub(/_$/, "", inst)
    instID = "EM_" toupper(inst)
    gsub(/\./, "_", instID)
    if (unused) {
      if (index(inst, "illegalOp") || index(inst, "ignoredOp"))
	next;
    }
    ops = "";
    equal = 0;
    target = tag = -1;
    for (i = 3; i <= NF; i++) {
      if ($i == "=")
	equal = 1;
      if (index($i, "target"))
	target = i - equal - 3;
      if (index($i, "tag"))
	tag = i - equal - 3;
    }

    printf("  { \"%s\", %d, %d, %d },\n", mnem, target, tag, pseudo[instID]) > CFILE
}

END {
    print "};" > CFILE
    print "" > CFILE
    print "DasInstrPtr dasInstrs = dasBaseInstrs;" > CFILE
}
