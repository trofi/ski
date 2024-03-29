#!/bin/sh
#
# Copyright (C) 1995-2007, Hewlett-Packard Development Company, L.P.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or 
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, 
# but WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
#

(gperf --version) < /dev/null > /dev/null 2>&1 || { 
    echo 
    echo "**Error**: You must have \`gperf' installed to generate asm_hash.c" 
    exit 1
}

srcdir=`dirname $0`
test -z "${srcdir}" && srcdir=.

awk_file=$srcdir/asm.awk
C_file=$srcdir/Copyright
c_file="asm_hash.gen.c"
err_file="asm_hash.gen.err"
temp_dir="asm_hash.gen"
opcode_file=$srcdir/encodings/encoding.opcode
cmd_opts=""
awk="all"
lets="a b c d e f g h i j k l m n o p q r s t u v w x y z"

while getopts AC:c:de:f:l:o:ST: name
do
  case $name in
  A) awk="only";;
  C) C_file="$OPTARG";;
  c) c_file="$OPTARG";;
  d) cmd_opts="$cmd_opts -$name";;
  e) err_file="$OPTARG";;
  f) awk_file="$OPTARG";;
  l) lets="$OPTARG";;
  o) opcode_file="$OPTARG";;
  S) awk="skip";;
  T) temp_dir="$OPTARG";;
  ?) printf "Usage: %s [-A] [-C Copyright] [-c c_file] [-d] [-e err_file] [-f awk_file] [-l lets] [-o opcode_file] [-S] [-T temp_dir]\n" $0 1>&2
     exit 2;;
  esac
done

if [ $awk != "skip" ]
then
    mkdir $temp_dir
    sort -b -k2 < $opcode_file | \
        awk -f $awk_file -v TEMP="$temp_dir" -
fi

if [ $awk = "only" ]
then
    exit
fi

#                                                  keywords  hash entries   min
a_opts="-j 1 -n -k 2,4,6,8"                      #       11,           11     *
b_opts="-D -j 3 -k 3-5,7-11,13-15,16,17,19-21,23-25,$" #      471,         5253
c_opts="-o -j 1 -n -k 4-8,10-16,18-20"           #      117,          280
d_opts="-n -k 6,8"                               #        4,            4     *
e_opts="-n -k 2,5"                               #        3,            3     *
f_opts="-o -j 1 -k 2-18"                         #      242,          796
g_opts="-n -k 6-7"                               #        4,            4     *
h_opts="-o -j 1 -n -k 2,6"                       #        7,            7     *
i_opts="-j 1 -n -k 3,5,10"                       #        7,            7     *
j_opts=""                                        #        0,            0     #
k_opts=""                                        #        0,            0     #
#l_opts="-o -j 3 -k 3-17,21,$"                    #      715,         4247
#l_opts="-o -j 3 -k 2,3-17,21,$"                  #      721,         5747
#l_opts="-o -j 1 -k 2,3-17,21,$"                  #      721,         4876
l_opts="-o -j 1 -k 3-17,21,$"                    #      721,         4643
m_opts="-D -o -j 1 -k 4-12,14-17,19,21-23,$"     #      235,         1164
n_opts="-j 1 -n -k 5"                            #        5,            5     *
o_opts="-n -k 5"                                 #        2,            2     *
p_opts="-o -j 1 -k 3-5,7-9,11"                   #       70,          111
q_opts=""                                        #        0,            0     #
r_opts="-n -k 2,4"                               #        4,            4     *
s_opts="-j 1 -k 2-4,6"                           #       77,          135
t_opts="-j 1 -n -k 2,6,10,11"                    #       20,           20     *
u_opts="-j 1 -n -k 7,9"                          #        6,            6     *
v_opts=""                                        #        0,            0     #
w_opts=""                                        #        0,            0     #
x_opts="-o -j 1 -n -k 5,6,9"                     #       17,           17     *
y_opts=""                                        #        0,            0     #
z_opts="-n -k 4"                                 #        3,            3     *

# old attempts at optimal option values
#a_opts="-n -k 2,4,6,8"                          #       11,           31
#a_opts="-j 3 -n -k 2,4,6,8"                     #       11,           19
#c_opts="-n -k 4-8,10-16,19,20"                  #      110,          460
#c_opts="-j 1 -n -k 4-8,10-16,19,20"             #      110,          318
#c_opts="-o -j 1 -n -k 4-8,10-16,19,20"           #      110,          304
#f_opts="-n -k 2-18"                             #      242,         1239
#i_opts="-k 5,$"                                 #        7,           18
#i_opts="-j 1 -k 5,$"                            #        7,           17
#l_opts="-j 3 -k 3-17,21,$"                      #      715,         6003
#m_opts="-k 4-12,14-17,19,21-23,$"               #      235,         1322
#n_opts="-n -k 5"                                #        5,            8
#n_opts="-j 3 -n -k 5"                           #        5,            7
#p_opts="-n -k 3,4,5,7,8,9,$"                    #       70,          378
#p_opts="-j 3 -n -k 3,4,5,7,8,9,$"               #       70,          313
#p_opts="-j 1 -n -k 3,4,5,7,8,9,$"               #       70,          213
#p_opts="-j 1 -k 3,4,5,7,8,9,$"                  #       70,          155
#p_opts="-j 1 -k 3,4,5,7,8,9,11"                 #       70,          148
#s_opts="-k 2-6"                                 #       77,          243
#s_opts="-k 2,3,4,6"                             #       77,          227
#s_opts="-j 3 -k 2,3,4,6"                        #       77,          192
#t_opts="-k 2,8,$"                               #       20,           46
#t_opts="-j 1 -k 2,8"                            #       20,           36
#t_opts="-j 1 -k 2,10"                           #       20,           28
#t_opts="-o -j 1 -n -k 2,10,11"                  #       20,           26
#x_opts="-j 1 -n -k 5,6,9"                       #       17,           31

cat << EOF > $c_file
/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* DO NOT EDIT - Automatically generated using:
     make_asm.sh
 */
EOF
cat $C_file >> $c_file
cat << EOF >> $c_file

#include <asm.h>
EOF
for let in $lets
do
  file="$temp_dir/asm_$let.gperf"
  if [ -r $file ]
  then
    echo $file":" 1>&2
    echo ""
    eval gperf -t -L ANSI-C -p -C -E -H ${let}_hash -N ${let}_lookup \
     \$${let}_opts -T $cmd_opts $file |
       sed 's/^const struct asm_id .$/static const struct asm_id */'
  else
    cat << EOF

static const struct asm_id *
${let}_lookup (register const char *str, register unsigned int len)
{
  return 0;
}
EOF
  fi
done >> $c_file 2> $err_file
cat << EOF >> $c_file

const struct asm_id * inst_lookup(const char *str)
{
    size_t len = strlen(str);

    switch (str[0]) {
    case 'a':	return a_lookup(str, len);
    case 'b':	return b_lookup(str, len);
    case 'c':	return c_lookup(str, len);
    case 'd':	return d_lookup(str, len);
    case 'e':	return e_lookup(str, len);
    case 'f':	return f_lookup(str, len);
    case 'g':	return g_lookup(str, len);
    case 'h':	return h_lookup(str, len);
    case 'i':	return i_lookup(str, len);
    case 'j':	return j_lookup(str, len);
    case 'k':	return k_lookup(str, len);
    case 'l':	return l_lookup(str, len);
    case 'm':	return m_lookup(str, len);
    case 'n':	return n_lookup(str, len);
    case 'o':	return o_lookup(str, len);
    case 'p':	return p_lookup(str, len);
    case 'q':	return q_lookup(str, len);
    case 'r':	return r_lookup(str, len);
    case 's':	return s_lookup(str, len);
    case 't':	return t_lookup(str, len);
    case 'u':	return u_lookup(str, len);
    case 'v':	return v_lookup(str, len);
    case 'w':	return w_lookup(str, len);
    case 'x':	return x_lookup(str, len);
    case 'y':	return y_lookup(str, len);
    case 'z':	return z_lookup(str, len);
    }
    return 0;
}
EOF
