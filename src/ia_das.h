/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator IA Instruction Disassembly Header
 *
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
#ifndef _SKI_IA_DAS_H
#define _SKI_IA_DAS_H

void add_ExGx_das(IAinstInfoPtr info, char *str);	/* 00 - 01 */
void add_GxEx_das(IAinstInfoPtr info, char *str);	/* 02 - 03 */
void add_reg_imm_das(IAinstInfoPtr info, char *str);	/* 04 - 05 */
void push_seg_das(IAinstInfoPtr info, char *str);	/* 06 */
void pop_seg_das(IAinstInfoPtr info, char *str);	/* 07 */
void or_ExGx_das(IAinstInfoPtr info, char *str);	/* 08 - 09 */
void or_GxEx_das(IAinstInfoPtr info, char *str);	/* 0A - 0B */
void or_reg_imm_das(IAinstInfoPtr info, char *str);	/* 0C - 0D */
/* push_CS uses push_seg_das (06) */			/* 0E */
/* two-byte escape */					/* 0F */

void adc_ExGx_das(IAinstInfoPtr info, char *str);	/* 10 - 11 */
void adc_GxEx_das(IAinstInfoPtr info, char *str);	/* 12 - 13 */
void adc_reg_imm_das(IAinstInfoPtr info, char *str);	/* 14 - 15 */
/* push_SS uses push_seg_das (06) */			/* 16 */
/* pop_SS uses pop_seg_das (07) */			/* 17 */
void sbb_ExGx_das(IAinstInfoPtr info, char *str);	/* 18 - 19 */
void sbb_GxEx_das(IAinstInfoPtr info, char *str);	/* 1A - 1B */
void sbb_reg_imm_das(IAinstInfoPtr info, char *str);	/* 1C - 1D */
/* push_DS uses push_seg_das (06) */			/* 1E */
/* pop_DS uses pop_seg_das (07) */			/* 1F */

void and_ExGx_das(IAinstInfoPtr info, char *str);	/* 20 - 21 */
void and_GxEx_das(IAinstInfoPtr info, char *str);	/* 22 - 23 */
void and_reg_imm_das(IAinstInfoPtr info, char *str);	/* 24 - 25 */
/* ES segment override */				/* 26 */
void daa_das(IAinstInfoPtr info, char *str);		/* 27 */
void sub_ExGx_das(IAinstInfoPtr info, char *str);	/* 28 - 29 */
void sub_GxEx_das(IAinstInfoPtr info, char *str);	/* 2A - 2B */
void sub_reg_imm_das(IAinstInfoPtr info, char *str);	/* 2C - 2D */
/* CS segment override */				/* 2E */
void das_das(IAinstInfoPtr info, char *str);		/* 2F */

void xor_ExGx_das(IAinstInfoPtr info, char *str);	/* 30 - 31 */
void xor_GxEx_das(IAinstInfoPtr info, char *str);	/* 32 - 33 */
void xor_reg_imm_das(IAinstInfoPtr info, char *str);	/* 34 - 35 */
/* SS segment override */				/* 36 */
void aaa_das(IAinstInfoPtr info, char *str);		/* 37 */
void cmp_ExGx_das(IAinstInfoPtr info, char *str);	/* 38 - 39 */
void cmp_GxEx_das(IAinstInfoPtr info, char *str);	/* 3A - 3B */
void cmp_reg_imm_das(IAinstInfoPtr info, char *str);	/* 3C - 3D */
/* DS segment override */				/* 3E */
void aas_das(IAinstInfoPtr info, char *str);		/* 3F */

void inc_reg_das(IAinstInfoPtr info, char *str);	/* 40 - 47 */
void dec_reg_das(IAinstInfoPtr info, char *str);	/* 48 - 4F */

void push_reg_das(IAinstInfoPtr info, char *str);	/* 50 - 57 */
void pop_reg_das(IAinstInfoPtr info, char *str);	/* 58 - 5F */

void pusha_das(IAinstInfoPtr info, char *str);		/* 60 */
void popa_das(IAinstInfoPtr info, char *str);		/* 61 */
void bound_GvMa_das(IAinstInfoPtr info, char *str);	/* 62 */
void arpl_EwGw_das(IAinstInfoPtr info, char *str);	/* 63 */
/* FS segment override */				/* 64 */
/* GS segment override */				/* 65 */
/* operand size override */				/* 66 */
/* address size override */				/* 67 */
void push_imm_das(IAinstInfoPtr info, char *str);	/* 68 */
void imul_GvEvIv_das(IAinstInfoPtr info, char *str);	/* 69 */
/* push_Ib uses push_imm_das (68) */			/* 6A */
void imul_GvEvIb_das(IAinstInfoPtr info, char *str);	/* 6B */
void ins_YxDX_das(IAinstInfoPtr info, char *str);	/* 6C - 6D */
void outs_DXXx_das(IAinstInfoPtr info, char *str);	/* 6E - 6F */

void jcc_das(IAinstInfoPtr info, char *str);		/* 70 - 7F */

void group1_das(IAinstInfoPtr info, char *str);		/* 80 */
/* group1_EvIv uses group1_das (80) */			/* 81 */
void movb_ALIb_das(IAinstInfoPtr info, char *str);	/* 82 */
/* group1_EvIb uses group1_das (80) */			/* 83 */
void test_ExGx_das(IAinstInfoPtr info, char *str);	/* 84 - 85 */
void xchg_ExGx_das(IAinstInfoPtr info, char *str);	/* 86 - 87 */
void mov_ExGx_das(IAinstInfoPtr info, char *str);	/* 88 - 89 */
void mov_GxEx_das(IAinstInfoPtr info, char *str);	/* 8A - 8B */
void mov_EwSw_das(IAinstInfoPtr info, char *str);	/* 8C */
void lea_GvM_das(IAinstInfoPtr info, char *str);	/* 8D */
void mov_SwEw_das(IAinstInfoPtr info, char *str);	/* 8E */
void pop_Ev_das(IAinstInfoPtr info, char *str);		/* 8F */

void nop_das(IAinstInfoPtr info, char *str);		/* 90 */
void xchg_reg_reg_das(IAinstInfoPtr info, char *str);	/* 91 - 97 */
void cbw_das(IAinstInfoPtr info, char *str);		/* 98 */
void cwd_das(IAinstInfoPtr info, char *str);		/* 99 */
void call_Ap_das(IAinstInfoPtr info, char *str);	/* 9A */
void wait_das(IAinstInfoPtr info, char *str);		/* 9B */
void pushf_Fv_das(IAinstInfoPtr info, char *str);	/* 9C */
void popf_Fv_das(IAinstInfoPtr info, char *str);	/* 9D */
void sahf_das(IAinstInfoPtr info, char *str);		/* 9E */
void lahf_das(IAinstInfoPtr info, char *str);		/* 9F */

void mov_reg_Ox_das(IAinstInfoPtr info, char *str);	/* A0 - A1 */
void mov_Ox_reg_das(IAinstInfoPtr info, char *str);	/* A2 - A3 */
void movs_YxXx_das(IAinstInfoPtr info, char *str);	/* A4 - A5 */
void cmps_XxYx_das(IAinstInfoPtr info, char *str);	/* A6 - A7 */
void test_reg_imm_das(IAinstInfoPtr info, char *str);	/* A8 - A9 */
void stos_Yx_reg_das(IAinstInfoPtr info, char *str);	/* AA - AB */
void lods_reg_Xx_das(IAinstInfoPtr info, char *str);	/* AC -AD */
void scas_reg_Yx_das(IAinstInfoPtr info, char *str);	/* AE - AF */

void mov_reg_imm_das(IAinstInfoPtr info, char *str);	/* B0 - BF */

void group2_ExIb_das(IAinstInfoPtr info, char *str);	/* C0 - C1 */
void ret_near_Iw_das(IAinstInfoPtr info, char *str);	/* C2 */
void ret_near_das(IAinstInfoPtr info, char *str);	/* C3 */
void load_far_ptr_das(IAinstInfoPtr info, char *str);	/* C4 */
/* lds_GvMp uses load_far_ptr_das (C4) */		/* C5 */
void mov_ExIx_das(IAinstInfoPtr info, char *str);	/* C6 - C7 */
void enter_IwIb_das(IAinstInfoPtr info, char *str);	/* C8 */
void leave_das(IAinstInfoPtr info, char *str);		/* C9 */
void ret_far_Iw_das(IAinstInfoPtr info, char *str);	/* CA */
void ret_far_das(IAinstInfoPtr info, char *str);	/* CB */
/* int3 uses int_Ib_das (CD) */				/* CC */
void int_Ib_das(IAinstInfoPtr info, char *str);		/* CD */
void into_das(IAinstInfoPtr info, char *str);		/* CE */
void iret_das(IAinstInfoPtr info, char *str);		/* CF */

/* group2_Eb1 uses group2_ExIb_das (C0 - C1) */		/* D0 */
/* group2_Ev1 uses group2_ExIb_das (C0 - C1) */		/* D1 */
void group2_ExCL_das(IAinstInfoPtr info, char *str);	/* D2 - D3 */
void aam_Ib_das(IAinstInfoPtr info, char *str);		/* D4 */
void aad_Ib_das(IAinstInfoPtr info, char *str);		/* D5 */
void salc_das(IAinstInfoPtr info, char *str);		/* D6 */
void xlat_das(IAinstInfoPtr info, char *str);		/* D7 */
void esc_D8_das(IAinstInfoPtr info, char *str);		/* D8 */
void esc_D9_das(IAinstInfoPtr info, char *str);		/* D9 */
void esc_DA_das(IAinstInfoPtr info, char *str);		/* DA */
void esc_DB_das(IAinstInfoPtr info, char *str);		/* DB */
void esc_DC_das(IAinstInfoPtr info, char *str);		/* DC */
void esc_DD_das(IAinstInfoPtr info, char *str);		/* DD */
void esc_DE_das(IAinstInfoPtr info, char *str);		/* DE */
void esc_DF_das(IAinstInfoPtr info, char *str);		/* DF */

void loop_das(IAinstInfoPtr info, char *str);		/* E0 - E2 */
void jcxz_Jb_das(IAinstInfoPtr info, char *str);	/* E3 */
void in_reg_imm_das(IAinstInfoPtr info, char *str);	/* E4 - E5 */
void out_imm_reg_das(IAinstInfoPtr info, char *str);	/* E6 - E7 */
void call_Jv_das(IAinstInfoPtr info, char *str);	/* E8 */
void jmp_Jv_das(IAinstInfoPtr info, char *str);		/* E9 */
void jmp_Ap_das(IAinstInfoPtr info, char *str);		/* EA */
void jmp_Jb_das(IAinstInfoPtr info, char *str);		/* EB */
void in_reg_DX_das(IAinstInfoPtr info, char *str);	/* EC - ED */
void out_DX_reg_das(IAinstInfoPtr info, char *str);	/* EE - EF */

void lock_das(IAinstInfoPtr info, char *str);		/* F0 */
void reserved_das(IAinstInfoPtr info, char *str);	/* F1 */
/* repne prefix */					/* F2 */
/* rep prefix */					/* F3 */
void hlt_das(IAinstInfoPtr info, char *str);		/* F4 */
void cmc_das(IAinstInfoPtr info, char *str);		/* F5 */
void group3_das(IAinstInfoPtr info, char *str);		/* F6 - F7 */
void clc_das(IAinstInfoPtr info, char *str);		/* F8 */
void stc_das(IAinstInfoPtr info, char *str);		/* F9 */
void cli_das(IAinstInfoPtr info, char *str);		/* FA */
void sti_das(IAinstInfoPtr info, char *str);		/* FB */
void cld_das(IAinstInfoPtr info, char *str);		/* FC */
void std_das(IAinstInfoPtr info, char *str);		/* FD */
void group4_das(IAinstInfoPtr info, char *str);		/* FE */
void group5_das(IAinstInfoPtr info, char *str);		/* FF */

void group6_das(IAinstInfoPtr info, char *str);		/* 0F,00 */
void group7_das(IAinstInfoPtr info, char *str);		/* 0F,01 */
void lar_GvEw_das(IAinstInfoPtr info, char *str);	/* 0F,02 */
void lsl_GvEw_das(IAinstInfoPtr info, char *str);	/* 0F,03 */
void clts_das(IAinstInfoPtr info, char *str);		/* 0F,06 */
void invd_das(IAinstInfoPtr info, char *str);		/* 0F,08 */
void wbinvd_das(IAinstInfoPtr info, char *str);		/* 0F,09 */
void ud2_das(IAinstInfoPtr info, char *str);		/* 0F,0B */

void mov_RdCd_das(IAinstInfoPtr info, char *str);	/* 0F,20 */
void mov_RdDd_das(IAinstInfoPtr info, char *str);	/* 0F,21 */
void mov_CdRd_das(IAinstInfoPtr info, char *str);	/* 0F,22 */
void mov_DdRd_das(IAinstInfoPtr info, char *str);	/* 0F,23 */
void mov_RdTd_das(IAinstInfoPtr info, char *str);	/* 0F,24 */
void mov_TdRd_das(IAinstInfoPtr info, char *str);	/* 0F,26 */

void wrmsr_das(IAinstInfoPtr info, char *str);		/* 0F,30 */
void rdtsc_das(IAinstInfoPtr info, char *str);		/* 0F,31 */
void rdmsr_das(IAinstInfoPtr info, char *str);		/* 0F,32 */
void rdpmc_das(IAinstInfoPtr info, char *str);		/* 0F,33 */

void cmovcc_das(IAinstInfoPtr info, char *str);		/* 0F,40 - 0F,4F */

/* uses jcc_das (70 - 7F) */				/* 0F,80 - 0F,8F */

void setcc_das(IAinstInfoPtr info, char *str);		/* 0F,90 - 0F,9F */

/* push_FS uses push_seg_das (06) */			/* 0F,A0 */
/* pop_FS uses pop_seg_das (07) */			/* 0F,A1 */
void cpuid_das(IAinstInfoPtr info, char *str);		/* 0F,A2 */
void bt_EvGv_das(IAinstInfoPtr info, char *str);	/* 0F,A3 */
void shld_EvGvIb_das(IAinstInfoPtr info, char *str);	/* 0F,A4 */
void shld_EvGvCL_das(IAinstInfoPtr info, char *str);	/* 0F,A5 */
void cmpxchg_XBTS_das(IAinstInfoPtr info, char *str);	/* 0F,A6 */
void cmpxchg_IBTS_das(IAinstInfoPtr info, char *str);	/* 0F,A7 */
/* push_GS uses push_seg_das (06) */			/* 0F,A8 */
/* pop_GS uses pop_seg_das (07) */			/* 0F,A9 */
void rsm_das(IAinstInfoPtr info, char *str);		/* 0F,AA */
void bts_EvGv_das(IAinstInfoPtr info, char *str);	/* 0F,AB */
void shrd_EvGvIb_das(IAinstInfoPtr info, char *str);	/* 0F,AC */
void shrd_EvGvCL_das(IAinstInfoPtr info, char *str);	/* 0F,AD */
void imul_GvEv_das(IAinstInfoPtr info, char *str);	/* 0F,AF */

void cmpxchg_ExGx_das(IAinstInfoPtr info, char *str);	/* 0F,B0 - 0F,B1 */
/* lss_GvMp uses load_far_ptr_das (C4) */		/* 0F,B2 */
void btr_EvGv_das(IAinstInfoPtr info, char *str);	/* 0F,B3 */
/* lfs_GvMp uses load_far_ptr_das (C4) */		/* 0F,B4 */
/* lgs_GvMp uses load_far_ptr_das (C4) */		/* 0F,B5 */
void movzx_GvEb_das(IAinstInfoPtr info, char *str);	/* 0F,B6 */
void movzx_GvEw_das(IAinstInfoPtr info, char *str);	/* 0F,B7 */
void jmpe_Jv_das(IAinstInfoPtr info, char *str);	/* 0F,B8 */
void group8_das(IAinstInfoPtr info, char *str);		/* 0F,BA */
void btc_EvGv_das(IAinstInfoPtr info, char *str);	/* 0F,BB */
void bsf_GvEv_das(IAinstInfoPtr info, char *str);	/* 0F,BC */
void bsr_GvEv_das(IAinstInfoPtr info, char *str);	/* 0F,BD */
void movsx_GvEb_das(IAinstInfoPtr info, char *str);	/* 0F,BE */
void movsx_GvEw_das(IAinstInfoPtr info, char *str);	/* 0F,BF */

void xadd_ExGx_das(IAinstInfoPtr info, char *str);	/* 0F,C0 - 0F,C1 */
void group9_das(IAinstInfoPtr info, char *str);		/* 0F,C7 */
void bswap_reg_das(IAinstInfoPtr info, char *str);	/* 0F,C8 - 0F,CF */

#endif	/* _SKI_IA_DAS_H */
