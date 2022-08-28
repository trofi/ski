# -*- Mode: AWK; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-

BEGIN {
    Emacs="/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t;" \
      " c-basic-offset: 4 -*- */\n"
    printf "%s", Emacs > CFILE
    printf "%s", Emacs > PHFILE
    printf "%s", Emacs > IHFILE

    Warn="/* DO NOT EDIT - Automatically generated using:\n" \
	 "     awk -f fmtimm.awk encodings/encoding.format | awk -f predecode.awk -\n" \
	 " */\n"
    printf "%s", Warn > CFILE
    printf "%s", Warn > PHFILE
    printf "%s", Warn > IHFILE
    while (getline ln < COPYRIGHT == 1) {
	print ln > CFILE
	print ln > PHFILE
	print ln > IHFILE
    }
    close(COPYRIGHT)

    print "#include \"sim.h\"" > PHFILE
    print "#include \"std.h\"" > PHFILE
    print "" > PHFILE
    print "BOOL illpredecode(DWORD, INSTINFO *);" > PHFILE
    print "BOOL illQPpredecode(DWORD, INSTINFO *);" > PHFILE
    print "BOOL illBQPpredecode(DWORD, INSTINFO *);" > PHFILE
    print "BOOL ignpredecode(DWORD, INSTINFO *);" > PHFILE
    print "BOOL lUnitImm41predecode(DWORD, INSTINFO *);" > PHFILE

    print "#include \"std.h\"" > CFILE
    print "#include \"bits.h\"" > CFILE
    print "#include \"types.h\"" > CFILE
    print "#include \"sim.h\"" > CFILE
    print "#include \"state.h\"" > CFILE
    print "#include \"instinfo.h\"" > CFILE
    print "#include \"immed.h\"" > CFILE
    print "#include \"predecode.h\"" > CFILE
    print "" > CFILE
    print "static DWORD imm41;" > CFILE
    print "static ADDR ippg;" > CFILE
    print "" > CFILE
    print "BOOL illpredecode(DWORD instruct, INSTINFO *info)" > CFILE
    print "{" > CFILE
    print "    qp = 0;" > CFILE
    print "    imm64 = instruct;" > CFILE
    print "    return NO;" > CFILE
    print "}" > CFILE
    print "" > CFILE
    print "BOOL illQPpredecode(DWORD instruct, INSTINFO *info)" > CFILE
    print "{" > CFILE
    print "    qp = BitfX(instruct,58,6);" > CFILE
    print "    imm64 = instruct;" > CFILE
    print "    return NO;" > CFILE
    print "}" > CFILE
    print "" > CFILE
    print "BOOL illBQPpredecode(DWORD instruct, INSTINFO *info)" > CFILE
    print "{" > CFILE
    print "    qp = BitfX(instruct,58,6);" > CFILE
    print "    imm64 = instruct;" > CFILE
    print "    return NO;" > CFILE
    print "}" > CFILE
    print "" > CFILE
    print "BOOL lUnitImm41predecode(DWORD instruct, INSTINFO *info)" > CFILE
    print "{" > CFILE
    print "    qp = 0;" > CFILE
    print "    imm41 = instruct;" > CFILE
    print "    return YES;" > CFILE
    print "}" > CFILE
    print "" > CFILE
    print "BOOL ignpredecode(DWORD instruct, INSTINFO *info)" > CFILE
    print "{" > CFILE
    print "    qp = 0;" > CFILE
    print "    imm64 = instruct;" > CFILE

    print "#include \"sign_ext.h\"" > IHFILE
    print "#define IP (imm64 & ~(ADDR)0xF)" > IHFILE
    print "#define PG(a) ((a) & ~(ADDR)0xFFF)" > IHFILE
}

function intDestCode()
{
    print "    if (!r1)" > CFILE
    print "\treturn NO;" > CFILE
    print "    if (r1 < 32) {" > CFILE
    print "\tinfo->pgrtgt = r1 + 1;" > CFILE
    print "    }" > CFILE
}

/^[A-Z]/ {
    if (intdest)
	intDestCode()
    print "    return YES;" > CFILE
    print "}" > CFILE
    print "" > CFILE
    format = $1
    intdest = 0
    print "BOOL " format "predecode(DWORD, INSTINFO *);" > PHFILE
    print "BOOL " format "predecode(DWORD instruct, INSTINFO *info)" > CFILE
    print "{" > CFILE
    print "    qp = 0;" > CFILE
    print "    info->samepg = 0;" > CFILE
    print "    info->pgrtgt = 0;" > CFILE
    print "    info->pgrr2 = info->pgrr3 = 0;" > CFILE
    if (format == "I25")	# XXX - hack so "mov r1=ip" gets ip
	print "    imm64 = IP;" > CFILE
    immeds = ""
    immx = ""
}

function getField(imm) {
    lb = index($1, "{")
    colon = index($1, ":")
    rb = index($1, "}")

    field = substr($1,1,lb-1)
    if (field == "0")
	  return
    if (colon) {
	left = substr($1,lb+1,colon-lb)
	right = substr($1,colon+1,rb-colon)
    } else
	left = right = substr($1,lb+1,rb-lb)
    if (field == "imm41")	# special case for imm41 field of movl
	bitfr = field
    else if (field == "imm39")	# special case for imm39 field of brl
	bitfr = "BitfX(imm41," 63-left "," left-right+1 ")"
    else
	bitfr = "BitfX(instruct," 63-left "," left-right+1 ")"
    if (imm) {
	immeds = "," bitfr immeds
	immx = "," field immx
    } else
	print "    " field " = " bitfr ";" > CFILE
}

/Address Source/ {
    getField(0)
}

/Branch Dest/ {
    getField(0)
#    if (format ~ "B" || format ~ "X")
#	print "    retIP = IP + 16;" > CFILE
}

/Branch Source/ {
    getField(0)
}

/Branch Tag/ {
    getField(1)
}

/FP Dest/ {
    getField(0)
}

/FP Source/ {
    getField(0)
}

/Immediate/ {
    getField(1)
}

/Indirect Source/ {
    getField(0)
}

/Int Dest/ {
    getField(0)
    intdest = 1
}

/\(Int Source\)/ {
    getField(0)
    s = substr($1,1,2)
    if (s == "r2" || s == "r3") {
	print "    if (" s " < 32) {" > CFILE
	print "\tinfo->pgr" s " = " s " + 1;" > CFILE
	print "    }" > CFILE
    }
}

/Mem Source & Dest/ {
    getField(0)
}

/\(Mem Source\)/ {
    getField(0)
}

/Pred Dest/ {
    getField(0)
}

/Qual Pred/ {
    getField(0)
}

/Shift Imm/ {
    getField(1)
}

/Shift Source/ {
    getField(0)
}

/Special Dest/ {
    getField(0)
}

/Special Source/ {
    getField(0)
}

#/=.*sign_ext/ {
/=/ {
    gsub(/@/, "_", $0)
    if ($1 == "target25" || $1 == "target64")
	print "    ippg = PG(IP);" > CFILE
    print "    " $1 " = " $1 format "(" substr(immeds,2) ");" > CFILE
    if ($1 == "target25")
	print "    info->samepg = ippg == PG(target25);" > CFILE
    else if ($1 == "target64")
	print "    info->samepg = ippg == PG(target64);" > CFILE
    print "#define " $1 format "(" substr(immx,2) ") (" substr($0,index($0,"=")+2) ")" > IHFILE
}

END {
    if (intdest)
	intDestCode()
    print "    return YES;" > CFILE
    print "}" > CFILE
}
