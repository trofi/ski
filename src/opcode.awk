#!/usr/bin/awk -f
# -*- Mode: AWK; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-

function printEmacs(file)
{
    Emacs="/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t;" \
      " c-basic-offset: 4 -*- */"
    printf "%s\n", Emacs > file
}

function printWarn(file, i, date)
{
  "date" | getline date
  printf "/* DO NOT EDIT - Automatically generated %s using:\n" \
	 "    ./opcode.awk", date > file
  for (i = 1; i < ARGC; i++)
	printf " %s", ARGV[i] > file
  print "\n */" > file
}

function printInstrData(pdec,comb,flags)
{
    print "{ " pdec ", " comb "," > CFILE
    print " " flags ", { 0ULL, 0ULL, 0ULL, 0ULL} }," > CFILE
    imports[pdec] = 0
    imports[comb] = 0
}

BEGIN {
    unused = 0
    CFILE="instr.c"
    printEmacs(CFILE)
    printWarn(CFILE)
    while (getline ln < (prefix "Copyright") == 1)
	printf "%s\n", ln > CFILE
    close((prefix "Copyright"))

    print "#include \"std.h\"" > CFILE
    print "#include \"types.h\"" > CFILE
    print "#include \"sim.h\"" > CFILE
    print "#include \"instr.h\"" > CFILE
    print "#include \"predecode.h\"" > CFILE
    print "#include \"combfns.h\"" > CFILE

    print "Instr baseInstrs[] = {" > CFILE

    printInstrData("illpredecode","illComb","0x0")
    printInstrData("illQPpredecode","illQpComb","0x1")
    printInstrData("illBQPpredecode","illBQpComb","0x1")
    printInstrData("ignpredecode","ignComb","0x0")
    printInstrData("lUnitImm41predecode","illComb","0x0")
    while (getline ln < ("execTbl") == 1)
	if (index(ln,"alias")) {
	    split(ln,arr)
	    temp = arr[3]
	    while (temp in exec_alias)
		temp = exec_alias[temp]
	    exec_alias[arr[1]] = temp
	}
}
/^#/ {
    next
}
{
    format = $1
    mnem = $2
    mnem_start = index($0, $2)
    semi = index($0, ";")
    pound = index($0, "#")
    slash = index($0, "/")
    if (pound) {
	flags = substr($0, semi+2, (pound-2)-(semi+2)+1)
	comment = substr($0, pound+2, length($0)-(pound+2)+1)
    } else
	flags = substr($0, semi+2, (slash-2)-(semi+2)+1)
    sub(/:.*$/, "")
    inst = substr($0, mnem_start)
    gsub(/-/, "m", inst)
    gsub(/[^a-zA-Z0-9]/, "_", inst)
    gsub(/__+/, "_", inst)
    sub(/_$/, "", inst)
    ops = ""
    for (i = 3; i <= NF; i++)
	ops = ops $i

    if (unused) {
	if (index(inst, "illegalOp") || index(inst, "ignoredOp"))
	    next
	# XXX - add support for multiple hint aliases
	equal = index(comment, " = ")
	slash = index(comment, "/")
	gsub(/[^a-zA-Z0-9]/, "_", comment)
	hint = substr(comment, 1, equal-1)
	alias = substr(comment, equal+3, (slash-2)-(equal+3)+1)
	if (alias == "none")
	    alias = ""
	gsub(hint, alias, inst)
    }
    if (inst in exec_alias)
	inst = exec_alias[inst]
    printInstrData(format "predecode",inst "Comb",flags)
}

END {
    print "};" > CFILE
    print "InstrPtr instrs = baseInstrs;" > CFILE
    print "PCF illCombFP = illComb;" > CFILE
    print "PCF illQpCombFP = illQpComb;" > CFILE
    print "PCF instFetchDecodeFP = instFetchDecode;" > CFILE
}
