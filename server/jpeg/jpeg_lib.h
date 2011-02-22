// 
// File:        jpeg_lib.h
// Description: Main header file
// Program:     Simple jpeg coding/decoding library (8bits precision)
//              MMX optimisation supported for both Visual C++ and gcc
// Author:      JL Pons 2009
//

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
