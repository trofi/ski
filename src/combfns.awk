# -*- Mode: AWK; tab-width: 8; indent-tabs-mode: t; awk-basic-offset: 4 -*-

BEGIN {
    Emacs="/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t;" \
      " c-basic-offset: 4 -*- */\n"
    printf "%s", Emacs > CFILE
    printf "%s", Emacs > HFILE

    Warn="/* DO NOT EDIT - Automatically generated from:\n" \
	 "     awk -f combfns.awk execTbl\n" \
	 " */\n"
    printf "%s", Warn > CFILE
    printf "%s", Warn > HFILE
    while (getline ln < COPYRIGHT == 1) {
	print ln > CFILE
	print ln > HFILE
    }
    close(COPYRIGHT)
    print "#include \"ssDBT.h\"" > CFILE
    print "#include \"ssDDM.h\"" > CFILE
    print "#include \"trace.h\"" > CFILE
    print "#include \"exec.incl.c\"" > CFILE

    print "#include \"sim.h\"" > HFILE
    print "" > HFILE
    print "Status illComb(INSTINFO *info);" > HFILE
    print "Status illQpComb(INSTINFO *info);" > HFILE
    print "Status illBQpComb(INSTINFO *info);" > HFILE
    print "Status instFetchDecode(INSTINFO *info);" > HFILE
    print "Status lastInstComb(INSTINFO *info);" > HFILE
    print "Status ignComb(INSTINFO *info);" > HFILE
    print "Status movGrComb(INSTINFO *info);" > HFILE
}

/^#/ {
    next
}

function copyfile(fname) {
    x = getline ln < fname
    if (x == -1) {
	print "file " fname " not found"
	exit 1
    }
    if (x != 0)
	do
	    print ln > CFILE
	while (getline ln < fname == 1)
    close(fname)
}

{
    comb = $1 "Comb"
    if ($2 == "alias") {
	print "#define " comb " " $3 "Comb" > HFILE
	next
    }
    exfn = TEMP "/" $2
    rdfn = TEMP "/" $3
    wrfn = TEMP "/" $4
    retfn = rdfn ".return"
    print "extern Status " comb "(INSTINFO *info);" > HFILE
    print "" > CFILE
    print "Status " comb "(INSTINFO *info)" > CFILE
    print "{" > CFILE
    print "    REG src1, src2, src3, cntx, dst1, dst2;" > CFILE
    print "    BOOL srcnat1, srcnat2, srcnat3, dstnat1, dstnat2;" > CFILE
    print "    BOOL cmpres1, cmpres2, pdst1, qual;" > CFILE
    print "    FREG fsrc1, fsrc2, fsrc3, fdst1, fdst2;" > CFILE
    print "    HWORD syscallno; Status st = 0;" > CFILE
    print "    BYTE sofx, soilx, sorx, posx, lenx, mhtypex;" > CFILE
    print "    BYTE cpl, traps, ctrls;" > CFILE
    print "    REG arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7;" > CFILE
    print "    REG scStatus, scRetval;\n" > CFILE
    print "    /* suppress unused */" > CFILE
    print "    (void)src1;(void)src2;(void)src3;(void)cntx;(void)dst1;(void)dst2;" > CFILE
    print "    (void)srcnat1;(void)srcnat2;(void)srcnat3;(void)dstnat1;(void)dstnat2;" > CFILE
    print "    (void)cmpres1;(void)cmpres2;(void)pdst1;(void)qual;" > CFILE
    print "    (void)fsrc1;(void)fsrc2;(void)fsrc3;(void)fdst1;(void)fdst2;(void)syscallno;(void)st;" > CFILE
    print "    (void)sofx;(void)soilx;(void)sorx;(void)posx;(void)lenx;(void)mhtypex;" > CFILE
    print "    (void)cpl;(void)traps;(void)ctrls;" > CFILE
    print "    (void)arg0;(void)arg1;(void)arg2;(void)arg3;(void)arg4;(void)arg5;(void)arg6;(void)arg7;" > CFILE
    print "    (void)scStatus;(void)scRetval;" > CFILE
    copyfile(retfn)
    print "\treturn StSuccess;" > CFILE
    print "    {" > CFILE
    copyfile(rdfn)
    print "    }" > CFILE
    print "" > CFILE
    print "    {" > CFILE
    copyfile(exfn)
    print "    }" > CFILE
    print "" > CFILE
    print "    {" > CFILE
    copyfile(wrfn)
    print "    }" > CFILE
    print "    return StSuccess | st;" > CFILE
    print "}" > CFILE
}
