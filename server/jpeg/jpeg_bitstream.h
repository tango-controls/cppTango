///=============================================================================	
//
// file :		jpeg_bitstream.h
//
// description :        Simple jpeg coding/decoding library
//                      Bitstream management and huffman coding
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
