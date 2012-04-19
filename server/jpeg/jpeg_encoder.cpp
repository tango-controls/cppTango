///=============================================================================
//
// file :		jpeg_encoder.cpp
//
// description :        Simple jpeg coding/decoding library
//                      Main encoding functions
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
// Revision 1.3  2009/09/08 14:23:16  taurel
// - No real change, just to make CVS quiet
//
// Revision 1.2  2009/04/20 14:55:58  jlpons
// Added GPL header, changed memory allocation to C++ fashion.
//
//=============================================================================

//
// File:        jpeg_encoder.cpp
// Description: Main encoding functions
// Program:     Simple jpeg coding/decoding library
// Author:      JL Pons 2009
//

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "jpeg_lib.h"
#include "jpeg_const.h"
#include "jpeg_memory.h"
#include "jpeg_bitstream.h"

/* These are the sample quantization tables given in JPEG spec section K.1.
 * The spec says that the values given produce "good" quality, and
 * when divided by 2, "very good" quality.
 */
static double std_luminance_quant_tbl[] = {
  16.0,  11.0,  10.0,  16.0,  24.0,  40.0,  51.0,  61.0,
  12.0,  12.0,  14.0,  19.0,  26.0,  58.0,  60.0,  55.0,
  14.0,  13.0,  16.0,  24.0,  40.0,  57.0,  69.0,  56.0,
  14.0,  17.0,  22.0,  29.0,  51.0,  87.0,  80.0,  62.0,
  18.0,  22.0,  37.0,  56.0,  68.0, 109.0, 103.0,  77.0,
  24.0,  35.0,  55.0,  64.0,  81.0, 104.0, 113.0,  92.0,
  49.0,  64.0,  78.0,  87.0, 103.0, 121.0, 120.0, 101.0,
  72.0,  92.0,  95.0,  98.0, 112.0, 100.0, 103.0,  99.0
};

static double std_chrominance_quant_tbl[] = {
  17.0,  18.0,  24.0,  47.0,  99.0,  99.0,  99.0,  99.0,
  18.0,  21.0,  26.0,  66.0,  99.0,  99.0,  99.0,  99.0,
  24.0,  26.0,  56.0,  99.0,  99.0,  99.0,  99.0,  99.0,
  47.0,  66.0,  99.0,  99.0,  99.0,  99.0,  99.0,  99.0,
  99.0,  99.0,  99.0,  99.0,  99.0,  99.0,  99.0,  99.0,
  99.0,  99.0,  99.0,  99.0,  99.0,  99.0,  99.0,  99.0,
  99.0,  99.0,  99.0,  99.0,  99.0,  99.0,  99.0,  99.0,
  99.0,  99.0,  99.0,  99.0,  99.0,  99.0,  99.0,  99.0
};

// Zig zag order
int jpgZag[64] =
{
  0,  1,  8, 16,  9,  2,  3, 10,
 17, 24, 32, 25, 18, 11,  4,  5,
 12, 19, 26, 33, 40, 48, 41, 34,
 27, 20, 13,  6,  7, 14, 21, 28,
 35, 42, 49, 56, 57, 50, 43, 36,
 29, 22, 15, 23, 30, 37, 44, 51,
 58, 59, 52, 45, 38, 31, 39, 46,
 53, 60, 61, 54, 47, 55, 62, 63,
};

// Default Huffman table
static unsigned char bits_dc_luminance[17] =
  { 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
static unsigned char val_dc_luminance[] =
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

static unsigned char bits_dc_chrominance[17] =
  { 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
static unsigned char val_dc_chrominance[] =
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

static unsigned char bits_ac_luminance[17] =
  { 0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
static unsigned char val_ac_luminance[] =
  { 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
    0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
    0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
    0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
    0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
    0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
    0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
    0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
    0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa };

static unsigned char bits_ac_chrominance[17] =
  { 0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
static unsigned char val_ac_chrominance[] =
  { 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
    0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
    0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
    0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
    0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
    0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
    0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
    0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
    0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
    0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa };

// color conversion declaration (jpeg_color.cpp)
void jpeg_init_color();
void jpeg_rgb32_to_ycc(int width,int height,int outWidth,int outHeight,unsigned char *rgb32,short *ycc);
void jpeg_rgb24_to_ycc(int width,int height,int outWidth,int outHeight,unsigned char *rgb32,short *ycc);
void jpeg_gray8_to_y(int width,int height,int outWidth,int outHeight,unsigned char *gray8,short *y);

// Forward dct (jpeg_dct.cpp)
void jpeg_fdct(short *block);
void jpeg_fdct_mmx(short *block);

// ----------------------------------------------------------------
// Start of Image marker

static void jpeg_write_SOI(OutputBitStream *bs) {

  bs->put_byte(0xFF);
  bs->put_byte(M_SOI);

}

static void jpeg_write_EOI(OutputBitStream *bs) {

  bs->put_byte(0xFF);
  bs->put_byte(M_EOI);

}

// ----------------------------------------------------------------
// Quantization table

static void jpeg_write_DQT(OutputBitStream *bs,short *qTable,int tableId,int prec) {

  int mSize;

  bs->put_byte(0xFF);
  bs->put_byte(M_DQT);

  if( prec ) mSize = 64*2 + 1 + 2;
  else       mSize = 64 + 1 + 2;
  bs->put_short( mSize );
  bs->put_byte( (prec<<4) + tableId );

  for (int i=0;i<64;i++) {
    unsigned short val = (qTable[jpgZag[i]] >> 3);
    if (prec)  bs->put_byte( val>>8 );
    bs->put_byte( val & 0xFF );
  }

}

// ----------------------------------------------------------------

static void jpeg_scale_qtable(double quality,double *qTable,short *out,int *prec) {

  // Set up quality 1->poor, 100->max
  // This model gives approximately a linear increase of the file size
  // according to the quality value. When quality is set to 100.0, all
  // quatization values are set to 1 => No loss due to quatization.
  double qualityFactor = 0.01;

  if( quality<1.0 )   quality=1.0;
  if( quality>100.0 ) quality=100.0;

  if( quality<100.0 ) qualityFactor = - 0.1/log( 1.0 - (quality/100.0) );

  *prec = 0;
  for(int i=0;i<64;i++) {
    short val = (short)( qTable[i]*qualityFactor + 0.5 );
    if( val<1 ) val = 1;
    if( val>255 ) *prec=1;
    out[i] = val << 3;  // *8 for DCT scaling
  }

}

// ----------------------------------------------------------------

static void jpeg_init_htable(HUFFMANTABLE *table,unsigned char *bits,unsigned char *vals) {

  int p, i, l, lastp, si;
  char huffsize[257];
  unsigned int huffcode[257];
  unsigned int code;

  // Does not perform checks (encoding phase with valid tables)
  table->huffBits = bits;
  table->huffVal = vals;

  p = 0;
  for (l = 1; l <= 16; l++) {
    i = (int) bits[l];
    while (i--) huffsize[p++] = (char) l;
  }
  huffsize[p] = 0;
  lastp = p;

  code = 0;
  si = huffsize[0];
  p = 0;
  while (huffsize[p]) {
    while (((int) huffsize[p]) == si) {
      huffcode[p++] = code;
      code++;
    }
    code <<= 1;
    si++;
  }

  memset(table->huffSize,0,256);

  for (p = 0; p < lastp; p++) {
    i = vals[p];
    table->huffCode[i] = huffcode[p];
    table->huffSize[i] = huffsize[p];
  }

}

// ----------------------------------------------------------------

static void jpeg_write_DHT(OutputBitStream *bs,HUFFMANTABLE *table,int tableId) {

  int mSize;
  int lgth = 0;

  bs->put_byte(0xFF);
  bs->put_byte(M_DHT);

  for (int i=1;i<=16;i++)
    lgth += table->huffBits[i];
  mSize = lgth + 2 + 1 + 16;

  bs->put_short( mSize );
  bs->put_byte( tableId );

  for (int i=1;i<=16;i++)
    bs->put_byte(table->huffBits[i]);

  for (int i=0;i<lgth;i++)
    bs->put_byte(table->huffVal[i]);

}
// ----------------------------------------------------------------

static void jpeg_write_SOF(OutputBitStream *bs,int width,int height,JPGCOMPONENT *comps,int nbComp) {

  int mSize = 3 * nbComp + 2 + 5 + 1;

  bs->put_byte(0xFF);
  bs->put_byte(M_SOF1);
  bs->put_short( mSize );
  bs->put_byte( 8 ); // Precision
  bs->put_short(height);
  bs->put_short(width);
  bs->put_byte(nbComp);

  for (int i=0;i<nbComp;i++) {
    bs->put_byte(comps[i].id);
    bs->put_byte((comps[i].horzSampling << 4) + comps[i].vertSampling);
    bs->put_byte(comps[i].quantIdx);
  }

}

// ----------------------------------------------------------------

static void jpeg_write_SOS(OutputBitStream *bs,JPGCOMPONENT *comps,int nbComp) {

  int mSize = 2 * nbComp + 2 + 1 + 3;

  bs->put_byte(0xFF);
  bs->put_byte(M_SOS);
  bs->put_short( mSize );
  bs->put_byte(nbComp);

  for (int i=0;i<nbComp;i++) {
    bs->put_byte(comps[i].id);
    bs->put_byte((comps[i].dcIdx << 4) + comps[i].acIdx);
  }

  bs->put_byte(0);  // Spectral start               (progressive only)
  bs->put_byte(63); // Spectral end                 (progressive only)
  bs->put_byte(0);  // Successive approx. high,low  (progressive only)

}

// ----------------------------------------------------------------

static void jpeg_quantize_block(short *blocks,unsigned short *qDiv) {

//#ifdef JPG_USE_ASM
#if 0

  short mmRound[]  = { 0x7FFF,0x7FFF,0,0 };
  short mmOne[]    = { 1,1,0,0 };

  _asm {

    mov  edi,blocks
    mov  esi,qDiv
    movq mm0,mmRound
    movq mm1,mmOne
    mov  ecx,8

_sseqrows:

    movd mm2,[edi]
    movd mm4,[edi+4]
    movd mm3,[esi]
    movd mm5,[esi+4]
    punpcklwd mm2,mm0
    punpcklwd mm3,mm1
    punpcklwd mm4,mm0
    punpcklwd mm5,mm1
    pmaddwd mm2,mm3
    pmaddwd mm4,mm5
    pshufw mm6,mm2,141
    pshufw mm7,mm4,141
    movd [edi],mm6
    movd [edi+4],mm7

    movd mm2,[edi+8]
    movd mm4,[edi+12]
    movd mm3,[esi+8]
    movd mm5,[esi+12]
    punpcklwd mm2,mm0
    punpcklwd mm3,mm1
    punpcklwd mm4,mm0
    punpcklwd mm5,mm1
    pmaddwd mm2,mm3
    pmaddwd mm4,mm5
    pshufw mm6,mm2,141
    pshufw mm7,mm4,141
    movd [edi+8],mm6
    movd [edi+12],mm7

    add edi,16
    add esi,16
    dec ecx
    jnz _sseqrows
    emms

  }

#else

  int val;

  for(int i=0;i<64;i++) {

    val = (int)blocks[i];
    val *= (int)qDiv[i];
    val += 32767; // round
    val >>= 16;
    blocks[i] = val;

  }

#endif

}

// ----------------------------------------------------------------

static void jpeg_encode_rgb(int width,int height,unsigned char *rgb,double quality,
                            int *jpegSize,unsigned char **jpegData,int rgbW) {

  short lumQuant[64];        // Luminance quantization table
  short chrQuant[64];        // Chrominance quantization table
  unsigned short *lumDiv;    // Luminance quantization table divisor
  unsigned short *chrDiv;    // Chrominance quantization table divisor
  JPGCOMPONENT comps[3];     // Components YCbCr
  HUFFMANTABLE hTables[4];   // Huffman tables

  int prec;
  int rWidth  = ((width +15)>>4) * 16;
  int rHeight = ((height+15)>>4) * 16;

  OutputBitStream *bs = new OutputBitStream();

  // Header
  jpeg_write_SOI(bs);

  // Quatization tables
  jpeg_scale_qtable(quality,std_luminance_quant_tbl,lumQuant,&prec);
  jpeg_scale_qtable(quality,std_chrominance_quant_tbl,chrQuant,&prec);
  jpeg_write_DQT(bs,lumQuant,0,prec);
  jpeg_write_DQT(bs,chrQuant,1,prec);

  // Huffman tables
  jpeg_init_htable(hTables+0,bits_dc_luminance,val_dc_luminance);
  jpeg_init_htable(hTables+1,bits_ac_luminance,val_ac_luminance);
  jpeg_init_htable(hTables+2,bits_dc_chrominance,val_dc_luminance);
  jpeg_init_htable(hTables+3,bits_ac_chrominance,val_ac_luminance);
  jpeg_write_DHT(bs,hTables+0,0);
  jpeg_write_DHT(bs,hTables+1,0+0x10);
  jpeg_write_DHT(bs,hTables+2,1);
  jpeg_write_DHT(bs,hTables+3,1+0x10);

  // Luminance component (Y)
  comps[0].horzSampling = 2;
  comps[0].vertSampling = 2;
  comps[0].id = 1;
  comps[0].quantIdx = 0;
  comps[0].dcIdx = 0;
  comps[0].acIdx = 0;
  comps[0].lastDc = 0;

  // Chrominance component (Cb)
  comps[1].horzSampling = 1;
  comps[1].vertSampling = 1;
  comps[1].id = 2;
  comps[1].quantIdx = 1;
  comps[1].dcIdx = 1;
  comps[1].acIdx = 1;
  comps[1].lastDc = 0;

  // Chrominance component (Cr)
  comps[2].horzSampling = 1;
  comps[2].vertSampling = 1;
  comps[2].id = 3;
  comps[2].quantIdx = 1;
  comps[2].dcIdx = 1;
  comps[2].acIdx = 1;
  comps[2].lastDc = 0;

  jpeg_write_SOF(bs,width,height,comps,3);
  jpeg_write_SOS(bs,comps,3);

  // Initialise quantization divisor
  lumDiv = (unsigned short *)malloc_16(64*2);
  chrDiv = (unsigned short *)malloc_16(64*2);
  for(int i=0;i<64;i++) {
    lumDiv[i] = (unsigned short)( 65536.0/(double)lumQuant[i] + 0.5 );
    chrDiv[i] = (unsigned short)( 65536.0/(double)chrQuant[i] + 0.5 );
  }

  // Convert to YUV
  jpeg_init_color();
  short *ycc = (short *)malloc_16(rWidth*rHeight*3);
  if( rgbW==24 )
    jpeg_rgb24_to_ycc(width,height,rWidth,rHeight,rgb,ycc);
  else
    jpeg_rgb32_to_ycc(width,height,rWidth,rHeight,rgb,ycc);

  // Encode blocks (downsampling :2 for Cb and Cr)
  int nbMCU = rWidth/16 * rHeight/16;
  short *block = ycc;
  for(int i=0;i<nbMCU;i++) {

    // Luminace (Y)
#ifdef JPG_USE_ASM
    jpeg_fdct_mmx(block+0);
    jpeg_fdct_mmx(block+64);
    jpeg_fdct_mmx(block+128);
    jpeg_fdct_mmx(block+192);
#else
    jpeg_fdct(block+0);
    jpeg_fdct(block+64);
    jpeg_fdct(block+128);
    jpeg_fdct(block+192);
#endif
    jpeg_quantize_block(block+0  ,lumDiv);
    jpeg_quantize_block(block+64 ,lumDiv);
    jpeg_quantize_block(block+128,lumDiv);
    jpeg_quantize_block(block+192,lumDiv);

    // Chrominance (Cb)
    jpeg_fdct(block+256);
    jpeg_quantize_block(block+256,chrDiv);

    // Chrominance (Cr)
    jpeg_fdct(block+320);
    jpeg_quantize_block(block+320,chrDiv);

    block+=384;

  }

  block = ycc;
  bs->init();
  for(int i=0;i<nbMCU;i++) {

    // Luminace
    bs->encode_block(block+0  ,hTables+0,hTables+1,&(comps[0].lastDc));
    bs->encode_block(block+64 ,hTables+0,hTables+1,&(comps[0].lastDc));
    bs->encode_block(block+128,hTables+0,hTables+1,&(comps[0].lastDc));
    bs->encode_block(block+192,hTables+0,hTables+1,&(comps[0].lastDc));
    // Chrominance
    bs->encode_block(block+256,hTables+2,hTables+3,&(comps[1].lastDc));
    bs->encode_block(block+320,hTables+2,hTables+3,&(comps[2].lastDc));

    block+=384;

  }

  bs->flush();

  jpeg_write_EOI(bs);
  free_16(ycc);
  free_16(lumDiv);
  free_16(chrDiv);
  *jpegData = (unsigned char *)malloc(bs->get_size());
  *jpegSize = bs->get_size();
  memcpy(*jpegData,bs->get_data(),bs->get_size());
  delete bs;

}

// --------------------------------------------------------------------------

void jpeg_encode_rgb32(int width,int height,unsigned char *rgb32,double quality,
                     int *jpegSize,unsigned char **jpegData) {
  jpeg_encode_rgb(width,height,rgb32,quality,jpegSize,jpegData,32);
}

void jpeg_encode_rgb24(int width,int height,unsigned char *rgb24,double quality,
                     int *jpegSize,unsigned char **jpegData) {
  jpeg_encode_rgb(width,height,rgb24,quality,jpegSize,jpegData,24);
}

// --------------------------------------------------------------------------

void jpeg_encode_gray8(int width,int height,unsigned char *gray8,double quality,
                       int *jpegSize,unsigned char **jpegData) {

  short lumQuant[64];        // Luminance quantization table
  unsigned short *lumDiv;    // Luminance quantization table divisor
  JPGCOMPONENT comps[1];     // Component Y
  HUFFMANTABLE hTables[2];   // Huffman tables

  int prec;
  int rWidth  = ((width +7)>>3) * 8;
  int rHeight = ((height+7)>>3) * 8;

  OutputBitStream *bs = new OutputBitStream();

  // Header
  jpeg_write_SOI(bs);

  // Quatization tables
  jpeg_scale_qtable(quality,std_luminance_quant_tbl,lumQuant,&prec);
  jpeg_write_DQT(bs,lumQuant,0,prec);

  // Huffman tables
  jpeg_init_htable(hTables+0,bits_dc_luminance,val_dc_luminance);
  jpeg_init_htable(hTables+1,bits_ac_luminance,val_ac_luminance);
  jpeg_write_DHT(bs,hTables+0,0);
  jpeg_write_DHT(bs,hTables+1,0+0x10);

  // Luminance component (Y)
  comps[0].horzSampling = 1;
  comps[0].vertSampling = 1;
  comps[0].id = 1;
  comps[0].quantIdx = 0;
  comps[0].dcIdx = 0;
  comps[0].acIdx = 0;
  comps[0].lastDc = 0;

  jpeg_write_SOF(bs,width,height,comps,1);
  jpeg_write_SOS(bs,comps,1);

  // Initialise quantization divisor
  lumDiv = (unsigned short *)malloc_16(64*2);
  for(int i=0;i<64;i++) {
    lumDiv[i] = (unsigned short)( 65536.0/(double)lumQuant[i] + 0.5 );
  }

  // Convert to YUV
  jpeg_init_color();
  short *ycc = (short *)malloc_16(rWidth*rHeight*2);
  jpeg_gray8_to_y(width,height,rWidth,rHeight,gray8,ycc);

  // Encode blocks (downsampling :2 for Cb and Cr)
  int nbMCU = rWidth/8 * rHeight/8;
  short *block = ycc;
  for(int i=0;i<nbMCU;i++) {

    // Luminace (Y)
#ifdef JPG_USE_ASM
    jpeg_fdct_mmx(block);
#else
    jpeg_fdct(block);
#endif
    jpeg_quantize_block(block,lumDiv);

    block+=64;

  }

  block = ycc;
  bs->init();
  for(int i=0;i<nbMCU;i++) {
    bs->encode_block(block,hTables+0,hTables+1,&(comps[0].lastDc));
    block+=64;
  }
  bs->flush();

  jpeg_write_EOI(bs);
  free_16(ycc);
  free_16(lumDiv);
  *jpegData = (unsigned char *)malloc(bs->get_size());
  *jpegSize = bs->get_size();
  memcpy(*jpegData,bs->get_data(),bs->get_size());
  delete bs;


}
