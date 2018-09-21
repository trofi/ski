# -*- Mode: AWK; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-

BEGIN {
    LET1="a"
    GFILE="asm_a.gperf"
    print "struct asm_id { char *name; InstID id; };" > GFILE
    print "%%" > GFILE
    opMap["ar.pfs"] = "A";
    opMap["ar.ccv"] = "A";
    opMap["ar.csd"] = "A";
    opMap["ar3"] = "A";
    opMap["cr3"] = "C";
    opMap["b1"] = "B";
    opMap["b2"] = "B";
    opMap["f1"] = "F";
    opMap["f2"] = "F";
    opMap["f3"] = "F";
    opMap["f4"] = "F";
    opMap["p1"] = "P";
    opMap["p2"] = "P";
    opMap["r0"] = "R";
    opMap["r1"] = "R";
    opMap["r2"] = "R";
    opMap["r3"] = "R";
    opMap["1"] = "I";
    opMap["8"] = "I";
    opMap["16"] = "I";
    opMap["count2"] = "I";
    opMap["count5"] = "I";
    opMap["count6"] = "I";
    opMap["fclass9"] = "I";
    opMap["amask7"] = "I";
    opMap["omask7"] = "I";
    opMap["pos6"] = "I";
    opMap["len4"] = "I";
    opMap["len6"] = "I";
    opMap["imm1"] = "I";
    opMap["imm2"] = "I";
    opMap["imm8"] = "I";
    opMap["imm9"] = "I";
    opMap["imm14"] = "I";
    opMap["imm21"] = "I";
    opMap["imm22"] = "I";
    opMap["imm24"] = "I";
    opMap["imm44"] = "I";
    opMap["imm62"] = "I";
    opMap["imm64"] = "I";
    opMap["inc3"] = "I";
    opMap["mask17"] = "I";
    opMap["mbtype4"] = "I";
    opMap["mhtype8"] = "I";
    opMap["tag13"] = "I";
    opMap["target25"] = "I";
    opMap["target64"] = "I";
    opMap["i"] = "I";
    opMap["l"] = "I";
    opMap["o"] = "I";
    opMap["r"] = "I";
    opMap["[r3]"] = "M";
    opMap["rr[r3]"] = "rr";
    opMap["dbr[r3]"] = "dbr";
    opMap["ibr[r3]"] = "ibr";
    opMap["pkr[r3]"] = "pkr";
    opMap["pmc[r3]"] = "pmc";
    opMap["pmd[r3]"] = "pmd";
    opMap["msr[r3]"] = "msr";
    opMap["dtr[r3]"] = "dtr";
    opMap["itr[r3]"] = "itr";
    opMap["cpuid[r3]"] = "cpuid";
}
/^#/ {
    next
}
{
    format = $1;
    mnem = $2;
    mnem_start = index($0, $2);
    first = substr(mnem,1,1);
    if (first != LET1) {
      close(GFILE);
      GFILE="asm_" first ".gperf";
      LET1 = first;
      print "struct asm_id { char *name; InstID id; };" > GFILE;
      print "%%" > GFILE
    }
    sub(/:.*$/, "")
    inst = substr($0, mnem_start)
    gsub(/-/, "m", inst)
    gsub(/[^a-zA-Z0-9.]/, "_", inst)
    gsub(/__+/, "_", inst)
    sub(/_$/, "", inst)
    instID = "EM_" toupper(inst)
    gsub(/\./, "_", instID)
    ops = ""
      for (i = 3; i <= NF; i++) {
	gsub(/[=,]/, "", $i);
	if ($i in opMap)
	  ops = ops opMap[$i];
	else
	  ops = ops $i;
      }

    if (ops == "")
      print mnem ", " instID > GFILE;
    else
      print mnem "_" ops ", " instID > GFILE
}
