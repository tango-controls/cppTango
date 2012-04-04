///=============================================================================
//
// file :		jpeg_color.cpp
//
// description :        Simple jpeg coding/decoding library
//                      Color space conversion
//
// project :		TANGO
//
// author(s) :		JL Pons
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
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
// Revision 1.3  2009/04/20 14:55:58  jlpons
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

#define TRUNC(i)  ((i) & 0xFFFFFF00)?(unsigned char)(((~(i)) >> 31) & 0xFF):(unsigned char)(i)

static long *ry = NULL;
static long *gy = NULL;
static long *by = NULL;
static long *rcb = NULL;
static long *gcb = NULL;
static long *bcb = NULL;
static long *rcr = NULL;
static long *gcr = NULL;
static long *bcr = NULL;
static long *crr = NULL;
static long *cbb = NULL;
static long *crg = NULL;
static long *cbg = NULL;

// ------------------------------------------------------------------------------

void jpeg_init_color() {

  // Conversion table to speed up RGB <-> YCbCr conversion (fixed point)
  if( !ry ) {

    ry = (long *)malloc(256 * sizeof(long));
    for(long i=0;i<256;i++) ry[i] = (long)(0.299*65536.0+0.5) * i;
    gy = (long *)malloc(256 * sizeof(long));
    for(long i=0;i<256;i++) gy[i] = (long)(0.587*65536.0+0.5) * i;
    by = (long *)malloc(256 * sizeof(long));
    for(long i=0;i<256;i++) by[i] = (long)(0.114*65536.0+0.5) * i + 32767L;

    rcb = (long *)malloc(256 * sizeof(long));
    for(long i=0;i<256;i++) rcb[i] = (long)(0.16874*65536.0+0.5) * (-i);
    gcb = (long *)malloc(256 * sizeof(long));
    for(long i=0;i<256;i++) gcb[i] = (long)(0.33126*65536.0+0.5) * (-i);
    bcb = (long *)malloc(256 * sizeof(long));
    for(long i=0;i<256;i++) bcb[i] = (long)(0.5*65536.0+0.5) * (i) + 32767L;

    rcr = (long *)malloc(256 * sizeof(long));
    for(long i=0;i<256;i++) rcr[i] = (long)(0.5*65536.0+0.5) * (i);
    gcr = (long *)malloc(256 * sizeof(long));
    for(long i=0;i<256;i++) gcr[i] = (long)(0.41869*65536.0+0.5) * (-i);
    bcr = (long *)malloc(256 * sizeof(long));
    for(long i=0;i<256;i++) bcr[i] = (long)(0.08131*65536.0+0.5) * (-i) + 32767L;

    crr = (long *)malloc(256 * sizeof(long));
    cbb = (long *)malloc(256 * sizeof(long));
    crg = (long *)malloc(256 * sizeof(long));
    cbg = (long *)malloc(256 * sizeof(long));
    for(long i=0;i<256;i++) {
      double k = (double)((i * 2) - 256);
      crr[i] = (long)( 1.402*32768.0*k + 32767.0 ) >> 16;
      cbb[i] = (long)( 1.772*32768.0*k + 32767.0 ) >> 16;
      crg[i] = (long)( -0.71414*32768.0*k );
      cbg[i] = (long)( -0.34414*32768.0*k + 32767.0 );
    }

  }

}


//------------------------------------------------------------------------------
// MCU16x16 YCbCr H2V2 (2x2:1:1, 6 blocks per MCU) to 32-bit RGB
//------------------------------------------------------------------------------

void jpeg_yh2v2_to_rgb32(unsigned char *block,int width,unsigned char *rgb) {

  unsigned char *y  = block;
  unsigned char *cb = block+64*4;
  unsigned char *cr = block+64*5;
  int pitch = width*4;
  unsigned char *row;
  unsigned char *yP;
  long y0,yy,rc,gc,bc;

  for(int j=0;j<8;j++) {
    row = rgb;
    y0 = ((j&4)<<5) + ((j&3)<<4);
    for(int i=0;i<8;i++) {

      yP = y + (y0 + ((i&4)<<4) + ((i&3)<<1));

      rc = crr[*cr];
      gc = ((crg[*cr] + cbg[*cb]) >> 16);
      bc = cbb[*cb];

      yy = yP[0];
      row[0] = TRUNC(yy+rc);
      row[1] = TRUNC(yy+gc);
      row[2] = TRUNC(yy+bc);

      yy = yP[1];
      row[4] = TRUNC(yy+rc);
      row[5] = TRUNC(yy+gc);
      row[6] = TRUNC(yy+bc);

      yy = yP[8];
      row[0+pitch] = TRUNC(yy+rc);
      row[1+pitch] = TRUNC(yy+gc);
      row[2+pitch] = TRUNC(yy+bc);

      yy = yP[9];
      row[4+pitch] = TRUNC(yy+rc);
      row[5+pitch] = TRUNC(yy+gc);
      row[6+pitch] = TRUNC(yy+bc);

      row+=8;
      cr++;
      cb++;
    }
    rgb+=2*pitch;
  }

}

//------------------------------------------------------------------------------
// MCU8x8 YCbCr H1V1 (1x1:1:1, 3 blocks per MCU) to 32-bit RGB
//------------------------------------------------------------------------------

void jpeg_yh1v1_to_rgb32(unsigned char *block,int width,unsigned char *rgb) {

  int pitch = width*4;
  unsigned char *row;
  unsigned char *ycc  = block;

  for (int i=0; i<8 ; i++)
  {
    row = rgb;
    for (int j=0; j<8; j++)
    {
      int y =  ycc[0];
      int cb = ycc[64];
      int cr = ycc[128];
      row[0] = TRUNC(y + crr[cr]);
      row[1] = TRUNC(y + ((crg[cr] + cbg[cb]) >> 16));
      row[2] = TRUNC(y + cbb[cb]);
      row += 4;
      ycc++;
    }
    rgb+=pitch;
  }

}

//------------------------------------------------------------------------------
// MCU8x8 Y (1 block per MCU) to 8-bit grayscale
//------------------------------------------------------------------------------

void jpeg_y_to_gray(unsigned char *block,int width,unsigned char *rgb)
{

  unsigned char *s = block;
  unsigned char *d = rgb;
  for (int i=0;i<8;i++)
  {
    *(unsigned long *)d = *(unsigned long *)s;
    *(unsigned long *)(&d[4]) = *(unsigned long *)(&s[4]);
    s += 8;
    d += width;
  }

}

#ifdef JPG_USE_ASM
extern void conv_block_RGB32H2V2_mmx(long width,unsigned char *rgb,short *y,short *cb,short *cr);
extern void conv_block_RGB24H2V2_mmx(long width,unsigned char *rgb,short *y,short *cb,short *cr);
extern void conv_block_GRAY8Y_mmx(long width,unsigned char *g,short *y);
#endif

// --------------------------------------------------------------------------------------
// Convert 16x16 RGB32 pixel map to (4xY 1xCb 1xCr) block (4:2:0)
// --------------------------------------------------------------------------------------

void conv_block_RGB32H2V2(int width,unsigned char *rgb,short *y,short *cb,short *cr)
{

  short r00,g00,b00,r10,g10,b10,r01,g01,b01,r11,g11,b11;
  int y0,yIdx;
  short rd,gd,bd;

  for(int j=0;j<8;j++) {
    y0 = ((j&4)<<5) + ((j&3)<<4);
    for(int i=0;i<8;i++) {

      yIdx = y0 + ((i&4)<<4) + ((i&3)<<1);
      r00 = (short)rgb[0];
      g00 = (short)rgb[1];
      b00 = (short)rgb[2];

      r10 = (short)rgb[4];
      g10 = (short)rgb[5];
      b10 = (short)rgb[6];

      r01 = (short)rgb[0+width*4];
      g01 = (short)rgb[1+width*4];
      b01 = (short)rgb[2+width*4];

      r11 = (short)rgb[4+width*4];
      g11 = (short)rgb[5+width*4];
      b11 = (short)rgb[6+width*4];

      y[0+yIdx] = (short)( (ry[r00] + gy[g00] + by[b00]) >> 16 ) - 128;
      y[1+yIdx] = (short)( (ry[r10] + gy[g10] + by[b10]) >> 16 ) - 128;
      y[8+yIdx] = (short)( (ry[r01] + gy[g01] + by[b01]) >> 16 ) - 128;
      y[9+yIdx] = (short)( (ry[r11] + gy[g11] + by[b11]) >> 16 ) - 128;

      rd = (r00 + r10 + r01 + r11) >> 2;
      gd = (g00 + g10 + g01 + g11) >> 2;
      bd = (b00 + b10 + b01 + b11) >> 2;

      *cb = (short)( (rcb[rd] + gcb[gd] + bcb[bd]) >> 16 );
      *cr = (short)( (rcr[rd] + gcr[gd] + bcr[bd]) >> 16 );

      rgb+=8;
      cb++;
      cr++;
    }
    rgb += 8*width-64;
  }

}

void jpeg_rgb32_to_ycc(int width,int height,int outWidth,int outHeight,unsigned char *rgb32,short *ycc)
{

  // outWidth and outHeight must be multiple of 16
  // ycc map is stored as jpeg 8x8 block order (4xY 1xCb 1xCr)
  // ycc must reference a buffer of at least (outWidth*outHeight*3) bytes

  short *y = ycc;
  unsigned char *rgb = rgb32;
  short *cb = ycc + 64*4;
  short *cr = ycc + 64*5;
  int   k,l,yp,scr;
  unsigned char rgbScrath[16*16*4];
  int w16 = (width >>4) * 16;
  int h16 = (height>>4) * 16;
  int paddWidth  = (w16<outWidth);
  int paddHeight = (h16<outHeight);

  for(l=0;l<h16;l+=16) {

    for(k=0;k<w16;k+=16) {
      rgb = rgb32 + (k + l*width)*4;
#ifdef JPG_USE_ASM
      conv_block_RGB32H2V2_mmx((long)width,rgb,y,cb,cr);
#else
      conv_block_RGB32H2V2(width,rgb,y,cb,cr);
#endif
      y  += 64*6;
      cb += 64*6;
      cr += 64*6;
    }

    // border
    if( paddWidth ) {

      // Copy last byte and padd
      for(yp=l;yp<l+16;yp++) {
        scr = (yp-l)*64;
        memcpy(rgbScrath+scr,rgb32 + (w16 + yp*width)*4,(width-w16)*4);
        unsigned char rPadd = *( rgb32 + ((width-1) + yp*width)*4 + 0);
        unsigned char gPadd = *( rgb32 + ((width-1) + yp*width)*4 + 1);
        unsigned char bPadd = *( rgb32 + ((width-1) + yp*width)*4 + 2);
        for(int w=width-w16;w<16;w++) {
          rgbScrath[scr+w*4+0]=rPadd;
          rgbScrath[scr+w*4+1]=gPadd;
          rgbScrath[scr+w*4+2]=bPadd;
          rgbScrath[scr+w*4+3]=0;
        }
      }
      rgb = (unsigned char *)rgbScrath;
#ifdef JPG_USE_ASM
      conv_block_RGB32H2V2_mmx((long)16,rgb,y,cb,cr);
#else
      conv_block_RGB32H2V2(16,rgb,y,cb,cr);
#endif
      y  += 64*6;
      cb += 64*6;
      cr += 64*6;

    }

  }

  // Bottom
  if(paddHeight) {

    for(k=0;k<w16;k+=16) {

      for(yp=l;yp<height;yp++) {
        scr = (yp-l)*64;
        memcpy(rgbScrath+scr,rgb32 + (k + yp*width)*4,16*4);
      }
      for(;yp<outHeight;yp++) {
        scr = (yp-l)*64;
        memcpy(rgbScrath+scr,rgb32 + (k + (height-1)*width)*4,16*4);
      }
      rgb = (unsigned char *)rgbScrath;
#ifdef JPG_USE_ASM
      conv_block_RGB32H2V2_mmx((long)16,rgb,y,cb,cr);
#else
      conv_block_RGB32H2V2(16,rgb,y,cb,cr);
#endif
      y  += 64*6;
      cb += 64*6;
      cr += 64*6;

    }

    // bottom corner
    if( paddWidth ) {

      // Copy last byte and padd
      for(yp=h16;yp<height;yp++) {
        scr = (yp-l)*64;
        memcpy(rgbScrath+scr,rgb32 + (w16 + yp*width)*4,(width-w16)*4);
        unsigned char rPadd = *( rgb32 + ((width-1) + yp*width)*4 + 0);
        unsigned char gPadd = *( rgb32 + ((width-1) + yp*width)*4 + 1);
        unsigned char bPadd = *( rgb32 + ((width-1) + yp*width)*4 + 2);
        for(int w=width-w16;w<16;w++) {
          rgbScrath[scr+w*4+0]=rPadd;
          rgbScrath[scr+w*4+1]=gPadd;
          rgbScrath[scr+w*4+2]=bPadd;
          rgbScrath[scr+w*4+3]=0;
        }
      }
      int scrL = (height-h16-1)*16;
      for(;yp<outHeight;yp++) {
        scr = (yp-l)*64;
        memcpy(rgbScrath+scr,rgbScrath+scrL,16*4);
      }
      rgb = (unsigned char *)rgbScrath;
#ifdef JPG_USE_ASM
      conv_block_RGB32H2V2_mmx((long)16,rgb,y,cb,cr);
#else
      conv_block_RGB32H2V2(16,rgb,y,cb,cr);
#endif

    }

  }

#ifdef JPG_USE_ASM
#ifdef _WINDOWS
  __asm emms;
#else
  __asm__ ("emms\n"::);
#endif
#endif

}

// --------------------------------------------------------------------------------------
// Convert 16x16 RGB24 pixel map to (4xY 1xCb 1xCr) block (4:2:0)
// --------------------------------------------------------------------------------------

void conv_block_RGB24H2V2(int width,unsigned char *rgb,short *y,short *cb,short *cr)
{

  short r00,g00,b00,r10,g10,b10,r01,g01,b01,r11,g11,b11;
  int y0,yIdx;
  short rd,gd,bd;

  for(int j=0;j<8;j++) {
    y0 = ((j&4)<<5) + ((j&3)<<4);
    for(int i=0;i<8;i++) {

      yIdx = y0 + ((i&4)<<4) + ((i&3)<<1);
      r00 = (short)rgb[0];
      g00 = (short)rgb[1];
      b00 = (short)rgb[2];

      r10 = (short)rgb[3];
      g10 = (short)rgb[4];
      b10 = (short)rgb[5];

      r01 = (short)rgb[0+width*3];
      g01 = (short)rgb[1+width*3];
      b01 = (short)rgb[2+width*3];

      r11 = (short)rgb[3+width*3];
      g11 = (short)rgb[4+width*3];
      b11 = (short)rgb[5+width*3];

      y[0+yIdx] = (short)( (ry[r00] + gy[g00] + by[b00]) >> 16 ) - 128;
      y[1+yIdx] = (short)( (ry[r10] + gy[g10] + by[b10]) >> 16 ) - 128;
      y[8+yIdx] = (short)( (ry[r01] + gy[g01] + by[b01]) >> 16 ) - 128;
      y[9+yIdx] = (short)( (ry[r11] + gy[g11] + by[b11]) >> 16 ) - 128;

      rd = (r00 + r10 + r01 + r11) >> 2;
      gd = (g00 + g10 + g01 + g11) >> 2;
      bd = (b00 + b10 + b01 + b11) >> 2;

      *cb = (short)( (rcb[rd] + gcb[gd] + bcb[bd]) >> 16 );
      *cr = (short)( (rcr[rd] + gcr[gd] + bcr[bd]) >> 16 );

      rgb+=6;
      cb++;
      cr++;
    }
    rgb += 6*width-48;
  }

}

void jpeg_rgb24_to_ycc(int width,int height,int outWidth,int outHeight,unsigned char *rgb24,short *ycc)
{

  // outWidth and outHeight must be multiple of 16
  // ycc map is stored as jpeg 8x8 block order (4xY 1xCb 1xCr)
  // ycc must reference a buffer of at least (outWidth*outHeight*3) bytes

  short *y = ycc;
  unsigned char *rgb = rgb24;
  short *cb = ycc + 64*4;
  short *cr = ycc + 64*5;
  int   k,l,yp,scr;
  unsigned char rgbScrath[16*16*3];
  int w16 = (width >>4) * 16;
  int h16 = (height>>4) * 16;
  int paddWidth  = (w16<outWidth);
  int paddHeight = (h16<outHeight);

  for(l=0;l<h16;l+=16) {

    for(k=0;k<w16;k+=16) {
      rgb = rgb24 + (k + l*width)*3;
#ifdef JPG_USE_ASM
      conv_block_RGB24H2V2_mmx((long)width,rgb,y,cb,cr);
#else
      conv_block_RGB24H2V2(width,rgb,y,cb,cr);
#endif
      y  += 64*6;
      cb += 64*6;
      cr += 64*6;
    }

    // border
    if( paddWidth ) {

      // Copy last byte and padd
      for(yp=l;yp<l+16;yp++) {
        scr = (yp-l)*48;
        memcpy(rgbScrath+scr,rgb24 + (w16 + yp*width)*3,(width-w16)*3);
        unsigned char rPadd = *( rgb24 + ((width-1) + yp*width)*3 + 0);
        unsigned char gPadd = *( rgb24 + ((width-1) + yp*width)*3 + 1);
        unsigned char bPadd = *( rgb24 + ((width-1) + yp*width)*3 + 2);
        for(int w=width-w16;w<16;w++) {
          rgbScrath[scr+w*3+0]=rPadd;
          rgbScrath[scr+w*3+1]=gPadd;
          rgbScrath[scr+w*3+2]=bPadd;
        }
      }
      rgb = (unsigned char *)rgbScrath;
#ifdef JPG_USE_ASM
      conv_block_RGB24H2V2_mmx((long)16,rgb,y,cb,cr);
#else
      conv_block_RGB24H2V2(16,rgb,y,cb,cr);
#endif
      y  += 64*6;
      cb += 64*6;
      cr += 64*6;

    }

  }

  // Bottom
  if(paddHeight) {

    for(k=0;k<w16;k+=16) {

      for(yp=l;yp<height;yp++) {
        scr = (yp-l)*48;
        memcpy(rgbScrath+scr,rgb24 + (k + yp*width)*3,16*3);
      }
      for(;yp<outHeight;yp++) {
        scr = (yp-l)*48;
        memcpy(rgbScrath+scr,rgb24 + (k + (height-1)*width)*3,16*3);
      }
      rgb = (unsigned char *)rgbScrath;
#ifdef JPG_USE_ASM
      conv_block_RGB24H2V2_mmx((long)16,rgb,y,cb,cr);
#else
      conv_block_RGB24H2V2(16,rgb,y,cb,cr);
#endif
      y  += 64*6;
      cb += 64*6;
      cr += 64*6;

    }

    // bottom corner
    if( paddWidth ) {

      // Copy last byte and padd
      for(yp=h16;yp<height;yp++) {
        scr = (yp-l)*48;
        memcpy(rgbScrath+scr,rgb24 + (w16 + yp*width)*3,(width-w16)*3);
        unsigned char rPadd = *( rgb24 + ((width-1) + yp*width)*3 + 0);
        unsigned char gPadd = *( rgb24 + ((width-1) + yp*width)*3 + 1);
        unsigned char bPadd = *( rgb24 + ((width-1) + yp*width)*3 + 2);
        for(int w=width-w16;w<16;w++) {
          rgbScrath[scr+w*3+0]=rPadd;
          rgbScrath[scr+w*3+1]=gPadd;
          rgbScrath[scr+w*3+2]=bPadd;
        }
      }
      int scrL = (height-h16-1)*16;
      for(;yp<outHeight;yp++) {
        scr = (yp-l)*48;
        memcpy(rgbScrath+scr,rgbScrath+scrL,16*3);
      }
      rgb = (unsigned char *)rgbScrath;
#ifdef JPG_USE_ASM
      conv_block_RGB24H2V2_mmx((long)16,rgb,y,cb,cr);
#else
      conv_block_RGB24H2V2(16,rgb,y,cb,cr);
#endif

    }

  }

#ifdef JPG_USE_ASM
#ifdef _WINDOWS
  __asm emms;
#else
  __asm__ ("emms\n"::);
#endif
#endif

}

// --------------------------------------------------------------------------------------
// Convert GRAY8 pixel map to Y blocks
// --------------------------------------------------------------------------------------
void conv_block_GRAY8Y(int width,unsigned char *g,short *y) {

  for(int j=0;j<8;j++) {
    for(int i=0;i<8;i++) {
      *y = (short)(*g) - 128;
      y++;
      g++;
    }
    g += (width - 8);
  }

}

void jpeg_gray8_to_y(int width,int height,int outWidth,int outHeight,unsigned char *gray8,short *yy)
{

  // outWidth and outHeight must be multiple of 8
  // ycc map is stored as jpeg 8x8 block order
  // ycc must reference a buffer of at least (outWidth*outHeight*2) bytes

  short *y = yy;
  unsigned char *g;
  unsigned char gScrath[8*8];
  int    k,l,yp,scr;
  int    w8 = (width >>3) * 8;
  int    h8 = (height>>3) * 8;
  int    paddWidth  = (w8<outWidth);
  int    paddHeight = (h8<outHeight);

  for(l=0;l<h8;l+=8) {
    for(k=0;k<w8;k+=8) {
      g = gray8 + (k + l*width);
#ifdef JPG_USE_ASM
      conv_block_GRAY8Y_mmx((long)width,g,y);
#else
      conv_block_GRAY8Y(width,g,y);
#endif
      y  += 64;
    }

    // border
    if( paddWidth ) {

      // Copy last byte and padd
      for(yp=l;yp<l+8;yp++) {
        scr = (yp-l)*8;
        memcpy(gScrath+scr,gray8 + (w8 + yp*width),(width-w8));
        unsigned char gPadd = *(gray8 + ((width-1) + yp*width));
        for(int w=width-w8;w<8;w++) {
          gScrath[scr+w]=gPadd;
        }
      }
      g = (unsigned char *)gScrath;
#ifdef JPG_USE_ASM
      conv_block_GRAY8Y_mmx((long)8,g,y);
#else
      conv_block_GRAY8Y(8,g,y);
#endif
      y  += 64;

    }

  }

  // Bottom
  if(paddHeight) {

    for(k=0;k<w8;k+=8) {

      for(yp=l;yp<height;yp++) {
        scr = (yp-l)*8;
        memcpy(gScrath+scr,gray8 + (k + yp*width),8);
      }
      for(;yp<outHeight;yp++) {
        scr = (yp-l)*8;
        memcpy(gScrath+scr,gray8 + (k + (height-1)*width),8);
      }
      g = (unsigned char *)gScrath;
#ifdef JPG_USE_ASM
      conv_block_GRAY8Y_mmx((long)8,g,y);
#else
      conv_block_GRAY8Y(8,g,y);
#endif
      y  += 64;

    }

    // bottom corner
    if( paddWidth ) {

      // Copy last byte and padd
      for(yp=h8;yp<height;yp++) {
        scr = (yp-l)*8;
        memcpy(gScrath+scr,gray8 + (w8 + yp*width),(width-w8));
        unsigned char gPadd = *(gray8 + ((width-1) + yp*width));
        for(int w=width-w8;w<8;w++) {
          gScrath[scr+w]=gPadd;
        }
      }
      int scrL = (height-h8-1)*8;
      for(;yp<outHeight;yp++) {
        scr = (yp-l)*8;
        memcpy(gScrath+scr,gScrath+scrL,8);
      }
      g = (unsigned char *)gScrath;
#ifdef JPG_USE_ASM
      conv_block_GRAY8Y_mmx((long)8,g,y);
#else
      conv_block_GRAY8Y(8,g,y);
#endif

    }

  }

#ifdef JPG_USE_ASM
#ifdef _WINDOWS
  __asm emms;
#else
  __asm__ ("emms\n"::);
#endif
#endif

}

