///=============================================================================	
//
// file :		jpeg_bitstream.cpp
//
// description :        Simple jpeg coding/decoding library
//                      Bitstream management and huffman coding
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
// Revision 1.3  2009/11/02 08:36:17  taurel
// - Fix warnings reported when compiling using the option -Wall
//
// Revision 1.2  2009/04/20 14:55:58  jlpons
// Added GPL header, changed memory allocation to C++ fashion.
//
//=============================================================================

#include "jpeg_bitstream.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WINDOWS
// Disable "has no EMMS instruction" warning
#pragma warning( disable : 4799 )
#endif

#ifdef _WINDOWS
#ifdef JPG_USE_ASM
// MMX putbits only for Windows
#define JPG_USE_ASM_PB
static unsigned short mmComp[] = { 0xFFFF , 0xFFFF , 0xFFFF , 0xFFFF };
#endif
#endif

extern int jpgZag[];

// ---------------------------------------------------------------------
// Output bitstream
// ----------------------------------------------------------------

OutputBitStream::OutputBitStream() {

  buffer = (unsigned char *)malloc(BUFFER_SIZE);
  buffSize = BUFFER_SIZE;
  nbByte = 0;
  nbBits = 0;
  bits = 0;
  bufferPtr = buffer;

  // numbits table (speed up huffman coding)
  numbits = (unsigned char *)malloc(2048);
  numbits[0]=0;
  numbits[1]=1;
  for(int i=2;i<12;i++) {
    int s = (1L<<(i-1));
    int e = (1L<<(i));
    memset(numbits+s,i,e-s);
  }

}

// ----------------------------------------------------------------
OutputBitStream::~OutputBitStream() {
  if(buffer) free(buffer);
  if(numbits) free(numbits);
}

// ----------------------------------------------------------------

void OutputBitStream::init() {

#ifdef JPG_USE_ASM_PB
  __asm {
    pxor mm1,mm1
    movq mm5,mmComp
  }
#endif

}


void OutputBitStream::flush() {

  align();

#ifdef JPG_USE_ASM_PB

  // Flush mm buffer
  _asm {
    mov   edi,this
    psrlq mm1,32
    movd  ebx,mm1
    mov   dword ptr [edi].bScratch,ebx
    emms
  }

  if(nbBits>=8) { put_byte(bScratch[3]);if(bScratch[3]==0xFF) put_byte(0);nbBits-=8; }
  if(nbBits>=8) { put_byte(bScratch[2]);if(bScratch[2]==0xFF) put_byte(0);nbBits-=8; }
  if(nbBits>=8) { put_byte(bScratch[1]);if(bScratch[1]==0xFF) put_byte(0);nbBits-=8; }
  if(nbBits>=8) { put_byte(bScratch[0]);if(bScratch[0]==0xFF) put_byte(0);nbBits-=8; }

#endif

}

void OutputBitStream::load_mm() {

#ifdef JPG_USE_ASM_PB

  __asm {

    mov     edi,this
    movq    mm2,mm1
    psllq   mm1,32
    psrlq   mm2,32
    movd    ebx,mm2

    // Enough free bytes ?
    mov     eax,[edi].buffSize
    sub     eax,[edi].nbByte
    cmp     eax,4
    jle     _pbload8

    // Any 0xFF in the stream ?
    pcmpeqb mm2,mm5
    movd    eax,mm2
    or      eax,eax
    jz      _pbload32

_pbload8:

    mov dword ptr [edi].bScratch,ebx

  }

  put_byte(bScratch[3]);if(bScratch[3]==0xFF) put_byte(0);
  put_byte(bScratch[2]);if(bScratch[2]==0xFF) put_byte(0);
  put_byte(bScratch[1]);if(bScratch[1]==0xFF) put_byte(0);
  put_byte(bScratch[0]);if(bScratch[0]==0xFF) put_byte(0);
  nbBits -= 32;
  return;

_pbload32:

  _asm {
    mov   esi,[edi].bufferPtr
    bswap ebx
    mov   [esi],ebx
  }
  bufferPtr += 4;
  nbByte    += 4;
  nbBits    -= 32;

#endif
}

// ----------------------------------------------------------------
#ifdef _WINDOWS
__forceinline void OutputBitStream::put_bits(int code,int _size) {
#else
inline void OutputBitStream::put_bits(int code,int _size) {
#endif

#ifdef JPG_USE_ASM_PB

  _asm {
    mov  edi,this
    movd mm2,code
    mov  edx,[edi].nbBits
    add  edx,_size
    mov  ebx,64
    mov  [edi].nbBits,edx
    sub  ebx,edx
    movd mm3,ebx
    psllq mm2,mm3
    por   mm1,mm2
  }

  if( nbBits>=32 ) load_mm();

#else

  nbBits += _size;
  code <<= 32 - nbBits;
  bits |= code;

  while( nbBits>=8 ) {
    unsigned char c = (unsigned char)(bits >> 24);
    put_byteI(c);
    if( c==0xFF ) put_byteI(0);
    bits <<= 8;
    nbBits-=8;
  }

#endif

}

// ----------------------------------------------------------------
void OutputBitStream::align() {

  int s   = 8-(nbBits&7);
  int val ((1L << s) - 1);
  put_bits(val,s);

}

// ----------------------------------------------------------------
#ifdef _WINDOWS
__forceinline void OutputBitStream::put_byteI(unsigned char code) {
#else
inline void OutputBitStream::put_byteI(unsigned char code) {
#endif

  if( nbByte==buffSize ) more_byte();
  buffer[nbByte]=code;
  nbByte++;
  bufferPtr++;

}

// ----------------------------------------------------------------
void OutputBitStream::put_byte(unsigned char code) {
  put_byteI(code);
}

// ----------------------------------------------------------------
void OutputBitStream::put_short(unsigned short code) {

  put_byte( code>>8 );
  put_byte( code&0xFF );

}

// ----------------------------------------------------------------
unsigned char *OutputBitStream::get_data() {
  return buffer;
}

// ----------------------------------------------------------------
unsigned long OutputBitStream::get_size() {
  return nbByte;
}

// ----------------------------------------------------------------
void OutputBitStream::more_byte() {

  unsigned char *newBuffer = (unsigned char *)malloc(nbByte + BUFFER_SIZE);
  memcpy(newBuffer,buffer,nbByte);
  free(buffer);
  buffer = newBuffer;
  buffSize += BUFFER_SIZE;
  bufferPtr = buffer + nbByte;

}

// ----------------------------------------------------------------
#define NUMBITS11()               \
                                  \
  if (val < 0) {                  \
    if( val<-2047 ) val=-2047;    \
    numBits = numbits[-val];      \
    val--;                        \
    val &= ((1L << numBits) - 1); \
  } else {                        \
    if( val>2047 ) val=2047;      \
    numBits = numbits[val];       \
  }

#define NUMBITS10()               \
                                  \
  if (val < 0) {                  \
    if( val<-1023 ) val=-1023;    \
    numBits = numbits[-val];      \
    val--;                        \
    val &= ((1L << numBits) - 1); \
  } else {                        \
    if( val>1023 ) val=1023;      \
    numBits = numbits[val];       \
  }

void OutputBitStream::encode_block(short *block,HUFFMANTABLE *hDC,HUFFMANTABLE *hAC,short *lastDc) {

  int   numBits;
  int   i, zero, sym;
  short val;
#ifdef JPG_USE_ASM_PB
  int codeV,codeS;
#endif

  // DC difference (10+1 bits interval)
  val = block[0] - (*lastDc);
  *lastDc = block[0];

  NUMBITS11();  
  put_bits(hDC->huffCode[numBits], hDC->huffSize[numBits]);
  if (numBits) put_bits(val, numBits);

  // AC values
  zero = 0;
  
  for (i=1;i<64;i++) {

    val = block[jpgZag[i]];

    if (val==0) {

      zero++;

    } else {

      // If number of zero >= 16, put run-length-16 code (0xF0)
      while (zero >= 16) {
        put_bits(hAC->huffCode[0xF0],hAC->huffSize[0xF0]);
        zero -= 16;
      }

      NUMBITS10();

#ifdef JPG_USE_ASM_PB
      // MMX put_bits allows up to 32 bit code
      sym = (zero << 4) + numBits;
      codeS = hAC->huffSize[sym] + numBits;
      codeV = hAC->huffCode[sym] << numBits;
      codeV |= ((unsigned int)val);
      put_bits(codeV, codeS);
#else
      sym = (zero << 4) + numBits;
      put_bits(hAC->huffCode[sym], hAC->huffSize[sym]);
      put_bits(val, numBits);
#endif

      zero=0;

    }

  }

  // EOB
  if (zero > 0)
    put_bits(hAC->huffCode[0], hAC->huffSize[0]);

}

// ---------------------------------------------------------------------
// Input bitstream
// ---------------------------------------------------------------------

//------------------------------------------------------------------------------
// Sign extension

static const int extend_test[16] =   /* entry n is 2**(n-1) */
  { 0, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
    0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000 };

static const int extend_offset[16] = /* entry n is (-1 << n) + 1 */
  { 0, ((-1)<<1) + 1, ((-1)<<2) + 1, ((-1)<<3) + 1, ((-1)<<4) + 1,
   ((-1)<<5) + 1, ((-1)<<6) + 1, ((-1)<<7) + 1, ((-1)<<8) + 1,
   ((-1)<<9) + 1, ((-1)<<10) + 1, ((-1)<<11) + 1, ((-1)<<12) + 1,
   ((-1)<<13) + 1, ((-1)<<14) + 1, ((-1)<<15) + 1 };

// Tables are slightly faster with Visual C++
#ifdef _WINDOWS
#define HUFF_EXTEND(x,s) s = ((x) < extend_test[s] ? (x) + extend_offset[s] : (x))
#else
#define HUFF_EXTEND(x,s)  s = ((x) < (1<<((s)-1)) ? (x) + (((-1)<<(s)) + 1) : (x))
#endif

//------------------------------------------------------------------------------
// Retrieve n first bits from the input stream.
#define SHOWBITS(n) (_bits >> (32 - (n)))

//------------------------------------------------------------------------------
// Remove n bits from the input stream.
#define DROPBITS(n) {                       \
  _nbBits -= (n);                           \
  _bits <<= (n);                            \
  while( _nbBits<=16 ) {                    \
    int c1 = get_byte();                    \
    /*Remove stuffed 0 (following 0xFF)*/   \
    if (c1 == 0xFF) {                       \
      int c2 = get_byte();                  \
      if (c2!=0) {_bufferPtr--;_byteRead--;}\
    }                                       \
    _bits |= c1 << (24 - _nbBits);          \
    _nbBits+=8;                             \
  }}

InputBitStream::InputBitStream(unsigned char *buff,int buffSize) {

  _buffer = buff;
  _buffSize = buffSize;
  _nbBits = 0;
  _bits = 0;
  _byteRead = 0;
  _bufferPtr = buff;

}

InputBitStream::~InputBitStream() {
}

void InputBitStream::align() {
  get_bits(_nbBits & 7);
}

void InputBitStream::flush() {

  align();
  // Make stream ready for get_byte()
  if( _nbBits>0 ) {
    if(_nbBits==32) {_bufferPtr-=4;_byteRead-=4;}
    if(_nbBits==24) {_bufferPtr-=3;_byteRead-=3;}
    if(_nbBits==16) {_bufferPtr-=2;_byteRead-=2;}
    if(_nbBits==8)  {_bufferPtr-=1;_byteRead-=1;}
    _nbBits = 0;
  }

}

void InputBitStream::init() {
  get_bits(0);
}

int InputBitStream::get_byte() {

  if( _byteRead<_buffSize ) {
    _byteRead++;
    return *_bufferPtr++;
  } else {
    return 0;
  }

}

int InputBitStream::get_bits(int numbits) {

  int i = SHOWBITS(numbits);
  DROPBITS(numbits);
  return i;

}

// Inverse dct (jpeg_dct.cpp)
void jpeg_idct(short *block,unsigned char *dst);
void jpeg_idct_mmx(short *block,unsigned char *dst);

int InputBitStream::decode_mcu(JPGDECODER *decoder) {

   int r, s, k, c;
   int *clist = decoder->compList;
   short *block = decoder->blocks;
   unsigned char *ycc = decoder->yccFrame;

   for(int i=0;i<decoder->mcuNbBlock;i++) {

    HUFFMANTABLE *hDC = decoder->hTables + decoder->comps[clist[i]].dcIdx;
    HUFFMANTABLE *hAC = decoder->hTables + decoder->comps[clist[i]].acIdx;
    short *qTable = decoder->qTables[decoder->comps[clist[i]].quantIdx];

    // -- DC value --------------------------------

    // Use a tree traversal to find symbol.
    c = SHOWBITS(8);
    if ((s = hDC->lookUp[c]) < 0)
    {
      DROPBITS(8);
      int drop = 0;
      c = ~SHOWBITS(8);
      do {
        drop++;
        s = hAC->tree[~s + ((c&0x80)>>7)];
        c <<= 1;
      } while (s < 0);
      DROPBITS(drop);
    } else
      DROPBITS(hDC->huffSize[s]);

    // Decode
    if (s!= 0) {
      r = get_bits(s);
      HUFF_EXTEND(r, s);
    }

    decoder->comps[clist[i]].lastDc += s;
    s = decoder->comps[clist[i]].lastDc;
    block[0] = (short)s * qTable[0];

    // -- AC values -------------------------------

    for (k = 1; k < 64; k++)
    {
      // Use a tree traversal to find symbol.
      c = SHOWBITS(8);
      if ((s = hAC->lookUp[c]) < 0)
      {
        DROPBITS(8);
        int drop = 0;
        c = ~SHOWBITS(8);
        do {
          drop++;
          s = hAC->tree[~s + ((c&0x80)>>7)];
          c <<= 1;
        } while (s < 0);
        DROPBITS(drop);
      } else
        DROPBITS(hAC->huffSize[s]);

      // Decode
      r = s >> 4;
      s &= 15;
      if (s) {
        k += r;
        if (k>63) return -24; // Decode error
        r = SHOWBITS(s);
        DROPBITS(s);
        HUFF_EXTEND(r, s);
        block[jpgZag[k]] = (short)s * qTable[k];
      } else {
        if (r == 15) {
          k += 15;
        } else {
          break;
        }
      }

    } // end block loop

#ifdef JPG_USE_ASM
    jpeg_idct_mmx(block,ycc);
#else
    jpeg_idct(block,ycc);
#endif

    block += 64;
    ycc += 64;

  }

  return 0;
}

