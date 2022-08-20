# -*- Mode: AWK; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-

/^Status/ {
    split($2, arr, "(")
    file = TEMP"/"arr[1]
    system("touch " file)
    next
}

/^void.*INSTINFO/ {
    split($2, arr, "(")
    file = TEMP"/"arr[1]
    system("touch " file)
    next
}

/^\{/ {
    if (file != "")
	inproc = 1
    next
}

/^    return/ {
    if (inproc) {
	retfile = file".return"
	printf "    if (%s != 1)\n", substr($2,1,length($2)-1) > retfile
	close(retfile)
	next
    }
}

/^\treturn;/ {
    if (inproc) {
	print "\treturn StFault;" > file
	next
    }
}

/^}/ {
    inproc = 0
    close(file)
    file = ""
    next
}

{
    if (inproc)
	print > file
}
