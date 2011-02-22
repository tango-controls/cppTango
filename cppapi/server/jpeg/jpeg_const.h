///=============================================================================	
//
// file :		jpeg_const.h
//
// description :        Simple jpeg coding/decoding library
//                      Various constants and internal type definitions
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
// Revision 1.2  2009/04/20 14:55:58  jlpons
// Added GPL header, changed memory allocation to C++ fashion.
//
//=============================================================================

#ifndef _JPEGCONSTH_
#define _JPEGCONSTH_

// Huffman table structure

typedef struct {

  unsigned char *huffBits;       // Number of Huffman codes per bit size
  unsigned char *huffVal;        // Huffman codes per bit size
  unsigned char  huffSize[256];  // Huffman codes size
  unsigned int   huffCode[256];  // Huffman codes
  unsigned int   lookUp[256];    // look up table (decoding purpose)
  unsigned int   tree[512];      // Huffman tree (decoding purpose)
  int            inited;         // Initialisation flag (decoding purpose)

} HUFFMANTABLE;

// Components structure

typedef struct {

  int   id;             // Identifier
  int   horzSampling;   // Horizontal sampling factor
  int   vertSampling;   // Vertical sampling factor
  int   quantIdx;       // Quantization table index
  int   dcIdx;          // Huffman table DC index
  int   acIdx;          // Huffman table AC index
  short lastDc;         // Last DC value

} JPGCOMPONENT;

// Decoder structure

typedef struct {

  int   width;         // Image width
  int   height;        // Image height
  int   scanType;      // Type of scan
  int   outFormat;     // Output format
  int   compNb;        // Number of component (frame)
  int   compInScan;    // Number of component (scan)
  int   mcuWidth;      // MCU width
  int   mcuHeight;     // MCU height
  int   mcuNbBlock;    // Number of block per MCU (total)
  int   mcuNbRow;      // Number of MCU per row
  int   mcuNbCol;      // Number of MCU per col
  int   restartInterval; // Restart interval
  int   restartsLeft;    // Number of MCU between DRI marker
  int   nextRestart;     // Next RST marker code
  int   compList[6];     // List of component (scan)

  HUFFMANTABLE hTables[8];  // Huffman tables
  short *qTables[4];        // Quantization tables
  JPGCOMPONENT comps[4];    // Components

  short *blocks;            // Blocks
  unsigned char *yccFrame;  // Decoded frame (RGB24 or GRAY8)
  unsigned char *rgbFrame;  // Decoded frame (RGB24 or GRAY8)

} JPGDECODER;

// JPEG scan type

#define JPG_SCAN_GRAYSCALE 0
#define JPG_SCAN_YH1V1     1
#define JPG_SCAN_YH2V1     2
#define JPG_SCAN_YH1V2     3
#define JPG_SCAN_YH2V2     4

// JPEG marker codes

#define M_SOF0  0xc0
#define M_SOF1  0xc1
#define M_SOF2  0xc2
#define M_SOF3  0xc3
  
#define M_SOF5  0xc5
#define M_SOF6  0xc6
#define M_SOF7  0xc7
  
#define M_JPG   0xc8
#define M_SOF9  0xc9
#define M_SOF10 0xca
#define M_SOF11 0xcb
  
#define M_SOF13 0xcd
#define M_SOF14 0xce
#define M_SOF15 0xcf
  
#define M_DHT   0xc4
  
#define M_DAC   0xcc
  
#define M_RST0  0xd0
#define M_RST1  0xd1
#define M_RST2  0xd2
#define M_RST3  0xd3
#define M_RST4  0xd4
#define M_RST5  0xd5
#define M_RST6  0xd6
#define M_RST7  0xd7
  
#define M_SOI   0xd8
#define M_EOI   0xd9
#define M_SOS   0xda
#define M_DQT   0xdb
#define M_DNL   0xdc
#define M_DRI   0xdd
#define M_DHP   0xde
#define M_EXP   0xdf
  
#define M_APP0  0xe0
#define M_APP1  0xe1
#define M_APP2  0xe2
#define M_APP3  0xe3
#define M_APP4  0xe4
#define M_APP5  0xe5
#define M_APP6  0xe6
#define M_APP7  0xe7
#define M_APP8  0xe8
#define M_APP9  0xe9
#define M_APP10 0xea
#define M_APP11 0xeb
#define M_APP12 0xec
#define M_APP13 0xed
#define M_APP14 0xee
#define M_APP15 0xef
  
#define M_JPG0  0xf0
#define M_JPG13 0xfd
#define M_COM   0xfe
  
#define M_TEM   0x01

#endif /* _JPEGCONSTH_ */
