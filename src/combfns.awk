# -*- Mode: AWK; tab-width: 8; indent-tabs-mode: t; awk-basic-offset: 4 -*-

BEGIN {
    CFILE="combfns.c"
    HFILE="combfns.h"
    Emacs="/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t;" \
      " c-basic-offset: 4 -*- */\n"
    printf "%s", Emacs > CFILE
    printf "%s", Emacs > HFILE

    Warn="/* DO NOT EDIT - Automatically generated from:\n" \
	 "     awk -f combfns.awk execTbl\n" \
	 " */\n"
    printf "%s", Warn > CFILE
    printf "%s", Warn > HFILE
    while (getline ln < (prefix "Copyright") == 1) {
	print ln > CFILE
	print ln > HFILE
    }
    close((prefix "Copyright"))
    print "#include \"exec_hd.c\"" > CFILE
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
    exfn = "temp/" $2
    rdfn = "temp/" $3
    wrfn = "temp/" $4
    retfn = rdfn ".return"
    print "extern Status " comb "(INSTINFO *info);" > HFILE
    printf "\nStatus " comb "(INSTINFO *info)\n{\n" > CFILE
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
    printf "    {\n" > CFILE
    copyfile(rdfn)
    print "    }" > CFILE
    printf "\n    {\n" > CFILE
    copyfile(exfn)
    printf "    }\n\n    {\n" > CFILE
    copyfile(wrfn)
    printf "    }\n" > CFILE
    print "    if (0) {" > CFILE
    print "      src1=0;src2=0;src3=0;cntx=0;dst1=0;dst2=0;" > CFILE
    print "      srcnat1=0;srcnat2=0;srcnat3=0;dstnat1=0;dstnat2=0;" > CFILE
    print "      cmpres1=0;cmpres2=0;pdst1=0;qual=0;" > CFILE
    print "      fsrc1.mant=0;fsrc2.mant=0;fsrc3.mant=0;" > CFILE
    print "      fdst1.mant=0;fdst2.mant=0;syscallno=0;" > CFILE
    print "      sofx=0;soilx=0;sorx=0;posx=0;lenx=0;mhtypex=0;" > CFILE
    print "      cpl=0;traps=0;ctrls=0;" > CFILE
    print "      arg0=arg1=arg2=arg3=arg4=arg5=arg6=arg7=0;" > CFILE
    print "      scStatus=0;scRetval=0;\n" > CFILE
    print "    }" > CFILE
    printf "    return StSuccess | st;\n}\n" > CFILE
}
