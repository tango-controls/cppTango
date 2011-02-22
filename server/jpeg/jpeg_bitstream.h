// 
// File:        jpeg_bitstream.h
// Description: Bitstream management and huffman coding
// Program:     Simple jpeg coding/decoding library
// Author:      JL Pons 2009
//

#ifndef _JPEGBITSTREAMH_
#define _JPEGBITSTREAMH_

#include "jpeg_lib.h"
#include "jpeg_const.h"

#define BUFFER_SIZE 524288

// ------------------------------------------------------------

class OutputBitStream {

 public:

    OutputBitStream();
    ~OutputBitStream();

    void align();
    void flush();
    void init();

    unsigned char *get_data();
    unsigned long get_size();

    void put_bits(int code,int _size);
    void put_byte(unsigned char code);
    void put_byteI(unsigned char code);
    void put_short(unsigned short code);
    void encode_block(short *block,HUFFMANTABLE *hDC,HUFFMANTABLE *hAC,short *lastDc);

 private:

   void more_byte();
   void load_mm();

   unsigned char *buffer;
   int            nbByte;
   int            buffSize;
   int            nbBits;
#ifdef _WINDOWS
   unsigned long  bits;
#else
   unsigned int   bits;
#endif
   unsigned char *bufferPtr;
   unsigned char *numbits;
   unsigned char  bScratch[4];

};

// ------------------------------------------------------------

class InputBitStream {

 public:

    InputBitStream(unsigned char *buff,int buffSize);
    ~InputBitStream();

    void align();
    void flush();
    void init();
    int  get_byte();
    int  backward(int l);
    int  get_bits(int numbits);
    int  decode_mcu(JPGDECODER *decoder);

 private:

   unsigned char *_buffer;
   int            _buffSize;
   int            _byteRead;
   int            _nbBits;
#ifdef _WINDOWS
   unsigned long  _bits;
#else
   unsigned int   _bits;
#endif
   unsigned char *_bufferPtr;

};

#endif /* _JPEGBITSTREAMH_ */
