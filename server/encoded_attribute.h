///=============================================================================	
//
// file :		encoded_attribute.h
//
// description :	Include file for the management of Tango::DevEncoded format
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
// Revision 3.3  2009/03/26 16:08:44  jlpons
// Added decoding interface (DevEncoded format)
//
// Revision 3.2  2009/03/20 15:15:31  jlpons
// Fixed inline assembly error on gcc 4 release
//
// Revision 3.1  2009/03/19 17:50:29  jlpons
// Added management of DevEncoded format
//
//
//=============================================================================

#include <tango.h>

#ifndef _ENCODED_ATT_H
#define _ENCODED_ATT_H

namespace Tango
{


/**
 * This class provides method to deal with Tango::DevEncoded attribute format.
 */
 
class EncodedAttribute
{

public:

/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Create a new EncodedAttribute object.
 *
 */
	EncodedAttribute();
//@}

/**@name Destructor
 * Only one desctructor is defined for this class
 */
//@{
/**
 * The attribute desctructor.
 */
	~EncodedAttribute();
//@}

/**@name Image Encoding Methods
 */
//@{

/**
 * Encode a 8 bit grayscale image as JPEG format
 *
 * @param gray8    Array of 8bit gray sample
 * @param width    The image width
 * @param height   The image height
 * @param quality  Quality of JPEG (0=poor quality 100=max quality)
 *
 */
 void encode_jpeg_gray8(unsigned char *gray8,int width,int height,double quality);

/**
 * Encode a 32 bit rgb color image as JPEG format
 *
 * @param rgb32   Array of 32bit RGB sample (RGB0RGB0...)
 * @param width   The image width
 * @param height  The image height
 * @param quality  Quality of JPEG (0=poor quality 100=max quality)
 *
 */
 void encode_jpeg_rgb32(unsigned char *rgb32,int width,int height,double quality);

/**
 * Encode a 24 bit rgb color image as JPEG format
 *
 * @param rgb24   Array of 32bit RGB sample (RGBRGB...)
 * @param width   The image width
 * @param height  The image height
 * @param quality  Quality of JPEG (0=poor quality 100=max quality)
 *
 */
 void encode_jpeg_rgb24(unsigned char *rgb24,int width,int height,double quality);

/**
 * Encode a 8 bit grayscale image (no compression)
 *
 * @param gray8    Array of 8bit gray sample
 * @param width    The image width
 * @param height   The image height
 *
 */
 void encode_gray8(unsigned char *gray8,int width,int height);

 /**
 * Encode a 16 bit grayscale image (no compression)
 *
 * @param gray16   Array of 16bit gray sample
 * @param width    The image width
 * @param height   The image height
 *
 */
 void encode_gray16(unsigned short *gray16,int width,int height);

 /**
 * Encode a 24 bit color image (no compression)
 *
 * @param rgb24    Array of 24bit RGB sample
 * @param width    The image width
 * @param height   The image height
 *
 */
 void encode_rgb24(unsigned char *rgb24,int width,int height);
//@}


/**@name Image Decoding Methods
 */
//@{
 /**
 * Decode a color image (JPEG_RGB or RGB24) and returns a 32 bits RGB image.
 * Throws DevFailed in case of failure.
 *
 * @param attr     DeviceAttribute that contains the image
 * @param width    Width of the image
 * @param height   Height of the image
 * @param rgb32    Image (memory allocated by the function)
 */
 void decode_rgb32(DeviceAttribute *attr,int *width,int *height,unsigned char **rgb32);

 /**
 * Decode a 8 bits grayscale image (JPEG_GRAY8 or GRAY8) and returns a 8 bits gray scale image.
 * Throws DevFailed in case of failure.
 *
 * @param attr     DeviceAttribute that contains the image
 * @param width    Width of the image
 * @param height   Height of the image
 * @param gray8    Image (memory allocated by the function)
 */
 void decode_gray8(DeviceAttribute *attr,int *width,int *height,unsigned char **gray8);

 /**
 * Decode a 16 bits grayscale image (GRAY16) and returns a 16 bits gray scale image.
 * Throws DevFailed in case of failure.
 *
 * @param attr     DeviceAttribute that contains the image
 * @param width    Width of the image
 * @param height   Height of the image
 * @param gray16   Image (memory allocated by the function)
 */
 void decode_gray16(DeviceAttribute *attr,int *width,int *height,unsigned short **gray16);

//@}

 DevUChar  *get_data()   {return (DevUChar *)buffer;}
 DevString *get_format() {return &format;}
 long       get_size()   {return (long)buffSize;}

private:

  unsigned char *buffer;
  int            buffSize;
  char          *format;

};

} // End of Tango namespace

#endif // _ENCODED_ATT_H
