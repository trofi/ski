/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* DO NOT EDIT - Automatically generated using:
     make_asm
 */
/*
 * Copyright (C) 1995-2007, Hewlett-Packard Development Company, L.P.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */


#include <asm.h>

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H a_hash -N a_lookup -j 1 -n -k 2,4,6,8 -T asm_a.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 11, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
a_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13,  0, 13, 13, 13, 13,
      13, 13, 13,  1, 13, 13, 13, 13, 13, 13,
      13, 13,  0, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13,  3, 13, 13, 13,  6,
       2, 13, 13, 13, 13, 13, 13, 13,  9, 13,
       0,  0,  2, 13, 13,  0, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13
    };
  register int hval = 0;

  switch (len)
    {
      default:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
      case 2:
        hval += asso_values[(unsigned char)str[1]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
a_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 11,
      MIN_WORD_LENGTH = 7,
      MAX_WORD_LENGTH = 12,
      MIN_HASH_VALUE = 2,
      MAX_HASH_VALUE = 12
    };

  static const struct asm_id wordlist[] =
    {
      {""}, {""},
#line 8 "asm_a.gperf"
      {"adds_RIR", EM_ADDS_R1_IMM14_R3},
#line 13 "asm_a.gperf"
      {"and_RRR", EM_AND_R1_R2_R3},
#line 12 "asm_a.gperf"
      {"and_RIR", EM_AND_R1_IMM8_R3},
#line 7 "asm_a.gperf"
      {"add_RRR", EM_ADD_R1_R2_R3},
#line 6 "asm_a.gperf"
      {"add_RRRI", EM_ADD_R1_R2_R3_1},
#line 5 "asm_a.gperf"
      {"addp4_RRR", EM_ADDP4_R1_R2_R3},
#line 4 "asm_a.gperf"
      {"addp4_RIR", EM_ADDP4_R1_IMM14_R3},
#line 11 "asm_a.gperf"
      {"andcm_RRR", EM_ANDCM_R1_R2_R3},
#line 10 "asm_a.gperf"
      {"andcm_RIR", EM_ANDCM_R1_IMM8_R3},
#line 3 "asm_a.gperf"
      {"addl_RIR", EM_ADDL_R1_IMM22_R3},
#line 9 "asm_a.gperf"
      {"alloc_RAIIII", EM_ALLOC_R1_AR_PFS_I_L_O_R}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = a_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H b_hash -N b_lookup -D -j 3 -k '3-5,7-11,13-15,16,17,19-21,23-25,$' -T asm_b.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 1137, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
b_hash (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160,    0, 1160,    3,    0,
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160,    6, 1160, 1160, 1160,
      1160, 1160, 1160,   18, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160,  159, 1160,   93,    3,    0,
         3,   15,   81, 1160, 1160,   12, 1160,   12,    0,   24,
         6,  328,  237, 1160,    0,    0,    0, 1160, 1160,   18,
         0,  183, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160, 1160,
      1160, 1160, 1160, 1160, 1160, 1160
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[24]];
      /*FALLTHROUGH*/
      case 24:
        hval += asso_values[(unsigned char)str[23]];
      /*FALLTHROUGH*/
      case 23:
        hval += asso_values[(unsigned char)str[22]];
      /*FALLTHROUGH*/
      case 22:
      case 21:
        hval += asso_values[(unsigned char)str[20]];
      /*FALLTHROUGH*/
      case 20:
        hval += asso_values[(unsigned char)str[19]];
      /*FALLTHROUGH*/
      case 19:
        hval += asso_values[(unsigned char)str[18]];
      /*FALLTHROUGH*/
      case 18:
      case 17:
        hval += asso_values[(unsigned char)str[16]];
      /*FALLTHROUGH*/
      case 16:
        hval += asso_values[(unsigned char)str[15]];
      /*FALLTHROUGH*/
      case 15:
        hval += asso_values[(unsigned char)str[14]];
      /*FALLTHROUGH*/
      case 14:
        hval += asso_values[(unsigned char)str[13]];
      /*FALLTHROUGH*/
      case 13:
        hval += asso_values[(unsigned char)str[12]];
      /*FALLTHROUGH*/
      case 12:
      case 11:
        hval += asso_values[(unsigned char)str[10]];
      /*FALLTHROUGH*/
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      /*FALLTHROUGH*/
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      /*FALLTHROUGH*/
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
        hval += asso_values[(unsigned char)str[2]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
b_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 231,
      MIN_WORD_LENGTH = 5,
      MAX_WORD_LENGTH = 25,
      MIN_HASH_VALUE = 23,
      MAX_HASH_VALUE = 1159
    };

  static const struct asm_id wordlist[] =
    {
#line 233 "asm_b.gperf"
      {"bsw.1", EM_BSW_1},
#line 232 "asm_b.gperf"
      {"bsw.0", EM_BSW_0},
#line 119 "asm_b.gperf"
      {"break.x_I", EM_BREAK_X_IMM62},
#line 115 "asm_b.gperf"
      {"break.b_I", EM_BREAK_B_IMM21},
#line 117 "asm_b.gperf"
      {"break.i_I", EM_BREAK_I_IMM21},
#line 118 "asm_b.gperf"
      {"break.m_I", EM_BREAK_M_IMM21},
#line 116 "asm_b.gperf"
      {"break.f_I", EM_BREAK_F_IMM21},
#line 144 "asm_b.gperf"
      {"brl.call.spnt.few_BI", EM_BRL_CALL_SPNT_FEW_B1_TARGET64},
#line 161 "asm_b.gperf"
      {"brl.cond.spnt.few_I", EM_BRL_COND_SPNT_FEW_TARGET64},
#line 136 "asm_b.gperf"
      {"brl.call.dpnt.few_BI", EM_BRL_CALL_DPNT_FEW_B1_TARGET64},
#line 145 "asm_b.gperf"
      {"brl.call.spnt.few.clr_BI", EM_BRL_CALL_SPNT_FEW_CLR_B1_TARGET64},
#line 153 "asm_b.gperf"
      {"brl.cond.dpnt.few_I", EM_BRL_COND_DPNT_FEW_TARGET64},
#line 160 "asm_b.gperf"
      {"brl.cond.spnt.few.clr_I", EM_BRL_COND_SPNT_FEW_CLR_TARGET64},
#line 137 "asm_b.gperf"
      {"brl.call.dpnt.few.clr_BI", EM_BRL_CALL_DPNT_FEW_CLR_B1_TARGET64},
#line 152 "asm_b.gperf"
      {"brl.cond.dpnt.few.clr_I", EM_BRL_COND_DPNT_FEW_CLR_TARGET64},
#line 148 "asm_b.gperf"
      {"brl.call.sptk.few_BI", EM_BRL_CALL_SPTK_FEW_B1_TARGET64},
#line 165 "asm_b.gperf"
      {"brl.cond.sptk.few_I", EM_BRL_COND_SPTK_FEW_TARGET64},
#line 140 "asm_b.gperf"
      {"brl.call.dptk.few_BI", EM_BRL_CALL_DPTK_FEW_B1_TARGET64},
#line 149 "asm_b.gperf"
      {"brl.call.sptk.few.clr_BI", EM_BRL_CALL_SPTK_FEW_CLR_B1_TARGET64},
#line 157 "asm_b.gperf"
      {"brl.cond.dptk.few_I", EM_BRL_COND_DPTK_FEW_TARGET64},
#line 164 "asm_b.gperf"
      {"brl.cond.sptk.few.clr_I", EM_BRL_COND_SPTK_FEW_CLR_TARGET64},
#line 141 "asm_b.gperf"
      {"brl.call.dptk.few.clr_BI", EM_BRL_CALL_DPTK_FEW_CLR_B1_TARGET64},
#line 44 "asm_b.gperf"
      {"br.cexit.spnt.few_I", EM_BR_CEXIT_SPNT_FEW_TARGET25},
#line 156 "asm_b.gperf"
      {"brl.cond.dptk.few.clr_I", EM_BRL_COND_DPTK_FEW_CLR_TARGET64},
#line 36 "asm_b.gperf"
      {"br.cexit.dpnt.few_I", EM_BR_CEXIT_DPNT_FEW_TARGET25},
#line 43 "asm_b.gperf"
      {"br.cexit.spnt.few.clr_I", EM_BR_CEXIT_SPNT_FEW_CLR_TARGET25},
#line 35 "asm_b.gperf"
      {"br.cexit.dpnt.few.clr_I", EM_BR_CEXIT_DPNT_FEW_CLR_TARGET25},
#line 48 "asm_b.gperf"
      {"br.cexit.sptk.few_I", EM_BR_CEXIT_SPTK_FEW_TARGET25},
#line 40 "asm_b.gperf"
      {"br.cexit.dptk.few_I", EM_BR_CEXIT_DPTK_FEW_TARGET25},
#line 47 "asm_b.gperf"
      {"br.cexit.sptk.few.clr_I", EM_BR_CEXIT_SPTK_FEW_CLR_TARGET25},
#line 209 "asm_b.gperf"
      {"br.wexit.spnt.few_I", EM_BR_WEXIT_SPNT_FEW_TARGET25},
#line 39 "asm_b.gperf"
      {"br.cexit.dptk.few.clr_I", EM_BR_CEXIT_DPTK_FEW_CLR_TARGET25},
#line 201 "asm_b.gperf"
      {"br.wexit.dpnt.few_I", EM_BR_WEXIT_DPNT_FEW_TARGET25},
#line 208 "asm_b.gperf"
      {"br.wexit.spnt.few.clr_I", EM_BR_WEXIT_SPNT_FEW_CLR_TARGET25},
#line 200 "asm_b.gperf"
      {"br.wexit.dpnt.few.clr_I", EM_BR_WEXIT_DPNT_FEW_CLR_TARGET25},
#line 180 "asm_b.gperf"
      {"brp.sptk_BI", EM_BRP_SPTK_B2_TAG13},
#line 213 "asm_b.gperf"
      {"br.wexit.sptk.few_I", EM_BR_WEXIT_SPTK_FEW_TARGET25},
#line 168 "asm_b.gperf"
      {"brp.dptk_BI", EM_BRP_DPTK_B2_TAG13},
#line 205 "asm_b.gperf"
      {"br.wexit.dptk.few_I", EM_BR_WEXIT_DPTK_FEW_TARGET25},
#line 212 "asm_b.gperf"
      {"br.wexit.sptk.few.clr_I", EM_BR_WEXIT_SPTK_FEW_CLR_TARGET25},
#line 204 "asm_b.gperf"
      {"br.wexit.dptk.few.clr_I", EM_BR_WEXIT_DPTK_FEW_CLR_TARGET25},
#line 183 "asm_b.gperf"
      {"brp.sptk_II", EM_BRP_SPTK_TARGET25_TAG13},
#line 171 "asm_b.gperf"
      {"brp.dptk_II", EM_BRP_DPTK_TARGET25_TAG13},
#line 173 "asm_b.gperf"
      {"brp.exit_II", EM_BRP_EXIT_TARGET25_TAG13},
#line 181 "asm_b.gperf"
      {"brp.sptk.imp_BI", EM_BRP_SPTK_IMP_B2_TAG13},
#line 169 "asm_b.gperf"
      {"brp.dptk.imp_BI", EM_BRP_DPTK_IMP_B2_TAG13},
#line 182 "asm_b.gperf"
      {"brp.sptk.imp_II", EM_BRP_SPTK_IMP_TARGET25_TAG13},
#line 170 "asm_b.gperf"
      {"brp.dptk.imp_II", EM_BRP_DPTK_IMP_TARGET25_TAG13},
#line 172 "asm_b.gperf"
      {"brp.exit.imp_II", EM_BRP_EXIT_IMP_TARGET25_TAG13},
#line 192 "asm_b.gperf"
      {"br.ret.spnt.few_B", EM_BR_RET_SPNT_FEW_B2},
#line 184 "asm_b.gperf"
      {"br.ret.dpnt.few_B", EM_BR_RET_DPNT_FEW_B2},
#line 193 "asm_b.gperf"
      {"br.ret.spnt.few.clr_B", EM_BR_RET_SPNT_FEW_CLR_B2},
#line 196 "asm_b.gperf"
      {"br.ret.sptk.few_B", EM_BR_RET_SPTK_FEW_B2},
#line 185 "asm_b.gperf"
      {"br.ret.dpnt.few.clr_B", EM_BR_RET_DPNT_FEW_CLR_B2},
#line 128 "asm_b.gperf"
      {"br.ia.spnt.few_B", EM_BR_IA_SPNT_FEW_B2},
#line 188 "asm_b.gperf"
      {"br.ret.dptk.few_B", EM_BR_RET_DPTK_FEW_B2},
#line 197 "asm_b.gperf"
      {"br.ret.sptk.few.clr_B", EM_BR_RET_SPTK_FEW_CLR_B2},
#line 120 "asm_b.gperf"
      {"br.ia.dpnt.few_B", EM_BR_IA_DPNT_FEW_B2},
#line 129 "asm_b.gperf"
      {"br.ia.spnt.few.clr_B", EM_BR_IA_SPNT_FEW_CLR_B2},
#line 189 "asm_b.gperf"
      {"br.ret.dptk.few.clr_B", EM_BR_RET_DPTK_FEW_CLR_B2},
#line 132 "asm_b.gperf"
      {"br.ia.sptk.few_B", EM_BR_IA_SPTK_FEW_B2},
#line 121 "asm_b.gperf"
      {"br.ia.dpnt.few.clr_B", EM_BR_IA_DPNT_FEW_CLR_B2},
#line 124 "asm_b.gperf"
      {"br.ia.dptk.few_B", EM_BR_IA_DPTK_FEW_B2},
#line 133 "asm_b.gperf"
      {"br.ia.sptk.few.clr_B", EM_BR_IA_SPTK_FEW_CLR_B2},
#line 125 "asm_b.gperf"
      {"br.ia.dptk.few.clr_B", EM_BR_IA_DPTK_FEW_CLR_B2},
#line 146 "asm_b.gperf"
      {"brl.call.spnt.many_BI", EM_BRL_CALL_SPNT_MANY_B1_TARGET64},
#line 163 "asm_b.gperf"
      {"brl.cond.spnt.many_I", EM_BRL_COND_SPNT_MANY_TARGET64},
#line 138 "asm_b.gperf"
      {"brl.call.dpnt.many_BI", EM_BRL_CALL_DPNT_MANY_B1_TARGET64},
#line 147 "asm_b.gperf"
      {"brl.call.spnt.many.clr_BI", EM_BRL_CALL_SPNT_MANY_CLR_B1_TARGET64},
#line 155 "asm_b.gperf"
      {"brl.cond.dpnt.many_I", EM_BRL_COND_DPNT_MANY_TARGET64},
#line 162 "asm_b.gperf"
      {"brl.cond.spnt.many.clr_I", EM_BRL_COND_SPNT_MANY_CLR_TARGET64},
#line 139 "asm_b.gperf"
      {"brl.call.dpnt.many.clr_BI", EM_BRL_CALL_DPNT_MANY_CLR_B1_TARGET64},
#line 154 "asm_b.gperf"
      {"brl.cond.dpnt.many.clr_I", EM_BRL_COND_DPNT_MANY_CLR_TARGET64},
#line 150 "asm_b.gperf"
      {"brl.call.sptk.many_BI", EM_BRL_CALL_SPTK_MANY_B1_TARGET64},
#line 167 "asm_b.gperf"
      {"brl.cond.sptk.many_I", EM_BRL_COND_SPTK_MANY_TARGET64},
#line 142 "asm_b.gperf"
      {"brl.call.dptk.many_BI", EM_BRL_CALL_DPTK_MANY_B1_TARGET64},
#line 151 "asm_b.gperf"
      {"brl.call.sptk.many.clr_BI", EM_BRL_CALL_SPTK_MANY_CLR_B1_TARGET64},
#line 159 "asm_b.gperf"
      {"brl.cond.dptk.many_I", EM_BRL_COND_DPTK_MANY_TARGET64},
#line 166 "asm_b.gperf"
      {"brl.cond.sptk.many.clr_I", EM_BRL_COND_SPTK_MANY_CLR_TARGET64},
#line 143 "asm_b.gperf"
      {"brl.call.dptk.many.clr_BI", EM_BRL_CALL_DPTK_MANY_CLR_B1_TARGET64},
#line 46 "asm_b.gperf"
      {"br.cexit.spnt.many_I", EM_BR_CEXIT_SPNT_MANY_TARGET25},
#line 158 "asm_b.gperf"
      {"brl.cond.dptk.many.clr_I", EM_BRL_COND_DPTK_MANY_CLR_TARGET64},
#line 38 "asm_b.gperf"
      {"br.cexit.dpnt.many_I", EM_BR_CEXIT_DPNT_MANY_TARGET25},
#line 45 "asm_b.gperf"
      {"br.cexit.spnt.many.clr_I", EM_BR_CEXIT_SPNT_MANY_CLR_TARGET25},
#line 37 "asm_b.gperf"
      {"br.cexit.dpnt.many.clr_I", EM_BR_CEXIT_DPNT_MANY_CLR_TARGET25},
#line 50 "asm_b.gperf"
      {"br.cexit.sptk.many_I", EM_BR_CEXIT_SPTK_MANY_TARGET25},
#line 42 "asm_b.gperf"
      {"br.cexit.dptk.many_I", EM_BR_CEXIT_DPTK_MANY_TARGET25},
#line 49 "asm_b.gperf"
      {"br.cexit.sptk.many.clr_I", EM_BR_CEXIT_SPTK_MANY_CLR_TARGET25},
#line 211 "asm_b.gperf"
      {"br.wexit.spnt.many_I", EM_BR_WEXIT_SPNT_MANY_TARGET25},
#line 41 "asm_b.gperf"
      {"br.cexit.dptk.many.clr_I", EM_BR_CEXIT_DPTK_MANY_CLR_TARGET25},
#line 203 "asm_b.gperf"
      {"br.wexit.dpnt.many_I", EM_BR_WEXIT_DPNT_MANY_TARGET25},
#line 210 "asm_b.gperf"
      {"br.wexit.spnt.many.clr_I", EM_BR_WEXIT_SPNT_MANY_CLR_TARGET25},
#line 202 "asm_b.gperf"
      {"br.wexit.dpnt.many.clr_I", EM_BR_WEXIT_DPNT_MANY_CLR_TARGET25},
#line 215 "asm_b.gperf"
      {"br.wexit.sptk.many_I", EM_BR_WEXIT_SPTK_MANY_TARGET25},
#line 27 "asm_b.gperf"
      {"br.call.sptk.few_BB", EM_BR_CALL_SPTK_FEW_B1_B2},
#line 207 "asm_b.gperf"
      {"br.wexit.dptk.many_I", EM_BR_WEXIT_DPTK_MANY_TARGET25},
#line 214 "asm_b.gperf"
      {"br.wexit.sptk.many.clr_I", EM_BR_WEXIT_SPTK_MANY_CLR_TARGET25},
#line 11 "asm_b.gperf"
      {"br.call.dptk.few_BB", EM_BR_CALL_DPTK_FEW_B1_B2},
#line 29 "asm_b.gperf"
      {"br.call.sptk.few.clr_BB", EM_BR_CALL_SPTK_FEW_CLR_B1_B2},
#line 206 "asm_b.gperf"
      {"br.wexit.dptk.many.clr_I", EM_BR_WEXIT_DPTK_MANY_CLR_TARGET25},
#line 19 "asm_b.gperf"
      {"br.call.spnt.few_BB", EM_BR_CALL_SPNT_FEW_B1_B2},
#line 13 "asm_b.gperf"
      {"br.call.dptk.few.clr_BB", EM_BR_CALL_DPTK_FEW_CLR_B1_B2},
#line 3 "asm_b.gperf"
      {"br.call.dpnt.few_BB", EM_BR_CALL_DPNT_FEW_B1_B2},
#line 21 "asm_b.gperf"
      {"br.call.spnt.few.clr_BB", EM_BR_CALL_SPNT_FEW_CLR_B1_B2},
#line 5 "asm_b.gperf"
      {"br.call.dpnt.few.clr_BB", EM_BR_CALL_DPNT_FEW_CLR_B1_B2},
#line 28 "asm_b.gperf"
      {"br.call.sptk.few_BI", EM_BR_CALL_SPTK_FEW_B1_TARGET25},
#line 12 "asm_b.gperf"
      {"br.call.dptk.few_BI", EM_BR_CALL_DPTK_FEW_B1_TARGET25},
#line 30 "asm_b.gperf"
      {"br.call.sptk.few.clr_BI", EM_BR_CALL_SPTK_FEW_CLR_B1_TARGET25},
#line 20 "asm_b.gperf"
      {"br.call.spnt.few_BI", EM_BR_CALL_SPNT_FEW_B1_TARGET25},
#line 14 "asm_b.gperf"
      {"br.call.dptk.few.clr_BI", EM_BR_CALL_DPTK_FEW_CLR_B1_TARGET25},
#line 4 "asm_b.gperf"
      {"br.call.dpnt.few_BI", EM_BR_CALL_DPNT_FEW_B1_TARGET25},
#line 22 "asm_b.gperf"
      {"br.call.spnt.few.clr_BI", EM_BR_CALL_SPNT_FEW_CLR_B1_TARGET25},
#line 6 "asm_b.gperf"
      {"br.call.dpnt.few.clr_BI", EM_BR_CALL_DPNT_FEW_CLR_B1_TARGET25},
#line 31 "asm_b.gperf"
      {"br.call.sptk.many_BB", EM_BR_CALL_SPTK_MANY_B1_B2},
#line 15 "asm_b.gperf"
      {"br.call.dptk.many_BB", EM_BR_CALL_DPTK_MANY_B1_B2},
#line 33 "asm_b.gperf"
      {"br.call.sptk.many.clr_BB", EM_BR_CALL_SPTK_MANY_CLR_B1_B2},
#line 23 "asm_b.gperf"
      {"br.call.spnt.many_BB", EM_BR_CALL_SPNT_MANY_B1_B2},
#line 17 "asm_b.gperf"
      {"br.call.dptk.many.clr_BB", EM_BR_CALL_DPTK_MANY_CLR_B1_B2},
#line 7 "asm_b.gperf"
      {"br.call.dpnt.many_BB", EM_BR_CALL_DPNT_MANY_B1_B2},
#line 25 "asm_b.gperf"
      {"br.call.spnt.many.clr_BB", EM_BR_CALL_SPNT_MANY_CLR_B1_B2},
#line 9 "asm_b.gperf"
      {"br.call.dpnt.many.clr_BB", EM_BR_CALL_DPNT_MANY_CLR_B1_B2},
#line 178 "asm_b.gperf"
      {"brp.ret.sptk_BI", EM_BRP_RET_SPTK_B2_TAG13},
#line 176 "asm_b.gperf"
      {"brp.ret.dptk_BI", EM_BRP_RET_DPTK_B2_TAG13},
#line 32 "asm_b.gperf"
      {"br.call.sptk.many_BI", EM_BR_CALL_SPTK_MANY_B1_TARGET25},
#line 16 "asm_b.gperf"
      {"br.call.dptk.many_BI", EM_BR_CALL_DPTK_MANY_B1_TARGET25},
#line 34 "asm_b.gperf"
      {"br.call.sptk.many.clr_BI", EM_BR_CALL_SPTK_MANY_CLR_B1_TARGET25},
#line 24 "asm_b.gperf"
      {"br.call.spnt.many_BI", EM_BR_CALL_SPNT_MANY_B1_TARGET25},
#line 18 "asm_b.gperf"
      {"br.call.dptk.many.clr_BI", EM_BR_CALL_DPTK_MANY_CLR_B1_TARGET25},
#line 8 "asm_b.gperf"
      {"br.call.dpnt.many_BI", EM_BR_CALL_DPNT_MANY_B1_TARGET25},
#line 26 "asm_b.gperf"
      {"br.call.spnt.many.clr_BI", EM_BR_CALL_SPNT_MANY_CLR_B1_TARGET25},
#line 10 "asm_b.gperf"
      {"br.call.dpnt.many.clr_BI", EM_BR_CALL_DPNT_MANY_CLR_B1_TARGET25},
#line 194 "asm_b.gperf"
      {"br.ret.spnt.many_B", EM_BR_RET_SPNT_MANY_B2},
#line 186 "asm_b.gperf"
      {"br.ret.dpnt.many_B", EM_BR_RET_DPNT_MANY_B2},
#line 195 "asm_b.gperf"
      {"br.ret.spnt.many.clr_B", EM_BR_RET_SPNT_MANY_CLR_B2},
#line 198 "asm_b.gperf"
      {"br.ret.sptk.many_B", EM_BR_RET_SPTK_MANY_B2},
#line 187 "asm_b.gperf"
      {"br.ret.dpnt.many.clr_B", EM_BR_RET_DPNT_MANY_CLR_B2},
#line 190 "asm_b.gperf"
      {"br.ret.dptk.many_B", EM_BR_RET_DPTK_MANY_B2},
#line 199 "asm_b.gperf"
      {"br.ret.sptk.many.clr_B", EM_BR_RET_SPTK_MANY_CLR_B2},
#line 191 "asm_b.gperf"
      {"br.ret.dptk.many.clr_B", EM_BR_RET_DPTK_MANY_CLR_B2},
#line 112 "asm_b.gperf"
      {"br.ctop.sptk.few_I", EM_BR_CTOP_SPTK_FEW_TARGET25},
#line 104 "asm_b.gperf"
      {"br.ctop.dptk.few_I", EM_BR_CTOP_DPTK_FEW_TARGET25},
#line 111 "asm_b.gperf"
      {"br.ctop.sptk.few.clr_I", EM_BR_CTOP_SPTK_FEW_CLR_TARGET25},
#line 108 "asm_b.gperf"
      {"br.ctop.spnt.few_I", EM_BR_CTOP_SPNT_FEW_TARGET25},
#line 103 "asm_b.gperf"
      {"br.ctop.dptk.few.clr_I", EM_BR_CTOP_DPTK_FEW_CLR_TARGET25},
#line 100 "asm_b.gperf"
      {"br.ctop.dpnt.few_I", EM_BR_CTOP_DPNT_FEW_TARGET25},
#line 107 "asm_b.gperf"
      {"br.ctop.spnt.few.clr_I", EM_BR_CTOP_SPNT_FEW_CLR_TARGET25},
#line 99 "asm_b.gperf"
      {"br.ctop.dpnt.few.clr_I", EM_BR_CTOP_DPNT_FEW_CLR_TARGET25},
#line 229 "asm_b.gperf"
      {"br.wtop.sptk.few_I", EM_BR_WTOP_SPTK_FEW_TARGET25},
#line 221 "asm_b.gperf"
      {"br.wtop.dptk.few_I", EM_BR_WTOP_DPTK_FEW_TARGET25},
#line 228 "asm_b.gperf"
      {"br.wtop.sptk.few.clr_I", EM_BR_WTOP_SPTK_FEW_CLR_TARGET25},
#line 225 "asm_b.gperf"
      {"br.wtop.spnt.few_I", EM_BR_WTOP_SPNT_FEW_TARGET25},
#line 220 "asm_b.gperf"
      {"br.wtop.dptk.few.clr_I", EM_BR_WTOP_DPTK_FEW_CLR_TARGET25},
#line 217 "asm_b.gperf"
      {"br.wtop.dpnt.few_I", EM_BR_WTOP_DPNT_FEW_TARGET25},
#line 224 "asm_b.gperf"
      {"br.wtop.spnt.few.clr_I", EM_BR_WTOP_SPNT_FEW_CLR_TARGET25},
#line 216 "asm_b.gperf"
      {"br.wtop.dpnt.few.clr_I", EM_BR_WTOP_DPNT_FEW_CLR_TARGET25},
#line 130 "asm_b.gperf"
      {"br.ia.spnt.many_B", EM_BR_IA_SPNT_MANY_B2},
#line 122 "asm_b.gperf"
      {"br.ia.dpnt.many_B", EM_BR_IA_DPNT_MANY_B2},
#line 131 "asm_b.gperf"
      {"br.ia.spnt.many.clr_B", EM_BR_IA_SPNT_MANY_CLR_B2},
#line 134 "asm_b.gperf"
      {"br.ia.sptk.many_B", EM_BR_IA_SPTK_MANY_B2},
#line 123 "asm_b.gperf"
      {"br.ia.dpnt.many.clr_B", EM_BR_IA_DPNT_MANY_CLR_B2},
#line 126 "asm_b.gperf"
      {"br.ia.dptk.many_B", EM_BR_IA_DPTK_MANY_B2},
#line 135 "asm_b.gperf"
      {"br.ia.sptk.many.clr_B", EM_BR_IA_SPTK_MANY_CLR_B2},
#line 127 "asm_b.gperf"
      {"br.ia.dptk.many.clr_B", EM_BR_IA_DPTK_MANY_CLR_B2},
#line 114 "asm_b.gperf"
      {"br.ctop.sptk.many_I", EM_BR_CTOP_SPTK_MANY_TARGET25},
#line 106 "asm_b.gperf"
      {"br.ctop.dptk.many_I", EM_BR_CTOP_DPTK_MANY_TARGET25},
#line 113 "asm_b.gperf"
      {"br.ctop.sptk.many.clr_I", EM_BR_CTOP_SPTK_MANY_CLR_TARGET25},
#line 110 "asm_b.gperf"
      {"br.ctop.spnt.many_I", EM_BR_CTOP_SPNT_MANY_TARGET25},
#line 105 "asm_b.gperf"
      {"br.ctop.dptk.many.clr_I", EM_BR_CTOP_DPTK_MANY_CLR_TARGET25},
#line 102 "asm_b.gperf"
      {"br.ctop.dpnt.many_I", EM_BR_CTOP_DPNT_MANY_TARGET25},
#line 109 "asm_b.gperf"
      {"br.ctop.spnt.many.clr_I", EM_BR_CTOP_SPNT_MANY_CLR_TARGET25},
#line 101 "asm_b.gperf"
      {"br.ctop.dpnt.many.clr_I", EM_BR_CTOP_DPNT_MANY_CLR_TARGET25},
#line 231 "asm_b.gperf"
      {"br.wtop.sptk.many_I", EM_BR_WTOP_SPTK_MANY_TARGET25},
#line 223 "asm_b.gperf"
      {"br.wtop.dptk.many_I", EM_BR_WTOP_DPTK_MANY_TARGET25},
#line 230 "asm_b.gperf"
      {"br.wtop.sptk.many.clr_I", EM_BR_WTOP_SPTK_MANY_CLR_TARGET25},
#line 227 "asm_b.gperf"
      {"br.wtop.spnt.many_I", EM_BR_WTOP_SPNT_MANY_TARGET25},
#line 222 "asm_b.gperf"
      {"br.wtop.dptk.many.clr_I", EM_BR_WTOP_DPTK_MANY_CLR_TARGET25},
#line 219 "asm_b.gperf"
      {"br.wtop.dpnt.many_I", EM_BR_WTOP_DPNT_MANY_TARGET25},
#line 226 "asm_b.gperf"
      {"br.wtop.spnt.many.clr_I", EM_BR_WTOP_SPNT_MANY_CLR_TARGET25},
#line 91 "asm_b.gperf"
      {"br.cond.sptk.few_B", EM_BR_COND_SPTK_FEW_B2},
#line 218 "asm_b.gperf"
      {"br.wtop.dpnt.many.clr_I", EM_BR_WTOP_DPNT_MANY_CLR_TARGET25},
#line 75 "asm_b.gperf"
      {"br.cond.dptk.few_B", EM_BR_COND_DPTK_FEW_B2},
#line 92 "asm_b.gperf"
      {"br.cond.sptk.few.clr_B", EM_BR_COND_SPTK_FEW_CLR_B2},
#line 83 "asm_b.gperf"
      {"br.cond.spnt.few_B", EM_BR_COND_SPNT_FEW_B2},
#line 76 "asm_b.gperf"
      {"br.cond.dptk.few.clr_B", EM_BR_COND_DPTK_FEW_CLR_B2},
#line 67 "asm_b.gperf"
      {"br.cond.dpnt.few_B", EM_BR_COND_DPNT_FEW_B2},
#line 84 "asm_b.gperf"
      {"br.cond.spnt.few.clr_B", EM_BR_COND_SPNT_FEW_CLR_B2},
#line 94 "asm_b.gperf"
      {"br.cond.sptk.few_I", EM_BR_COND_SPTK_FEW_TARGET25},
#line 68 "asm_b.gperf"
      {"br.cond.dpnt.few.clr_B", EM_BR_COND_DPNT_FEW_CLR_B2},
#line 78 "asm_b.gperf"
      {"br.cond.dptk.few_I", EM_BR_COND_DPTK_FEW_TARGET25},
#line 93 "asm_b.gperf"
      {"br.cond.sptk.few.clr_I", EM_BR_COND_SPTK_FEW_CLR_TARGET25},
#line 86 "asm_b.gperf"
      {"br.cond.spnt.few_I", EM_BR_COND_SPNT_FEW_TARGET25},
#line 77 "asm_b.gperf"
      {"br.cond.dptk.few.clr_I", EM_BR_COND_DPTK_FEW_CLR_TARGET25},
#line 70 "asm_b.gperf"
      {"br.cond.dpnt.few_I", EM_BR_COND_DPNT_FEW_TARGET25},
#line 85 "asm_b.gperf"
      {"br.cond.spnt.few.clr_I", EM_BR_COND_SPNT_FEW_CLR_TARGET25},
#line 69 "asm_b.gperf"
      {"br.cond.dpnt.few.clr_I", EM_BR_COND_DPNT_FEW_CLR_TARGET25},
#line 95 "asm_b.gperf"
      {"br.cond.sptk.many_B", EM_BR_COND_SPTK_MANY_B2},
#line 79 "asm_b.gperf"
      {"br.cond.dptk.many_B", EM_BR_COND_DPTK_MANY_B2},
#line 96 "asm_b.gperf"
      {"br.cond.sptk.many.clr_B", EM_BR_COND_SPTK_MANY_CLR_B2},
#line 87 "asm_b.gperf"
      {"br.cond.spnt.many_B", EM_BR_COND_SPNT_MANY_B2},
#line 80 "asm_b.gperf"
      {"br.cond.dptk.many.clr_B", EM_BR_COND_DPTK_MANY_CLR_B2},
#line 71 "asm_b.gperf"
      {"br.cond.dpnt.many_B", EM_BR_COND_DPNT_MANY_B2},
#line 88 "asm_b.gperf"
      {"br.cond.spnt.many.clr_B", EM_BR_COND_SPNT_MANY_CLR_B2},
#line 72 "asm_b.gperf"
      {"br.cond.dpnt.many.clr_B", EM_BR_COND_DPNT_MANY_CLR_B2},
#line 98 "asm_b.gperf"
      {"br.cond.sptk.many_I", EM_BR_COND_SPTK_MANY_TARGET25},
#line 82 "asm_b.gperf"
      {"br.cond.dptk.many_I", EM_BR_COND_DPTK_MANY_TARGET25},
#line 97 "asm_b.gperf"
      {"br.cond.sptk.many.clr_I", EM_BR_COND_SPTK_MANY_CLR_TARGET25},
#line 90 "asm_b.gperf"
      {"br.cond.spnt.many_I", EM_BR_COND_SPNT_MANY_TARGET25},
#line 81 "asm_b.gperf"
      {"br.cond.dptk.many.clr_I", EM_BR_COND_DPTK_MANY_CLR_TARGET25},
#line 74 "asm_b.gperf"
      {"br.cond.dpnt.many_I", EM_BR_COND_DPNT_MANY_TARGET25},
#line 89 "asm_b.gperf"
      {"br.cond.spnt.many.clr_I", EM_BR_COND_SPNT_MANY_CLR_TARGET25},
#line 73 "asm_b.gperf"
      {"br.cond.dpnt.many.clr_I", EM_BR_COND_DPNT_MANY_CLR_TARGET25},
#line 179 "asm_b.gperf"
      {"brp.ret.sptk.imp_BI", EM_BRP_RET_SPTK_IMP_B2_TAG13},
#line 177 "asm_b.gperf"
      {"brp.ret.dptk.imp_BI", EM_BRP_RET_DPTK_IMP_B2_TAG13},
#line 60 "asm_b.gperf"
      {"br.cloop.spnt.few_I", EM_BR_CLOOP_SPNT_FEW_TARGET25},
#line 52 "asm_b.gperf"
      {"br.cloop.dpnt.few_I", EM_BR_CLOOP_DPNT_FEW_TARGET25},
#line 59 "asm_b.gperf"
      {"br.cloop.spnt.few.clr_I", EM_BR_CLOOP_SPNT_FEW_CLR_TARGET25},
#line 51 "asm_b.gperf"
      {"br.cloop.dpnt.few.clr_I", EM_BR_CLOOP_DPNT_FEW_CLR_TARGET25},
#line 64 "asm_b.gperf"
      {"br.cloop.sptk.few_I", EM_BR_CLOOP_SPTK_FEW_TARGET25},
#line 56 "asm_b.gperf"
      {"br.cloop.dptk.few_I", EM_BR_CLOOP_DPTK_FEW_TARGET25},
#line 63 "asm_b.gperf"
      {"br.cloop.sptk.few.clr_I", EM_BR_CLOOP_SPTK_FEW_CLR_TARGET25},
#line 55 "asm_b.gperf"
      {"br.cloop.dptk.few.clr_I", EM_BR_CLOOP_DPTK_FEW_CLR_TARGET25},
#line 175 "asm_b.gperf"
      {"brp.loop_II", EM_BRP_LOOP_TARGET25_TAG13},
#line 174 "asm_b.gperf"
      {"brp.loop.imp_II", EM_BRP_LOOP_IMP_TARGET25_TAG13},
#line 62 "asm_b.gperf"
      {"br.cloop.spnt.many_I", EM_BR_CLOOP_SPNT_MANY_TARGET25},
#line 54 "asm_b.gperf"
      {"br.cloop.dpnt.many_I", EM_BR_CLOOP_DPNT_MANY_TARGET25},
#line 61 "asm_b.gperf"
      {"br.cloop.spnt.many.clr_I", EM_BR_CLOOP_SPNT_MANY_CLR_TARGET25},
#line 53 "asm_b.gperf"
      {"br.cloop.dpnt.many.clr_I", EM_BR_CLOOP_DPNT_MANY_CLR_TARGET25},
#line 66 "asm_b.gperf"
      {"br.cloop.sptk.many_I", EM_BR_CLOOP_SPTK_MANY_TARGET25},
#line 58 "asm_b.gperf"
      {"br.cloop.dptk.many_I", EM_BR_CLOOP_DPTK_MANY_TARGET25},
#line 65 "asm_b.gperf"
      {"br.cloop.sptk.many.clr_I", EM_BR_CLOOP_SPTK_MANY_CLR_TARGET25},
#line 57 "asm_b.gperf"
      {"br.cloop.dptk.many.clr_I", EM_BR_CLOOP_DPTK_MANY_CLR_TARGET25}
    };

  static const short lookup[] =
    {
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,   0,  -1,  -1,  -1,  -1,  -1,   1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,   2,  -1,  -1,   3,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,   4,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   5,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,   6,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,   7,  -1,   8,   9,  10,  11,  12,
       13,  -1,  14,  -1,  -1,  15,  -1,  16,  17,  18,
       19,  20,  21,  22,  23,  -1,  24,  25,  -1,  -1,
       26,  -1,  -1,  -1,  -1,  27,  -1,  -1,  28,  29,
       -1,  30,  31,  -1,  32,  33,  -1,  -1,  34,  -1,
       -1,  35,  -1,  36,  37,  -1,  38,  39,  -1,  -1,
       40,  -1,  -1,  41,  -1,  -1,  42,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  43,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  44,  -1,  -1,  45,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  46,  -1,  -1,  47,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  48,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       49,  -1,  -1,  50,  51,  -1,  52,  53,  54,  55,
       56,  57,  58,  59,  60,  61,  -1,  62,  63,  -1,
       -1,  64,  65,  -1,  66,  67,  68,  69,  70,  71,
       -1,  72,  -1,  -1,  73,  -1,  74,  75,  76,  77,
       78,  79,  80,  81,  -1,  82,  83,  -1,  -1,  84,
       -1,  -1,  -1,  -1,  85,  -1,  -1,  86,  87,  -1,
       88,  89,  -1,  90,  91,  -1,  -1,  92,  -1,  -1,
       -1,  -1,  93,  -1,  94,  95,  96,  97,  98,  99,
      100, 101,  -1, 102, 103,  -1,  -1, 104,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 105,  -1,
       -1, 106, 107,  -1, 108, 109,  -1, 110, 111,  -1,
       -1, 112,  -1,  -1, 113,  -1,  -1, 114, 115,  -1,
      116, 117,  -1, 118, 119,  -1,  -1, 120,  -1,  -1,
      121,  -1,  -1, 122,  -1,  -1,  -1,  -1, 123,  -1,
       -1, 124, 125,  -1, 126, 127,  -1, 128, 129,  -1,
       -1, 130,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 131,  -1,  -1,
      132, 133,  -1, 134, 135,  -1, 136, 137,  -1,  -1,
      138,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 139,  -1,  -1, 140, 141,  -1, 142,
      143,  -1, 144, 145,  -1,  -1, 146,  -1,  -1,  -1,
       -1, 147,  -1,  -1, 148, 149,  -1, 150, 151,  -1,
      152, 153,  -1,  -1, 154,  -1,  -1,  -1, 155,  -1,
       -1, 156, 157,  -1, 158, 159,  -1, 160, 161,  -1,
       -1, 162,  -1,  -1,  -1, 163,  -1,  -1, 164, 165,
       -1, 166, 167,  -1, 168, 169,  -1,  -1, 170,  -1,
       -1,  -1,  -1, 171,  -1,  -1, 172, 173,  -1, 174,
      175,  -1, 176, 177,  -1, 178, 179,  -1, 180, 181,
       -1, 182, 183,  -1, 184, 185,  -1, 186, 187,  -1,
      188, 189,  -1, 190, 191,  -1, 192, 193,  -1,  -1,
      194,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 195,  -1,  -1, 196, 197,
       -1, 198, 199,  -1, 200, 201,  -1,  -1, 202,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 203,
       -1,  -1, 204, 205,  -1, 206, 207,  -1, 208, 209,
       -1,  -1, 210,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 211,  -1,  -1, 212,  -1,  -1,  -1,  -1,  -1,
       -1, 213,  -1,  -1, 214, 215,  -1,  -1, 216,  -1,
       -1,  -1,  -1, 217,  -1,  -1, 218, 219,  -1,  -1,
      220,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 221,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 222,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      223,  -1,  -1, 224, 225,  -1,  -1, 226,  -1,  -1,
       -1,  -1, 227,  -1,  -1, 228, 229,  -1,  -1, 230
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = b_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = wordlist[index].name;

              if (*str == *s && !strcmp (str + 1, s + 1))
                return &wordlist[index];
            }
        }
    }
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H c_hash -N c_lookup -o -j 1 -n -k 4-8,10-16,18-20 -T asm_c.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 228, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
c_hash (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      259, 259, 259, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259, 259,  19, 259, 259,  60,
       67, 259,   2, 259,   1, 259,  25, 259, 259, 259,
      259, 259, 259, 259, 259,   4, 259, 259, 259, 259,
       25, 259, 259,   1, 259, 259, 259,  11, 259, 259,
        0, 259,   0, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259,  18, 259,  16,   2,   0,
       11,   0, 259,  42,  11,   2, 259, 259,   4,   0,
        2,  39,  21,   0,  31,  46,   1,   6, 259, 259,
       10, 259, 259, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259, 259, 259, 259, 259, 259,
      259, 259, 259, 259, 259, 259
    };
  register int hval = 0;

  switch (len)
    {
      default:
        hval += asso_values[(unsigned char)str[19]];
      /*FALLTHROUGH*/
      case 19:
        hval += asso_values[(unsigned char)str[18]];
      /*FALLTHROUGH*/
      case 18:
        hval += asso_values[(unsigned char)str[17]];
      /*FALLTHROUGH*/
      case 17:
      case 16:
        hval += asso_values[(unsigned char)str[15]];
      /*FALLTHROUGH*/
      case 15:
        hval += asso_values[(unsigned char)str[14]];
      /*FALLTHROUGH*/
      case 14:
        hval += asso_values[(unsigned char)str[13]];
      /*FALLTHROUGH*/
      case 13:
        hval += asso_values[(unsigned char)str[12]];
      /*FALLTHROUGH*/
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      /*FALLTHROUGH*/
      case 11:
        hval += asso_values[(unsigned char)str[10]];
      /*FALLTHROUGH*/
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      /*FALLTHROUGH*/
      case 9:
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
        hval += asso_values[(unsigned char)str[3]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
c_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 116,
      MIN_WORD_LENGTH = 5,
      MAX_WORD_LENGTH = 24,
      MIN_HASH_VALUE = 31,
      MAX_HASH_VALUE = 258
    };

  static const struct asm_id wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""},
#line 114 "asm_c.gperf"
      {"cover", EM_COVER},
#line 39 "asm_c.gperf"
      {"cmp4.ltu_PPRR", EM_CMP4_LTU_P1_P2_R2_R3},
#line 38 "asm_c.gperf"
      {"cmp4.ltu_PPIR", EM_CMP4_LTU_P1_P2_IMM8_R3},
      {""}, {""}, {""},
#line 61 "asm_c.gperf"
      {"cmp.eq_PPRR", EM_CMP_EQ_P1_P2_R2_R3},
#line 60 "asm_c.gperf"
      {"cmp.eq_PPIR", EM_CMP_EQ_P1_P2_IMM8_R3},
#line 19 "asm_c.gperf"
      {"cmp4.eq_PPRR", EM_CMP4_EQ_P1_P2_R2_R3},
#line 18 "asm_c.gperf"
      {"cmp4.eq_PPIR", EM_CMP4_EQ_P1_P2_IMM8_R3},
      {""},
#line 77 "asm_c.gperf"
      {"cmp.lt_PPRR", EM_CMP_LT_P1_P2_R2_R3},
#line 76 "asm_c.gperf"
      {"cmp.lt_PPIR", EM_CMP_LT_P1_P2_IMM8_R3},
#line 35 "asm_c.gperf"
      {"cmp4.lt_PPRR", EM_CMP4_LT_P1_P2_R2_R3},
#line 34 "asm_c.gperf"
      {"cmp4.lt_PPIR", EM_CMP4_LT_P1_P2_IMM8_R3},
      {""}, {""},
#line 81 "asm_c.gperf"
      {"cmp.ltu_PPRR", EM_CMP_LTU_P1_P2_R2_R3},
#line 80 "asm_c.gperf"
      {"cmp.ltu_PPIR", EM_CMP_LTU_P1_P2_IMM8_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 6 "asm_c.gperf"
      {"chk.a.nc_RI", EM_CHK_A_NC_R1_TARGET25},
#line 41 "asm_c.gperf"
      {"cmp4.ltu.unc_PPRR", EM_CMP4_LTU_UNC_P1_P2_R2_R3},
#line 40 "asm_c.gperf"
      {"cmp4.ltu.unc_PPIR", EM_CMP4_LTU_UNC_P1_P2_IMM8_R3},
#line 21 "asm_c.gperf"
      {"cmp4.eq.unc_PPRR", EM_CMP4_EQ_UNC_P1_P2_R2_R3},
#line 20 "asm_c.gperf"
      {"cmp4.eq.unc_PPIR", EM_CMP4_EQ_UNC_P1_P2_IMM8_R3},
#line 63 "asm_c.gperf"
      {"cmp.eq.unc_PPRR", EM_CMP_EQ_UNC_P1_P2_R2_R3},
#line 62 "asm_c.gperf"
      {"cmp.eq.unc_PPIR", EM_CMP_EQ_UNC_P1_P2_IMM8_R3},
#line 10 "asm_c.gperf"
      {"clrrrb", EM_CLRRRB},
#line 37 "asm_c.gperf"
      {"cmp4.lt.unc_PPRR", EM_CMP4_LT_UNC_P1_P2_R2_R3},
#line 36 "asm_c.gperf"
      {"cmp4.lt.unc_PPIR", EM_CMP4_LT_UNC_P1_P2_IMM8_R3},
#line 79 "asm_c.gperf"
      {"cmp.lt.unc_PPRR", EM_CMP_LT_UNC_P1_P2_R2_R3},
#line 78 "asm_c.gperf"
      {"cmp.lt.unc_PPIR", EM_CMP_LT_UNC_P1_P2_IMM8_R3},
#line 83 "asm_c.gperf"
      {"cmp.ltu.unc_PPRR", EM_CMP_LTU_UNC_P1_P2_R2_R3},
#line 82 "asm_c.gperf"
      {"cmp.ltu.unc_PPIR", EM_CMP_LTU_UNC_P1_P2_IMM8_R3},
#line 13 "asm_c.gperf"
      {"cmp4.eq.and_PPRR", EM_CMP4_EQ_AND_P1_P2_R2_R3},
#line 12 "asm_c.gperf"
      {"cmp4.eq.and_PPIR", EM_CMP4_EQ_AND_P1_P2_IMM8_R3},
#line 43 "asm_c.gperf"
      {"cmp4.ne.and_PPRR", EM_CMP4_NE_AND_P1_P2_R2_R3},
#line 42 "asm_c.gperf"
      {"cmp4.ne.and_PPIR", EM_CMP4_NE_AND_P1_P2_IMM8_R3},
#line 28 "asm_c.gperf"
      {"cmp4.le.and_PPRR", EM_CMP4_LE_AND_P1_P2_R0_R3},
#line 31 "asm_c.gperf"
      {"cmp4.lt.and_PPRR", EM_CMP4_LT_AND_P1_P2_R0_R3},
#line 4 "asm_c.gperf"
      {"chk.a.clr_RI", EM_CHK_A_CLR_R1_TARGET25},
      {""}, {""}, {""}, {""},
#line 5 "asm_c.gperf"
      {"chk.a.nc_FI", EM_CHK_A_NC_F1_TARGET25},
#line 55 "asm_c.gperf"
      {"cmp.eq.and_PPRR", EM_CMP_EQ_AND_P1_P2_R2_R3},
#line 54 "asm_c.gperf"
      {"cmp.eq.and_PPIR", EM_CMP_EQ_AND_P1_P2_IMM8_R3},
#line 85 "asm_c.gperf"
      {"cmp.ne.and_PPRR", EM_CMP_NE_AND_P1_P2_R2_R3},
#line 84 "asm_c.gperf"
      {"cmp.ne.and_PPIR", EM_CMP_NE_AND_P1_P2_IMM8_R3},
#line 70 "asm_c.gperf"
      {"cmp.le.and_PPRR", EM_CMP_LE_AND_P1_P2_R0_R3},
#line 73 "asm_c.gperf"
      {"cmp.lt.and_PPRR", EM_CMP_LT_AND_P1_P2_R0_R3},
#line 17 "asm_c.gperf"
      {"cmp4.eq.or_PPRR", EM_CMP4_EQ_OR_P1_P2_R2_R3},
#line 16 "asm_c.gperf"
      {"cmp4.eq.or_PPIR", EM_CMP4_EQ_OR_P1_P2_IMM8_R3},
#line 47 "asm_c.gperf"
      {"cmp4.ne.or_PPRR", EM_CMP4_NE_OR_P1_P2_R2_R3},
#line 46 "asm_c.gperf"
      {"cmp4.ne.or_PPIR", EM_CMP4_NE_OR_P1_P2_IMM8_R3},
#line 30 "asm_c.gperf"
      {"cmp4.le.or_PPRR", EM_CMP4_LE_OR_P1_P2_R0_R3},
#line 33 "asm_c.gperf"
      {"cmp4.lt.or_PPRR", EM_CMP4_LT_OR_P1_P2_R0_R3},
#line 59 "asm_c.gperf"
      {"cmp.eq.or_PPRR", EM_CMP_EQ_OR_P1_P2_R2_R3},
#line 58 "asm_c.gperf"
      {"cmp.eq.or_PPIR", EM_CMP_EQ_OR_P1_P2_IMM8_R3},
#line 89 "asm_c.gperf"
      {"cmp.ne.or_PPRR", EM_CMP_NE_OR_P1_P2_R2_R3},
#line 88 "asm_c.gperf"
      {"cmp.ne.or_PPIR", EM_CMP_NE_OR_P1_P2_IMM8_R3},
#line 72 "asm_c.gperf"
      {"cmp.le.or_PPRR", EM_CMP_LE_OR_P1_P2_R0_R3},
#line 75 "asm_c.gperf"
      {"cmp.lt.or_PPRR", EM_CMP_LT_OR_P1_P2_R0_R3},
#line 115 "asm_c.gperf"
      {"czx1.l_RR", EM_CZX1_L_R1_R3},
#line 3 "asm_c.gperf"
      {"chk.a.clr_FI", EM_CHK_A_CLR_F1_TARGET25},
#line 9 "asm_c.gperf"
      {"chk.s.m_RI", EM_CHK_S_M_R2_TARGET25},
#line 11 "asm_c.gperf"
      {"clrrrb.pr", EM_CLRRRB_PR},
#line 8 "asm_c.gperf"
      {"chk.s.i_RI", EM_CHK_S_I_R2_TARGET25},
      {""}, {""},
#line 117 "asm_c.gperf"
      {"czx2.l_RR", EM_CZX2_L_R1_R3},
#line 7 "asm_c.gperf"
      {"chk.s_FI", EM_CHK_S_F2_TARGET25},
#line 104 "asm_c.gperf"
      {"cmpxchg4.acq_RMRA", EM_CMPXCHG4_ACQ_R1_R3_R2_AR_CCV},
      {""}, {""},
#line 22 "asm_c.gperf"
      {"cmp4.ge.and_PPRR", EM_CMP4_GE_AND_P1_P2_R0_R3},
#line 25 "asm_c.gperf"
      {"cmp4.gt.and_PPRR", EM_CMP4_GT_AND_P1_P2_R0_R3},
      {""}, {""}, {""}, {""},
#line 15 "asm_c.gperf"
      {"cmp4.eq.or.andcm_PPRR", EM_CMP4_EQ_OR_ANDCM_P1_P2_R2_R3},
#line 14 "asm_c.gperf"
      {"cmp4.eq.or.andcm_PPIR", EM_CMP4_EQ_OR_ANDCM_P1_P2_IMM8_R3},
#line 45 "asm_c.gperf"
      {"cmp4.ne.or.andcm_PPRR", EM_CMP4_NE_OR_ANDCM_P1_P2_R2_R3},
#line 44 "asm_c.gperf"
      {"cmp4.ne.or.andcm_PPIR", EM_CMP4_NE_OR_ANDCM_P1_P2_IMM8_R3},
#line 29 "asm_c.gperf"
      {"cmp4.le.or.andcm_PPRR", EM_CMP4_LE_OR_ANDCM_P1_P2_R0_R3},
#line 32 "asm_c.gperf"
      {"cmp4.lt.or.andcm_PPRR", EM_CMP4_LT_OR_ANDCM_P1_P2_R0_R3},
#line 64 "asm_c.gperf"
      {"cmp.ge.and_PPRR", EM_CMP_GE_AND_P1_P2_R0_R3},
#line 67 "asm_c.gperf"
      {"cmp.gt.and_PPRR", EM_CMP_GT_AND_P1_P2_R0_R3},
      {""},
#line 116 "asm_c.gperf"
      {"czx1.r_RR", EM_CZX1_R_R1_R3},
#line 107 "asm_c.gperf"
      {"cmpxchg4.rel_RMRA", EM_CMPXCHG4_REL_R1_R3_R2_AR_CCV},
#line 103 "asm_c.gperf"
      {"cmpxchg4.acq.nta_RMRA", EM_CMPXCHG4_ACQ_NTA_R1_R3_R2_AR_CCV},
#line 24 "asm_c.gperf"
      {"cmp4.ge.or_PPRR", EM_CMP4_GE_OR_P1_P2_R0_R3},
#line 27 "asm_c.gperf"
      {"cmp4.gt.or_PPRR", EM_CMP4_GT_OR_P1_P2_R0_R3},
#line 110 "asm_c.gperf"
      {"cmpxchg8.acq_RMRA", EM_CMPXCHG8_ACQ_R1_R3_R2_AR_CCV},
      {""},
#line 118 "asm_c.gperf"
      {"czx2.r_RR", EM_CZX2_R_R1_R3},
      {""},
#line 66 "asm_c.gperf"
      {"cmp.ge.or_PPRR", EM_CMP_GE_OR_P1_P2_R0_R3},
#line 69 "asm_c.gperf"
      {"cmp.gt.or_PPRR", EM_CMP_GT_OR_P1_P2_R0_R3},
      {""}, {""}, {""}, {""},
#line 57 "asm_c.gperf"
      {"cmp.eq.or.andcm_PPRR", EM_CMP_EQ_OR_ANDCM_P1_P2_R2_R3},
#line 56 "asm_c.gperf"
      {"cmp.eq.or.andcm_PPIR", EM_CMP_EQ_OR_ANDCM_P1_P2_IMM8_R3},
#line 87 "asm_c.gperf"
      {"cmp.ne.or.andcm_PPRR", EM_CMP_NE_OR_ANDCM_P1_P2_R2_R3},
#line 86 "asm_c.gperf"
      {"cmp.ne.or.andcm_PPIR", EM_CMP_NE_OR_ANDCM_P1_P2_IMM8_R3},
#line 71 "asm_c.gperf"
      {"cmp.le.or.andcm_PPRR", EM_CMP_LE_OR_ANDCM_P1_P2_R0_R3},
#line 74 "asm_c.gperf"
      {"cmp.lt.or.andcm_PPRR", EM_CMP_LT_OR_ANDCM_P1_P2_R0_R3},
#line 106 "asm_c.gperf"
      {"cmpxchg4.rel.nta_RMRA", EM_CMPXCHG4_REL_NTA_R1_R3_R2_AR_CCV},
#line 50 "asm_c.gperf"
      {"cmp8xchg16.acq_RMRAA", EM_CMP8XCHG16_ACQ_R1_R3_R2_AR_CSD_AR_CCV},
      {""},
#line 113 "asm_c.gperf"
      {"cmpxchg8.rel_RMRA", EM_CMPXCHG8_REL_R1_R3_R2_AR_CCV},
#line 109 "asm_c.gperf"
      {"cmpxchg8.acq.nta_RMRA", EM_CMPXCHG8_ACQ_NTA_R1_R3_R2_AR_CCV},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 23 "asm_c.gperf"
      {"cmp4.ge.or.andcm_PPRR", EM_CMP4_GE_OR_ANDCM_P1_P2_R0_R3},
#line 26 "asm_c.gperf"
      {"cmp4.gt.or.andcm_PPRR", EM_CMP4_GT_OR_ANDCM_P1_P2_R0_R3},
      {""}, {""}, {""}, {""}, {""},
#line 92 "asm_c.gperf"
      {"cmpxchg1.acq_RMRA", EM_CMPXCHG1_ACQ_R1_R3_R2_AR_CCV},
#line 53 "asm_c.gperf"
      {"cmp8xchg16.rel_RMRAA", EM_CMP8XCHG16_REL_R1_R3_R2_AR_CSD_AR_CCV},
      {""}, {""},
#line 112 "asm_c.gperf"
      {"cmpxchg8.rel.nta_RMRA", EM_CMPXCHG8_REL_NTA_R1_R3_R2_AR_CCV},
      {""},
#line 102 "asm_c.gperf"
      {"cmpxchg4.acq.nt1_RMRA", EM_CMPXCHG4_ACQ_NT1_R1_R3_R2_AR_CCV},
#line 98 "asm_c.gperf"
      {"cmpxchg2.acq_RMRA", EM_CMPXCHG2_ACQ_R1_R3_R2_AR_CCV},
      {""}, {""}, {""},
#line 49 "asm_c.gperf"
      {"cmp8xchg16.acq.nta_RMRAA", EM_CMP8XCHG16_ACQ_NTA_R1_R3_R2_AR_CSD_AR_CCV},
      {""}, {""}, {""}, {""}, {""},
#line 65 "asm_c.gperf"
      {"cmp.ge.or.andcm_PPRR", EM_CMP_GE_OR_ANDCM_P1_P2_R0_R3},
#line 68 "asm_c.gperf"
      {"cmp.gt.or.andcm_PPRR", EM_CMP_GT_OR_ANDCM_P1_P2_R0_R3},
#line 95 "asm_c.gperf"
      {"cmpxchg1.rel_RMRA", EM_CMPXCHG1_REL_R1_R3_R2_AR_CCV},
#line 91 "asm_c.gperf"
      {"cmpxchg1.acq.nta_RMRA", EM_CMPXCHG1_ACQ_NTA_R1_R3_R2_AR_CCV},
      {""}, {""}, {""}, {""},
#line 105 "asm_c.gperf"
      {"cmpxchg4.rel.nt1_RMRA", EM_CMPXCHG4_REL_NT1_R1_R3_R2_AR_CCV},
#line 101 "asm_c.gperf"
      {"cmpxchg2.rel_RMRA", EM_CMPXCHG2_REL_R1_R3_R2_AR_CCV},
#line 97 "asm_c.gperf"
      {"cmpxchg2.acq.nta_RMRA", EM_CMPXCHG2_ACQ_NTA_R1_R3_R2_AR_CCV},
      {""},
#line 108 "asm_c.gperf"
      {"cmpxchg8.acq.nt1_RMRA", EM_CMPXCHG8_ACQ_NT1_R1_R3_R2_AR_CCV},
#line 52 "asm_c.gperf"
      {"cmp8xchg16.rel.nta_RMRAA", EM_CMP8XCHG16_REL_NTA_R1_R3_R2_AR_CSD_AR_CCV},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 94 "asm_c.gperf"
      {"cmpxchg1.rel.nta_RMRA", EM_CMPXCHG1_REL_NTA_R1_R3_R2_AR_CCV},
      {""}, {""}, {""}, {""}, {""}, {""},
#line 100 "asm_c.gperf"
      {"cmpxchg2.rel.nta_RMRA", EM_CMPXCHG2_REL_NTA_R1_R3_R2_AR_CCV},
      {""},
#line 111 "asm_c.gperf"
      {"cmpxchg8.rel.nt1_RMRA", EM_CMPXCHG8_REL_NT1_R1_R3_R2_AR_CCV},
      {""}, {""}, {""}, {""}, {""}, {""},
#line 48 "asm_c.gperf"
      {"cmp8xchg16.acq.nt1_RMRAA", EM_CMP8XCHG16_ACQ_NT1_R1_R3_R2_AR_CSD_AR_CCV},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 90 "asm_c.gperf"
      {"cmpxchg1.acq.nt1_RMRA", EM_CMPXCHG1_ACQ_NT1_R1_R3_R2_AR_CCV},
      {""}, {""}, {""}, {""}, {""}, {""},
#line 96 "asm_c.gperf"
      {"cmpxchg2.acq.nt1_RMRA", EM_CMPXCHG2_ACQ_NT1_R1_R3_R2_AR_CCV},
      {""}, {""},
#line 51 "asm_c.gperf"
      {"cmp8xchg16.rel.nt1_RMRAA", EM_CMP8XCHG16_REL_NT1_R1_R3_R2_AR_CSD_AR_CCV},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 93 "asm_c.gperf"
      {"cmpxchg1.rel.nt1_RMRA", EM_CMPXCHG1_REL_NT1_R1_R3_R2_AR_CCV},
      {""}, {""}, {""}, {""}, {""}, {""},
#line 99 "asm_c.gperf"
      {"cmpxchg2.rel.nt1_RMRA", EM_CMPXCHG2_REL_NT1_R1_R3_R2_AR_CCV}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = c_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H d_hash -N d_lookup -n -k 6,8 -T asm_d.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 9, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
/*ARGSUSED*/
static unsigned int
d_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11,  5, 11, 11, 11, 11, 11, 11,
      11, 11,  2, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11,  0, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11
    };
  return asso_values[(unsigned char)str[7]] + asso_values[(unsigned char)str[5]];
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
d_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 4,
      MIN_WORD_LENGTH = 9,
      MAX_WORD_LENGTH = 10,
      MIN_HASH_VALUE = 2,
      MAX_HASH_VALUE = 10
    };

  static const struct asm_id wordlist[] =
    {
      {""}, {""},
#line 6 "asm_d.gperf"
      {"dep.z_RRII", EM_DEP_Z_R1_R2_POS6_LEN6},
      {""}, {""},
#line 5 "asm_d.gperf"
      {"dep.z_RIII", EM_DEP_Z_R1_IMM8_POS6_LEN6},
      {""},
#line 4 "asm_d.gperf"
      {"dep_RRRII", EM_DEP_R1_R2_R3_POS6_LEN4},
      {""}, {""},
#line 3 "asm_d.gperf"
      {"dep_RIRII", EM_DEP_R1_IMM1_R3_POS6_LEN6}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = d_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H e_hash -N e_lookup -n -k 2,5 -T asm_e.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 3, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
e_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 2, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 1, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 0, 3, 3, 3, 3, 3, 3, 3,
      0, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3
    };
  register int hval = 0;

  switch (len)
    {
      default:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
      case 3:
      case 2:
        hval += asso_values[(unsigned char)str[1]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
e_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 3,
      MIN_WORD_LENGTH = 3,
      MAX_WORD_LENGTH = 11,
      MIN_HASH_VALUE = 0,
      MAX_HASH_VALUE = 2
    };

  static const struct asm_id wordlist[] =
    {
#line 3 "asm_e.gperf"
      {"epc", EM_EPC},
#line 4 "asm_e.gperf"
      {"extr_RRII", EM_EXTR_R1_R3_POS6_LEN6},
#line 5 "asm_e.gperf"
      {"extr.u_RRII", EM_EXTR_U_R1_R3_POS6_LEN6}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = e_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H f_hash -N f_lookup -o -j 1 -k 2-18 -T asm_f.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 886, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
f_hash (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      922, 922, 922, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922, 922,   0, 922,   3,   2,
        1,   0,   1, 922, 922, 922,   0, 922, 922, 922,
      922, 922, 922, 922, 922, 922, 922, 922, 922, 922,
        0, 922, 922,  15, 922, 922, 922,   2, 922, 922,
       35, 922,   0, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922,  32, 922,   7,   0,   0,
      293,  49,  24, 152,  30, 240, 922,   0,  71,   0,
        7,   5,   3,  99,  13,  39,  24,   7,  23,  46,
       16, 922, 922, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922, 922, 922, 922, 922, 922,
      922, 922, 922, 922, 922, 922
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[17]];
      /*FALLTHROUGH*/
      case 17:
        hval += asso_values[(unsigned char)str[16]];
      /*FALLTHROUGH*/
      case 16:
        hval += asso_values[(unsigned char)str[15]];
      /*FALLTHROUGH*/
      case 15:
        hval += asso_values[(unsigned char)str[14]];
      /*FALLTHROUGH*/
      case 14:
        hval += asso_values[(unsigned char)str[13]];
      /*FALLTHROUGH*/
      case 13:
        hval += asso_values[(unsigned char)str[12]];
      /*FALLTHROUGH*/
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      /*FALLTHROUGH*/
      case 11:
        hval += asso_values[(unsigned char)str[10]];
      /*FALLTHROUGH*/
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      /*FALLTHROUGH*/
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      /*FALLTHROUGH*/
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
f_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 243,
      MIN_WORD_LENGTH = 3,
      MAX_WORD_LENGTH = 22,
      MIN_HASH_VALUE = 36,
      MAX_HASH_VALUE = 921
    };

  static const struct asm_id wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 56 "asm_f.gperf"
      {"fc_R", EM_FC_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""},
#line 244 "asm_f.gperf"
      {"fwb", EM_FWB},
      {""},
#line 138 "asm_f.gperf"
      {"fpack_FFF", EM_FPACK_F1_F2_F3},
      {""}, {""}, {""}, {""}, {""},
#line 137 "asm_f.gperf"
      {"for_FFF", EM_FOR_F1_F2_F3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 245 "asm_f.gperf"
      {"fxor_FFF", EM_FXOR_F1_F2_F3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""},
#line 94 "asm_f.gperf"
      {"fma.s3_FFFF", EM_FMA_S3_F1_F3_F4_F2},
#line 93 "asm_f.gperf"
      {"fma.s2_FFFF", EM_FMA_S2_F1_F3_F4_F2},
#line 92 "asm_f.gperf"
      {"fma.s1_FFFF", EM_FMA_S1_F1_F3_F4_F2},
#line 91 "asm_f.gperf"
      {"fma.s0_FFFF", EM_FMA_S0_F1_F3_F4_F2},
#line 198 "asm_f.gperf"
      {"fpma.s3_FFFF", EM_FPMA_S3_F1_F3_F4_F2},
#line 197 "asm_f.gperf"
      {"fpma.s2_FFFF", EM_FPMA_S2_F1_F3_F4_F2},
#line 196 "asm_f.gperf"
      {"fpma.s1_FFFF", EM_FPMA_S1_F1_F3_F4_F2},
#line 195 "asm_f.gperf"
      {"fpma.s0_FFFF", EM_FPMA_S0_F1_F3_F4_F2},
#line 132 "asm_f.gperf"
      {"fnma.s3_FFFF", EM_FNMA_S3_F1_F3_F4_F2},
#line 131 "asm_f.gperf"
      {"fnma.s2_FFFF", EM_FNMA_S2_F1_F3_F4_F2},
#line 130 "asm_f.gperf"
      {"fnma.s1_FFFF", EM_FNMA_S1_F1_F3_F4_F2},
#line 129 "asm_f.gperf"
      {"fnma.s0_FFFF", EM_FNMA_S0_F1_F3_F4_F2},
#line 217 "asm_f.gperf"
      {"fpnma.s3_FFFF", EM_FPNMA_S3_F1_F3_F4_F2},
#line 216 "asm_f.gperf"
      {"fpnma.s2_FFFF", EM_FPNMA_S2_F1_F3_F4_F2},
#line 215 "asm_f.gperf"
      {"fpnma.s1_FFFF", EM_FPNMA_S1_F1_F3_F4_F2},
#line 214 "asm_f.gperf"
      {"fpnma.s0_FFFF", EM_FPNMA_S0_F1_F3_F4_F2},
#line 102 "asm_f.gperf"
      {"fmax.s3_FFF", EM_FMAX_S3_F1_F2_F3},
#line 101 "asm_f.gperf"
      {"fmax.s2_FFF", EM_FMAX_S2_F1_F2_F3},
#line 100 "asm_f.gperf"
      {"fmax.s1_FFF", EM_FMAX_S1_F1_F2_F3},
#line 99 "asm_f.gperf"
      {"fmax.s0_FFF", EM_FMAX_S0_F1_F2_F3},
#line 202 "asm_f.gperf"
      {"fpmax.s3_FFF", EM_FPMAX_S3_F1_F2_F3},
#line 201 "asm_f.gperf"
      {"fpmax.s2_FFF", EM_FPMAX_S2_F1_F2_F3},
#line 200 "asm_f.gperf"
      {"fpmax.s1_FFF", EM_FPMAX_S1_F1_F2_F3},
#line 199 "asm_f.gperf"
      {"fpmax.s0_FFF", EM_FPMAX_S0_F1_F2_F3},
#line 6 "asm_f.gperf"
      {"famax.s3_FFF", EM_FAMAX_S3_F1_F2_F3},
#line 5 "asm_f.gperf"
      {"famax.s2_FFF", EM_FAMAX_S2_F1_F2_F3},
#line 4 "asm_f.gperf"
      {"famax.s1_FFF", EM_FAMAX_S1_F1_F2_F3},
#line 3 "asm_f.gperf"
      {"famax.s0_FFF", EM_FAMAX_S0_F1_F2_F3},
#line 142 "asm_f.gperf"
      {"fpamax.s3_FFF", EM_FPAMAX_S3_F1_F2_F3},
#line 141 "asm_f.gperf"
      {"fpamax.s2_FFF", EM_FPAMAX_S2_F1_F2_F3},
#line 140 "asm_f.gperf"
      {"fpamax.s1_FFF", EM_FPAMAX_S1_F1_F2_F3},
#line 139 "asm_f.gperf"
      {"fpamax.s0_FFF", EM_FPAMAX_S0_F1_F2_F3},
#line 120 "asm_f.gperf"
      {"fms.s3_FFFF", EM_FMS_S3_F1_F3_F4_F2},
#line 119 "asm_f.gperf"
      {"fms.s2_FFFF", EM_FMS_S2_F1_F3_F4_F2},
#line 118 "asm_f.gperf"
      {"fms.s1_FFFF", EM_FMS_S1_F1_F3_F4_F2},
#line 117 "asm_f.gperf"
      {"fms.s0_FFFF", EM_FMS_S0_F1_F3_F4_F2},
#line 213 "asm_f.gperf"
      {"fpms.s3_FFFF", EM_FPMS_S3_F1_F3_F4_F2},
#line 212 "asm_f.gperf"
      {"fpms.s2_FFFF", EM_FPMS_S2_F1_F3_F4_F2},
#line 211 "asm_f.gperf"
      {"fpms.s1_FFFF", EM_FPMS_S1_F1_F3_F4_F2},
#line 210 "asm_f.gperf"
      {"fpms.s0_FFFF", EM_FPMS_S0_F1_F3_F4_F2},
#line 73 "asm_f.gperf"
      {"fcvt.xf_FF", EM_FCVT_XF_F1_F2},
#line 98 "asm_f.gperf"
      {"fma.s.s3_FFFF", EM_FMA_S_S3_F1_F3_F4_F2},
#line 97 "asm_f.gperf"
      {"fma.s.s2_FFFF", EM_FMA_S_S2_F1_F3_F4_F2},
#line 96 "asm_f.gperf"
      {"fma.s.s1_FFFF", EM_FMA_S_S1_F1_F3_F4_F2},
#line 95 "asm_f.gperf"
      {"fma.s.s0_FFFF", EM_FMA_S_S0_F1_F3_F4_F2},
#line 243 "asm_f.gperf"
      {"fsxt.r_FFF", EM_FSXT_R_F1_F2_F3},
      {""},
#line 239 "asm_f.gperf"
      {"fswap_FFF", EM_FSWAP_F1_F2_F3},
      {""},
#line 136 "asm_f.gperf"
      {"fnma.s.s3_FFFF", EM_FNMA_S_S3_F1_F3_F4_F2},
#line 135 "asm_f.gperf"
      {"fnma.s.s2_FFFF", EM_FNMA_S_S2_F1_F3_F4_F2},
#line 134 "asm_f.gperf"
      {"fnma.s.s1_FFFF", EM_FNMA_S_S1_F1_F3_F4_F2},
#line 133 "asm_f.gperf"
      {"fnma.s.s0_FFFF", EM_FNMA_S_S0_F1_F3_F4_F2},
#line 229 "asm_f.gperf"
      {"frcpa.s3_FPFF", EM_FRCPA_S3_F1_P2_F2_F3},
#line 228 "asm_f.gperf"
      {"frcpa.s2_FPFF", EM_FRCPA_S2_F1_P2_F2_F3},
#line 227 "asm_f.gperf"
      {"frcpa.s1_FPFF", EM_FRCPA_S1_F1_P2_F2_F3},
#line 226 "asm_f.gperf"
      {"frcpa.s0_FPFF", EM_FRCPA_S0_F1_P2_F2_F3},
#line 221 "asm_f.gperf"
      {"fprcpa.s3_FPFF", EM_FPRCPA_S3_F1_P2_F2_F3},
#line 220 "asm_f.gperf"
      {"fprcpa.s2_FPFF", EM_FPRCPA_S2_F1_P2_F2_F3},
#line 219 "asm_f.gperf"
      {"fprcpa.s1_FPFF", EM_FPRCPA_S1_F1_P2_F2_F3},
#line 218 "asm_f.gperf"
      {"fprcpa.s0_FPFF", EM_FPRCPA_S0_F1_P2_F2_F3},
#line 16 "asm_f.gperf"
      {"fchkf.s3_I", EM_FCHKF_S3_TARGET25},
#line 15 "asm_f.gperf"
      {"fchkf.s2_I", EM_FCHKF_S2_TARGET25},
#line 14 "asm_f.gperf"
      {"fchkf.s1_I", EM_FCHKF_S1_TARGET25},
#line 13 "asm_f.gperf"
      {"fchkf.s0_I", EM_FCHKF_S0_TARGET25},
      {""},
#line 23 "asm_f.gperf"
      {"fclrf.s3", EM_FCLRF_S3},
#line 22 "asm_f.gperf"
      {"fclrf.s2", EM_FCLRF_S2},
#line 21 "asm_f.gperf"
      {"fclrf.s1", EM_FCLRF_S1},
#line 20 "asm_f.gperf"
      {"fclrf.s0", EM_FCLRF_S0},
#line 241 "asm_f.gperf"
      {"fswap.nr_FFF", EM_FSWAP_NR_F1_F2_F3},
      {""}, {""},
#line 124 "asm_f.gperf"
      {"fms.s.s3_FFFF", EM_FMS_S_S3_F1_F3_F4_F2},
#line 123 "asm_f.gperf"
      {"fms.s.s2_FFFF", EM_FMS_S_S2_F1_F3_F4_F2},
#line 122 "asm_f.gperf"
      {"fms.s.s1_FFFF", EM_FMS_S_S1_F1_F3_F4_F2},
#line 121 "asm_f.gperf"
      {"fms.s.s0_FFFF", EM_FMS_S_S0_F1_F3_F4_F2},
      {""}, {""}, {""}, {""}, {""},
#line 60 "asm_f.gperf"
      {"fcvt.fx.s3_FF", EM_FCVT_FX_S3_F1_F2},
#line 59 "asm_f.gperf"
      {"fcvt.fx.s2_FF", EM_FCVT_FX_S2_F1_F2},
#line 58 "asm_f.gperf"
      {"fcvt.fx.s1_FF", EM_FCVT_FX_S1_F1_F2},
#line 57 "asm_f.gperf"
      {"fcvt.fx.s0_FF", EM_FCVT_FX_S0_F1_F2},
#line 182 "asm_f.gperf"
      {"fpcvt.fx.s3_FF", EM_FPCVT_FX_S3_F1_F2},
#line 181 "asm_f.gperf"
      {"fpcvt.fx.s2_FF", EM_FPCVT_FX_S2_F1_F2},
#line 180 "asm_f.gperf"
      {"fpcvt.fx.s1_FF", EM_FPCVT_FX_S1_F1_F2},
#line 179 "asm_f.gperf"
      {"fpcvt.fx.s0_FF", EM_FPCVT_FX_S0_F1_F2},
#line 68 "asm_f.gperf"
      {"fcvt.fxu.s3_FF", EM_FCVT_FXU_S3_F1_F2},
#line 67 "asm_f.gperf"
      {"fcvt.fxu.s2_FF", EM_FCVT_FXU_S2_F1_F2},
#line 66 "asm_f.gperf"
      {"fcvt.fxu.s1_FF", EM_FCVT_FXU_S1_F1_F2},
#line 65 "asm_f.gperf"
      {"fcvt.fxu.s0_FF", EM_FCVT_FXU_S0_F1_F2},
#line 190 "asm_f.gperf"
      {"fpcvt.fxu.s3_FF", EM_FPCVT_FXU_S3_F1_F2},
#line 189 "asm_f.gperf"
      {"fpcvt.fxu.s2_FF", EM_FPCVT_FXU_S2_F1_F2},
#line 188 "asm_f.gperf"
      {"fpcvt.fxu.s1_FF", EM_FPCVT_FXU_S1_F1_F2},
#line 187 "asm_f.gperf"
      {"fpcvt.fxu.s0_FF", EM_FPCVT_FXU_S0_F1_F2},
#line 158 "asm_f.gperf"
      {"fpcmp.lt.s3_FFF", EM_FPCMP_LT_S3_F1_F2_F3},
#line 157 "asm_f.gperf"
      {"fpcmp.lt.s2_FFF", EM_FPCMP_LT_S2_F1_F2_F3},
#line 156 "asm_f.gperf"
      {"fpcmp.lt.s1_FFF", EM_FPCMP_LT_S1_F1_F2_F3},
#line 155 "asm_f.gperf"
      {"fpcmp.lt.s0_FFF", EM_FPCMP_LT_S0_F1_F2_F3},
      {""},
#line 242 "asm_f.gperf"
      {"fsxt.l_FFF", EM_FSXT_L_F1_F2_F3},
      {""}, {""},
#line 170 "asm_f.gperf"
      {"fpcmp.nlt.s3_FFF", EM_FPCMP_NLT_S3_F1_F2_F3},
#line 169 "asm_f.gperf"
      {"fpcmp.nlt.s2_FFF", EM_FPCMP_NLT_S2_F1_F2_F3},
#line 168 "asm_f.gperf"
      {"fpcmp.nlt.s1_FFF", EM_FPCMP_NLT_S1_F1_F2_F3},
#line 167 "asm_f.gperf"
      {"fpcmp.nlt.s0_FFF", EM_FPCMP_NLT_S0_F1_F2_F3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 86 "asm_f.gperf"
      {"flushrs", EM_FLUSHRS},
      {""},
#line 194 "asm_f.gperf"
      {"fpcvt.fxu.trunc.s3_FF", EM_FPCVT_FXU_TRUNC_S3_F1_F2},
#line 193 "asm_f.gperf"
      {"fpcvt.fxu.trunc.s2_FF", EM_FPCVT_FXU_TRUNC_S2_F1_F2},
#line 192 "asm_f.gperf"
      {"fpcvt.fxu.trunc.s1_FF", EM_FPCVT_FXU_TRUNC_S1_F1_F2},
#line 191 "asm_f.gperf"
      {"fpcvt.fxu.trunc.s0_FF", EM_FPCVT_FXU_TRUNC_S0_F1_F2},
#line 154 "asm_f.gperf"
      {"fpcmp.le.s3_FFF", EM_FPCMP_LE_S3_F1_F2_F3},
#line 153 "asm_f.gperf"
      {"fpcmp.le.s2_FFF", EM_FPCMP_LE_S2_F1_F2_F3},
#line 152 "asm_f.gperf"
      {"fpcmp.le.s1_FFF", EM_FPCMP_LE_S1_F1_F2_F3},
#line 151 "asm_f.gperf"
      {"fpcmp.le.s0_FFF", EM_FPCMP_LE_S0_F1_F2_F3},
      {""},
#line 240 "asm_f.gperf"
      {"fswap.nl_FFF", EM_FSWAP_NL_F1_F2_F3},
      {""}, {""},
#line 166 "asm_f.gperf"
      {"fpcmp.nle.s3_FFF", EM_FPCMP_NLE_S3_F1_F2_F3},
#line 165 "asm_f.gperf"
      {"fpcmp.nle.s2_FFF", EM_FPCMP_NLE_S2_F1_F2_F3},
#line 164 "asm_f.gperf"
      {"fpcmp.nle.s1_FFF", EM_FPCMP_NLE_S1_F1_F2_F3},
#line 163 "asm_f.gperf"
      {"fpcmp.nle.s0_FFF", EM_FPCMP_NLE_S0_F1_F2_F3},
#line 238 "asm_f.gperf"
      {"fsetc.s3_II", EM_FSETC_S3_AMASK7_OMASK7},
#line 237 "asm_f.gperf"
      {"fsetc.s2_II", EM_FSETC_S2_AMASK7_OMASK7},
#line 236 "asm_f.gperf"
      {"fsetc.s1_II", EM_FSETC_S1_AMASK7_OMASK7},
#line 235 "asm_f.gperf"
      {"fsetc.s0_II", EM_FSETC_S0_AMASK7_OMASK7},
#line 64 "asm_f.gperf"
      {"fcvt.fx.trunc.s3_FF", EM_FCVT_FX_TRUNC_S3_F1_F2},
#line 63 "asm_f.gperf"
      {"fcvt.fx.trunc.s2_FF", EM_FCVT_FX_TRUNC_S2_F1_F2},
#line 62 "asm_f.gperf"
      {"fcvt.fx.trunc.s1_FF", EM_FCVT_FX_TRUNC_S1_F1_F2},
#line 61 "asm_f.gperf"
      {"fcvt.fx.trunc.s0_FF", EM_FCVT_FX_TRUNC_S0_F1_F2},
#line 186 "asm_f.gperf"
      {"fpcvt.fx.trunc.s3_FF", EM_FPCVT_FX_TRUNC_S3_F1_F2},
#line 185 "asm_f.gperf"
      {"fpcvt.fx.trunc.s2_FF", EM_FPCVT_FX_TRUNC_S2_F1_F2},
#line 184 "asm_f.gperf"
      {"fpcvt.fx.trunc.s1_FF", EM_FPCVT_FX_TRUNC_S1_F1_F2},
#line 183 "asm_f.gperf"
      {"fpcvt.fx.trunc.s0_FF", EM_FPCVT_FX_TRUNC_S0_F1_F2},
#line 72 "asm_f.gperf"
      {"fcvt.fxu.trunc.s3_FF", EM_FCVT_FXU_TRUNC_S3_F1_F2},
#line 71 "asm_f.gperf"
      {"fcvt.fxu.trunc.s2_FF", EM_FCVT_FXU_TRUNC_S2_F1_F2},
#line 70 "asm_f.gperf"
      {"fcvt.fxu.trunc.s1_FF", EM_FCVT_FXU_TRUNC_S1_F1_F2},
#line 69 "asm_f.gperf"
      {"fcvt.fxu.trunc.s0_FF", EM_FCVT_FXU_TRUNC_S0_F1_F2},
#line 150 "asm_f.gperf"
      {"fpcmp.eq.s3_FFF", EM_FPCMP_EQ_S3_F1_F2_F3},
#line 149 "asm_f.gperf"
      {"fpcmp.eq.s2_FFF", EM_FPCMP_EQ_S2_F1_F2_F3},
#line 148 "asm_f.gperf"
      {"fpcmp.eq.s1_FFF", EM_FPCMP_EQ_S1_F1_F2_F3},
#line 147 "asm_f.gperf"
      {"fpcmp.eq.s0_FFF", EM_FPCMP_EQ_S0_F1_F2_F3},
      {""}, {""}, {""}, {""},
#line 162 "asm_f.gperf"
      {"fpcmp.neq.s3_FFF", EM_FPCMP_NEQ_S3_F1_F2_F3},
#line 161 "asm_f.gperf"
      {"fpcmp.neq.s2_FFF", EM_FPCMP_NEQ_S2_F1_F2_F3},
#line 160 "asm_f.gperf"
      {"fpcmp.neq.s1_FFF", EM_FPCMP_NEQ_S1_F1_F2_F3},
#line 159 "asm_f.gperf"
      {"fpcmp.neq.s0_FFF", EM_FPCMP_NEQ_S0_F1_F2_F3},
      {""}, {""},
#line 43 "asm_f.gperf"
      {"fcmp.lt.s3_PPFF", EM_FCMP_LT_S3_P1_P2_F2_F3},
#line 42 "asm_f.gperf"
      {"fcmp.lt.s2_PPFF", EM_FCMP_LT_S2_P1_P2_F2_F3},
#line 41 "asm_f.gperf"
      {"fcmp.lt.s1_PPFF", EM_FCMP_LT_S1_P1_P2_F2_F3},
#line 40 "asm_f.gperf"
      {"fcmp.lt.s0_PPFF", EM_FCMP_LT_S0_P1_P2_F2_F3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""},
#line 47 "asm_f.gperf"
      {"fcmp.lt.unc.s3_PPFF", EM_FCMP_LT_UNC_S3_P1_P2_F2_F3},
#line 46 "asm_f.gperf"
      {"fcmp.lt.unc.s2_PPFF", EM_FCMP_LT_UNC_S2_P1_P2_F2_F3},
#line 45 "asm_f.gperf"
      {"fcmp.lt.unc.s1_PPFF", EM_FCMP_LT_UNC_S1_P1_P2_F2_F3},
#line 44 "asm_f.gperf"
      {"fcmp.lt.unc.s0_PPFF", EM_FCMP_LT_UNC_S0_P1_P2_F2_F3},
#line 234 "asm_f.gperf"
      {"fselect_FFFF", EM_FSELECT_F1_F3_F4_F2},
      {""},
#line 17 "asm_f.gperf"
      {"fc.i_R", EM_FC_I_R3},
#line 35 "asm_f.gperf"
      {"fcmp.le.s3_PPFF", EM_FCMP_LE_S3_P1_P2_F2_F3},
#line 34 "asm_f.gperf"
      {"fcmp.le.s2_PPFF", EM_FCMP_LE_S2_P1_P2_F2_F3},
#line 33 "asm_f.gperf"
      {"fcmp.le.s1_PPFF", EM_FCMP_LE_S1_P1_P2_F2_F3},
#line 32 "asm_f.gperf"
      {"fcmp.le.s0_PPFF", EM_FCMP_LE_S0_P1_P2_F2_F3},
      {""}, {""}, {""},
#line 18 "asm_f.gperf"
      {"fclass.m_PPFI", EM_FCLASS_M_P1_P2_F2_FCLASS9},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""},
#line 39 "asm_f.gperf"
      {"fcmp.le.unc.s3_PPFF", EM_FCMP_LE_UNC_S3_P1_P2_F2_F3},
#line 38 "asm_f.gperf"
      {"fcmp.le.unc.s2_PPFF", EM_FCMP_LE_UNC_S2_P1_P2_F2_F3},
#line 37 "asm_f.gperf"
      {"fcmp.le.unc.s1_PPFF", EM_FCMP_LE_UNC_S1_P1_P2_F2_F3},
#line 36 "asm_f.gperf"
      {"fcmp.le.unc.s0_PPFF", EM_FCMP_LE_UNC_S0_P1_P2_F2_F3},
      {""}, {""}, {""},
#line 19 "asm_f.gperf"
      {"fclass.m.unc_PPFI", EM_FCLASS_M_UNC_P1_P2_F2_FCLASS9},
      {""}, {""},
#line 27 "asm_f.gperf"
      {"fcmp.eq.s3_PPFF", EM_FCMP_EQ_S3_P1_P2_F2_F3},
#line 26 "asm_f.gperf"
      {"fcmp.eq.s2_PPFF", EM_FCMP_EQ_S2_P1_P2_F2_F3},
#line 25 "asm_f.gperf"
      {"fcmp.eq.s1_PPFF", EM_FCMP_EQ_S1_P1_P2_F2_F3},
#line 24 "asm_f.gperf"
      {"fcmp.eq.s0_PPFF", EM_FCMP_EQ_S0_P1_P2_F2_F3},
#line 112 "asm_f.gperf"
      {"fmix.r_FFF", EM_FMIX_R_F1_F2_F3},
      {""}, {""}, {""},
#line 233 "asm_f.gperf"
      {"frsqrta.s3_FPF", EM_FRSQRTA_S3_F1_P2_F3},
#line 232 "asm_f.gperf"
      {"frsqrta.s2_FPF", EM_FRSQRTA_S2_F1_P2_F3},
#line 231 "asm_f.gperf"
      {"frsqrta.s1_FPF", EM_FRSQRTA_S1_F1_P2_F3},
#line 230 "asm_f.gperf"
      {"frsqrta.s0_FPF", EM_FRSQRTA_S0_F1_P2_F3},
#line 225 "asm_f.gperf"
      {"fprsqrta.s3_FPF", EM_FPRSQRTA_S3_F1_P2_F3},
#line 224 "asm_f.gperf"
      {"fprsqrta.s2_FPF", EM_FPRSQRTA_S2_F1_P2_F3},
#line 223 "asm_f.gperf"
      {"fprsqrta.s1_FPF", EM_FPRSQRTA_S1_F1_P2_F3},
#line 222 "asm_f.gperf"
      {"fprsqrta.s0_FPF", EM_FPRSQRTA_S0_F1_P2_F3},
      {""}, {""},
#line 31 "asm_f.gperf"
      {"fcmp.eq.unc.s3_PPFF", EM_FCMP_EQ_UNC_S3_P1_P2_F2_F3},
#line 30 "asm_f.gperf"
      {"fcmp.eq.unc.s2_PPFF", EM_FCMP_EQ_UNC_S2_P1_P2_F2_F3},
#line 29 "asm_f.gperf"
      {"fcmp.eq.unc.s1_PPFF", EM_FCMP_EQ_UNC_S1_P1_P2_F2_F3},
#line 28 "asm_f.gperf"
      {"fcmp.eq.unc.s0_PPFF", EM_FCMP_EQ_UNC_S0_P1_P2_F2_F3},
#line 109 "asm_f.gperf"
      {"fmin.s3_FFF", EM_FMIN_S3_F1_F2_F3},
#line 108 "asm_f.gperf"
      {"fmin.s2_FFF", EM_FMIN_S2_F1_F2_F3},
#line 107 "asm_f.gperf"
      {"fmin.s1_FFF", EM_FMIN_S1_F1_F2_F3},
#line 106 "asm_f.gperf"
      {"fmin.s0_FFF", EM_FMIN_S0_F1_F2_F3},
#line 209 "asm_f.gperf"
      {"fpmin.s3_FFF", EM_FPMIN_S3_F1_F2_F3},
#line 208 "asm_f.gperf"
      {"fpmin.s2_FFF", EM_FPMIN_S2_F1_F2_F3},
#line 207 "asm_f.gperf"
      {"fpmin.s1_FFF", EM_FPMIN_S1_F1_F2_F3},
#line 206 "asm_f.gperf"
      {"fpmin.s0_FFF", EM_FPMIN_S0_F1_F2_F3},
#line 10 "asm_f.gperf"
      {"famin.s3_FFF", EM_FAMIN_S3_F1_F2_F3},
#line 9 "asm_f.gperf"
      {"famin.s2_FFF", EM_FAMIN_S2_F1_F2_F3},
#line 8 "asm_f.gperf"
      {"famin.s1_FFF", EM_FAMIN_S1_F1_F2_F3},
#line 7 "asm_f.gperf"
      {"famin.s0_FFF", EM_FAMIN_S0_F1_F2_F3},
#line 146 "asm_f.gperf"
      {"fpamin.s3_FFF", EM_FPAMIN_S3_F1_F2_F3},
#line 145 "asm_f.gperf"
      {"fpamin.s2_FFF", EM_FPAMIN_S2_F1_F2_F3},
#line 144 "asm_f.gperf"
      {"fpamin.s1_FFF", EM_FPAMIN_S1_F1_F2_F3},
#line 143 "asm_f.gperf"
      {"fpamin.s0_FFF", EM_FPAMIN_S0_F1_F2_F3},
      {""},
#line 105 "asm_f.gperf"
      {"fmerge.s_FFF", EM_FMERGE_S_F1_F2_F3},
#line 12 "asm_f.gperf"
      {"fand_FFF", EM_FAND_F1_F2_F3},
      {""},
#line 11 "asm_f.gperf"
      {"fandcm_FFF", EM_FANDCM_F1_F2_F3},
#line 205 "asm_f.gperf"
      {"fpmerge.s_FFF", EM_FPMERGE_S_F1_F2_F3},
      {""}, {""}, {""},
#line 103 "asm_f.gperf"
      {"fmerge.ns_FFF", EM_FMERGE_NS_F1_F2_F3},
      {""}, {""}, {""},
#line 203 "asm_f.gperf"
      {"fpmerge.ns_FFF", EM_FPMERGE_NS_F1_F2_F3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""},
#line 110 "asm_f.gperf"
      {"fmix.l_FFF", EM_FMIX_L_F1_F2_F3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""},
#line 111 "asm_f.gperf"
      {"fmix.lr_FFF", EM_FMIX_LR_F1_F2_F3},
#line 90 "asm_f.gperf"
      {"fma.d.s3_FFFF", EM_FMA_D_S3_F1_F3_F4_F2},
#line 89 "asm_f.gperf"
      {"fma.d.s2_FFFF", EM_FMA_D_S2_F1_F3_F4_F2},
#line 88 "asm_f.gperf"
      {"fma.d.s1_FFFF", EM_FMA_D_S1_F1_F3_F4_F2},
#line 87 "asm_f.gperf"
      {"fma.d.s0_FFFF", EM_FMA_D_S0_F1_F3_F4_F2},
      {""}, {""}, {""}, {""},
#line 128 "asm_f.gperf"
      {"fnma.d.s3_FFFF", EM_FNMA_D_S3_F1_F3_F4_F2},
#line 127 "asm_f.gperf"
      {"fnma.d.s2_FFFF", EM_FNMA_D_S2_F1_F3_F4_F2},
#line 126 "asm_f.gperf"
      {"fnma.d.s1_FFFF", EM_FNMA_D_S1_F1_F3_F4_F2},
#line 125 "asm_f.gperf"
      {"fnma.d.s0_FFFF", EM_FNMA_D_S0_F1_F3_F4_F2},
#line 104 "asm_f.gperf"
      {"fmerge.se_FFF", EM_FMERGE_SE_F1_F2_F3},
      {""}, {""}, {""},
#line 204 "asm_f.gperf"
      {"fpmerge.se_FFF", EM_FPMERGE_SE_F1_F2_F3},
      {""}, {""}, {""},
#line 174 "asm_f.gperf"
      {"fpcmp.ord.s3_FFF", EM_FPCMP_ORD_S3_F1_F2_F3},
#line 173 "asm_f.gperf"
      {"fpcmp.ord.s2_FFF", EM_FPCMP_ORD_S2_F1_F2_F3},
#line 172 "asm_f.gperf"
      {"fpcmp.ord.s1_FFF", EM_FPCMP_ORD_S1_F1_F2_F3},
#line 171 "asm_f.gperf"
      {"fpcmp.ord.s0_FFF", EM_FPCMP_ORD_S0_F1_F2_F3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 116 "asm_f.gperf"
      {"fms.d.s3_FFFF", EM_FMS_D_S3_F1_F3_F4_F2},
#line 115 "asm_f.gperf"
      {"fms.d.s2_FFFF", EM_FMS_D_S2_F1_F3_F4_F2},
#line 114 "asm_f.gperf"
      {"fms.d.s1_FFFF", EM_FMS_D_S1_F1_F3_F4_F2},
#line 113 "asm_f.gperf"
      {"fms.d.s0_FFFF", EM_FMS_D_S0_F1_F3_F4_F2},
#line 178 "asm_f.gperf"
      {"fpcmp.unord.s3_FFF", EM_FPCMP_UNORD_S3_F1_F2_F3},
#line 177 "asm_f.gperf"
      {"fpcmp.unord.s2_FFF", EM_FPCMP_UNORD_S2_F1_F2_F3},
#line 176 "asm_f.gperf"
      {"fpcmp.unord.s1_FFF", EM_FPCMP_UNORD_S1_F1_F2_F3},
#line 175 "asm_f.gperf"
      {"fpcmp.unord.s0_FFF", EM_FPCMP_UNORD_S0_F1_F2_F3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""},
#line 55 "asm_f.gperf"
      {"fcmp.unord.unc.s3_PPFF", EM_FCMP_UNORD_UNC_S3_P1_P2_F2_F3},
#line 54 "asm_f.gperf"
      {"fcmp.unord.unc.s2_PPFF", EM_FCMP_UNORD_UNC_S2_P1_P2_F2_F3},
#line 53 "asm_f.gperf"
      {"fcmp.unord.unc.s1_PPFF", EM_FCMP_UNORD_UNC_S1_P1_P2_F2_F3},
#line 52 "asm_f.gperf"
      {"fcmp.unord.unc.s0_PPFF", EM_FCMP_UNORD_UNC_S0_P1_P2_F2_F3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""},
#line 51 "asm_f.gperf"
      {"fcmp.unord.s3_PPFF", EM_FCMP_UNORD_S3_P1_P2_F2_F3},
#line 50 "asm_f.gperf"
      {"fcmp.unord.s2_PPFF", EM_FCMP_UNORD_S2_P1_P2_F2_F3},
#line 49 "asm_f.gperf"
      {"fcmp.unord.s1_PPFF", EM_FCMP_UNORD_S1_P1_P2_F2_F3},
#line 48 "asm_f.gperf"
      {"fcmp.unord.s0_PPFF", EM_FCMP_UNORD_S0_P1_P2_F2_F3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 82 "asm_f.gperf"
      {"fetchadd8.acq_RMI", EM_FETCHADD8_ACQ_R1_R3_INC3},
#line 76 "asm_f.gperf"
      {"fetchadd4.acq_RMI", EM_FETCHADD4_ACQ_R1_R3_INC3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 80 "asm_f.gperf"
      {"fetchadd8.acq.nt1_RMI", EM_FETCHADD8_ACQ_NT1_R1_R3_INC3},
#line 74 "asm_f.gperf"
      {"fetchadd4.acq.nt1_RMI", EM_FETCHADD4_ACQ_NT1_R1_R3_INC3},
      {""}, {""}, {""},
#line 81 "asm_f.gperf"
      {"fetchadd8.acq.nta_RMI", EM_FETCHADD8_ACQ_NTA_R1_R3_INC3},
#line 75 "asm_f.gperf"
      {"fetchadd4.acq.nta_RMI", EM_FETCHADD4_ACQ_NTA_R1_R3_INC3},
#line 85 "asm_f.gperf"
      {"fetchadd8.rel_RMI", EM_FETCHADD8_REL_R1_R3_INC3},
#line 79 "asm_f.gperf"
      {"fetchadd4.rel_RMI", EM_FETCHADD4_REL_R1_R3_INC3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 83 "asm_f.gperf"
      {"fetchadd8.rel.nt1_RMI", EM_FETCHADD8_REL_NT1_R1_R3_INC3},
#line 77 "asm_f.gperf"
      {"fetchadd4.rel.nt1_RMI", EM_FETCHADD4_REL_NT1_R1_R3_INC3},
      {""}, {""}, {""},
#line 84 "asm_f.gperf"
      {"fetchadd8.rel.nta_RMI", EM_FETCHADD8_REL_NTA_R1_R3_INC3},
#line 78 "asm_f.gperf"
      {"fetchadd4.rel.nta_RMI", EM_FETCHADD4_REL_NTA_R1_R3_INC3}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = f_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H g_hash -N g_lookup -n -k 6-7 -T asm_g.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 8, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
/*ARGSUSED*/
static unsigned int
g_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 2, 8, 8, 8, 8,
      5, 5, 8, 8, 8, 0, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 0, 8, 8, 8, 8,
      0, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8
    };
  return asso_values[(unsigned char)str[6]] + asso_values[(unsigned char)str[5]];
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
g_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 4,
      MIN_WORD_LENGTH = 9,
      MAX_WORD_LENGTH = 11,
      MIN_HASH_VALUE = 0,
      MAX_HASH_VALUE = 7
    };

  static const struct asm_id wordlist[] =
    {
#line 5 "asm_g.gperf"
      {"getf.sig_RF", EM_GETF_SIG_R1_F2},
      {""},
#line 6 "asm_g.gperf"
      {"getf.s_RF", EM_GETF_S_R1_F2},
      {""}, {""},
#line 4 "asm_g.gperf"
      {"getf.exp_RF", EM_GETF_EXP_R1_F2},
      {""},
#line 3 "asm_g.gperf"
      {"getf.d_RF", EM_GETF_D_R1_F2}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = g_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H h_hash -N h_lookup -o -j 1 -n -k 2,6 -T asm_h.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 5, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
/*ARGSUSED*/
static unsigned int
h_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 4, 7,
      7, 7, 3, 7, 7, 2, 7, 7, 7, 1,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      0, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7
    };
  return asso_values[(unsigned char)str[5]] + asso_values[(unsigned char)str[1]];
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
h_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 5,
      MIN_WORD_LENGTH = 8,
      MAX_WORD_LENGTH = 8,
      MIN_HASH_VALUE = 2,
      MAX_HASH_VALUE = 6
    };

  static const struct asm_id wordlist[] =
    {
      {""}, {""},
#line 7 "asm_h.gperf"
      {"hint.x_I", EM_HINT_X_IMM62},
#line 6 "asm_h.gperf"
      {"hint.m_I", EM_HINT_M_IMM21},
#line 5 "asm_h.gperf"
      {"hint.i_I", EM_HINT_I_IMM21},
#line 4 "asm_h.gperf"
      {"hint.f_I", EM_HINT_F_IMM21},
#line 3 "asm_h.gperf"
      {"hint.b_I", EM_HINT_B_IMM21}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = h_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H i_hash -N i_lookup -j 1 -n -k 3,5,10 -T asm_i.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 7, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
i_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      6, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 1, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 4,
      1, 7, 7, 7, 7, 0, 7, 7, 0, 7,
      7, 7, 7, 7, 1, 7, 7, 7, 0, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7
    };
  register int hval = 0;

  switch (len)
    {
      default:
        hval += asso_values[(unsigned char)str[9]];
      /*FALLTHROUGH*/
      case 9:
      case 8:
      case 7:
      case 6:
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
      case 3:
        hval += asso_values[(unsigned char)str[2]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
i_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 7,
      MIN_WORD_LENGTH = 6,
      MAX_WORD_LENGTH = 10,
      MIN_HASH_VALUE = 0,
      MAX_HASH_VALUE = 6
    };

  static const struct asm_id wordlist[] =
    {
#line 5 "asm_i.gperf"
      {"invala", EM_INVALA},
#line 4 "asm_i.gperf"
      {"invala.e_R", EM_INVALA_E_R1},
#line 9 "asm_i.gperf"
      {"itr.i_itrR", EM_ITR_I_ITR_R3_R2},
#line 8 "asm_i.gperf"
      {"itr.d_dtrR", EM_ITR_D_DTR_R3_R2},
#line 7 "asm_i.gperf"
      {"itc.i_R", EM_ITC_I_R2},
#line 6 "asm_i.gperf"
      {"itc.d_R", EM_ITC_D_R2},
#line 3 "asm_i.gperf"
      {"invala.e_F", EM_INVALA_E_F1}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = i_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

static const struct asm_id *
j_lookup (register const char *str, register unsigned int len)
{
  return 0;
}

static const struct asm_id *
k_lookup (register const char *str, register unsigned int len)
{
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H l_hash -N l_lookup -o -j 1 -k '3-17,21,$' -T asm_l.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 3438, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
l_hash (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477, 3477,    1, 3477, 3477,    2,
       907, 3477, 1010, 3477,   16, 3477,   84, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477,   15, 3477, 3477, 3477, 3477,
         0, 3477, 3477,    0, 3477, 3477, 3477,   10, 3477, 3477,
      3477, 3477,   19, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477,    1, 3477,    0,  244,  233,
       256,  943,  152, 3477,   12,  889, 3477, 3477,    4, 3477,
         0, 3477,  132,  529,    3,   17,    0,    0, 3477, 3477,
         0, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477, 3477,
      3477, 3477, 3477, 3477, 3477, 3477
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[20]];
      /*FALLTHROUGH*/
      case 20:
      case 19:
      case 18:
      case 17:
        hval += asso_values[(unsigned char)str[16]];
      /*FALLTHROUGH*/
      case 16:
        hval += asso_values[(unsigned char)str[15]];
      /*FALLTHROUGH*/
      case 15:
        hval += asso_values[(unsigned char)str[14]];
      /*FALLTHROUGH*/
      case 14:
        hval += asso_values[(unsigned char)str[13]];
      /*FALLTHROUGH*/
      case 13:
        hval += asso_values[(unsigned char)str[12]];
      /*FALLTHROUGH*/
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      /*FALLTHROUGH*/
      case 11:
        hval += asso_values[(unsigned char)str[10]];
      /*FALLTHROUGH*/
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      /*FALLTHROUGH*/
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      /*FALLTHROUGH*/
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
        hval += asso_values[(unsigned char)str[2]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
l_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 721,
      MIN_WORD_LENGTH = 6,
      MAX_WORD_LENGTH = 24,
      MIN_HASH_VALUE = 39,
      MAX_HASH_VALUE = 3476
    };

  static const struct asm_id wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""},
#line 69 "asm_l.gperf"
      {"ld1_RMI", EM_LD1_R1_R3_IMM9},
      {""}, {""},
#line 24 "asm_l.gperf"
      {"ld1.a_RMI", EM_LD1_A_R1_R3_IMM9},
      {""},
#line 66 "asm_l.gperf"
      {"ld1.nta_RMI", EM_LD1_NTA_R1_R3_IMM9},
      {""},
#line 63 "asm_l.gperf"
      {"ld1.nt1_RMI", EM_LD1_NT1_R1_R3_IMM9},
#line 21 "asm_l.gperf"
      {"ld1.a.nta_RMI", EM_LD1_A_NTA_R1_R3_IMM9},
#line 70 "asm_l.gperf"
      {"ld1_RM", EM_LD1_R1_R3},
#line 18 "asm_l.gperf"
      {"ld1.a.nt1_RMI", EM_LD1_A_NT1_R1_R3_IMM9},
      {""},
#line 25 "asm_l.gperf"
      {"ld1.a_RM", EM_LD1_A_R1_R3},
      {""},
#line 67 "asm_l.gperf"
      {"ld1.nta_RM", EM_LD1_NTA_R1_R3},
      {""},
#line 64 "asm_l.gperf"
      {"ld1.nt1_RM", EM_LD1_NT1_R1_R3},
#line 22 "asm_l.gperf"
      {"ld1.a.nta_RM", EM_LD1_A_NTA_R1_R3},
      {""},
#line 19 "asm_l.gperf"
      {"ld1.a.nt1_RM", EM_LD1_A_NT1_R1_R3},
#line 87 "asm_l.gperf"
      {"ld1.s_RMI", EM_LD1_S_R1_R3_IMM9},
#line 78 "asm_l.gperf"
      {"ld1.sa_RMI", EM_LD1_SA_R1_R3_IMM9},
      {""}, {""}, {""},
#line 84 "asm_l.gperf"
      {"ld1.s.nta_RMI", EM_LD1_S_NTA_R1_R3_IMM9},
#line 75 "asm_l.gperf"
      {"ld1.sa.nta_RMI", EM_LD1_SA_NTA_R1_R3_IMM9},
#line 81 "asm_l.gperf"
      {"ld1.s.nt1_RMI", EM_LD1_S_NT1_R1_R3_IMM9},
#line 72 "asm_l.gperf"
      {"ld1.sa.nt1_RMI", EM_LD1_SA_NT1_R1_R3_IMM9},
#line 88 "asm_l.gperf"
      {"ld1.s_RM", EM_LD1_S_R1_R3},
#line 79 "asm_l.gperf"
      {"ld1.sa_RM", EM_LD1_SA_R1_R3},
      {""}, {""}, {""},
#line 85 "asm_l.gperf"
      {"ld1.s.nta_RM", EM_LD1_S_NTA_R1_R3},
#line 76 "asm_l.gperf"
      {"ld1.sa.nta_RM", EM_LD1_SA_NTA_R1_R3},
#line 82 "asm_l.gperf"
      {"ld1.s.nt1_RM", EM_LD1_S_NT1_R1_R3},
#line 73 "asm_l.gperf"
      {"ld1.sa.nt1_RM", EM_LD1_SA_NT1_R1_R3},
#line 71 "asm_l.gperf"
      {"ld1_RMR", EM_LD1_R1_R3_R2},
      {""}, {""},
#line 26 "asm_l.gperf"
      {"ld1.a_RMR", EM_LD1_A_R1_R3_R2},
#line 8 "asm_l.gperf"
      {"ld16_RAM", EM_LD16_R1_AR_CSD_R3},
#line 68 "asm_l.gperf"
      {"ld1.nta_RMR", EM_LD1_NTA_R1_R3_R2},
      {""},
#line 65 "asm_l.gperf"
      {"ld1.nt1_RMR", EM_LD1_NT1_R1_R3_R2},
#line 23 "asm_l.gperf"
      {"ld1.a.nta_RMR", EM_LD1_A_NTA_R1_R3_R2},
#line 7 "asm_l.gperf"
      {"ld16.nta_RAM", EM_LD16_NTA_R1_AR_CSD_R3},
#line 20 "asm_l.gperf"
      {"ld1.a.nt1_RMR", EM_LD1_A_NT1_R1_R3_R2},
#line 6 "asm_l.gperf"
      {"ld16.nt1_RAM", EM_LD16_NT1_R1_AR_CSD_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 89 "asm_l.gperf"
      {"ld1.s_RMR", EM_LD1_S_R1_R3_R2},
#line 80 "asm_l.gperf"
      {"ld1.sa_RMR", EM_LD1_SA_R1_R3_R2},
      {""}, {""}, {""},
#line 86 "asm_l.gperf"
      {"ld1.s.nta_RMR", EM_LD1_S_NTA_R1_R3_R2},
#line 77 "asm_l.gperf"
      {"ld1.sa.nta_RMR", EM_LD1_SA_NTA_R1_R3_R2},
#line 83 "asm_l.gperf"
      {"ld1.s.nt1_RMR", EM_LD1_S_NT1_R1_R3_R2},
#line 74 "asm_l.gperf"
      {"ld1.sa.nt1_RMR", EM_LD1_SA_NT1_R1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""},
#line 321 "asm_l.gperf"
      {"ld8_RMI", EM_LD8_R1_R3_IMM9},
      {""}, {""},
#line 267 "asm_l.gperf"
      {"ld8.a_RMI", EM_LD8_A_R1_R3_IMM9},
      {""},
#line 318 "asm_l.gperf"
      {"ld8.nta_RMI", EM_LD8_NTA_R1_R3_IMM9},
      {""},
#line 315 "asm_l.gperf"
      {"ld8.nt1_RMI", EM_LD8_NT1_R1_R3_IMM9},
#line 264 "asm_l.gperf"
      {"ld8.a.nta_RMI", EM_LD8_A_NTA_R1_R3_IMM9},
#line 322 "asm_l.gperf"
      {"ld8_RM", EM_LD8_R1_R3},
#line 261 "asm_l.gperf"
      {"ld8.a.nt1_RMI", EM_LD8_A_NT1_R1_R3_IMM9},
      {""},
#line 268 "asm_l.gperf"
      {"ld8.a_RM", EM_LD8_A_R1_R3},
      {""},
#line 319 "asm_l.gperf"
      {"ld8.nta_RM", EM_LD8_NTA_R1_R3},
      {""},
#line 316 "asm_l.gperf"
      {"ld8.nt1_RM", EM_LD8_NT1_R1_R3},
#line 265 "asm_l.gperf"
      {"ld8.a.nta_RM", EM_LD8_A_NTA_R1_R3},
      {""},
#line 262 "asm_l.gperf"
      {"ld8.a.nt1_RM", EM_LD8_A_NT1_R1_R3},
#line 339 "asm_l.gperf"
      {"ld8.s_RMI", EM_LD8_S_R1_R3_IMM9},
#line 330 "asm_l.gperf"
      {"ld8.sa_RMI", EM_LD8_SA_R1_R3_IMM9},
      {""}, {""}, {""},
#line 336 "asm_l.gperf"
      {"ld8.s.nta_RMI", EM_LD8_S_NTA_R1_R3_IMM9},
#line 327 "asm_l.gperf"
      {"ld8.sa.nta_RMI", EM_LD8_SA_NTA_R1_R3_IMM9},
#line 333 "asm_l.gperf"
      {"ld8.s.nt1_RMI", EM_LD8_S_NT1_R1_R3_IMM9},
#line 324 "asm_l.gperf"
      {"ld8.sa.nt1_RMI", EM_LD8_SA_NT1_R1_R3_IMM9},
#line 340 "asm_l.gperf"
      {"ld8.s_RM", EM_LD8_S_R1_R3},
#line 331 "asm_l.gperf"
      {"ld8.sa_RM", EM_LD8_SA_R1_R3},
      {""}, {""}, {""},
#line 337 "asm_l.gperf"
      {"ld8.s.nta_RM", EM_LD8_S_NTA_R1_R3},
#line 328 "asm_l.gperf"
      {"ld8.sa.nta_RM", EM_LD8_SA_NTA_R1_R3},
#line 334 "asm_l.gperf"
      {"ld8.s.nt1_RM", EM_LD8_S_NT1_R1_R3},
#line 325 "asm_l.gperf"
      {"ld8.sa.nt1_RM", EM_LD8_SA_NT1_R1_R3},
#line 323 "asm_l.gperf"
      {"ld8_RMR", EM_LD8_R1_R3_R2},
      {""}, {""},
#line 269 "asm_l.gperf"
      {"ld8.a_RMR", EM_LD8_A_R1_R3_R2},
      {""},
#line 320 "asm_l.gperf"
      {"ld8.nta_RMR", EM_LD8_NTA_R1_R3_R2},
      {""},
#line 317 "asm_l.gperf"
      {"ld8.nt1_RMR", EM_LD8_NT1_R1_R3_R2},
#line 266 "asm_l.gperf"
      {"ld8.a.nta_RMR", EM_LD8_A_NTA_R1_R3_R2},
      {""},
#line 263 "asm_l.gperf"
      {"ld8.a.nt1_RMR", EM_LD8_A_NT1_R1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 341 "asm_l.gperf"
      {"ld8.s_RMR", EM_LD8_S_R1_R3_R2},
#line 332 "asm_l.gperf"
      {"ld8.sa_RMR", EM_LD8_SA_R1_R3_R2},
      {""}, {""}, {""},
#line 338 "asm_l.gperf"
      {"ld8.s.nta_RMR", EM_LD8_S_NTA_R1_R3_R2},
#line 329 "asm_l.gperf"
      {"ld8.sa.nta_RMR", EM_LD8_SA_NTA_R1_R3_R2},
#line 335 "asm_l.gperf"
      {"ld8.s.nt1_RMR", EM_LD8_S_NT1_R1_R3_R2},
#line 326 "asm_l.gperf"
      {"ld8.sa.nt1_RMR", EM_LD8_SA_NT1_R1_R3_R2},
#line 648 "asm_l.gperf"
      {"ldfs_FMI", EM_LDFS_F1_R3_IMM9},
      {""}, {""},
#line 621 "asm_l.gperf"
      {"ldfs.a_FMI", EM_LDFS_A_F1_R3_IMM9},
      {""},
#line 654 "asm_l.gperf"
      {"ldfs.nta_FMI", EM_LDFS_NTA_F1_R3_IMM9},
      {""},
#line 651 "asm_l.gperf"
      {"ldfs.nt1_FMI", EM_LDFS_NT1_F1_R3_IMM9},
#line 627 "asm_l.gperf"
      {"ldfs.a.nta_FMI", EM_LDFS_A_NTA_F1_R3_IMM9},
#line 649 "asm_l.gperf"
      {"ldfs_FM", EM_LDFS_F1_R3},
#line 624 "asm_l.gperf"
      {"ldfs.a.nt1_FMI", EM_LDFS_A_NT1_F1_R3_IMM9},
      {""},
#line 622 "asm_l.gperf"
      {"ldfs.a_FM", EM_LDFS_A_F1_R3},
      {""},
#line 655 "asm_l.gperf"
      {"ldfs.nta_FM", EM_LDFS_NTA_F1_R3},
      {""},
#line 652 "asm_l.gperf"
      {"ldfs.nt1_FM", EM_LDFS_NT1_F1_R3},
#line 628 "asm_l.gperf"
      {"ldfs.a.nta_FM", EM_LDFS_A_NTA_F1_R3},
      {""},
#line 625 "asm_l.gperf"
      {"ldfs.a.nt1_FM", EM_LDFS_A_NT1_F1_R3},
#line 666 "asm_l.gperf"
      {"ldfs.s_FMI", EM_LDFS_S_F1_R3_IMM9},
#line 657 "asm_l.gperf"
      {"ldfs.sa_FMI", EM_LDFS_SA_F1_R3_IMM9},
      {""}, {""}, {""},
#line 672 "asm_l.gperf"
      {"ldfs.s.nta_FMI", EM_LDFS_S_NTA_F1_R3_IMM9},
#line 663 "asm_l.gperf"
      {"ldfs.sa.nta_FMI", EM_LDFS_SA_NTA_F1_R3_IMM9},
#line 669 "asm_l.gperf"
      {"ldfs.s.nt1_FMI", EM_LDFS_S_NT1_F1_R3_IMM9},
#line 660 "asm_l.gperf"
      {"ldfs.sa.nt1_FMI", EM_LDFS_SA_NT1_F1_R3_IMM9},
#line 667 "asm_l.gperf"
      {"ldfs.s_FM", EM_LDFS_S_F1_R3},
#line 658 "asm_l.gperf"
      {"ldfs.sa_FM", EM_LDFS_SA_F1_R3},
      {""}, {""}, {""},
#line 673 "asm_l.gperf"
      {"ldfs.s.nta_FM", EM_LDFS_S_NTA_F1_R3},
#line 664 "asm_l.gperf"
      {"ldfs.sa.nta_FM", EM_LDFS_SA_NTA_F1_R3},
#line 670 "asm_l.gperf"
      {"ldfs.s.nt1_FM", EM_LDFS_S_NT1_F1_R3},
#line 661 "asm_l.gperf"
      {"ldfs.sa.nt1_FM", EM_LDFS_SA_NT1_F1_R3},
#line 650 "asm_l.gperf"
      {"ldfs_FMR", EM_LDFS_F1_R3_R2},
      {""}, {""},
#line 623 "asm_l.gperf"
      {"ldfs.a_FMR", EM_LDFS_A_F1_R3_R2},
      {""},
#line 656 "asm_l.gperf"
      {"ldfs.nta_FMR", EM_LDFS_NTA_F1_R3_R2},
      {""},
#line 653 "asm_l.gperf"
      {"ldfs.nt1_FMR", EM_LDFS_NT1_F1_R3_R2},
#line 629 "asm_l.gperf"
      {"ldfs.a.nta_FMR", EM_LDFS_A_NTA_F1_R3_R2},
      {""},
#line 626 "asm_l.gperf"
      {"ldfs.a.nt1_FMR", EM_LDFS_A_NT1_F1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 668 "asm_l.gperf"
      {"ldfs.s_FMR", EM_LDFS_S_F1_R3_R2},
#line 659 "asm_l.gperf"
      {"ldfs.sa_FMR", EM_LDFS_SA_F1_R3_R2},
      {""}, {""}, {""},
#line 674 "asm_l.gperf"
      {"ldfs.s.nta_FMR", EM_LDFS_S_NTA_F1_R3_R2},
#line 665 "asm_l.gperf"
      {"ldfs.sa.nta_FMR", EM_LDFS_SA_NTA_F1_R3_R2},
#line 671 "asm_l.gperf"
      {"ldfs.s.nt1_FMR", EM_LDFS_S_NT1_F1_R3_R2},
#line 662 "asm_l.gperf"
      {"ldfs.sa.nt1_FMR", EM_LDFS_SA_NT1_F1_R3_R2},
#line 369 "asm_l.gperf"
      {"ldf8_FMI", EM_LDF8_F1_R3_IMM9},
      {""}, {""},
#line 342 "asm_l.gperf"
      {"ldf8.a_FMI", EM_LDF8_A_F1_R3_IMM9},
      {""},
#line 375 "asm_l.gperf"
      {"ldf8.nta_FMI", EM_LDF8_NTA_F1_R3_IMM9},
      {""},
#line 372 "asm_l.gperf"
      {"ldf8.nt1_FMI", EM_LDF8_NT1_F1_R3_IMM9},
#line 348 "asm_l.gperf"
      {"ldf8.a.nta_FMI", EM_LDF8_A_NTA_F1_R3_IMM9},
#line 370 "asm_l.gperf"
      {"ldf8_FM", EM_LDF8_F1_R3},
#line 345 "asm_l.gperf"
      {"ldf8.a.nt1_FMI", EM_LDF8_A_NT1_F1_R3_IMM9},
      {""},
#line 343 "asm_l.gperf"
      {"ldf8.a_FM", EM_LDF8_A_F1_R3},
      {""},
#line 376 "asm_l.gperf"
      {"ldf8.nta_FM", EM_LDF8_NTA_F1_R3},
      {""},
#line 373 "asm_l.gperf"
      {"ldf8.nt1_FM", EM_LDF8_NT1_F1_R3},
#line 349 "asm_l.gperf"
      {"ldf8.a.nta_FM", EM_LDF8_A_NTA_F1_R3},
      {""},
#line 346 "asm_l.gperf"
      {"ldf8.a.nt1_FM", EM_LDF8_A_NT1_F1_R3},
#line 387 "asm_l.gperf"
      {"ldf8.s_FMI", EM_LDF8_S_F1_R3_IMM9},
#line 378 "asm_l.gperf"
      {"ldf8.sa_FMI", EM_LDF8_SA_F1_R3_IMM9},
      {""}, {""}, {""},
#line 393 "asm_l.gperf"
      {"ldf8.s.nta_FMI", EM_LDF8_S_NTA_F1_R3_IMM9},
#line 384 "asm_l.gperf"
      {"ldf8.sa.nta_FMI", EM_LDF8_SA_NTA_F1_R3_IMM9},
#line 390 "asm_l.gperf"
      {"ldf8.s.nt1_FMI", EM_LDF8_S_NT1_F1_R3_IMM9},
#line 381 "asm_l.gperf"
      {"ldf8.sa.nt1_FMI", EM_LDF8_SA_NT1_F1_R3_IMM9},
#line 388 "asm_l.gperf"
      {"ldf8.s_FM", EM_LDF8_S_F1_R3},
#line 379 "asm_l.gperf"
      {"ldf8.sa_FM", EM_LDF8_SA_F1_R3},
      {""}, {""}, {""},
#line 394 "asm_l.gperf"
      {"ldf8.s.nta_FM", EM_LDF8_S_NTA_F1_R3},
#line 385 "asm_l.gperf"
      {"ldf8.sa.nta_FM", EM_LDF8_SA_NTA_F1_R3},
#line 391 "asm_l.gperf"
      {"ldf8.s.nt1_FM", EM_LDF8_S_NT1_F1_R3},
#line 382 "asm_l.gperf"
      {"ldf8.sa.nt1_FM", EM_LDF8_SA_NT1_F1_R3},
#line 371 "asm_l.gperf"
      {"ldf8_FMR", EM_LDF8_F1_R3_R2},
      {""}, {""},
#line 344 "asm_l.gperf"
      {"ldf8.a_FMR", EM_LDF8_A_F1_R3_R2},
      {""},
#line 377 "asm_l.gperf"
      {"ldf8.nta_FMR", EM_LDF8_NTA_F1_R3_R2},
#line 723 "asm_l.gperf"
      {"loadrs", EM_LOADRS},
#line 374 "asm_l.gperf"
      {"ldf8.nt1_FMR", EM_LDF8_NT1_F1_R3_R2},
#line 350 "asm_l.gperf"
      {"ldf8.a.nta_FMR", EM_LDF8_A_NTA_F1_R3_R2},
      {""},
#line 347 "asm_l.gperf"
      {"ldf8.a.nt1_FMR", EM_LDF8_A_NT1_F1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 389 "asm_l.gperf"
      {"ldf8.s_FMR", EM_LDF8_S_F1_R3_R2},
#line 380 "asm_l.gperf"
      {"ldf8.sa_FMR", EM_LDF8_SA_F1_R3_R2},
      {""}, {""}, {""},
#line 395 "asm_l.gperf"
      {"ldf8.s.nta_FMR", EM_LDF8_S_NTA_F1_R3_R2},
#line 386 "asm_l.gperf"
      {"ldf8.sa.nta_FMR", EM_LDF8_SA_NTA_F1_R3_R2},
#line 392 "asm_l.gperf"
      {"ldf8.s.nt1_FMR", EM_LDF8_S_NT1_F1_R3_R2},
#line 383 "asm_l.gperf"
      {"ldf8.sa.nt1_FMR", EM_LDF8_SA_NT1_F1_R3_R2},
#line 603 "asm_l.gperf"
      {"ldfps_FFMI", EM_LDFPS_F1_F2_R3_8},
      {""}, {""},
#line 585 "asm_l.gperf"
      {"ldfps.a_FFMI", EM_LDFPS_A_F1_F2_R3_8},
      {""},
#line 607 "asm_l.gperf"
      {"ldfps.nta_FFMI", EM_LDFPS_NTA_F1_F2_R3_8},
      {""},
#line 605 "asm_l.gperf"
      {"ldfps.nt1_FFMI", EM_LDFPS_NT1_F1_F2_R3_8},
#line 589 "asm_l.gperf"
      {"ldfps.a.nta_FFMI", EM_LDFPS_A_NTA_F1_F2_R3_8},
#line 604 "asm_l.gperf"
      {"ldfps_FFM", EM_LDFPS_F1_F2_R3},
#line 587 "asm_l.gperf"
      {"ldfps.a.nt1_FFMI", EM_LDFPS_A_NT1_F1_F2_R3_8},
      {""},
#line 586 "asm_l.gperf"
      {"ldfps.a_FFM", EM_LDFPS_A_F1_F2_R3},
      {""},
#line 608 "asm_l.gperf"
      {"ldfps.nta_FFM", EM_LDFPS_NTA_F1_F2_R3},
      {""},
#line 606 "asm_l.gperf"
      {"ldfps.nt1_FFM", EM_LDFPS_NT1_F1_F2_R3},
#line 590 "asm_l.gperf"
      {"ldfps.a.nta_FFM", EM_LDFPS_A_NTA_F1_F2_R3},
      {""},
#line 588 "asm_l.gperf"
      {"ldfps.a.nt1_FFM", EM_LDFPS_A_NT1_F1_F2_R3},
#line 615 "asm_l.gperf"
      {"ldfps.s_FFMI", EM_LDFPS_S_F1_F2_R3_8},
#line 609 "asm_l.gperf"
      {"ldfps.sa_FFMI", EM_LDFPS_SA_F1_F2_R3_8},
      {""}, {""}, {""},
#line 619 "asm_l.gperf"
      {"ldfps.s.nta_FFMI", EM_LDFPS_S_NTA_F1_F2_R3_8},
#line 613 "asm_l.gperf"
      {"ldfps.sa.nta_FFMI", EM_LDFPS_SA_NTA_F1_F2_R3_8},
#line 617 "asm_l.gperf"
      {"ldfps.s.nt1_FFMI", EM_LDFPS_S_NT1_F1_F2_R3_8},
#line 611 "asm_l.gperf"
      {"ldfps.sa.nt1_FFMI", EM_LDFPS_SA_NT1_F1_F2_R3_8},
#line 616 "asm_l.gperf"
      {"ldfps.s_FFM", EM_LDFPS_S_F1_F2_R3},
#line 610 "asm_l.gperf"
      {"ldfps.sa_FFM", EM_LDFPS_SA_F1_F2_R3},
      {""}, {""}, {""},
#line 620 "asm_l.gperf"
      {"ldfps.s.nta_FFM", EM_LDFPS_S_NTA_F1_F2_R3},
#line 614 "asm_l.gperf"
      {"ldfps.sa.nta_FFM", EM_LDFPS_SA_NTA_F1_F2_R3},
#line 618 "asm_l.gperf"
      {"ldfps.s.nt1_FFM", EM_LDFPS_S_NT1_F1_F2_R3},
#line 612 "asm_l.gperf"
      {"ldfps.sa.nt1_FFM", EM_LDFPS_SA_NT1_F1_F2_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""},
#line 531 "asm_l.gperf"
      {"ldfp8_FFMI", EM_LDFP8_F1_F2_R3_16},
      {""}, {""},
#line 513 "asm_l.gperf"
      {"ldfp8.a_FFMI", EM_LDFP8_A_F1_F2_R3_16},
      {""},
#line 535 "asm_l.gperf"
      {"ldfp8.nta_FFMI", EM_LDFP8_NTA_F1_F2_R3_16},
      {""},
#line 533 "asm_l.gperf"
      {"ldfp8.nt1_FFMI", EM_LDFP8_NT1_F1_F2_R3_16},
#line 517 "asm_l.gperf"
      {"ldfp8.a.nta_FFMI", EM_LDFP8_A_NTA_F1_F2_R3_16},
#line 532 "asm_l.gperf"
      {"ldfp8_FFM", EM_LDFP8_F1_F2_R3},
#line 515 "asm_l.gperf"
      {"ldfp8.a.nt1_FFMI", EM_LDFP8_A_NT1_F1_F2_R3_16},
      {""},
#line 514 "asm_l.gperf"
      {"ldfp8.a_FFM", EM_LDFP8_A_F1_F2_R3},
      {""},
#line 536 "asm_l.gperf"
      {"ldfp8.nta_FFM", EM_LDFP8_NTA_F1_F2_R3},
      {""},
#line 534 "asm_l.gperf"
      {"ldfp8.nt1_FFM", EM_LDFP8_NT1_F1_F2_R3},
#line 518 "asm_l.gperf"
      {"ldfp8.a.nta_FFM", EM_LDFP8_A_NTA_F1_F2_R3},
      {""},
#line 516 "asm_l.gperf"
      {"ldfp8.a.nt1_FFM", EM_LDFP8_A_NT1_F1_F2_R3},
#line 543 "asm_l.gperf"
      {"ldfp8.s_FFMI", EM_LDFP8_S_F1_F2_R3_16},
#line 537 "asm_l.gperf"
      {"ldfp8.sa_FFMI", EM_LDFP8_SA_F1_F2_R3_16},
      {""}, {""}, {""},
#line 547 "asm_l.gperf"
      {"ldfp8.s.nta_FFMI", EM_LDFP8_S_NTA_F1_F2_R3_16},
#line 541 "asm_l.gperf"
      {"ldfp8.sa.nta_FFMI", EM_LDFP8_SA_NTA_F1_F2_R3_16},
#line 545 "asm_l.gperf"
      {"ldfp8.s.nt1_FFMI", EM_LDFP8_S_NT1_F1_F2_R3_16},
#line 539 "asm_l.gperf"
      {"ldfp8.sa.nt1_FFMI", EM_LDFP8_SA_NT1_F1_F2_R3_16},
#line 544 "asm_l.gperf"
      {"ldfp8.s_FFM", EM_LDFP8_S_F1_F2_R3},
#line 538 "asm_l.gperf"
      {"ldfp8.sa_FFM", EM_LDFP8_SA_F1_F2_R3},
      {""}, {""}, {""},
#line 548 "asm_l.gperf"
      {"ldfp8.s.nta_FFM", EM_LDFP8_S_NTA_F1_F2_R3},
#line 542 "asm_l.gperf"
      {"ldfp8.sa.nta_FFM", EM_LDFP8_SA_NTA_F1_F2_R3},
#line 546 "asm_l.gperf"
      {"ldfp8.s.nt1_FFM", EM_LDFP8_S_NT1_F1_F2_R3},
#line 540 "asm_l.gperf"
      {"ldfp8.sa.nt1_FFM", EM_LDFP8_SA_NT1_F1_F2_R3},
#line 423 "asm_l.gperf"
      {"ldfd_FMI", EM_LDFD_F1_R3_IMM9},
      {""}, {""},
#line 396 "asm_l.gperf"
      {"ldfd.a_FMI", EM_LDFD_A_F1_R3_IMM9},
      {""},
#line 429 "asm_l.gperf"
      {"ldfd.nta_FMI", EM_LDFD_NTA_F1_R3_IMM9},
      {""},
#line 426 "asm_l.gperf"
      {"ldfd.nt1_FMI", EM_LDFD_NT1_F1_R3_IMM9},
#line 402 "asm_l.gperf"
      {"ldfd.a.nta_FMI", EM_LDFD_A_NTA_F1_R3_IMM9},
#line 424 "asm_l.gperf"
      {"ldfd_FM", EM_LDFD_F1_R3},
#line 399 "asm_l.gperf"
      {"ldfd.a.nt1_FMI", EM_LDFD_A_NT1_F1_R3_IMM9},
      {""},
#line 397 "asm_l.gperf"
      {"ldfd.a_FM", EM_LDFD_A_F1_R3},
      {""},
#line 430 "asm_l.gperf"
      {"ldfd.nta_FM", EM_LDFD_NTA_F1_R3},
      {""},
#line 427 "asm_l.gperf"
      {"ldfd.nt1_FM", EM_LDFD_NT1_F1_R3},
#line 403 "asm_l.gperf"
      {"ldfd.a.nta_FM", EM_LDFD_A_NTA_F1_R3},
      {""},
#line 400 "asm_l.gperf"
      {"ldfd.a.nt1_FM", EM_LDFD_A_NT1_F1_R3},
#line 441 "asm_l.gperf"
      {"ldfd.s_FMI", EM_LDFD_S_F1_R3_IMM9},
#line 432 "asm_l.gperf"
      {"ldfd.sa_FMI", EM_LDFD_SA_F1_R3_IMM9},
      {""}, {""}, {""},
#line 447 "asm_l.gperf"
      {"ldfd.s.nta_FMI", EM_LDFD_S_NTA_F1_R3_IMM9},
#line 438 "asm_l.gperf"
      {"ldfd.sa.nta_FMI", EM_LDFD_SA_NTA_F1_R3_IMM9},
#line 444 "asm_l.gperf"
      {"ldfd.s.nt1_FMI", EM_LDFD_S_NT1_F1_R3_IMM9},
#line 435 "asm_l.gperf"
      {"ldfd.sa.nt1_FMI", EM_LDFD_SA_NT1_F1_R3_IMM9},
#line 442 "asm_l.gperf"
      {"ldfd.s_FM", EM_LDFD_S_F1_R3},
#line 433 "asm_l.gperf"
      {"ldfd.sa_FM", EM_LDFD_SA_F1_R3},
      {""}, {""}, {""},
#line 448 "asm_l.gperf"
      {"ldfd.s.nta_FM", EM_LDFD_S_NTA_F1_R3},
#line 439 "asm_l.gperf"
      {"ldfd.sa.nta_FM", EM_LDFD_SA_NTA_F1_R3},
#line 445 "asm_l.gperf"
      {"ldfd.s.nt1_FM", EM_LDFD_S_NT1_F1_R3},
#line 436 "asm_l.gperf"
      {"ldfd.sa.nt1_FM", EM_LDFD_SA_NT1_F1_R3},
#line 425 "asm_l.gperf"
      {"ldfd_FMR", EM_LDFD_F1_R3_R2},
      {""}, {""},
#line 398 "asm_l.gperf"
      {"ldfd.a_FMR", EM_LDFD_A_F1_R3_R2},
      {""},
#line 431 "asm_l.gperf"
      {"ldfd.nta_FMR", EM_LDFD_NTA_F1_R3_R2},
      {""},
#line 428 "asm_l.gperf"
      {"ldfd.nt1_FMR", EM_LDFD_NT1_F1_R3_R2},
#line 404 "asm_l.gperf"
      {"ldfd.a.nta_FMR", EM_LDFD_A_NTA_F1_R3_R2},
      {""},
#line 401 "asm_l.gperf"
      {"ldfd.a.nt1_FMR", EM_LDFD_A_NT1_F1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 443 "asm_l.gperf"
      {"ldfd.s_FMR", EM_LDFD_S_F1_R3_R2},
#line 434 "asm_l.gperf"
      {"ldfd.sa_FMR", EM_LDFD_SA_F1_R3_R2},
      {""}, {""}, {""},
#line 449 "asm_l.gperf"
      {"ldfd.s.nta_FMR", EM_LDFD_S_NTA_F1_R3_R2},
#line 440 "asm_l.gperf"
      {"ldfd.sa.nta_FMR", EM_LDFD_SA_NTA_F1_R3_R2},
#line 446 "asm_l.gperf"
      {"ldfd.s.nt1_FMR", EM_LDFD_S_NT1_F1_R3_R2},
#line 437 "asm_l.gperf"
      {"ldfd.sa.nt1_FMR", EM_LDFD_SA_NT1_F1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 60 "asm_l.gperf"
      {"ld1.c.nc_RMI", EM_LD1_C_NC_R1_R3_IMM9},
      {""}, {""}, {""}, {""},
#line 57 "asm_l.gperf"
      {"ld1.c.nc.nta_RMI", EM_LD1_C_NC_NTA_R1_R3_IMM9},
      {""},
#line 54 "asm_l.gperf"
      {"ld1.c.nc.nt1_RMI", EM_LD1_C_NC_NT1_R1_R3_IMM9},
#line 51 "asm_l.gperf"
      {"ld1.c.clr_RMI", EM_LD1_C_CLR_R1_R3_IMM9},
#line 61 "asm_l.gperf"
      {"ld1.c.nc_RM", EM_LD1_C_NC_R1_R3},
      {""}, {""}, {""},
#line 48 "asm_l.gperf"
      {"ld1.c.clr.nta_RMI", EM_LD1_C_CLR_NTA_R1_R3_IMM9},
#line 58 "asm_l.gperf"
      {"ld1.c.nc.nta_RM", EM_LD1_C_NC_NTA_R1_R3},
#line 45 "asm_l.gperf"
      {"ld1.c.clr.nt1_RMI", EM_LD1_C_CLR_NT1_R1_R3_IMM9},
#line 55 "asm_l.gperf"
      {"ld1.c.nc.nt1_RM", EM_LD1_C_NC_NT1_R1_R3},
#line 52 "asm_l.gperf"
      {"ld1.c.clr_RM", EM_LD1_C_CLR_R1_R3},
      {""}, {""}, {""}, {""},
#line 49 "asm_l.gperf"
      {"ld1.c.clr.nta_RM", EM_LD1_C_CLR_NTA_R1_R3},
      {""},
#line 46 "asm_l.gperf"
      {"ld1.c.clr.nt1_RM", EM_LD1_C_CLR_NT1_R1_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""},
#line 62 "asm_l.gperf"
      {"ld1.c.nc_RMR", EM_LD1_C_NC_R1_R3_R2},
      {""}, {""}, {""}, {""},
#line 59 "asm_l.gperf"
      {"ld1.c.nc.nta_RMR", EM_LD1_C_NC_NTA_R1_R3_R2},
      {""},
#line 56 "asm_l.gperf"
      {"ld1.c.nc.nt1_RMR", EM_LD1_C_NC_NT1_R1_R3_R2},
#line 53 "asm_l.gperf"
      {"ld1.c.clr_RMR", EM_LD1_C_CLR_R1_R3_R2},
      {""}, {""},
#line 567 "asm_l.gperf"
      {"ldfpd_FFMI", EM_LDFPD_F1_F2_R3_16},
      {""},
#line 50 "asm_l.gperf"
      {"ld1.c.clr.nta_RMR", EM_LD1_C_CLR_NTA_R1_R3_R2},
#line 549 "asm_l.gperf"
      {"ldfpd.a_FFMI", EM_LDFPD_A_F1_F2_R3_16},
#line 47 "asm_l.gperf"
      {"ld1.c.clr.nt1_RMR", EM_LD1_C_CLR_NT1_R1_R3_R2},
#line 571 "asm_l.gperf"
      {"ldfpd.nta_FFMI", EM_LDFPD_NTA_F1_F2_R3_16},
      {""},
#line 569 "asm_l.gperf"
      {"ldfpd.nt1_FFMI", EM_LDFPD_NT1_F1_F2_R3_16},
#line 553 "asm_l.gperf"
      {"ldfpd.a.nta_FFMI", EM_LDFPD_A_NTA_F1_F2_R3_16},
#line 568 "asm_l.gperf"
      {"ldfpd_FFM", EM_LDFPD_F1_F2_R3},
#line 551 "asm_l.gperf"
      {"ldfpd.a.nt1_FFMI", EM_LDFPD_A_NT1_F1_F2_R3_16},
      {""},
#line 550 "asm_l.gperf"
      {"ldfpd.a_FFM", EM_LDFPD_A_F1_F2_R3},
      {""},
#line 572 "asm_l.gperf"
      {"ldfpd.nta_FFM", EM_LDFPD_NTA_F1_F2_R3},
      {""},
#line 570 "asm_l.gperf"
      {"ldfpd.nt1_FFM", EM_LDFPD_NT1_F1_F2_R3},
#line 554 "asm_l.gperf"
      {"ldfpd.a.nta_FFM", EM_LDFPD_A_NTA_F1_F2_R3},
      {""},
#line 552 "asm_l.gperf"
      {"ldfpd.a.nt1_FFM", EM_LDFPD_A_NT1_F1_F2_R3},
#line 579 "asm_l.gperf"
      {"ldfpd.s_FFMI", EM_LDFPD_S_F1_F2_R3_16},
#line 573 "asm_l.gperf"
      {"ldfpd.sa_FFMI", EM_LDFPD_SA_F1_F2_R3_16},
      {""}, {""}, {""},
#line 583 "asm_l.gperf"
      {"ldfpd.s.nta_FFMI", EM_LDFPD_S_NTA_F1_F2_R3_16},
#line 577 "asm_l.gperf"
      {"ldfpd.sa.nta_FFMI", EM_LDFPD_SA_NTA_F1_F2_R3_16},
#line 581 "asm_l.gperf"
      {"ldfpd.s.nt1_FFMI", EM_LDFPD_S_NT1_F1_F2_R3_16},
#line 575 "asm_l.gperf"
      {"ldfpd.sa.nt1_FFMI", EM_LDFPD_SA_NT1_F1_F2_R3_16},
#line 580 "asm_l.gperf"
      {"ldfpd.s_FFM", EM_LDFPD_S_F1_F2_R3},
#line 574 "asm_l.gperf"
      {"ldfpd.sa_FFM", EM_LDFPD_SA_F1_F2_R3},
      {""}, {""},
#line 303 "asm_l.gperf"
      {"ld8.c.nc_RMI", EM_LD8_C_NC_R1_R3_IMM9},
#line 584 "asm_l.gperf"
      {"ldfpd.s.nta_FFM", EM_LDFPD_S_NTA_F1_F2_R3},
#line 578 "asm_l.gperf"
      {"ldfpd.sa.nta_FFM", EM_LDFPD_SA_NTA_F1_F2_R3},
#line 582 "asm_l.gperf"
      {"ldfpd.s.nt1_FFM", EM_LDFPD_S_NT1_F1_F2_R3},
#line 576 "asm_l.gperf"
      {"ldfpd.sa.nt1_FFM", EM_LDFPD_SA_NT1_F1_F2_R3},
#line 300 "asm_l.gperf"
      {"ld8.c.nc.nta_RMI", EM_LD8_C_NC_NTA_R1_R3_IMM9},
      {""},
#line 297 "asm_l.gperf"
      {"ld8.c.nc.nt1_RMI", EM_LD8_C_NC_NT1_R1_R3_IMM9},
#line 294 "asm_l.gperf"
      {"ld8.c.clr_RMI", EM_LD8_C_CLR_R1_R3_IMM9},
#line 304 "asm_l.gperf"
      {"ld8.c.nc_RM", EM_LD8_C_NC_R1_R3},
      {""}, {""}, {""},
#line 291 "asm_l.gperf"
      {"ld8.c.clr.nta_RMI", EM_LD8_C_CLR_NTA_R1_R3_IMM9},
#line 301 "asm_l.gperf"
      {"ld8.c.nc.nta_RM", EM_LD8_C_NC_NTA_R1_R3},
#line 288 "asm_l.gperf"
      {"ld8.c.clr.nt1_RMI", EM_LD8_C_CLR_NT1_R1_R3_IMM9},
#line 298 "asm_l.gperf"
      {"ld8.c.nc.nt1_RM", EM_LD8_C_NC_NT1_R1_R3},
#line 295 "asm_l.gperf"
      {"ld8.c.clr_RM", EM_LD8_C_CLR_R1_R3},
      {""}, {""}, {""}, {""},
#line 292 "asm_l.gperf"
      {"ld8.c.clr.nta_RM", EM_LD8_C_CLR_NTA_R1_R3},
      {""},
#line 289 "asm_l.gperf"
      {"ld8.c.clr.nt1_RM", EM_LD8_C_CLR_NT1_R1_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""},
#line 305 "asm_l.gperf"
      {"ld8.c.nc_RMR", EM_LD8_C_NC_R1_R3_R2},
      {""}, {""}, {""}, {""},
#line 302 "asm_l.gperf"
      {"ld8.c.nc.nta_RMR", EM_LD8_C_NC_NTA_R1_R3_R2},
      {""},
#line 299 "asm_l.gperf"
      {"ld8.c.nc.nt1_RMR", EM_LD8_C_NC_NT1_R1_R3_R2},
#line 296 "asm_l.gperf"
      {"ld8.c.clr_RMR", EM_LD8_C_CLR_R1_R3_R2},
      {""}, {""}, {""}, {""},
#line 293 "asm_l.gperf"
      {"ld8.c.clr.nta_RMR", EM_LD8_C_CLR_NTA_R1_R3_R2},
      {""},
#line 290 "asm_l.gperf"
      {"ld8.c.clr.nt1_RMR", EM_LD8_C_CLR_NT1_R1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""},
#line 639 "asm_l.gperf"
      {"ldfs.c.nc_FMI", EM_LDFS_C_NC_F1_R3_IMM9},
      {""}, {""}, {""}, {""},
#line 645 "asm_l.gperf"
      {"ldfs.c.nc.nta_FMI", EM_LDFS_C_NC_NTA_F1_R3_IMM9},
      {""},
#line 642 "asm_l.gperf"
      {"ldfs.c.nc.nt1_FMI", EM_LDFS_C_NC_NT1_F1_R3_IMM9},
#line 630 "asm_l.gperf"
      {"ldfs.c.clr_FMI", EM_LDFS_C_CLR_F1_R3_IMM9},
#line 640 "asm_l.gperf"
      {"ldfs.c.nc_FM", EM_LDFS_C_NC_F1_R3},
      {""}, {""}, {""},
#line 636 "asm_l.gperf"
      {"ldfs.c.clr.nta_FMI", EM_LDFS_C_CLR_NTA_F1_R3_IMM9},
#line 646 "asm_l.gperf"
      {"ldfs.c.nc.nta_FM", EM_LDFS_C_NC_NTA_F1_R3},
#line 633 "asm_l.gperf"
      {"ldfs.c.clr.nt1_FMI", EM_LDFS_C_CLR_NT1_F1_R3_IMM9},
#line 643 "asm_l.gperf"
      {"ldfs.c.nc.nt1_FM", EM_LDFS_C_NC_NT1_F1_R3},
#line 631 "asm_l.gperf"
      {"ldfs.c.clr_FM", EM_LDFS_C_CLR_F1_R3},
      {""}, {""}, {""}, {""},
#line 637 "asm_l.gperf"
      {"ldfs.c.clr.nta_FM", EM_LDFS_C_CLR_NTA_F1_R3},
      {""},
#line 634 "asm_l.gperf"
      {"ldfs.c.clr.nt1_FM", EM_LDFS_C_CLR_NT1_F1_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 638 "asm_l.gperf"
      {"ldfs.c.clr.nta_FMR", EM_LDFS_C_CLR_NTA_F1_R3_R2},
      {""},
#line 635 "asm_l.gperf"
      {"ldfs.c.clr.nt1_FMR", EM_LDFS_C_CLR_NT1_F1_R3_R2},
      {""}, {""}, {""},
#line 641 "asm_l.gperf"
      {"ldfs.c.nc_FMR", EM_LDFS_C_NC_F1_R3_R2},
      {""}, {""}, {""}, {""},
#line 647 "asm_l.gperf"
      {"ldfs.c.nc.nta_FMR", EM_LDFS_C_NC_NTA_F1_R3_R2},
      {""},
#line 644 "asm_l.gperf"
      {"ldfs.c.nc.nt1_FMR", EM_LDFS_C_NC_NT1_F1_R3_R2},
#line 632 "asm_l.gperf"
      {"ldfs.c.clr_FMR", EM_LDFS_C_CLR_F1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""},
#line 360 "asm_l.gperf"
      {"ldf8.c.nc_FMI", EM_LDF8_C_NC_F1_R3_IMM9},
      {""}, {""}, {""}, {""},
#line 366 "asm_l.gperf"
      {"ldf8.c.nc.nta_FMI", EM_LDF8_C_NC_NTA_F1_R3_IMM9},
      {""},
#line 363 "asm_l.gperf"
      {"ldf8.c.nc.nt1_FMI", EM_LDF8_C_NC_NT1_F1_R3_IMM9},
#line 351 "asm_l.gperf"
      {"ldf8.c.clr_FMI", EM_LDF8_C_CLR_F1_R3_IMM9},
#line 361 "asm_l.gperf"
      {"ldf8.c.nc_FM", EM_LDF8_C_NC_F1_R3},
      {""}, {""}, {""},
#line 357 "asm_l.gperf"
      {"ldf8.c.clr.nta_FMI", EM_LDF8_C_CLR_NTA_F1_R3_IMM9},
#line 367 "asm_l.gperf"
      {"ldf8.c.nc.nta_FM", EM_LDF8_C_NC_NTA_F1_R3},
#line 354 "asm_l.gperf"
      {"ldf8.c.clr.nt1_FMI", EM_LDF8_C_CLR_NT1_F1_R3_IMM9},
#line 364 "asm_l.gperf"
      {"ldf8.c.nc.nt1_FM", EM_LDF8_C_NC_NT1_F1_R3},
#line 352 "asm_l.gperf"
      {"ldf8.c.clr_FM", EM_LDF8_C_CLR_F1_R3},
      {""}, {""}, {""}, {""},
#line 358 "asm_l.gperf"
      {"ldf8.c.clr.nta_FM", EM_LDF8_C_CLR_NTA_F1_R3},
      {""},
#line 355 "asm_l.gperf"
      {"ldf8.c.clr.nt1_FM", EM_LDF8_C_CLR_NT1_F1_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 359 "asm_l.gperf"
      {"ldf8.c.clr.nta_FMR", EM_LDF8_C_CLR_NTA_F1_R3_R2},
      {""},
#line 356 "asm_l.gperf"
      {"ldf8.c.clr.nt1_FMR", EM_LDF8_C_CLR_NT1_F1_R3_R2},
      {""}, {""}, {""},
#line 362 "asm_l.gperf"
      {"ldf8.c.nc_FMR", EM_LDF8_C_NC_F1_R3_R2},
      {""}, {""}, {""}, {""},
#line 368 "asm_l.gperf"
      {"ldf8.c.nc.nta_FMR", EM_LDF8_C_NC_NTA_F1_R3_R2},
      {""},
#line 365 "asm_l.gperf"
      {"ldf8.c.nc.nt1_FMR", EM_LDF8_C_NC_NT1_F1_R3_R2},
#line 353 "asm_l.gperf"
      {"ldf8.c.clr_FMR", EM_LDF8_C_CLR_F1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""},
#line 601 "asm_l.gperf"
      {"ldfps.c.nc.nta_FFMI", EM_LDFPS_C_NC_NTA_F1_F2_R3_8},
      {""},
#line 599 "asm_l.gperf"
      {"ldfps.c.nc.nt1_FFMI", EM_LDFPS_C_NC_NT1_F1_F2_R3_8},
      {""}, {""},
#line 597 "asm_l.gperf"
      {"ldfps.c.nc_FFMI", EM_LDFPS_C_NC_F1_F2_R3_8},
      {""}, {""},
#line 595 "asm_l.gperf"
      {"ldfps.c.clr.nta_FFMI", EM_LDFPS_C_CLR_NTA_F1_F2_R3_8},
#line 602 "asm_l.gperf"
      {"ldfps.c.nc.nta_FFM", EM_LDFPS_C_NC_NTA_F1_F2_R3},
#line 593 "asm_l.gperf"
      {"ldfps.c.clr.nt1_FFMI", EM_LDFPS_C_CLR_NT1_F1_F2_R3_8},
#line 600 "asm_l.gperf"
      {"ldfps.c.nc.nt1_FFM", EM_LDFPS_C_NC_NT1_F1_F2_R3},
      {""},
#line 591 "asm_l.gperf"
      {"ldfps.c.clr_FFMI", EM_LDFPS_C_CLR_F1_F2_R3_8},
#line 598 "asm_l.gperf"
      {"ldfps.c.nc_FFM", EM_LDFPS_C_NC_F1_F2_R3},
      {""},
#line 15 "asm_l.gperf"
      {"ld1.acq_RMI", EM_LD1_ACQ_R1_R3_IMM9},
#line 596 "asm_l.gperf"
      {"ldfps.c.clr.nta_FFM", EM_LDFPS_C_CLR_NTA_F1_F2_R3},
      {""},
#line 594 "asm_l.gperf"
      {"ldfps.c.clr.nt1_FFM", EM_LDFPS_C_CLR_NT1_F1_F2_R3},
      {""},
#line 12 "asm_l.gperf"
      {"ld1.acq.nta_RMI", EM_LD1_ACQ_NTA_R1_R3_IMM9},
#line 592 "asm_l.gperf"
      {"ldfps.c.clr_FFM", EM_LDFPS_C_CLR_F1_F2_R3},
#line 9 "asm_l.gperf"
      {"ld1.acq.nt1_RMI", EM_LD1_ACQ_NT1_R1_R3_IMM9},
      {""},
#line 16 "asm_l.gperf"
      {"ld1.acq_RM", EM_LD1_ACQ_R1_R3},
      {""}, {""}, {""}, {""},
#line 13 "asm_l.gperf"
      {"ld1.acq.nta_RM", EM_LD1_ACQ_NTA_R1_R3},
      {""},
#line 10 "asm_l.gperf"
      {"ld1.acq.nt1_RM", EM_LD1_ACQ_NT1_R1_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""},
#line 17 "asm_l.gperf"
      {"ld1.acq_RMR", EM_LD1_ACQ_R1_R3_R2},
      {""}, {""}, {""},
#line 5 "asm_l.gperf"
      {"ld16.acq_RAM", EM_LD16_ACQ_R1_AR_CSD_R3},
#line 14 "asm_l.gperf"
      {"ld1.acq.nta_RMR", EM_LD1_ACQ_NTA_R1_R3_R2},
      {""},
#line 11 "asm_l.gperf"
      {"ld1.acq.nt1_RMR", EM_LD1_ACQ_NT1_R1_R3_R2},
      {""},
#line 4 "asm_l.gperf"
      {"ld16.acq.nta_RAM", EM_LD16_ACQ_NTA_R1_AR_CSD_R3},
      {""},
#line 3 "asm_l.gperf"
      {"ld16.acq.nt1_RAM", EM_LD16_ACQ_NT1_R1_AR_CSD_R3},
      {""},
#line 529 "asm_l.gperf"
      {"ldfp8.c.nc.nta_FFMI", EM_LDFP8_C_NC_NTA_F1_F2_R3_16},
      {""},
#line 527 "asm_l.gperf"
      {"ldfp8.c.nc.nt1_FFMI", EM_LDFP8_C_NC_NT1_F1_F2_R3_16},
      {""}, {""},
#line 525 "asm_l.gperf"
      {"ldfp8.c.nc_FFMI", EM_LDFP8_C_NC_F1_F2_R3_16},
      {""}, {""},
#line 523 "asm_l.gperf"
      {"ldfp8.c.clr.nta_FFMI", EM_LDFP8_C_CLR_NTA_F1_F2_R3_16},
#line 530 "asm_l.gperf"
      {"ldfp8.c.nc.nta_FFM", EM_LDFP8_C_NC_NTA_F1_F2_R3},
#line 521 "asm_l.gperf"
      {"ldfp8.c.clr.nt1_FFMI", EM_LDFP8_C_CLR_NT1_F1_F2_R3_16},
#line 528 "asm_l.gperf"
      {"ldfp8.c.nc.nt1_FFM", EM_LDFP8_C_NC_NT1_F1_F2_R3},
      {""},
#line 519 "asm_l.gperf"
      {"ldfp8.c.clr_FFMI", EM_LDFP8_C_CLR_F1_F2_R3_16},
#line 526 "asm_l.gperf"
      {"ldfp8.c.nc_FFM", EM_LDFP8_C_NC_F1_F2_R3},
      {""}, {""},
#line 524 "asm_l.gperf"
      {"ldfp8.c.clr.nta_FFM", EM_LDFP8_C_CLR_NTA_F1_F2_R3},
      {""},
#line 522 "asm_l.gperf"
      {"ldfp8.c.clr.nt1_FFM", EM_LDFP8_C_CLR_NT1_F1_F2_R3},
      {""}, {""},
#line 520 "asm_l.gperf"
      {"ldfp8.c.clr_FFM", EM_LDFP8_C_CLR_F1_F2_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 258 "asm_l.gperf"
      {"ld8.acq_RMI", EM_LD8_ACQ_R1_R3_IMM9},
      {""}, {""}, {""}, {""},
#line 255 "asm_l.gperf"
      {"ld8.acq.nta_RMI", EM_LD8_ACQ_NTA_R1_R3_IMM9},
      {""},
#line 252 "asm_l.gperf"
      {"ld8.acq.nt1_RMI", EM_LD8_ACQ_NT1_R1_R3_IMM9},
      {""},
#line 259 "asm_l.gperf"
      {"ld8.acq_RM", EM_LD8_ACQ_R1_R3},
      {""}, {""},
#line 414 "asm_l.gperf"
      {"ldfd.c.nc_FMI", EM_LDFD_C_NC_F1_R3_IMM9},
      {""},
#line 256 "asm_l.gperf"
      {"ld8.acq.nta_RM", EM_LD8_ACQ_NTA_R1_R3},
      {""},
#line 253 "asm_l.gperf"
      {"ld8.acq.nt1_RM", EM_LD8_ACQ_NT1_R1_R3},
#line 420 "asm_l.gperf"
      {"ldfd.c.nc.nta_FMI", EM_LDFD_C_NC_NTA_F1_R3_IMM9},
      {""},
#line 417 "asm_l.gperf"
      {"ldfd.c.nc.nt1_FMI", EM_LDFD_C_NC_NT1_F1_R3_IMM9},
#line 405 "asm_l.gperf"
      {"ldfd.c.clr_FMI", EM_LDFD_C_CLR_F1_R3_IMM9},
#line 415 "asm_l.gperf"
      {"ldfd.c.nc_FM", EM_LDFD_C_NC_F1_R3},
      {""}, {""}, {""},
#line 411 "asm_l.gperf"
      {"ldfd.c.clr.nta_FMI", EM_LDFD_C_CLR_NTA_F1_R3_IMM9},
#line 421 "asm_l.gperf"
      {"ldfd.c.nc.nta_FM", EM_LDFD_C_NC_NTA_F1_R3},
#line 408 "asm_l.gperf"
      {"ldfd.c.clr.nt1_FMI", EM_LDFD_C_CLR_NT1_F1_R3_IMM9},
#line 418 "asm_l.gperf"
      {"ldfd.c.nc.nt1_FM", EM_LDFD_C_NC_NT1_F1_R3},
#line 406 "asm_l.gperf"
      {"ldfd.c.clr_FM", EM_LDFD_C_CLR_F1_R3},
      {""}, {""}, {""}, {""},
#line 412 "asm_l.gperf"
      {"ldfd.c.clr.nta_FM", EM_LDFD_C_CLR_NTA_F1_R3},
      {""},
#line 409 "asm_l.gperf"
      {"ldfd.c.clr.nt1_FM", EM_LDFD_C_CLR_NT1_F1_R3},
      {""},
#line 260 "asm_l.gperf"
      {"ld8.acq_RMR", EM_LD8_ACQ_R1_R3_R2},
      {""}, {""}, {""}, {""},
#line 257 "asm_l.gperf"
      {"ld8.acq.nta_RMR", EM_LD8_ACQ_NTA_R1_R3_R2},
#line 413 "asm_l.gperf"
      {"ldfd.c.clr.nta_FMR", EM_LDFD_C_CLR_NTA_F1_R3_R2},
#line 254 "asm_l.gperf"
      {"ld8.acq.nt1_RMR", EM_LD8_ACQ_NT1_R1_R3_R2},
#line 410 "asm_l.gperf"
      {"ldfd.c.clr.nt1_FMR", EM_LDFD_C_CLR_NT1_F1_R3_R2},
      {""}, {""}, {""},
#line 416 "asm_l.gperf"
      {"ldfd.c.nc_FMR", EM_LDFD_C_NC_F1_R3_R2},
      {""}, {""}, {""}, {""},
#line 422 "asm_l.gperf"
      {"ldfd.c.nc.nta_FMR", EM_LDFD_C_NC_NTA_F1_R3_R2},
#line 150 "asm_l.gperf"
      {"ld2_RMI", EM_LD2_R1_R3_IMM9},
#line 419 "asm_l.gperf"
      {"ldfd.c.nc.nt1_FMR", EM_LDFD_C_NC_NT1_F1_R3_R2},
#line 407 "asm_l.gperf"
      {"ldfd.c.clr_FMR", EM_LDFD_C_CLR_F1_R3_R2},
#line 105 "asm_l.gperf"
      {"ld2.a_RMI", EM_LD2_A_R1_R3_IMM9},
      {""},
#line 147 "asm_l.gperf"
      {"ld2.nta_RMI", EM_LD2_NTA_R1_R3_IMM9},
      {""},
#line 144 "asm_l.gperf"
      {"ld2.nt1_RMI", EM_LD2_NT1_R1_R3_IMM9},
#line 102 "asm_l.gperf"
      {"ld2.a.nta_RMI", EM_LD2_A_NTA_R1_R3_IMM9},
#line 151 "asm_l.gperf"
      {"ld2_RM", EM_LD2_R1_R3},
#line 99 "asm_l.gperf"
      {"ld2.a.nt1_RMI", EM_LD2_A_NT1_R1_R3_IMM9},
      {""},
#line 106 "asm_l.gperf"
      {"ld2.a_RM", EM_LD2_A_R1_R3},
      {""},
#line 148 "asm_l.gperf"
      {"ld2.nta_RM", EM_LD2_NTA_R1_R3},
      {""},
#line 145 "asm_l.gperf"
      {"ld2.nt1_RM", EM_LD2_NT1_R1_R3},
#line 103 "asm_l.gperf"
      {"ld2.a.nta_RM", EM_LD2_A_NTA_R1_R3},
      {""},
#line 100 "asm_l.gperf"
      {"ld2.a.nt1_RM", EM_LD2_A_NT1_R1_R3},
#line 168 "asm_l.gperf"
      {"ld2.s_RMI", EM_LD2_S_R1_R3_IMM9},
#line 159 "asm_l.gperf"
      {"ld2.sa_RMI", EM_LD2_SA_R1_R3_IMM9},
      {""}, {""}, {""},
#line 165 "asm_l.gperf"
      {"ld2.s.nta_RMI", EM_LD2_S_NTA_R1_R3_IMM9},
#line 156 "asm_l.gperf"
      {"ld2.sa.nta_RMI", EM_LD2_SA_NTA_R1_R3_IMM9},
#line 162 "asm_l.gperf"
      {"ld2.s.nt1_RMI", EM_LD2_S_NT1_R1_R3_IMM9},
#line 153 "asm_l.gperf"
      {"ld2.sa.nt1_RMI", EM_LD2_SA_NT1_R1_R3_IMM9},
#line 169 "asm_l.gperf"
      {"ld2.s_RM", EM_LD2_S_R1_R3},
#line 160 "asm_l.gperf"
      {"ld2.sa_RM", EM_LD2_SA_R1_R3},
      {""}, {""}, {""},
#line 166 "asm_l.gperf"
      {"ld2.s.nta_RM", EM_LD2_S_NTA_R1_R3},
#line 157 "asm_l.gperf"
      {"ld2.sa.nta_RM", EM_LD2_SA_NTA_R1_R3},
#line 163 "asm_l.gperf"
      {"ld2.s.nt1_RM", EM_LD2_S_NT1_R1_R3},
#line 154 "asm_l.gperf"
      {"ld2.sa.nt1_RM", EM_LD2_SA_NT1_R1_R3},
#line 152 "asm_l.gperf"
      {"ld2_RMR", EM_LD2_R1_R3_R2},
      {""}, {""},
#line 107 "asm_l.gperf"
      {"ld2.a_RMR", EM_LD2_A_R1_R3_R2},
      {""},
#line 149 "asm_l.gperf"
      {"ld2.nta_RMR", EM_LD2_NTA_R1_R3_R2},
      {""},
#line 146 "asm_l.gperf"
      {"ld2.nt1_RMR", EM_LD2_NT1_R1_R3_R2},
#line 104 "asm_l.gperf"
      {"ld2.a.nta_RMR", EM_LD2_A_NTA_R1_R3_R2},
      {""},
#line 101 "asm_l.gperf"
      {"ld2.a.nt1_RMR", EM_LD2_A_NT1_R1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 170 "asm_l.gperf"
      {"ld2.s_RMR", EM_LD2_S_R1_R3_R2},
#line 161 "asm_l.gperf"
      {"ld2.sa_RMR", EM_LD2_SA_R1_R3_R2},
      {""}, {""}, {""},
#line 167 "asm_l.gperf"
      {"ld2.s.nta_RMR", EM_LD2_S_NTA_R1_R3_R2},
#line 158 "asm_l.gperf"
      {"ld2.sa.nta_RMR", EM_LD2_SA_NTA_R1_R3_R2},
#line 164 "asm_l.gperf"
      {"ld2.s.nt1_RMR", EM_LD2_S_NT1_R1_R3_R2},
#line 155 "asm_l.gperf"
      {"ld2.sa.nt1_RMR", EM_LD2_SA_NT1_R1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 565 "asm_l.gperf"
      {"ldfpd.c.nc.nta_FFMI", EM_LDFPD_C_NC_NTA_F1_F2_R3_16},
      {""},
#line 563 "asm_l.gperf"
      {"ldfpd.c.nc.nt1_FFMI", EM_LDFPD_C_NC_NT1_F1_F2_R3_16},
      {""}, {""},
#line 561 "asm_l.gperf"
      {"ldfpd.c.nc_FFMI", EM_LDFPD_C_NC_F1_F2_R3_16},
      {""}, {""},
#line 559 "asm_l.gperf"
      {"ldfpd.c.clr.nta_FFMI", EM_LDFPD_C_CLR_NTA_F1_F2_R3_16},
#line 566 "asm_l.gperf"
      {"ldfpd.c.nc.nta_FFM", EM_LDFPD_C_NC_NTA_F1_F2_R3},
#line 557 "asm_l.gperf"
      {"ldfpd.c.clr.nt1_FFMI", EM_LDFPD_C_CLR_NT1_F1_F2_R3_16},
#line 564 "asm_l.gperf"
      {"ldfpd.c.nc.nt1_FFM", EM_LDFPD_C_NC_NT1_F1_F2_R3},
      {""},
#line 555 "asm_l.gperf"
      {"ldfpd.c.clr_FFMI", EM_LDFPD_C_CLR_F1_F2_R3_16},
#line 562 "asm_l.gperf"
      {"ldfpd.c.nc_FFM", EM_LDFPD_C_NC_F1_F2_R3},
      {""}, {""},
#line 560 "asm_l.gperf"
      {"ldfpd.c.clr.nta_FFM", EM_LDFPD_C_CLR_NTA_F1_F2_R3},
#line 231 "asm_l.gperf"
      {"ld4_RMI", EM_LD4_R1_R3_IMM9},
#line 558 "asm_l.gperf"
      {"ldfpd.c.clr.nt1_FFM", EM_LDFPD_C_CLR_NT1_F1_F2_R3},
      {""},
#line 186 "asm_l.gperf"
      {"ld4.a_RMI", EM_LD4_A_R1_R3_IMM9},
#line 556 "asm_l.gperf"
      {"ldfpd.c.clr_FFM", EM_LDFPD_C_CLR_F1_F2_R3},
#line 228 "asm_l.gperf"
      {"ld4.nta_RMI", EM_LD4_NTA_R1_R3_IMM9},
      {""},
#line 225 "asm_l.gperf"
      {"ld4.nt1_RMI", EM_LD4_NT1_R1_R3_IMM9},
#line 183 "asm_l.gperf"
      {"ld4.a.nta_RMI", EM_LD4_A_NTA_R1_R3_IMM9},
#line 232 "asm_l.gperf"
      {"ld4_RM", EM_LD4_R1_R3},
#line 180 "asm_l.gperf"
      {"ld4.a.nt1_RMI", EM_LD4_A_NT1_R1_R3_IMM9},
      {""},
#line 187 "asm_l.gperf"
      {"ld4.a_RM", EM_LD4_A_R1_R3},
      {""},
#line 229 "asm_l.gperf"
      {"ld4.nta_RM", EM_LD4_NTA_R1_R3},
      {""},
#line 226 "asm_l.gperf"
      {"ld4.nt1_RM", EM_LD4_NT1_R1_R3},
#line 184 "asm_l.gperf"
      {"ld4.a.nta_RM", EM_LD4_A_NTA_R1_R3},
      {""},
#line 181 "asm_l.gperf"
      {"ld4.a.nt1_RM", EM_LD4_A_NT1_R1_R3},
#line 249 "asm_l.gperf"
      {"ld4.s_RMI", EM_LD4_S_R1_R3_IMM9},
#line 240 "asm_l.gperf"
      {"ld4.sa_RMI", EM_LD4_SA_R1_R3_IMM9},
      {""}, {""}, {""},
#line 246 "asm_l.gperf"
      {"ld4.s.nta_RMI", EM_LD4_S_NTA_R1_R3_IMM9},
#line 237 "asm_l.gperf"
      {"ld4.sa.nta_RMI", EM_LD4_SA_NTA_R1_R3_IMM9},
#line 243 "asm_l.gperf"
      {"ld4.s.nt1_RMI", EM_LD4_S_NT1_R1_R3_IMM9},
#line 234 "asm_l.gperf"
      {"ld4.sa.nt1_RMI", EM_LD4_SA_NT1_R1_R3_IMM9},
#line 250 "asm_l.gperf"
      {"ld4.s_RM", EM_LD4_S_R1_R3},
#line 241 "asm_l.gperf"
      {"ld4.sa_RM", EM_LD4_SA_R1_R3},
      {""}, {""}, {""},
#line 247 "asm_l.gperf"
      {"ld4.s.nta_RM", EM_LD4_S_NTA_R1_R3},
#line 238 "asm_l.gperf"
      {"ld4.sa.nta_RM", EM_LD4_SA_NTA_R1_R3},
#line 244 "asm_l.gperf"
      {"ld4.s.nt1_RM", EM_LD4_S_NT1_R1_R3},
#line 235 "asm_l.gperf"
      {"ld4.sa.nt1_RM", EM_LD4_SA_NT1_R1_R3},
#line 233 "asm_l.gperf"
      {"ld4_RMR", EM_LD4_R1_R3_R2},
      {""}, {""},
#line 188 "asm_l.gperf"
      {"ld4.a_RMR", EM_LD4_A_R1_R3_R2},
      {""},
#line 230 "asm_l.gperf"
      {"ld4.nta_RMR", EM_LD4_NTA_R1_R3_R2},
      {""},
#line 227 "asm_l.gperf"
      {"ld4.nt1_RMR", EM_LD4_NT1_R1_R3_R2},
#line 185 "asm_l.gperf"
      {"ld4.a.nta_RMR", EM_LD4_A_NTA_R1_R3_R2},
      {""},
#line 182 "asm_l.gperf"
      {"ld4.a.nt1_RMR", EM_LD4_A_NT1_R1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 251 "asm_l.gperf"
      {"ld4.s_RMR", EM_LD4_S_R1_R3_R2},
#line 242 "asm_l.gperf"
      {"ld4.sa_RMR", EM_LD4_SA_R1_R3_R2},
      {""}, {""}, {""},
#line 248 "asm_l.gperf"
      {"ld4.s.nta_RMR", EM_LD4_S_NTA_R1_R3_R2},
#line 239 "asm_l.gperf"
      {"ld4.sa.nta_RMR", EM_LD4_SA_NTA_R1_R3_R2},
#line 245 "asm_l.gperf"
      {"ld4.s.nt1_RMR", EM_LD4_S_NT1_R1_R3_R2},
#line 236 "asm_l.gperf"
      {"ld4.sa.nt1_RMR", EM_LD4_SA_NT1_R1_R3_R2},
#line 477 "asm_l.gperf"
      {"ldfe_FMI", EM_LDFE_F1_R3_IMM9},
      {""}, {""},
#line 450 "asm_l.gperf"
      {"ldfe.a_FMI", EM_LDFE_A_F1_R3_IMM9},
      {""},
#line 483 "asm_l.gperf"
      {"ldfe.nta_FMI", EM_LDFE_NTA_F1_R3_IMM9},
      {""},
#line 480 "asm_l.gperf"
      {"ldfe.nt1_FMI", EM_LDFE_NT1_F1_R3_IMM9},
#line 456 "asm_l.gperf"
      {"ldfe.a.nta_FMI", EM_LDFE_A_NTA_F1_R3_IMM9},
#line 478 "asm_l.gperf"
      {"ldfe_FM", EM_LDFE_F1_R3},
#line 453 "asm_l.gperf"
      {"ldfe.a.nt1_FMI", EM_LDFE_A_NT1_F1_R3_IMM9},
      {""},
#line 451 "asm_l.gperf"
      {"ldfe.a_FM", EM_LDFE_A_F1_R3},
      {""},
#line 484 "asm_l.gperf"
      {"ldfe.nta_FM", EM_LDFE_NTA_F1_R3},
      {""},
#line 481 "asm_l.gperf"
      {"ldfe.nt1_FM", EM_LDFE_NT1_F1_R3},
#line 457 "asm_l.gperf"
      {"ldfe.a.nta_FM", EM_LDFE_A_NTA_F1_R3},
      {""},
#line 454 "asm_l.gperf"
      {"ldfe.a.nt1_FM", EM_LDFE_A_NT1_F1_R3},
#line 495 "asm_l.gperf"
      {"ldfe.s_FMI", EM_LDFE_S_F1_R3_IMM9},
#line 486 "asm_l.gperf"
      {"ldfe.sa_FMI", EM_LDFE_SA_F1_R3_IMM9},
      {""}, {""}, {""},
#line 501 "asm_l.gperf"
      {"ldfe.s.nta_FMI", EM_LDFE_S_NTA_F1_R3_IMM9},
#line 492 "asm_l.gperf"
      {"ldfe.sa.nta_FMI", EM_LDFE_SA_NTA_F1_R3_IMM9},
#line 498 "asm_l.gperf"
      {"ldfe.s.nt1_FMI", EM_LDFE_S_NT1_F1_R3_IMM9},
#line 489 "asm_l.gperf"
      {"ldfe.sa.nt1_FMI", EM_LDFE_SA_NT1_F1_R3_IMM9},
#line 496 "asm_l.gperf"
      {"ldfe.s_FM", EM_LDFE_S_F1_R3},
#line 487 "asm_l.gperf"
      {"ldfe.sa_FM", EM_LDFE_SA_F1_R3},
      {""}, {""}, {""},
#line 502 "asm_l.gperf"
      {"ldfe.s.nta_FM", EM_LDFE_S_NTA_F1_R3},
#line 493 "asm_l.gperf"
      {"ldfe.sa.nta_FM", EM_LDFE_SA_NTA_F1_R3},
#line 499 "asm_l.gperf"
      {"ldfe.s.nt1_FM", EM_LDFE_S_NT1_F1_R3},
#line 490 "asm_l.gperf"
      {"ldfe.sa.nt1_FM", EM_LDFE_SA_NT1_F1_R3},
#line 479 "asm_l.gperf"
      {"ldfe_FMR", EM_LDFE_F1_R3_R2},
      {""}, {""},
#line 452 "asm_l.gperf"
      {"ldfe.a_FMR", EM_LDFE_A_F1_R3_R2},
      {""},
#line 485 "asm_l.gperf"
      {"ldfe.nta_FMR", EM_LDFE_NTA_F1_R3_R2},
      {""},
#line 482 "asm_l.gperf"
      {"ldfe.nt1_FMR", EM_LDFE_NT1_F1_R3_R2},
#line 458 "asm_l.gperf"
      {"ldfe.a.nta_FMR", EM_LDFE_A_NTA_F1_R3_R2},
      {""},
#line 455 "asm_l.gperf"
      {"ldfe.a.nt1_FMR", EM_LDFE_A_NT1_F1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 497 "asm_l.gperf"
      {"ldfe.s_FMR", EM_LDFE_S_F1_R3_R2},
#line 488 "asm_l.gperf"
      {"ldfe.sa_FMR", EM_LDFE_SA_F1_R3_R2},
      {""}, {""},
#line 312 "asm_l.gperf"
      {"ld8.fill_RMI", EM_LD8_FILL_R1_R3_IMM9},
#line 503 "asm_l.gperf"
      {"ldfe.s.nta_FMR", EM_LDFE_S_NTA_F1_R3_R2},
#line 494 "asm_l.gperf"
      {"ldfe.sa.nta_FMR", EM_LDFE_SA_NTA_F1_R3_R2},
#line 500 "asm_l.gperf"
      {"ldfe.s.nt1_FMR", EM_LDFE_S_NT1_F1_R3_R2},
#line 491 "asm_l.gperf"
      {"ldfe.sa.nt1_FMR", EM_LDFE_SA_NT1_F1_R3_R2},
#line 309 "asm_l.gperf"
      {"ld8.fill.nta_RMI", EM_LD8_FILL_NTA_R1_R3_IMM9},
      {""},
#line 306 "asm_l.gperf"
      {"ld8.fill.nt1_RMI", EM_LD8_FILL_NT1_R1_R3_IMM9},
      {""},
#line 313 "asm_l.gperf"
      {"ld8.fill_RM", EM_LD8_FILL_R1_R3},
      {""}, {""}, {""}, {""},
#line 310 "asm_l.gperf"
      {"ld8.fill.nta_RM", EM_LD8_FILL_NTA_R1_R3},
      {""},
#line 307 "asm_l.gperf"
      {"ld8.fill.nt1_RM", EM_LD8_FILL_NT1_R1_R3},
      {""}, {""},
#line 33 "asm_l.gperf"
      {"ld1.bias_RMI", EM_LD1_BIAS_R1_R3_IMM9},
      {""}, {""}, {""}, {""},
#line 30 "asm_l.gperf"
      {"ld1.bias.nta_RMI", EM_LD1_BIAS_NTA_R1_R3_IMM9},
      {""},
#line 27 "asm_l.gperf"
      {"ld1.bias.nt1_RMI", EM_LD1_BIAS_NT1_R1_R3_IMM9},
      {""},
#line 34 "asm_l.gperf"
      {"ld1.bias_RM", EM_LD1_BIAS_R1_R3},
      {""}, {""}, {""},
#line 720 "asm_l.gperf"
      {"lfetch_MI", EM_LFETCH_R3_IMM9},
#line 31 "asm_l.gperf"
      {"ld1.bias.nta_RM", EM_LD1_BIAS_NTA_R1_R3},
      {""},
#line 28 "asm_l.gperf"
      {"ld1.bias.nt1_RM", EM_LD1_BIAS_NT1_R1_R3},
      {""},
#line 717 "asm_l.gperf"
      {"lfetch.nta_MI", EM_LFETCH_NTA_R3_IMM9},
#line 314 "asm_l.gperf"
      {"ld8.fill_RMR", EM_LD8_FILL_R1_R3_R2},
#line 711 "asm_l.gperf"
      {"lfetch.nt1_MI", EM_LFETCH_NT1_R3_IMM9},
      {""},
#line 721 "asm_l.gperf"
      {"lfetch_M", EM_LFETCH_R3},
      {""},
#line 311 "asm_l.gperf"
      {"ld8.fill.nta_RMR", EM_LD8_FILL_NTA_R1_R3_R2},
      {""},
#line 308 "asm_l.gperf"
      {"ld8.fill.nt1_RMR", EM_LD8_FILL_NT1_R1_R3_R2},
#line 718 "asm_l.gperf"
      {"lfetch.nta_M", EM_LFETCH_NTA_R3},
      {""},
#line 712 "asm_l.gperf"
      {"lfetch.nt1_M", EM_LFETCH_NT1_R3},
#line 504 "asm_l.gperf"
      {"ldf.fill_FMI", EM_LDF_FILL_F1_R3_IMM9},
      {""}, {""}, {""}, {""},
#line 510 "asm_l.gperf"
      {"ldf.fill.nta_FMI", EM_LDF_FILL_NTA_F1_R3_IMM9},
      {""},
#line 507 "asm_l.gperf"
      {"ldf.fill.nt1_FMI", EM_LDF_FILL_NT1_F1_R3_IMM9},
#line 35 "asm_l.gperf"
      {"ld1.bias_RMR", EM_LD1_BIAS_R1_R3_R2},
#line 505 "asm_l.gperf"
      {"ldf.fill_FM", EM_LDF_FILL_F1_R3},
      {""}, {""}, {""},
#line 32 "asm_l.gperf"
      {"ld1.bias.nta_RMR", EM_LD1_BIAS_NTA_R1_R3_R2},
#line 511 "asm_l.gperf"
      {"ldf.fill.nta_FM", EM_LDF_FILL_NTA_F1_R3},
#line 29 "asm_l.gperf"
      {"ld1.bias.nt1_RMR", EM_LD1_BIAS_NT1_R1_R3_R2},
#line 508 "asm_l.gperf"
      {"ldf.fill.nt1_FM", EM_LDF_FILL_NT1_F1_R3},
      {""}, {""}, {""}, {""},
#line 722 "asm_l.gperf"
      {"lfetch_MR", EM_LFETCH_R3_R2},
      {""}, {""}, {""}, {""},
#line 719 "asm_l.gperf"
      {"lfetch.nta_MR", EM_LFETCH_NTA_R3_R2},
      {""},
#line 713 "asm_l.gperf"
      {"lfetch.nt1_MR", EM_LFETCH_NT1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 39 "asm_l.gperf"
      {"ld1.c.clr.acq.nta_RMI", EM_LD1_C_CLR_ACQ_NTA_R1_R3_IMM9},
#line 506 "asm_l.gperf"
      {"ldf.fill_FMR", EM_LDF_FILL_F1_R3_R2},
#line 36 "asm_l.gperf"
      {"ld1.c.clr.acq.nt1_RMI", EM_LD1_C_CLR_ACQ_NT1_R1_R3_IMM9},
      {""}, {""}, {""},
#line 512 "asm_l.gperf"
      {"ldf.fill.nta_FMR", EM_LDF_FILL_NTA_F1_R3_R2},
      {""},
#line 509 "asm_l.gperf"
      {"ldf.fill.nt1_FMR", EM_LDF_FILL_NT1_F1_R3_R2},
#line 40 "asm_l.gperf"
      {"ld1.c.clr.acq.nta_RM", EM_LD1_C_CLR_ACQ_NTA_R1_R3},
      {""},
#line 37 "asm_l.gperf"
      {"ld1.c.clr.acq.nt1_RM", EM_LD1_C_CLR_ACQ_NT1_R1_R3},
      {""}, {""}, {""},
#line 276 "asm_l.gperf"
      {"ld8.bias_RMI", EM_LD8_BIAS_R1_R3_IMM9},
      {""}, {""}, {""}, {""},
#line 273 "asm_l.gperf"
      {"ld8.bias.nta_RMI", EM_LD8_BIAS_NTA_R1_R3_IMM9},
      {""},
#line 270 "asm_l.gperf"
      {"ld8.bias.nt1_RMI", EM_LD8_BIAS_NT1_R1_R3_IMM9},
      {""},
#line 277 "asm_l.gperf"
      {"ld8.bias_RM", EM_LD8_BIAS_R1_R3},
#line 42 "asm_l.gperf"
      {"ld1.c.clr.acq_RMI", EM_LD1_C_CLR_ACQ_R1_R3_IMM9},
      {""}, {""}, {""},
#line 274 "asm_l.gperf"
      {"ld8.bias.nta_RM", EM_LD8_BIAS_NTA_R1_R3},
      {""},
#line 271 "asm_l.gperf"
      {"ld8.bias.nt1_RM", EM_LD8_BIAS_NT1_R1_R3},
      {""}, {""},
#line 43 "asm_l.gperf"
      {"ld1.c.clr.acq_RM", EM_LD1_C_CLR_ACQ_R1_R3},
      {""}, {""}, {""},
#line 41 "asm_l.gperf"
      {"ld1.c.clr.acq.nta_RMR", EM_LD1_C_CLR_ACQ_NTA_R1_R3_R2},
      {""},
#line 38 "asm_l.gperf"
      {"ld1.c.clr.acq.nt1_RMR", EM_LD1_C_CLR_ACQ_NT1_R1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""},
#line 278 "asm_l.gperf"
      {"ld8.bias_RMR", EM_LD8_BIAS_R1_R3_R2},
      {""}, {""}, {""}, {""},
#line 275 "asm_l.gperf"
      {"ld8.bias.nta_RMR", EM_LD8_BIAS_NTA_R1_R3_R2},
      {""},
#line 272 "asm_l.gperf"
      {"ld8.bias.nt1_RMR", EM_LD8_BIAS_NT1_R1_R3_R2},
      {""}, {""},
#line 44 "asm_l.gperf"
      {"ld1.c.clr.acq_RMR", EM_LD1_C_CLR_ACQ_R1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 282 "asm_l.gperf"
      {"ld8.c.clr.acq.nta_RMI", EM_LD8_C_CLR_ACQ_NTA_R1_R3_IMM9},
      {""},
#line 279 "asm_l.gperf"
      {"ld8.c.clr.acq.nt1_RMI", EM_LD8_C_CLR_ACQ_NT1_R1_R3_IMM9},
      {""}, {""}, {""}, {""}, {""}, {""},
#line 283 "asm_l.gperf"
      {"ld8.c.clr.acq.nta_RM", EM_LD8_C_CLR_ACQ_NTA_R1_R3},
      {""},
#line 280 "asm_l.gperf"
      {"ld8.c.clr.acq.nt1_RM", EM_LD8_C_CLR_ACQ_NT1_R1_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""},
#line 705 "asm_l.gperf"
      {"lfetch.fault.nta_MI", EM_LFETCH_FAULT_NTA_R3_IMM9},
      {""},
#line 699 "asm_l.gperf"
      {"lfetch.fault.nt1_MI", EM_LFETCH_FAULT_NT1_R3_IMM9},
#line 285 "asm_l.gperf"
      {"ld8.c.clr.acq_RMI", EM_LD8_C_CLR_ACQ_R1_R3_IMM9},
      {""},
#line 708 "asm_l.gperf"
      {"lfetch.fault_MI", EM_LFETCH_FAULT_R3_IMM9},
      {""}, {""}, {""},
#line 706 "asm_l.gperf"
      {"lfetch.fault.nta_M", EM_LFETCH_FAULT_NTA_R3},
      {""},
#line 700 "asm_l.gperf"
      {"lfetch.fault.nt1_M", EM_LFETCH_FAULT_NT1_R3},
#line 286 "asm_l.gperf"
      {"ld8.c.clr.acq_RM", EM_LD8_C_CLR_ACQ_R1_R3},
      {""},
#line 709 "asm_l.gperf"
      {"lfetch.fault_M", EM_LFETCH_FAULT_R3},
      {""},
#line 284 "asm_l.gperf"
      {"ld8.c.clr.acq.nta_RMR", EM_LD8_C_CLR_ACQ_NTA_R1_R3_R2},
      {""},
#line 281 "asm_l.gperf"
      {"ld8.c.clr.acq.nt1_RMR", EM_LD8_C_CLR_ACQ_NT1_R1_R3_R2},
#line 707 "asm_l.gperf"
      {"lfetch.fault.nta_MR", EM_LFETCH_FAULT_NTA_R3_R2},
      {""},
#line 701 "asm_l.gperf"
      {"lfetch.fault.nt1_MR", EM_LFETCH_FAULT_NT1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""},
#line 287 "asm_l.gperf"
      {"ld8.c.clr.acq_RMR", EM_LD8_C_CLR_ACQ_R1_R3_R2},
      {""},
#line 710 "asm_l.gperf"
      {"lfetch.fault_MR", EM_LFETCH_FAULT_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 141 "asm_l.gperf"
      {"ld2.c.nc_RMI", EM_LD2_C_NC_R1_R3_IMM9},
      {""}, {""}, {""}, {""},
#line 138 "asm_l.gperf"
      {"ld2.c.nc.nta_RMI", EM_LD2_C_NC_NTA_R1_R3_IMM9},
      {""},
#line 135 "asm_l.gperf"
      {"ld2.c.nc.nt1_RMI", EM_LD2_C_NC_NT1_R1_R3_IMM9},
#line 132 "asm_l.gperf"
      {"ld2.c.clr_RMI", EM_LD2_C_CLR_R1_R3_IMM9},
#line 142 "asm_l.gperf"
      {"ld2.c.nc_RM", EM_LD2_C_NC_R1_R3},
      {""}, {""}, {""},
#line 129 "asm_l.gperf"
      {"ld2.c.clr.nta_RMI", EM_LD2_C_CLR_NTA_R1_R3_IMM9},
#line 139 "asm_l.gperf"
      {"ld2.c.nc.nta_RM", EM_LD2_C_NC_NTA_R1_R3},
#line 126 "asm_l.gperf"
      {"ld2.c.clr.nt1_RMI", EM_LD2_C_CLR_NT1_R1_R3_IMM9},
#line 136 "asm_l.gperf"
      {"ld2.c.nc.nt1_RM", EM_LD2_C_NC_NT1_R1_R3},
#line 133 "asm_l.gperf"
      {"ld2.c.clr_RM", EM_LD2_C_CLR_R1_R3},
      {""}, {""}, {""}, {""},
#line 130 "asm_l.gperf"
      {"ld2.c.clr.nta_RM", EM_LD2_C_CLR_NTA_R1_R3},
      {""},
#line 127 "asm_l.gperf"
      {"ld2.c.clr.nt1_RM", EM_LD2_C_CLR_NT1_R1_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""},
#line 143 "asm_l.gperf"
      {"ld2.c.nc_RMR", EM_LD2_C_NC_R1_R3_R2},
      {""}, {""}, {""}, {""},
#line 140 "asm_l.gperf"
      {"ld2.c.nc.nta_RMR", EM_LD2_C_NC_NTA_R1_R3_R2},
      {""},
#line 137 "asm_l.gperf"
      {"ld2.c.nc.nt1_RMR", EM_LD2_C_NC_NT1_R1_R3_R2},
#line 134 "asm_l.gperf"
      {"ld2.c.clr_RMR", EM_LD2_C_CLR_R1_R3_R2},
      {""}, {""}, {""}, {""},
#line 131 "asm_l.gperf"
      {"ld2.c.clr.nta_RMR", EM_LD2_C_CLR_NTA_R1_R3_R2},
      {""},
#line 128 "asm_l.gperf"
      {"ld2.c.clr.nt1_RMR", EM_LD2_C_CLR_NT1_R1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""},
#line 222 "asm_l.gperf"
      {"ld4.c.nc_RMI", EM_LD4_C_NC_R1_R3_IMM9},
      {""}, {""}, {""}, {""},
#line 219 "asm_l.gperf"
      {"ld4.c.nc.nta_RMI", EM_LD4_C_NC_NTA_R1_R3_IMM9},
      {""},
#line 216 "asm_l.gperf"
      {"ld4.c.nc.nt1_RMI", EM_LD4_C_NC_NT1_R1_R3_IMM9},
#line 213 "asm_l.gperf"
      {"ld4.c.clr_RMI", EM_LD4_C_CLR_R1_R3_IMM9},
#line 223 "asm_l.gperf"
      {"ld4.c.nc_RM", EM_LD4_C_NC_R1_R3},
      {""}, {""}, {""},
#line 210 "asm_l.gperf"
      {"ld4.c.clr.nta_RMI", EM_LD4_C_CLR_NTA_R1_R3_IMM9},
#line 220 "asm_l.gperf"
      {"ld4.c.nc.nta_RM", EM_LD4_C_NC_NTA_R1_R3},
#line 207 "asm_l.gperf"
      {"ld4.c.clr.nt1_RMI", EM_LD4_C_CLR_NT1_R1_R3_IMM9},
#line 217 "asm_l.gperf"
      {"ld4.c.nc.nt1_RM", EM_LD4_C_NC_NT1_R1_R3},
#line 214 "asm_l.gperf"
      {"ld4.c.clr_RM", EM_LD4_C_CLR_R1_R3},
      {""}, {""}, {""}, {""},
#line 211 "asm_l.gperf"
      {"ld4.c.clr.nta_RM", EM_LD4_C_CLR_NTA_R1_R3},
      {""},
#line 208 "asm_l.gperf"
      {"ld4.c.clr.nt1_RM", EM_LD4_C_CLR_NT1_R1_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""},
#line 224 "asm_l.gperf"
      {"ld4.c.nc_RMR", EM_LD4_C_NC_R1_R3_R2},
      {""}, {""}, {""}, {""},
#line 221 "asm_l.gperf"
      {"ld4.c.nc.nta_RMR", EM_LD4_C_NC_NTA_R1_R3_R2},
      {""},
#line 218 "asm_l.gperf"
      {"ld4.c.nc.nt1_RMR", EM_LD4_C_NC_NT1_R1_R3_R2},
#line 215 "asm_l.gperf"
      {"ld4.c.clr_RMR", EM_LD4_C_CLR_R1_R3_R2},
      {""}, {""}, {""}, {""},
#line 212 "asm_l.gperf"
      {"ld4.c.clr.nta_RMR", EM_LD4_C_CLR_NTA_R1_R3_R2},
      {""},
#line 209 "asm_l.gperf"
      {"ld4.c.clr.nt1_RMR", EM_LD4_C_CLR_NT1_R1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""},
#line 468 "asm_l.gperf"
      {"ldfe.c.nc_FMI", EM_LDFE_C_NC_F1_R3_IMM9},
      {""}, {""}, {""}, {""},
#line 474 "asm_l.gperf"
      {"ldfe.c.nc.nta_FMI", EM_LDFE_C_NC_NTA_F1_R3_IMM9},
      {""},
#line 471 "asm_l.gperf"
      {"ldfe.c.nc.nt1_FMI", EM_LDFE_C_NC_NT1_F1_R3_IMM9},
#line 459 "asm_l.gperf"
      {"ldfe.c.clr_FMI", EM_LDFE_C_CLR_F1_R3_IMM9},
#line 469 "asm_l.gperf"
      {"ldfe.c.nc_FM", EM_LDFE_C_NC_F1_R3},
      {""}, {""}, {""},
#line 465 "asm_l.gperf"
      {"ldfe.c.clr.nta_FMI", EM_LDFE_C_CLR_NTA_F1_R3_IMM9},
#line 475 "asm_l.gperf"
      {"ldfe.c.nc.nta_FM", EM_LDFE_C_NC_NTA_F1_R3},
#line 462 "asm_l.gperf"
      {"ldfe.c.clr.nt1_FMI", EM_LDFE_C_CLR_NT1_F1_R3_IMM9},
#line 472 "asm_l.gperf"
      {"ldfe.c.nc.nt1_FM", EM_LDFE_C_NC_NT1_F1_R3},
#line 460 "asm_l.gperf"
      {"ldfe.c.clr_FM", EM_LDFE_C_CLR_F1_R3},
      {""}, {""}, {""}, {""},
#line 466 "asm_l.gperf"
      {"ldfe.c.clr.nta_FM", EM_LDFE_C_CLR_NTA_F1_R3},
      {""},
#line 463 "asm_l.gperf"
      {"ldfe.c.clr.nt1_FM", EM_LDFE_C_CLR_NT1_F1_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 467 "asm_l.gperf"
      {"ldfe.c.clr.nta_FMR", EM_LDFE_C_CLR_NTA_F1_R3_R2},
      {""},
#line 464 "asm_l.gperf"
      {"ldfe.c.clr.nt1_FMR", EM_LDFE_C_CLR_NT1_F1_R3_R2},
      {""}, {""}, {""},
#line 470 "asm_l.gperf"
      {"ldfe.c.nc_FMR", EM_LDFE_C_NC_F1_R3_R2},
      {""}, {""}, {""}, {""},
#line 476 "asm_l.gperf"
      {"ldfe.c.nc.nta_FMR", EM_LDFE_C_NC_NTA_F1_R3_R2},
      {""},
#line 473 "asm_l.gperf"
      {"ldfe.c.nc.nt1_FMR", EM_LDFE_C_NC_NT1_F1_R3_R2},
#line 461 "asm_l.gperf"
      {"ldfe.c.clr_FMR", EM_LDFE_C_CLR_F1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""},
#line 96 "asm_l.gperf"
      {"ld2.acq_RMI", EM_LD2_ACQ_R1_R3_IMM9},
      {""}, {""}, {""}, {""},
#line 93 "asm_l.gperf"
      {"ld2.acq.nta_RMI", EM_LD2_ACQ_NTA_R1_R3_IMM9},
      {""},
#line 90 "asm_l.gperf"
      {"ld2.acq.nt1_RMI", EM_LD2_ACQ_NT1_R1_R3_IMM9},
      {""},
#line 97 "asm_l.gperf"
      {"ld2.acq_RM", EM_LD2_ACQ_R1_R3},
      {""}, {""}, {""}, {""},
#line 94 "asm_l.gperf"
      {"ld2.acq.nta_RM", EM_LD2_ACQ_NTA_R1_R3},
      {""},
#line 91 "asm_l.gperf"
      {"ld2.acq.nt1_RM", EM_LD2_ACQ_NT1_R1_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""},
#line 98 "asm_l.gperf"
      {"ld2.acq_RMR", EM_LD2_ACQ_R1_R3_R2},
      {""}, {""}, {""}, {""},
#line 95 "asm_l.gperf"
      {"ld2.acq.nta_RMR", EM_LD2_ACQ_NTA_R1_R3_R2},
      {""},
#line 92 "asm_l.gperf"
      {"ld2.acq.nt1_RMR", EM_LD2_ACQ_NT1_R1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""},
#line 177 "asm_l.gperf"
      {"ld4.acq_RMI", EM_LD4_ACQ_R1_R3_IMM9},
      {""}, {""}, {""}, {""},
#line 174 "asm_l.gperf"
      {"ld4.acq.nta_RMI", EM_LD4_ACQ_NTA_R1_R3_IMM9},
      {""},
#line 171 "asm_l.gperf"
      {"ld4.acq.nt1_RMI", EM_LD4_ACQ_NT1_R1_R3_IMM9},
      {""},
#line 178 "asm_l.gperf"
      {"ld4.acq_RM", EM_LD4_ACQ_R1_R3},
      {""}, {""}, {""}, {""},
#line 175 "asm_l.gperf"
      {"ld4.acq.nta_RM", EM_LD4_ACQ_NTA_R1_R3},
      {""},
#line 172 "asm_l.gperf"
      {"ld4.acq.nt1_RM", EM_LD4_ACQ_NT1_R1_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""},
#line 179 "asm_l.gperf"
      {"ld4.acq_RMR", EM_LD4_ACQ_R1_R3_R2},
      {""}, {""}, {""}, {""},
#line 176 "asm_l.gperf"
      {"ld4.acq.nta_RMR", EM_LD4_ACQ_NTA_R1_R3_R2},
      {""},
#line 173 "asm_l.gperf"
      {"ld4.acq.nt1_RMR", EM_LD4_ACQ_NT1_R1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""},
#line 114 "asm_l.gperf"
      {"ld2.bias_RMI", EM_LD2_BIAS_R1_R3_IMM9},
      {""}, {""}, {""}, {""},
#line 111 "asm_l.gperf"
      {"ld2.bias.nta_RMI", EM_LD2_BIAS_NTA_R1_R3_IMM9},
      {""},
#line 108 "asm_l.gperf"
      {"ld2.bias.nt1_RMI", EM_LD2_BIAS_NT1_R1_R3_IMM9},
      {""},
#line 115 "asm_l.gperf"
      {"ld2.bias_RM", EM_LD2_BIAS_R1_R3},
      {""}, {""}, {""}, {""},
#line 112 "asm_l.gperf"
      {"ld2.bias.nta_RM", EM_LD2_BIAS_NTA_R1_R3},
      {""},
#line 109 "asm_l.gperf"
      {"ld2.bias.nt1_RM", EM_LD2_BIAS_NT1_R1_R3},
      {""}, {""}, {""},
#line 714 "asm_l.gperf"
      {"lfetch.nt2_MI", EM_LFETCH_NT2_R3_IMM9},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 715 "asm_l.gperf"
      {"lfetch.nt2_M", EM_LFETCH_NT2_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 116 "asm_l.gperf"
      {"ld2.bias_RMR", EM_LD2_BIAS_R1_R3_R2},
      {""}, {""}, {""}, {""},
#line 113 "asm_l.gperf"
      {"ld2.bias.nta_RMR", EM_LD2_BIAS_NTA_R1_R3_R2},
      {""},
#line 110 "asm_l.gperf"
      {"ld2.bias.nt1_RMR", EM_LD2_BIAS_NT1_R1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""},
#line 716 "asm_l.gperf"
      {"lfetch.nt2_MR", EM_LFETCH_NT2_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 120 "asm_l.gperf"
      {"ld2.c.clr.acq.nta_RMI", EM_LD2_C_CLR_ACQ_NTA_R1_R3_IMM9},
      {""},
#line 117 "asm_l.gperf"
      {"ld2.c.clr.acq.nt1_RMI", EM_LD2_C_CLR_ACQ_NT1_R1_R3_IMM9},
      {""}, {""}, {""}, {""}, {""}, {""},
#line 121 "asm_l.gperf"
      {"ld2.c.clr.acq.nta_RM", EM_LD2_C_CLR_ACQ_NTA_R1_R3},
      {""},
#line 118 "asm_l.gperf"
      {"ld2.c.clr.acq.nt1_RM", EM_LD2_C_CLR_ACQ_NT1_R1_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""},
#line 123 "asm_l.gperf"
      {"ld2.c.clr.acq_RMI", EM_LD2_C_CLR_ACQ_R1_R3_IMM9},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 124 "asm_l.gperf"
      {"ld2.c.clr.acq_RM", EM_LD2_C_CLR_ACQ_R1_R3},
      {""},
#line 195 "asm_l.gperf"
      {"ld4.bias_RMI", EM_LD4_BIAS_R1_R3_IMM9},
      {""},
#line 122 "asm_l.gperf"
      {"ld2.c.clr.acq.nta_RMR", EM_LD2_C_CLR_ACQ_NTA_R1_R3_R2},
      {""},
#line 119 "asm_l.gperf"
      {"ld2.c.clr.acq.nt1_RMR", EM_LD2_C_CLR_ACQ_NT1_R1_R3_R2},
#line 192 "asm_l.gperf"
      {"ld4.bias.nta_RMI", EM_LD4_BIAS_NTA_R1_R3_IMM9},
      {""},
#line 189 "asm_l.gperf"
      {"ld4.bias.nt1_RMI", EM_LD4_BIAS_NT1_R1_R3_IMM9},
      {""},
#line 196 "asm_l.gperf"
      {"ld4.bias_RM", EM_LD4_BIAS_R1_R3},
      {""}, {""}, {""}, {""},
#line 193 "asm_l.gperf"
      {"ld4.bias.nta_RM", EM_LD4_BIAS_NTA_R1_R3},
      {""},
#line 190 "asm_l.gperf"
      {"ld4.bias.nt1_RM", EM_LD4_BIAS_NT1_R1_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""},
#line 125 "asm_l.gperf"
      {"ld2.c.clr.acq_RMR", EM_LD2_C_CLR_ACQ_R1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""},
#line 197 "asm_l.gperf"
      {"ld4.bias_RMR", EM_LD4_BIAS_R1_R3_R2},
      {""}, {""}, {""}, {""},
#line 194 "asm_l.gperf"
      {"ld4.bias.nta_RMR", EM_LD4_BIAS_NTA_R1_R3_R2},
      {""},
#line 191 "asm_l.gperf"
      {"ld4.bias.nt1_RMR", EM_LD4_BIAS_NT1_R1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""},
#line 201 "asm_l.gperf"
      {"ld4.c.clr.acq.nta_RMI", EM_LD4_C_CLR_ACQ_NTA_R1_R3_IMM9},
      {""},
#line 198 "asm_l.gperf"
      {"ld4.c.clr.acq.nt1_RMI", EM_LD4_C_CLR_ACQ_NT1_R1_R3_IMM9},
#line 702 "asm_l.gperf"
      {"lfetch.fault.nt2_MI", EM_LFETCH_FAULT_NT2_R3_IMM9},
      {""}, {""}, {""}, {""}, {""},
#line 202 "asm_l.gperf"
      {"ld4.c.clr.acq.nta_RM", EM_LD4_C_CLR_ACQ_NTA_R1_R3},
      {""},
#line 199 "asm_l.gperf"
      {"ld4.c.clr.acq.nt1_RM", EM_LD4_C_CLR_ACQ_NT1_R1_R3},
#line 703 "asm_l.gperf"
      {"lfetch.fault.nt2_M", EM_LFETCH_FAULT_NT2_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 704 "asm_l.gperf"
      {"lfetch.fault.nt2_MR", EM_LFETCH_FAULT_NT2_R3_R2},
      {""}, {""},
#line 204 "asm_l.gperf"
      {"ld4.c.clr.acq_RMI", EM_LD4_C_CLR_ACQ_R1_R3_IMM9},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 205 "asm_l.gperf"
      {"ld4.c.clr.acq_RM", EM_LD4_C_CLR_ACQ_R1_R3},
      {""}, {""}, {""},
#line 203 "asm_l.gperf"
      {"ld4.c.clr.acq.nta_RMR", EM_LD4_C_CLR_ACQ_NTA_R1_R3_R2},
      {""},
#line 200 "asm_l.gperf"
      {"ld4.c.clr.acq.nt1_RMR", EM_LD4_C_CLR_ACQ_NT1_R1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""},
#line 206 "asm_l.gperf"
      {"ld4.c.clr.acq_RMR", EM_LD4_C_CLR_ACQ_R1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""},
#line 684 "asm_l.gperf"
      {"lfetch.excl_MI", EM_LFETCH_EXCL_R3_IMM9},
      {""}, {""}, {""}, {""},
#line 681 "asm_l.gperf"
      {"lfetch.excl.nta_MI", EM_LFETCH_EXCL_NTA_R3_IMM9},
      {""},
#line 675 "asm_l.gperf"
      {"lfetch.excl.nt1_MI", EM_LFETCH_EXCL_NT1_R3_IMM9},
      {""},
#line 685 "asm_l.gperf"
      {"lfetch.excl_M", EM_LFETCH_EXCL_R3},
      {""}, {""}, {""}, {""},
#line 682 "asm_l.gperf"
      {"lfetch.excl.nta_M", EM_LFETCH_EXCL_NTA_R3},
      {""},
#line 676 "asm_l.gperf"
      {"lfetch.excl.nt1_M", EM_LFETCH_EXCL_NT1_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 683 "asm_l.gperf"
      {"lfetch.excl.nta_MR", EM_LFETCH_EXCL_NTA_R3_R2},
      {""},
#line 677 "asm_l.gperf"
      {"lfetch.excl.nt1_MR", EM_LFETCH_EXCL_NT1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""},
#line 686 "asm_l.gperf"
      {"lfetch.excl_MR", EM_LFETCH_EXCL_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""},
#line 696 "asm_l.gperf"
      {"lfetch.fault.excl_MI", EM_LFETCH_FAULT_EXCL_R3_IMM9},
      {""}, {""}, {""},
#line 693 "asm_l.gperf"
      {"lfetch.fault.excl.nta_MI", EM_LFETCH_FAULT_EXCL_NTA_R3_IMM9},
      {""},
#line 687 "asm_l.gperf"
      {"lfetch.fault.excl.nt1_MI", EM_LFETCH_FAULT_EXCL_NT1_R3_IMM9},
      {""}, {""},
#line 697 "asm_l.gperf"
      {"lfetch.fault.excl_M", EM_LFETCH_FAULT_EXCL_R3},
      {""}, {""}, {""},
#line 694 "asm_l.gperf"
      {"lfetch.fault.excl.nta_M", EM_LFETCH_FAULT_EXCL_NTA_R3},
      {""},
#line 688 "asm_l.gperf"
      {"lfetch.fault.excl.nt1_M", EM_LFETCH_FAULT_EXCL_NT1_R3},
      {""}, {""}, {""},
#line 698 "asm_l.gperf"
      {"lfetch.fault.excl_MR", EM_LFETCH_FAULT_EXCL_R3_R2},
      {""}, {""}, {""},
#line 695 "asm_l.gperf"
      {"lfetch.fault.excl.nta_MR", EM_LFETCH_FAULT_EXCL_NTA_R3_R2},
      {""},
#line 689 "asm_l.gperf"
      {"lfetch.fault.excl.nt1_MR", EM_LFETCH_FAULT_EXCL_NT1_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""},
#line 678 "asm_l.gperf"
      {"lfetch.excl.nt2_MI", EM_LFETCH_EXCL_NT2_R3_IMM9},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 679 "asm_l.gperf"
      {"lfetch.excl.nt2_M", EM_LFETCH_EXCL_NT2_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 680 "asm_l.gperf"
      {"lfetch.excl.nt2_MR", EM_LFETCH_EXCL_NT2_R3_R2},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""},
#line 690 "asm_l.gperf"
      {"lfetch.fault.excl.nt2_MI", EM_LFETCH_FAULT_EXCL_NT2_R3_IMM9},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 691 "asm_l.gperf"
      {"lfetch.fault.excl.nt2_M", EM_LFETCH_FAULT_EXCL_NT2_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 692 "asm_l.gperf"
      {"lfetch.fault.excl.nt2_MR", EM_LFETCH_FAULT_EXCL_NT2_R3_R2}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = l_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H m_hash -N m_lookup -D -o -j 1 -k '4-12,14-17,19,21-23,$' -T asm_m.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 69, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
m_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71, 71,  4, 71, 71, 24,
      22, 71, 26, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71,  0,  0, 29, 71, 71,
      71, 71, 71,  7, 71, 71, 71, 71, 71, 71,
      71, 71,  1, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71,  0, 71,  0, 11, 13,
       3,  1,  0, 71, 71,  0, 71, 18, 13,  2,
      71, 10,  2, 71,  0,  7,  0,  4, 71, 71,
      71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
      71, 71, 71, 71, 71, 71
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[18]];
      /*FALLTHROUGH*/
      case 18:
      case 17:
        hval += asso_values[(unsigned char)str[16]];
      /*FALLTHROUGH*/
      case 16:
        hval += asso_values[(unsigned char)str[15]];
      /*FALLTHROUGH*/
      case 15:
        hval += asso_values[(unsigned char)str[14]];
      /*FALLTHROUGH*/
      case 14:
        hval += asso_values[(unsigned char)str[13]];
      /*FALLTHROUGH*/
      case 13:
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      /*FALLTHROUGH*/
      case 11:
        hval += asso_values[(unsigned char)str[10]];
      /*FALLTHROUGH*/
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      /*FALLTHROUGH*/
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      /*FALLTHROUGH*/
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
      case 2:
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
m_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 53,
      MIN_WORD_LENGTH = 2,
      MAX_WORD_LENGTH = 20,
      MIN_HASH_VALUE = 2,
      MAX_HASH_VALUE = 70
    };

  static const struct asm_id wordlist[] =
    {
#line 4 "asm_m.gperf"
      {"mf", EM_MF},
#line 3 "asm_m.gperf"
      {"mf.a", EM_MF_A},
#line 32 "asm_m.gperf"
      {"mov_RB", EM_MOV_R1_B2},
#line 44 "asm_m.gperf"
      {"mov_Rrr", EM_MOV_R1_RR_R3},
#line 51 "asm_m.gperf"
      {"mov_rrR", EM_MOV_RR_R3_R2},
#line 41 "asm_m.gperf"
      {"mov_Rpr", EM_MOV_R1_PR},
#line 37 "asm_m.gperf"
      {"mov_Rip", EM_MOV_R1_IP},
#line 20 "asm_m.gperf"
      {"mov.i_RA", EM_MOV_I_R1_AR3},
#line 17 "asm_m.gperf"
      {"mov.i_AR", EM_MOV_I_AR3_R2},
#line 24 "asm_m.gperf"
      {"mov.m_RA", EM_MOV_M_R1_AR3},
#line 23 "asm_m.gperf"
      {"mov.m_AR", EM_MOV_M_AR3_R2},
#line 27 "asm_m.gperf"
      {"mov_pmdR", EM_MOV_PMD_R3_R2},
#line 42 "asm_m.gperf"
      {"mov_Rpsr", EM_MOV_R1_PSR},
#line 40 "asm_m.gperf"
      {"mov_Rpmd", EM_MOV_R1_PMD_R3},
#line 36 "asm_m.gperf"
      {"mov_Ribr", EM_MOV_R1_IBR_R3},
#line 18 "asm_m.gperf"
      {"mov_ibrR", EM_MOV_IBR_R3_R2},
#line 11 "asm_m.gperf"
      {"mov_BRI", EM_MOV_B1_R2_TAG13},
#line 35 "asm_m.gperf"
      {"mov_Rdbr", EM_MOV_R1_DBR_R3},
#line 13 "asm_m.gperf"
      {"mov_dbrR", EM_MOV_DBR_R3_R2},
#line 28 "asm_m.gperf"
      {"mov_prRI", EM_MOV_PR_R2_MASK17},
#line 16 "asm_m.gperf"
      {"mov.i_AI", EM_MOV_I_AR3_IMM8},
#line 26 "asm_m.gperf"
      {"mov_pmcR", EM_MOV_PMC_R3_R2},
#line 22 "asm_m.gperf"
      {"mov.m_AI", EM_MOV_M_AR3_IMM8},
#line 38 "asm_m.gperf"
      {"mov_Rpkr", EM_MOV_R1_PKR_R3},
#line 25 "asm_m.gperf"
      {"mov_pkrR", EM_MOV_PKR_R3_R2},
#line 45 "asm_m.gperf"
      {"mov.ret_BRI", EM_MOV_RET_B1_R2_TAG13},
#line 31 "asm_m.gperf"
      {"mov_psr.umR", EM_MOV_PSR_UM_R2},
#line 43 "asm_m.gperf"
      {"mov_Rpsr.um", EM_MOV_R1_PSR_UM},
#line 19 "asm_m.gperf"
      {"mov.imp_BRI", EM_MOV_IMP_B1_R2_TAG13},
#line 21 "asm_m.gperf"
      {"movl_RI", EM_MOVL_R1_IMM64},
#line 33 "asm_m.gperf"
      {"mov_Rcpuid", EM_MOV_R1_CPUID_R3},
#line 12 "asm_m.gperf"
      {"mov_CR", EM_MOV_CR3_R2},
#line 30 "asm_m.gperf"
      {"mov_psr.lR", EM_MOV_PSR_L_R2},
#line 39 "asm_m.gperf"
      {"mov_Rpmc", EM_MOV_R1_PMC_R3},
#line 8 "asm_m.gperf"
      {"mix2.r_RRR", EM_MIX2_R_R1_R2_R3},
#line 29 "asm_m.gperf"
      {"mov_pr.rotI", EM_MOV_PR_ROT_IMM44},
#line 6 "asm_m.gperf"
      {"mix1.r_RRR", EM_MIX1_R_R1_R2_R3},
#line 48 "asm_m.gperf"
      {"mov.ret.imp_BRI", EM_MOV_RET_IMP_B1_R2_TAG13},
#line 10 "asm_m.gperf"
      {"mix4.r_RRR", EM_MIX4_R_R1_R2_R3},
#line 55 "asm_m.gperf"
      {"mux2_RRI", EM_MUX2_R1_R2_MHTYPE8},
#line 54 "asm_m.gperf"
      {"mux1_RRI", EM_MUX1_R1_R2_MBTYPE4},
#line 7 "asm_m.gperf"
      {"mix2.l_RRR", EM_MIX2_L_R1_R2_R3},
#line 14 "asm_m.gperf"
      {"mov.dptk_BRI", EM_MOV_DPTK_B1_R2_TAG13},
#line 5 "asm_m.gperf"
      {"mix1.l_RRR", EM_MIX1_L_R1_R2_R3},
#line 9 "asm_m.gperf"
      {"mix4.l_RRR", EM_MIX4_L_R1_R2_R3},
#line 52 "asm_m.gperf"
      {"mov.sptk_BRI", EM_MOV_SPTK_B1_R2_TAG13},
#line 46 "asm_m.gperf"
      {"mov.ret.dptk_BRI", EM_MOV_RET_DPTK_B1_R2_TAG13},
#line 47 "asm_m.gperf"
      {"mov.ret.dptk.imp_BRI", EM_MOV_RET_DPTK_IMP_B1_R2_TAG13},
#line 34 "asm_m.gperf"
      {"mov_RC", EM_MOV_R1_CR3},
#line 15 "asm_m.gperf"
      {"mov.dptk.imp_BRI", EM_MOV_DPTK_IMP_B1_R2_TAG13},
#line 49 "asm_m.gperf"
      {"mov.ret.sptk_BRI", EM_MOV_RET_SPTK_B1_R2_TAG13},
#line 50 "asm_m.gperf"
      {"mov.ret.sptk.imp_BRI", EM_MOV_RET_SPTK_IMP_B1_R2_TAG13},
#line 53 "asm_m.gperf"
      {"mov.sptk.imp_BRI", EM_MOV_SPTK_IMP_B1_R2_TAG13}
    };

  static const signed char lookup[] =
    {
      -1, -1,  0, -1,  1, -1, -1,  2,  3,  4,  5, -1,  6,  7,
       8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
      22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
      36, 37, 38, -1, 39, -1, 40, -1, -1, -1, -1, 41, 42, 43,
      -1, 44, 45, -1, -1, -1, -1, 46, 47, 48, 49, 50, 51, -1,
      52
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = m_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = wordlist[index].name;

              if (*str == *s && !strcmp (str + 1, s + 1))
                return &wordlist[index];
            }
        }
    }
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H n_hash -N n_lookup -j 1 -n -k 5 -T asm_n.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 5, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
/*ARGSUSED*/
static unsigned int
n_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 4, 5,
      5, 5, 3, 5, 5, 2, 5, 5, 5, 1,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      0, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5
    };
  return asso_values[(unsigned char)str[4]];
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
n_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 5,
      MIN_WORD_LENGTH = 7,
      MAX_WORD_LENGTH = 7,
      MIN_HASH_VALUE = 0,
      MAX_HASH_VALUE = 4
    };

  static const struct asm_id wordlist[] =
    {
#line 7 "asm_n.gperf"
      {"nop.x_I", EM_NOP_X_IMM62},
#line 6 "asm_n.gperf"
      {"nop.m_I", EM_NOP_M_IMM21},
#line 5 "asm_n.gperf"
      {"nop.i_I", EM_NOP_I_IMM21},
#line 4 "asm_n.gperf"
      {"nop.f_I", EM_NOP_F_IMM21},
#line 3 "asm_n.gperf"
      {"nop.b_I", EM_NOP_B_IMM21}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = n_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H o_hash -N o_lookup -n -k 5 -T asm_o.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 2, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
/*ARGSUSED*/
static unsigned int
o_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 1, 2, 2, 2, 2, 2, 2,
      2, 2, 0, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2
    };
  return asso_values[(unsigned char)str[4]];
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
o_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 2,
      MIN_WORD_LENGTH = 6,
      MAX_WORD_LENGTH = 6,
      MIN_HASH_VALUE = 0,
      MAX_HASH_VALUE = 1
    };

  static const struct asm_id wordlist[] =
    {
#line 4 "asm_o.gperf"
      {"or_RRR", EM_OR_R1_R2_R3},
#line 3 "asm_o.gperf"
      {"or_RIR", EM_OR_R1_IMM8_R3}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = o_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H p_hash -N p_lookup -o -j 1 -k 3-5,7-9,11 -T asm_p.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 95, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
p_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107,  11, 107, 107,   4,
        0, 107,  16, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107,   3, 107, 107, 107, 107, 107, 107,
      107, 107,   1, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107,   0, 107,  21,   0,  37,
        4,   0,   3,   0,   9,  53, 107,  14,  14,  19,
        9,  21,  14,  20,   9,   1,  19,   0,  26,  27,
       42,  14,   3, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[10]];
      /*FALLTHROUGH*/
      case 10:
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      /*FALLTHROUGH*/
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
        hval += asso_values[(unsigned char)str[2]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
p_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 70,
      MIN_WORD_LENGTH = 7,
      MAX_WORD_LENGTH = 17,
      MIN_HASH_VALUE = 12,
      MAX_HASH_VALUE = 106
    };

  static const struct asm_id wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""},
#line 62 "asm_p.gperf"
      {"psub2_RRR", EM_PSUB2_R1_R2_R3},
      {""},
#line 65 "asm_p.gperf"
      {"psub2.uuu_RRR", EM_PSUB2_UUU_R1_R2_R3},
#line 64 "asm_p.gperf"
      {"psub2.uus_RRR", EM_PSUB2_UUS_R1_R2_R3},
#line 58 "asm_p.gperf"
      {"psub1_RRR", EM_PSUB1_R1_R2_R3},
#line 63 "asm_p.gperf"
      {"psub2.sss_RRR", EM_PSUB2_SSS_R1_R2_R3},
#line 61 "asm_p.gperf"
      {"psub1.uuu_RRR", EM_PSUB1_UUU_R1_R2_R3},
#line 60 "asm_p.gperf"
      {"psub1.uus_RRR", EM_PSUB1_UUS_R1_R2_R3},
#line 10 "asm_p.gperf"
      {"padd2_RRR", EM_PADD2_R1_R2_R3},
#line 59 "asm_p.gperf"
      {"psub1.sss_RRR", EM_PSUB1_SSS_R1_R2_R3},
#line 13 "asm_p.gperf"
      {"padd2.uuu_RRR", EM_PADD2_UUU_R1_R2_R3},
#line 12 "asm_p.gperf"
      {"padd2.uus_RRR", EM_PADD2_UUS_R1_R2_R3},
#line 6 "asm_p.gperf"
      {"padd1_RRR", EM_PADD1_R1_R2_R3},
#line 11 "asm_p.gperf"
      {"padd2.sss_RRR", EM_PADD2_SSS_R1_R2_R3},
#line 9 "asm_p.gperf"
      {"padd1.uuu_RRR", EM_PADD1_UUU_R1_R2_R3},
#line 8 "asm_p.gperf"
      {"padd1.uus_RRR", EM_PADD1_UUS_R1_R2_R3},
#line 66 "asm_p.gperf"
      {"psub4_RRR", EM_PSUB4_R1_R2_R3},
#line 7 "asm_p.gperf"
      {"padd1.sss_RRR", EM_PADD1_SSS_R1_R2_R3},
#line 50 "asm_p.gperf"
      {"pshr2_RRR", EM_PSHR2_R1_R3_R2},
#line 52 "asm_p.gperf"
      {"pshr2.u_RRR", EM_PSHR2_U_R1_R3_R2},
#line 49 "asm_p.gperf"
      {"pshr2_RRI", EM_PSHR2_R1_R3_COUNT5},
#line 51 "asm_p.gperf"
      {"pshr2.u_RRI", EM_PSHR2_U_R1_R3_COUNT5},
#line 71 "asm_p.gperf"
      {"ptr.d_RR", EM_PTR_D_R3_R2},
#line 45 "asm_p.gperf"
      {"pshl2_RRR", EM_PSHL2_R1_R2_R3},
#line 14 "asm_p.gperf"
      {"padd4_RRR", EM_PADD4_R1_R2_R3},
#line 44 "asm_p.gperf"
      {"pshl2_RRI", EM_PSHL2_R1_R2_COUNT5},
#line 17 "asm_p.gperf"
      {"pavg2_RRR", EM_PAVG2_R1_R2_R3},
      {""},
#line 20 "asm_p.gperf"
      {"pavgsub2_RRR", EM_PAVGSUB2_R1_R2_R3},
#line 43 "asm_p.gperf"
      {"psad1_RRR", EM_PSAD1_R1_R2_R3},
#line 15 "asm_p.gperf"
      {"pavg1_RRR", EM_PAVG1_R1_R2_R3},
#line 38 "asm_p.gperf"
      {"probe.r_RRR", EM_PROBE_R_R1_R3_R2},
#line 19 "asm_p.gperf"
      {"pavgsub1_RRR", EM_PAVGSUB1_R1_R2_R3},
#line 37 "asm_p.gperf"
      {"probe.r_RRI", EM_PROBE_R_R1_R3_IMM2},
#line 54 "asm_p.gperf"
      {"pshr4_RRR", EM_PSHR4_R1_R3_R2},
#line 56 "asm_p.gperf"
      {"pshr4.u_RRR", EM_PSHR4_U_R1_R3_R2},
#line 53 "asm_p.gperf"
      {"pshr4_RRI", EM_PSHR4_R1_R3_COUNT5},
#line 55 "asm_p.gperf"
      {"pshr4.u_RRI", EM_PSHR4_U_R1_R3_COUNT5},
#line 32 "asm_p.gperf"
      {"pmpy2.r_RRR", EM_PMPY2_R_R1_R2_R3},
#line 47 "asm_p.gperf"
      {"pshl4_RRR", EM_PSHL4_R1_R2_R3},
#line 33 "asm_p.gperf"
      {"pmpyshr2_RRRI", EM_PMPYSHR2_R1_R2_R3_COUNT2},
#line 46 "asm_p.gperf"
      {"pshl4_RRI", EM_PSHL4_R1_R2_COUNT5},
      {""},
#line 31 "asm_p.gperf"
      {"pmpy2.l_RRR", EM_PMPY2_L_R1_R2_R3},
#line 67 "asm_p.gperf"
      {"ptc.e_R", EM_PTC_E_R3},
#line 57 "asm_p.gperf"
      {"pshradd2_RRIR", EM_PSHRADD2_R1_R2_COUNT2_R3},
#line 69 "asm_p.gperf"
      {"ptc.g_RR", EM_PTC_G_R3_R2},
#line 68 "asm_p.gperf"
      {"ptc.ga_RR", EM_PTC_GA_R3_R2},
#line 36 "asm_p.gperf"
      {"probe.r.fault_RI", EM_PROBE_R_FAULT_R3_IMM2},
#line 42 "asm_p.gperf"
      {"probe.w_RRR", EM_PROBE_W_R1_R3_R2},
#line 48 "asm_p.gperf"
      {"pshladd2_RRIR", EM_PSHLADD2_R1_R2_COUNT2_R3},
#line 41 "asm_p.gperf"
      {"probe.w_RRI", EM_PROBE_W_R1_R3_IMM2},
#line 34 "asm_p.gperf"
      {"pmpyshr2.u_RRRI", EM_PMPYSHR2_U_R1_R2_R3_COUNT2},
#line 24 "asm_p.gperf"
      {"pcmp2.gt_RRR", EM_PCMP2_GT_R1_R2_R3},
#line 23 "asm_p.gperf"
      {"pcmp2.eq_RRR", EM_PCMP2_EQ_R1_R2_R3},
#line 4 "asm_p.gperf"
      {"pack2.uss_RRR", EM_PACK2_USS_R1_R2_R3},
#line 3 "asm_p.gperf"
      {"pack2.sss_RRR", EM_PACK2_SSS_R1_R2_R3},
#line 22 "asm_p.gperf"
      {"pcmp1.gt_RRR", EM_PCMP1_GT_R1_R2_R3},
#line 21 "asm_p.gperf"
      {"pcmp1.eq_RRR", EM_PCMP1_EQ_R1_R2_R3},
#line 35 "asm_p.gperf"
      {"popcnt_RR", EM_POPCNT_R1_R3},
#line 70 "asm_p.gperf"
      {"ptc.l_RR", EM_PTC_L_R3_R2},
#line 18 "asm_p.gperf"
      {"pavg2.raz_RRR", EM_PAVG2_RAZ_R1_R2_R3},
#line 30 "asm_p.gperf"
      {"pmin2_RRR", EM_PMIN2_R1_R2_R3},
#line 28 "asm_p.gperf"
      {"pmax2_RRR", EM_PMAX2_R1_R2_R3},
      {""},
#line 16 "asm_p.gperf"
      {"pavg1.raz_RRR", EM_PAVG1_RAZ_R1_R2_R3},
#line 40 "asm_p.gperf"
      {"probe.w.fault_RI", EM_PROBE_W_FAULT_R3_IMM2},
#line 29 "asm_p.gperf"
      {"pmin1.u_RRR", EM_PMIN1_U_R1_R2_R3},
#line 27 "asm_p.gperf"
      {"pmax1.u_RRR", EM_PMAX1_U_R1_R2_R3},
#line 26 "asm_p.gperf"
      {"pcmp4.gt_RRR", EM_PCMP4_GT_R1_R2_R3},
#line 25 "asm_p.gperf"
      {"pcmp4.eq_RRR", EM_PCMP4_EQ_R1_R2_R3},
#line 72 "asm_p.gperf"
      {"ptr.i_RR", EM_PTR_I_R3_R2},
#line 5 "asm_p.gperf"
      {"pack4.sss_RRR", EM_PACK4_SSS_R1_R2_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""},
#line 39 "asm_p.gperf"
      {"probe.rw.fault_RI", EM_PROBE_RW_FAULT_R3_IMM2}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = p_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

static const struct asm_id *
q_lookup (register const char *str, register unsigned int len)
{
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H r_hash -N r_lookup -n -k 2,4 -T asm_r.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 3, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
r_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 1, 3, 3, 3, 3,
      3, 3, 0, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 1, 3, 0, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3
    };
  register int hval = 0;

  switch (len)
    {
      default:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
      case 2:
        hval += asso_values[(unsigned char)str[1]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
r_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 3,
      MIN_WORD_LENGTH = 3,
      MAX_WORD_LENGTH = 5,
      MIN_HASH_VALUE = 0,
      MAX_HASH_VALUE = 2
    };

  static const struct asm_id wordlist[] =
    {
#line 3 "asm_r.gperf"
      {"rfi", EM_RFI},
#line 5 "asm_r.gperf"
      {"rum_I", EM_RUM_IMM24},
#line 4 "asm_r.gperf"
      {"rsm_I", EM_RSM_IMM24}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = r_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H s_hash -N s_lookup -j 1 -k 2-4,6 -T asm_s.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 92, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
s_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116, 116,   0, 116, 116,  20,
        2, 116,   0, 116,   4, 116,   8, 116, 116, 116,
      116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116,   5, 116, 116, 116,  39, 116, 116,
      116, 116,   0, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116,  32, 116,   5,  39,  30,
       44,   0,  10, 116,  14,  24, 116, 116,  22,   3,
        7, 116,  28, 116,  38,  41,  14,  16, 116, 116,
       59,  30,   0, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116, 116, 116, 116, 116, 116,
      116, 116, 116, 116, 116, 116
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
s_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 81,
      MIN_WORD_LENGTH = 5,
      MAX_WORD_LENGTH = 17,
      MIN_HASH_VALUE = 24,
      MAX_HASH_VALUE = 115
    };

  static const struct asm_id wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""},
#line 43 "asm_s.gperf"
      {"st4.rel_MR", EM_ST4_REL_R3_R2},
#line 42 "asm_s.gperf"
      {"st4.rel_MRI", EM_ST4_REL_R3_R2_IMM9},
#line 35 "asm_s.gperf"
      {"st2.rel_MR", EM_ST2_REL_R3_R2},
#line 34 "asm_s.gperf"
      {"st2.rel_MRI", EM_ST2_REL_R3_R2_IMM9},
#line 41 "asm_s.gperf"
      {"st4.rel.nta_MR", EM_ST4_REL_NTA_R3_R2},
#line 40 "asm_s.gperf"
      {"st4.rel.nta_MRI", EM_ST4_REL_NTA_R3_R2_IMM9},
#line 33 "asm_s.gperf"
      {"st2.rel.nta_MR", EM_ST2_REL_NTA_R3_R2},
#line 32 "asm_s.gperf"
      {"st2.rel.nta_MRI", EM_ST2_REL_NTA_R3_R2_IMM9},
#line 51 "asm_s.gperf"
      {"st8.rel_MR", EM_ST8_REL_R3_R2},
#line 50 "asm_s.gperf"
      {"st8.rel_MRI", EM_ST8_REL_R3_R2_IMM9},
      {""},
#line 4 "asm_s.gperf"
      {"setf.exp_FR", EM_SETF_EXP_F1_R2},
#line 49 "asm_s.gperf"
      {"st8.rel.nta_MR", EM_ST8_REL_NTA_R3_R2},
#line 48 "asm_s.gperf"
      {"st8.rel.nta_MRI", EM_ST8_REL_NTA_R3_R2_IMM9},
#line 37 "asm_s.gperf"
      {"st4.nta_MR", EM_ST4_NTA_R3_R2},
#line 36 "asm_s.gperf"
      {"st4.nta_MRI", EM_ST4_NTA_R3_R2_IMM9},
#line 29 "asm_s.gperf"
      {"st2.nta_MR", EM_ST2_NTA_R3_R2},
#line 28 "asm_s.gperf"
      {"st2.nta_MRI", EM_ST2_NTA_R3_R2_IMM9},
#line 65 "asm_s.gperf"
      {"stfe.nta_MF", EM_STFE_NTA_R3_F2},
#line 64 "asm_s.gperf"
      {"stfe.nta_MFI", EM_STFE_NTA_R3_F2_IMM9},
#line 27 "asm_s.gperf"
      {"st1.rel_MR", EM_ST1_REL_R3_R2},
#line 26 "asm_s.gperf"
      {"st1.rel_MRI", EM_ST1_REL_R3_R2_IMM9},
#line 45 "asm_s.gperf"
      {"st8.nta_MR", EM_ST8_NTA_R3_R2},
#line 44 "asm_s.gperf"
      {"st8.nta_MRI", EM_ST8_NTA_R3_R2_IMM9},
#line 25 "asm_s.gperf"
      {"st1.rel.nta_MR", EM_ST1_REL_NTA_R3_R2},
#line 24 "asm_s.gperf"
      {"st1.rel.nta_MRI", EM_ST1_REL_NTA_R3_R2_IMM9},
#line 57 "asm_s.gperf"
      {"stf8.nta_MF", EM_STF8_NTA_R3_F2},
#line 56 "asm_s.gperf"
      {"stf8.nta_MFI", EM_STF8_NTA_R3_F2_IMM9},
#line 39 "asm_s.gperf"
      {"st4_MR", EM_ST4_R3_R2},
#line 38 "asm_s.gperf"
      {"st4_MRI", EM_ST4_R3_R2_IMM9},
#line 31 "asm_s.gperf"
      {"st2_MR", EM_ST2_R3_R2},
#line 30 "asm_s.gperf"
      {"st2_MRI", EM_ST2_R3_R2_IMM9},
#line 79 "asm_s.gperf"
      {"sum_I", EM_SUM_IMM24},
#line 16 "asm_s.gperf"
      {"st16.nta_MRA", EM_ST16_NTA_R3_R2_AR_CSD},
#line 21 "asm_s.gperf"
      {"st1.nta_MR", EM_ST1_NTA_R3_R2},
#line 20 "asm_s.gperf"
      {"st1.nta_MRI", EM_ST1_NTA_R3_R2_IMM9},
#line 47 "asm_s.gperf"
      {"st8_MR", EM_ST8_R3_R2},
#line 46 "asm_s.gperf"
      {"st8_MRI", EM_ST8_R3_R2_IMM9},
#line 55 "asm_s.gperf"
      {"st8.spill_MR", EM_ST8_SPILL_R3_R2},
#line 54 "asm_s.gperf"
      {"st8.spill_MRI", EM_ST8_SPILL_R3_R2_IMM9},
#line 73 "asm_s.gperf"
      {"stf.spill_MF", EM_STF_SPILL_R3_F2},
#line 72 "asm_s.gperf"
      {"stf.spill_MFI", EM_STF_SPILL_R3_F2_IMM9},
#line 53 "asm_s.gperf"
      {"st8.spill.nta_MR", EM_ST8_SPILL_NTA_R3_R2},
#line 52 "asm_s.gperf"
      {"st8.spill.nta_MRI", EM_ST8_SPILL_NTA_R3_R2_IMM9},
#line 71 "asm_s.gperf"
      {"stf.spill.nta_MF", EM_STF_SPILL_NTA_R3_F2},
#line 70 "asm_s.gperf"
      {"stf.spill.nta_MFI", EM_STF_SPILL_NTA_R3_F2_IMM9},
#line 67 "asm_s.gperf"
      {"stfe_MF", EM_STFE_R3_F2},
#line 66 "asm_s.gperf"
      {"stfe_MFI", EM_STFE_R3_F2_IMM9},
#line 23 "asm_s.gperf"
      {"st1_MR", EM_ST1_R3_R2},
#line 22 "asm_s.gperf"
      {"st1_MRI", EM_ST1_R3_R2_IMM9},
#line 5 "asm_s.gperf"
      {"setf.s_FR", EM_SETF_S_F1_R2},
#line 9 "asm_s.gperf"
      {"shl_RRR", EM_SHL_R1_R2_R3},
#line 6 "asm_s.gperf"
      {"setf.sig_FR", EM_SETF_SIG_F1_R2},
#line 3 "asm_s.gperf"
      {"setf.d_FR", EM_SETF_D_F1_R2},
#line 59 "asm_s.gperf"
      {"stf8_MF", EM_STF8_R3_F2},
#line 58 "asm_s.gperf"
      {"stf8_MFI", EM_STF8_R3_F2_IMM9},
#line 82 "asm_s.gperf"
      {"sxt4_RR", EM_SXT4_R1_R3},
#line 15 "asm_s.gperf"
      {"ssm_I", EM_SSM_IMM24},
#line 81 "asm_s.gperf"
      {"sxt2_RR", EM_SXT2_R1_R3},
#line 69 "asm_s.gperf"
      {"stfs.nta_MF", EM_STFS_NTA_R3_F2},
#line 68 "asm_s.gperf"
      {"stfs.nta_MFI", EM_STFS_NTA_R3_F2_IMM9},
#line 17 "asm_s.gperf"
      {"st16_MRA", EM_ST16_R3_R2_AR_CSD},
#line 61 "asm_s.gperf"
      {"stfd.nta_MF", EM_STFD_NTA_R3_F2},
#line 60 "asm_s.gperf"
      {"stfd.nta_MFI", EM_STFD_NTA_R3_F2_IMM9},
#line 19 "asm_s.gperf"
      {"st16.rel_MRA", EM_ST16_REL_R3_R2_AR_CSD},
#line 10 "asm_s.gperf"
      {"shrp_RRRI", EM_SHRP_R1_R2_R3_COUNT6},
#line 14 "asm_s.gperf"
      {"srlz.i", EM_SRLZ_I},
#line 11 "asm_s.gperf"
      {"shr_RRR", EM_SHR_R1_R3_R2},
#line 18 "asm_s.gperf"
      {"st16.rel.nta_MRA", EM_ST16_REL_NTA_R3_R2_AR_CSD},
#line 12 "asm_s.gperf"
      {"shr.u_RRR", EM_SHR_U_R1_R3_R2},
#line 78 "asm_s.gperf"
      {"sub_RRR", EM_SUB_R1_R2_R3},
#line 77 "asm_s.gperf"
      {"sub_RRRI", EM_SUB_R1_R2_R3_1},
#line 8 "asm_s.gperf"
      {"shladd_RRIR", EM_SHLADD_R1_R2_COUNT2_R3},
#line 83 "asm_s.gperf"
      {"sync.i", EM_SYNC_I},
#line 7 "asm_s.gperf"
      {"shladdp4_RRIR", EM_SHLADDP4_R1_R2_COUNT2_R3},
#line 76 "asm_s.gperf"
      {"sub_RIR", EM_SUB_R1_IMM8_R3},
#line 80 "asm_s.gperf"
      {"sxt1_RR", EM_SXT1_R1_R3},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 13 "asm_s.gperf"
      {"srlz.d", EM_SRLZ_D},
#line 75 "asm_s.gperf"
      {"stfs_MF", EM_STFS_R3_F2},
#line 74 "asm_s.gperf"
      {"stfs_MFI", EM_STFS_R3_F2_IMM9},
      {""},
#line 63 "asm_s.gperf"
      {"stfd_MF", EM_STFD_R3_F2},
#line 62 "asm_s.gperf"
      {"stfd_MFI", EM_STFD_R3_F2_IMM9}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = s_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H t_hash -N t_lookup -j 1 -n -k 2,6,10,11 -T asm_t.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 20, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
t_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21,  7, 21, 21, 21,
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21,  5, 21, 21, 21, 21, 21, 21,
      12, 21, 13, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21,  3, 21,  2,  1,  8,
       1, 21, 21, 21,  6, 21, 21, 21, 21, 21,
       0, 21,  6, 21,  0, 21,  5, 21, 21, 21,
      21, 21,  1, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
      21, 21, 21, 21, 21, 21
    };
  register int hval = 0;

  switch (len)
    {
      default:
        hval += asso_values[(unsigned char)str[10]];
      /*FALLTHROUGH*/
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      /*FALLTHROUGH*/
      case 9:
      case 8:
      case 7:
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
      case 4:
      case 3:
      case 2:
        hval += asso_values[(unsigned char)str[1]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
t_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 20,
      MIN_WORD_LENGTH = 6,
      MAX_WORD_LENGTH = 21,
      MIN_HASH_VALUE = 1,
      MAX_HASH_VALUE = 20
    };

  static const struct asm_id wordlist[] =
    {
      {""},
#line 13 "asm_t.gperf"
      {"tnat.nz.and_PPR", EM_TNAT_NZ_AND_P1_P2_R3},
#line 4 "asm_t.gperf"
      {"tbit.nz.and_PPRI", EM_TBIT_NZ_AND_P1_P2_R3_POS6},
#line 15 "asm_t.gperf"
      {"tnat.nz.or_PPR", EM_TNAT_NZ_OR_P1_P2_R3},
#line 6 "asm_t.gperf"
      {"tbit.nz.or_PPRI", EM_TBIT_NZ_OR_P1_P2_R3_POS6},
#line 16 "asm_t.gperf"
      {"tnat.z.and_PPR", EM_TNAT_Z_AND_P1_P2_R3},
#line 7 "asm_t.gperf"
      {"tbit.z.and_PPRI", EM_TBIT_Z_AND_P1_P2_R3_POS6},
#line 14 "asm_t.gperf"
      {"tnat.nz.or.andcm_PPR", EM_TNAT_NZ_OR_ANDCM_P1_P2_R3},
#line 5 "asm_t.gperf"
      {"tbit.nz.or.andcm_PPRI", EM_TBIT_NZ_OR_ANDCM_P1_P2_R3_POS6},
#line 12 "asm_t.gperf"
      {"thash_RR", EM_THASH_R1_R3},
#line 17 "asm_t.gperf"
      {"tnat.z.or.andcm_PPR", EM_TNAT_Z_OR_ANDCM_P1_P2_R3},
#line 8 "asm_t.gperf"
      {"tbit.z.or.andcm_PPRI", EM_TBIT_Z_OR_ANDCM_P1_P2_R3_POS6},
#line 20 "asm_t.gperf"
      {"tnat.z.unc_PPR", EM_TNAT_Z_UNC_P1_P2_R3},
#line 11 "asm_t.gperf"
      {"tbit.z.unc_PPRI", EM_TBIT_Z_UNC_P1_P2_R3_POS6},
#line 19 "asm_t.gperf"
      {"tnat.z_PPR", EM_TNAT_Z_P1_P2_R3},
#line 3 "asm_t.gperf"
      {"tak_RR", EM_TAK_R1_R3},
#line 18 "asm_t.gperf"
      {"tnat.z.or_PPR", EM_TNAT_Z_OR_P1_P2_R3},
#line 9 "asm_t.gperf"
      {"tbit.z.or_PPRI", EM_TBIT_Z_OR_P1_P2_R3_POS6},
#line 22 "asm_t.gperf"
      {"ttag_RR", EM_TTAG_R1_R3},
#line 21 "asm_t.gperf"
      {"tpa_RR", EM_TPA_R1_R3},
#line 10 "asm_t.gperf"
      {"tbit.z_PPRI", EM_TBIT_Z_P1_P2_R3_POS6}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = t_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H u_hash -N u_lookup -j 1 -n -k 7,9 -T asm_u.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 6, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
/*ARGSUSED*/
static unsigned int
u_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 4,
      2, 6, 0, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 1, 6, 6, 6, 0, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6
    };
  return asso_values[(unsigned char)str[8]] + asso_values[(unsigned char)str[6]];
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
u_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 6,
      MIN_WORD_LENGTH = 13,
      MAX_WORD_LENGTH = 13,
      MIN_HASH_VALUE = 0,
      MAX_HASH_VALUE = 5
    };

  static const struct asm_id wordlist[] =
    {
#line 8 "asm_u.gperf"
      {"unpack4.l_RRR", EM_UNPACK4_L_R1_R2_R3},
#line 7 "asm_u.gperf"
      {"unpack4.h_RRR", EM_UNPACK4_H_R1_R2_R3},
#line 6 "asm_u.gperf"
      {"unpack2.l_RRR", EM_UNPACK2_L_R1_R2_R3},
#line 5 "asm_u.gperf"
      {"unpack2.h_RRR", EM_UNPACK2_H_R1_R2_R3},
#line 4 "asm_u.gperf"
      {"unpack1.l_RRR", EM_UNPACK1_L_R1_R2_R3},
#line 3 "asm_u.gperf"
      {"unpack1.h_RRR", EM_UNPACK1_H_R1_R2_R3}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = u_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

static const struct asm_id *
v_lookup (register const char *str, register unsigned int len)
{
  return 0;
}

static const struct asm_id *
w_lookup (register const char *str, register unsigned int len)
{
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H x_hash -N x_lookup -o -j 1 -n -k 5,6,9 -T asm_x.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 17, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
x_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17, 17,  0, 17, 17,  1,
      10, 17,  7, 17, 17, 17,  4, 17, 17, 17,
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
       6, 17, 17, 16, 17, 17, 17, 17, 17, 17,
      17, 17,  0, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17,  2, 17,  0, 17, 17,
      17, 17, 17, 17,  5, 17, 17, 17,  7, 17,
      17, 17, 17, 17, 17, 17, 17,  3, 17, 17,
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
      17, 17, 17, 17, 17, 17
    };
  register int hval = 0;

  switch (len)
    {
      default:
        hval += asso_values[(unsigned char)str[8]];
      /*FALLTHROUGH*/
      case 8:
      case 7:
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
        hval += asso_values[(unsigned char)str[4]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
x_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 17,
      MIN_WORD_LENGTH = 7,
      MAX_WORD_LENGTH = 13,
      MIN_HASH_VALUE = 0,
      MAX_HASH_VALUE = 16
    };

  static const struct asm_id wordlist[] =
    {
#line 19 "asm_x.gperf"
      {"xor_RRR", EM_XOR_R1_R2_R3},
#line 4 "asm_x.gperf"
      {"xchg1.nta_RMR", EM_XCHG1_NTA_R1_R3_R2},
#line 3 "asm_x.gperf"
      {"xchg1.nt1_RMR", EM_XCHG1_NT1_R1_R3_R2},
#line 5 "asm_x.gperf"
      {"xchg1_RMR", EM_XCHG1_R1_R3_R2},
#line 13 "asm_x.gperf"
      {"xchg8.nta_RMR", EM_XCHG8_NTA_R1_R3_R2},
#line 12 "asm_x.gperf"
      {"xchg8.nt1_RMR", EM_XCHG8_NT1_R1_R3_R2},
#line 14 "asm_x.gperf"
      {"xchg8_RMR", EM_XCHG8_R1_R3_R2},
#line 10 "asm_x.gperf"
      {"xchg4.nta_RMR", EM_XCHG4_NTA_R1_R3_R2},
#line 9 "asm_x.gperf"
      {"xchg4.nt1_RMR", EM_XCHG4_NT1_R1_R3_R2},
#line 11 "asm_x.gperf"
      {"xchg4_RMR", EM_XCHG4_R1_R3_R2},
#line 7 "asm_x.gperf"
      {"xchg2.nta_RMR", EM_XCHG2_NTA_R1_R3_R2},
#line 6 "asm_x.gperf"
      {"xchg2.nt1_RMR", EM_XCHG2_NT1_R1_R3_R2},
#line 8 "asm_x.gperf"
      {"xchg2_RMR", EM_XCHG2_R1_R3_R2},
#line 15 "asm_x.gperf"
      {"xma.h_FFFF", EM_XMA_H_F1_F3_F4_F2},
#line 16 "asm_x.gperf"
      {"xma.hu_FFFF", EM_XMA_HU_F1_F3_F4_F2},
#line 17 "asm_x.gperf"
      {"xma.l_FFFF", EM_XMA_L_F1_F3_F4_F2},
#line 18 "asm_x.gperf"
      {"xor_RIR", EM_XOR_R1_IMM8_R3}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = x_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

static const struct asm_id *
y_lookup (register const char *str, register unsigned int len)
{
  return 0;
}

/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -t -L ANSI-C -p -C -E -H z_hash -N z_lookup -n -k 4 -T asm_z.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

/* maximum key range = 3, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
/*ARGSUSED*/
static unsigned int
z_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 2,
      1, 3, 0, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3
    };
  return asso_values[(unsigned char)str[3]];
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
static const struct asm_id *
z_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 3,
      MIN_WORD_LENGTH = 7,
      MAX_WORD_LENGTH = 7,
      MIN_HASH_VALUE = 0,
      MAX_HASH_VALUE = 2
    };

  static const struct asm_id wordlist[] =
    {
#line 5 "asm_z.gperf"
      {"zxt4_RR", EM_ZXT4_R1_R3},
#line 4 "asm_z.gperf"
      {"zxt2_RR", EM_ZXT2_R1_R3},
#line 3 "asm_z.gperf"
      {"zxt1_RR", EM_ZXT1_R1_R3}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = z_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

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
