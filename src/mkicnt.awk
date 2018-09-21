# -*- Mode: AWK; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-

BEGIN {
    unused = 0
    HFILE_UI="icnt_ui.h"
    HFILE_CORE="icnt_core.h"
    Emacs="/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t;" \
      " c-basic-offset: 4 -*- */\n"
    printf "%s", Emacs > HFILE_UI
    printf "%s", Emacs > HFILE_CORE

    Warn="/* DO NOT EDIT - Automatically generated using:\n" \
	 "     awk -f mkicnt.awk encodings/encoding.opcode\n" \
	 " */\n"
    printf "%s", Warn > HFILE_UI
    printf "%s", Warn > HFILE_CORE
    while (getline ln < (prefix "Copyright") == 1) {
	print ln > HFILE_UI
	print ln > HFILE_CORE
    }
    close((prefix "Copyright"))

    print "" > HFILE_UI
    print "typedef struct {" > HFILE_UI
    print "    char *name;" > HFILE_UI
    print "    ICNTS cnt;" > HFILE_UI
    print "} ICnt, *ICntPtr;" > HFILE_UI
    print "" > HFILE_UI
    print "static ICnt icnt[] = {" > HFILE_UI
    printf "    " > HFILE_UI

    print "" > HFILE_CORE
    print "extern Instr baseInstrs[];" > HFILE_CORE
    print "static struct {" > HFILE_CORE
    print "    char *name;" > HFILE_CORE
    print "    ICNTS *cnt;" > HFILE_CORE
    print "} icnt[] = {" > HFILE_CORE
    printf "    " > HFILE_CORE
}

/^#/ {
    next
}
{
    mnem = $2
    mnem_start = index($0, $2)
    sub(/:.*$/, "")
    inst = substr($0, mnem_start)
    gsub(/-/, "m", inst)
    gsub(/[^a-zA-Z0-9]/, "_", inst)
    gsub(/__+/, "_", inst)
    sub(/_$/, "", inst)
    if (unused && (index(inst, "illegalOp") || index(inst, "ignoredOp")))
	next
    inst = "EM_" toupper(inst)
    ops = ""
    for (i = 3; i <= NF; i++)
	ops = ops $i
    if (ops != "")
	ops = " " ops
    if (needcomma) {
	printf ",\n    " > HFILE_UI
	printf ",\n    " > HFILE_CORE
    }
    printf "{\"%s%s\", {0}}", mnem, ops > HFILE_UI
    printf "{\"%s%s\", &baseInstrs[%s].cnt}", mnem, ops, inst > HFILE_CORE
    needcomma = 1
}

END {
    printf "\n};\n" > HFILE_UI
    printf "\n};\n" > HFILE_CORE
}
