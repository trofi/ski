# -*- Mode: AWK; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-

/^#/ {
    next
}
/^[A-Z]/ {
    if (format) {
	cmd = "grep \"" format "	.*=\" " ENCODING_IMM
	while (cmd | getline ln == 1) {
	    split(ln,arr,"\t")
	    print " " arr[2]
	}
	close(cmd)
    }
    format = $1
    print
}
/^ / {
    print
}
END {
    if (format) {
	cmd = "grep \"" format "	.*=\" " ENCODING_IMM
	while (cmd | getline ln == 1) {
	    split(ln,arr,"\t")
	    print " " arr[2]
	}
	close(cmd)
    }
}
