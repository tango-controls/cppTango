///=============================================================================	
//
// file :		jpeg_color_mmx.cpp
//
// description :        Simple jpeg coding/decoding library
//                      Color space conversion (MMX routines)
//
// project :		TANGO
//
// author(s) :		JL Pons
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011
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
// Revision 1.7  2009/08/27 07:24:30  taurel
// - Commit after another merge with Release_7_0_2-bugfixes branch
//
// Revision 1.6.2.2  2009/08/26 07:16:52  taurel
// - Fix bugs in previous change related to PIC code
//
// Revision 1.6.2.1  2009/08/25 14:03:10  taurel
// - First attempt to clarify the gcc PIC problem
//
// Revision 1.6  2009/04/20 14:55:58  jlpons
// Added GPL header, changed memory allocation to C++ fashion.
//
//=============================================================================

//------------------------------------------------------------------------------
//
// (YCbCr to RGB) Y,Cb and Cr range in [0,255]
// R = Y + 1.402   * (Cr-128)
// G = Y - 0.34414 * (Cb-128) - 0.71414*(Cr-128)
// B = Y + 1.772   * (Cb-128)
//
// (RGB to YCbCr) R,G,B range in [0,255]
// Y  =  0.299   * R + 0.587   * G + 0.114   * B
// Cb = -0.16874 * R - 0.33126 * G + 0.5     * B  + 128
// Cr =  0.5     * R - 0.41869 * G - 0.08131 * B  + 128
//
//------------------------------------------------------------------------------

#include "jpeg_memory.h"
#include "jpeg_lib.h"
#include <string.h>

#ifdef JPG_USE_ASM

#ifdef _WINDOWS
// Disable "has no EMMS instruction" warning
#pragma warning( disable : 4799 )
#endif

#define FIX14(x)  ((short) ((x) * (1L<<14) + 0.5))
#define FIX15(x)  ((short) ((x) * (1L<<15) + 0.5))
#define FIX16(x)  ((short) ((x) * (1L<<16) + 0.5))

#ifdef _WINDOWS
// Visual C++ align directive
#define ALIGN8 __declspec(align(8))
#else
// gcc align directive
#define ALIGN8 __attribute__ ((aligned (8)))
#endif


// MMX constant (YCbCr to RGB)
ALIGN8 short  _cbgmm[]  = { -FIX14(0.34414),-FIX14(0.34414),-FIX14(0.34414),-FIX14(0.34414) };
ALIGN8 short  _cbbmm[]  = {  FIX14(1.772)  , FIX14(1.772)  , FIX14(1.772)  , FIX14(1.772)   };
ALIGN8 short  _crgmm[]  = { -FIX14(0.71414),-FIX14(0.71414),-FIX14(0.71414),-FIX14(0.71414) };
ALIGN8 short  _crrmm[]  = {  FIX14(1.402)  , FIX14(1.402)  , FIX14(1.402)  , FIX14(1.402)   };
ALIGN8 short  _128mm[]  = { 128,128,128,128 };

// RGB to YCbCr
ALIGN8 short  _rymm[]    = { FIX15(0.299), FIX15(0.299), FIX15(0.299), FIX15(0.299) };
ALIGN8 short  _gymm[]    = { FIX15(0.587), FIX15(0.587), FIX15(0.587), FIX15(0.587) };
ALIGN8 short  _bymm[]    = { FIX15(0.114), FIX15(0.114), FIX15(0.114), FIX15(0.114) };
ALIGN8 short  _offymm[]  = { -127,-127,-127,-127 }; // +1 for rounding
ALIGN8 short  _rcbcrmm[] = {-FIX15(0.16874),-FIX15(0.16874), FIX15(0.5)    , FIX15(0.5)    };
ALIGN8 short  _gcbcrmm[] = {-FIX15(0.33126),-FIX15(0.33126),-FIX15(0.41869),-FIX15(0.41869)};
ALIGN8 short  _bcbcrmm[] = { FIX15(0.5)    , FIX15(0.5)    ,-FIX15(0.08131),-FIX15(0.08131)};
ALIGN8 short  _rcmm[]    = { 1,1,1,1 };

//
// Unfortunately with gcc, code like "psubw     mm2,_128mm" is considered like non PIC
// and the scanelf utility complains about this. This is also a problem for distribution like
// Debian. Why, I don't know !!
// This is a known problem and some WEB page have been written relatedto this issue.
// I have used the page http://www.gentoo.org/proj/en/hardened/pic-fix-guide.xml
// Within this page, they give 3 pssobilities to remove this kind of code.
// I have use case number 1. Case number 2 and 3 are not really usable because
// all the registers are already used (Not in all functions but in most of them)
// Therefore, every variable defined previously are "manually" loaded into the MMX registers
// using the stack as temporary storage
//
// -FIX14(0.34414) --> 0xe9fa
// FIX14(1.772)    --> 0x7168
// -FIX14(0.71414) --> 0xd24c
// FIX14(1.402)	   --> 0x59ba
// 128             --> 0x0080
// FIX15(0.299)    --> 0x2646
// FIX15(0.587)    --> 0x4b23
// FIX15(0.114)    --> 0x0e98
// -127            --> 0xff81
// -FIX15(0.16874) --> 0xea67
// FIX15(0.5)      --> 0x4000
// -FIX15(0.33126) --> 0xd599
// -FIX15(0.41869) --> 0xca68
// -FIX15(0.08131) --> 0xf598
// 1               --> 0x0001
//
// This problem is the reason of all the following macros.
// It's not a nice way to solve it> When we will have time, we will 
// try to find a cleaner solution
//

#ifndef _WINDOWS

#ifdef __PIC__

#define sub_128_mmx_reg(REG1,REG2) \
	"push	0x00800080	\n" \
	"push	0x00800080	\n"	\
	"psubw	"#REG1", QWORD PTR [esp]	\n" \
	"psubw	"#REG2", QWORD PTR [esp]	\n" \
	"add	esp,8	\n"
	
#define add_127_mmx_reg(REG1) \
	"push	0xff81ff81	\n" \
	"push	0xff81ff81	\n"	\
	"paddw	"#REG1", QWORD PTR [esp]	\n" \
	"add	esp,8	\n"
	
#define add_1_mmx_reg(REG1) \
	"push	0x00010001	\n" \
	"push	0x00010001	\n"	\
	"paddw	"#REG1", QWORD PTR [esp]	\n" \
	"add	esp,8	\n"
	
#define mul_cxx_mmx_reg(REG1,REG2,REG3,REG4) \
	"push	0xe9fae9fa	\n" \
	"push	0xe9fae9fa	\n"	\
	"pmulhw "#REG1", QWORD PTR [esp]	\n" \
	"push	0x71687168	\n" \
	"push	0x71687168	\n"	\
	"pmulhw "#REG2", QWORD PTR [esp]	\n" \
	"push	0xd24cd24c	\n" \
	"push	0xd24cd24c	\n"	\
	"pmulhw "#REG3", QWORD PTR [esp]	\n" \
	"push	0x59ba59ba	\n" \
	"push	0x59ba59ba	\n"	\
	"pmulhw "#REG4", QWORD PTR [esp]	\n" \
	"add	esp,32	\n"
	
#define mul_rgb_mmx_reg(REG1,REG2,REG3) \
	"push	0x26462646	\n" \
	"push	0x26462646	\n"	\
	"pmulhw "#REG1", QWORD PTR [esp]	\n" \
	"push	0x4b234b23	\n" \
	"push	0x4b234b23	\n"	\
	"pmulhw "#REG2", QWORD PTR [esp]	\n" \
	"push	0x0e980e98	\n" \
	"push	0x0e980e98	\n"	\
	"pmulhw "#REG3", QWORD PTR [esp]	\n" \
	"add	esp,24	\n"
	
#define mul_rgbc_mmx_reg(REG1,REG2,REG3) \
	"push	0x40004000	\n"	\
	"push	0xea67ea67	\n" \
	"pmulhw "#REG1", QWORD PTR [esp]	\n" \
	"push	0xca68ca68	\n"	\
	"push	0xd599d599	\n" \
	"pmulhw "#REG2", QWORD PTR [esp]	\n" \
	"push	0xf598f598	\n"	\
	"push	0x40004000	\n" \
	"pmulhw "#REG3", QWORD PTR [esp]	\n" \
	"add	esp,24	\n"

#else /* __PIC__ */

#define sub_128_mmx_reg(REG1,REG2) \
    "psubw     "#REG1",_128mm      \n" \
    "psubw     "#REG2",_128mm      \n"
	
#define add_127_mmx_reg(REG1) \
    "paddw "#REG1",_offymm       \n"
	
#define add_1_mmx_reg(REG1) \
    "paddw "#REG1",_rcmm         \n"
	
#define mul_cxx_mmx_reg(REG1,REG2,REG3,REG4) \
    "pmulhw    "#REG1",_cbgmm      \n" \
    "pmulhw    "#REG2",_cbbmm      \n" \
    "pmulhw    "#REG3",_crgmm      \n" \
    "pmulhw    "#REG4",_crrmm      \n"
	
#define mul_rgb_mmx_reg(REG1,REG2,REG3) \
      "pmulhw "#REG1",_rymm        \n" \
      "pmulhw "#REG2",_gymm        \n" \
      "pmulhw "#REG3",_bymm        \n"
	
#define mul_rgbc_mmx_reg(REG1,REG2,REG3) \
      "pmulhw "#REG1",_rcbcrmm     \n" \
      "pmulhw "#REG2",_gcbcrmm     \n" \
      "pmulhw "#REG3",_bcbcrmm     \n"
	  
#endif /* __PIC__ */

		
#endif /* _WINDOWS */

//------------------------------------------------------------------------------
// MCU16x16 YCbCr H2V2 (2x2:1:1, 6 blocks per MCU) to 32-bit RGB
//------------------------------------------------------------------------------

void jpeg_yh2v2_to_rgb32_mmx(unsigned char *block,long width,unsigned char *rgb) {

  unsigned char *y  = block;
  unsigned char *cb = block+64*4;
  long  j,y0;
  // ! Due to wrong gcc stack code (does not detect push), y0 is cleared during asm !

  for(j=0;j<8;j++) {
    y0 = ((j&4)<<5) + ((j&3)<<4);

#ifdef _WINDOWS

    // Visual C++ inline assembly    
    _asm {

      mov  edi,rgb
      mov  esi,y
      mov  eax,width
      mov  ebx,cb
      mov  edx,y0

      xor  ecx,ecx
      shl  eax,2

__blrow_h2v2:

      // Y block offset
      add edx,ecx

      // -- 00 --

      movd mm1,[esi+edx] // [0000][y11][y10][y01][y00]
      movd mm2,[ebx]     // [0000][cb3][cb2][cb1][cb0]
      movd mm3,[ebx+64]  // [0000][cr3][cr2][cr1][cr0]

      pxor      mm0,mm0
      punpcklbw mm1,mm0   // [y11][y10][y01][y00]
      punpcklbw mm2,mm0   // [cb3][cb2][cb1][cb0]
      punpcklbw mm3,mm0   // [cr3][cr2][cr1][cr0]
      psubw     mm2,_128mm
      psubw     mm3,_128mm
      psllw     mm2,2
      psllw     mm3,2
      movq      mm4,mm2
      movq      mm5,mm3
      pmulhw    mm2,_cbgmm
      pmulhw    mm4,_cbbmm
      pmulhw    mm3,_crgmm
      pmulhw    mm5,_crrmm

      movq      mm6,mm5
      punpcklwd mm6,mm6    // [cr1*crr][cr1*crr][cr0*crr][cr0*crr]
      paddw     mm6,mm1    // R3R2R1R0

      movq      mm0,mm2
      movq      mm7,mm3
      punpcklwd mm0,mm0    // [cb1*cbg][cb1*cbg][cb0*cbg][cb0*cbg]
      punpcklwd mm7,mm7    // [cr1*crg][cr1*crg][cr0*crg][cr0*crg]
      paddw     mm0,mm1
      paddw     mm0,mm7    // G3G2G1G0

      movq      mm7,mm4
      punpcklwd mm7,mm7    // [cb1*cbb][cb1*cbb][cb0*cbb][cb0*cbb]
      paddw     mm7,mm1    // B3B2B1B0

      pxor      mm1,mm1
      packuswb  mm6,mm1    // [0000]R3R2R1R0
      packuswb  mm0,mm1    // [0000]G3G2G1G0
      packuswb  mm7,mm1    // [0000]B3B2B1B0

      punpcklbw mm6,mm0    // G3R3G2R2G1R1G0R0
      punpcklbw mm7,mm1    // 00B300B200B100B0
      movq      mm1,mm6
      punpcklwd mm1,mm7    // 00B1G1R100B0G0R0      
      punpckhwd mm6,mm7    // 00B3G3R300B2G2R2

      movq      [edi]  ,mm1
      movq      [edi+8],mm6

      // -- 01 --

      movd mm1,[esi+edx+4] // [0000][y31][y30][y21][y20]
      pxor      mm0,mm0
      punpcklbw mm1,mm0

      movq      mm6,mm5
      punpckhwd mm6,mm6    // [cr3*crr][cr3*crr][cr2*crr][cr2*crr]
      paddw     mm6,mm1    // R3R2R1R0

      movq      mm0,mm2
      movq      mm7,mm3
      punpckhwd mm0,mm0    // [cb3*cbg][cb3*cbg][cb2*cbg][cb2*cbg]
      punpckhwd mm7,mm7    // [cr3*crg][cr3*crg][cr2*crg][cr2*crg]
      paddw     mm0,mm1
      paddw     mm0,mm7    // G3G2G1G0

      movq      mm7,mm4
      punpckhwd mm7,mm7    // [cb3*cbb][cb3*cbb][cb2*cbb][cb2*cbb]
      paddw     mm7,mm1    // B3B2B1B0

      pxor      mm1,mm1
      packuswb  mm6,mm1    // [0000]R3R2R1R0
      packuswb  mm0,mm1    // [0000]G3G2G1G0
      packuswb  mm7,mm1    // [0000]B3B2B1B0

      punpcklbw mm6,mm0    // G3R3G2R2G1R1G0R0
      punpcklbw mm7,mm1    // 00B300B200B100B0
      movq      mm1,mm6
      punpcklwd mm1,mm7    // 00B1G1R100B0G0R0      
      punpckhwd mm6,mm7    // 00B3G3R300B2G2R2

      movq      [edi+16],mm1
      movq      [edi+24],mm6

      // -- 10 --

      movd mm1,[esi+edx+8] // [0000][y11][y10][y01][y00]
      pxor      mm0,mm0
      punpcklbw mm1,mm0

      movq      mm6,mm5
      punpcklwd mm6,mm6    // [cr1*crr][cr1*crr][cr0*crr][cr0*crr]
      paddw     mm6,mm1    // R3R2R1R0

      movq      mm0,mm2
      movq      mm7,mm3
      punpcklwd mm0,mm0    // [cb1*cbg][cb1*cbg][cb0*cbg][cb0*cbg]
      punpcklwd mm7,mm7    // [cr1*crg][cr1*crg][cr0*crg][cr0*crg]
      paddw     mm0,mm1
      paddw     mm0,mm7    // G3G2G1G0

      movq      mm7,mm4
      punpcklwd mm7,mm7    // [cb1*cbb][cb1*cbb][cb0*cbb][cb0*cbb]
      paddw     mm7,mm1    // B3B2B1B0

      pxor      mm1,mm1
      packuswb  mm6,mm1    // [0000]R3R2R1R0
      packuswb  mm0,mm1    // [0000]G3G2G1G0
      packuswb  mm7,mm1    // [0000]B3B2B1B0

      punpcklbw mm6,mm0    // G3R3G2R2G1R1G0R0
      punpcklbw mm7,mm1    // 00B300B200B100B0
      movq      mm1,mm6
      punpcklwd mm1,mm7    // 00B1G1R100B0G0R0      
      punpckhwd mm6,mm7    // 00B3G3R300B2G2R2

      movq      [edi+eax]  ,mm1
      movq      [edi+eax+8],mm6

      // -- 11 --

      movd mm1,[esi+edx+12] // [0000][y31][y30][y21][y20]
      pxor      mm0,mm0
      punpcklbw mm1,mm0

      movq      mm6,mm5
      punpckhwd mm6,mm6    // [cr3*crr][cr3*crr][cr2*crr][cr2*crr]
      paddw     mm6,mm1    // R3R2R1R0

      movq      mm0,mm2
      movq      mm7,mm3
      punpckhwd mm0,mm0    // [cb3*cbg][cb3*cbg][cb2*cbg][cb2*cbg]
      punpckhwd mm7,mm7    // [cr3*crg][cr3*crg][cr2*crg][cr2*crg]
      paddw     mm0,mm1
      paddw     mm0,mm7    // G3G2G1G0

      movq      mm7,mm4
      punpckhwd mm7,mm7    // [cb3*cbb][cb3*cbb][cb2*cbb][cb2*cbb]
      paddw     mm7,mm1    // B3B2B1B0

      pxor      mm1,mm1
      packuswb  mm6,mm1    // [0000]R3R2R1R0
      packuswb  mm0,mm1    // [0000]G3G2G1G0
      packuswb  mm7,mm1    // [0000]B3B2B1B0

      punpcklbw mm6,mm0    // G3R3G2R2G1R1G0R0
      punpcklbw mm7,mm1    // 00B300B200B100B0
      movq      mm1,mm6
      punpcklwd mm1,mm7    // 00B1G1R100B0G0R0      
      punpckhwd mm6,mm7    // 00B3G3R300B2G2R2

      movq      [edi+eax+16],mm1
      movq      [edi+eax+24],mm6

      sub edx,ecx          // Restore edx
      add edi,32
      add ebx,4
      add ecx,64
      cmp ecx,128
      jl  __blrow_h2v2

    }

#else

    // GCC inline assembly code
    __asm__ (
    ".intel_syntax noprefix    \n"
#ifdef _64BITS
    "push rbx                  \n"
    "mov  rbx,rcx              \n"    
    "xor  rcx,rcx              \n"
    "shl  rax,2                \n"
"__blrow_h2v2:                 \n"
    "add rdx,rcx               \n"
    "movd mm1,[rsi+rdx]        \n"
    "movd mm2,[rbx]            \n"
    "movd mm3,[rbx+64]         \n"
#else
    "push ebx                  \n"
    "mov  ebx,ecx              \n"    
    "xor  ecx,ecx              \n"
    "shl  eax,2                \n"
"__blrow_h2v2:                 \n"
    "add edx,ecx               \n"
    "movd mm1,[esi+edx]        \n"
    "movd mm2,[ebx]            \n"
    "movd mm3,[ebx+64]         \n"
#endif
    "pxor      mm0,mm0         \n"
    "punpcklbw mm1,mm0         \n"
    "punpcklbw mm2,mm0         \n"
    "punpcklbw mm3,mm0         \n"
	sub_128_mmx_reg(mm2,mm3)
    "psllw     mm2,2           \n"
    "psllw     mm3,2           \n"
    "movq      mm4,mm2         \n"
    "movq      mm5,mm3         \n"
	 mul_cxx_mmx_reg(mm2,mm4,mm3,mm5)
    "movq      mm6,mm5         \n"
    "punpcklwd mm6,mm6         \n"
    "paddw     mm6,mm1         \n"
    "movq      mm0,mm2         \n"
    "movq      mm7,mm3         \n"
    "punpcklwd mm0,mm0         \n"
    "punpcklwd mm7,mm7         \n"
    "paddw     mm0,mm1         \n"
    "paddw     mm0,mm7         \n"
    "movq      mm7,mm4         \n"
    "punpcklwd mm7,mm7         \n"
    "paddw     mm7,mm1         \n"
    "pxor      mm1,mm1         \n"
    "packuswb  mm6,mm1         \n"
    "packuswb  mm0,mm1         \n"
    "packuswb  mm7,mm1         \n"
    "punpcklbw mm6,mm0         \n"
    "punpcklbw mm7,mm1         \n"
    "movq      mm1,mm6         \n"
    "punpcklwd mm1,mm7         \n"  
    "punpckhwd mm6,mm7         \n"
#ifdef _64BITS
    "movq      [rdi]  ,mm1     \n"
    "movq      [rdi+8],mm6     \n"
    "movd mm1,[rsi+rdx+4]      \n"
#else
    "movq      [edi]  ,mm1     \n"
    "movq      [edi+8],mm6     \n"
    "movd mm1,[esi+edx+4]      \n"
#endif
    "pxor      mm0,mm0         \n"
    "punpcklbw mm1,mm0         \n"
    "movq      mm6,mm5         \n"
    "punpckhwd mm6,mm6         \n"
    "paddw     mm6,mm1         \n"
    "movq      mm0,mm2         \n"
    "movq      mm7,mm3         \n"
    "punpckhwd mm0,mm0         \n"
    "punpckhwd mm7,mm7         \n"
    "paddw     mm0,mm1         \n"
    "paddw     mm0,mm7         \n"
    "movq      mm7,mm4         \n"
    "punpckhwd mm7,mm7         \n"
    "paddw     mm7,mm1         \n"
    "pxor      mm1,mm1         \n"
    "packuswb  mm6,mm1         \n"
    "packuswb  mm0,mm1         \n"
    "packuswb  mm7,mm1         \n"
    "punpcklbw mm6,mm0         \n"
    "punpcklbw mm7,mm1         \n"
    "movq      mm1,mm6         \n"
    "punpcklwd mm1,mm7         \n"
    "punpckhwd mm6,mm7         \n"
#ifdef _64BITS
    "movq      [rdi+16],mm1    \n"
    "movq      [rdi+24],mm6    \n"
    "movd mm1,[rsi+rdx+8]      \n"
#else
    "movq      [edi+16],mm1    \n"
    "movq      [edi+24],mm6    \n"
    "movd mm1,[esi+edx+8]      \n"
#endif
    "pxor      mm0,mm0         \n"
    "punpcklbw mm1,mm0         \n"
    "movq      mm6,mm5         \n"
    "punpcklwd mm6,mm6         \n"
    "paddw     mm6,mm1         \n"
    "movq      mm0,mm2         \n"
    "movq      mm7,mm3         \n"
    "punpcklwd mm0,mm0         \n"
    "punpcklwd mm7,mm7         \n"
    "paddw     mm0,mm1         \n"
    "paddw     mm0,mm7         \n"
    "movq      mm7,mm4         \n"
    "punpcklwd mm7,mm7         \n"
    "paddw     mm7,mm1         \n"
    "pxor      mm1,mm1         \n"
    "packuswb  mm6,mm1         \n"
    "packuswb  mm0,mm1         \n"
    "packuswb  mm7,mm1         \n"
    "punpcklbw mm6,mm0         \n"
    "punpcklbw mm7,mm1         \n"
    "movq      mm1,mm6         \n"
    "punpcklwd mm1,mm7         \n"
    "punpckhwd mm6,mm7         \n"
#ifdef _64BITS
    "movq      [rdi+rax]  ,mm1 \n"
    "movq      [rdi+rax+8],mm6 \n"
    "movd mm1,[rsi+rdx+12]     \n"
#else 
    "movq      [edi+eax]  ,mm1 \n"
    "movq      [edi+eax+8],mm6 \n"
    "movd mm1,[esi+edx+12]     \n"
#endif   
    "pxor      mm0,mm0         \n"
    "punpcklbw mm1,mm0         \n"
    "movq      mm6,mm5         \n"
    "punpckhwd mm6,mm6         \n"
    "paddw     mm6,mm1         \n"
    "movq      mm0,mm2         \n"
    "movq      mm7,mm3         \n"
    "punpckhwd mm0,mm0         \n"
    "punpckhwd mm7,mm7         \n"
    "paddw     mm0,mm1         \n"
    "paddw     mm0,mm7         \n"
    "movq      mm7,mm4         \n"
    "punpckhwd mm7,mm7         \n"
    "paddw     mm7,mm1         \n"
    "pxor      mm1,mm1         \n"
    "packuswb  mm6,mm1         \n"
    "packuswb  mm0,mm1         \n"
    "packuswb  mm7,mm1         \n"
    "punpcklbw mm6,mm0         \n"
    "punpcklbw mm7,mm1         \n"
    "movq      mm1,mm6         \n"
    "punpcklwd mm1,mm7         \n"
    "punpckhwd mm6,mm7         \n"
#ifdef _64BITS
    "movq     [rdi+rax+16],mm1 \n"
    "movq     [rdi+rax+24],mm6 \n"
    "sub rdx,rcx               \n"
    "add rdi,32                \n"
    "add rbx,4                 \n"
    "add rcx,64                \n"
    "cmp rcx,128               \n"
    "jl  __blrow_h2v2          \n"
    "pop rbx                   \n"
#else 
    "movq     [edi+eax+16],mm1 \n"
    "movq     [edi+eax+24],mm6 \n"
    "sub edx,ecx               \n"
    "add edi,32                \n"
    "add ebx,4                 \n"
    "add ecx,64                \n"
    "cmp ecx,128               \n"
    "jl  __blrow_h2v2          \n"
    "pop ebx                   \n"
#endif
    ".att_syntax               \n"
    : /* no output */
    : "D"(rgb),"S"(y),"a"(width),"c"(cb),"d"(y0)
    : "memory","mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7"
    );

#endif

    cb += 8;
    rgb += 8*width;

  }

}

//------------------------------------------------------------------------------
// MCU8x8 YCbCr H1V1 (1x1:1:1, 3 blocks per MCU) to 32-bit RGB
//------------------------------------------------------------------------------

void jpeg_yh1v1_to_rgb32_mmx(unsigned char *block,long width,unsigned char *rgb) {

#ifdef _WINDOWS

  // Visual C++ inline assembly
  _asm {

    mov  edi,rgb
    mov  esi,block
    mov  eax,width
    shl  eax,2
    mov  ecx,8
    pxor mm0,mm0

__blcol_h1v1:

    // -----

    movd mm1,[esi]     // [0000][y03][y02][y01][y00]
    movd mm2,[esi+64]  // [0000][cb3][cb2][cb1][cb0]
    movd mm3,[esi+128] // [0000][cr3][cr2][cr1][cr0]

    punpcklbw mm1,mm0   // [y03][y02][y01][y00]
    punpcklbw mm2,mm0   // [cb3][cb2][cb1][cb0]
    punpcklbw mm3,mm0   // [cr3][cr2][cr1][cr0]
    psubw     mm2,_128mm
    psubw     mm3,_128mm
    psllw     mm2,2
    psllw     mm3,2
    movq      mm4,mm2
    movq      mm5,mm3
    pmulhw    mm2,_cbgmm  // [cb3*cbg][cb2*cbg][cb1*cbg][cb0*cbg]
    pmulhw    mm4,_cbbmm  // [cb3*cbb][cb2*cbb][cb1*cbb][cb0*cbb]
    pmulhw    mm3,_crgmm  // [cb3*crg][cb2*crg][cb1*crg][cb0*crg]
    pmulhw    mm5,_crrmm  // [cb3*crr][cb2*crr][cb1*crr][cb0*crr]

    paddw     mm5,mm1    // R3R2R1R0
    paddw     mm2,mm1
    paddw     mm2,mm3    // G3G2G1G0
    paddw     mm4,mm1    // B3B2B1B0

    packuswb  mm5,mm0    // [0000]R3R2R1R0
    packuswb  mm2,mm0    // [0000]G3G2G1G0
    packuswb  mm4,mm0    // [0000]B3B2B1B0

    punpcklbw mm5,mm2    // G3R3G2R2G1R1G0R0
    punpcklbw mm4,mm0    // 00B300B200B100B0
    movq      mm1,mm5
    punpcklwd mm1,mm4    // 00B1G1R100B0G0R0      
    punpckhwd mm5,mm4    // 00B3G3R300B2G2R2

    movq      [edi],mm1
    movq      [edi+8],mm5

    // -----

    movd mm1,[esi+4]   // [0000][y03][y02][y01][y00]
    movd mm2,[esi+68]  // [0000][cb3][cb2][cb1][cb0]
    movd mm3,[esi+132] // [0000][cr3][cr2][cr1][cr0]

    punpcklbw mm1,mm0   // [y03][y02][y01][y00]
    punpcklbw mm2,mm0   // [cb3][cb2][cb1][cb0]
    punpcklbw mm3,mm0   // [cr3][cr2][cr1][cr0]
    psubw     mm2,_128mm
    psubw     mm3,_128mm
    psllw     mm2,2
    psllw     mm3,2
    movq      mm4,mm2
    movq      mm5,mm3
    pmulhw    mm2,_cbgmm  // [cb3*cbg][cb2*cbg][cb1*cbg][cb0*cbg]
    pmulhw    mm4,_cbbmm  // [cb3*cbb][cb2*cbb][cb1*cbb][cb0*cbb]
    pmulhw    mm3,_crgmm  // [cb3*crg][cb2*crg][cb1*crg][cb0*crg]
    pmulhw    mm5,_crrmm  // [cb3*crr][cb2*crr][cb1*crr][cb0*crr]

    paddw     mm5,mm1    // R3R2R1R0
    paddw     mm2,mm1
    paddw     mm2,mm3    // G3G2G1G0
    paddw     mm4,mm1    // B3B2B1B0

    packuswb  mm5,mm0    // [0000]R3R2R1R0
    packuswb  mm2,mm0    // [0000]G3G2G1G0
    packuswb  mm4,mm0    // [0000]B3B2B1B0

    punpcklbw mm5,mm2    // G3R3G2R2G1R1G0R0
    punpcklbw mm4,mm0    // 00B300B200B100B0
    movq      mm1,mm5
    punpcklwd mm1,mm4    // 00B1G1R100B0G0R0      
    punpckhwd mm5,mm4    // 00B3G3R300B2G2R2

    movq      [edi+16],mm1
    movq      [edi+24],mm5

    add esi,8
    add edi,eax
    dec ecx
    jnz __blcol_h1v1

  }

#else

  // GCC inline assembly code
  __asm__ (
  ".intel_syntax noprefix  \n"
#ifdef _64BITS
  "shl  rax,2              \n"
  "mov  rcx,8              \n"
  "pxor mm0,mm0            \n"
"__blcol_h1v1:             \n"
  "movd mm1,[rsi]          \n"
  "movd mm2,[rsi+64]       \n"
  "movd mm3,[rsi+128]      \n"
#else
  "shl  eax,2              \n"
  "mov  ecx,8              \n"
  "pxor mm0,mm0            \n"
"__blcol_h1v1:             \n"
  "movd mm1,[esi]          \n"
  "movd mm2,[esi+64]       \n"
  "movd mm3,[esi+128]      \n"
#endif
  "punpcklbw mm1,mm0       \n"
  "punpcklbw mm2,mm0       \n"
  "punpcklbw mm3,mm0       \n"
   sub_128_mmx_reg(mm2,mm3)
  "psllw     mm2,2         \n"
  "psllw     mm3,2         \n"
  "movq      mm4,mm2       \n"
  "movq      mm5,mm3       \n"
   mul_cxx_mmx_reg(mm2,mm4,mm3,mm5)
  "paddw     mm5,mm1       \n"
  "paddw     mm2,mm1       \n"
  "paddw     mm2,mm3       \n"
  "paddw     mm4,mm1       \n"
  "packuswb  mm5,mm0       \n"
  "packuswb  mm2,mm0       \n"
  "packuswb  mm4,mm0       \n"
  "punpcklbw mm5,mm2       \n"
  "punpcklbw mm4,mm0       \n"
  "movq      mm1,mm5       \n"
  "punpcklwd mm1,mm4       \n"
  "punpckhwd mm5,mm4       \n"
#ifdef _64BITS
  "movq      [rdi],mm1     \n"
  "movq      [rdi+8],mm5   \n"
  "movd mm1,[rsi+4]        \n"
  "movd mm2,[rsi+68]       \n"
  "movd mm3,[rsi+132]      \n"
#else
  "movq      [edi],mm1     \n"
  "movq      [edi+8],mm5   \n"
  "movd mm1,[esi+4]        \n"
  "movd mm2,[esi+68]       \n"
  "movd mm3,[esi+132]      \n"
#endif
  "punpcklbw mm1,mm0       \n"
  "punpcklbw mm2,mm0       \n"
  "punpcklbw mm3,mm0       \n"
   sub_128_mmx_reg(mm2,mm3)
  "psllw     mm2,2         \n"
  "psllw     mm3,2         \n"
  "movq      mm4,mm2       \n"
  "movq      mm5,mm3       \n"
   mul_cxx_mmx_reg(mm2,mm4,mm3,mm5)
  "paddw     mm5,mm1       \n"
  "paddw     mm2,mm1       \n"
  "paddw     mm2,mm3       \n"
  "paddw     mm4,mm1       \n"
  "packuswb  mm5,mm0       \n"
  "packuswb  mm2,mm0       \n"
  "packuswb  mm4,mm0       \n"
  "punpcklbw mm5,mm2       \n"
  "punpcklbw mm4,mm0       \n"
  "movq      mm1,mm5       \n"
  "punpcklwd mm1,mm4       \n"
  "punpckhwd mm5,mm4       \n"
#ifdef _64BITS
  "movq      [rdi+16],mm1  \n"
  "movq      [rdi+24],mm5  \n"
  "add rsi,8               \n"
  "add rdi,rax             \n"
  "dec rcx                 \n"
  "jnz __blcol_h1v1        \n"
#else
  "movq      [edi+16],mm1  \n"
  "movq      [edi+24],mm5  \n"
  "add esi,8               \n"
  "add edi,eax             \n"
  "dec ecx                 \n"
  "jnz __blcol_h1v1        \n"
#endif
  ".att_syntax             \n"
  : /* no output */
  : "D"(rgb),"S"(block),"a"(width)
#ifdef _64BITS
  : "memory","rcx","mm0","mm1","mm2","mm3","mm4","mm5"
#else
  : "memory","ecx","mm0","mm1","mm2","mm3","mm4","mm5"
#endif
  );

#endif

}

// Convert 8x8 GRAY8 pixel map to (1xY) block
void conv_block_GRAY8Y_mmx(long width,unsigned char *g,short *y) {

#ifdef _WINDOWS

  // Visual C++ inline assembly
  _asm {
      
    mov esi,g
    mov edi,y
    mov eax,width

    mov  ecx,8
    pxor mm0,mm0

__blrow_gray8:

    movd mm1,[esi]
    movd mm2,[esi+4]
    punpcklbw mm1,mm0
    punpcklbw mm2,mm0
    psubw mm1,_128mm
    psubw mm2,_128mm
    movq [edi]  ,mm1
    movq [edi+8],mm2

    add esi,eax
    add edi,16
    dec ecx
    jnz __blrow_gray8

  }

#else

  // GCC inline assembly code
  __asm__ (
  ".intel_syntax noprefix  \n"
#ifdef _64BITS
  "mov  rcx,8              \n"
  "pxor mm0,mm0            \n"
"__blrow_gray8:            \n"
  "movd mm1,[rsi]          \n"
  "movd mm2,[rsi+4]        \n"
  "punpcklbw mm1,mm0       \n"
  "punpcklbw mm2,mm0       \n"
   sub_128_mmx_reg(mm1,mm2)
  "movq [rdi]  ,mm1        \n"
  "movq [rdi+8],mm2        \n"
  "add rsi,rax             \n"
  "add rdi,16              \n"
  "dec rcx                 \n"
  "jnz __blrow_gray8       \n"
  ".att_syntax             \n"
  : /* no output */
  : "D"(y),"S"(g),"a"(width)
  : "memory","rcx","mm0","mm1","mm2"
#else
  "mov  ecx,8              \n"
  "pxor mm0,mm0            \n"
"__blrow_gray8:            \n"
  "movd mm1,[esi]          \n"
  "movd mm2,[esi+4]        \n"
  "punpcklbw mm1,mm0       \n"
  "punpcklbw mm2,mm0       \n"
   sub_128_mmx_reg(mm1,mm2)
  "movq [edi]  ,mm1        \n"
  "movq [edi+8],mm2        \n"
  "add esi,eax             \n"
  "add edi,16              \n"
  "dec ecx                 \n"
  "jnz __blrow_gray8       \n"
  ".att_syntax             \n"
  : /* no output */
  : "D"(y),"S"(g),"a"(width)
  : "memory","ecx","mm0","mm1","mm2"
#endif
  );

#endif

}


// Convert 16x16 RGB24 pixel map to (4xY 1xCb 1xCr) block
void conv_block_RGB24H2V2_mmx(long width,unsigned char *rgb,short *y,short *cb,short *cr)
{

  long   i,j,y0,pitch;
  short *yB;
  // ! Due to wrong gcc stack code (does not detect push), yB is cleared during asm !
  
  pitch = 6*width-48;

  for(j=0;j<8;j++) {
    y0 = (((j&4)<<5) + ((j&3)<<4));
    for(i=0;i<4;i++) {
      yB = y + (y0 + (((i&2)<<5) + ((i&1)<<2)));

#ifdef _WINDOWS

      // Visual C++ inline assembly
      _asm {

        mov  esi,rgb
        mov  edi,yB
        mov  eax,width
        mov  ebx,cb
        mov  edx,cr

        mov  ecx,eax
        shl  eax,1
        add  eax,ecx
        pxor mm0,mm0

        // Y 1st row

        movd mm1,[esi]      // [0000]xxB0G0R0
        movd mm3,[esi+6]    // [0000]xxB2G2R2
        movd mm2,[esi+3]    // [0000]xxB1G1R1
        movd mm4,[esi+8]    // [0000]B3G3R3xx
        psrlq mm4,8         // [0000]xxB3G3R3
        punpcklbw mm1,mm0   // xxB0G0R0
        punpcklbw mm2,mm0   // xxB1G1R1
        punpcklbw mm3,mm0   // xxB2G2R2
        punpcklbw mm4,mm0   // xxB3G3R3
        movq      mm6,mm3
        movq      mm7,mm1
        punpcklwd mm1,mm2   // G1G0R1R0
        punpcklwd mm3,mm4   // G3G2R3R2
        movq      mm5,mm1
        punpckldq mm1,mm3   // R3R2R1R0
        punpckhdq mm5,mm3   // G3G2G1G0
        punpckhwd mm7,mm2   // xxxxB1B0
        punpckhwd mm6,mm4   // xxxxB3B2
        punpckldq mm7,mm6   // B3B2B1B0
        psllw   mm1,1
        psllw   mm5,1
        psllw   mm7,1
        pmulhw mm1,_rymm
        pmulhw mm5,_gymm
        pmulhw mm7,_bymm
        paddw mm1,mm5
        paddw mm1,mm7      // Y3Y2Y1Y0
        paddw mm1,_offymm
        movq [edi],mm1

        // 2nd row

        movd mm1,[esi+eax]    // [0000]xxB0G0R0
        movd mm3,[esi+eax+6]  // [0000]xxB2G2R2
        movd mm2,[esi+eax+3]  // [0000]xxB1G1R1
        movd mm4,[esi+eax+8]  // [0000]B3G3R3xx
        psrlq mm4,8         // [0000]xxB3G3R3
        punpcklbw mm1,mm0   // xxB0G0R0
        punpcklbw mm2,mm0   // xxB1G1R1
        punpcklbw mm3,mm0   // xxB2G2R2
        punpcklbw mm4,mm0   // xxB3G3R3
        movq      mm6,mm3
        movq      mm7,mm1
        punpcklwd mm1,mm2   // G1G0R1R0
        punpcklwd mm3,mm4   // G3G2R3R2
        movq      mm5,mm1
        punpckldq mm1,mm3   // R3R2R1R0
        punpckhdq mm5,mm3   // G3G2G1G0
        punpckhwd mm7,mm2   // xxxxB1B0
        punpckhwd mm6,mm4   // xxxxB3B2
        punpckldq mm7,mm6   // B3B2B1B0
        psllw   mm1,1
        psllw   mm5,1
        psllw   mm7,1
        pmulhw mm1,_rymm
        pmulhw mm5,_gymm
        pmulhw mm7,_bymm
        paddw mm1,mm5
        paddw mm1,mm7       // Y3Y2Y1Y0
        paddw mm1,_offymm
        movq [edi+16],mm1

        // CbCr (2x downsampling)

        movd mm1,[esi]        // [0000]xxB00G00R00
        movd mm3,[esi+eax]    // [0000]xxB01G01R01
        movd mm2,[esi+3]      // [0000]xxB10G10R10
        movd mm4,[esi+eax+3]  // [0000]xxB11G11R11

        punpcklbw mm1,mm0
        punpcklbw mm2,mm0
        punpcklbw mm3,mm0
        punpcklbw mm4,mm0

        paddw mm1,mm2         // xx[B00+B10][G00+G10][R00+R10]
        paddw mm3,mm4         // xx[B01+B11][G01+G11][R01+R11]
        paddw mm1,mm3
        psrlw mm1,1           // xx[B0][G0][R0]

        movd mm2,[esi+6]      // [0000]xxB00G00R00
        movd mm4,[esi+eax+6]  // [0000]B01G01R01xx
        movd mm3,[esi+8]      // [0000]xxB10G10R10
        movd mm5,[esi+eax+8]  // [0000]B11G11R11xx
        psrlq mm3,8           // [0000]xxB01G01R01
        psrlq mm5,8           // [0000]xxB11G11R11

        punpcklbw mm2,mm0
        punpcklbw mm3,mm0
        punpcklbw mm4,mm0
        punpcklbw mm5,mm0

        paddw mm2,mm3       // xx[B00+B10][G00+G10][R00+R10]
        paddw mm4,mm5       // xx[B01+B11][G01+G11][R01+R11]
        paddw mm2,mm4
        psrlw mm2,1         // xx[B1][G1][R1]

        movq      mm7,mm1

        punpcklwd mm1,mm2   // G1G0R1R0
        movq      mm5,mm1
        punpckldq mm1,mm1   // R1R0R1R0
        punpckhdq mm5,mm5   // G1G0G1G0
        punpckhwd mm7,mm2   // xxxxB1B0
        punpckldq mm7,mm7   // B1B0B1B0

        pmulhw mm1,_rcbcrmm
        pmulhw mm5,_gcbcrmm
        pmulhw mm7,_bcbcrmm

        paddw mm1,mm5
        paddw mm1,mm7      // cb1cb0cr1cr0
        paddw mm1,_rcmm

        movd [ebx],mm1
        psrlq mm1,32
        movd [edx],mm1

      } // end asm

#else

      // GCC inline assembly code
      __asm__ (
      ".intel_syntax noprefix  \n"
#ifdef _64BITS
      "push rbx                \n"      
      "mov  rbx,rcx            \n"
      "mov  rcx,rax            \n"
      "shl  rax,1              \n"
      "add  rax,rcx            \n"
      "pxor mm0,mm0            \n"
      "movd mm1,[rsi]          \n"
      "movd mm3,[rsi+6]        \n"
      "movd mm2,[rsi+3]        \n"
      "movd mm4,[rsi+8]        \n"
#else
      "push ebx                \n"      
      "mov  ebx,ecx            \n"
      "mov  ecx,eax            \n"
      "shl  eax,1              \n"
      "add  eax,ecx            \n"
      "pxor mm0,mm0            \n"
      "movd mm1,[esi]          \n"
      "movd mm3,[esi+6]        \n"
      "movd mm2,[esi+3]        \n"
      "movd mm4,[esi+8]        \n"
#endif      
      "psrlq mm4,8             \n"
      "punpcklbw mm1,mm0       \n"
      "punpcklbw mm2,mm0       \n"
      "punpcklbw mm3,mm0       \n"
      "punpcklbw mm4,mm0       \n"
      "movq      mm6,mm3       \n"
      "movq      mm7,mm1       \n"
      "punpcklwd mm1,mm2       \n"
      "punpcklwd mm3,mm4       \n"
      "movq      mm5,mm1       \n"
      "punpckldq mm1,mm3       \n"
      "punpckhdq mm5,mm3       \n"
      "punpckhwd mm7,mm2       \n"
      "punpckhwd mm6,mm4       \n"
      "punpckldq mm7,mm6       \n"
      "psllw   mm1,1           \n"
      "psllw   mm5,1           \n"
      "psllw   mm7,1           \n"
	   mul_rgb_mmx_reg(mm1,mm5,mm7)
      "paddw mm1,mm5           \n"
      "paddw mm1,mm7           \n"
	   add_127_mmx_reg(mm1)
#ifdef _64BITS
      "movq [rdi],mm1          \n"
      "movd mm1,[rsi+rax]      \n"
      "movd mm3,[rsi+rax+6]    \n"
      "movd mm2,[rsi+rax+3]    \n"
      "movd mm4,[rsi+rax+8]    \n"
#else
      "movq [edi],mm1          \n"
      "movd mm1,[esi+eax]      \n"
      "movd mm3,[esi+eax+6]    \n"
      "movd mm2,[esi+eax+3]    \n"
      "movd mm4,[esi+eax+8]    \n"
#endif      
      "psrlq mm4,8             \n"
      "punpcklbw mm1,mm0       \n"
      "punpcklbw mm2,mm0       \n"
      "punpcklbw mm3,mm0       \n"
      "punpcklbw mm4,mm0       \n"
      "movq      mm6,mm3       \n"
      "movq      mm7,mm1       \n"
      "punpcklwd mm1,mm2       \n"
      "punpcklwd mm3,mm4       \n"
      "movq      mm5,mm1       \n"
      "punpckldq mm1,mm3       \n"
      "punpckhdq mm5,mm3       \n"
      "punpckhwd mm7,mm2       \n"
      "punpckhwd mm6,mm4       \n"
      "punpckldq mm7,mm6       \n"
      "psllw   mm1,1           \n"
      "psllw   mm5,1           \n"
      "psllw   mm7,1           \n"
       mul_rgb_mmx_reg(mm1,mm5,mm7)
      "paddw mm1,mm5           \n"
      "paddw mm1,mm7           \n"
	   add_127_mmx_reg(mm1)
#ifdef _64BITS
      "movq [rdi+16],mm1       \n"
      "movd mm1,[rsi]          \n"
      "movd mm3,[rsi+rax]      \n"
      "movd mm2,[rsi+3]        \n"
      "movd mm4,[rsi+rax+3]    \n"
#else
      "movq [edi+16],mm1       \n"
      "movd mm1,[esi]          \n"
      "movd mm3,[esi+eax]      \n"
      "movd mm2,[esi+3]        \n"
      "movd mm4,[esi+eax+3]    \n"
#endif      
      "punpcklbw mm1,mm0       \n"
      "punpcklbw mm2,mm0       \n"
      "punpcklbw mm3,mm0       \n"
      "punpcklbw mm4,mm0       \n"
      "paddw mm1,mm2           \n"
      "paddw mm3,mm4           \n"
      "paddw mm1,mm3           \n"
      "psrlw mm1,1             \n"
#ifdef _64BITS
      "movd mm2,[rsi+6]        \n"
      "movd mm4,[rsi+rax+6]    \n"
      "movd mm3,[rsi+8]        \n"
      "movd mm5,[rsi+rax+8]    \n"
#else
      "movd mm2,[esi+6]        \n"
      "movd mm4,[esi+eax+6]    \n"
      "movd mm3,[esi+8]        \n"
      "movd mm5,[esi+eax+8]    \n"
#endif      
      "psrlq mm3,8             \n"
      "psrlq mm5,8             \n"
      "punpcklbw mm2,mm0       \n"
      "punpcklbw mm3,mm0       \n"
      "punpcklbw mm4,mm0       \n"
      "punpcklbw mm5,mm0       \n"
      "paddw mm2,mm3           \n"
      "paddw mm4,mm5           \n"
      "paddw mm2,mm4           \n"
      "psrlw mm2,1             \n"
      "movq      mm7,mm1       \n"
      "punpcklwd mm1,mm2       \n"
      "movq      mm5,mm1       \n"
      "punpckldq mm1,mm1       \n"
      "punpckhdq mm5,mm5       \n"
      "punpckhwd mm7,mm2       \n"
      "punpckldq mm7,mm7       \n"
	   mul_rgbc_mmx_reg(mm1,mm5,mm7)
      "paddw mm1,mm5           \n"
      "paddw mm1,mm7           \n"
	   add_1_mmx_reg(mm1)
#ifdef _64BITS
      "movd [rbx],mm1          \n"
      "psrlq mm1,32            \n"
      "movd [rdx],mm1          \n"
      "pop rbx                 \n"      
#else
      "movd [ebx],mm1          \n"
      "psrlq mm1,32            \n"
      "movd [edx],mm1          \n"
      "pop ebx                 \n"      
#endif      
      ".att_syntax             \n"
      : /* no output */
      : "D"(yB),"S"(rgb),"a"(width),"c"(cb),"d"(cr)
      : "memory","mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7"
      );

#endif

      cb+=2;
      cr+=2;
      rgb+=12;
    }

    rgb+=pitch;
  }

}

// Convert 16x16 RGB32 pixel map to (4xY 1xCb 1xCr) block
void conv_block_RGB32H2V2_mmx(long width,unsigned char *rgb,short *y,short *cb,short *cr)
{

  long   i,j,y0,pitch;
  short *yB;
  // ! Due to wrong gcc stack code (does not detect push), yB is cleared during asm !

  pitch = 8*width-64;

  for(j=0;j<8;j++) {
    y0 = (((j&4)<<5) + ((j&3)<<4));
    for(i=0;i<4;i++) {
      yB = y + (y0 + (((i&2)<<5) + ((i&1)<<2)));

#ifdef _WINDOWS

      // Visual C++ inline assembly
      _asm {

        mov  esi,rgb
        mov  edi,yB
        mov  eax,width
        mov  ebx,cb
        mov  edx,cr

        pxor mm0,mm0
        shl  eax,2

        // Y 1st row

        movd mm1,[esi]      // [0000]xxB0G0R0
        movd mm3,[esi+8]    // [0000]xxB2G2R2
        movd mm2,[esi+4]    // [0000]xxB1G1R1
        movd mm4,[esi+12]   // [0000]xxB3G3R3
        punpcklbw mm1,mm0   // xxB0G0R0
        punpcklbw mm2,mm0   // xxB1G1R1
        punpcklbw mm3,mm0   // xxB2G2R2
        punpcklbw mm4,mm0   // xxB3G3R3
        movq      mm6,mm3
        movq      mm7,mm1
        punpcklwd mm1,mm2   // G1G0R1R0
        punpcklwd mm3,mm4   // G3G2R3R2
        movq      mm5,mm1
        punpckldq mm1,mm3   // R3R2R1R0
        punpckhdq mm5,mm3   // G3G2G1G0
        punpckhwd mm7,mm2   // xxxxB1B0
        punpckhwd mm6,mm4   // xxxxB3B2
        punpckldq mm7,mm6   // B3B2B1B0
        psllw   mm1,1
        psllw   mm5,1
        psllw   mm7,1
        pmulhw mm1,_rymm
        pmulhw mm5,_gymm
        pmulhw mm7,_bymm
        paddw mm1,mm5
        paddw mm1,mm7      // Y3Y2Y1Y0
        paddw mm1,_offymm
        movq [edi],mm1

        // 2nd row

        movd mm1,[esi+eax]    // [0000]xxB0G0R0
        movd mm3,[esi+eax+8]  // [0000]xxB2G2R2
        movd mm2,[esi+eax+4]  // [0000]xxB1G1R1
        movd mm4,[esi+eax+12] // [0000]xxB3G3R3
        punpcklbw mm1,mm0   // xxB0G0R0
        punpcklbw mm2,mm0   // xxB1G1R1
        punpcklbw mm3,mm0   // xxB2G2R2
        punpcklbw mm4,mm0   // xxB3G3R3
        movq      mm6,mm3
        movq      mm7,mm1
        punpcklwd mm1,mm2   // G1G0R1R0
        punpcklwd mm3,mm4   // G3G2R3R2
        movq      mm5,mm1
        punpckldq mm1,mm3   // R3R2R1R0
        punpckhdq mm5,mm3   // G3G2G1G0
        punpckhwd mm7,mm2   // xxxxB1B0
        punpckhwd mm6,mm4   // xxxxB3B2
        punpckldq mm7,mm6   // B3B2B1B0
        psllw   mm1,1
        psllw   mm5,1
        psllw   mm7,1
        pmulhw mm1,_rymm
        pmulhw mm5,_gymm
        pmulhw mm7,_bymm
        paddw mm1,mm5
        paddw mm1,mm7       // Y3Y2Y1Y0
        paddw mm1,_offymm
        movq [edi+16],mm1

        // CbCr (2x downsampling)

        movd mm1,[esi]        // [0000]xxB00G00R00
        movd mm3,[esi+eax]    // [0000]xxB01G01R01
        movd mm2,[esi+4]      // [0000]xxB10G10R10
        movd mm4,[esi+eax+4]  // [0000]xxB11G11R11

        punpcklbw mm1,mm0
        punpcklbw mm2,mm0
        punpcklbw mm3,mm0
        punpcklbw mm4,mm0

        paddw mm1,mm2         // xx[B00+B10][G00+G10][R00+R10]
        paddw mm3,mm4         // xx[B01+B11][G01+G11][R01+R11]
        paddw mm1,mm3
        psrlw mm1,1           // xx[B0][G0][R0]

        movd mm2,[esi+8]      // [0000]xxB00G00R00
        movd mm4,[esi+eax+8]  // [0000]B01G01R01xx
        movd mm3,[esi+12]     // [0000]xxB10G10R10
        movd mm5,[esi+eax+12] // [0000]xxB11G11R11

        punpcklbw mm2,mm0
        punpcklbw mm3,mm0
        punpcklbw mm4,mm0
        punpcklbw mm5,mm0

        paddw mm2,mm3       // xx[B00+B10][G00+G10][R00+R10]
        paddw mm4,mm5       // xx[B01+B11][G01+G11][R01+R11]
        paddw mm2,mm4
        psrlw mm2,1         // xx[B1][G1][R1]

        movq      mm7,mm1

        punpcklwd mm1,mm2   // G1G0R1R0
        movq      mm5,mm1
        punpckldq mm1,mm1   // R1R0R1R0
        punpckhdq mm5,mm5   // G1G0G1G0
        punpckhwd mm7,mm2   // xxxxB1B0
        punpckldq mm7,mm7   // B1B0B1B0

        pmulhw mm1,_rcbcrmm
        pmulhw mm5,_gcbcrmm
        pmulhw mm7,_bcbcrmm

        paddw mm1,mm5
        paddw mm1,mm7      // cb1cb0cr1cr0
        paddw mm1,_rcmm

        movd [ebx],mm1
        psrlq mm1,32
        movd [edx],mm1

      } // end asm

#else

      // GCC inline assembly code
      __asm__ (
      ".intel_syntax noprefix  \n"
#ifdef _64BITS
      "push rbx                \n"      
      "mov rbx,rcx             \n"
      "pxor mm0,mm0            \n"
      "shl  rax,2              \n"
      "movd mm1,[rsi]          \n"
      "movd mm3,[rsi+8]        \n"
      "movd mm2,[rsi+4]        \n"
      "movd mm4,[rsi+12]       \n"
#else
      "push ebx                \n"      
      "mov ebx,ecx             \n"
      "pxor mm0,mm0            \n"
      "shl  eax,2              \n"
      "movd mm1,[esi]          \n"
      "movd mm3,[esi+8]        \n"
      "movd mm2,[esi+4]        \n"
      "movd mm4,[esi+12]       \n"
#endif
      "punpcklbw mm1,mm0       \n"
      "punpcklbw mm2,mm0       \n"
      "punpcklbw mm3,mm0       \n"
      "punpcklbw mm4,mm0       \n"
      "movq      mm6,mm3       \n"
      "movq      mm7,mm1       \n"
      "punpcklwd mm1,mm2       \n"
      "punpcklwd mm3,mm4       \n"
      "movq      mm5,mm1       \n"
      "punpckldq mm1,mm3       \n"
      "punpckhdq mm5,mm3       \n"
      "punpckhwd mm7,mm2       \n"
      "punpckhwd mm6,mm4       \n"
      "punpckldq mm7,mm6       \n"
      "psllw   mm1,1           \n"
      "psllw   mm5,1           \n"
      "psllw   mm7,1           \n"
	   mul_rgb_mmx_reg(mm1,mm5,mm7)
      "paddw mm1,mm5           \n"
      "paddw mm1,mm7           \n"
	   add_127_mmx_reg(mm1)
#ifdef _64BITS
      "movq [rdi],mm1          \n"
      "movd mm1,[rsi+rax]      \n"
      "movd mm3,[rsi+rax+8]    \n"
      "movd mm2,[rsi+rax+4]    \n"
      "movd mm4,[rsi+rax+12]   \n"
#else
      "movq [edi],mm1          \n"
      "movd mm1,[esi+eax]      \n"
      "movd mm3,[esi+eax+8]    \n"
      "movd mm2,[esi+eax+4]    \n"
      "movd mm4,[esi+eax+12]   \n"
#endif
      "punpcklbw mm1,mm0       \n"
      "punpcklbw mm2,mm0       \n"
      "punpcklbw mm3,mm0       \n"
      "punpcklbw mm4,mm0       \n"
      "movq      mm6,mm3       \n"
      "movq      mm7,mm1       \n"
      "punpcklwd mm1,mm2       \n"
      "punpcklwd mm3,mm4       \n"
      "movq      mm5,mm1       \n"
      "punpckldq mm1,mm3       \n"
      "punpckhdq mm5,mm3       \n"
      "punpckhwd mm7,mm2       \n"
      "punpckhwd mm6,mm4       \n"
      "punpckldq mm7,mm6       \n"
      "psllw   mm1,1           \n"
      "psllw   mm5,1           \n"
      "psllw   mm7,1           \n"
	   mul_rgb_mmx_reg(mm1,mm5,mm7)
      "paddw mm1,mm5           \n"
      "paddw mm1,mm7           \n"
	   add_127_mmx_reg(mm1)
#ifdef _64BITS
      "movq [rdi+16],mm1       \n"
      "movd mm1,[rsi]          \n"
      "movd mm3,[rsi+rax]      \n"
      "movd mm2,[rsi+4]        \n"
      "movd mm4,[rsi+rax+4]    \n"
#else
      "movq [edi+16],mm1       \n"
      "movd mm1,[esi]          \n"
      "movd mm3,[esi+eax]      \n"
      "movd mm2,[esi+4]        \n"
      "movd mm4,[esi+eax+4]    \n"
#endif
      "punpcklbw mm1,mm0       \n"
      "punpcklbw mm2,mm0       \n"
      "punpcklbw mm3,mm0       \n"
      "punpcklbw mm4,mm0       \n"
      "paddw mm1,mm2           \n"
      "paddw mm3,mm4           \n"
      "paddw mm1,mm3           \n"
      "psrlw mm1,1             \n"
#ifdef _64BITS
      "movd mm2,[rsi+8]        \n"
      "movd mm4,[rsi+rax+8]    \n"
      "movd mm3,[rsi+12]       \n"
      "movd mm5,[rsi+rax+12]   \n"
#else
      "movd mm2,[esi+8]        \n"
      "movd mm4,[esi+eax+8]    \n"
      "movd mm3,[esi+12]       \n"
      "movd mm5,[esi+eax+12]   \n"
#endif
      "punpcklbw mm2,mm0       \n"
      "punpcklbw mm3,mm0       \n"
      "punpcklbw mm4,mm0       \n"
      "punpcklbw mm5,mm0       \n"
      "paddw mm2,mm3           \n"
      "paddw mm4,mm5           \n"
      "paddw mm2,mm4           \n"
      "psrlw mm2,1             \n"
      "movq      mm7,mm1       \n"
      "punpcklwd mm1,mm2       \n"
      "movq      mm5,mm1       \n"
      "punpckldq mm1,mm1       \n"
      "punpckhdq mm5,mm5       \n"
      "punpckhwd mm7,mm2       \n"
      "punpckldq mm7,mm7       \n"
	   mul_rgbc_mmx_reg(mm1,mm5,mm7)
      "paddw mm1,mm5           \n"
      "paddw mm1,mm7           \n"
	   add_1_mmx_reg(mm1)
#ifdef _64BITS
      "movd [rbx],mm1          \n"
      "psrlq mm1,32            \n"
      "movd [rdx],mm1          \n"
      "pop rbx                 \n"      
#else
      "movd [ebx],mm1          \n"
      "psrlq mm1,32            \n"
      "movd [edx],mm1          \n"
      "pop ebx                 \n"      
#endif
      ".att_syntax             \n"
      : /* no output */
      : "D"(yB),"S"(rgb),"a"(width),"c"(cb),"d"(cr)
      : "memory","mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7"
      );

#endif

      cb+=2;
      cr+=2;
      rgb+=16;
    }
    rgb+=pitch;
  }

}

#endif /* JPG_USE_ASM */
