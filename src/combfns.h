/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* DO NOT EDIT - Automatically generated from:
     awk -f combfns.awk execTbl
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

Status illComb(INSTINFO *info);
Status illQpComb(INSTINFO *info);
Status illBQpComb(INSTINFO *info);
Status instFetchDecode(INSTINFO *info);
Status lastInstComb(INSTINFO *info);
Status ignComb(INSTINFO *info);
Status movGrComb(INSTINFO *info);
extern Status add_r1_r2_r3Comb(INSTINFO *info);
extern Status add_r1_r2_r3_1Comb(INSTINFO *info);
extern Status sub_r1_r2_r3Comb(INSTINFO *info);
extern Status sub_r1_r2_r3_1Comb(INSTINFO *info);
extern Status addp4_r1_r2_r3Comb(INSTINFO *info);
extern Status and_r1_r2_r3Comb(INSTINFO *info);
extern Status andcm_r1_r2_r3Comb(INSTINFO *info);
extern Status or_r1_r2_r3Comb(INSTINFO *info);
extern Status xor_r1_r2_r3Comb(INSTINFO *info);
extern Status shladd_r1_r2_count2_r3Comb(INSTINFO *info);
extern Status shladdp4_r1_r2_count2_r3Comb(INSTINFO *info);
extern Status sub_r1_imm8_r3Comb(INSTINFO *info);
extern Status and_r1_imm8_r3Comb(INSTINFO *info);
extern Status andcm_r1_imm8_r3Comb(INSTINFO *info);
extern Status or_r1_imm8_r3Comb(INSTINFO *info);
extern Status xor_r1_imm8_r3Comb(INSTINFO *info);
extern Status adds_r1_imm14_r3Comb(INSTINFO *info);
extern Status addp4_r1_imm14_r3Comb(INSTINFO *info);
#define addl_r1_imm22_r3Comb adds_r1_imm14_r3Comb
extern Status cmp_lt_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp_ltu_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp_eq_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp_lt_unc_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp_ltu_unc_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp_eq_unc_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp_eq_and_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp_eq_or_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp_eq_or_andcm_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp_ne_and_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp_ne_or_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp_ne_or_andcm_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp4_lt_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp4_ltu_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp4_eq_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp4_lt_unc_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp4_ltu_unc_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp4_eq_unc_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp4_eq_and_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp4_eq_or_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp4_eq_or_andcm_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp4_ne_and_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp4_ne_or_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp4_ne_or_andcm_p1_p2_r2_r3Comb(INSTINFO *info);
extern Status cmp_gt_and_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp_gt_or_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp_gt_or_andcm_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp_le_and_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp_le_or_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp_le_or_andcm_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp_ge_and_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp_ge_or_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp_ge_or_andcm_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp_lt_and_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp_lt_or_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp_lt_or_andcm_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp4_gt_and_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp4_gt_or_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp4_gt_or_andcm_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp4_le_and_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp4_le_or_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp4_le_or_andcm_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp4_ge_and_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp4_ge_or_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp4_ge_or_andcm_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp4_lt_and_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp4_lt_or_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp4_lt_or_andcm_p1_p2_r0_r3Comb(INSTINFO *info);
extern Status cmp_lt_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp_ltu_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp_eq_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp_lt_unc_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp_ltu_unc_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp_eq_unc_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp_eq_and_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp_eq_or_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp_eq_or_andcm_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp_ne_and_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp_ne_or_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp_ne_or_andcm_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp4_lt_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp4_ltu_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp4_eq_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp4_lt_unc_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp4_ltu_unc_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp4_eq_unc_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp4_eq_and_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp4_eq_or_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp4_eq_or_andcm_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp4_ne_and_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp4_ne_or_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status cmp4_ne_or_andcm_p1_p2_imm8_r3Comb(INSTINFO *info);
extern Status padd1_r1_r2_r3Comb(INSTINFO *info);
extern Status padd2_r1_r2_r3Comb(INSTINFO *info);
extern Status padd4_r1_r2_r3Comb(INSTINFO *info);
extern Status padd1_sss_r1_r2_r3Comb(INSTINFO *info);
extern Status padd2_sss_r1_r2_r3Comb(INSTINFO *info);
extern Status padd1_uuu_r1_r2_r3Comb(INSTINFO *info);
extern Status padd2_uuu_r1_r2_r3Comb(INSTINFO *info);
extern Status padd1_uus_r1_r2_r3Comb(INSTINFO *info);
extern Status padd2_uus_r1_r2_r3Comb(INSTINFO *info);
extern Status psub1_r1_r2_r3Comb(INSTINFO *info);
extern Status psub2_r1_r2_r3Comb(INSTINFO *info);
extern Status psub4_r1_r2_r3Comb(INSTINFO *info);
extern Status psub1_sss_r1_r2_r3Comb(INSTINFO *info);
extern Status psub2_sss_r1_r2_r3Comb(INSTINFO *info);
extern Status psub1_uuu_r1_r2_r3Comb(INSTINFO *info);
extern Status psub2_uuu_r1_r2_r3Comb(INSTINFO *info);
extern Status psub1_uus_r1_r2_r3Comb(INSTINFO *info);
extern Status psub2_uus_r1_r2_r3Comb(INSTINFO *info);
extern Status pavg1_r1_r2_r3Comb(INSTINFO *info);
extern Status pavg2_r1_r2_r3Comb(INSTINFO *info);
extern Status pavg1_raz_r1_r2_r3Comb(INSTINFO *info);
extern Status pavg2_raz_r1_r2_r3Comb(INSTINFO *info);
extern Status pavgsub1_r1_r2_r3Comb(INSTINFO *info);
extern Status pavgsub2_r1_r2_r3Comb(INSTINFO *info);
extern Status pcmp1_eq_r1_r2_r3Comb(INSTINFO *info);
extern Status pcmp2_eq_r1_r2_r3Comb(INSTINFO *info);
extern Status pcmp4_eq_r1_r2_r3Comb(INSTINFO *info);
extern Status pcmp1_gt_r1_r2_r3Comb(INSTINFO *info);
extern Status pcmp2_gt_r1_r2_r3Comb(INSTINFO *info);
extern Status pcmp4_gt_r1_r2_r3Comb(INSTINFO *info);
extern Status pshladd2_r1_r2_count2_r3Comb(INSTINFO *info);
extern Status pshradd2_r1_r2_count2_r3Comb(INSTINFO *info);
extern Status pmpyshr2_r1_r2_r3_count2Comb(INSTINFO *info);
extern Status pmpyshr2_u_r1_r2_r3_count2Comb(INSTINFO *info);
extern Status pmpy2_r_r1_r2_r3Comb(INSTINFO *info);
extern Status pmpy2_l_r1_r2_r3Comb(INSTINFO *info);
extern Status mix1_r_r1_r2_r3Comb(INSTINFO *info);
extern Status mix2_r_r1_r2_r3Comb(INSTINFO *info);
extern Status mix4_r_r1_r2_r3Comb(INSTINFO *info);
extern Status mix1_l_r1_r2_r3Comb(INSTINFO *info);
extern Status mix2_l_r1_r2_r3Comb(INSTINFO *info);
extern Status mix4_l_r1_r2_r3Comb(INSTINFO *info);
extern Status pack2_uss_r1_r2_r3Comb(INSTINFO *info);
extern Status pack2_sss_r1_r2_r3Comb(INSTINFO *info);
extern Status pack4_sss_r1_r2_r3Comb(INSTINFO *info);
extern Status unpack1_h_r1_r2_r3Comb(INSTINFO *info);
extern Status unpack2_h_r1_r2_r3Comb(INSTINFO *info);
#define unpack4_h_r1_r2_r3Comb mix4_l_r1_r2_r3Comb
extern Status unpack1_l_r1_r2_r3Comb(INSTINFO *info);
extern Status unpack2_l_r1_r2_r3Comb(INSTINFO *info);
#define unpack4_l_r1_r2_r3Comb mix4_r_r1_r2_r3Comb
extern Status pmin1_u_r1_r2_r3Comb(INSTINFO *info);
extern Status pmax1_u_r1_r2_r3Comb(INSTINFO *info);
extern Status pmin2_r1_r2_r3Comb(INSTINFO *info);
extern Status pmax2_r1_r2_r3Comb(INSTINFO *info);
extern Status psad1_r1_r2_r3Comb(INSTINFO *info);
extern Status mux1_r1_r2_mbtype4Comb(INSTINFO *info);
extern Status mux2_r1_r2_mhtype8Comb(INSTINFO *info);
extern Status pshr2_r1_r3_r2Comb(INSTINFO *info);
extern Status pshr4_r1_r3_r2Comb(INSTINFO *info);
extern Status shr_r1_r3_r2Comb(INSTINFO *info);
extern Status pshr2_u_r1_r3_r2Comb(INSTINFO *info);
extern Status pshr4_u_r1_r3_r2Comb(INSTINFO *info);
extern Status shr_u_r1_r3_r2Comb(INSTINFO *info);
extern Status pshr2_r1_r3_count5Comb(INSTINFO *info);
extern Status pshr4_r1_r3_count5Comb(INSTINFO *info);
extern Status pshr2_u_r1_r3_count5Comb(INSTINFO *info);
extern Status pshr4_u_r1_r3_count5Comb(INSTINFO *info);
extern Status pshl2_r1_r2_r3Comb(INSTINFO *info);
extern Status pshl4_r1_r2_r3Comb(INSTINFO *info);
extern Status shl_r1_r2_r3Comb(INSTINFO *info);
extern Status pshl2_r1_r2_count5Comb(INSTINFO *info);
extern Status pshl4_r1_r2_count5Comb(INSTINFO *info);
extern Status popcnt_r1_r3Comb(INSTINFO *info);
extern Status shrp_r1_r2_r3_count6Comb(INSTINFO *info);
extern Status extr_u_r1_r3_pos6_len6Comb(INSTINFO *info);
extern Status extr_r1_r3_pos6_len6Comb(INSTINFO *info);
extern Status dep_z_r1_r2_pos6_len6Comb(INSTINFO *info);
extern Status dep_z_r1_imm8_pos6_len6Comb(INSTINFO *info);
extern Status dep_r1_imm1_r3_pos6_len6Comb(INSTINFO *info);
extern Status dep_r1_r2_r3_pos6_len4Comb(INSTINFO *info);
extern Status tbit_z_p1_p2_r3_pos6Comb(INSTINFO *info);
extern Status tbit_z_unc_p1_p2_r3_pos6Comb(INSTINFO *info);
extern Status tbit_z_and_p1_p2_r3_pos6Comb(INSTINFO *info);
extern Status tbit_nz_and_p1_p2_r3_pos6Comb(INSTINFO *info);
extern Status tbit_z_or_p1_p2_r3_pos6Comb(INSTINFO *info);
extern Status tbit_nz_or_p1_p2_r3_pos6Comb(INSTINFO *info);
extern Status tbit_z_or_andcm_p1_p2_r3_pos6Comb(INSTINFO *info);
extern Status tbit_nz_or_andcm_p1_p2_r3_pos6Comb(INSTINFO *info);
extern Status tnat_z_p1_p2_r3Comb(INSTINFO *info);
extern Status tnat_z_unc_p1_p2_r3Comb(INSTINFO *info);
extern Status tnat_z_and_p1_p2_r3Comb(INSTINFO *info);
extern Status tnat_nz_and_p1_p2_r3Comb(INSTINFO *info);
extern Status tnat_z_or_p1_p2_r3Comb(INSTINFO *info);
extern Status tnat_nz_or_p1_p2_r3Comb(INSTINFO *info);
extern Status tnat_z_or_andcm_p1_p2_r3Comb(INSTINFO *info);
extern Status tnat_nz_or_andcm_p1_p2_r3Comb(INSTINFO *info);
extern Status nop_i_imm21Comb(INSTINFO *info);
extern Status hint_i_imm21Comb(INSTINFO *info);
extern Status break_i_imm21Comb(INSTINFO *info);
extern Status chk_s_i_r2_target25Comb(INSTINFO *info);
extern Status mov_b1_r2_tag13Comb(INSTINFO *info);
#define mov_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_many_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_many_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_many_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_many_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_many_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_many_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dc_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dc_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_many_dc_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_many_dc_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_dc_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_dc_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_many_dc_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_many_dc_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_dc_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_dc_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_many_dc_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_many_dc_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_tk_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_tk_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_many_tk_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_many_tk_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_tk_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_tk_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_many_tk_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_many_tk_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_tk_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_tk_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_many_tk_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_many_tk_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_tk_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_tk_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_many_tk_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_many_tk_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_tk_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_tk_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_many_tk_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_many_tk_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_tk_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_tk_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_many_tk_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_many_tk_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_tk_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_tk_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_many_tk_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_many_tk_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_tk_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_tk_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_many_tk_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_many_tk_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_tk_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_tk_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_many_tk_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_many_tk_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_nt_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_nt_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_many_nt_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_many_nt_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_nt_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_nt_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_many_nt_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_many_nt_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_nt_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_nt_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_many_nt_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_many_nt_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_nt_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_nt_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_many_nt_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_many_nt_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_nt_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_nt_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_many_nt_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_many_nt_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_nt_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_nt_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_many_nt_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_many_nt_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_nt_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_nt_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_many_nt_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_many_nt_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_nt_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_nt_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_many_nt_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_sptk_many_nt_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_nt_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_nt_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_many_nt_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_dptk_many_nt_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_many_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_many_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_many_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_many_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_many_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_many_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dc_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dc_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_many_dc_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_many_dc_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_dc_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_dc_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_many_dc_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_many_dc_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_dc_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_dc_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_many_dc_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_many_dc_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_tk_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_tk_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_many_tk_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_many_tk_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_tk_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_tk_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_many_tk_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_many_tk_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_tk_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_tk_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_many_tk_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_many_tk_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_tk_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_tk_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_many_tk_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_many_tk_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_tk_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_tk_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_many_tk_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_many_tk_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_tk_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_tk_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_many_tk_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_many_tk_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_tk_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_tk_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_many_tk_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_many_tk_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_tk_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_tk_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_many_tk_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_many_tk_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_tk_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_tk_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_many_tk_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_many_tk_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_nt_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_nt_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_many_nt_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_many_nt_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_nt_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_nt_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_many_nt_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_many_nt_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_nt_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_nt_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_many_nt_dc_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_many_nt_dc_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_nt_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_nt_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_many_nt_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_many_nt_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_nt_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_nt_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_many_nt_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_many_nt_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_nt_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_nt_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_many_nt_tk_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_many_nt_tk_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_nt_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_nt_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_many_nt_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_many_nt_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_nt_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_nt_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_many_nt_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_sptk_many_nt_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_nt_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_nt_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_many_nt_nt_b1_r2_tag13Comb mov_b1_r2_tag13Comb
#define mov_ret_dptk_many_nt_nt_imp_b1_r2_tag13Comb mov_b1_r2_tag13Comb
extern Status mov_r1_b2Comb(INSTINFO *info);
extern Status mov_pr_r2_mask17Comb(INSTINFO *info);
extern Status mov_pr_rot_imm44Comb(INSTINFO *info);
extern Status mov_r1_ipComb(INSTINFO *info);
extern Status mov_r1_prComb(INSTINFO *info);
extern Status mov_i_ar3_r2Comb(INSTINFO *info);
extern Status mov_i_ar3_imm8Comb(INSTINFO *info);
extern Status mov_i_r1_ar3Comb(INSTINFO *info);
extern Status zxt1_r1_r3Comb(INSTINFO *info);
extern Status zxt2_r1_r3Comb(INSTINFO *info);
extern Status zxt4_r1_r3Comb(INSTINFO *info);
extern Status sxt1_r1_r3Comb(INSTINFO *info);
extern Status sxt2_r1_r3Comb(INSTINFO *info);
extern Status sxt4_r1_r3Comb(INSTINFO *info);
extern Status czx1_l_r1_r3Comb(INSTINFO *info);
extern Status czx2_l_r1_r3Comb(INSTINFO *info);
extern Status czx1_r_r1_r3Comb(INSTINFO *info);
extern Status czx2_r_r1_r3Comb(INSTINFO *info);
extern Status ld1_r1_r3Comb(INSTINFO *info);
#define ld1_nt1_r1_r3Comb ld1_r1_r3Comb
#define ld1_nta_r1_r3Comb ld1_r1_r3Comb
extern Status ld2_r1_r3Comb(INSTINFO *info);
#define ld2_nt1_r1_r3Comb ld2_r1_r3Comb
#define ld2_nta_r1_r3Comb ld2_r1_r3Comb
extern Status ld4_r1_r3Comb(INSTINFO *info);
#define ld4_nt1_r1_r3Comb ld4_r1_r3Comb
#define ld4_nta_r1_r3Comb ld4_r1_r3Comb
extern Status ld8_r1_r3Comb(INSTINFO *info);
#define ld8_nt1_r1_r3Comb ld8_r1_r3Comb
#define ld8_nta_r1_r3Comb ld8_r1_r3Comb
extern Status ld16_r1_ar_csd_r3Comb(INSTINFO *info);
#define ld16_nt1_r1_ar_csd_r3Comb ld16_r1_ar_csd_r3Comb
#define ld16_nta_r1_ar_csd_r3Comb ld16_r1_ar_csd_r3Comb
extern Status ld1_s_r1_r3Comb(INSTINFO *info);
#define ld1_s_nt1_r1_r3Comb ld1_s_r1_r3Comb
#define ld1_s_nta_r1_r3Comb ld1_s_r1_r3Comb
extern Status ld2_s_r1_r3Comb(INSTINFO *info);
#define ld2_s_nt1_r1_r3Comb ld2_s_r1_r3Comb
#define ld2_s_nta_r1_r3Comb ld2_s_r1_r3Comb
extern Status ld4_s_r1_r3Comb(INSTINFO *info);
#define ld4_s_nt1_r1_r3Comb ld4_s_r1_r3Comb
#define ld4_s_nta_r1_r3Comb ld4_s_r1_r3Comb
extern Status ld8_s_r1_r3Comb(INSTINFO *info);
#define ld8_s_nt1_r1_r3Comb ld8_s_r1_r3Comb
#define ld8_s_nta_r1_r3Comb ld8_s_r1_r3Comb
extern Status ld1_a_r1_r3Comb(INSTINFO *info);
#define ld1_a_nt1_r1_r3Comb ld1_a_r1_r3Comb
#define ld1_a_nta_r1_r3Comb ld1_a_r1_r3Comb
extern Status ld2_a_r1_r3Comb(INSTINFO *info);
#define ld2_a_nt1_r1_r3Comb ld2_a_r1_r3Comb
#define ld2_a_nta_r1_r3Comb ld2_a_r1_r3Comb
extern Status ld4_a_r1_r3Comb(INSTINFO *info);
#define ld4_a_nt1_r1_r3Comb ld4_a_r1_r3Comb
#define ld4_a_nta_r1_r3Comb ld4_a_r1_r3Comb
extern Status ld8_a_r1_r3Comb(INSTINFO *info);
#define ld8_a_nt1_r1_r3Comb ld8_a_r1_r3Comb
#define ld8_a_nta_r1_r3Comb ld8_a_r1_r3Comb
extern Status ld1_sa_r1_r3Comb(INSTINFO *info);
#define ld1_sa_nt1_r1_r3Comb ld1_sa_r1_r3Comb
#define ld1_sa_nta_r1_r3Comb ld1_sa_r1_r3Comb
extern Status ld2_sa_r1_r3Comb(INSTINFO *info);
#define ld2_sa_nt1_r1_r3Comb ld2_sa_r1_r3Comb
#define ld2_sa_nta_r1_r3Comb ld2_sa_r1_r3Comb
extern Status ld4_sa_r1_r3Comb(INSTINFO *info);
#define ld4_sa_nt1_r1_r3Comb ld4_sa_r1_r3Comb
#define ld4_sa_nta_r1_r3Comb ld4_sa_r1_r3Comb
extern Status ld8_sa_r1_r3Comb(INSTINFO *info);
#define ld8_sa_nt1_r1_r3Comb ld8_sa_r1_r3Comb
#define ld8_sa_nta_r1_r3Comb ld8_sa_r1_r3Comb
#define ld1_bias_r1_r3Comb ld1_r1_r3Comb
#define ld1_bias_nt1_r1_r3Comb ld1_bias_r1_r3Comb
#define ld1_bias_nta_r1_r3Comb ld1_bias_r1_r3Comb
#define ld2_bias_r1_r3Comb ld2_r1_r3Comb
#define ld2_bias_nt1_r1_r3Comb ld2_bias_r1_r3Comb
#define ld2_bias_nta_r1_r3Comb ld2_bias_r1_r3Comb
#define ld4_bias_r1_r3Comb ld4_r1_r3Comb
#define ld4_bias_nt1_r1_r3Comb ld4_bias_r1_r3Comb
#define ld4_bias_nta_r1_r3Comb ld4_bias_r1_r3Comb
#define ld8_bias_r1_r3Comb ld8_r1_r3Comb
#define ld8_bias_nt1_r1_r3Comb ld8_bias_r1_r3Comb
#define ld8_bias_nta_r1_r3Comb ld8_bias_r1_r3Comb
#define ld1_acq_r1_r3Comb ld1_r1_r3Comb
#define ld1_acq_nt1_r1_r3Comb ld1_acq_r1_r3Comb
#define ld1_acq_nta_r1_r3Comb ld1_acq_r1_r3Comb
#define ld2_acq_r1_r3Comb ld2_r1_r3Comb
#define ld2_acq_nt1_r1_r3Comb ld2_acq_r1_r3Comb
#define ld2_acq_nta_r1_r3Comb ld2_acq_r1_r3Comb
#define ld4_acq_r1_r3Comb ld4_r1_r3Comb
#define ld4_acq_nt1_r1_r3Comb ld4_acq_r1_r3Comb
#define ld4_acq_nta_r1_r3Comb ld4_acq_r1_r3Comb
#define ld8_acq_r1_r3Comb ld8_r1_r3Comb
#define ld8_acq_nt1_r1_r3Comb ld8_acq_r1_r3Comb
#define ld8_acq_nta_r1_r3Comb ld8_acq_r1_r3Comb
#define ld16_acq_r1_ar_csd_r3Comb ld16_r1_ar_csd_r3Comb
#define ld16_acq_nt1_r1_ar_csd_r3Comb ld16_acq_r1_ar_csd_r3Comb
#define ld16_acq_nta_r1_ar_csd_r3Comb ld16_acq_r1_ar_csd_r3Comb
extern Status ld8_fill_r1_r3Comb(INSTINFO *info);
#define ld8_fill_nt1_r1_r3Comb ld8_fill_r1_r3Comb
#define ld8_fill_nta_r1_r3Comb ld8_fill_r1_r3Comb
extern Status ld1_c_clr_r1_r3Comb(INSTINFO *info);
#define ld1_c_clr_nt1_r1_r3Comb ld1_c_clr_r1_r3Comb
#define ld1_c_clr_nta_r1_r3Comb ld1_c_clr_r1_r3Comb
extern Status ld2_c_clr_r1_r3Comb(INSTINFO *info);
#define ld2_c_clr_nt1_r1_r3Comb ld2_c_clr_r1_r3Comb
#define ld2_c_clr_nta_r1_r3Comb ld2_c_clr_r1_r3Comb
extern Status ld4_c_clr_r1_r3Comb(INSTINFO *info);
#define ld4_c_clr_nt1_r1_r3Comb ld4_c_clr_r1_r3Comb
#define ld4_c_clr_nta_r1_r3Comb ld4_c_clr_r1_r3Comb
extern Status ld8_c_clr_r1_r3Comb(INSTINFO *info);
#define ld8_c_clr_nt1_r1_r3Comb ld8_c_clr_r1_r3Comb
#define ld8_c_clr_nta_r1_r3Comb ld8_c_clr_r1_r3Comb
extern Status ld1_c_nc_r1_r3Comb(INSTINFO *info);
#define ld1_c_nc_nt1_r1_r3Comb ld1_c_nc_r1_r3Comb
#define ld1_c_nc_nta_r1_r3Comb ld1_c_nc_r1_r3Comb
extern Status ld2_c_nc_r1_r3Comb(INSTINFO *info);
#define ld2_c_nc_nt1_r1_r3Comb ld2_c_nc_r1_r3Comb
#define ld2_c_nc_nta_r1_r3Comb ld2_c_nc_r1_r3Comb
extern Status ld4_c_nc_r1_r3Comb(INSTINFO *info);
#define ld4_c_nc_nt1_r1_r3Comb ld4_c_nc_r1_r3Comb
#define ld4_c_nc_nta_r1_r3Comb ld4_c_nc_r1_r3Comb
extern Status ld8_c_nc_r1_r3Comb(INSTINFO *info);
#define ld8_c_nc_nt1_r1_r3Comb ld8_c_nc_r1_r3Comb
#define ld8_c_nc_nta_r1_r3Comb ld8_c_nc_r1_r3Comb
#define ld1_c_clr_acq_r1_r3Comb ld1_c_clr_r1_r3Comb
#define ld1_c_clr_acq_nt1_r1_r3Comb ld1_c_clr_acq_r1_r3Comb
#define ld1_c_clr_acq_nta_r1_r3Comb ld1_c_clr_acq_r1_r3Comb
#define ld2_c_clr_acq_r1_r3Comb ld2_c_clr_r1_r3Comb
#define ld2_c_clr_acq_nt1_r1_r3Comb ld2_c_clr_acq_r1_r3Comb
#define ld2_c_clr_acq_nta_r1_r3Comb ld2_c_clr_acq_r1_r3Comb
#define ld4_c_clr_acq_r1_r3Comb ld4_c_clr_r1_r3Comb
#define ld4_c_clr_acq_nt1_r1_r3Comb ld4_c_clr_acq_r1_r3Comb
#define ld4_c_clr_acq_nta_r1_r3Comb ld4_c_clr_acq_r1_r3Comb
#define ld8_c_clr_acq_r1_r3Comb ld8_c_clr_r1_r3Comb
#define ld8_c_clr_acq_nt1_r1_r3Comb ld8_c_clr_acq_r1_r3Comb
#define ld8_c_clr_acq_nta_r1_r3Comb ld8_c_clr_acq_r1_r3Comb
extern Status ld1_r1_r3_r2Comb(INSTINFO *info);
#define ld1_nt1_r1_r3_r2Comb ld1_r1_r3_r2Comb
#define ld1_nta_r1_r3_r2Comb ld1_r1_r3_r2Comb
extern Status ld2_r1_r3_r2Comb(INSTINFO *info);
#define ld2_nt1_r1_r3_r2Comb ld2_r1_r3_r2Comb
#define ld2_nta_r1_r3_r2Comb ld2_r1_r3_r2Comb
extern Status ld4_r1_r3_r2Comb(INSTINFO *info);
#define ld4_nt1_r1_r3_r2Comb ld4_r1_r3_r2Comb
#define ld4_nta_r1_r3_r2Comb ld4_r1_r3_r2Comb
extern Status ld8_r1_r3_r2Comb(INSTINFO *info);
#define ld8_nt1_r1_r3_r2Comb ld8_r1_r3_r2Comb
#define ld8_nta_r1_r3_r2Comb ld8_r1_r3_r2Comb
extern Status ld1_s_r1_r3_r2Comb(INSTINFO *info);
#define ld1_s_nt1_r1_r3_r2Comb ld1_s_r1_r3_r2Comb
#define ld1_s_nta_r1_r3_r2Comb ld1_s_r1_r3_r2Comb
extern Status ld2_s_r1_r3_r2Comb(INSTINFO *info);
#define ld2_s_nt1_r1_r3_r2Comb ld2_s_r1_r3_r2Comb
#define ld2_s_nta_r1_r3_r2Comb ld2_s_r1_r3_r2Comb
extern Status ld4_s_r1_r3_r2Comb(INSTINFO *info);
#define ld4_s_nt1_r1_r3_r2Comb ld4_s_r1_r3_r2Comb
#define ld4_s_nta_r1_r3_r2Comb ld4_s_r1_r3_r2Comb
extern Status ld8_s_r1_r3_r2Comb(INSTINFO *info);
#define ld8_s_nt1_r1_r3_r2Comb ld8_s_r1_r3_r2Comb
#define ld8_s_nta_r1_r3_r2Comb ld8_s_r1_r3_r2Comb
extern Status ld1_a_r1_r3_r2Comb(INSTINFO *info);
#define ld1_a_nt1_r1_r3_r2Comb ld1_a_r1_r3_r2Comb
#define ld1_a_nta_r1_r3_r2Comb ld1_a_r1_r3_r2Comb
extern Status ld2_a_r1_r3_r2Comb(INSTINFO *info);
#define ld2_a_nt1_r1_r3_r2Comb ld2_a_r1_r3_r2Comb
#define ld2_a_nta_r1_r3_r2Comb ld2_a_r1_r3_r2Comb
extern Status ld4_a_r1_r3_r2Comb(INSTINFO *info);
#define ld4_a_nt1_r1_r3_r2Comb ld4_a_r1_r3_r2Comb
#define ld4_a_nta_r1_r3_r2Comb ld4_a_r1_r3_r2Comb
extern Status ld8_a_r1_r3_r2Comb(INSTINFO *info);
#define ld8_a_nt1_r1_r3_r2Comb ld8_a_r1_r3_r2Comb
#define ld8_a_nta_r1_r3_r2Comb ld8_a_r1_r3_r2Comb
extern Status ld1_sa_r1_r3_r2Comb(INSTINFO *info);
#define ld1_sa_nt1_r1_r3_r2Comb ld1_sa_r1_r3_r2Comb
#define ld1_sa_nta_r1_r3_r2Comb ld1_sa_r1_r3_r2Comb
extern Status ld2_sa_r1_r3_r2Comb(INSTINFO *info);
#define ld2_sa_nt1_r1_r3_r2Comb ld2_sa_r1_r3_r2Comb
#define ld2_sa_nta_r1_r3_r2Comb ld2_sa_r1_r3_r2Comb
extern Status ld4_sa_r1_r3_r2Comb(INSTINFO *info);
#define ld4_sa_nt1_r1_r3_r2Comb ld4_sa_r1_r3_r2Comb
#define ld4_sa_nta_r1_r3_r2Comb ld4_sa_r1_r3_r2Comb
extern Status ld8_sa_r1_r3_r2Comb(INSTINFO *info);
#define ld8_sa_nt1_r1_r3_r2Comb ld8_sa_r1_r3_r2Comb
#define ld8_sa_nta_r1_r3_r2Comb ld8_sa_r1_r3_r2Comb
#define ld1_bias_r1_r3_r2Comb ld1_r1_r3_r2Comb
#define ld1_bias_nt1_r1_r3_r2Comb ld1_bias_r1_r3_r2Comb
#define ld1_bias_nta_r1_r3_r2Comb ld1_bias_r1_r3_r2Comb
#define ld2_bias_r1_r3_r2Comb ld2_r1_r3_r2Comb
#define ld2_bias_nt1_r1_r3_r2Comb ld2_bias_r1_r3_r2Comb
#define ld2_bias_nta_r1_r3_r2Comb ld2_bias_r1_r3_r2Comb
#define ld4_bias_r1_r3_r2Comb ld4_r1_r3_r2Comb
#define ld4_bias_nt1_r1_r3_r2Comb ld4_bias_r1_r3_r2Comb
#define ld4_bias_nta_r1_r3_r2Comb ld4_bias_r1_r3_r2Comb
#define ld8_bias_r1_r3_r2Comb ld8_r1_r3_r2Comb
#define ld8_bias_nt1_r1_r3_r2Comb ld8_bias_r1_r3_r2Comb
#define ld8_bias_nta_r1_r3_r2Comb ld8_bias_r1_r3_r2Comb
#define ld1_acq_r1_r3_r2Comb ld1_r1_r3_r2Comb
#define ld1_acq_nt1_r1_r3_r2Comb ld1_acq_r1_r3_r2Comb
#define ld1_acq_nta_r1_r3_r2Comb ld1_acq_r1_r3_r2Comb
#define ld2_acq_r1_r3_r2Comb ld2_r1_r3_r2Comb
#define ld2_acq_nt1_r1_r3_r2Comb ld2_acq_r1_r3_r2Comb
#define ld2_acq_nta_r1_r3_r2Comb ld2_acq_r1_r3_r2Comb
#define ld4_acq_r1_r3_r2Comb ld4_r1_r3_r2Comb
#define ld4_acq_nt1_r1_r3_r2Comb ld4_acq_r1_r3_r2Comb
#define ld4_acq_nta_r1_r3_r2Comb ld4_acq_r1_r3_r2Comb
#define ld8_acq_r1_r3_r2Comb ld8_r1_r3_r2Comb
#define ld8_acq_nt1_r1_r3_r2Comb ld8_acq_r1_r3_r2Comb
#define ld8_acq_nta_r1_r3_r2Comb ld8_acq_r1_r3_r2Comb
extern Status ld8_fill_r1_r3_r2Comb(INSTINFO *info);
#define ld8_fill_nt1_r1_r3_r2Comb ld8_fill_r1_r3_r2Comb
#define ld8_fill_nta_r1_r3_r2Comb ld8_fill_r1_r3_r2Comb
extern Status ld1_c_clr_r1_r3_r2Comb(INSTINFO *info);
#define ld1_c_clr_nt1_r1_r3_r2Comb ld1_c_clr_r1_r3_r2Comb
#define ld1_c_clr_nta_r1_r3_r2Comb ld1_c_clr_r1_r3_r2Comb
extern Status ld2_c_clr_r1_r3_r2Comb(INSTINFO *info);
#define ld2_c_clr_nt1_r1_r3_r2Comb ld2_c_clr_r1_r3_r2Comb
#define ld2_c_clr_nta_r1_r3_r2Comb ld2_c_clr_r1_r3_r2Comb
extern Status ld4_c_clr_r1_r3_r2Comb(INSTINFO *info);
#define ld4_c_clr_nt1_r1_r3_r2Comb ld4_c_clr_r1_r3_r2Comb
#define ld4_c_clr_nta_r1_r3_r2Comb ld4_c_clr_r1_r3_r2Comb
extern Status ld8_c_clr_r1_r3_r2Comb(INSTINFO *info);
#define ld8_c_clr_nt1_r1_r3_r2Comb ld8_c_clr_r1_r3_r2Comb
#define ld8_c_clr_nta_r1_r3_r2Comb ld8_c_clr_r1_r3_r2Comb
extern Status ld1_c_nc_r1_r3_r2Comb(INSTINFO *info);
#define ld1_c_nc_nt1_r1_r3_r2Comb ld1_c_nc_r1_r3_r2Comb
#define ld1_c_nc_nta_r1_r3_r2Comb ld1_c_nc_r1_r3_r2Comb
extern Status ld2_c_nc_r1_r3_r2Comb(INSTINFO *info);
#define ld2_c_nc_nt1_r1_r3_r2Comb ld2_c_nc_r1_r3_r2Comb
#define ld2_c_nc_nta_r1_r3_r2Comb ld2_c_nc_r1_r3_r2Comb
extern Status ld4_c_nc_r1_r3_r2Comb(INSTINFO *info);
#define ld4_c_nc_nt1_r1_r3_r2Comb ld4_c_nc_r1_r3_r2Comb
#define ld4_c_nc_nta_r1_r3_r2Comb ld4_c_nc_r1_r3_r2Comb
extern Status ld8_c_nc_r1_r3_r2Comb(INSTINFO *info);
#define ld8_c_nc_nt1_r1_r3_r2Comb ld8_c_nc_r1_r3_r2Comb
#define ld8_c_nc_nta_r1_r3_r2Comb ld8_c_nc_r1_r3_r2Comb
#define ld1_c_clr_acq_r1_r3_r2Comb ld1_c_clr_r1_r3_r2Comb
#define ld1_c_clr_acq_nt1_r1_r3_r2Comb ld1_c_clr_acq_r1_r3_r2Comb
#define ld1_c_clr_acq_nta_r1_r3_r2Comb ld1_c_clr_acq_r1_r3_r2Comb
#define ld2_c_clr_acq_r1_r3_r2Comb ld2_c_clr_r1_r3_r2Comb
#define ld2_c_clr_acq_nt1_r1_r3_r2Comb ld2_c_clr_acq_r1_r3_r2Comb
#define ld2_c_clr_acq_nta_r1_r3_r2Comb ld2_c_clr_acq_r1_r3_r2Comb
#define ld4_c_clr_acq_r1_r3_r2Comb ld4_c_clr_r1_r3_r2Comb
#define ld4_c_clr_acq_nt1_r1_r3_r2Comb ld4_c_clr_acq_r1_r3_r2Comb
#define ld4_c_clr_acq_nta_r1_r3_r2Comb ld4_c_clr_acq_r1_r3_r2Comb
#define ld8_c_clr_acq_r1_r3_r2Comb ld8_c_clr_r1_r3_r2Comb
#define ld8_c_clr_acq_nt1_r1_r3_r2Comb ld8_c_clr_acq_r1_r3_r2Comb
#define ld8_c_clr_acq_nta_r1_r3_r2Comb ld8_c_clr_acq_r1_r3_r2Comb
extern Status ld1_r1_r3_imm9Comb(INSTINFO *info);
#define ld1_nt1_r1_r3_imm9Comb ld1_r1_r3_imm9Comb
#define ld1_nta_r1_r3_imm9Comb ld1_r1_r3_imm9Comb
extern Status ld2_r1_r3_imm9Comb(INSTINFO *info);
#define ld2_nt1_r1_r3_imm9Comb ld2_r1_r3_imm9Comb
#define ld2_nta_r1_r3_imm9Comb ld2_r1_r3_imm9Comb
extern Status ld4_r1_r3_imm9Comb(INSTINFO *info);
#define ld4_nt1_r1_r3_imm9Comb ld4_r1_r3_imm9Comb
#define ld4_nta_r1_r3_imm9Comb ld4_r1_r3_imm9Comb
extern Status ld8_r1_r3_imm9Comb(INSTINFO *info);
#define ld8_nt1_r1_r3_imm9Comb ld8_r1_r3_imm9Comb
#define ld8_nta_r1_r3_imm9Comb ld8_r1_r3_imm9Comb
extern Status ld1_s_r1_r3_imm9Comb(INSTINFO *info);
#define ld1_s_nt1_r1_r3_imm9Comb ld1_s_r1_r3_imm9Comb
#define ld1_s_nta_r1_r3_imm9Comb ld1_s_r1_r3_imm9Comb
extern Status ld2_s_r1_r3_imm9Comb(INSTINFO *info);
#define ld2_s_nt1_r1_r3_imm9Comb ld2_s_r1_r3_imm9Comb
#define ld2_s_nta_r1_r3_imm9Comb ld2_s_r1_r3_imm9Comb
extern Status ld4_s_r1_r3_imm9Comb(INSTINFO *info);
#define ld4_s_nt1_r1_r3_imm9Comb ld4_s_r1_r3_imm9Comb
#define ld4_s_nta_r1_r3_imm9Comb ld4_s_r1_r3_imm9Comb
extern Status ld8_s_r1_r3_imm9Comb(INSTINFO *info);
#define ld8_s_nt1_r1_r3_imm9Comb ld8_s_r1_r3_imm9Comb
#define ld8_s_nta_r1_r3_imm9Comb ld8_s_r1_r3_imm9Comb
extern Status ld1_a_r1_r3_imm9Comb(INSTINFO *info);
#define ld1_a_nt1_r1_r3_imm9Comb ld1_a_r1_r3_imm9Comb
#define ld1_a_nta_r1_r3_imm9Comb ld1_a_r1_r3_imm9Comb
extern Status ld2_a_r1_r3_imm9Comb(INSTINFO *info);
#define ld2_a_nt1_r1_r3_imm9Comb ld2_a_r1_r3_imm9Comb
#define ld2_a_nta_r1_r3_imm9Comb ld2_a_r1_r3_imm9Comb
extern Status ld4_a_r1_r3_imm9Comb(INSTINFO *info);
#define ld4_a_nt1_r1_r3_imm9Comb ld4_a_r1_r3_imm9Comb
#define ld4_a_nta_r1_r3_imm9Comb ld4_a_r1_r3_imm9Comb
extern Status ld8_a_r1_r3_imm9Comb(INSTINFO *info);
#define ld8_a_nt1_r1_r3_imm9Comb ld8_a_r1_r3_imm9Comb
#define ld8_a_nta_r1_r3_imm9Comb ld8_a_r1_r3_imm9Comb
extern Status ld1_sa_r1_r3_imm9Comb(INSTINFO *info);
#define ld1_sa_nt1_r1_r3_imm9Comb ld1_sa_r1_r3_imm9Comb
#define ld1_sa_nta_r1_r3_imm9Comb ld1_sa_r1_r3_imm9Comb
extern Status ld2_sa_r1_r3_imm9Comb(INSTINFO *info);
#define ld2_sa_nt1_r1_r3_imm9Comb ld2_sa_r1_r3_imm9Comb
#define ld2_sa_nta_r1_r3_imm9Comb ld2_sa_r1_r3_imm9Comb
extern Status ld4_sa_r1_r3_imm9Comb(INSTINFO *info);
#define ld4_sa_nt1_r1_r3_imm9Comb ld4_sa_r1_r3_imm9Comb
#define ld4_sa_nta_r1_r3_imm9Comb ld4_sa_r1_r3_imm9Comb
extern Status ld8_sa_r1_r3_imm9Comb(INSTINFO *info);
#define ld8_sa_nt1_r1_r3_imm9Comb ld8_sa_r1_r3_imm9Comb
#define ld8_sa_nta_r1_r3_imm9Comb ld8_sa_r1_r3_imm9Comb
#define ld1_bias_r1_r3_imm9Comb ld1_r1_r3_imm9Comb
#define ld1_bias_nt1_r1_r3_imm9Comb ld1_bias_r1_r3_imm9Comb
#define ld1_bias_nta_r1_r3_imm9Comb ld1_bias_r1_r3_imm9Comb
#define ld2_bias_r1_r3_imm9Comb ld2_r1_r3_imm9Comb
#define ld2_bias_nt1_r1_r3_imm9Comb ld2_bias_r1_r3_imm9Comb
#define ld2_bias_nta_r1_r3_imm9Comb ld2_bias_r1_r3_imm9Comb
#define ld4_bias_r1_r3_imm9Comb ld4_r1_r3_imm9Comb
#define ld4_bias_nt1_r1_r3_imm9Comb ld4_bias_r1_r3_imm9Comb
#define ld4_bias_nta_r1_r3_imm9Comb ld4_bias_r1_r3_imm9Comb
#define ld8_bias_r1_r3_imm9Comb ld8_r1_r3_imm9Comb
#define ld8_bias_nt1_r1_r3_imm9Comb ld8_bias_r1_r3_imm9Comb
#define ld8_bias_nta_r1_r3_imm9Comb ld8_bias_r1_r3_imm9Comb
#define ld1_acq_r1_r3_imm9Comb ld1_r1_r3_imm9Comb
#define ld1_acq_nt1_r1_r3_imm9Comb ld1_acq_r1_r3_imm9Comb
#define ld1_acq_nta_r1_r3_imm9Comb ld1_acq_r1_r3_imm9Comb
#define ld2_acq_r1_r3_imm9Comb ld2_r1_r3_imm9Comb
#define ld2_acq_nt1_r1_r3_imm9Comb ld2_acq_r1_r3_imm9Comb
#define ld2_acq_nta_r1_r3_imm9Comb ld2_acq_r1_r3_imm9Comb
#define ld4_acq_r1_r3_imm9Comb ld4_r1_r3_imm9Comb
#define ld4_acq_nt1_r1_r3_imm9Comb ld4_acq_r1_r3_imm9Comb
#define ld4_acq_nta_r1_r3_imm9Comb ld4_acq_r1_r3_imm9Comb
#define ld8_acq_r1_r3_imm9Comb ld8_r1_r3_imm9Comb
#define ld8_acq_nt1_r1_r3_imm9Comb ld8_acq_r1_r3_imm9Comb
#define ld8_acq_nta_r1_r3_imm9Comb ld8_acq_r1_r3_imm9Comb
extern Status ld8_fill_r1_r3_imm9Comb(INSTINFO *info);
#define ld8_fill_nt1_r1_r3_imm9Comb ld8_fill_r1_r3_imm9Comb
#define ld8_fill_nta_r1_r3_imm9Comb ld8_fill_r1_r3_imm9Comb
extern Status ld1_c_clr_r1_r3_imm9Comb(INSTINFO *info);
#define ld1_c_clr_nt1_r1_r3_imm9Comb ld1_c_clr_r1_r3_imm9Comb
#define ld1_c_clr_nta_r1_r3_imm9Comb ld1_c_clr_r1_r3_imm9Comb
extern Status ld2_c_clr_r1_r3_imm9Comb(INSTINFO *info);
#define ld2_c_clr_nt1_r1_r3_imm9Comb ld2_c_clr_r1_r3_imm9Comb
#define ld2_c_clr_nta_r1_r3_imm9Comb ld2_c_clr_r1_r3_imm9Comb
extern Status ld4_c_clr_r1_r3_imm9Comb(INSTINFO *info);
#define ld4_c_clr_nt1_r1_r3_imm9Comb ld4_c_clr_r1_r3_imm9Comb
#define ld4_c_clr_nta_r1_r3_imm9Comb ld4_c_clr_r1_r3_imm9Comb
extern Status ld8_c_clr_r1_r3_imm9Comb(INSTINFO *info);
#define ld8_c_clr_nt1_r1_r3_imm9Comb ld8_c_clr_r1_r3_imm9Comb
#define ld8_c_clr_nta_r1_r3_imm9Comb ld8_c_clr_r1_r3_imm9Comb
extern Status ld1_c_nc_r1_r3_imm9Comb(INSTINFO *info);
#define ld1_c_nc_nt1_r1_r3_imm9Comb ld1_c_nc_r1_r3_imm9Comb
#define ld1_c_nc_nta_r1_r3_imm9Comb ld1_c_nc_r1_r3_imm9Comb
extern Status ld2_c_nc_r1_r3_imm9Comb(INSTINFO *info);
#define ld2_c_nc_nt1_r1_r3_imm9Comb ld2_c_nc_r1_r3_imm9Comb
#define ld2_c_nc_nta_r1_r3_imm9Comb ld2_c_nc_r1_r3_imm9Comb
extern Status ld4_c_nc_r1_r3_imm9Comb(INSTINFO *info);
#define ld4_c_nc_nt1_r1_r3_imm9Comb ld4_c_nc_r1_r3_imm9Comb
#define ld4_c_nc_nta_r1_r3_imm9Comb ld4_c_nc_r1_r3_imm9Comb
extern Status ld8_c_nc_r1_r3_imm9Comb(INSTINFO *info);
#define ld8_c_nc_nt1_r1_r3_imm9Comb ld8_c_nc_r1_r3_imm9Comb
#define ld8_c_nc_nta_r1_r3_imm9Comb ld8_c_nc_r1_r3_imm9Comb
#define ld1_c_clr_acq_r1_r3_imm9Comb ld1_c_clr_r1_r3_imm9Comb
#define ld1_c_clr_acq_nt1_r1_r3_imm9Comb ld1_c_clr_acq_r1_r3_imm9Comb
#define ld1_c_clr_acq_nta_r1_r3_imm9Comb ld1_c_clr_acq_r1_r3_imm9Comb
#define ld2_c_clr_acq_r1_r3_imm9Comb ld2_c_clr_r1_r3_imm9Comb
#define ld2_c_clr_acq_nt1_r1_r3_imm9Comb ld2_c_clr_acq_r1_r3_imm9Comb
#define ld2_c_clr_acq_nta_r1_r3_imm9Comb ld2_c_clr_acq_r1_r3_imm9Comb
#define ld4_c_clr_acq_r1_r3_imm9Comb ld4_c_clr_r1_r3_imm9Comb
#define ld4_c_clr_acq_nt1_r1_r3_imm9Comb ld4_c_clr_acq_r1_r3_imm9Comb
#define ld4_c_clr_acq_nta_r1_r3_imm9Comb ld4_c_clr_acq_r1_r3_imm9Comb
#define ld8_c_clr_acq_r1_r3_imm9Comb ld8_c_clr_r1_r3_imm9Comb
#define ld8_c_clr_acq_nt1_r1_r3_imm9Comb ld8_c_clr_acq_r1_r3_imm9Comb
#define ld8_c_clr_acq_nta_r1_r3_imm9Comb ld8_c_clr_acq_r1_r3_imm9Comb
extern Status st1_r3_r2Comb(INSTINFO *info);
#define st1_nta_r3_r2Comb st1_r3_r2Comb
extern Status st2_r3_r2Comb(INSTINFO *info);
#define st2_nta_r3_r2Comb st2_r3_r2Comb
extern Status st4_r3_r2Comb(INSTINFO *info);
#define st4_nta_r3_r2Comb st4_r3_r2Comb
extern Status st8_r3_r2Comb(INSTINFO *info);
#define st8_nta_r3_r2Comb st8_r3_r2Comb
extern Status st16_r3_r2_ar_csdComb(INSTINFO *info);
#define st16_nta_r3_r2_ar_csdComb st16_r3_r2_ar_csdComb
#define st1_rel_r3_r2Comb st1_r3_r2Comb
#define st1_rel_nta_r3_r2Comb st1_rel_r3_r2Comb
#define st2_rel_r3_r2Comb st2_r3_r2Comb
#define st2_rel_nta_r3_r2Comb st2_rel_r3_r2Comb
#define st4_rel_r3_r2Comb st4_r3_r2Comb
#define st4_rel_nta_r3_r2Comb st4_rel_r3_r2Comb
#define st8_rel_r3_r2Comb st8_r3_r2Comb
#define st8_rel_nta_r3_r2Comb st8_rel_r3_r2Comb
#define st16_rel_r3_r2_ar_csdComb st16_r3_r2_ar_csdComb
#define st16_rel_nta_r3_r2_ar_csdComb st16_r3_r2_ar_csdComb
extern Status st8_spill_r3_r2Comb(INSTINFO *info);
#define st8_spill_nta_r3_r2Comb st8_spill_r3_r2Comb
extern Status st1_r3_r2_imm9Comb(INSTINFO *info);
#define st1_nta_r3_r2_imm9Comb st1_r3_r2_imm9Comb
extern Status st2_r3_r2_imm9Comb(INSTINFO *info);
#define st2_nta_r3_r2_imm9Comb st2_r3_r2_imm9Comb
extern Status st4_r3_r2_imm9Comb(INSTINFO *info);
#define st4_nta_r3_r2_imm9Comb st4_r3_r2_imm9Comb
extern Status st8_r3_r2_imm9Comb(INSTINFO *info);
#define st8_nta_r3_r2_imm9Comb st8_r3_r2_imm9Comb
#define st1_rel_r3_r2_imm9Comb st1_r3_r2_imm9Comb
#define st1_rel_nta_r3_r2_imm9Comb st1_rel_r3_r2_imm9Comb
#define st2_rel_r3_r2_imm9Comb st2_r3_r2_imm9Comb
#define st2_rel_nta_r3_r2_imm9Comb st2_rel_r3_r2_imm9Comb
#define st4_rel_r3_r2_imm9Comb st4_r3_r2_imm9Comb
#define st4_rel_nta_r3_r2_imm9Comb st4_rel_r3_r2_imm9Comb
#define st8_rel_r3_r2_imm9Comb st8_r3_r2_imm9Comb
#define st8_rel_nta_r3_r2_imm9Comb st8_rel_r3_r2_imm9Comb
extern Status st8_spill_r3_r2_imm9Comb(INSTINFO *info);
#define st8_spill_nta_r3_r2_imm9Comb st8_spill_r3_r2_imm9Comb
extern Status ldfs_f1_r3Comb(INSTINFO *info);
#define ldfs_nt1_f1_r3Comb ldfs_f1_r3Comb
#define ldfs_nta_f1_r3Comb ldfs_f1_r3Comb
extern Status ldfd_f1_r3Comb(INSTINFO *info);
#define ldfd_nt1_f1_r3Comb ldfd_f1_r3Comb
#define ldfd_nta_f1_r3Comb ldfd_f1_r3Comb
extern Status ldf8_f1_r3Comb(INSTINFO *info);
#define ldf8_nt1_f1_r3Comb ldf8_f1_r3Comb
#define ldf8_nta_f1_r3Comb ldf8_f1_r3Comb
extern Status ldfe_f1_r3Comb(INSTINFO *info);
#define ldfe_nt1_f1_r3Comb ldfe_f1_r3Comb
#define ldfe_nta_f1_r3Comb ldfe_f1_r3Comb
extern Status ldfs_s_f1_r3Comb(INSTINFO *info);
#define ldfs_s_nt1_f1_r3Comb ldfs_s_f1_r3Comb
#define ldfs_s_nta_f1_r3Comb ldfs_s_f1_r3Comb
extern Status ldfd_s_f1_r3Comb(INSTINFO *info);
#define ldfd_s_nt1_f1_r3Comb ldfd_s_f1_r3Comb
#define ldfd_s_nta_f1_r3Comb ldfd_s_f1_r3Comb
extern Status ldf8_s_f1_r3Comb(INSTINFO *info);
#define ldf8_s_nt1_f1_r3Comb ldf8_s_f1_r3Comb
#define ldf8_s_nta_f1_r3Comb ldf8_s_f1_r3Comb
extern Status ldfe_s_f1_r3Comb(INSTINFO *info);
#define ldfe_s_nt1_f1_r3Comb ldfe_s_f1_r3Comb
#define ldfe_s_nta_f1_r3Comb ldfe_s_f1_r3Comb
extern Status ldfs_a_f1_r3Comb(INSTINFO *info);
#define ldfs_a_nt1_f1_r3Comb ldfs_a_f1_r3Comb
#define ldfs_a_nta_f1_r3Comb ldfs_a_f1_r3Comb
extern Status ldfd_a_f1_r3Comb(INSTINFO *info);
#define ldfd_a_nt1_f1_r3Comb ldfd_a_f1_r3Comb
#define ldfd_a_nta_f1_r3Comb ldfd_a_f1_r3Comb
extern Status ldf8_a_f1_r3Comb(INSTINFO *info);
#define ldf8_a_nt1_f1_r3Comb ldf8_a_f1_r3Comb
#define ldf8_a_nta_f1_r3Comb ldf8_a_f1_r3Comb
extern Status ldfe_a_f1_r3Comb(INSTINFO *info);
#define ldfe_a_nt1_f1_r3Comb ldfe_a_f1_r3Comb
#define ldfe_a_nta_f1_r3Comb ldfe_a_f1_r3Comb
extern Status ldfs_sa_f1_r3Comb(INSTINFO *info);
#define ldfs_sa_nt1_f1_r3Comb ldfs_sa_f1_r3Comb
#define ldfs_sa_nta_f1_r3Comb ldfs_sa_f1_r3Comb
extern Status ldfd_sa_f1_r3Comb(INSTINFO *info);
#define ldfd_sa_nt1_f1_r3Comb ldfd_sa_f1_r3Comb
#define ldfd_sa_nta_f1_r3Comb ldfd_sa_f1_r3Comb
extern Status ldf8_sa_f1_r3Comb(INSTINFO *info);
#define ldf8_sa_nt1_f1_r3Comb ldf8_sa_f1_r3Comb
#define ldf8_sa_nta_f1_r3Comb ldf8_sa_f1_r3Comb
extern Status ldfe_sa_f1_r3Comb(INSTINFO *info);
#define ldfe_sa_nt1_f1_r3Comb ldfe_sa_f1_r3Comb
#define ldfe_sa_nta_f1_r3Comb ldfe_sa_f1_r3Comb
extern Status ldf_fill_f1_r3Comb(INSTINFO *info);
#define ldf_fill_nt1_f1_r3Comb ldf_fill_f1_r3Comb
#define ldf_fill_nta_f1_r3Comb ldf_fill_f1_r3Comb
extern Status ldfs_c_clr_f1_r3Comb(INSTINFO *info);
#define ldfs_c_clr_nt1_f1_r3Comb ldfs_c_clr_f1_r3Comb
#define ldfs_c_clr_nta_f1_r3Comb ldfs_c_clr_f1_r3Comb
extern Status ldfd_c_clr_f1_r3Comb(INSTINFO *info);
#define ldfd_c_clr_nt1_f1_r3Comb ldfd_c_clr_f1_r3Comb
#define ldfd_c_clr_nta_f1_r3Comb ldfd_c_clr_f1_r3Comb
extern Status ldf8_c_clr_f1_r3Comb(INSTINFO *info);
#define ldf8_c_clr_nt1_f1_r3Comb ldf8_c_clr_f1_r3Comb
#define ldf8_c_clr_nta_f1_r3Comb ldf8_c_clr_f1_r3Comb
extern Status ldfe_c_clr_f1_r3Comb(INSTINFO *info);
#define ldfe_c_clr_nt1_f1_r3Comb ldfe_c_clr_f1_r3Comb
#define ldfe_c_clr_nta_f1_r3Comb ldfe_c_clr_f1_r3Comb
extern Status ldfs_c_nc_f1_r3Comb(INSTINFO *info);
#define ldfs_c_nc_nt1_f1_r3Comb ldfs_c_nc_f1_r3Comb
#define ldfs_c_nc_nta_f1_r3Comb ldfs_c_nc_f1_r3Comb
extern Status ldfd_c_nc_f1_r3Comb(INSTINFO *info);
#define ldfd_c_nc_nt1_f1_r3Comb ldfd_c_nc_f1_r3Comb
#define ldfd_c_nc_nta_f1_r3Comb ldfd_c_nc_f1_r3Comb
extern Status ldf8_c_nc_f1_r3Comb(INSTINFO *info);
#define ldf8_c_nc_nt1_f1_r3Comb ldf8_c_nc_f1_r3Comb
#define ldf8_c_nc_nta_f1_r3Comb ldf8_c_nc_f1_r3Comb
extern Status ldfe_c_nc_f1_r3Comb(INSTINFO *info);
#define ldfe_c_nc_nt1_f1_r3Comb ldfe_c_nc_f1_r3Comb
#define ldfe_c_nc_nta_f1_r3Comb ldfe_c_nc_f1_r3Comb
extern Status ldfs_f1_r3_r2Comb(INSTINFO *info);
#define ldfs_nt1_f1_r3_r2Comb ldfs_f1_r3_r2Comb
#define ldfs_nta_f1_r3_r2Comb ldfs_f1_r3_r2Comb
extern Status ldfd_f1_r3_r2Comb(INSTINFO *info);
#define ldfd_nt1_f1_r3_r2Comb ldfd_f1_r3_r2Comb
#define ldfd_nta_f1_r3_r2Comb ldfd_f1_r3_r2Comb
extern Status ldf8_f1_r3_r2Comb(INSTINFO *info);
#define ldf8_nt1_f1_r3_r2Comb ldf8_f1_r3_r2Comb
#define ldf8_nta_f1_r3_r2Comb ldf8_f1_r3_r2Comb
extern Status ldfe_f1_r3_r2Comb(INSTINFO *info);
#define ldfe_nt1_f1_r3_r2Comb ldfe_f1_r3_r2Comb
#define ldfe_nta_f1_r3_r2Comb ldfe_f1_r3_r2Comb
extern Status ldfs_s_f1_r3_r2Comb(INSTINFO *info);
#define ldfs_s_nt1_f1_r3_r2Comb ldfs_s_f1_r3_r2Comb
#define ldfs_s_nta_f1_r3_r2Comb ldfs_s_f1_r3_r2Comb
extern Status ldfd_s_f1_r3_r2Comb(INSTINFO *info);
#define ldfd_s_nt1_f1_r3_r2Comb ldfd_s_f1_r3_r2Comb
#define ldfd_s_nta_f1_r3_r2Comb ldfd_s_f1_r3_r2Comb
extern Status ldf8_s_f1_r3_r2Comb(INSTINFO *info);
#define ldf8_s_nt1_f1_r3_r2Comb ldf8_s_f1_r3_r2Comb
#define ldf8_s_nta_f1_r3_r2Comb ldf8_s_f1_r3_r2Comb
extern Status ldfe_s_f1_r3_r2Comb(INSTINFO *info);
#define ldfe_s_nt1_f1_r3_r2Comb ldfe_s_f1_r3_r2Comb
#define ldfe_s_nta_f1_r3_r2Comb ldfe_s_f1_r3_r2Comb
extern Status ldfs_a_f1_r3_r2Comb(INSTINFO *info);
#define ldfs_a_nt1_f1_r3_r2Comb ldfs_a_f1_r3_r2Comb
#define ldfs_a_nta_f1_r3_r2Comb ldfs_a_f1_r3_r2Comb
extern Status ldfd_a_f1_r3_r2Comb(INSTINFO *info);
#define ldfd_a_nt1_f1_r3_r2Comb ldfd_a_f1_r3_r2Comb
#define ldfd_a_nta_f1_r3_r2Comb ldfd_a_f1_r3_r2Comb
extern Status ldf8_a_f1_r3_r2Comb(INSTINFO *info);
#define ldf8_a_nt1_f1_r3_r2Comb ldf8_a_f1_r3_r2Comb
#define ldf8_a_nta_f1_r3_r2Comb ldf8_a_f1_r3_r2Comb
extern Status ldfe_a_f1_r3_r2Comb(INSTINFO *info);
#define ldfe_a_nt1_f1_r3_r2Comb ldfe_a_f1_r3_r2Comb
#define ldfe_a_nta_f1_r3_r2Comb ldfe_a_f1_r3_r2Comb
extern Status ldfs_sa_f1_r3_r2Comb(INSTINFO *info);
#define ldfs_sa_nt1_f1_r3_r2Comb ldfs_sa_f1_r3_r2Comb
#define ldfs_sa_nta_f1_r3_r2Comb ldfs_sa_f1_r3_r2Comb
extern Status ldfd_sa_f1_r3_r2Comb(INSTINFO *info);
#define ldfd_sa_nt1_f1_r3_r2Comb ldfd_sa_f1_r3_r2Comb
#define ldfd_sa_nta_f1_r3_r2Comb ldfd_sa_f1_r3_r2Comb
extern Status ldf8_sa_f1_r3_r2Comb(INSTINFO *info);
#define ldf8_sa_nt1_f1_r3_r2Comb ldf8_sa_f1_r3_r2Comb
#define ldf8_sa_nta_f1_r3_r2Comb ldf8_sa_f1_r3_r2Comb
extern Status ldfe_sa_f1_r3_r2Comb(INSTINFO *info);
#define ldfe_sa_nt1_f1_r3_r2Comb ldfe_sa_f1_r3_r2Comb
#define ldfe_sa_nta_f1_r3_r2Comb ldfe_sa_f1_r3_r2Comb
extern Status ldf_fill_f1_r3_r2Comb(INSTINFO *info);
#define ldf_fill_nt1_f1_r3_r2Comb ldf_fill_f1_r3_r2Comb
#define ldf_fill_nta_f1_r3_r2Comb ldf_fill_f1_r3_r2Comb
extern Status ldfs_c_clr_f1_r3_r2Comb(INSTINFO *info);
#define ldfs_c_clr_nt1_f1_r3_r2Comb ldfs_c_clr_f1_r3_r2Comb
#define ldfs_c_clr_nta_f1_r3_r2Comb ldfs_c_clr_f1_r3_r2Comb
extern Status ldfd_c_clr_f1_r3_r2Comb(INSTINFO *info);
#define ldfd_c_clr_nt1_f1_r3_r2Comb ldfd_c_clr_f1_r3_r2Comb
#define ldfd_c_clr_nta_f1_r3_r2Comb ldfd_c_clr_f1_r3_r2Comb
extern Status ldf8_c_clr_f1_r3_r2Comb(INSTINFO *info);
#define ldf8_c_clr_nt1_f1_r3_r2Comb ldf8_c_clr_f1_r3_r2Comb
#define ldf8_c_clr_nta_f1_r3_r2Comb ldf8_c_clr_f1_r3_r2Comb
extern Status ldfe_c_clr_f1_r3_r2Comb(INSTINFO *info);
#define ldfe_c_clr_nt1_f1_r3_r2Comb ldfe_c_clr_f1_r3_r2Comb
#define ldfe_c_clr_nta_f1_r3_r2Comb ldfe_c_clr_f1_r3_r2Comb
extern Status ldfs_c_nc_f1_r3_r2Comb(INSTINFO *info);
#define ldfs_c_nc_nt1_f1_r3_r2Comb ldfs_c_nc_f1_r3_r2Comb
#define ldfs_c_nc_nta_f1_r3_r2Comb ldfs_c_nc_f1_r3_r2Comb
extern Status ldfd_c_nc_f1_r3_r2Comb(INSTINFO *info);
#define ldfd_c_nc_nt1_f1_r3_r2Comb ldfd_c_nc_f1_r3_r2Comb
#define ldfd_c_nc_nta_f1_r3_r2Comb ldfd_c_nc_f1_r3_r2Comb
extern Status ldf8_c_nc_f1_r3_r2Comb(INSTINFO *info);
#define ldf8_c_nc_nt1_f1_r3_r2Comb ldf8_c_nc_f1_r3_r2Comb
#define ldf8_c_nc_nta_f1_r3_r2Comb ldf8_c_nc_f1_r3_r2Comb
extern Status ldfe_c_nc_f1_r3_r2Comb(INSTINFO *info);
#define ldfe_c_nc_nt1_f1_r3_r2Comb ldfe_c_nc_f1_r3_r2Comb
#define ldfe_c_nc_nta_f1_r3_r2Comb ldfe_c_nc_f1_r3_r2Comb
extern Status ldfs_f1_r3_imm9Comb(INSTINFO *info);
#define ldfs_nt1_f1_r3_imm9Comb ldfs_f1_r3_imm9Comb
#define ldfs_nta_f1_r3_imm9Comb ldfs_f1_r3_imm9Comb
extern Status ldfd_f1_r3_imm9Comb(INSTINFO *info);
#define ldfd_nt1_f1_r3_imm9Comb ldfd_f1_r3_imm9Comb
#define ldfd_nta_f1_r3_imm9Comb ldfd_f1_r3_imm9Comb
extern Status ldf8_f1_r3_imm9Comb(INSTINFO *info);
#define ldf8_nt1_f1_r3_imm9Comb ldf8_f1_r3_imm9Comb
#define ldf8_nta_f1_r3_imm9Comb ldf8_f1_r3_imm9Comb
extern Status ldfe_f1_r3_imm9Comb(INSTINFO *info);
#define ldfe_nt1_f1_r3_imm9Comb ldfe_f1_r3_imm9Comb
#define ldfe_nta_f1_r3_imm9Comb ldfe_f1_r3_imm9Comb
extern Status ldfs_s_f1_r3_imm9Comb(INSTINFO *info);
#define ldfs_s_nt1_f1_r3_imm9Comb ldfs_s_f1_r3_imm9Comb
#define ldfs_s_nta_f1_r3_imm9Comb ldfs_s_f1_r3_imm9Comb
extern Status ldfd_s_f1_r3_imm9Comb(INSTINFO *info);
#define ldfd_s_nt1_f1_r3_imm9Comb ldfd_s_f1_r3_imm9Comb
#define ldfd_s_nta_f1_r3_imm9Comb ldfd_s_f1_r3_imm9Comb
extern Status ldf8_s_f1_r3_imm9Comb(INSTINFO *info);
#define ldf8_s_nt1_f1_r3_imm9Comb ldf8_s_f1_r3_imm9Comb
#define ldf8_s_nta_f1_r3_imm9Comb ldf8_s_f1_r3_imm9Comb
extern Status ldfe_s_f1_r3_imm9Comb(INSTINFO *info);
#define ldfe_s_nt1_f1_r3_imm9Comb ldfe_s_f1_r3_imm9Comb
#define ldfe_s_nta_f1_r3_imm9Comb ldfe_s_f1_r3_imm9Comb
extern Status ldfs_a_f1_r3_imm9Comb(INSTINFO *info);
#define ldfs_a_nt1_f1_r3_imm9Comb ldfs_a_f1_r3_imm9Comb
#define ldfs_a_nta_f1_r3_imm9Comb ldfs_a_f1_r3_imm9Comb
extern Status ldfd_a_f1_r3_imm9Comb(INSTINFO *info);
#define ldfd_a_nt1_f1_r3_imm9Comb ldfd_a_f1_r3_imm9Comb
#define ldfd_a_nta_f1_r3_imm9Comb ldfd_a_f1_r3_imm9Comb
extern Status ldf8_a_f1_r3_imm9Comb(INSTINFO *info);
#define ldf8_a_nt1_f1_r3_imm9Comb ldf8_a_f1_r3_imm9Comb
#define ldf8_a_nta_f1_r3_imm9Comb ldf8_a_f1_r3_imm9Comb
extern Status ldfe_a_f1_r3_imm9Comb(INSTINFO *info);
#define ldfe_a_nt1_f1_r3_imm9Comb ldfe_a_f1_r3_imm9Comb
#define ldfe_a_nta_f1_r3_imm9Comb ldfe_a_f1_r3_imm9Comb
extern Status ldfs_sa_f1_r3_imm9Comb(INSTINFO *info);
#define ldfs_sa_nt1_f1_r3_imm9Comb ldfs_sa_f1_r3_imm9Comb
#define ldfs_sa_nta_f1_r3_imm9Comb ldfs_sa_f1_r3_imm9Comb
extern Status ldfd_sa_f1_r3_imm9Comb(INSTINFO *info);
#define ldfd_sa_nt1_f1_r3_imm9Comb ldfd_sa_f1_r3_imm9Comb
#define ldfd_sa_nta_f1_r3_imm9Comb ldfd_sa_f1_r3_imm9Comb
extern Status ldf8_sa_f1_r3_imm9Comb(INSTINFO *info);
#define ldf8_sa_nt1_f1_r3_imm9Comb ldf8_sa_f1_r3_imm9Comb
#define ldf8_sa_nta_f1_r3_imm9Comb ldf8_sa_f1_r3_imm9Comb
extern Status ldfe_sa_f1_r3_imm9Comb(INSTINFO *info);
#define ldfe_sa_nt1_f1_r3_imm9Comb ldfe_sa_f1_r3_imm9Comb
#define ldfe_sa_nta_f1_r3_imm9Comb ldfe_sa_f1_r3_imm9Comb
extern Status ldf_fill_f1_r3_imm9Comb(INSTINFO *info);
#define ldf_fill_nt1_f1_r3_imm9Comb ldf_fill_f1_r3_imm9Comb
#define ldf_fill_nta_f1_r3_imm9Comb ldf_fill_f1_r3_imm9Comb
extern Status ldfs_c_clr_f1_r3_imm9Comb(INSTINFO *info);
#define ldfs_c_clr_nt1_f1_r3_imm9Comb ldfs_c_clr_f1_r3_imm9Comb
#define ldfs_c_clr_nta_f1_r3_imm9Comb ldfs_c_clr_f1_r3_imm9Comb
extern Status ldfd_c_clr_f1_r3_imm9Comb(INSTINFO *info);
#define ldfd_c_clr_nt1_f1_r3_imm9Comb ldfd_c_clr_f1_r3_imm9Comb
#define ldfd_c_clr_nta_f1_r3_imm9Comb ldfd_c_clr_f1_r3_imm9Comb
extern Status ldf8_c_clr_f1_r3_imm9Comb(INSTINFO *info);
#define ldf8_c_clr_nt1_f1_r3_imm9Comb ldf8_c_clr_f1_r3_imm9Comb
#define ldf8_c_clr_nta_f1_r3_imm9Comb ldf8_c_clr_f1_r3_imm9Comb
extern Status ldfe_c_clr_f1_r3_imm9Comb(INSTINFO *info);
#define ldfe_c_clr_nt1_f1_r3_imm9Comb ldfe_c_clr_f1_r3_imm9Comb
#define ldfe_c_clr_nta_f1_r3_imm9Comb ldfe_c_clr_f1_r3_imm9Comb
extern Status ldfs_c_nc_f1_r3_imm9Comb(INSTINFO *info);
#define ldfs_c_nc_nt1_f1_r3_imm9Comb ldfs_c_nc_f1_r3_imm9Comb
#define ldfs_c_nc_nta_f1_r3_imm9Comb ldfs_c_nc_f1_r3_imm9Comb
extern Status ldfd_c_nc_f1_r3_imm9Comb(INSTINFO *info);
#define ldfd_c_nc_nt1_f1_r3_imm9Comb ldfd_c_nc_f1_r3_imm9Comb
#define ldfd_c_nc_nta_f1_r3_imm9Comb ldfd_c_nc_f1_r3_imm9Comb
extern Status ldf8_c_nc_f1_r3_imm9Comb(INSTINFO *info);
#define ldf8_c_nc_nt1_f1_r3_imm9Comb ldf8_c_nc_f1_r3_imm9Comb
#define ldf8_c_nc_nta_f1_r3_imm9Comb ldf8_c_nc_f1_r3_imm9Comb
extern Status ldfe_c_nc_f1_r3_imm9Comb(INSTINFO *info);
#define ldfe_c_nc_nt1_f1_r3_imm9Comb ldfe_c_nc_f1_r3_imm9Comb
#define ldfe_c_nc_nta_f1_r3_imm9Comb ldfe_c_nc_f1_r3_imm9Comb
extern Status stfs_r3_f2Comb(INSTINFO *info);
#define stfs_nta_r3_f2Comb stfs_r3_f2Comb
extern Status stfd_r3_f2Comb(INSTINFO *info);
#define stfd_nta_r3_f2Comb stfd_r3_f2Comb
extern Status stf8_r3_f2Comb(INSTINFO *info);
#define stf8_nta_r3_f2Comb stf8_r3_f2Comb
extern Status stfe_r3_f2Comb(INSTINFO *info);
#define stfe_nta_r3_f2Comb stfe_r3_f2Comb
extern Status stf_spill_r3_f2Comb(INSTINFO *info);
#define stf_spill_nta_r3_f2Comb stf_spill_r3_f2Comb
extern Status stfs_r3_f2_imm9Comb(INSTINFO *info);
#define stfs_nta_r3_f2_imm9Comb stfs_r3_f2_imm9Comb
extern Status stfd_r3_f2_imm9Comb(INSTINFO *info);
#define stfd_nta_r3_f2_imm9Comb stfd_r3_f2_imm9Comb
extern Status stf8_r3_f2_imm9Comb(INSTINFO *info);
#define stf8_nta_r3_f2_imm9Comb stf8_r3_f2_imm9Comb
extern Status stfe_r3_f2_imm9Comb(INSTINFO *info);
#define stfe_nta_r3_f2_imm9Comb stfe_r3_f2_imm9Comb
extern Status stf_spill_r3_f2_imm9Comb(INSTINFO *info);
#define stf_spill_nta_r3_f2_imm9Comb stf_spill_r3_f2_imm9Comb
extern Status ldfps_f1_f2_r3Comb(INSTINFO *info);
#define ldfps_nt1_f1_f2_r3Comb ldfps_f1_f2_r3Comb
#define ldfps_nta_f1_f2_r3Comb ldfps_f1_f2_r3Comb
extern Status ldfpd_f1_f2_r3Comb(INSTINFO *info);
#define ldfpd_nt1_f1_f2_r3Comb ldfpd_f1_f2_r3Comb
#define ldfpd_nta_f1_f2_r3Comb ldfpd_f1_f2_r3Comb
extern Status ldfp8_f1_f2_r3Comb(INSTINFO *info);
#define ldfp8_nt1_f1_f2_r3Comb ldfp8_f1_f2_r3Comb
#define ldfp8_nta_f1_f2_r3Comb ldfp8_f1_f2_r3Comb
extern Status ldfps_s_f1_f2_r3Comb(INSTINFO *info);
#define ldfps_s_nt1_f1_f2_r3Comb ldfps_s_f1_f2_r3Comb
#define ldfps_s_nta_f1_f2_r3Comb ldfps_s_f1_f2_r3Comb
extern Status ldfpd_s_f1_f2_r3Comb(INSTINFO *info);
#define ldfpd_s_nt1_f1_f2_r3Comb ldfpd_s_f1_f2_r3Comb
#define ldfpd_s_nta_f1_f2_r3Comb ldfpd_s_f1_f2_r3Comb
extern Status ldfp8_s_f1_f2_r3Comb(INSTINFO *info);
#define ldfp8_s_nt1_f1_f2_r3Comb ldfp8_s_f1_f2_r3Comb
#define ldfp8_s_nta_f1_f2_r3Comb ldfp8_s_f1_f2_r3Comb
extern Status ldfps_a_f1_f2_r3Comb(INSTINFO *info);
#define ldfps_a_nt1_f1_f2_r3Comb ldfps_a_f1_f2_r3Comb
#define ldfps_a_nta_f1_f2_r3Comb ldfps_a_f1_f2_r3Comb
extern Status ldfpd_a_f1_f2_r3Comb(INSTINFO *info);
#define ldfpd_a_nt1_f1_f2_r3Comb ldfpd_a_f1_f2_r3Comb
#define ldfpd_a_nta_f1_f2_r3Comb ldfpd_a_f1_f2_r3Comb
extern Status ldfp8_a_f1_f2_r3Comb(INSTINFO *info);
#define ldfp8_a_nt1_f1_f2_r3Comb ldfp8_a_f1_f2_r3Comb
#define ldfp8_a_nta_f1_f2_r3Comb ldfp8_a_f1_f2_r3Comb
extern Status ldfps_sa_f1_f2_r3Comb(INSTINFO *info);
#define ldfps_sa_nt1_f1_f2_r3Comb ldfps_sa_f1_f2_r3Comb
#define ldfps_sa_nta_f1_f2_r3Comb ldfps_sa_f1_f2_r3Comb
extern Status ldfpd_sa_f1_f2_r3Comb(INSTINFO *info);
#define ldfpd_sa_nt1_f1_f2_r3Comb ldfpd_sa_f1_f2_r3Comb
#define ldfpd_sa_nta_f1_f2_r3Comb ldfpd_sa_f1_f2_r3Comb
extern Status ldfp8_sa_f1_f2_r3Comb(INSTINFO *info);
#define ldfp8_sa_nt1_f1_f2_r3Comb ldfp8_sa_f1_f2_r3Comb
#define ldfp8_sa_nta_f1_f2_r3Comb ldfp8_sa_f1_f2_r3Comb
extern Status ldfps_c_clr_f1_f2_r3Comb(INSTINFO *info);
#define ldfps_c_clr_nt1_f1_f2_r3Comb ldfps_c_clr_f1_f2_r3Comb
#define ldfps_c_clr_nta_f1_f2_r3Comb ldfps_c_clr_f1_f2_r3Comb
extern Status ldfpd_c_clr_f1_f2_r3Comb(INSTINFO *info);
#define ldfpd_c_clr_nt1_f1_f2_r3Comb ldfpd_c_clr_f1_f2_r3Comb
#define ldfpd_c_clr_nta_f1_f2_r3Comb ldfpd_c_clr_f1_f2_r3Comb
extern Status ldfp8_c_clr_f1_f2_r3Comb(INSTINFO *info);
#define ldfp8_c_clr_nt1_f1_f2_r3Comb ldfp8_c_clr_f1_f2_r3Comb
#define ldfp8_c_clr_nta_f1_f2_r3Comb ldfp8_c_clr_f1_f2_r3Comb
extern Status ldfps_c_nc_f1_f2_r3Comb(INSTINFO *info);
#define ldfps_c_nc_nt1_f1_f2_r3Comb ldfps_c_nc_f1_f2_r3Comb
#define ldfps_c_nc_nta_f1_f2_r3Comb ldfps_c_nc_f1_f2_r3Comb
extern Status ldfpd_c_nc_f1_f2_r3Comb(INSTINFO *info);
#define ldfpd_c_nc_nt1_f1_f2_r3Comb ldfpd_c_nc_f1_f2_r3Comb
#define ldfpd_c_nc_nta_f1_f2_r3Comb ldfpd_c_nc_f1_f2_r3Comb
extern Status ldfp8_c_nc_f1_f2_r3Comb(INSTINFO *info);
#define ldfp8_c_nc_nt1_f1_f2_r3Comb ldfp8_c_nc_f1_f2_r3Comb
#define ldfp8_c_nc_nta_f1_f2_r3Comb ldfp8_c_nc_f1_f2_r3Comb
extern Status ldfps_f1_f2_r3_8Comb(INSTINFO *info);
#define ldfps_nt1_f1_f2_r3_8Comb ldfps_f1_f2_r3_8Comb
#define ldfps_nta_f1_f2_r3_8Comb ldfps_f1_f2_r3_8Comb
extern Status ldfpd_f1_f2_r3_16Comb(INSTINFO *info);
#define ldfpd_nt1_f1_f2_r3_16Comb ldfpd_f1_f2_r3_16Comb
#define ldfpd_nta_f1_f2_r3_16Comb ldfpd_f1_f2_r3_16Comb
extern Status ldfp8_f1_f2_r3_16Comb(INSTINFO *info);
#define ldfp8_nt1_f1_f2_r3_16Comb ldfp8_f1_f2_r3_16Comb
#define ldfp8_nta_f1_f2_r3_16Comb ldfp8_f1_f2_r3_16Comb
extern Status ldfps_s_f1_f2_r3_8Comb(INSTINFO *info);
#define ldfps_s_nt1_f1_f2_r3_8Comb ldfps_s_f1_f2_r3_8Comb
#define ldfps_s_nta_f1_f2_r3_8Comb ldfps_s_f1_f2_r3_8Comb
extern Status ldfpd_s_f1_f2_r3_16Comb(INSTINFO *info);
#define ldfpd_s_nt1_f1_f2_r3_16Comb ldfpd_s_f1_f2_r3_16Comb
#define ldfpd_s_nta_f1_f2_r3_16Comb ldfpd_s_f1_f2_r3_16Comb
extern Status ldfp8_s_f1_f2_r3_16Comb(INSTINFO *info);
#define ldfp8_s_nt1_f1_f2_r3_16Comb ldfp8_s_f1_f2_r3_16Comb
#define ldfp8_s_nta_f1_f2_r3_16Comb ldfp8_s_f1_f2_r3_16Comb
extern Status ldfps_a_f1_f2_r3_8Comb(INSTINFO *info);
#define ldfps_a_nt1_f1_f2_r3_8Comb ldfps_a_f1_f2_r3_8Comb
#define ldfps_a_nta_f1_f2_r3_8Comb ldfps_a_f1_f2_r3_8Comb
extern Status ldfpd_a_f1_f2_r3_16Comb(INSTINFO *info);
#define ldfpd_a_nt1_f1_f2_r3_16Comb ldfpd_a_f1_f2_r3_16Comb
#define ldfpd_a_nta_f1_f2_r3_16Comb ldfpd_a_f1_f2_r3_16Comb
extern Status ldfp8_a_f1_f2_r3_16Comb(INSTINFO *info);
#define ldfp8_a_nt1_f1_f2_r3_16Comb ldfp8_a_f1_f2_r3_16Comb
#define ldfp8_a_nta_f1_f2_r3_16Comb ldfp8_a_f1_f2_r3_16Comb
extern Status ldfps_sa_f1_f2_r3_8Comb(INSTINFO *info);
#define ldfps_sa_nt1_f1_f2_r3_8Comb ldfps_sa_f1_f2_r3_8Comb
#define ldfps_sa_nta_f1_f2_r3_8Comb ldfps_sa_f1_f2_r3_8Comb
extern Status ldfpd_sa_f1_f2_r3_16Comb(INSTINFO *info);
#define ldfpd_sa_nt1_f1_f2_r3_16Comb ldfpd_sa_f1_f2_r3_16Comb
#define ldfpd_sa_nta_f1_f2_r3_16Comb ldfpd_sa_f1_f2_r3_16Comb
extern Status ldfp8_sa_f1_f2_r3_16Comb(INSTINFO *info);
#define ldfp8_sa_nt1_f1_f2_r3_16Comb ldfp8_sa_f1_f2_r3_16Comb
#define ldfp8_sa_nta_f1_f2_r3_16Comb ldfp8_sa_f1_f2_r3_16Comb
extern Status ldfps_c_clr_f1_f2_r3_8Comb(INSTINFO *info);
#define ldfps_c_clr_nt1_f1_f2_r3_8Comb ldfps_c_clr_f1_f2_r3_8Comb
#define ldfps_c_clr_nta_f1_f2_r3_8Comb ldfps_c_clr_f1_f2_r3_8Comb
extern Status ldfpd_c_clr_f1_f2_r3_16Comb(INSTINFO *info);
#define ldfpd_c_clr_nt1_f1_f2_r3_16Comb ldfpd_c_clr_f1_f2_r3_16Comb
#define ldfpd_c_clr_nta_f1_f2_r3_16Comb ldfpd_c_clr_f1_f2_r3_16Comb
extern Status ldfp8_c_clr_f1_f2_r3_16Comb(INSTINFO *info);
#define ldfp8_c_clr_nt1_f1_f2_r3_16Comb ldfp8_c_clr_f1_f2_r3_16Comb
#define ldfp8_c_clr_nta_f1_f2_r3_16Comb ldfp8_c_clr_f1_f2_r3_16Comb
extern Status ldfps_c_nc_f1_f2_r3_8Comb(INSTINFO *info);
#define ldfps_c_nc_nt1_f1_f2_r3_8Comb ldfps_c_nc_f1_f2_r3_8Comb
#define ldfps_c_nc_nta_f1_f2_r3_8Comb ldfps_c_nc_f1_f2_r3_8Comb
extern Status ldfpd_c_nc_f1_f2_r3_16Comb(INSTINFO *info);
#define ldfpd_c_nc_nt1_f1_f2_r3_16Comb ldfpd_c_nc_f1_f2_r3_16Comb
#define ldfpd_c_nc_nta_f1_f2_r3_16Comb ldfpd_c_nc_f1_f2_r3_16Comb
extern Status ldfp8_c_nc_f1_f2_r3_16Comb(INSTINFO *info);
#define ldfp8_c_nc_nt1_f1_f2_r3_16Comb ldfp8_c_nc_f1_f2_r3_16Comb
#define ldfp8_c_nc_nta_f1_f2_r3_16Comb ldfp8_c_nc_f1_f2_r3_16Comb
extern Status lfetch_r3Comb(INSTINFO *info);
#define lfetch_nt1_r3Comb lfetch_r3Comb
#define lfetch_nt2_r3Comb lfetch_r3Comb
#define lfetch_nta_r3Comb lfetch_r3Comb
#define lfetch_excl_r3Comb lfetch_r3Comb
#define lfetch_excl_nt1_r3Comb lfetch_excl_r3Comb
#define lfetch_excl_nt2_r3Comb lfetch_excl_r3Comb
#define lfetch_excl_nta_r3Comb lfetch_excl_r3Comb
extern Status lfetch_fault_r3Comb(INSTINFO *info);
#define lfetch_fault_nt1_r3Comb lfetch_fault_r3Comb
#define lfetch_fault_nt2_r3Comb lfetch_fault_r3Comb
#define lfetch_fault_nta_r3Comb lfetch_fault_r3Comb
#define lfetch_fault_excl_r3Comb lfetch_fault_r3Comb
#define lfetch_fault_excl_nt1_r3Comb lfetch_fault_excl_r3Comb
#define lfetch_fault_excl_nt2_r3Comb lfetch_fault_excl_r3Comb
#define lfetch_fault_excl_nta_r3Comb lfetch_fault_excl_r3Comb
extern Status lfetch_r3_r2Comb(INSTINFO *info);
#define lfetch_nt1_r3_r2Comb lfetch_r3_r2Comb
#define lfetch_nt2_r3_r2Comb lfetch_r3_r2Comb
#define lfetch_nta_r3_r2Comb lfetch_r3_r2Comb
#define lfetch_excl_r3_r2Comb lfetch_r3_r2Comb
#define lfetch_excl_nt1_r3_r2Comb lfetch_excl_r3_r2Comb
#define lfetch_excl_nt2_r3_r2Comb lfetch_excl_r3_r2Comb
#define lfetch_excl_nta_r3_r2Comb lfetch_excl_r3_r2Comb
extern Status lfetch_fault_r3_r2Comb(INSTINFO *info);
#define lfetch_fault_nt1_r3_r2Comb lfetch_fault_r3_r2Comb
#define lfetch_fault_nt2_r3_r2Comb lfetch_fault_r3_r2Comb
#define lfetch_fault_nta_r3_r2Comb lfetch_fault_r3_r2Comb
#define lfetch_fault_excl_r3_r2Comb lfetch_fault_r3_r2Comb
#define lfetch_fault_excl_nt1_r3_r2Comb lfetch_fault_excl_r3_r2Comb
#define lfetch_fault_excl_nt2_r3_r2Comb lfetch_fault_excl_r3_r2Comb
#define lfetch_fault_excl_nta_r3_r2Comb lfetch_fault_excl_r3_r2Comb
extern Status lfetch_r3_imm9Comb(INSTINFO *info);
#define lfetch_nt1_r3_imm9Comb lfetch_r3_imm9Comb
#define lfetch_nt2_r3_imm9Comb lfetch_r3_imm9Comb
#define lfetch_nta_r3_imm9Comb lfetch_r3_imm9Comb
#define lfetch_excl_r3_imm9Comb lfetch_r3_imm9Comb
#define lfetch_excl_nt1_r3_imm9Comb lfetch_excl_r3_imm9Comb
#define lfetch_excl_nt2_r3_imm9Comb lfetch_excl_r3_imm9Comb
#define lfetch_excl_nta_r3_imm9Comb lfetch_excl_r3_imm9Comb
extern Status lfetch_fault_r3_imm9Comb(INSTINFO *info);
#define lfetch_fault_nt1_r3_imm9Comb lfetch_fault_r3_imm9Comb
#define lfetch_fault_nt2_r3_imm9Comb lfetch_fault_r3_imm9Comb
#define lfetch_fault_nta_r3_imm9Comb lfetch_fault_r3_imm9Comb
#define lfetch_fault_excl_r3_imm9Comb lfetch_fault_r3_imm9Comb
#define lfetch_fault_excl_nt1_r3_imm9Comb lfetch_fault_excl_r3_imm9Comb
#define lfetch_fault_excl_nt2_r3_imm9Comb lfetch_fault_excl_r3_imm9Comb
#define lfetch_fault_excl_nta_r3_imm9Comb lfetch_fault_excl_r3_imm9Comb
extern Status cmpxchg1_acq_r1_r3_r2_ar_ccvComb(INSTINFO *info);
#define cmpxchg1_acq_nt1_r1_r3_r2_ar_ccvComb cmpxchg1_acq_r1_r3_r2_ar_ccvComb
#define cmpxchg1_acq_nta_r1_r3_r2_ar_ccvComb cmpxchg1_acq_r1_r3_r2_ar_ccvComb
extern Status cmpxchg2_acq_r1_r3_r2_ar_ccvComb(INSTINFO *info);
#define cmpxchg2_acq_nt1_r1_r3_r2_ar_ccvComb cmpxchg2_acq_r1_r3_r2_ar_ccvComb
#define cmpxchg2_acq_nta_r1_r3_r2_ar_ccvComb cmpxchg2_acq_r1_r3_r2_ar_ccvComb
extern Status cmpxchg4_acq_r1_r3_r2_ar_ccvComb(INSTINFO *info);
#define cmpxchg4_acq_nt1_r1_r3_r2_ar_ccvComb cmpxchg4_acq_r1_r3_r2_ar_ccvComb
#define cmpxchg4_acq_nta_r1_r3_r2_ar_ccvComb cmpxchg4_acq_r1_r3_r2_ar_ccvComb
extern Status cmpxchg8_acq_r1_r3_r2_ar_ccvComb(INSTINFO *info);
#define cmpxchg8_acq_nt1_r1_r3_r2_ar_ccvComb cmpxchg8_acq_r1_r3_r2_ar_ccvComb
#define cmpxchg8_acq_nta_r1_r3_r2_ar_ccvComb cmpxchg8_acq_r1_r3_r2_ar_ccvComb
extern Status cmp8xchg16_acq_r1_r3_r2_ar_csd_ar_ccvComb(INSTINFO *info);
#define cmp8xchg16_acq_nt1_r1_r3_r2_ar_csd_ar_ccvComb cmp8xchg16_acq_r1_r3_r2_ar_csd_ar_ccvComb
#define cmp8xchg16_acq_nta_r1_r3_r2_ar_csd_ar_ccvComb cmp8xchg16_acq_r1_r3_r2_ar_csd_ar_ccvComb
#define cmpxchg1_rel_r1_r3_r2_ar_ccvComb cmpxchg1_acq_r1_r3_r2_ar_ccvComb
#define cmpxchg1_rel_nt1_r1_r3_r2_ar_ccvComb cmpxchg1_rel_r1_r3_r2_ar_ccvComb
#define cmpxchg1_rel_nta_r1_r3_r2_ar_ccvComb cmpxchg1_rel_r1_r3_r2_ar_ccvComb
#define cmpxchg2_rel_r1_r3_r2_ar_ccvComb cmpxchg2_acq_r1_r3_r2_ar_ccvComb
#define cmpxchg2_rel_nt1_r1_r3_r2_ar_ccvComb cmpxchg2_rel_r1_r3_r2_ar_ccvComb
#define cmpxchg2_rel_nta_r1_r3_r2_ar_ccvComb cmpxchg2_rel_r1_r3_r2_ar_ccvComb
#define cmpxchg4_rel_r1_r3_r2_ar_ccvComb cmpxchg4_acq_r1_r3_r2_ar_ccvComb
#define cmpxchg4_rel_nt1_r1_r3_r2_ar_ccvComb cmpxchg4_rel_r1_r3_r2_ar_ccvComb
#define cmpxchg4_rel_nta_r1_r3_r2_ar_ccvComb cmpxchg4_rel_r1_r3_r2_ar_ccvComb
#define cmpxchg8_rel_r1_r3_r2_ar_ccvComb cmpxchg8_acq_r1_r3_r2_ar_ccvComb
#define cmpxchg8_rel_nt1_r1_r3_r2_ar_ccvComb cmpxchg8_rel_r1_r3_r2_ar_ccvComb
#define cmpxchg8_rel_nta_r1_r3_r2_ar_ccvComb cmpxchg8_rel_r1_r3_r2_ar_ccvComb
#define cmp8xchg16_rel_r1_r3_r2_ar_csd_ar_ccvComb cmp8xchg16_acq_r1_r3_r2_ar_csd_ar_ccvComb
#define cmp8xchg16_rel_nt1_r1_r3_r2_ar_csd_ar_ccvComb cmp8xchg16_rel_r1_r3_r2_ar_csd_ar_ccvComb
#define cmp8xchg16_rel_nta_r1_r3_r2_ar_csd_ar_ccvComb cmp8xchg16_rel_r1_r3_r2_ar_csd_ar_ccvComb
extern Status xchg1_r1_r3_r2Comb(INSTINFO *info);
#define xchg1_nt1_r1_r3_r2Comb xchg1_r1_r3_r2Comb
#define xchg1_nta_r1_r3_r2Comb xchg1_r1_r3_r2Comb
extern Status xchg2_r1_r3_r2Comb(INSTINFO *info);
#define xchg2_nt1_r1_r3_r2Comb xchg2_r1_r3_r2Comb
#define xchg2_nta_r1_r3_r2Comb xchg2_r1_r3_r2Comb
extern Status xchg4_r1_r3_r2Comb(INSTINFO *info);
#define xchg4_nt1_r1_r3_r2Comb xchg4_r1_r3_r2Comb
#define xchg4_nta_r1_r3_r2Comb xchg4_r1_r3_r2Comb
extern Status xchg8_r1_r3_r2Comb(INSTINFO *info);
#define xchg8_nt1_r1_r3_r2Comb xchg8_r1_r3_r2Comb
#define xchg8_nta_r1_r3_r2Comb xchg8_r1_r3_r2Comb
extern Status fetchadd4_acq_r1_r3_inc3Comb(INSTINFO *info);
#define fetchadd4_acq_nt1_r1_r3_inc3Comb fetchadd4_acq_r1_r3_inc3Comb
#define fetchadd4_acq_nta_r1_r3_inc3Comb fetchadd4_acq_r1_r3_inc3Comb
extern Status fetchadd8_acq_r1_r3_inc3Comb(INSTINFO *info);
#define fetchadd8_acq_nt1_r1_r3_inc3Comb fetchadd8_acq_r1_r3_inc3Comb
#define fetchadd8_acq_nta_r1_r3_inc3Comb fetchadd8_acq_r1_r3_inc3Comb
#define fetchadd4_rel_r1_r3_inc3Comb fetchadd4_acq_r1_r3_inc3Comb
#define fetchadd4_rel_nt1_r1_r3_inc3Comb fetchadd4_rel_r1_r3_inc3Comb
#define fetchadd4_rel_nta_r1_r3_inc3Comb fetchadd4_rel_r1_r3_inc3Comb
#define fetchadd8_rel_r1_r3_inc3Comb fetchadd8_acq_r1_r3_inc3Comb
#define fetchadd8_rel_nt1_r1_r3_inc3Comb fetchadd8_rel_r1_r3_inc3Comb
#define fetchadd8_rel_nta_r1_r3_inc3Comb fetchadd8_rel_r1_r3_inc3Comb
extern Status setf_sig_f1_r2Comb(INSTINFO *info);
extern Status setf_exp_f1_r2Comb(INSTINFO *info);
extern Status setf_s_f1_r2Comb(INSTINFO *info);
extern Status setf_d_f1_r2Comb(INSTINFO *info);
extern Status getf_sig_r1_f2Comb(INSTINFO *info);
extern Status getf_exp_r1_f2Comb(INSTINFO *info);
extern Status getf_s_r1_f2Comb(INSTINFO *info);
extern Status getf_d_r1_f2Comb(INSTINFO *info);
#define chk_s_m_r2_target25Comb chk_s_i_r2_target25Comb
extern Status chk_s_f2_target25Comb(INSTINFO *info);
extern Status chk_a_nc_r1_target25Comb(INSTINFO *info);
extern Status chk_a_clr_r1_target25Comb(INSTINFO *info);
extern Status chk_a_nc_f1_target25Comb(INSTINFO *info);
extern Status chk_a_clr_f1_target25Comb(INSTINFO *info);
extern Status invalaComb(INSTINFO *info);
#define fwbComb nop_i_imm21Comb
#define mfComb nop_i_imm21Comb
#define mf_aComb nop_i_imm21Comb
#define srlz_dComb nop_i_imm21Comb
#define srlz_iComb nop_i_imm21Comb
#define sync_iComb nop_i_imm21Comb
extern Status flushrsComb(INSTINFO *info);
extern Status loadrsComb(INSTINFO *info);
extern Status invala_e_r1Comb(INSTINFO *info);
extern Status invala_e_f1Comb(INSTINFO *info);
extern Status fc_r3Comb(INSTINFO *info);
#define fc_i_r3Comb fc_r3Comb
extern Status mov_m_ar3_r2Comb(INSTINFO *info);
extern Status mov_m_ar3_imm8Comb(INSTINFO *info);
extern Status mov_m_r1_ar3Comb(INSTINFO *info);
extern Status mov_cr3_r2Comb(INSTINFO *info);
extern Status mov_r1_cr3Comb(INSTINFO *info);
extern Status alloc_r1_ar_pfs_i_l_o_rComb(INSTINFO *info);
extern Status mov_psr_l_r2Comb(INSTINFO *info);
extern Status mov_psr_um_r2Comb(INSTINFO *info);
extern Status mov_r1_psrComb(INSTINFO *info);
extern Status mov_r1_psr_umComb(INSTINFO *info);
#define break_m_imm21Comb break_i_imm21Comb
#define nop_m_imm21Comb nop_i_imm21Comb
extern Status probe_r_r1_r3_r2Comb(INSTINFO *info);
extern Status probe_w_r1_r3_r2Comb(INSTINFO *info);
extern Status probe_r_r1_r3_imm2Comb(INSTINFO *info);
extern Status probe_w_r1_r3_imm2Comb(INSTINFO *info);
extern Status probe_rw_fault_r3_imm2Comb(INSTINFO *info);
extern Status probe_r_fault_r3_imm2Comb(INSTINFO *info);
extern Status probe_w_fault_r3_imm2Comb(INSTINFO *info);
extern Status itc_d_r2Comb(INSTINFO *info);
extern Status itc_i_r2Comb(INSTINFO *info);
extern Status mov_rr_r3_r2Comb(INSTINFO *info);
extern Status mov_dbr_r3_r2Comb(INSTINFO *info);
extern Status mov_ibr_r3_r2Comb(INSTINFO *info);
extern Status mov_pkr_r3_r2Comb(INSTINFO *info);
extern Status mov_pmc_r3_r2Comb(INSTINFO *info);
extern Status mov_pmd_r3_r2Comb(INSTINFO *info);
extern Status itr_d_dtr_r3_r2Comb(INSTINFO *info);
extern Status itr_i_itr_r3_r2Comb(INSTINFO *info);
extern Status mov_r1_rr_r3Comb(INSTINFO *info);
extern Status mov_r1_dbr_r3Comb(INSTINFO *info);
extern Status mov_r1_ibr_r3Comb(INSTINFO *info);
extern Status mov_r1_pkr_r3Comb(INSTINFO *info);
extern Status mov_r1_pmc_r3Comb(INSTINFO *info);
extern Status mov_r1_pmd_r3Comb(INSTINFO *info);
extern Status mov_r1_cpuid_r3Comb(INSTINFO *info);
extern Status sum_imm24Comb(INSTINFO *info);
extern Status rum_imm24Comb(INSTINFO *info);
extern Status ssm_imm24Comb(INSTINFO *info);
extern Status rsm_imm24Comb(INSTINFO *info);
extern Status ptc_l_r3_r2Comb(INSTINFO *info);
#define ptc_g_r3_r2Comb ptc_l_r3_r2Comb
#define ptc_ga_r3_r2Comb ptc_l_r3_r2Comb
extern Status ptr_d_r3_r2Comb(INSTINFO *info);
extern Status ptr_i_r3_r2Comb(INSTINFO *info);
extern Status thash_r1_r3Comb(INSTINFO *info);
extern Status ttag_r1_r3Comb(INSTINFO *info);
extern Status tpa_r1_r3Comb(INSTINFO *info);
extern Status tak_r1_r3Comb(INSTINFO *info);
extern Status ptc_e_r3Comb(INSTINFO *info);
#define nop_m_imm21Comb nop_i_imm21Comb
#define hint_m_imm21Comb hint_i_imm21Comb
extern Status br_cond_spnt_few_target25Comb(INSTINFO *info);
#define br_cond_spnt_many_target25Comb br_cond_spnt_few_target25Comb
#define br_cond_sptk_few_target25Comb br_cond_spnt_few_target25Comb
#define br_cond_sptk_many_target25Comb br_cond_spnt_few_target25Comb
#define br_cond_dpnt_few_target25Comb br_cond_spnt_few_target25Comb
#define br_cond_dpnt_many_target25Comb br_cond_spnt_few_target25Comb
#define br_cond_dptk_few_target25Comb br_cond_spnt_few_target25Comb
#define br_cond_dptk_many_target25Comb br_cond_spnt_few_target25Comb
#define br_cond_spnt_few_clr_target25Comb br_cond_spnt_few_target25Comb
#define br_cond_spnt_many_clr_target25Comb br_cond_spnt_few_target25Comb
#define br_cond_sptk_few_clr_target25Comb br_cond_spnt_few_target25Comb
#define br_cond_sptk_many_clr_target25Comb br_cond_spnt_few_target25Comb
#define br_cond_dpnt_few_clr_target25Comb br_cond_spnt_few_target25Comb
#define br_cond_dpnt_many_clr_target25Comb br_cond_spnt_few_target25Comb
#define br_cond_dptk_few_clr_target25Comb br_cond_spnt_few_target25Comb
#define br_cond_dptk_many_clr_target25Comb br_cond_spnt_few_target25Comb
extern Status br_wexit_spnt_few_target25Comb(INSTINFO *info);
#define br_wexit_spnt_many_target25Comb br_wexit_spnt_few_target25Comb
#define br_wexit_sptk_few_target25Comb br_wexit_spnt_few_target25Comb
#define br_wexit_sptk_many_target25Comb br_wexit_spnt_few_target25Comb
#define br_wexit_dpnt_few_target25Comb br_wexit_spnt_few_target25Comb
#define br_wexit_dpnt_many_target25Comb br_wexit_spnt_few_target25Comb
#define br_wexit_dptk_few_target25Comb br_wexit_spnt_few_target25Comb
#define br_wexit_dptk_many_target25Comb br_wexit_spnt_few_target25Comb
#define br_wexit_spnt_few_clr_target25Comb br_wexit_spnt_few_target25Comb
#define br_wexit_spnt_many_clr_target25Comb br_wexit_spnt_few_target25Comb
#define br_wexit_sptk_few_clr_target25Comb br_wexit_spnt_few_target25Comb
#define br_wexit_sptk_many_clr_target25Comb br_wexit_spnt_few_target25Comb
#define br_wexit_dpnt_few_clr_target25Comb br_wexit_spnt_few_target25Comb
#define br_wexit_dpnt_many_clr_target25Comb br_wexit_spnt_few_target25Comb
#define br_wexit_dptk_few_clr_target25Comb br_wexit_spnt_few_target25Comb
#define br_wexit_dptk_many_clr_target25Comb br_wexit_spnt_few_target25Comb
extern Status br_wtop_spnt_few_target25Comb(INSTINFO *info);
#define br_wtop_spnt_many_target25Comb br_wtop_spnt_few_target25Comb
#define br_wtop_sptk_few_target25Comb br_wtop_spnt_few_target25Comb
#define br_wtop_sptk_many_target25Comb br_wtop_spnt_few_target25Comb
#define br_wtop_dpnt_few_target25Comb br_wtop_spnt_few_target25Comb
#define br_wtop_dpnt_many_target25Comb br_wtop_spnt_few_target25Comb
#define br_wtop_dptk_few_target25Comb br_wtop_spnt_few_target25Comb
#define br_wtop_dptk_many_target25Comb br_wtop_spnt_few_target25Comb
#define br_wtop_spnt_few_clr_target25Comb br_wtop_spnt_few_target25Comb
#define br_wtop_spnt_many_clr_target25Comb br_wtop_spnt_few_target25Comb
#define br_wtop_sptk_few_clr_target25Comb br_wtop_spnt_few_target25Comb
#define br_wtop_sptk_many_clr_target25Comb br_wtop_spnt_few_target25Comb
#define br_wtop_dpnt_few_clr_target25Comb br_wtop_spnt_few_target25Comb
#define br_wtop_dpnt_many_clr_target25Comb br_wtop_spnt_few_target25Comb
#define br_wtop_dptk_few_clr_target25Comb br_wtop_spnt_few_target25Comb
#define br_wtop_dptk_many_clr_target25Comb br_wtop_spnt_few_target25Comb
extern Status br_cloop_spnt_few_target25Comb(INSTINFO *info);
#define br_cloop_spnt_many_target25Comb br_cloop_spnt_few_target25Comb
#define br_cloop_sptk_few_target25Comb br_cloop_spnt_few_target25Comb
#define br_cloop_sptk_many_target25Comb br_cloop_spnt_few_target25Comb
#define br_cloop_dpnt_few_target25Comb br_cloop_spnt_few_target25Comb
#define br_cloop_dpnt_many_target25Comb br_cloop_spnt_few_target25Comb
#define br_cloop_dptk_few_target25Comb br_cloop_spnt_few_target25Comb
#define br_cloop_dptk_many_target25Comb br_cloop_spnt_few_target25Comb
#define br_cloop_spnt_few_clr_target25Comb br_cloop_spnt_few_target25Comb
#define br_cloop_spnt_many_clr_target25Comb br_cloop_spnt_few_target25Comb
#define br_cloop_sptk_few_clr_target25Comb br_cloop_spnt_few_target25Comb
#define br_cloop_sptk_many_clr_target25Comb br_cloop_spnt_few_target25Comb
#define br_cloop_dpnt_few_clr_target25Comb br_cloop_spnt_few_target25Comb
#define br_cloop_dpnt_many_clr_target25Comb br_cloop_spnt_few_target25Comb
#define br_cloop_dptk_few_clr_target25Comb br_cloop_spnt_few_target25Comb
#define br_cloop_dptk_many_clr_target25Comb br_cloop_spnt_few_target25Comb
extern Status br_cexit_spnt_few_target25Comb(INSTINFO *info);
#define br_cexit_spnt_many_target25Comb br_cexit_spnt_few_target25Comb
#define br_cexit_sptk_few_target25Comb br_cexit_spnt_few_target25Comb
#define br_cexit_sptk_many_target25Comb br_cexit_spnt_few_target25Comb
#define br_cexit_dpnt_few_target25Comb br_cexit_spnt_few_target25Comb
#define br_cexit_dpnt_many_target25Comb br_cexit_spnt_few_target25Comb
#define br_cexit_dptk_few_target25Comb br_cexit_spnt_few_target25Comb
#define br_cexit_dptk_many_target25Comb br_cexit_spnt_few_target25Comb
#define br_cexit_spnt_few_clr_target25Comb br_cexit_spnt_few_target25Comb
#define br_cexit_spnt_many_clr_target25Comb br_cexit_spnt_few_target25Comb
#define br_cexit_sptk_few_clr_target25Comb br_cexit_spnt_few_target25Comb
#define br_cexit_sptk_many_clr_target25Comb br_cexit_spnt_few_target25Comb
#define br_cexit_dpnt_few_clr_target25Comb br_cexit_spnt_few_target25Comb
#define br_cexit_dpnt_many_clr_target25Comb br_cexit_spnt_few_target25Comb
#define br_cexit_dptk_few_clr_target25Comb br_cexit_spnt_few_target25Comb
#define br_cexit_dptk_many_clr_target25Comb br_cexit_spnt_few_target25Comb
extern Status br_ctop_spnt_few_target25Comb(INSTINFO *info);
#define br_ctop_spnt_many_target25Comb br_ctop_spnt_few_target25Comb
#define br_ctop_sptk_few_target25Comb br_ctop_spnt_few_target25Comb
#define br_ctop_sptk_many_target25Comb br_ctop_spnt_few_target25Comb
#define br_ctop_dpnt_few_target25Comb br_ctop_spnt_few_target25Comb
#define br_ctop_dpnt_many_target25Comb br_ctop_spnt_few_target25Comb
#define br_ctop_dptk_few_target25Comb br_ctop_spnt_few_target25Comb
#define br_ctop_dptk_many_target25Comb br_ctop_spnt_few_target25Comb
#define br_ctop_spnt_few_clr_target25Comb br_ctop_spnt_few_target25Comb
#define br_ctop_spnt_many_clr_target25Comb br_ctop_spnt_few_target25Comb
#define br_ctop_sptk_few_clr_target25Comb br_ctop_spnt_few_target25Comb
#define br_ctop_sptk_many_clr_target25Comb br_ctop_spnt_few_target25Comb
#define br_ctop_dpnt_few_clr_target25Comb br_ctop_spnt_few_target25Comb
#define br_ctop_dpnt_many_clr_target25Comb br_ctop_spnt_few_target25Comb
#define br_ctop_dptk_few_clr_target25Comb br_ctop_spnt_few_target25Comb
#define br_ctop_dptk_many_clr_target25Comb br_ctop_spnt_few_target25Comb
extern Status br_call_spnt_few_b1_target25Comb(INSTINFO *info);
#define br_call_spnt_many_b1_target25Comb br_call_spnt_few_b1_target25Comb
#define br_call_sptk_few_b1_target25Comb br_call_spnt_few_b1_target25Comb
#define br_call_sptk_many_b1_target25Comb br_call_spnt_few_b1_target25Comb
#define br_call_dpnt_few_b1_target25Comb br_call_spnt_few_b1_target25Comb
#define br_call_dpnt_many_b1_target25Comb br_call_spnt_few_b1_target25Comb
#define br_call_dptk_few_b1_target25Comb br_call_spnt_few_b1_target25Comb
#define br_call_dptk_many_b1_target25Comb br_call_spnt_few_b1_target25Comb
#define br_call_spnt_few_clr_b1_target25Comb br_call_spnt_few_b1_target25Comb
#define br_call_spnt_many_clr_b1_target25Comb br_call_spnt_few_b1_target25Comb
#define br_call_sptk_few_clr_b1_target25Comb br_call_spnt_few_b1_target25Comb
#define br_call_sptk_many_clr_b1_target25Comb br_call_spnt_few_b1_target25Comb
#define br_call_dpnt_few_clr_b1_target25Comb br_call_spnt_few_b1_target25Comb
#define br_call_dpnt_many_clr_b1_target25Comb br_call_spnt_few_b1_target25Comb
#define br_call_dptk_few_clr_b1_target25Comb br_call_spnt_few_b1_target25Comb
#define br_call_dptk_many_clr_b1_target25Comb br_call_spnt_few_b1_target25Comb
extern Status br_cond_spnt_few_b2Comb(INSTINFO *info);
#define br_cond_spnt_many_b2Comb br_cond_spnt_few_b2Comb
#define br_cond_sptk_few_b2Comb br_cond_spnt_few_b2Comb
#define br_cond_sptk_many_b2Comb br_cond_spnt_few_b2Comb
#define br_cond_dpnt_few_b2Comb br_cond_spnt_few_b2Comb
#define br_cond_dpnt_many_b2Comb br_cond_spnt_few_b2Comb
#define br_cond_dptk_few_b2Comb br_cond_spnt_few_b2Comb
#define br_cond_dptk_many_b2Comb br_cond_spnt_few_b2Comb
#define br_cond_spnt_few_clr_b2Comb br_cond_spnt_few_b2Comb
#define br_cond_spnt_many_clr_b2Comb br_cond_spnt_few_b2Comb
#define br_cond_sptk_few_clr_b2Comb br_cond_spnt_few_b2Comb
#define br_cond_sptk_many_clr_b2Comb br_cond_spnt_few_b2Comb
#define br_cond_dpnt_few_clr_b2Comb br_cond_spnt_few_b2Comb
#define br_cond_dpnt_many_clr_b2Comb br_cond_spnt_few_b2Comb
#define br_cond_dptk_few_clr_b2Comb br_cond_spnt_few_b2Comb
#define br_cond_dptk_many_clr_b2Comb br_cond_spnt_few_b2Comb
extern Status br_ia_spnt_few_b2Comb(INSTINFO *info);
#define br_ia_spnt_many_b2Comb br_ia_spnt_few_b2Comb
#define br_ia_sptk_few_b2Comb br_ia_spnt_few_b2Comb
#define br_ia_sptk_many_b2Comb br_ia_spnt_few_b2Comb
#define br_ia_dpnt_few_b2Comb br_ia_spnt_few_b2Comb
#define br_ia_dpnt_many_b2Comb br_ia_spnt_few_b2Comb
#define br_ia_dptk_few_b2Comb br_ia_spnt_few_b2Comb
#define br_ia_dptk_many_b2Comb br_ia_spnt_few_b2Comb
#define br_ia_spnt_few_clr_b2Comb br_ia_spnt_few_b2Comb
#define br_ia_spnt_many_clr_b2Comb br_ia_spnt_few_b2Comb
#define br_ia_sptk_few_clr_b2Comb br_ia_spnt_few_b2Comb
#define br_ia_sptk_many_clr_b2Comb br_ia_spnt_few_b2Comb
#define br_ia_dpnt_few_clr_b2Comb br_ia_spnt_few_b2Comb
#define br_ia_dpnt_many_clr_b2Comb br_ia_spnt_few_b2Comb
#define br_ia_dptk_few_clr_b2Comb br_ia_spnt_few_b2Comb
#define br_ia_dptk_many_clr_b2Comb br_ia_spnt_few_b2Comb
extern Status br_ret_spnt_few_b2Comb(INSTINFO *info);
#define br_ret_spnt_many_b2Comb br_ret_spnt_few_b2Comb
#define br_ret_sptk_few_b2Comb br_ret_spnt_few_b2Comb
#define br_ret_sptk_many_b2Comb br_ret_spnt_few_b2Comb
#define br_ret_dpnt_few_b2Comb br_ret_spnt_few_b2Comb
#define br_ret_dpnt_many_b2Comb br_ret_spnt_few_b2Comb
#define br_ret_dptk_few_b2Comb br_ret_spnt_few_b2Comb
#define br_ret_dptk_many_b2Comb br_ret_spnt_few_b2Comb
#define br_ret_spnt_few_clr_b2Comb br_ret_spnt_few_b2Comb
#define br_ret_spnt_many_clr_b2Comb br_ret_spnt_few_b2Comb
#define br_ret_sptk_few_clr_b2Comb br_ret_spnt_few_b2Comb
#define br_ret_sptk_many_clr_b2Comb br_ret_spnt_few_b2Comb
#define br_ret_dpnt_few_clr_b2Comb br_ret_spnt_few_b2Comb
#define br_ret_dpnt_many_clr_b2Comb br_ret_spnt_few_b2Comb
#define br_ret_dptk_few_clr_b2Comb br_ret_spnt_few_b2Comb
#define br_ret_dptk_many_clr_b2Comb br_ret_spnt_few_b2Comb
extern Status br_call_spnt_few_b1_b2Comb(INSTINFO *info);
#define br_call_spnt_many_b1_b2Comb br_call_spnt_few_b1_b2Comb
#define br_call_sptk_few_b1_b2Comb br_call_spnt_few_b1_b2Comb
#define br_call_sptk_many_b1_b2Comb br_call_spnt_few_b1_b2Comb
#define br_call_dpnt_few_b1_b2Comb br_call_spnt_few_b1_b2Comb
#define br_call_dpnt_many_b1_b2Comb br_call_spnt_few_b1_b2Comb
#define br_call_dptk_few_b1_b2Comb br_call_spnt_few_b1_b2Comb
#define br_call_dptk_many_b1_b2Comb br_call_spnt_few_b1_b2Comb
#define br_call_spnt_few_clr_b1_b2Comb br_call_spnt_few_b1_b2Comb
#define br_call_spnt_many_clr_b1_b2Comb br_call_spnt_few_b1_b2Comb
#define br_call_sptk_few_clr_b1_b2Comb br_call_spnt_few_b1_b2Comb
#define br_call_sptk_many_clr_b1_b2Comb br_call_spnt_few_b1_b2Comb
#define br_call_dpnt_few_clr_b1_b2Comb br_call_spnt_few_b1_b2Comb
#define br_call_dpnt_many_clr_b1_b2Comb br_call_spnt_few_b1_b2Comb
#define br_call_dptk_few_clr_b1_b2Comb br_call_spnt_few_b1_b2Comb
#define br_call_dptk_many_clr_b1_b2Comb br_call_spnt_few_b1_b2Comb
extern Status brp_loop_target25_tag13Comb(INSTINFO *info);
#define brp_loop_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_many_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_many_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_many_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_many_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_many_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_many_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_many_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_many_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_dc_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_dc_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_many_dc_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_many_dc_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_dc_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_dc_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_many_dc_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_many_dc_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_dc_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_dc_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_many_dc_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_many_dc_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_dc_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_dc_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_many_dc_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_many_dc_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_tk_dc_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_tk_dc_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_many_tk_dc_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_many_tk_dc_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_tk_dc_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_tk_dc_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_many_tk_dc_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_many_tk_dc_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_tk_dc_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_tk_dc_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_many_tk_dc_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_many_tk_dc_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_tk_dc_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_tk_dc_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_many_tk_dc_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_many_tk_dc_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_tk_tk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_tk_tk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_many_tk_tk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_many_tk_tk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_tk_tk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_tk_tk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_many_tk_tk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_many_tk_tk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_tk_tk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_tk_tk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_many_tk_tk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_many_tk_tk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_tk_tk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_tk_tk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_many_tk_tk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_many_tk_tk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_tk_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_tk_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_many_tk_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_many_tk_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_tk_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_tk_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_many_tk_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_many_tk_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_tk_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_tk_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_many_tk_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_many_tk_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_tk_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_tk_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_many_tk_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_many_tk_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_nt_dc_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_nt_dc_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_many_nt_dc_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_many_nt_dc_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_nt_dc_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_nt_dc_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_many_nt_dc_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_many_nt_dc_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_nt_dc_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_nt_dc_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_many_nt_dc_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_many_nt_dc_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_nt_dc_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_nt_dc_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_many_nt_dc_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_many_nt_dc_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_nt_tk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_nt_tk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_many_nt_tk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_many_nt_tk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_nt_tk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_nt_tk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_many_nt_tk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_many_nt_tk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_nt_tk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_nt_tk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_many_nt_tk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_many_nt_tk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_nt_tk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_nt_tk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_many_nt_tk_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_many_nt_tk_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_nt_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_nt_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_many_nt_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_loop_many_nt_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_nt_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_nt_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_many_nt_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_sptk_many_nt_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_nt_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_nt_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_many_nt_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_exit_many_nt_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_nt_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_nt_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_many_nt_nt_target25_tag13Comb brp_loop_target25_tag13Comb
#define brp_dptk_many_nt_nt_imp_target25_tag13Comb brp_loop_target25_tag13Comb
extern Status brp_sptk_b2_tag13Comb(INSTINFO *info);
#define brp_sptk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_many_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_many_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_many_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_many_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_dc_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_dc_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_many_dc_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_many_dc_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_dc_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_dc_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_many_dc_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_many_dc_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_tk_dc_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_tk_dc_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_many_tk_dc_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_many_tk_dc_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_tk_dc_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_tk_dc_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_many_tk_dc_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_many_tk_dc_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_tk_tk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_tk_tk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_many_tk_tk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_many_tk_tk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_tk_tk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_tk_tk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_many_tk_tk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_many_tk_tk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_tk_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_tk_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_many_tk_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_many_tk_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_tk_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_tk_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_many_tk_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_many_tk_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_nt_dc_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_nt_dc_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_many_nt_dc_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_many_nt_dc_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_nt_dc_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_nt_dc_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_many_nt_dc_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_many_nt_dc_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_nt_tk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_nt_tk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_many_nt_tk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_many_nt_tk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_nt_tk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_nt_tk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_many_nt_tk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_many_nt_tk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_nt_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_nt_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_many_nt_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_sptk_many_nt_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_nt_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_nt_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_many_nt_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_dptk_many_nt_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_many_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_many_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_many_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_many_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_dc_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_dc_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_many_dc_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_many_dc_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_dc_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_dc_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_many_dc_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_many_dc_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_tk_dc_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_tk_dc_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_many_tk_dc_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_many_tk_dc_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_tk_dc_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_tk_dc_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_many_tk_dc_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_many_tk_dc_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_tk_tk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_tk_tk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_many_tk_tk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_many_tk_tk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_tk_tk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_tk_tk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_many_tk_tk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_many_tk_tk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_tk_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_tk_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_many_tk_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_many_tk_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_tk_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_tk_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_many_tk_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_many_tk_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_nt_dc_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_nt_dc_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_many_nt_dc_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_many_nt_dc_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_nt_dc_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_nt_dc_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_many_nt_dc_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_many_nt_dc_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_nt_tk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_nt_tk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_many_nt_tk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_many_nt_tk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_nt_tk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_nt_tk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_many_nt_tk_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_many_nt_tk_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_nt_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_nt_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_many_nt_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_sptk_many_nt_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_nt_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_nt_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_many_nt_nt_b2_tag13Comb brp_sptk_b2_tag13Comb
#define brp_ret_dptk_many_nt_nt_imp_b2_tag13Comb brp_sptk_b2_tag13Comb
extern Status coverComb(INSTINFO *info);
extern Status clrrrbComb(INSTINFO *info);
extern Status clrrrb_prComb(INSTINFO *info);
extern Status rfiComb(INSTINFO *info);
extern Status bsw_0Comb(INSTINFO *info);
extern Status bsw_1Comb(INSTINFO *info);
extern Status epcComb(INSTINFO *info);
extern Status break_b_imm21Comb(INSTINFO *info);
#define nop_b_imm21Comb nop_i_imm21Comb
#define hint_b_imm21Comb hint_i_imm21Comb
extern Status fma_s0_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fma_s1_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fma_s2_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fma_s3_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fma_s_s0_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fma_s_s1_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fma_s_s2_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fma_s_s3_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fma_d_s0_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fma_d_s1_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fma_d_s2_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fma_d_s3_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fpma_s0_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fpma_s1_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fpma_s2_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fpma_s3_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fms_s0_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fms_s1_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fms_s2_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fms_s3_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fms_s_s0_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fms_s_s1_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fms_s_s2_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fms_s_s3_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fms_d_s0_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fms_d_s1_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fms_d_s2_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fms_d_s3_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fpms_s0_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fpms_s1_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fpms_s2_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fpms_s3_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fnma_s0_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fnma_s1_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fnma_s2_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fnma_s3_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fnma_s_s0_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fnma_s_s1_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fnma_s_s2_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fnma_s_s3_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fnma_d_s0_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fnma_d_s1_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fnma_d_s2_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fnma_d_s3_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fpnma_s0_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fpnma_s1_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fpnma_s2_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fpnma_s3_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status xma_l_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status xma_h_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status xma_hu_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fselect_f1_f3_f4_f2Comb(INSTINFO *info);
extern Status fcmp_eq_s0_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_eq_s1_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_eq_s2_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_eq_s3_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_lt_s0_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_lt_s1_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_lt_s2_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_lt_s3_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_le_s0_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_le_s1_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_le_s2_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_le_s3_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_unord_s0_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_unord_s1_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_unord_s2_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_unord_s3_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_eq_unc_s0_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_eq_unc_s1_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_eq_unc_s2_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_eq_unc_s3_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_lt_unc_s0_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_lt_unc_s1_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_lt_unc_s2_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_lt_unc_s3_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_le_unc_s0_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_le_unc_s1_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_le_unc_s2_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_le_unc_s3_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_unord_unc_s0_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_unord_unc_s1_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_unord_unc_s2_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fcmp_unord_unc_s3_p1_p2_f2_f3Comb(INSTINFO *info);
extern Status fclass_m_p1_p2_f2_fclass9Comb(INSTINFO *info);
extern Status fclass_m_unc_p1_p2_f2_fclass9Comb(INSTINFO *info);
extern Status frcpa_s0_f1_p2_f2_f3Comb(INSTINFO *info);
extern Status frcpa_s1_f1_p2_f2_f3Comb(INSTINFO *info);
extern Status frcpa_s2_f1_p2_f2_f3Comb(INSTINFO *info);
extern Status frcpa_s3_f1_p2_f2_f3Comb(INSTINFO *info);
extern Status fprcpa_s0_f1_p2_f2_f3Comb(INSTINFO *info);
extern Status fprcpa_s1_f1_p2_f2_f3Comb(INSTINFO *info);
extern Status fprcpa_s2_f1_p2_f2_f3Comb(INSTINFO *info);
extern Status fprcpa_s3_f1_p2_f2_f3Comb(INSTINFO *info);
extern Status frsqrta_s0_f1_p2_f3Comb(INSTINFO *info);
extern Status frsqrta_s1_f1_p2_f3Comb(INSTINFO *info);
extern Status frsqrta_s2_f1_p2_f3Comb(INSTINFO *info);
extern Status frsqrta_s3_f1_p2_f3Comb(INSTINFO *info);
extern Status fprsqrta_s0_f1_p2_f3Comb(INSTINFO *info);
extern Status fprsqrta_s1_f1_p2_f3Comb(INSTINFO *info);
extern Status fprsqrta_s2_f1_p2_f3Comb(INSTINFO *info);
extern Status fprsqrta_s3_f1_p2_f3Comb(INSTINFO *info);
extern Status fmin_s0_f1_f2_f3Comb(INSTINFO *info);
extern Status fmin_s1_f1_f2_f3Comb(INSTINFO *info);
extern Status fmin_s2_f1_f2_f3Comb(INSTINFO *info);
extern Status fmin_s3_f1_f2_f3Comb(INSTINFO *info);
extern Status fmax_s0_f1_f2_f3Comb(INSTINFO *info);
extern Status fmax_s1_f1_f2_f3Comb(INSTINFO *info);
extern Status fmax_s2_f1_f2_f3Comb(INSTINFO *info);
extern Status fmax_s3_f1_f2_f3Comb(INSTINFO *info);
extern Status famin_s0_f1_f2_f3Comb(INSTINFO *info);
extern Status famin_s1_f1_f2_f3Comb(INSTINFO *info);
extern Status famin_s2_f1_f2_f3Comb(INSTINFO *info);
extern Status famin_s3_f1_f2_f3Comb(INSTINFO *info);
extern Status famax_s0_f1_f2_f3Comb(INSTINFO *info);
extern Status famax_s1_f1_f2_f3Comb(INSTINFO *info);
extern Status famax_s2_f1_f2_f3Comb(INSTINFO *info);
extern Status famax_s3_f1_f2_f3Comb(INSTINFO *info);
extern Status fpmin_s0_f1_f2_f3Comb(INSTINFO *info);
extern Status fpmin_s1_f1_f2_f3Comb(INSTINFO *info);
extern Status fpmin_s2_f1_f2_f3Comb(INSTINFO *info);
extern Status fpmin_s3_f1_f2_f3Comb(INSTINFO *info);
extern Status fpmax_s0_f1_f2_f3Comb(INSTINFO *info);
extern Status fpmax_s1_f1_f2_f3Comb(INSTINFO *info);
extern Status fpmax_s2_f1_f2_f3Comb(INSTINFO *info);
extern Status fpmax_s3_f1_f2_f3Comb(INSTINFO *info);
extern Status fpamin_s0_f1_f2_f3Comb(INSTINFO *info);
extern Status fpamin_s1_f1_f2_f3Comb(INSTINFO *info);
extern Status fpamin_s2_f1_f2_f3Comb(INSTINFO *info);
extern Status fpamin_s3_f1_f2_f3Comb(INSTINFO *info);
extern Status fpamax_s0_f1_f2_f3Comb(INSTINFO *info);
extern Status fpamax_s1_f1_f2_f3Comb(INSTINFO *info);
extern Status fpamax_s2_f1_f2_f3Comb(INSTINFO *info);
extern Status fpamax_s3_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_eq_s0_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_eq_s1_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_eq_s2_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_eq_s3_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_lt_s0_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_lt_s1_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_lt_s2_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_lt_s3_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_le_s0_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_le_s1_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_le_s2_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_le_s3_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_unord_s0_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_unord_s1_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_unord_s2_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_unord_s3_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_neq_s0_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_neq_s1_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_neq_s2_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_neq_s3_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_nlt_s0_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_nlt_s1_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_nlt_s2_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_nlt_s3_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_nle_s0_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_nle_s1_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_nle_s2_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_nle_s3_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_ord_s0_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_ord_s1_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_ord_s2_f1_f2_f3Comb(INSTINFO *info);
extern Status fpcmp_ord_s3_f1_f2_f3Comb(INSTINFO *info);
extern Status fmerge_s_f1_f2_f3Comb(INSTINFO *info);
extern Status fmerge_ns_f1_f2_f3Comb(INSTINFO *info);
extern Status fmerge_se_f1_f2_f3Comb(INSTINFO *info);
extern Status fmix_lr_f1_f2_f3Comb(INSTINFO *info);
extern Status fmix_r_f1_f2_f3Comb(INSTINFO *info);
extern Status fmix_l_f1_f2_f3Comb(INSTINFO *info);
extern Status fsxt_r_f1_f2_f3Comb(INSTINFO *info);
extern Status fsxt_l_f1_f2_f3Comb(INSTINFO *info);
extern Status fpack_f1_f2_f3Comb(INSTINFO *info);
extern Status fswap_f1_f2_f3Comb(INSTINFO *info);
extern Status fswap_nl_f1_f2_f3Comb(INSTINFO *info);
extern Status fswap_nr_f1_f2_f3Comb(INSTINFO *info);
extern Status fand_f1_f2_f3Comb(INSTINFO *info);
extern Status fandcm_f1_f2_f3Comb(INSTINFO *info);
extern Status for_f1_f2_f3Comb(INSTINFO *info);
extern Status fxor_f1_f2_f3Comb(INSTINFO *info);
extern Status fpmerge_s_f1_f2_f3Comb(INSTINFO *info);
extern Status fpmerge_ns_f1_f2_f3Comb(INSTINFO *info);
extern Status fpmerge_se_f1_f2_f3Comb(INSTINFO *info);
extern Status fcvt_fx_s0_f1_f2Comb(INSTINFO *info);
extern Status fcvt_fx_s1_f1_f2Comb(INSTINFO *info);
extern Status fcvt_fx_s2_f1_f2Comb(INSTINFO *info);
extern Status fcvt_fx_s3_f1_f2Comb(INSTINFO *info);
extern Status fcvt_fxu_s0_f1_f2Comb(INSTINFO *info);
extern Status fcvt_fxu_s1_f1_f2Comb(INSTINFO *info);
extern Status fcvt_fxu_s2_f1_f2Comb(INSTINFO *info);
extern Status fcvt_fxu_s3_f1_f2Comb(INSTINFO *info);
extern Status fcvt_fx_trunc_s0_f1_f2Comb(INSTINFO *info);
extern Status fcvt_fx_trunc_s1_f1_f2Comb(INSTINFO *info);
extern Status fcvt_fx_trunc_s2_f1_f2Comb(INSTINFO *info);
extern Status fcvt_fx_trunc_s3_f1_f2Comb(INSTINFO *info);
extern Status fcvt_fxu_trunc_s0_f1_f2Comb(INSTINFO *info);
extern Status fcvt_fxu_trunc_s1_f1_f2Comb(INSTINFO *info);
extern Status fcvt_fxu_trunc_s2_f1_f2Comb(INSTINFO *info);
extern Status fcvt_fxu_trunc_s3_f1_f2Comb(INSTINFO *info);
extern Status fpcvt_fx_s0_f1_f2Comb(INSTINFO *info);
extern Status fpcvt_fx_s1_f1_f2Comb(INSTINFO *info);
extern Status fpcvt_fx_s2_f1_f2Comb(INSTINFO *info);
extern Status fpcvt_fx_s3_f1_f2Comb(INSTINFO *info);
extern Status fpcvt_fxu_s0_f1_f2Comb(INSTINFO *info);
extern Status fpcvt_fxu_s1_f1_f2Comb(INSTINFO *info);
extern Status fpcvt_fxu_s2_f1_f2Comb(INSTINFO *info);
extern Status fpcvt_fxu_s3_f1_f2Comb(INSTINFO *info);
extern Status fpcvt_fx_trunc_s0_f1_f2Comb(INSTINFO *info);
extern Status fpcvt_fx_trunc_s1_f1_f2Comb(INSTINFO *info);
extern Status fpcvt_fx_trunc_s2_f1_f2Comb(INSTINFO *info);
extern Status fpcvt_fx_trunc_s3_f1_f2Comb(INSTINFO *info);
extern Status fpcvt_fxu_trunc_s0_f1_f2Comb(INSTINFO *info);
extern Status fpcvt_fxu_trunc_s1_f1_f2Comb(INSTINFO *info);
extern Status fpcvt_fxu_trunc_s2_f1_f2Comb(INSTINFO *info);
extern Status fpcvt_fxu_trunc_s3_f1_f2Comb(INSTINFO *info);
extern Status fcvt_xf_f1_f2Comb(INSTINFO *info);
extern Status fsetc_s0_amask7_omask7Comb(INSTINFO *info);
extern Status fsetc_s1_amask7_omask7Comb(INSTINFO *info);
extern Status fsetc_s2_amask7_omask7Comb(INSTINFO *info);
extern Status fsetc_s3_amask7_omask7Comb(INSTINFO *info);
extern Status fclrf_s0Comb(INSTINFO *info);
extern Status fclrf_s1Comb(INSTINFO *info);
extern Status fclrf_s2Comb(INSTINFO *info);
extern Status fclrf_s3Comb(INSTINFO *info);
extern Status fchkf_s0_target25Comb(INSTINFO *info);
extern Status fchkf_s1_target25Comb(INSTINFO *info);
extern Status fchkf_s2_target25Comb(INSTINFO *info);
extern Status fchkf_s3_target25Comb(INSTINFO *info);
#define break_f_imm21Comb break_i_imm21Comb
#define nop_f_imm21Comb nop_i_imm21Comb
#define hint_f_imm21Comb hint_i_imm21Comb
extern Status break_x_imm62Comb(INSTINFO *info);
extern Status movl_r1_imm64Comb(INSTINFO *info);
extern Status brl_cond_spnt_few_target64Comb(INSTINFO *info);
#define brl_cond_spnt_many_target64Comb brl_cond_spnt_few_target64Comb
#define brl_cond_sptk_few_target64Comb brl_cond_spnt_few_target64Comb
#define brl_cond_sptk_many_target64Comb brl_cond_spnt_few_target64Comb
#define brl_cond_dpnt_few_target64Comb brl_cond_spnt_few_target64Comb
#define brl_cond_dpnt_many_target64Comb brl_cond_spnt_few_target64Comb
#define brl_cond_dptk_few_target64Comb brl_cond_spnt_few_target64Comb
#define brl_cond_dptk_many_target64Comb brl_cond_spnt_few_target64Comb
#define brl_cond_spnt_few_clr_target64Comb brl_cond_spnt_few_target64Comb
#define brl_cond_spnt_many_clr_target64Comb brl_cond_spnt_few_target64Comb
#define brl_cond_sptk_few_clr_target64Comb brl_cond_spnt_few_target64Comb
#define brl_cond_sptk_many_clr_target64Comb brl_cond_spnt_few_target64Comb
#define brl_cond_dpnt_few_clr_target64Comb brl_cond_spnt_few_target64Comb
#define brl_cond_dpnt_many_clr_target64Comb brl_cond_spnt_few_target64Comb
#define brl_cond_dptk_few_clr_target64Comb brl_cond_spnt_few_target64Comb
#define brl_cond_dptk_many_clr_target64Comb brl_cond_spnt_few_target64Comb
extern Status brl_call_spnt_few_b1_target64Comb(INSTINFO *info);
#define brl_call_spnt_many_b1_target64Comb brl_call_spnt_few_b1_target64Comb
#define brl_call_sptk_few_b1_target64Comb brl_call_spnt_few_b1_target64Comb
#define brl_call_sptk_many_b1_target64Comb brl_call_spnt_few_b1_target64Comb
#define brl_call_dpnt_few_b1_target64Comb brl_call_spnt_few_b1_target64Comb
#define brl_call_dpnt_many_b1_target64Comb brl_call_spnt_few_b1_target64Comb
#define brl_call_dptk_few_b1_target64Comb brl_call_spnt_few_b1_target64Comb
#define brl_call_dptk_many_b1_target64Comb brl_call_spnt_few_b1_target64Comb
#define brl_call_spnt_few_clr_b1_target64Comb brl_call_spnt_few_b1_target64Comb
#define brl_call_spnt_many_clr_b1_target64Comb brl_call_spnt_few_b1_target64Comb
#define brl_call_sptk_few_clr_b1_target64Comb brl_call_spnt_few_b1_target64Comb
#define brl_call_sptk_many_clr_b1_target64Comb brl_call_spnt_few_b1_target64Comb
#define brl_call_dpnt_few_clr_b1_target64Comb brl_call_spnt_few_b1_target64Comb
#define brl_call_dpnt_many_clr_b1_target64Comb brl_call_spnt_few_b1_target64Comb
#define brl_call_dptk_few_clr_b1_target64Comb brl_call_spnt_few_b1_target64Comb
#define brl_call_dptk_many_clr_b1_target64Comb brl_call_spnt_few_b1_target64Comb
#define nop_x_imm62Comb nop_i_imm21Comb
#define hint_x_imm62Comb hint_i_imm21Comb
