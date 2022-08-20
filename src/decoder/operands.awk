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
    printEmacs(OPFILE);
    printWarn(OPFILE);
    while (getline ln < TEMPLATE == 1) {
	print ln > OPFILE
    }
    close(TEMPLATE);
}

/^#/ {
    next
}

{
    format = $1
    mnem = $2
    sub(/:.*$/, "")
    ops = ""
    for (i = 3; i <= NF; i++)
	ops = ops $i;
    gsub(/=/, ",", ops);
    gsub(/\./, "_", ops);
    gsub(/,8/, ",imm", ops);		# fix ldfp (M12)
    gsub(/[a-z]*\[r3\]/, "r3", ops);

    if (ops != "" && !seen[format]) {
      print "	struct { Operand " ops "; } " format ";" > OPFILE;
      seen[format] = 1;
    }
}

END {
    print "    } u;" > OPFILE
    print "    unsigned char qp;" > OPFILE
    print "    Flags flags;" > OPFILE
    print "} Operands, *OperandsPtr;" > OPFILE
    print "" > OPFILE
    print "#endif /* _OPERANDS_H */" > OPFILE
}
