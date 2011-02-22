///=============================================================================	
//
// file :		jpeg_decoder.cpp
//
// description :        Simple jpeg coding/decoding library
//                      Main decoding functions
//                      (does not support progressive decoding)
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

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "jpeg_lib.h"
#include "jpeg_const.h"
#include "jpeg_memory.h"
#include "jpeg_bitstream.h"


// color conversion declaration (jpeg_color.cpp)
void jpeg_init_color();
void jpeg_yh2v2_to_rgb32(unsigned char *block,int width,unsigned char *rgb);
void jpeg_yh1v1_to_rgb32(unsigned char *block,int width,unsigned char *rgb);
void jpeg_y_to_gray(unsigned char *block,int width,unsigned char *rgb);
#ifdef JPG_USE_ASM
void jpeg_yh2v2_to_rgb32_mmx(unsigned char *block,long width,unsigned char *rgb);
void jpeg_yh1v1_to_rgb32_mmx(unsigned char *block,long width,unsigned char *rgb);
#endif

extern int jpgZag[];

// ------------------------------------------------------------------

char *jpeg_get_error_msg(int code) {

  static char retMsg[1024];
  switch(-code) {
    case  0: sprintf(retMsg,"No error");break;
    case  1: sprintf(retMsg,"Can't find start of image (SOI), may be not a jpeg stream");break;
    case  2: sprintf(retMsg,"Not supported jpeg format");break;
    case  3: sprintf(retMsg,"Unexpected jpeg marker found");break;
    case  4: sprintf(retMsg,"Unexpected end of stream or marker not found");break;
    case  5: sprintf(retMsg,"Bad DQT marker");break;
    case  6: sprintf(retMsg,"Invalid quantization table index");break;
    case  7: sprintf(retMsg,"Invalid quantization table size");break;
    case  8: sprintf(retMsg,"Invalid variable marker length");break;
    case  9: sprintf(retMsg,"Bad DHT marker");break;
    case 10: sprintf(retMsg,"Bad DHT count");break;
    case 11: sprintf(retMsg,"Bad DHT index");break;
    case 12: sprintf(retMsg,"Bad SOS marker");break;
    case 13: sprintf(retMsg,"Bad component id");break;
    case 14: sprintf(retMsg,"Only 8 bit precission supported");break;
    case 15: sprintf(retMsg,"Invalid image size (8192x8192 max)");break;
    case 16: sprintf(retMsg,"Too many components in frame");break;
    case 17: sprintf(retMsg,"Bad SOF marker");break;
    case 18: sprintf(retMsg,"Bad DRI marker");break;
    case 19: sprintf(retMsg,"One or more quantisation table(s) are missing");break;
    case 20: sprintf(retMsg,"One or more huffman table(s) are missing");break;
    case 21: sprintf(retMsg,"Downsampling factor not supported");break;
    case 22: sprintf(retMsg,"Not enough memory");break;
    case 23: sprintf(retMsg,"Bad restart marker");break;
    case 24: sprintf(retMsg,"Decoding error");break;
    case 25: sprintf(retMsg,"Component info missing");break;
    case 26: sprintf(retMsg,"Progressive format not supported");break;
    case 27: sprintf(retMsg,"YH2V1 format not supported");break;
    case 28: sprintf(retMsg,"YH1V2 format not supported");break;
  }

  return retMsg;

}

#define ERROR(code) if(code<0) {jpeg_decoder_free(&decoder);if(bs) delete bs;return code;}
#define CHECK_ERR(code) if(code<0) return code;

// ------------------------------------------------------------------

static int jpeg_read_soi_marker(InputBitStream *bs)
{

  int lastchar, thischar;
  int bytesleft;

  lastchar = bs->get_byte();
  thischar = bs->get_byte();
  if ((lastchar == 0xFF) && (thischar == M_SOI))
    return 0;

  bytesleft = 512;

  while(1)
  {
    if (--bytesleft == 0)
      return -1;

    lastchar = thischar;
    thischar = bs->get_byte();

    if ((lastchar == 0xFF) && (thischar == M_SOI))
      break;
  }

  return 0;

}

// ------------------------------------------------------------------

int jpeg_next_marker(InputBitStream *bs) {

  int c,count=0;
  do {
    do {
      c = bs->get_byte();
      count++;
    } while ( (c!=0xFF) && (count<1024) );
    if( count>=1024 ) return 0;
    do {
      c = bs->get_byte();
    } while (c == 0xFF);
  } while (c==0);

  return c;

}

// ------------------------------------------------------------------

int jpeg_skip_marker(InputBitStream *bs) {

  int left;

  left = (bs->get_byte() << 8) | bs->get_byte();

  if (left < 2)
    return -8;

  left -= 2;

  while (left)
  {
    bs->get_byte();
    left--;
  }

  return 0;
}

// ------------------------------------------------------------------

int jpeg_read_dqt_marker(JPGDECODER *decoder,InputBitStream *bs) {

  int n, i, prec;
  int left;
  int temp;

  left = (bs->get_byte() << 8) | bs->get_byte();

  if (left < 2)
    return -5;

  left -= 2;

  while (left)
  {
    n = bs->get_byte();
    prec = n >> 4;
    n &= 0x0F;

    if (n >= 4)
      return -6;

    if( !decoder->qTables[n] ) decoder->qTables[n] = (short *)malloc_16(64*2);

    // read quantization entries, in zag order
    for (i = 0; i < 64; i++)
    {
      temp = bs->get_byte();
      if (prec)
        temp = (temp << 8) + bs->get_byte();
      decoder->qTables[n][i] = (short)temp;
    }

    i = 64 + 1;

    if (prec)
      i += 64;

    if (left < i)
      return -7;

    left -= i;
  }

  return 0;
}

// ------------------------------------------------------------------

int jpeg_read_dht_marker(JPGDECODER *decoder,InputBitStream *bs) {

  int i, index, count, p, l, si;
  int left;
  int huff_num[17];
  int huff_val[256];
  int huffsize[257];
  int huffcode[257];
  int code;
  int subtree;
  int code_size;
  int lastp;
  int nextfreeentry;
  int currententry;

  left = (bs->get_byte() << 8) | bs->get_byte();

  if (left < 2) return -9;
  left -= 2;

  while (left)
  {

    memset(huff_num,0,17*sizeof(int));
    memset(huff_val,0,256*sizeof(int));
    memset(huffsize,0,257*sizeof(int));
    memset(huffcode,0,257*sizeof(int));

    // Read a Huffman table
    index = bs->get_byte();
    huff_num[0] = 0;
    count = 0;
    for (i = 1; i <= 16; i++) {
      huff_num[i] = bs->get_byte();
      count += huff_num[i];
    }
    if (count > 255) return -10;

    for (i = 0; i < count; i++)
      huff_val[i] = bs->get_byte();

    i = 1 + 16 + count;

    if (left < i) return -9;

    left -= i;
    if ((index & 0x10) > 0x10)
      return -11;

    index = (index & 0x0F) + ((index & 0x10) >> 4) * 4;

    if (index >= 8)  return -11;

    // Initialise huffman tree (fast decoding)
    HUFFMANTABLE *hs = &(decoder->hTables[index]);

    p = 0;
    for (l = 1; l <= 16; l++) {
    for (i = 1; i <= huff_num[l]; i++)
      huffsize[p++] = l;
    }

    lastp = p;
    code = 0;
    si = huffsize[0];
    p = 0;

    while (huffsize[p])
    {
      while (huffsize[p] == si)
      {
        huffcode[p++] = code;
        code++;
      }
      code <<= 1;
      si++;
    }

    nextfreeentry = -1;

    p = 0;
    while (p < lastp)
    {
      i = huff_val[p];
      code = huffcode[p];
      code_size = huffsize[p];

      hs->huffSize[i] = code_size;
      if (code_size <= 8)
      {
        code <<= (8 - code_size);
        for (l = 1 << (8 - code_size); l > 0; l--)
        {
          hs->lookUp[code] = i;
          code++;
        }
      } else {
        subtree = (code >> (code_size - 8)) & 0xFF;
        currententry = hs->lookUp[subtree];
        if (currententry == 0) {
          hs->lookUp[subtree] = currententry = nextfreeentry;
          nextfreeentry -= 2;
        }
        code <<= (16 - (code_size - 8));

        for (l = code_size; l > 9; l--) {
          if ((code & 0x8000) == 0)
            currententry--;

          if (hs->tree[-currententry - 1] == 0) {
            hs->tree[-currententry - 1] = nextfreeentry;
            currententry = nextfreeentry;
            nextfreeentry -= 2;
          } else {
            currententry = hs->tree[-currententry - 1];
          }

          code <<= 1;
        }

        if ((code & 0x8000) == 0)
          currententry--;

        hs->tree[-currententry - 1] = i;
      }

      p++;
    }

    hs->inited = 1;

  }

  return 0;
}

// ------------------------------------------------------------------

int jpeg_read_sos_marker(JPGDECODER *decoder,InputBitStream *bs) {

  int left;
  int i,cId,hId,n;

  left = (bs->get_byte() << 8) | bs->get_byte();

  n = bs->get_byte();

  decoder->compInScan = n;

  left -= 3;

  if ((left != (n*2 + 3)) || (n < 1) || (n > 4) )
    return -12;

  for (i = 0; i < n; i++)
  {
    cId = bs->get_byte();
    hId = bs->get_byte();
    left -= 2;

    if (cId > 4)
      return -13;

    decoder->comps[cId].dcIdx = (hId >> 4) & 15;
    decoder->comps[cId].acIdx = (hId & 15) + 4;
  }

  bs->get_byte(); // spectral_start
  bs->get_byte(); // spectral_end
  bs->get_byte(); // successive app. low,high
  left -= 3;

  while (left) {
    bs->get_byte();
    left--;
  }

  return 0;
}

// ------------------------------------------------------------------

int jpeg_read_sof_marker(JPGDECODER *decoder,InputBitStream *bs) {

  int i;
  int left;

  left = (bs->get_byte() << 8) | bs->get_byte();

  if (bs->get_byte() != 8)
    return -14;

  decoder->height = (bs->get_byte() << 8) | bs->get_byte();
  decoder->width  = (bs->get_byte() << 8) | bs->get_byte();

  if ((decoder->height < 1) || (decoder->height > 8192))
    return -15;
  if ((decoder->width < 1) || (decoder->width > 8192))
    return -15;

  decoder->compNb = bs->get_byte();

  if (decoder->compNb > 4)
    return -16;

  if (left != (decoder->compNb * 3 + 8))
    return -17;

  for (i = 0; i < decoder->compNb; i++)
  {
    int compId = bs->get_byte();
    if( compId>4 ) return -13;
    decoder->comps[compId].id = compId;
    int samp = bs->get_byte();
    decoder->comps[compId].horzSampling = samp >> 4;
    decoder->comps[compId].vertSampling = samp & 0xF;
    decoder->comps[compId].quantIdx  = bs->get_byte();
  }

  return 0;
}

// ------------------------------------------------------------------

int jpeg_read_dri_marker(JPGDECODER *decoder,InputBitStream *bs) {

  int left = (bs->get_byte() << 8) | bs->get_byte();
  if (left != 4) return -18;
  decoder->restartInterval = (bs->get_byte() << 8) | bs->get_byte();
  if (decoder->restartInterval) {
    decoder->restartsLeft = decoder->restartInterval;
    decoder->nextRestart = 0;
  }

  return 0;
}

// ------------------------------------------------------------------

int jpeg_process_marker(JPGDECODER *decoder,InputBitStream *bs) {

  int m,err;

  while(1) {

    m = jpeg_next_marker(bs);
    switch (m)
    {
      case 0:
        return -4; // Marker not found

      case M_SOF0:
      case M_SOF1:
        err=jpeg_read_sof_marker(decoder,bs);CHECK_ERR(err);
        break;

      case M_SOS:
        err=jpeg_read_sos_marker(decoder,bs);CHECK_ERR(err);
        return 0;

      case M_DHT:
        err=jpeg_read_dht_marker(decoder,bs);CHECK_ERR(err);
        break;

      case M_DQT:
        err=jpeg_read_dqt_marker(decoder,bs);CHECK_ERR(err);
        break;

      case M_DRI:
        err=jpeg_read_dri_marker(decoder,bs);CHECK_ERR(err);
        break;

      case M_JPG:
      case M_RST0:
      case M_RST1:
      case M_RST2:
      case M_RST3:
      case M_RST4:
      case M_RST5:
      case M_RST6:
      case M_RST7:
      case M_SOI:
        return -3;

      case M_SOF2:
        return -26;

      case M_TEM:
      case M_SOF3:
      case M_SOF5:
      case M_SOF6:
      case M_SOF7:
      case M_SOF9:
      case M_SOF10:
      case M_SOF11:
      case M_SOF13:
      case M_SOF14:
      case M_SOF15:
        return -2;

      case M_EOI:
        break;

      default:
      {
        err=jpeg_skip_marker(bs);CHECK_ERR(err);
        break;
      }

    }
  }

}

// ------------------------------------------------------------------

int jpeg_read_restart_marker(JPGDECODER *decoder,InputBitStream *bs) {

  unsigned char c;
  bs->align();

  // Is it the expected marker? If not, something bad happened.
  c=bs->get_bits(8);
  if( c!=0xFF ) return -23;
  c=bs->get_bits(8);
  if (c != (decoder->nextRestart + M_RST0))
    return -23;

  // Reset each component's DC prediction values.
  for(int i=0;i<4;i++) decoder->comps[i].lastDc = 0;
  decoder->restartsLeft = decoder->restartInterval;
  decoder->nextRestart = (decoder->nextRestart + 1) & 7;

  return 0;
}

// ------------------------------------------------------------------

int jpeg_decoder_init(JPGDECODER *decoder) {

  if( decoder->compNb!=3 && decoder->compNb!=1 )
    return -2;

  if( decoder->compInScan != decoder->compNb )
    return -2;

  // Check tables
  for(int i=0;i<4;i++) {
    int id = decoder->comps[i].id;
    if(id>=0) {
      int qIx = decoder->comps[i].quantIdx;
      if( !decoder->qTables[qIx] ) return -19;
      int dcIdx = decoder->comps[i].dcIdx;
      if( !decoder->hTables[dcIdx].inited ) return -20;
      int acIdx = decoder->comps[i].acIdx;
      if( !decoder->hTables[acIdx].inited ) return -20;
    }
  }

  // Check comp
  int found=0;int fComp=0;
  while(!found && fComp<4) {
    found = (decoder->comps[fComp].id!=-100);
    if(!found) fComp++;
  }
  if(!found) return -25;

  // Init scan type
  if (decoder->compNb == 1)
  {

    decoder->scanType    = JPG_SCAN_GRAYSCALE;
    decoder->outFormat   = JPEG_GRAY_FORMAT; // Output format
    decoder->mcuNbBlock  = 1;
    decoder->compList[0] = fComp;
    decoder->mcuWidth    = 8;
    decoder->mcuHeight   = 8;

  } else if (decoder->compNb == 3) {

    if ( ((decoder->comps[2].horzSampling != 1) || (decoder->comps[2].vertSampling != 1)) ||
         ((decoder->comps[3].horzSampling != 1) || (decoder->comps[3].vertSampling != 1)) )
      return -21;

    if ((decoder->comps[1].horzSampling == 1) && (decoder->comps[1].vertSampling == 1)) {

      decoder->scanType    = JPG_SCAN_YH1V1;
      decoder->outFormat   = JPEG_RGB32_FORMAT; // Output format
      decoder->mcuNbBlock  = 3;
      decoder->compList[0] = fComp;
      decoder->compList[1] = fComp+1;
      decoder->compList[2] = fComp+2;
      decoder->mcuWidth    = 8;
      decoder->mcuHeight   = 8;

    } else if ((decoder->comps[1].horzSampling == 2) && (decoder->comps[1].vertSampling == 1)) {

      //decoder->scanType    = JPG_SCAN_YH2V1;
      //decoder->mcuNbBlock  = 4;
      //decoder->compList[0] = fComp;
      //decoder->compList[1] = fComp;
      //decoder->compList[2] = fComp+1;
      //decoder->compList[3] = fComp+2;
      //decoder->mcuWidth    = 16;
      //decoder->mcuHeight   = 8;
      return -27;

    } else if ((decoder->comps[1].horzSampling == 1) && (decoder->comps[1].vertSampling == 2)) {

      //decoder->scanType    = JPG_SCAN_YH1V2;
      //decoder->mcuNbBlock  = 4;
      //decoder->compList[0] = fComp;
      //decoder->compList[1] = fComp;
      //decoder->compList[2] = fComp+1;
      //decoder->compList[3] = fComp+2;
      //decoder->mcuWidth    = 8;
      //decoder->mcuHeight   = 16;
      return -28;

    } else if ((decoder->comps[1].horzSampling == 2) && (decoder->comps[1].vertSampling == 2)) {

      decoder->scanType    = JPG_SCAN_YH2V2;
      decoder->outFormat   = JPEG_RGB32_FORMAT; // Output format
      decoder->mcuNbBlock  = 6;
      decoder->compList[0] = fComp;
      decoder->compList[1] = fComp;
      decoder->compList[2] = fComp;
      decoder->compList[3] = fComp;
      decoder->compList[4] = fComp+1;
      decoder->compList[5] = fComp+2;
      decoder->mcuWidth    = 16;
      decoder->mcuHeight   = 16;

    } else
      return -21;
  }

  decoder->mcuNbRow = (decoder->width + (decoder->mcuWidth - 1))  / decoder->mcuWidth;
  decoder->mcuNbCol = (decoder->height + (decoder->mcuHeight - 1)) / decoder->mcuHeight;

  return 0;
}

// ------------------------------------------------------------------

void jpeg_decoder_free(JPGDECODER *decoder) {

  for(int i=0;i<4;i++) if( decoder->qTables[i] ) free_16(decoder->qTables[i]);
  if( decoder->blocks   ) free_16( decoder->blocks );
  if( decoder->yccFrame ) free_16( decoder->yccFrame );
  if( decoder->rgbFrame ) free_16( decoder->rgbFrame );

}

// ------------------------------------------------------------------

int jpeg_decode(int jpegSize,unsigned char *jpegData,
                int *width,int *height,int *format,unsigned char **frame) {

  int errCode = 0;
  JPGDECODER decoder;
  memset(&decoder,0,sizeof(JPGDECODER));
  for(int i=0;i<4;i++) decoder.comps[i].id = -100;
  InputBitStream *bs = new InputBitStream(jpegData,jpegSize);
  jpeg_init_color();

  // Header
  errCode = jpeg_read_soi_marker(bs);ERROR(errCode);
  errCode = jpeg_process_marker(&decoder,bs);ERROR(errCode);
  errCode = jpeg_decoder_init(&decoder);ERROR(errCode);

  int nbMCU   = decoder.mcuNbCol * decoder.mcuNbRow;
  int nbBlock = nbMCU * decoder.mcuNbBlock;
  int rWidth  = decoder.mcuNbRow * decoder.mcuWidth;
  int rHeight = decoder.mcuNbCol * decoder.mcuHeight;
  int mcuSize = decoder.mcuNbBlock*64;

  decoder.blocks   = (short *)malloc_16(mcuSize*2);
  decoder.yccFrame = (unsigned char *)malloc_16(mcuSize);
  if( decoder.scanType==JPG_SCAN_GRAYSCALE )
    decoder.rgbFrame = (unsigned char *)malloc_16(rWidth*rHeight);
  else
    decoder.rgbFrame = (unsigned char *)malloc_16(rWidth*rHeight*4);
  if(!decoder.rgbFrame) ERROR(-22);

  // Decode blocks
  bs->init();
  for(int j=0;j<decoder.mcuNbCol;j++) {
    for(int i=0;i<decoder.mcuNbRow;i++) {

      // DRI
      if ((decoder.restartInterval) && (decoder.restartsLeft==0)) {
        errCode = jpeg_read_restart_marker(&decoder,bs);
        ERROR(errCode);
      }
      decoder.restartsLeft--;

      // Decode MCU
      memset(decoder.blocks,0,mcuSize*2);
      errCode = bs->decode_mcu(&decoder);
      ERROR(errCode);

      // Convert to RGB
      int rgbOffset;
      switch( decoder.scanType ) {

        case JPG_SCAN_YH2V2:
          rgbOffset = (i*decoder.mcuWidth + j*decoder.mcuHeight*rWidth) << 2;
#ifdef JPG_USE_ASM
          jpeg_yh2v2_to_rgb32_mmx(decoder.yccFrame,(long)rWidth,decoder.rgbFrame+rgbOffset);
#else
          jpeg_yh2v2_to_rgb32(decoder.yccFrame,rWidth,decoder.rgbFrame+rgbOffset);
#endif
          break;

        case JPG_SCAN_YH1V1:
          rgbOffset = (i*decoder.mcuWidth + j*decoder.mcuHeight*rWidth) << 2;
#ifdef JPG_USE_ASM
          jpeg_yh1v1_to_rgb32_mmx(decoder.yccFrame,(long)rWidth,decoder.rgbFrame+rgbOffset);
#else
          jpeg_yh1v1_to_rgb32(decoder.yccFrame,rWidth,decoder.rgbFrame+rgbOffset);
#endif
          break;

        case JPG_SCAN_GRAYSCALE:
          rgbOffset = (i*decoder.mcuWidth + j*decoder.mcuHeight*rWidth);
          jpeg_y_to_gray(decoder.yccFrame,rWidth,decoder.rgbFrame+rgbOffset);
          break;

      }

    }
  }
  bs->flush();

#ifdef JPG_USE_ASM
#ifdef _WINDOWS
  __asm emms;
#else
  __asm__ ("emms\n"::);  
#endif
#endif


  // Clip and Copy frame
  int rPitch;
  int rowSize;
  int rowSizeL = decoder.width;

  *width = decoder.width;
  *height = decoder.height;
  *format = decoder.outFormat;
  if( decoder.scanType==JPG_SCAN_GRAYSCALE ) {
    *frame = new unsigned char[decoder.width*decoder.height];
    rPitch = rWidth;
    rowSize  = decoder.width;
  } else {
    *frame = new unsigned char[decoder.width*decoder.height*4];
    rPitch = rWidth * 4;
    rowSize  = decoder.width * 4;
  }
  if(!*frame) ERROR(-22);

  unsigned char *src = decoder.rgbFrame;
  unsigned char *dest = *frame;
  for(int y=0;y<decoder.height;y++) {
    memcpy(dest,src,rowSize);
    dest += rowSize;
    src += rPitch;
  }

  jpeg_decoder_free(&decoder);
  delete bs;
  return 0;
}
