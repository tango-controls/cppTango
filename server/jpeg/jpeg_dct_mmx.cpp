///=============================================================================	
//
// file :		jpeg_dct_mmx.cpp
//
// description :        Simple jpeg coding/decoding library
//                      Discrete Cosine Transform (8x8) MMX code
//
// project :		TANGO
//
// author(s) :		JL Pons
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009
//                      European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
// $Log$
//=============================================================================

// MMX implementation has been provided by Intel at AP-922

#include "jpeg_lib.h"

#ifdef JPG_USE_ASM

#ifdef _WINDOWS
// Visual C++ align directive
#define ALIGN8 __declspec(align(8))
#else
// gcc align directive
#define ALIGN8 __attribute__ ((aligned (8)))
#endif

#define BITS_FRW_ACC   3 //// 2 or 3 for accuracy
#define SHIFT_FRW_COL  BITS_FRW_ACC
#define SHIFT_FRW_ROW  (BITS_FRW_ACC + 14)
#define RND_FRW_ROW    (1 << (SHIFT_FRW_ROW-1))

// MMX constants

ALIGN8 short __jpmm_one_corr[] = {1,1,1,1};
ALIGN8 long  __jpmm_round_frw_row[] = {RND_FRW_ROW,RND_FRW_ROW};

ALIGN8 short __jpmm_tg_1_16[]   = {  13036,  13036,  13036,  13036 }; //tg * (2<<16) + 0.5
ALIGN8 short __jpmm_tg_2_16[]   = {  27146,  27146,  27146,  27146 }; //tg * (2<<16) + 0.5
ALIGN8 short __jpmm_tg_3_16[]   = { -21746, -21746, -21746, -21746 }; //tg * (2<<16) + 0.5
ALIGN8 short __jpmm_cos_4_16[]  = { -19195, -19195, -19195, -19195 }; //cos * (2<<16) + 0.5
ALIGN8 short __jpmm_ocos_4_16[] = {  23170,  23170,  23170,  23170 }; //cos * (2<<15) + 0.5

ALIGN8 short __jpmm_row_tab_frw[] = {  // forward_dct coeff table

    //row0
    16384, 16384, 21407, -8867,     //    w09 w01 w08 w00
    16384, 16384, 8867, -21407,     //    w13 w05 w12 w04
    16384, -16384, 8867, 21407,     //    w11 w03 w10 w02
    -16384, 16384, -21407, -8867,   //    w15 w07 w14 w06
    22725, 12873, 19266, -22725,    //    w22 w20 w18 w16
    19266, 4520, -4520, -12873,     //    w23 w21 w19 w17
    12873, 4520, 4520, 19266,       //    w30 w28 w26 w24
    -22725, 19266, -12873, -22725,  //    w31 w29 w27 w25

    //row1
    22725, 22725, 29692, -12299,    //    w09 w01 w08 w00
    22725, 22725, 12299, -29692,    //    w13 w05 w12 w04
    22725, -22725, 12299, 29692,    //    w11 w03 w10 w02
    -22725, 22725, -29692, -12299,  //    w15 w07 w14 w06
    31521, 17855, 26722, -31521,    //    w22 w20 w18 w16
    26722, 6270, -6270, -17855,     //    w23 w21 w19 w17
    17855, 6270, 6270, 26722,       //    w30 w28 w26 w24
    -31521, 26722, -17855, -31521,  //    w31 w29 w27 w25

    //row2
    21407, 21407, 27969, -11585,    //    w09 w01 w08 w00
    21407, 21407, 11585, -27969,    //    w13 w05 w12 w04
    21407, -21407, 11585, 27969,    //    w11 w03 w10 w02
    -21407, 21407, -27969, -11585,  //    w15 w07 w14 w06
    29692, 16819, 25172, -29692,    //    w22 w20 w18 w16
    25172, 5906, -5906, -16819,     //    w23 w21 w19 w17
    16819, 5906, 5906, 25172,       //    w30 w28 w26 w24
    -29692, 25172, -16819, -29692,  //    w31 w29 w27 w25

    //row3
    19266, 19266, 25172, -10426,    //    w09 w01 w08 w00
    19266, 19266, 10426, -25172,    //    w13 w05 w12 w04
    19266, -19266, 10426, 25172,    //    w11 w03 w10 w02
    -19266, 19266, -25172, -10426,  //    w15 w07 w14 w06, 
    26722, 15137, 22654, -26722,    //    w22 w20 w18 w16
    22654, 5315, -5315, -15137,     //    w23 w21 w19 w17
    15137, 5315, 5315, 22654,       //    w30 w28 w26 w24
    -26722, 22654, -15137, -26722,  //    w31 w29 w27 w25, 

    //row4
    16384, 16384, 21407, -8867,     //    w09 w01 w08 w00
    16384, 16384, 8867, -21407,     //    w13 w05 w12 w04
    16384, -16384, 8867, 21407,     //    w11 w03 w10 w02
    -16384, 16384, -21407, -8867,   //    w15 w07 w14 w06
    22725, 12873, 19266, -22725,    //    w22 w20 w18 w16
    19266, 4520, -4520, -12873,     //    w23 w21 w19 w17
    12873, 4520, 4520, 19266,       //    w30 w28 w26 w24
    -22725, 19266, -12873, -22725,  //    w31 w29 w27 w25 

    //row5
    19266, 19266, 25172, -10426,    //    w09 w01 w08 w00
    19266, 19266, 10426, -25172,    //    w13 w05 w12 w04
    19266, -19266, 10426, 25172,    //    w11 w03 w10 w02
    -19266, 19266, -25172, -10426,  //    w15 w07 w14 w06
    26722, 15137, 22654, -26722,    //    w22 w20 w18 w16
    22654, 5315, -5315, -15137,     //    w23 w21 w19 w17
    15137, 5315, 5315, 22654,       //    w30 w28 w26 w24
    -26722, 22654, -15137, -26722,  //    w31 w29 w27 w25

    //row6
    21407, 21407, 27969, -11585,    //    w09 w01 w08 w00
    21407, 21407, 11585, -27969,    //    w13 w05 w12 w04
    21407, -21407, 11585, 27969,    //    w11 w03 w10 w02
    -21407, 21407, -27969, -11585,  //    w15 w07 w14 w06, 
    29692, 16819, 25172, -29692,    //    w22 w20 w18 w16
    25172, 5906, -5906, -16819,     //    w23 w21 w19 w17
    16819, 5906, 5906, 25172,       //    w30 w28 w26 w24
    -29692, 25172, -16819, -29692,  //    w31 w29 w27 w25,

    //row7
    22725, 22725, 29692, -12299,    //    w09 w01 w08 w00
    22725, 22725, 12299, -29692,    //    w13 w05 w12 w04
    22725, -22725, 12299, 29692,    //    w11 w03 w10 w02
    -22725, 22725, -29692, -12299,  //    w15 w07 w14 w06,
    31521, 17855, 26722, -31521,    //    w22 w20 w18 w16
    26722, 6270, -6270, -17855,     //    w23 w21 w19 w17
    17855, 6270, 6270, 26722,       //    w30 w28 w26 w24
    -31521, 26722, -17855, -31521   //    w31 w29 w27 w25
};

void jpeg_fdct_mmx( short *block )
{

#ifdef _WINDOWS

  // Visual C++ inline assembly code
  __asm {

    // Columns

    mov eax, block

    lea ebx, __jpmm_row_tab_frw
    mov ecx, eax

    movq mm0, [eax + 1*16] //; 0 ; x1

    movq mm1, [eax + 6*16] //; 1 ; x6
     movq mm2, mm0 //; 2 ; x1

    movq mm3, [eax + 2*16] //; 3 ; x2
     paddsw mm0, mm1 //; t1 = x[1] + x[6]

    movq mm4, [eax + 5*16] //; 4 ; x5
     psllw mm0, SHIFT_FRW_COL //; t1

    movq mm5, [eax + 0*16] //; 5 ; x0
     paddsw mm4, mm3 //; t2 = x[2] + x[5]

    paddsw mm5, [eax + 7*16] // t0 = x[0] + x[7]
     psllw mm4, SHIFT_FRW_COL // t2

    movq mm6, mm0 // 6 ; t1
     psubsw mm2, mm1 // 1 ; t6 = x[1] - x[6]

    movq mm1, __jpmm_tg_2_16 // 1 ; tg_2_16
     psubsw mm0, mm4 // tm12 = t1 - t2

    movq mm7, [eax + 3*16] // 7 ; x3
     pmulhw mm1, mm0 // tm12*tg_2_16

    paddsw mm7, [eax + 4*16] // t3 = x[3] + x[4]
     psllw mm5, SHIFT_FRW_COL // t0

    paddsw mm6, mm4 // 4 ; tp12 = t1 + t2
     psllw mm7, SHIFT_FRW_COL // t3

    movq mm4, mm5 // 4 ; t0
     psubsw mm5, mm7 // tm03 = t0 - t3

    paddsw mm1, mm5 // y2 = tm03 + tm12*tg_2_16
     paddsw mm4, mm7 // 7 ; tp03 = t0 + t3

    por mm1, __jpmm_one_corr // correction y2 +0.5
     psllw mm2, SHIFT_FRW_COL+1 // t6

    pmulhw mm5, __jpmm_tg_2_16 // tm03*tg_2_16
     movq mm7, mm4 // 7 // tp03

    psubsw mm3, [eax + 5*16] // t5 = x[2] - x[5]
     psubsw mm4, mm6 // y4 = tp03 - tp12

    movq [ecx + 2*16], mm1 // 1 // save y2
     paddsw mm7, mm6 // 6 // y0 = tp03 + tp12

    movq mm1, [eax + 3*16] // 1 // x3
     psllw mm3, SHIFT_FRW_COL+1 // t5

    psubsw mm1, [eax + 4*16] // t4 = x[3] - x[4]
     movq mm6, mm2 // 6 // t6

    movq [ecx + 4*16], mm4 // 4 // save y4
     paddsw mm2, mm3 // t6 + t5

    pmulhw mm2, __jpmm_ocos_4_16 // tp65 = (t6 + t5)*cos_4_16
     psubsw mm6, mm3 // 3 // t6 - t5

    pmulhw mm6, __jpmm_ocos_4_16 // tm65 = (t6 - t5)*cos_4_16
     psubsw mm5, mm0 // 0 // y6 = tm03*tg_2_16 - tm12

    por mm5,  __jpmm_one_corr // correction y6 +0.5
     psllw mm1, SHIFT_FRW_COL // t4

    por mm2,  __jpmm_one_corr // correction tp65 +0.5
     movq mm4, mm1 // 4 // t4

    movq mm3, [eax + 0*16] // 3 // x0
     paddsw mm1, mm6 // tp465 = t4 + tm65

    psubsw mm3, [eax + 7*16] // t7 = x[0] - x[7]
     psubsw mm4, mm6 // 6 // tm465 = t4 - tm65

    movq mm0, __jpmm_tg_1_16 // 0 // tg_1_16
     psllw mm3, SHIFT_FRW_COL // t7

    movq mm6, __jpmm_tg_3_16 // 6 // tg_3_16
     pmulhw mm0, mm1 // tp465*tg_1_16

    movq [ecx + 0*16], mm7 // 7 // save y0
     pmulhw mm6, mm4 // tm465*tg_3_16

    movq [ecx + 6*16], mm5 // 5 // save y6
     movq mm7, mm3 // 7 // t7

    movq mm5, __jpmm_tg_3_16 // 5 // tg_3_16
     psubsw mm7, mm2 // tm765 = t7 - tp65

    paddsw mm3, mm2 // 2 // tp765 = t7 + tp65
     pmulhw mm5, mm7 // tm765*tg_3_16

    paddsw mm0, mm3 // y1 = tp765 + tp465*tg_1_16
     paddsw mm6, mm4 // tm465*tg_3_16

    pmulhw mm3, __jpmm_tg_1_16 // tp765*tg_1_16

    por mm0, __jpmm_one_corr // correction y1 +0.5
     paddsw mm5, mm7 // tm765*tg_3_16

    psubsw mm7, mm6 // 6 // y3 = tm765 - tm465*tg_3_16
     add eax, 0x08   // // increment pointer

    movq [ecx + 1*16], mm0 // 0 // save y1
     paddsw mm5, mm4 // 4 // y5 = tm765*tg_3_16 + tm465

    movq [ecx + 3*16], mm7 // 7 // save y3
     psubsw mm3, mm1 // 1 // y7 = tp765*tg_1_16 - tp465

    movq [ecx + 5*16], mm5 // 5 // save y5


    movq mm0, [eax + 1*16] // 0 // x1
    movq [ecx + 7*16], mm3 // 3 // save y7 (columns 0-4)

    movq mm1, [eax + 6*16] // 1 // x6
     movq mm2, mm0 // 2 // x1

    movq mm3, [eax + 2*16] // 3 // x2
     paddsw mm0, mm1 // t1 = x[1] + x[6]

    movq mm4, [eax + 5*16] // 4 // x5
     psllw mm0, SHIFT_FRW_COL // t1

    movq mm5, [eax + 0*16] // 5 // x0
     paddsw mm4, mm3 // t2 = x[2] + x[5]

    paddsw mm5, [eax + 7*16] // t0 = x[0] + x[7]
     psllw mm4, SHIFT_FRW_COL // t2

    movq mm6, mm0 // 6 // t1
     psubsw mm2, mm1 // 1 // t6 = x[1] - x[6]

    movq mm1, __jpmm_tg_2_16 // 1 // tg_2_16
     psubsw mm0, mm4 // tm12 = t1 - t2

    movq mm7, [eax + 3*16] // 7 // x3
     pmulhw mm1, mm0 // tm12*tg_2_16

    paddsw mm7, [eax + 4*16] // t3 = x[3] + x[4]
     psllw mm5, SHIFT_FRW_COL // t0

    paddsw mm6, mm4 // 4 // tp12 = t1 + t2
     psllw mm7, SHIFT_FRW_COL // t3

    movq mm4, mm5 // 4 // t0
     psubsw mm5, mm7 // tm03 = t0 - t3

    paddsw mm1, mm5 // y2 = tm03 + tm12*tg_2_16
     paddsw mm4, mm7 // 7 // tp03 = t0 + t3

    por mm1, __jpmm_one_corr // correction y2 +0.5
     psllw mm2, SHIFT_FRW_COL+1 // t6

    pmulhw mm5, __jpmm_tg_2_16 // tm03*tg_2_16
     movq mm7, mm4 // 7 // tp03

    psubsw mm3, [eax + 5*16] // t5 = x[2] - x[5]
     psubsw mm4, mm6 // y4 = tp03 - tp12

    movq [ecx + 2*16+8], mm1 // 1 // save y2
     paddsw mm7, mm6 // 6 // y0 = tp03 + tp12
     
    movq mm1, [eax + 3*16] // 1 // x3
     psllw mm3, SHIFT_FRW_COL+1 // t5

    psubsw mm1, [eax + 4*16] // t4 = x[3] - x[4]
     movq mm6, mm2 // 6 // t6
    
    movq [ecx + 4*16+8], mm4 // 4 // save y4
     paddsw mm2, mm3 // t6 + t5

    pmulhw mm2, __jpmm_ocos_4_16 // tp65 = (t6 + t5)*cos_4_16
     psubsw mm6, mm3 // 3 // t6 - t5

    pmulhw mm6, __jpmm_ocos_4_16 // tm65 = (t6 - t5)*cos_4_16
     psubsw mm5, mm0 // 0 // y6 = tm03*tg_2_16 - tm12

    por mm5, __jpmm_one_corr // correction y6 +0.5
     psllw mm1, SHIFT_FRW_COL // t4

    por mm2, __jpmm_one_corr // correction tp65 +0.5
     movq mm4, mm1 // 4 // t4

    movq mm3, [eax + 0*16] // 3 // x0
     paddsw mm1, mm6 // tp465 = t4 + tm65

    psubsw mm3, [eax + 7*16] // t7 = x[0] - x[7]
     psubsw mm4, mm6 // 6 // tm465 = t4 - tm65

    movq mm0, __jpmm_tg_1_16 // 0 // tg_1_16
     psllw mm3, SHIFT_FRW_COL // t7

    movq mm6, __jpmm_tg_3_16 // 6 // tg_3_16
     pmulhw mm0, mm1 // tp465*tg_1_16

    movq [ecx +8], mm7 // 7 // save y0
     pmulhw mm6, mm4 // tm465*tg_3_16

    movq [ecx + 6*16+8], mm5 // 5 // save y6
     movq mm7, mm3 // 7 // t7

    movq mm5, __jpmm_tg_3_16 // 5 // tg_3_16
     psubsw mm7, mm2 // tm765 = t7 - tp65

    paddsw mm3, mm2 // 2 // tp765 = t7 + tp65
     pmulhw mm5, mm7 // tm765*tg_3_16

    paddsw mm0, mm3 // y1 = tp765 + tp465*tg_1_16
     paddsw mm6, mm4 // tm465*tg_3_16

    pmulhw mm3, __jpmm_tg_1_16 // tp765*tg_1_16

    por mm0, __jpmm_one_corr // correction y1 +0.5
     paddsw mm5, mm7 // tm765*tg_3_16

    psubsw mm7, mm6 // 6 // y3 = tm765 - tm465*tg_3_16

    movq [ecx + 1*16+8], mm0 // 0 // save y1
     paddsw mm5, mm4 // 4 // y5 = tm765*tg_3_16 + tm465

    movq [ecx + 3*16+8], mm7 // 7 // save y3
     psubsw mm3, mm1 // 1 // y7 = tp765*tg_1_16 - tp465

    movq [ecx + 5*16+8], mm5 // 5 // save y5

    movq [ecx + 7*16+8], mm3 // 3 // save y7

    // Rows -----------------------------------------------------------------

    mov eax, block
    mov edi, 0x08

  lp_mmx_fdct_row1:

    movd mm5, dword ptr [eax+12]// // mm5 = 7 6

    punpcklwd mm5, qword ptr [eax+8] // mm5 =  5 7 4 6

    movq mm2, mm5//     // mm2 = 5 7 4 6
    psrlq mm5, 32//     // mm5 = _ _ 5 7

    movq mm0, qword ptr [eax]// // mm0 = 3 2 1 0
    punpcklwd mm5, mm2//// mm5 = 4 5 6 7

    movq mm1, mm0//     // mm1 = 3 2 1 0
    paddsw mm0, mm5//   // mm0 = [3+4, 2+5, 1+6, 0+7] (xt3, xt2, xt1, xt0)

    psubsw mm1, mm5//   // mm1 = [3-4, 2-5, 1-6, 0-7] (xt7, xt6, xt5, xt4)
    movq mm2, mm0//     // mm2 = [ xt3 xt2 xt1 xt0 ]

    punpcklwd mm0, mm1//// mm0 = [ xt5 xt1 xt4 xt0 ]

    punpckhwd mm2, mm1//// mm2 = [ xt7 xt3 xt6 xt2 ]
    movq mm1, mm2//     // mm1

    //// shuffle bytes around

    movq mm2, mm0 // 2 // x3 x2 x1 x0

    movq mm3, qword ptr [ebx] // 3 // w06 w04 w02 w00
    punpcklwd mm0, mm1 // x5 x1 x4 x0

    movq mm5, mm0 // 5 // x5 x1 x4 x0
    punpckldq mm0, mm0 // x4 x0 x4 x0  [ xt2 xt0 xt2 xt0 ]

    movq mm4, qword ptr [ebx+8] // 4 // w07 w05 w03 w01
    punpckhwd mm2, mm1 // 1 // x7 x3 x6 x2

    pmaddwd mm3, mm0 // x4*w06+x0*w04 x4*w02+x0*w00
    movq mm6, mm2 // 6 // x7 x3 x6 x2

    movq mm1, qword ptr [ebx+32] // 1 // w22 w20 w18 w16
    punpckldq mm2, mm2 // x6 x2 x6 x2  [ xt3 xt1 xt3 xt1 ]

    pmaddwd mm4, mm2 // x6*w07+x2*w05 x6*w03+x2*w01
    punpckhdq mm5, mm5 // x5 x1 x5 x1  [ xt6 xt4 xt6 xt4 ]

    pmaddwd mm0, qword ptr [ebx+16] // x4*w14+x0*w12 x4*w10+x0*w08
    punpckhdq mm6, mm6 // x7 x3 x7 x3  [ xt7 xt5 xt7 xt5 ]

    movq mm7, qword ptr [ebx+40] // 7 // w23 w21 w19 w17
    pmaddwd mm1, mm5 // x5*w22+x1*w20 x5*w18+x1*w16

    paddd mm3, __jpmm_round_frw_row // +rounder (y2,y0)
    pmaddwd mm7, mm6 // x7*w23+x3*w21 x7*w19+x3*w17

    pmaddwd mm2, qword ptr [ebx+24] // x6*w15+x2*w13 x6*w11+x2*w09
    paddd mm3, mm4 // 4 // a1=sum(even1) a0=sum(even0) // now ( y2, y0)

    pmaddwd mm5, qword ptr [ebx+48] // x5*w30+x1*w28 x5*w26+x1*w24

    pmaddwd mm6, qword ptr [ebx+56] // x7*w31+x3*w29 x7*w27+x3*w25
    paddd mm1, mm7 // 7 // b1=sum(odd1) b0=sum(odd0) // now ( y3, y1)

    paddd mm0, __jpmm_round_frw_row // +rounder (y6,y4)
    psrad mm3, SHIFT_FRW_ROW // (y2, y0)

    paddd mm1, __jpmm_round_frw_row // +rounder (y3,y1)
    paddd mm0, mm2 // 2 // a3=sum(even3) a2=sum(even2) // now (y6, y4)

    paddd mm5, __jpmm_round_frw_row // +rounder (y7,y5)
    psrad mm1, SHIFT_FRW_ROW // y1=a1+b1 y0=a0+b0

    paddd mm5, mm6 // 6 // b3=sum(odd3) b2=sum(odd2) // now ( y7, y5)
    psrad mm0, SHIFT_FRW_ROW //y3=a3+b3 y2=a2+b2

    add ecx, 16//  // increment row-output address by 1 row
    psrad mm5, SHIFT_FRW_ROW // y4=a3-b3 y5=a2-b2

    add eax, 16//  // increment row-address by 1 row
    packssdw mm3, mm0 // 0 // y6 y4 y2 y0

    packssdw mm1, mm5 // 3 // y7 y5 y3 y1
    movq mm6, mm3//    // mm0 = y6 y4 y2 y0

    punpcklwd mm3, mm1// // y3 y2 y1 y0
    sub edi, 0x01//   // i = i - 1
    
    punpckhwd mm6, mm1// // y7 y6 y5 y4
    add ebx,64//  // increment to next table

    movq qword ptr [ecx-16], mm3 // 1 // save y3 y2 y1 y0

    movq qword ptr [ecx-8], mm6 // 7 // save y7 y6 y5 y4

    cmp edi, 0x00//
    jg lp_mmx_fdct_row1//  // begin fdct processing on next row

    emms//
    }

#else


#ifdef _64BITS

  // gcc inline assembly code (64bits)
  // Columns

  __asm__ (
  ".intel_syntax noprefix     \n"
  "mov rcx, rax               \n"
  "movq mm0, [rax + 1*16]     \n"
  "movq mm1, [rax + 6*16]     \n"
  "movq mm2, mm0              \n"
  "movq mm3, [rax + 2*16]     \n"
  "paddsw mm0, mm1            \n" 
  "movq mm4, [rax + 5*16]     \n"
  "psllw mm0, 3               \n"
  "movq mm5, [rax + 0*16]     \n"
  "paddsw mm4, mm3            \n"
  "paddsw mm5, [rax + 7*16]   \n"
  "psllw mm4, 3               \n"
  "movq mm6, mm0              \n"
  "psubsw mm2, mm1            \n"
  "movq mm1, __jpmm_tg_2_16   \n"
  "psubsw mm0, mm4            \n"
  "movq mm7, [rax + 3*16]     \n"
  "pmulhw mm1, mm0            \n"
  "paddsw mm7, [rax + 4*16]   \n"
  "psllw mm5, 3               \n"
  "paddsw mm6, mm4            \n"
  "psllw mm7, 3               \n"
  "movq mm4, mm5              \n"
  "psubsw mm5, mm7            \n"
  "paddsw mm1, mm5            \n"
  "paddsw mm4, mm7            \n"
  "por mm1, __jpmm_one_corr   \n"
  "psllw mm2, 4               \n"
  "pmulhw mm5, __jpmm_tg_2_16 \n"
  "movq mm7, mm4              \n"
  "psubsw mm3, [rax + 5*16]   \n"
  "psubsw mm4, mm6            \n"
  "movq [rcx + 2*16], mm1     \n"
  "paddsw mm7, mm6            \n"
  "movq mm1, [rax + 3*16]     \n"
  "psllw mm3, 4               \n"
  "psubsw mm1, [rax + 4*16]   \n"
  "movq mm6, mm2              \n"
  "movq [rcx + 4*16], mm4     \n"
  "paddsw mm2, mm3            \n"
  "pmulhw mm2, __jpmm_ocos_4_16 \n"
  "psubsw mm6, mm3            \n"
  "pmulhw mm6, __jpmm_ocos_4_16 \n"
  "psubsw mm5, mm0            \n"
  "por mm5,  __jpmm_one_corr  \n"
  "psllw mm1, 3               \n"
  "por mm2,  __jpmm_one_corr  \n"
  "movq mm4, mm1              \n"
  "movq mm3, [rax + 0*16]     \n"
  "paddsw mm1, mm6            \n"
  "psubsw mm3, [rax + 7*16]   \n"
  "psubsw mm4, mm6            \n"
  "movq mm0, __jpmm_tg_1_16   \n"
  "psllw mm3, 3               \n"
  "movq mm6, __jpmm_tg_3_16   \n"
  "pmulhw mm0, mm1            \n"
  "movq [rcx + 0*16], mm7     \n"
  "pmulhw mm6, mm4            \n"
  "movq [rcx + 6*16], mm5     \n"
  "movq mm7, mm3              \n"
  "movq mm5, __jpmm_tg_3_16   \n"
  "psubsw mm7, mm2            \n"
  "paddsw mm3, mm2            \n"
  "pmulhw mm5, mm7            \n"
  "paddsw mm0, mm3            \n"
  "paddsw mm6, mm4            \n"
  "pmulhw mm3, __jpmm_tg_1_16 \n"
  "por mm0, __jpmm_one_corr   \n"
  "paddsw mm5, mm7            \n"
  "psubsw mm7, mm6            \n"
  "add rax, 0x08              \n"
  "movq [rcx + 1*16], mm0     \n"
  "paddsw mm5, mm4            \n"
  "movq [rcx + 3*16], mm7     \n"
  "psubsw mm3, mm1            \n"
  "movq [rcx + 5*16], mm5     \n"
  "movq mm0, [rax + 1*16]     \n"
  "movq [rcx + 7*16], mm3     \n"
  "movq mm1, [rax + 6*16]     \n"
  "movq mm2, mm0              \n"
  "movq mm3, [rax + 2*16]     \n"
  "paddsw mm0, mm1            \n"
  "movq mm4, [rax + 5*16]     \n"
  "psllw mm0, 3               \n"
  "movq mm5, [rax + 0*16]     \n"
  "paddsw mm4, mm3            \n"
  "paddsw mm5, [rax + 7*16]   \n"
  "psllw mm4, 3               \n"
  "movq mm6, mm0              \n"
  "psubsw mm2, mm1            \n"
  "movq mm1, __jpmm_tg_2_16   \n"
  "psubsw mm0, mm4            \n"
  "movq mm7, [rax + 3*16]     \n"
  "pmulhw mm1, mm0            \n"
  "paddsw mm7, [rax + 4*16]   \n"
  "psllw mm5, 3               \n"
  "paddsw mm6, mm4            \n"
  "psllw mm7, 3               \n"
  "movq mm4, mm5              \n"
  "psubsw mm5, mm7            \n"
  "paddsw mm1, mm5            \n"
  "paddsw mm4, mm7            \n"
  "por mm1, __jpmm_one_corr   \n"
  "psllw mm2, 4               \n"
  "pmulhw mm5, __jpmm_tg_2_16 \n"
  "movq mm7, mm4              \n"
  "psubsw mm3, [rax + 5*16]   \n"
  "psubsw mm4, mm6            \n"
  "movq [rcx + 2*16+8], mm1   \n"
  "paddsw mm7, mm6            \n"
  "movq mm1, [rax + 3*16]     \n"
  "psllw mm3, 3+1             \n"
  "psubsw mm1, [rax + 4*16]   \n"
  "movq mm6, mm2              \n"
  "movq [rcx + 4*16+8], mm4   \n"
  "paddsw mm2, mm3            \n"
  "pmulhw mm2, __jpmm_ocos_4_16 \n"
  "psubsw mm6, mm3            \n"
  "pmulhw mm6, __jpmm_ocos_4_16 \n"
  "psubsw mm5, mm0            \n"
  "por mm5, __jpmm_one_corr   \n"
  "psllw mm1, 3               \n"
  "por mm2, __jpmm_one_corr   \n"
  "movq mm4, mm1              \n"
  "movq mm3, [rax + 0*16]     \n"
  "paddsw mm1, mm6            \n"
  "psubsw mm3, [rax + 7*16]   \n"
  "psubsw mm4, mm6            \n"
  "movq mm0, __jpmm_tg_1_16   \n"
  "psllw mm3, 3               \n"
  "movq mm6, __jpmm_tg_3_16   \n"
  "pmulhw mm0, mm1            \n"
  "movq [rcx +8], mm7         \n"
  "pmulhw mm6, mm4            \n"
  "movq [rcx + 6*16+8], mm5   \n"
  "movq mm7, mm3              \n"
  "movq mm5, __jpmm_tg_3_16   \n"
  "psubsw mm7, mm2            \n"
  "paddsw mm3, mm2            \n"
  "pmulhw mm5, mm7            \n"
  "paddsw mm0, mm3            \n"
  "paddsw mm6, mm4            \n"
  "pmulhw mm3, __jpmm_tg_1_16 \n"
  "por mm0, __jpmm_one_corr   \n"
  "paddsw mm5, mm7            \n"
  "psubsw mm7, mm6            \n"
  "movq [rcx + 1*16+8], mm0   \n"
  "paddsw mm5, mm4            \n"
  "movq [rcx + 3*16+8], mm7   \n"
  "psubsw mm3, mm1            \n"
  "movq [rcx + 5*16+8], mm5   \n"
  "movq [rcx + 7*16+8], mm3   \n"
  ".att_syntax                \n"
  : /* no output */
  : "a"(block)
  : "memory","rcx","mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7"
  );

  // Rows
  __asm__ (
  ".intel_syntax noprefix     \n"
  "push rbx                   \n"  
  "lea rbx,__jpmm_row_tab_frw \n"
  "mov rcx, rax               \n"
  "mov rdi, 0x08              \n"
"lp_mmx_fdct_row1:            \n"
  "movd mm5, [rax+12]         \n"
  "punpcklwd mm5, [rax+8]     \n"
  "movq mm2, mm5              \n"
  "psrlq mm5, 32              \n"
  "movq mm0, [rax]            \n"
  "punpcklwd mm5, mm2         \n"
  "movq mm1, mm0              \n"
  "paddsw mm0, mm5            \n"
  "psubsw mm1, mm5            \n"
  "movq mm2, mm0              \n"
  "punpcklwd mm0, mm1         \n"
  "punpckhwd mm2, mm1         \n"
  "movq mm1, mm2              \n"
  "movq mm2, mm0              \n"
  "movq mm3, [rbx]            \n"
  "punpcklwd mm0, mm1         \n"
  "movq mm5, mm0              \n"
  "punpckldq mm0, mm0         \n"
  "movq mm4, [rbx+8]          \n"
  "punpckhwd mm2, mm1         \n"
  "pmaddwd mm3, mm0           \n"
  "movq mm6, mm2              \n"
  "movq mm1, [rbx+32]         \n"
  "punpckldq mm2, mm2         \n"
  "pmaddwd mm4, mm2           \n"
  "punpckhdq mm5, mm5         \n"
  "pmaddwd mm0, [rbx+16]      \n"
  "punpckhdq mm6, mm6         \n"
  "movq mm7, [rbx+40]         \n"
  "pmaddwd mm1, mm5           \n"
  "paddd mm3, __jpmm_round_frw_row  \n"
  "pmaddwd mm7, mm6           \n"
  "pmaddwd mm2, [rbx+24]      \n"
  "paddd mm3, mm4             \n"
  "pmaddwd mm5, [rbx+48]      \n"
  "pmaddwd mm6, [rbx+56]      \n"
  "paddd mm1, mm7             \n"
  "paddd mm0, __jpmm_round_frw_row \n"
  "psrad mm3, 17              \n"
  "paddd mm1, __jpmm_round_frw_row \n"
  "paddd mm0, mm2             \n"
  "paddd mm5, __jpmm_round_frw_row \n"
  "psrad mm1, 17              \n"
  "paddd mm5, mm6             \n"
  "psrad mm0, 17              \n"
  "add rcx, 16                \n"
  "psrad mm5, 17              \n"
  "add rax, 16                \n"
  "packssdw mm3, mm0          \n"
  "packssdw mm1, mm5          \n"
  "movq mm6, mm3              \n"
  "punpcklwd mm3, mm1         \n"
  "sub rdi, 0x01              \n"
  "punpckhwd mm6, mm1         \n"
  "add rbx,64                 \n"
  "movq [rcx-16], mm3         \n"
  "movq [rcx-8], mm6          \n"
  "cmp rdi, 0x00              \n"
  "jg lp_mmx_fdct_row1        \n"
  "pop rbx                    \n"  
  "emms                       \n"
  ".att_syntax                \n"
  : /* no output */
  : "a"(block)
  : "memory","rcx","rdi","mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7"
  );

#else

  // gcc inline assembly code (32bits)
  // Columns

  __asm__ (
  ".intel_syntax noprefix     \n"
  "mov ecx, eax               \n"
  "movq mm0, [eax + 1*16]     \n"
  "movq mm1, [eax + 6*16]     \n"
  "movq mm2, mm0              \n"
  "movq mm3, [eax + 2*16]     \n"
  "paddsw mm0, mm1            \n" 
  "movq mm4, [eax + 5*16]     \n"
  "psllw mm0, 3               \n"
  "movq mm5, [eax + 0*16]     \n"
  "paddsw mm4, mm3            \n"
  "paddsw mm5, [eax + 7*16]   \n"
  "psllw mm4, 3               \n"
  "movq mm6, mm0              \n"
  "psubsw mm2, mm1            \n"
  "movq mm1, __jpmm_tg_2_16   \n"
  "psubsw mm0, mm4            \n"
  "movq mm7, [eax + 3*16]     \n"
  "pmulhw mm1, mm0            \n"
  "paddsw mm7, [eax + 4*16]   \n"
  "psllw mm5, 3               \n"
  "paddsw mm6, mm4            \n"
  "psllw mm7, 3               \n"
  "movq mm4, mm5              \n"
  "psubsw mm5, mm7            \n"
  "paddsw mm1, mm5            \n"
  "paddsw mm4, mm7            \n"
  "por mm1, __jpmm_one_corr   \n"
  "psllw mm2, 4               \n"
  "pmulhw mm5, __jpmm_tg_2_16 \n"
  "movq mm7, mm4              \n"
  "psubsw mm3, [eax + 5*16]   \n"
  "psubsw mm4, mm6            \n"
  "movq [ecx + 2*16], mm1     \n"
  "paddsw mm7, mm6            \n"
  "movq mm1, [eax + 3*16]     \n"
  "psllw mm3, 3+1             \n"
  "psubsw mm1, [eax + 4*16]   \n"
  "movq mm6, mm2              \n"
  "movq [ecx + 4*16], mm4     \n"
  "paddsw mm2, mm3            \n"
  "pmulhw mm2, __jpmm_ocos_4_16 \n"
  "psubsw mm6, mm3            \n"
  "pmulhw mm6, __jpmm_ocos_4_16 \n"
  "psubsw mm5, mm0            \n"
  "por mm5,  __jpmm_one_corr  \n"
  "psllw mm1, 3               \n"
  "por mm2,  __jpmm_one_corr  \n"
  "movq mm4, mm1              \n"
  "movq mm3, [eax + 0*16]     \n"
  "paddsw mm1, mm6            \n"
  "psubsw mm3, [eax + 7*16]   \n"
  "psubsw mm4, mm6            \n"
  "movq mm0, __jpmm_tg_1_16   \n"
  "psllw mm3, 3               \n"
  "movq mm6, __jpmm_tg_3_16   \n"
  "pmulhw mm0, mm1            \n"
  "movq [ecx + 0*16], mm7     \n"
  "pmulhw mm6, mm4            \n"
  "movq [ecx + 6*16], mm5     \n"
  "movq mm7, mm3              \n"
  "movq mm5, __jpmm_tg_3_16   \n"
  "psubsw mm7, mm2            \n"
  "paddsw mm3, mm2            \n"
  "pmulhw mm5, mm7            \n"
  "paddsw mm0, mm3            \n"
  "paddsw mm6, mm4            \n"
  "pmulhw mm3, __jpmm_tg_1_16 \n"
  "por mm0, __jpmm_one_corr   \n"
  "paddsw mm5, mm7            \n"
  "psubsw mm7, mm6            \n"
  "add eax, 0x08              \n"
  "movq [ecx + 1*16], mm0     \n"
  "paddsw mm5, mm4            \n"
  "movq [ecx + 3*16], mm7     \n"
  "psubsw mm3, mm1            \n"
  "movq [ecx + 5*16], mm5     \n"
  "movq mm0, [eax + 1*16]     \n"
  "movq [ecx + 7*16], mm3     \n"
  "movq mm1, [eax + 6*16]     \n"
  "movq mm2, mm0              \n"
  "movq mm3, [eax + 2*16]     \n"
  "paddsw mm0, mm1            \n"
  "movq mm4, [eax + 5*16]     \n"
  "psllw mm0, 3               \n"
  "movq mm5, [eax + 0*16]     \n"
  "paddsw mm4, mm3            \n"
  "paddsw mm5, [eax + 7*16]   \n"
  "psllw mm4, 3               \n"
  "movq mm6, mm0              \n"
  "psubsw mm2, mm1            \n"
  "movq mm1, __jpmm_tg_2_16   \n"
  "psubsw mm0, mm4            \n"
  "movq mm7, [eax + 3*16]     \n"
  "pmulhw mm1, mm0            \n"
  "paddsw mm7, [eax + 4*16]   \n"
  "psllw mm5, 3               \n"
  "paddsw mm6, mm4            \n"
  "psllw mm7, 3               \n"
  "movq mm4, mm5              \n"
  "psubsw mm5, mm7            \n"
  "paddsw mm1, mm5            \n"
  "paddsw mm4, mm7            \n"
  "por mm1, __jpmm_one_corr   \n"
  "psllw mm2, 4               \n"
  "pmulhw mm5, __jpmm_tg_2_16 \n"
  "movq mm7, mm4              \n"
  "psubsw mm3, [eax + 5*16]   \n"
  "psubsw mm4, mm6            \n"
  "movq [ecx + 2*16+8], mm1   \n"
  "paddsw mm7, mm6            \n"
  "movq mm1, [eax + 3*16]     \n"
  "psllw mm3, 3+1             \n"
  "psubsw mm1, [eax + 4*16]   \n"
  "movq mm6, mm2              \n"
  "movq [ecx + 4*16+8], mm4   \n"
  "paddsw mm2, mm3            \n"
  "pmulhw mm2, __jpmm_ocos_4_16 \n"
  "psubsw mm6, mm3            \n"
  "pmulhw mm6, __jpmm_ocos_4_16 \n"
  "psubsw mm5, mm0            \n"
  "por mm5, __jpmm_one_corr   \n"
  "psllw mm1, 3               \n"
  "por mm2, __jpmm_one_corr   \n"
  "movq mm4, mm1              \n"
  "movq mm3, [eax + 0*16]     \n"
  "paddsw mm1, mm6            \n"
  "psubsw mm3, [eax + 7*16]   \n"
  "psubsw mm4, mm6            \n"
  "movq mm0, __jpmm_tg_1_16   \n"
  "psllw mm3, 3               \n"
  "movq mm6, __jpmm_tg_3_16   \n"
  "pmulhw mm0, mm1            \n"
  "movq [ecx +8], mm7         \n"
  "pmulhw mm6, mm4            \n"
  "movq [ecx + 6*16+8], mm5   \n"
  "movq mm7, mm3              \n"
  "movq mm5, __jpmm_tg_3_16   \n"
  "psubsw mm7, mm2            \n"
  "paddsw mm3, mm2            \n"
  "pmulhw mm5, mm7            \n"
  "paddsw mm0, mm3            \n"
  "paddsw mm6, mm4            \n"
  "pmulhw mm3, __jpmm_tg_1_16 \n"
  "por mm0, __jpmm_one_corr   \n"
  "paddsw mm5, mm7            \n"
  "psubsw mm7, mm6            \n"
  "movq [ecx + 1*16+8], mm0   \n"
  "paddsw mm5, mm4            \n"
  "movq [ecx + 3*16+8], mm7   \n"
  "psubsw mm3, mm1            \n"
  "movq [ecx + 5*16+8], mm5   \n"
  "movq [ecx + 7*16+8], mm3   \n"
  ".att_syntax                \n"
  : /* no output */
  : "a"(block)
  : "memory","ecx","mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7"
  );

  // Rows
  __asm__ (
  ".intel_syntax noprefix     \n"
  "push ebx                   \n"  
  "lea ebx,__jpmm_row_tab_frw \n"
  "mov edi, 0x08              \n"
  "mov ecx, eax               \n"
"lp_mmx_fdct_row1:            \n"
  "movd mm5, [eax+12]         \n"
  "punpcklwd mm5, [eax+8]     \n"
  "movq mm2, mm5              \n"
  "psrlq mm5, 32              \n"
  "movq mm0, [eax]            \n"
  "punpcklwd mm5, mm2         \n"
  "movq mm1, mm0              \n"
  "paddsw mm0, mm5            \n"
  "psubsw mm1, mm5            \n"
  "movq mm2, mm0              \n"
  "punpcklwd mm0, mm1         \n"
  "punpckhwd mm2, mm1         \n"
  "movq mm1, mm2              \n"
  "movq mm2, mm0              \n"
  "movq mm3, [ebx]            \n"
  "punpcklwd mm0, mm1         \n"
  "movq mm5, mm0              \n"
  "punpckldq mm0, mm0         \n"
  "movq mm4, [ebx+8]          \n"
  "punpckhwd mm2, mm1         \n"
  "pmaddwd mm3, mm0           \n"
  "movq mm6, mm2              \n"
  "movq mm1, [ebx+32]         \n"
  "punpckldq mm2, mm2         \n"
  "pmaddwd mm4, mm2           \n"
  "punpckhdq mm5, mm5         \n"
  "pmaddwd mm0, [ebx+16]      \n"
  "punpckhdq mm6, mm6         \n"
  "movq mm7, [ebx+40]         \n"
  "pmaddwd mm1, mm5           \n"
  "paddd mm3, __jpmm_round_frw_row  \n"
  "pmaddwd mm7, mm6           \n"
  "pmaddwd mm2, [ebx+24]      \n"
  "paddd mm3, mm4             \n"
  "pmaddwd mm5, [ebx+48]      \n"
  "pmaddwd mm6, [ebx+56]      \n"
  "paddd mm1, mm7             \n"
  "paddd mm0, __jpmm_round_frw_row \n"
  "psrad mm3, 17              \n"
  "paddd mm1, __jpmm_round_frw_row \n"
  "paddd mm0, mm2             \n"
  "paddd mm5, __jpmm_round_frw_row \n"
  "psrad mm1, 17              \n"
  "paddd mm5, mm6             \n"
  "psrad mm0, 17              \n"
  "add ecx, 16                \n"
  "psrad mm5, 17              \n"
  "add eax, 16                \n"
  "packssdw mm3, mm0          \n"
  "packssdw mm1, mm5          \n"
  "movq mm6, mm3              \n"
  "punpcklwd mm3, mm1         \n"
  "sub edi, 0x01              \n"
  "punpckhwd mm6, mm1         \n"
  "add ebx,64                 \n"
  "movq [ecx-16], mm3         \n"
  "movq [ecx-8], mm6          \n"
  "cmp edi, 0x00              \n"
  "jg lp_mmx_fdct_row1        \n"
  "pop ebx                    \n"  
  "emms                       \n"
  ".att_syntax                \n"
  : /* no output */
  : "a"(block)
  : "memory","ecx","edi","mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7"
  );

#endif /* _64BITS */

#endif /* _WINDOWS */

}

#define SHIFT_INV_ROW 11
#define SHIFT_INV_COL 6

ALIGN8 short __jpmm_row_tabs[] = {

    // Table for rows 0 - constants are multiplied by cos_4_16
    16384,  16384,  16384, -16384,
    21407,   8867,   8867, -21407,
    16384, -16384,  16384,  16384,
    -8867,  21407, -21407,  -8867,
    22725,  12873,  19266, -22725,
    19266,   4520,  -4520, -12873,
    12873,   4520,   4520,  19266,
    -22725,  19266, -12873, -22725,

    // Table for rows 1 - constants are multiplied by cos_1_16

    22725,  22725,  22725, -22725,
    29692,  12299,  12299, -29692,
    22725, -22725,  22725,  22725,
    -12299,  29692, -29692, -12299,
    31521,  17855,  26722, -31521,
    26722,   6270,  -6270, -17855,
    17855,   6270,   6270,  26722,
    -31521,  26722, -17855, -31521,
 
    // Table for rows 2 - constants are multiplied by cos_2_16

    21407,  21407,  21407, -21407,
    27969,  11585,  11585, -27969,
    21407, -21407,  21407,  21407,
    -11585,  27969, -27969, -11585,
    29692,  16819,  25172, -29692,
    25172,   5906,  -5906, -16819,
    16819,   5906,   5906,  25172,
    -29692,  25172, -16819, -29692,

    // Table for rows 3 - constants are multiplied by cos_3_16

    19266,  19266,  19266, -19266,
    25172,  10426,  10426, -25172,
    19266, -19266,  19266,  19266,
    -10426,  25172, -25172, -10426,
    26722,  15137,  22654, -26722,
    22654,   5315,  -5315, -15137,
    15137,   5315,   5315,  22654,
    -26722,  22654, -15137, -26722,

    // Table for rows 4 - constants are multiplied by cos_4_16
    16384,  16384,  16384, -16384,
    21407,   8867,   8867, -21407,
    16384, -16384,  16384,  16384,
    -8867,  21407, -21407,  -8867,
    22725,  12873,  19266, -22725,
    19266,   4520,  -4520, -12873,
    12873,   4520,   4520,  19266,
    -22725,  19266, -12873, -22725,

    // Table for rows 5 - constants are multiplied by cos_3_16

    19266,  19266,  19266, -19266,
    25172,  10426,  10426, -25172,
    19266, -19266,  19266,  19266,
    -10426,  25172, -25172, -10426,
    26722,  15137,  22654, -26722,
    22654,   5315,  -5315, -15137,
    15137,   5315,   5315,  22654,
    -26722,  22654, -15137, -26722,
 
    // Table for rows 6 - constants are multiplied by cos_2_16

    21407,  21407,  21407, -21407,
    27969,  11585,  11585, -27969,
    21407, -21407,  21407,  21407,
    -11585,  27969, -27969, -11585,
    29692,  16819,  25172, -29692,
    25172,   5906,  -5906, -16819,
    16819,   5906,   5906,  25172,
    -29692,  25172, -16819, -29692,

    // Table for rows 7 - constants are multiplied by cos_1_16

    22725,  22725,  22725, -22725,
    29692,  12299,  12299, -29692,
    22725, -22725,  22725,  22725,
    -12299,  29692, -29692, -12299,
    31521,  17855,  26722, -31521,
    26722,   6270,  -6270, -17855,
    17855,   6270,   6270,  26722,
    -31521,  26722, -17855, -31521
};

// Rounding
ALIGN8 long __jpmm_rounder[]  = {
   65536, 65536 ,
   3597,  3597 ,
   2260,  2260 ,
   1203,  1203 ,
   0,     0 ,
   120,   120 ,
   512,   512 ,
   512,   512 
};

// Offset
ALIGN8 short __jpmm_offset128[]  = { 128,128,128,128 };

void jpeg_idct_mmx(short *block, unsigned char *dest)
{

  short innerBuff[64];
  long  scratch[4];

#ifdef _WINDOWS

  // Visual C++ inline assembly code

  __asm {

  // Rows -------------------
  mov esi, block
  lea edi, innerBuff
  lea eax, __jpmm_row_tabs
  lea ebx, __jpmm_rounder
  mov ecx, 8

__mmx_idct_rows:

  movq mm0, [esi]     ; 0  ; x3 x2 x1 x0

  movq mm1, [esi+8]    ; 1  ; x7 x6 x5 x4
  movq mm2, mm0       ; 2  ; x3 x2 x1 x0

  movq mm3,  [eax]    ; 3  ; w06 w04 w02 w00
  punpcklwd mm0, mm1       ; x5 x1 x4 x0

  movq mm5, mm0       ; 5  ; x5 x1 x4 x0
  punpckldq mm0, mm0       ; x4 x0 x4 x0

  movq mm4,  [eax+8]   ; 4  ; w07 w05 w03 w01
  punpckhwd mm2, mm1    ; 1  ; x7 x3 x6 x2

  pmaddwd mm3, mm0       ; x4*w06+x0*w04 x4*w02+x0*w00
  movq mm6, mm2       ; 6   ; x7 x3 x6 x2

  movq mm1,  [eax+32]   ; 1   ; w22 w20 w18 w16
  punpckldq mm2, mm2       ; x6 x2 x6 x2

  pmaddwd mm4, mm2       ; x6*w07+x2*w05 x6*w03+x2*w01
  punpckhdq mm5, mm5       ; x5 x1 x5 x1

  pmaddwd mm0,  [eax+16]     ; x4*w14+x0*w12 x4*w10+x0*w08
  punpckhdq mm6, mm6       ; x7 x3 x7 x3

  movq mm7,  [eax+40]   ; 7   ; w23 w21 w19 w17
  pmaddwd mm1, mm5       ; x5*w22+x1*w20 x5*w18+x1*w16

  paddd mm3,  [ebx]     ; +rounder
  pmaddwd mm7, mm6       ; x7*w23+x3*w21 x7*w19+x3*w17

  pmaddwd mm2,  [eax+24]     ; x6*w15+x2*w13 x6*w11+x2*w09
  paddd mm3, mm4       ; 4   ; a1=sum(even1) a0=sum(even0)

  pmaddwd mm5,  [eax+48]     ; x5*w30+x1*w28 x5*w26+x1*w24
  movq mm4, mm3       ; 4   ; a1 a0

  pmaddwd mm6,  [eax+56]     ; x7*w31+x3*w29 x7*w27+x3*w25
  paddd mm1, mm7       ; 7   ; b1=sum(odd1) b0=sum(odd0)

  paddd mm0,  [ebx]    ; +rounder
  psubd mm3, mm1         ; a1-b1 a0-b0

  psrad mm3, SHIFT_INV_ROW     ; y6=a1-b1 y7=a0-b0
  paddd mm1, mm4       ; 4   ; a1+b1 a0+b0

  paddd mm0, mm2       ; 2   ; a3=sum(even3) a2=sum(even2)
  psrad mm1, SHIFT_INV_ROW     ; y1=a1+b1 y0=a0+b0

  paddd mm5, mm6       ; 6   ; b3=sum(odd3) b2=sum(odd2)
  movq mm4, mm0       ; 4   ; a3 a2

  paddd mm0, mm5         ; a3+b3 a2+b2
  psubd mm4, mm5       ; 5   ; a3-b3 a2-b2

  psrad mm0, SHIFT_INV_ROW     ; y3=a3+b3 y2=a2+b2
  psrad mm4, SHIFT_INV_ROW     ; y4=a3-b3 y5=a2-b2

  packssdw mm1, mm0     ; 0   ; y3 y2 y1 y0
  packssdw mm4, mm3     ; 3   ; y6 y7 y4 y5

  movq mm7, mm4       ; 7   ; y6 y7 y4 y5
  psrld mm4, 16         ; 0 y6 0 y4

  pslld mm7, 16         ; y7 0 y5 0
  movq  [edi], mm1     ; 1   ; save y3 y2 y1 y0
                               
  por mm7, mm4       ; 4   ; y7 y6 y5 y4
  movq  [edi+8], mm7     ; 7   ; save y7 y6 y5 y4

  add esi, 16
  add edi, 16
  add eax, 64
  add ebx, 8
  dec ecx
  jnz __mmx_idct_rows

  // Columns -------------------
  lea esi, innerBuff
  mov edi, dest
  lea eax, scratch
  mov ecx, 2

__mmx_idct_cols:

  movq  mm0, __jpmm_tg_3_16

  movq  mm3, [esi+16*3]
  movq  mm1, mm0      ; tg_3_16

  movq  mm5, [esi+16*5]
  pmulhw  mm0, mm3      ; x3*(tg_3_16-1)

  movq  mm4, __jpmm_tg_1_16
  pmulhw  mm1, mm5      ; x5*(tg_3_16-1)

  movq  mm7, [esi+16*7]
  movq  mm2, mm4      ; tg_1_16

  movq  mm6, [esi+16*1]
  pmulhw  mm4, mm7      ; x7*tg_1_16

  paddsw  mm0, mm3      ; x3*tg_3_16
  pmulhw  mm2, mm6      ; x1*tg_1_16

  paddsw  mm1, mm3      ; x3+x5*(tg_3_16-1)
  psubsw  mm0, mm5      ; x3*tg_3_16-x5 = tm35

  movq  mm3, __jpmm_ocos_4_16
  paddsw  mm1, mm5      ; x3+x5*tg_3_16 = tp35

  paddsw  mm4, mm6      ; x1+tg_1_16*x7 = tp17
  psubsw  mm2, mm7      ; x1*tg_1_16-x7 = tm17

  movq  mm5, mm4      ; tp17
  movq  mm6, mm2      ; tm17

  paddsw  mm5, mm1      ; tp17+tp35 = b0
  psubsw  mm6, mm0      ; tm17-tm35 = b3

  psubsw  mm4, mm1      ; tp17-tp35 = t1
  paddsw  mm2, mm0      ; tm17+tm35 = t2

  movq  mm7, __jpmm_tg_2_16
  movq  mm1, mm4      ; t1

  movq  [eax+0], mm5  ; save b0
  paddsw  mm1, mm2      ; t1+t2

  movq  [eax+8], mm6  ; save b3
  psubsw  mm4, mm2      ; t1-t2

  movq  mm5, [esi+2*16]
  movq  mm0, mm7      ; tg_2_16

  movq  mm6, [esi+6*16]
  pmulhw  mm0, mm5      ; x2*tg_2_16

  pmulhw  mm7, mm6      ; x6*tg_2_16
  pmulhw  mm1, mm3      ; ocos_4_16*(t1+t2) = b1/2
  movq  mm2, [esi+0*16]
  pmulhw  mm4, mm3      ; ocos_4_16*(t1-t2) = b2/2

  psubsw  mm0, mm6      ; t2*tg_2_16-x6 = tm26
  movq  mm3, mm2      ; x0

  movq  mm6, [esi+4*16]
  paddsw  mm7, mm5      ; x2+x6*tg_2_16 = tp26

  paddsw  mm2, mm6      ; x0+x4 = tp04
  psubsw  mm3, mm6      ; x0-x4 = tm04

  movq  mm5, mm2      ; tp04
  movq  mm6, mm3      ; tm04

  psubsw  mm2, mm7      ; tp04-tp26 = a3
  paddsw  mm3, mm0      ; tm04+tm26 = a1

  paddsw mm1, mm1        ; b1
  paddsw mm4, mm4        ; b2

  paddsw  mm5, mm7      ; tp04+tp26 = a0
  psubsw  mm6, mm0      ; tm04-tm26 = a2

  movq  mm7, mm3      ; a1
  movq  mm0, mm6      ; a2

  paddsw  mm3, mm1      ; a1+b1
  paddsw  mm6, mm4      ; a2+b2

  psraw  mm3, SHIFT_INV_COL    ; dst1
  psubsw  mm7, mm1      ; a1-b1

  psraw  mm6, SHIFT_INV_COL    ; dst2
  psubsw  mm0, mm4      ; a2-b2

   movq  mm1, [eax+0]  ; load b0
  psraw  mm7, SHIFT_INV_COL    ; dst6

  movq  mm4, mm5      ; a0
  psraw  mm0, SHIFT_INV_COL    ; dst5

  paddw mm3,__jpmm_offset128
  packuswb mm3,mm0
  movd  [edi+1*8], mm3 // R1
  paddsw  mm5, mm1      ; a0+b0

  paddw mm6,__jpmm_offset128
  packuswb mm6,mm0
  movd  [edi+2*8], mm6 // R2
  psubsw  mm4, mm1      ; a0-b0

  movq  mm3, [eax+8]  ; load b3
  psraw  mm5, SHIFT_INV_COL    ; dst0

  movq  mm6, mm2      ; a3
  psraw  mm4, SHIFT_INV_COL    ; dst7

  paddw mm0,__jpmm_offset128
  packuswb mm0,mm1
  movd  [edi+5*8], mm0 // R5
  paddsw  mm2, mm3      ; a3+b3

  paddw mm7,__jpmm_offset128
  packuswb mm7,mm0
  movd  [edi+6*8], mm7 // R6
  psubsw  mm6, mm3      ; a3-b3

  paddw mm5,__jpmm_offset128
  packuswb mm5,mm0
  movd  [edi+0*8], mm5 // R0
  psraw  mm2, SHIFT_INV_COL    ; dst3

  paddw mm4,__jpmm_offset128
  packuswb mm4,mm0
  movd  [edi+7*8], mm4 // R7
  psraw  mm6, SHIFT_INV_COL    ; dst4

  paddw mm2,__jpmm_offset128
  packuswb mm2,mm0
  movd  [edi+3*8], mm2  // R3
  paddw mm6,__jpmm_offset128
  packuswb mm6,mm0
  movd  [edi+4*8], mm6  // R4

  add edi,4
  add esi,8
  dec ecx
  jnz __mmx_idct_cols

  emms
  }

#else

#ifdef _64BITS

  // gcc inline assembly code (64bits)
  // Rows
  __asm__ (
  ".intel_syntax noprefix     \n"
  "push rbx                   \n"
  "mov rcx, 8                 \n"
  "lea rax, __jpmm_row_tabs   \n"
  "lea rbx, __jpmm_rounder    \n"
"__mmx_idct_rows:             \n"
  "movq mm0, [rsi]            \n"
  "movq mm1, [rsi+8]          \n"
  "movq mm2, mm0              \n"
  "movq mm3,  [rax]           \n"
  "punpcklwd mm0, mm1         \n"
  "movq mm5, mm0              \n"
  "punpckldq mm0, mm0         \n"
  "movq mm4,  [rax+8]         \n"
  "punpckhwd mm2, mm1         \n"
  "pmaddwd mm3, mm0           \n"
  "movq mm6, mm2              \n"
  "movq mm1,  [rax+32]        \n"
  "punpckldq mm2, mm2         \n"
  "pmaddwd mm4, mm2           \n"
  "punpckhdq mm5, mm5         \n"
  "pmaddwd mm0,  [rax+16]     \n"
  "punpckhdq mm6, mm6         \n"
  "movq mm7,  [rax+40]        \n"
  "pmaddwd mm1, mm5           \n"
  "paddd mm3,  [rbx]          \n"
  "pmaddwd mm7, mm6           \n"
  "pmaddwd mm2,  [rax+24]     \n"
  "paddd mm3, mm4             \n"
  "pmaddwd mm5,  [rax+48]     \n"
  "movq mm4, mm3              \n"
  "pmaddwd mm6,  [rax+56]     \n"
  "paddd mm1, mm7             \n"
  "paddd mm0,  [rbx]          \n"
  "psubd mm3, mm1             \n"
  "psrad mm3, 11              \n"
  "paddd mm1, mm4             \n"
  "paddd mm0, mm2             \n"
  "psrad mm1, 11              \n"
  "paddd mm5, mm6             \n"
  "movq mm4, mm0              \n"
  "paddd mm0, mm5             \n"
  "psubd mm4, mm5             \n"
  "psrad mm0, 11              \n"
  "psrad mm4, 11              \n"
  "packssdw mm1, mm0          \n"
  "packssdw mm4, mm3          \n"
  "movq mm7, mm4              \n"
  "psrld mm4, 16              \n"
  "pslld mm7, 16              \n"
  "movq  [rdi], mm1           \n"
  "por mm7, mm4               \n"
  "movq  [rdi+8], mm7         \n"
  "add rsi, 16                \n"
  "add rdi, 16                \n"
  "add rax, 64                \n"
  "add rbx, 8                 \n"
  "dec rcx                    \n"
  "jnz __mmx_idct_rows        \n"
  "pop rbx                    \n"
  ".att_syntax                \n"
  : /* no output */
  : "D"(innerBuff),"S"(block)
  : "memory","rax","rcx","mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7"
  );

  // Columns
  __asm__ (
  ".intel_syntax noprefix      \n"
  "mov rcx, 2                  \n"
"__mmx_idct_cols:              \n"
  "movq  mm0, __jpmm_tg_3_16   \n"
  "movq  mm3, [rsi+16*3]       \n"
  "movq  mm1, mm0              \n"
  "movq  mm5, [rsi+16*5]       \n"
  "pmulhw  mm0, mm3            \n"
  "movq  mm4, __jpmm_tg_1_16   \n"
  "pmulhw  mm1, mm5            \n"
  "movq  mm7, [rsi+16*7]       \n"
  "movq  mm2, mm4              \n"
  "movq  mm6, [rsi+16*1]       \n"
  "pmulhw  mm4, mm7            \n"
  "paddsw  mm0, mm3            \n"
  "pmulhw  mm2, mm6            \n"
  "paddsw  mm1, mm3            \n"
  "psubsw  mm0, mm5            \n"
  "movq  mm3, __jpmm_ocos_4_16 \n"
  "paddsw  mm1, mm5            \n"
  "paddsw  mm4, mm6            \n"
  "psubsw  mm2, mm7            \n"
  "movq  mm5, mm4              \n"
  "movq  mm6, mm2              \n"
  "paddsw  mm5, mm1            \n"
  "psubsw  mm6, mm0            \n"
  "psubsw  mm4, mm1            \n"
  "paddsw  mm2, mm0            \n"
  "movq  mm7, __jpmm_tg_2_16   \n"
  "movq  mm1, mm4              \n"
  "movq  [rax+0], mm5          \n"
  "paddsw  mm1, mm2            \n"
  "movq  [rax+8], mm6          \n"
  "psubsw  mm4, mm2            \n"
  "movq  mm5, [rsi+2*16]       \n"
  "movq  mm0, mm7              \n"
  "movq  mm6, [rsi+6*16]       \n"
  "pmulhw  mm0, mm5            \n"
  "pmulhw  mm7, mm6            \n"
  "pmulhw  mm1, mm3            \n"
  "movq  mm2, [rsi+0*16]       \n"
  "pmulhw  mm4, mm3            \n"
  "psubsw  mm0, mm6            \n"
  "movq  mm3, mm2              \n"
  "movq  mm6, [rsi+4*16]       \n"
  "paddsw  mm7, mm5            \n"
  "paddsw  mm2, mm6            \n"
  "psubsw  mm3, mm6            \n"
  "movq  mm5, mm2              \n"
  "movq  mm6, mm3              \n"
  "psubsw  mm2, mm7            \n"
  "paddsw  mm3, mm0            \n"
  "paddsw mm1, mm1             \n"
  "paddsw mm4, mm4             \n"
  "paddsw  mm5, mm7            \n"
  "psubsw  mm6, mm0            \n"
  "movq  mm7, mm3              \n"
  "movq  mm0, mm6              \n"
  "paddsw  mm3, mm1            \n"
  "paddsw  mm6, mm4            \n"
  "psraw  mm3, 6               \n"
  "psubsw  mm7, mm1            \n"
  "psraw  mm6, 6               \n"
  "psubsw  mm0, mm4            \n"
   "movq  mm1, [rax+0]         \n"
  "psraw  mm7, 6               \n"
  "movq  mm4, mm5              \n"
  "psraw  mm0, 6               \n"
  "paddw mm3,__jpmm_offset128  \n"
  "packuswb mm3,mm0            \n"
  "movd  [rdi+1*8], mm3        \n"
  "paddsw  mm5, mm1            \n"
  "paddw mm6,__jpmm_offset128  \n"
  "packuswb mm6,mm0            \n"
  "movd  [rdi+2*8], mm6        \n"
  "psubsw  mm4, mm1            \n"
  "movq  mm3, [rax+8]          \n"
  "psraw  mm5, 6               \n"
  "movq  mm6, mm2              \n"
  "psraw  mm4, 6               \n"
  "paddw mm0,__jpmm_offset128  \n"
  "packuswb mm0,mm1            \n"
  "movd  [rdi+5*8], mm0        \n"
  "paddsw  mm2, mm3            \n"
  "paddw mm7,__jpmm_offset128  \n"
  "packuswb mm7,mm0            \n"
  "movd  [rdi+6*8], mm7        \n"
  "psubsw  mm6, mm3            \n"
  "paddw mm5,__jpmm_offset128  \n"
  "packuswb mm5,mm0            \n"
  "movd  [rdi+0*8], mm5        \n"
  "psraw  mm2, 6               \n"
  "paddw mm4,__jpmm_offset128  \n"
  "packuswb mm4,mm0            \n"
  "movd  [rdi+7*8], mm4        \n"
  "psraw  mm6, 6               \n"
  "paddw mm2,__jpmm_offset128  \n"
  "packuswb mm2,mm0            \n"
  "movd  [rdi+3*8], mm2        \n"
  "paddw mm6,__jpmm_offset128  \n"
  "packuswb mm6,mm0            \n"
  "movd  [rdi+4*8], mm6        \n"
  "add rdi,4                   \n"
  "add rsi,8                   \n"
  "dec rcx                     \n"
  "jnz __mmx_idct_cols         \n"
  "emms                        \n"
  ".att_syntax                 \n"
  : /* no output */
  : "S"(innerBuff),"D"(dest),"a"(scratch)
  : "memory","rcx","mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7"
  );


#else

  // gcc inline assembly code (32bits)
  // Rows
  __asm__ (
  ".intel_syntax noprefix     \n"
  "push ebx                   \n"
  "mov ecx, 8                 \n"
  "lea eax, __jpmm_row_tabs   \n"
  "lea ebx, __jpmm_rounder    \n"
"__mmx_idct_rows:             \n"
  "movq mm0, [esi]            \n"
  "movq mm1, [esi+8]          \n"
  "movq mm2, mm0              \n"
  "movq mm3,  [eax]           \n"
  "punpcklwd mm0, mm1         \n"
  "movq mm5, mm0              \n"
  "punpckldq mm0, mm0         \n"
  "movq mm4,  [eax+8]         \n"
  "punpckhwd mm2, mm1         \n"
  "pmaddwd mm3, mm0           \n"
  "movq mm6, mm2              \n"
  "movq mm1,  [eax+32]        \n"
  "punpckldq mm2, mm2         \n"
  "pmaddwd mm4, mm2           \n"
  "punpckhdq mm5, mm5         \n"
  "pmaddwd mm0,  [eax+16]     \n"
  "punpckhdq mm6, mm6         \n"
  "movq mm7,  [eax+40]        \n"
  "pmaddwd mm1, mm5           \n"
  "paddd mm3,  [ebx]          \n"
  "pmaddwd mm7, mm6           \n"
  "pmaddwd mm2,  [eax+24]     \n"
  "paddd mm3, mm4             \n"
  "pmaddwd mm5,  [eax+48]     \n"
  "movq mm4, mm3              \n"
  "pmaddwd mm6,  [eax+56]     \n"
  "paddd mm1, mm7             \n"
  "paddd mm0,  [ebx]          \n"
  "psubd mm3, mm1             \n"
  "psrad mm3, 11              \n"
  "paddd mm1, mm4             \n"
  "paddd mm0, mm2             \n"
  "psrad mm1, 11              \n"
  "paddd mm5, mm6             \n"
  "movq mm4, mm0              \n"
  "paddd mm0, mm5             \n"
  "psubd mm4, mm5             \n"
  "psrad mm0, 11              \n"
  "psrad mm4, 11              \n"
  "packssdw mm1, mm0          \n"
  "packssdw mm4, mm3          \n"
  "movq mm7, mm4              \n"
  "psrld mm4, 16              \n"
  "pslld mm7, 16              \n"
  "movq  [edi], mm1           \n"
  "por mm7, mm4               \n"
  "movq  [edi+8], mm7         \n"
  "add esi, 16                \n"
  "add edi, 16                \n"
  "add eax, 64                \n"
  "add ebx, 8                 \n"
  "dec ecx                    \n"
  "jnz __mmx_idct_rows        \n"
  "pop ebx                    \n"
  ".att_syntax                \n"
  : /* no output */
  : "D"(innerBuff),"S"(block)
  : "memory","eax","ecx","mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7"
  );

  // Columns
  __asm__ (
  ".intel_syntax noprefix      \n"
  "mov ecx, 2                  \n"
"__mmx_idct_cols:              \n"
  "movq  mm0, __jpmm_tg_3_16   \n"
  "movq  mm3, [esi+16*3]       \n"
  "movq  mm1, mm0              \n"
  "movq  mm5, [esi+16*5]       \n"
  "pmulhw  mm0, mm3            \n"
  "movq  mm4, __jpmm_tg_1_16   \n"
  "pmulhw  mm1, mm5            \n"
  "movq  mm7, [esi+16*7]       \n"
  "movq  mm2, mm4              \n"
  "movq  mm6, [esi+16*1]       \n"
  "pmulhw  mm4, mm7            \n"
  "paddsw  mm0, mm3            \n"
  "pmulhw  mm2, mm6            \n"
  "paddsw  mm1, mm3            \n"
  "psubsw  mm0, mm5            \n"
  "movq  mm3, __jpmm_ocos_4_16 \n"
  "paddsw  mm1, mm5            \n"
  "paddsw  mm4, mm6            \n"
  "psubsw  mm2, mm7            \n"
  "movq  mm5, mm4              \n"
  "movq  mm6, mm2              \n"
  "paddsw  mm5, mm1            \n"
  "psubsw  mm6, mm0            \n"
  "psubsw  mm4, mm1            \n"
  "paddsw  mm2, mm0            \n"
  "movq  mm7, __jpmm_tg_2_16   \n"
  "movq  mm1, mm4              \n"
  "movq  [eax+0], mm5          \n"
  "paddsw  mm1, mm2            \n"
  "movq  [eax+8], mm6          \n"
  "psubsw  mm4, mm2            \n"
  "movq  mm5, [esi+2*16]       \n"
  "movq  mm0, mm7              \n"
  "movq  mm6, [esi+6*16]       \n"
  "pmulhw  mm0, mm5            \n"
  "pmulhw  mm7, mm6            \n"
  "pmulhw  mm1, mm3            \n"
  "movq  mm2, [esi+0*16]       \n"
  "pmulhw  mm4, mm3            \n"
  "psubsw  mm0, mm6            \n"
  "movq  mm3, mm2              \n"
  "movq  mm6, [esi+4*16]       \n"
  "paddsw  mm7, mm5            \n"
  "paddsw  mm2, mm6            \n"
  "psubsw  mm3, mm6            \n"
  "movq  mm5, mm2              \n"
  "movq  mm6, mm3              \n"
  "psubsw  mm2, mm7            \n"
  "paddsw  mm3, mm0            \n"
  "paddsw mm1, mm1             \n"
  "paddsw mm4, mm4             \n"
  "paddsw  mm5, mm7            \n"
  "psubsw  mm6, mm0            \n"
  "movq  mm7, mm3              \n"
  "movq  mm0, mm6              \n"
  "paddsw  mm3, mm1            \n"
  "paddsw  mm6, mm4            \n"
  "psraw  mm3, 6               \n"
  "psubsw  mm7, mm1            \n"
  "psraw  mm6, 6               \n"
  "psubsw  mm0, mm4            \n"
   "movq  mm1, [eax+0]         \n"
  "psraw  mm7, 6               \n"
  "movq  mm4, mm5              \n"
  "psraw  mm0, 6               \n"
  "paddw mm3,__jpmm_offset128  \n"
  "packuswb mm3,mm0            \n"
  "movd  [edi+1*8], mm3        \n"
  "paddsw  mm5, mm1            \n"
  "paddw mm6,__jpmm_offset128  \n"
  "packuswb mm6,mm0            \n"
  "movd  [edi+2*8], mm6        \n"
  "psubsw  mm4, mm1            \n"
  "movq  mm3, [eax+8]          \n"
  "psraw  mm5, 6               \n"
  "movq  mm6, mm2              \n"
  "psraw  mm4, 6               \n"
  "paddw mm0,__jpmm_offset128  \n"
  "packuswb mm0,mm1            \n"
  "movd  [edi+5*8], mm0        \n"
  "paddsw  mm2, mm3            \n"
  "paddw mm7,__jpmm_offset128  \n"
  "packuswb mm7,mm0            \n"
  "movd  [edi+6*8], mm7        \n"
  "psubsw  mm6, mm3            \n"
  "paddw mm5,__jpmm_offset128  \n"
  "packuswb mm5,mm0            \n"
  "movd  [edi+0*8], mm5        \n"
  "psraw  mm2, 6               \n"
  "paddw mm4,__jpmm_offset128  \n"
  "packuswb mm4,mm0            \n"
  "movd  [edi+7*8], mm4        \n"
  "psraw  mm6, 6               \n"
  "paddw mm2,__jpmm_offset128  \n"
  "packuswb mm2,mm0            \n"
  "movd  [edi+3*8], mm2        \n"
  "paddw mm6,__jpmm_offset128  \n"
  "packuswb mm6,mm0            \n"
  "movd  [edi+4*8], mm6        \n"
  "add edi,4                   \n"
  "add esi,8                   \n"
  "dec ecx                     \n"
  "jnz __mmx_idct_cols         \n"
  "emms                        \n"
  ".att_syntax                 \n"
  : /* no output */
  : "S"(innerBuff),"D"(dest),"a"(scratch)
  : "memory","ecx","mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7"
  );

#endif /* _64BITS  */

#endif /* _WINDOWS */

}

#endif /* JPG_USE_ASM */
