///=============================================================================
//
// file :		encoded_attribute.cpp
//
// description :	Management of Tango::DevEncoded format
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

#include <encoded_attribute.h>
#include <jpeg/jpeg_lib.h>

using namespace Tango;

#define SAFE_FREE(x) if(x) {free(x);x=NULL;}

// ----------------------------------------------------------------------------

EncodedAttribute::EncodedAttribute():manage_exclusion(false),ext(Tango_NullPtr) {

  buffer_array = (unsigned char **)calloc(1,sizeof(unsigned char *));
  buffer_array[0] = NULL;
  buffSize_array = (int *)calloc(1,sizeof(int));
  buffSize_array[0] = 0;
  format = NULL;
  mutex_array = NULL;
  index = 0;
  buf_elt_nb = 1;
}

EncodedAttribute::EncodedAttribute(int si,bool excl):manage_exclusion(excl),ext(Tango_NullPtr) {

  buffer_array = (unsigned char **)calloc(si,sizeof(unsigned char *));
  buffSize_array = (int *)calloc(si,sizeof(int));
  for (int i = 0;i < si;i++)
  {
     buffer_array[i] = NULL;
	 buffSize_array[i] = 0;
  }
  format = NULL;
  index = 0;
  buf_elt_nb = si;

  if (manage_exclusion == true)
  	mutex_array = new omni_mutex[si];
}

// ----------------------------------------------------------------------------

EncodedAttribute::~EncodedAttribute() {

  for (int i = 0;i < buf_elt_nb;i++)
  	SAFE_FREE(buffer_array[i]);
  SAFE_FREE(buffer_array);
  SAFE_FREE(buffSize_array);

  if (mutex_array != NULL)
    delete [] mutex_array;
}

// ----------------------------------------------------------------------------

void EncodedAttribute::encode_jpeg_gray8(unsigned char *gray8,int width,int height,double quality) {

  if (manage_exclusion == true)
  	mutex_array[index].lock();

  SAFE_FREE(buffer_array[index]);
  buffSize_array[index] = 0;
  format = (char *)JPEG_GRAY8;
  jpeg_encode_gray8(width,height,gray8,quality,&(buffSize_array[index]),&(buffer_array[index]));
  INC_INDEX()
}

// ----------------------------------------------------------------------------

void EncodedAttribute::encode_jpeg_rgb32(unsigned char *rgb32,int width,int height,double quality) {

  if (manage_exclusion == true)
  	mutex_array[index].lock();

  SAFE_FREE(buffer_array[index]);
  buffSize_array[index] = 0;
  format = (char *)JPEG_RGB;
  jpeg_encode_rgb32(width,height,rgb32,quality,&(buffSize_array[index]),&(buffer_array[index]));
  INC_INDEX()
}

// ----------------------------------------------------------------------------

void EncodedAttribute::encode_jpeg_rgb24(unsigned char *rgb24,int width,int height,double quality) {

  if (manage_exclusion == true)
  	mutex_array[index].lock();

  SAFE_FREE(buffer_array[index]);
  buffSize_array[index] = 0;
  format = (char *)JPEG_RGB;
  jpeg_encode_rgb24(width,height,rgb24,quality,&(buffSize_array[index]),&(buffer_array[index]));
  INC_INDEX()
}

// ----------------------------------------------------------------------------

void EncodedAttribute::encode_gray8(unsigned char *gray8,int width,int height) {

  int newSize = width*height + 4;

  if (manage_exclusion == true)
  	mutex_array[index].lock();

  if( newSize!=buffSize_array[index] ) {
    SAFE_FREE(buffer_array[index]);
    buffer_array[index] = (unsigned char *)malloc(newSize);
    buffSize_array[index] = newSize;
  }

  format = (char *)GRAY8;

  // Store image dimension (big endian)
  unsigned char *tmp_ptr = buffer_array[index];
  tmp_ptr[0] = (unsigned char)( (width>>8) & 0xFF );
  tmp_ptr[1] = (unsigned char)( width & 0xFF );
  tmp_ptr[2] = (unsigned char)( (height>>8) & 0xFF );
  tmp_ptr[3] = (unsigned char)( height & 0xFF );

  // Copy image
  memcpy(tmp_ptr+4,gray8,newSize-4);
  INC_INDEX()
}

// ----------------------------------------------------------------------------

void EncodedAttribute::encode_gray16(unsigned short *gray16,int width,int height) {

  int newSize = width*height*2 + 4;

  if (manage_exclusion == true)
  	mutex_array[index].lock();

  if( newSize!=buffSize_array[index] ) {
    SAFE_FREE(buffer_array[index]);
    buffer_array[index] = (unsigned char *)malloc(newSize);
    buffSize_array[index] = newSize;
  }

  format = (char *)GRAY16;

  // Store image dimension (big endian)
  unsigned char *tmp_ptr = buffer_array[index];
  tmp_ptr[0] = (unsigned char)( (width>>8) & 0xFF );
  tmp_ptr[1] = (unsigned char)( width & 0xFF );
  tmp_ptr[2] = (unsigned char)( (height>>8) & 0xFF );
  tmp_ptr[3] = (unsigned char)( height & 0xFF );

  // Store image (big endian)
  int srcIdx = 0;
  int dstIdx = 4;
  for(int j=0;j<height;j++) {
    for(int i=0;i<width;i++) {
      unsigned short s = gray16[srcIdx++];
      tmp_ptr[dstIdx++] = (unsigned char)(s >> 8);
      tmp_ptr[dstIdx++] = (unsigned char)(s & 0xFF);
    }
  }
  INC_INDEX()
}

// ----------------------------------------------------------------------------

void EncodedAttribute::encode_rgb24(unsigned char *rgb24,int width,int height) {

  int newSize = width*height*3 + 4;

  if (manage_exclusion == true)
  	mutex_array[index].lock();

  if( newSize!=buffSize_array[index] ) {
    SAFE_FREE(buffer_array[index]);
    buffer_array[index] = (unsigned char *)malloc(newSize);
    buffSize_array[index] = newSize;
  }

  format = (char *)RGB24;

  // Store image dimension (big endian)
  unsigned char *tmp_ptr = buffer_array[index];
  tmp_ptr[0] = (unsigned char)( (width>>8) & 0xFF );
  tmp_ptr[1] = (unsigned char)( width & 0xFF );
  tmp_ptr[2] = (unsigned char)( (height>>8) & 0xFF );
  tmp_ptr[3] = (unsigned char)( height & 0xFF );

  // Copy image
  memcpy(tmp_ptr+4,rgb24,newSize-4);

}

// ----------------------------------------------------------------------------

void EncodedAttribute::decode_rgb32(DeviceAttribute *attr,int *width,int *height,unsigned char **rgb32)
{
 	if (attr->is_empty())
	{
    	Except::throw_exception((const char *)"API_WrongFormat",
                            	(const char *)"Attribute contains no data",
                            	(const char *)"EncodedAttribute::decode_gray8");
	}

    DevVarEncodedArray_var &encDataSeq = attr->get_Encoded_data();
	if (encDataSeq.operator->() == NULL)
	{
    	ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
      			(const char*)"Cannot extract, data in DeviceAttribute object is not DevEncoded",
      			(const char*)"EncodedAttribute::decode_gray8");
	}

	string local_format(encDataSeq.in()[0].encoded_format);

	int isRGB  = (strcmp(local_format.c_str() ,RGB24 ) == 0);
	int isJPEG = (strcmp(local_format.c_str() ,JPEG_RGB ) == 0);

	if( !isRGB && !isJPEG )
	{
		Except::throw_exception((const char *)"API_WrongFormat",
                            	(const char *)"Not a color format",
                            	(const char *)"EncodedAttribute::decode_rgb32");
	}

	unsigned char *rawBuff = NULL;
	int size = -1;

    DevVarEncodedArray &encData = encDataSeq.inout();
    DevVarCharArray &encBuff = encData[0].encoded_data;
    size = encBuff.length();
    rawBuff = encBuff.get_buffer(false);

	if( isRGB )
	{

    	// Get width and height
    	int wh = ((int)rawBuff[0] & 0xFF);
    	int wl = ((int)rawBuff[1] & 0xFF);
    	wh = wh << 8;
    	int iWidth = wh | wl;

    	int hh = ((int)rawBuff[2] & 0xFF);
    	int hl = ((int)rawBuff[3] & 0xFF);
    	hh = hh << 8;
    	int iHeight = hh | hl;

    	unsigned char *data = new unsigned char[iWidth*iHeight*4];

    	// Convert to RGB32
    	int srcIdx = 4;
    	int dstIdx = 0;
    	for(int j=0;j<iHeight;j++)
		{
      		for(int i=0;i<iWidth;i++)
			{
        		data[dstIdx++] = rawBuff[srcIdx++]; // R
        		data[dstIdx++] = rawBuff[srcIdx++]; // G
        		data[dstIdx++] = rawBuff[srcIdx++]; // B
        		data[dstIdx++] = 0;
      		}
    	}

		*rgb32  = data;
		*width  = iWidth;
		*height = iHeight;

		return;
	}

	if( isJPEG )
	{
		int jFormat;
		int err = jpeg_decode(size,&(rawBuff[0]),width,height,&jFormat,rgb32);
		if(err)
		{
	    	TangoSys_OMemStream o;
			o << jpeg_get_error_msg(err);
	    	Except::throw_exception((const char *)"API_DecodeErr",
                              	o.str(),
                              (const char *)"EncodedAttribute::decode_rgb32");
		}

		if( jFormat==JPEG_GRAY_FORMAT )
		{
      		// Should not happen
      		Except::throw_exception((const char *)"API_WrongFormat",
                              		(const char *)"Not a color format",
                              		(const char *)"EncodedAttribute::decode_rgb32");
		}

		return;
	}
}

// ----------------------------------------------------------------------------

void EncodedAttribute::decode_gray8(DeviceAttribute *attr,int *width,int *height,unsigned char **gray8)
{

 	if (attr->is_empty())
	{
    	Except::throw_exception((const char *)"API_WrongFormat",
                            	(const char *)"Attribute contains no data",
                            	(const char *)"EncodedAttribute::decode_gray8");
	}

	DevVarEncodedArray_var &encDataSeq = attr->get_Encoded_data();
	if (encDataSeq.operator->() == NULL)
	{
    	ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
      			(const char*)"Cannot extract, data in DeviceAttribute object is not DevEncoded",
      			(const char*)"EncodedAttribute::decode_gray8");
	}

	string local_format(encDataSeq.in()[0].encoded_format);

	int isGrey  = (strcmp(local_format.c_str() ,GRAY8 ) == 0);
	int isJPEG = (strcmp(local_format.c_str() ,JPEG_GRAY8 ) == 0);

	if( !isGrey && !isJPEG )
	{
		Except::throw_exception((const char *)"API_WrongFormat",
                            	(const char *)"Not a grayscale 8bit format",
                            	(const char *)"EncodedAttribute::decode_gray8");
	}

	unsigned char *rawBuff = NULL;
	int size = -1;

    DevVarEncodedArray &encData = encDataSeq.inout();
    DevVarCharArray &encBuff = encData[0].encoded_data;
    size = encBuff.length();
    rawBuff = encBuff.get_buffer(false);

	if( isGrey )
	{

    	// Get width and height
    	int wh = ((int)rawBuff[0] & 0xFF);
    	int wl = ((int)rawBuff[1] & 0xFF);
    	wh = wh << 8;
    	int iWidth = wh | wl;

    	int hh = ((int)rawBuff[2] & 0xFF);
    	int hl = ((int)rawBuff[3] & 0xFF);
    	hh = hh << 8;
    	int iHeight = hh | hl;

    	unsigned char *data = new unsigned char[iWidth*iHeight];
    	memcpy(data,&(rawBuff[4]),iWidth*iHeight);

    	*gray8  = data;
    	*width  = iWidth;
    	*height = iHeight;

    	return;
	}

	if( isJPEG )
	{
    	int jFormat;
    	int err = jpeg_decode(size,&(rawBuff[0]),width,height,&jFormat,gray8);
    	if(err)
		{
	    	TangoSys_OMemStream o;
			o << jpeg_get_error_msg(err);
	    	Except::throw_exception((const char *)"API_DecodeErr",
                              		o.str(),
                              		(const char *)"EncodedAttribute::decode_gray8");

    	}

    	if( jFormat!=JPEG_GRAY_FORMAT )
		{
      // Should not happen
      		Except::throw_exception((const char *)"API_WrongFormat",
                              (const char *)"Not a grayscale 8bit format",
                              (const char *)"EncodedAttribute::decode_gray8");
    	}

		return;
	}

}

// ----------------------------------------------------------------------------

void EncodedAttribute::decode_gray16(DeviceAttribute *attr,int *width,int *height,unsigned short **gray16)
{

 	if (attr->is_empty())
	{
    	Except::throw_exception((const char *)"API_WrongFormat",
                            	(const char *)"Attribute contains no data",
                            	(const char *)"EncodedAttribute::decode_gray16");
	}

	DevVarEncodedArray_var &encDataSeq = attr->get_Encoded_data();
	if (encDataSeq.operator->() == NULL)
	{
    	ApiDataExcept::throw_exception((const char*)"API_IncompatibleAttrArgumentType",
      			(const char*)"Cannot extract, data in DeviceAttribute object is not DevEncoded",
      			(const char*)"EncodedAttribute::decode_gray16");
	}

	string local_format(encDataSeq.in()[0].encoded_format);

	int isGrey  = (strcmp(local_format.c_str() ,GRAY16 ) == 0);

	if( !isGrey )
	{
    	Except::throw_exception((const char *)"API_WrongFormat",
                            	(const char *)"Not a grayscale 16 bits format",
                            	(const char *)"EncodedAttribute::decode_gray16");
	}

	unsigned char *rawBuff = NULL;

    DevVarEncodedArray &encData = encDataSeq.inout();
    DevVarCharArray &encBuff = encData[0].encoded_data;
    rawBuff = encBuff.get_buffer(false);

	if( isGrey )
	{

    	// Get width and height
    	int wh = ((int)rawBuff[0] & 0xFF);
    	int wl = ((int)rawBuff[1] & 0xFF);
    	wh = wh << 8;
    	int iWidth = wh | wl;

    	int hh = ((int)rawBuff[2] & 0xFF);
    	int hl = ((int)rawBuff[3] & 0xFF);
    	hh = hh << 8;
    	int iHeight = hh | hl;

    	unsigned short *data = new unsigned short[ iWidth*iHeight*2 ];

    	int srcIdx = 4;
    	int dstIdx = 0;
    	for(int j=0;j<iHeight;j++)
		{
      		for(int i=0;i<iWidth;i++)
			{
        		unsigned short hh = ((unsigned short)rawBuff[srcIdx++] & 0xFF);
        		unsigned short hl = ((unsigned short)rawBuff[srcIdx++] & 0xFF);
        		data[dstIdx++] = (hh << 8) | hl;
      		}
    	}

    	*gray16 = data;
    	*width  = iWidth;
    	*height = iHeight;
  	}
}
