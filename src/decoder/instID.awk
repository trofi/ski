#!/usr/bin/awk -f
# -*- Mode: AWK; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-

function printEmacs(file)
{
    Emacs="/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t;" \
      " c-basic-offset: 4 -*- */"
    printf "%s\n", Emacs > file
}

function printWarn(file, i)
{
  printf("/* DO NOT EDIT - Automatically generated using:\n") > file;
  for (i = 0; i < ARGC; i++)
    printf(" %s", ARGV[i]) > file;
  printf("\n */\n\n") > file;
}

BEGIN {
    IDFILE="instID.h";
    printEmacs(IDFILE);
    printWarn(IDFILE);
    while (getline ln < (prefix "Copyright") == 1) {
	print ln > IDFILE
    }
    close((prefix "Copyright"))

    print "/* $Head" "er$ */" > IDFILE
    print "" > IDFILE
    print "/* Inst ID header file for encoder/decoder libraries */" > IDFILE
    print "" > IDFILE
    print "#ifndef INSTID_H" > IDFILE
    print "#define INSTID_H" > IDFILE
    print "" > IDFILE
    print "typedef" > IDFILE
#    print "#if !defined __cplusplus && !defined __GNUC__ && !defined __INTEL_COMPILER" > IDFILE
#    print "short" > IDFILE
#    print "#endif" > IDFILE
    print "enum {" > IDFILE
    print "    EM_INST_NONE = 0," > IDFILE
    print "    EM_ILLEGALOP = EM_INST_NONE," > IDFILE
    print "    EM_ILLEGALOPQP," > IDFILE
    print "    EM_ILLEGALOPBQP," > IDFILE
    print "    EM_IGNOREDOP," > IDFILE
    print "    EM_LUNIT_IMM41," > IDFILE
}

/^#/ {
  next;
}

/^[A-Z]0/ {
  next;
}

{
    format = $1
    mnem = $2
    mnem_start = index($0, $2);
    sub(/:.*$/, "")
    inst = substr($0, mnem_start)
    gsub(/-/, "m", inst)
    gsub(/[^a-zA-Z0-9]/, "_", inst)
    gsub(/__+/, "_", inst)
    sub(/_$/, "", inst)
    instID = "EM_" toupper(inst)
    print "    " instID "," > IDFILE
}

END {
    print "    EM_INST_LAST" > IDFILE
    print "} InstID;" > IDFILE
    print "" > IDFILE
    print "#endif /* INSTID_H */" > IDFILE
}
