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
//
//=============================================================================

#include <tango.h>

#ifndef _ENCODED_ATT_H
#define _ENCODED_ATT_H

#include <encoded_format.h>

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

/**
 * Create a new EncodedAttribute object with a user defined buffer pool.
 *
 * This constructor allows the user to define the size of the buffer pool used to
 * store the encoded images. This buffer pool is managed as a circular pool.
 * A different buffer is used each time an image is encoded. The last used buffer is then
 * passed to the attribute with the attribute::set_value() method
 *
 * @param buf_pool_size    Buffer pool size
 * @param serialization	   Set to true if the instance manage the data buffer serialization
 *
 */
	EncodedAttribute(int buf_pool_size,bool serialization = false);
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

 DevUChar  *get_data()
 {if (index==0)
    return (DevUChar *)buffer_array[buf_elt_nb-1];
  else
    return (DevUChar *)buffer_array[index-1];}

 long       get_size()
 {if (index==0)
    return (long)buffSize_array[buf_elt_nb-1];
  else
    return (long)buffSize_array[index-1];}

 DevString *get_format() {return &format;}
 bool get_exclusion() {return manage_exclusion;}
 omni_mutex *get_mutex()
 {if (index==0)
 	return &(mutex_array[buf_elt_nb-1]);
  else
    return &(mutex_array[index-1]);}

private:
    class EncodedAttributeExt
    {
    };

    unsigned char 		    **buffer_array;
    int           		    *buffSize_array;
    omni_mutex			    *mutex_array;
    char          		    *format;

    int 					index;
    int					    buf_elt_nb;
    bool					manage_exclusion;

#ifdef HAS_UNIQUE_PTR
    unique_ptr<EncodedAttributeExt>     ext;           // Class extension
#else
    EncodedAttributeExt	                *ext;
#endif

};

#define INC_INDEX() \
  index++; \
  if (index == buf_elt_nb) \
  	index = 0;

} // End of Tango namespace

#endif // _ENCODED_ATT_H
