///=============================================================================	
//
// file :		jpeg_lib.h
//
// description :        Simple jpeg coding/decoding library
//                      MMX optimisation supported for both Visual C++ and gcc
//                      Main library header file
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

#ifndef _JPEGLIBH_
#define _JPEGLIBH_

// ----------------------------------------------------------------------------
// Encode a RGB image to a buffer
// quality ranges in 0(poor), 100(max)
// jpegData is allocated by the function and must be freed by the caller.
// ----------------------------------------------------------------------------

void jpeg_encode_rgb32(int width,int height,unsigned char *rgb32,
                       double quality,int *jpegSize,unsigned char **jpegData);

void jpeg_encode_rgb24(int width,int height,unsigned char *rgb24,
                       double quality,int *jpegSize,unsigned char **jpegData);

void jpeg_encode_gray8(int width,int height,unsigned char *gray8,
                       double quality,int *jpegSize,unsigned char **jpegData);

// ----------------------------------------------------------------------------
// Decode a JPEG image and return error code in case of failure, 0 is returned 
// otherwise. frame is a pointer to a set of 8bit sample (8bit gray scale or 
// 32bit rgb format) which is allocated by the function and must be freed by
// the caller.
// ----------------------------------------------------------------------------

#define JPEG_GRAY_FORMAT  0
#define JPEG_RGB32_FORMAT 1

int  jpeg_decode(int jpegSize,unsigned char *jpegData,
                 int *width,int *height,int *format,unsigned char **frame);

// Return error message
char *jpeg_get_error_msg(int code);

#endif /* _JPEGLIBH_ */
