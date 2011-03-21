///=============================================================================	
//
// file :		jpeg_memory.h
//
// description :        Simple jpeg coding/decoding library
//                      Small memory manager (16 bytes aligned for MMX/SSE code)
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


#ifndef _JPEGMEMORYH_
#define _JPEGMEMORYH_

#include <stdlib.h>

// Allocate a buffer (aligned on 16 bytes boundary) 
void *malloc_16(size_t size);

// Allocate a buffer (aligned on 16 bytes boundary) 
void *calloc_16(size_t count,size_t size);

// Free a buffer
void free_16(void *ptr);

// Free all allocated buffer
void free_all_16();

//Return the size of a buffer previoulsy allocated by malloc_16
size_t _msize_16( void *ptr );

#endif /* _JPEGMEMORYH_ */
