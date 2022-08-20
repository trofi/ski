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
  printf("/* DO NOT EDIT - Automatically generated using:\n") > file;
  for (i = 0; i < ARGC; i++)
    printf(" %s", ARGV[i]) > file;
  printf("\n */\n\n") > file;
}

BEGIN {
    printEmacs(HFILE);
    printWarn(HFILE);
    while (getline ln < COPYRIGHT == 1) {
        print ln > HFILE
    }
    close(COPYRIGHT)

    print "" > HFILE
    print "/* $Head" "er$ */" > HFILE
    print "" > HFILE
    print "/* EM instruction formats header */" > HFILE
    print "" > HFILE
    print "#ifndef EMFORMATS_H" > HFILE
    print "#define EMFORMATS_H" > HFILE
    print "" > HFILE
    print "typedef enum {" > HFILE
    print "    EM_FORMAT_NONE = 0," > HFILE

    printWarn(CFILE);
    while (getline ln < TEMPLATE == 1) {
	print ln > CFILE
    }
    close(TEMPLATE);
    print "static char *formatName[NUM_FORMATS+1] = {" > CFILE
    print "    \"???\"," > CFILE
}

/^#/ {
    next
}

/^[A-Z]/ {
    print "    EM_FORMAT_" $1 "," > HFILE;
    print "    \"" $1 "\"," > CFILE;
    count++
}

/^ / {
    next
}

END {
    print "    EM_FORMAT_X41" > HFILE;
    print "} Format;" > HFILE;
    print "" > HFILE;
    print "#define NUM_FORMATS " count+1 > HFILE;
    print "" > HFILE;
    print "#endif /* EMFORMATS_H */" > HFILE;

    print "    \"X41\"" > CFILE;
    print "};" > CFILE;
}
