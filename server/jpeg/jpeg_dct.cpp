///=============================================================================	
//
// file :		jpeg_dct.cpp
//
// description :        Simple jpeg coding/decoding library
//                      Discrete Cosine Transform (8x8)
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

// C implementation comes from JPEG library by Thomas G. Lane

#include "jpeg_lib.h"

#define SHIFT2(x)        (((x) +     2) >> 2 )
#define SHIFT11(x)       (((x) +  2048) >> 11)
#define SHIFT15(x)       (((x) + 32768) >> 15)
#define FIXPOINT         13
#define _1               ((long)1)
#define SCALE            (_1 << FIXPOINT)
#define CLIP(x)          if (x & 0xFF00) x = (((~x) >> 15) & 0xFF)
#define ROUND(x,n)       (((x) + (_1 << ((n)-1))) >> (n))
#define FIX_0_298631336  ((long)  2446)        /* 0.298631336 -c1+c3+c5-c7 */
#define FIX_0_390180644  ((long)  3196)        /* 0.390180644 c5-c3 (neg)  */
#define FIX_0_541196100  ((long)  4433)        /* 0.541196100 c6           */
#define FIX_0_765366865  ((long)  6270)        /* 0.765366865 c2-c6        */ 
#define FIX_0_899976223  ((long)  7373)        /* 0.899976223 c7-c3 (neg)  */
#define FIX_1_175875602  ((long)  9633)        /* 1.175875602 c3           */
#define FIX_1_501321110  ((long)  12299)       /* 1.501321110 c1+c3-c5-c7  */
#define FIX_1_847759065  ((long)  15137)       /* 1.847759065 -c2-c6 (neg) */
#define FIX_1_961570560  ((long)  16069)       /* 1.961570560 -c5-c3 (neg) */
#define FIX_2_053119869  ((long)  16819)       /* 2.053119869 c1+c3-c5+c7  */
#define FIX_2_562915447  ((long)  20995)       /* 2.562915447 -c1-c3 (neg) */
#define FIX_3_072711026  ((long)  25172)       /* 3.072711026 c1+c3+c5-c7  */


void jpeg_fdct( short *block )
{

  long tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  long tmp10, tmp11, tmp12, tmp13;
  long z1, z2, z3, z4, z5;
  short *dataptr;
  int ctr;

  // Process rows
  dataptr = block;
  for (ctr = 7; ctr >= 0; ctr--) {

    tmp0 = (long)dataptr[0] + (long)dataptr[7];
    tmp7 = (long)dataptr[0] - (long)dataptr[7];
    tmp1 = (long)dataptr[1] + (long)dataptr[6];
    tmp6 = (long)dataptr[1] - (long)dataptr[6];
    tmp2 = (long)dataptr[2] + (long)dataptr[5];
    tmp5 = (long)dataptr[2] - (long)dataptr[5];
    tmp3 = (long)dataptr[3] + (long)dataptr[4];
    tmp4 = (long)dataptr[3] - (long)dataptr[4];
    
    tmp10 = tmp0 + tmp3;
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;
    
    dataptr[0] = (short) ((tmp10 + tmp11) << 2);
    dataptr[4] = (short) ((tmp10 - tmp11) << 2);
    
    z1 = (tmp12 + tmp13)*FIX_0_541196100;
    dataptr[2] = (short) SHIFT11(z1 + tmp13*FIX_0_765366865);
    dataptr[6] = (short) SHIFT11(z1 - tmp12*FIX_1_847759065);
    
    z1 = tmp4 + tmp7;
    z2 = tmp5 + tmp6;
    z3 = tmp4 + tmp6;
    z4 = tmp5 + tmp7;
    z5 = (z3 + z4)*FIX_1_175875602;
    
    tmp4 *= FIX_0_298631336;
    tmp5 *= FIX_2_053119869;
    tmp6 *= FIX_3_072711026;
    tmp7 *= FIX_1_501321110;
    z1 *= - FIX_0_899976223;
    z2 *= - FIX_2_562915447;
    z3 *= - FIX_1_961570560;
    z4 *= - FIX_0_390180644;    
    z3 += z5;
    z4 += z5;
    
    dataptr[7] = (short) SHIFT11(tmp4 + z1 + z3);
    dataptr[5] = (short) SHIFT11(tmp5 + z2 + z4);
    dataptr[3] = (short) SHIFT11(tmp6 + z2 + z3);
    dataptr[1] = (short) SHIFT11(tmp7 + z1 + z4);
    
    dataptr += 8;
  }

  // Process columns
  dataptr = block;
  for (ctr = 7; ctr >= 0; ctr--) {

    tmp0 = (long)dataptr[8*0] + (long)dataptr[8*7];
    tmp7 = (long)dataptr[8*0] - (long)dataptr[8*7];
    tmp1 = (long)dataptr[8*1] + (long)dataptr[8*6];
    tmp6 = (long)dataptr[8*1] - (long)dataptr[8*6];
    tmp2 = (long)dataptr[8*2] + (long)dataptr[8*5];
    tmp5 = (long)dataptr[8*2] - (long)dataptr[8*5];
    tmp3 = (long)dataptr[8*3] + (long)dataptr[8*4];
    tmp4 = (long)dataptr[8*3] - (long)dataptr[8*4];
    
    tmp10 = tmp0 + tmp3;
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;
    
    dataptr[8*0] = (short) SHIFT2(tmp10 + tmp11);
    dataptr[8*4] = (short) SHIFT2(tmp10 - tmp11);
    
    z1 = (tmp12 + tmp13)*FIX_0_541196100;
    dataptr[8*2] = (short) SHIFT15(z1 + tmp13*FIX_0_765366865);
    dataptr[8*6] = (short) SHIFT15(z1 - tmp12*FIX_1_847759065);
        
    z1 = tmp4 + tmp7;
    z2 = tmp5 + tmp6;
    z3 = tmp4 + tmp6;
    z4 = tmp5 + tmp7;
    z5 = (z3 + z4)*FIX_1_175875602;
    
    tmp4 *= FIX_0_298631336;
    tmp5 *= FIX_2_053119869;
    tmp6 *= FIX_3_072711026;
    tmp7 *= FIX_1_501321110;
    z1 *= -FIX_0_899976223;
    z2 *= -FIX_2_562915447;
    z3 *= -FIX_1_961570560;
    z4 *= -FIX_0_390180644;
    
    z3 += z5;
    z4 += z5;
    
    dataptr[8*7] = (short) SHIFT15(tmp4 + z1 + z3);
    dataptr[8*5] = (short) SHIFT15(tmp5 + z2 + z4);
    dataptr[8*3] = (short) SHIFT15(tmp6 + z2 + z3);
    dataptr[8*1] = (short) SHIFT15(tmp7 + z1 + z4);
    
    dataptr++;
  }

}

// -------------------------------------------------------------

void jpeg_idct(short *block, unsigned char *dest)
{

  short innerBuff[64];
  long  tmp0, tmp1, tmp2, tmp3;
  long  tmp10, tmp11, tmp12, tmp13;
  long  z1, z2, z3, z4, z5;
  short *in,*inner;
  int   i;
  short v;

  // Rows
  in = block;
  inner = innerBuff;
  for (i=0;i<8;i++) {

    if ((in[1] | in[2] | in[3] | in[4] |
         in[5] | in[6] | in[7]) == 0)
    {
      short dc = in[0] << 3;
      inner[0] = dc;
      inner[1] = dc;
      inner[2] = dc;
      inner[3] = dc;
      inner[4] = dc;
      inner[5] = dc;
      inner[6] = dc;
      inner[7] = dc;
      in += 8;
      inner += 8;
      continue;
    }

    z2 = (long) in[2];
    z3 = (long) in[6];

    z1 = (z2 + z3)*FIX_0_541196100;
    tmp2 = z1 + z3*(- FIX_1_847759065);
    tmp3 = z1 + z2*FIX_0_765366865;

    tmp0 = ((long) in[0] + (long) in[4]) << FIXPOINT;
    tmp1 = ((long) in[0] - (long) in[4]) << FIXPOINT;

    tmp10 = tmp0 + tmp3;
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;

    tmp0 = (long) in[7];
    tmp1 = (long) in[5];
    tmp2 = (long) in[3];
    tmp3 = (long) in[1];

    z1 = tmp0 + tmp3;
    z2 = tmp1 + tmp2;
    z3 = tmp0 + tmp2;
    z4 = tmp1 + tmp3;
    z5 = (z3 + z4)*FIX_1_175875602;

    tmp0 = tmp0 * FIX_0_298631336;
    tmp1 = tmp1 * FIX_2_053119869;
    tmp2 = tmp2 * FIX_3_072711026;
    tmp3 = tmp3 * FIX_1_501321110;
    z1 = z1 * (-FIX_0_899976223);
    z2 = z2 * (-FIX_2_562915447);
    z3 = z3 * (-FIX_1_961570560);
    z4 = z4 * (-FIX_0_390180644);

    z3 += z5;
    z4 += z5;

    tmp0 += z1 + z3;
    tmp1 += z2 + z4;
    tmp2 += z2 + z3;
    tmp3 += z1 + z4;

    inner[0] = (short) ROUND(tmp10 + tmp3, FIXPOINT-3);
    inner[1] = (short) ROUND(tmp11 + tmp2, FIXPOINT-3);
    inner[2] = (short) ROUND(tmp12 + tmp1, FIXPOINT-3);
    inner[3] = (short) ROUND(tmp13 + tmp0, FIXPOINT-3);
    inner[4] = (short) ROUND(tmp13 - tmp0, FIXPOINT-3);
    inner[5] = (short) ROUND(tmp12 - tmp1, FIXPOINT-3);
    inner[6] = (short) ROUND(tmp11 - tmp2, FIXPOINT-3);
    inner[7] = (short) ROUND(tmp10 - tmp3, FIXPOINT-3);

    in += 8;
    inner += 8;
  }

  // Columns
  in = innerBuff;
  for(i=0;i<8;i++)
  {
    if ((in[8*1] | in[8*2] | in[8*3] | in[8*4]| 
         in[8*5] | in[8*6] | in[8*7]) == 0)
    {
      short dc = (short) ROUND((long)in[0], 6) + 128;
      CLIP(dc);
      dest[8*0] = (unsigned char)dc;
      dest[8*1] = (unsigned char)dc;
      dest[8*2] = (unsigned char)dc;
      dest[8*3] = (unsigned char)dc;
      dest[8*4] = (unsigned char)dc;
      dest[8*5] = (unsigned char)dc;
      dest[8*6] = (unsigned char)dc;
      dest[8*7] = (unsigned char)dc;
      in++;
      dest++;
      continue;
    }

    z2 = (long) in[8*2];
    z3 = (long) in[8*6];

    z1 = (z2 + z3)*FIX_0_541196100;
    tmp2 = z1 + z3 * (-FIX_1_847759065);
    tmp3 = z1 + z2 * FIX_0_765366865;

    tmp0 = ((long) in[8*0] + (long) in[8*4]) << FIXPOINT;
    tmp1 = ((long) in[8*0] - (long) in[8*4]) << FIXPOINT;

    tmp10 = tmp0 + tmp3;
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;

    tmp0 = (long) in[8*7];
    tmp1 = (long) in[8*5];
    tmp2 = (long) in[8*3];
    tmp3 = (long) in[8*1];

    z1 = tmp0 + tmp3;
    z2 = tmp1 + tmp2;
    z3 = tmp0 + tmp2;
    z4 = tmp1 + tmp3;
    z5 = (z3 + z4)*FIX_1_175875602;

    tmp0 = tmp0*FIX_0_298631336;
    tmp1 = tmp1*FIX_2_053119869;
    tmp2 = tmp2*FIX_3_072711026;
    tmp3 = tmp3*FIX_1_501321110;
    z1 = z1 * (-FIX_0_899976223);
    z2 = z2 * (-FIX_2_562915447);
    z3 = z3 * (-FIX_1_961570560);
    z4 = z4 * (-FIX_0_390180644);

    z3 += z5;
    z4 += z5;

    tmp0 += z1 + z3;
    tmp1 += z2 + z4;
    tmp2 += z2 + z3;
    tmp3 += z1 + z4;

    v = (short) ROUND(tmp10 + tmp3, FIXPOINT+6) + 128;
    CLIP(v);dest[8*0] = (unsigned char)v;
    v = (short) ROUND(tmp11 + tmp2, FIXPOINT+6) + 128;
    CLIP(v);dest[8*1] = (unsigned char)v;
    v = (short) ROUND(tmp12 + tmp1, FIXPOINT+6) + 128;
    CLIP(v);dest[8*2] = (unsigned char)v;
    v = (short) ROUND(tmp13 + tmp0, FIXPOINT+6) + 128;
    CLIP(v);dest[8*3] = (unsigned char)v;
    v = (short) ROUND(tmp13 - tmp0, FIXPOINT+6) + 128;
    CLIP(v);dest[8*4] = (unsigned char)v;
    v = (short) ROUND(tmp12 - tmp1, FIXPOINT+6) + 128;
    CLIP(v);dest[8*5] = (unsigned char)v;
    v = (short) ROUND(tmp11 - tmp2, FIXPOINT+6) + 128;
    CLIP(v);dest[8*6] = (unsigned char)v;
    v = (short) ROUND(tmp10 - tmp3, FIXPOINT+6) + 128;
    CLIP(v);dest[8*7] = (unsigned char)v;

    dest++;
    in++;

  }

}

