///=============================================================================	
//
// file :		jpeg_memory.cpp
//
// description :        Simple jpeg coding/decoding library
//                      Small memory manager (16 bytes aligned for MMX/SSE code)
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

#include "jpeg_memory.h"
#include <stdio.h>
#include <string.h>

#define MAX_BUFFER 50

typedef struct {

  void   *buff;
  void   *buff16;
  size_t  size;

} MEMORY_HEADER;

static int nb_allocated = 0;
static MEMORY_HEADER MemTable[MAX_BUFFER];

// Allocate a buffer (always 16 bytes aligned) 
void *malloc_16(size_t size) {

  void *mptr;
  void *mptr16;

  if( nb_allocated == MAX_BUFFER ) {
    printf("Warning: jpeg_memory number of buffer exeeded\n");
    return NULL;
  }

  mptr = malloc(size+16);

  if( !mptr ) {
    printf("Warning: jpeg_memory not enough free memory\n");
    return NULL;
  }

  MemTable[nb_allocated].buff=mptr;
  MemTable[nb_allocated].size=size;

  // Aligns memory block on 16byte
  mptr16 = ((void *)(((size_t)mptr + 15) & ~15));
  MemTable[nb_allocated].buff16=mptr16;
  nb_allocated++;

  return mptr16;
}

// Allocate a buffer (always 16 bytes aligned) 
void *calloc_16(size_t count,size_t size) {

  void *mptr;
  void *mptr16;

  if( nb_allocated == MAX_BUFFER )
    return NULL;

  mptr = malloc(size*count+16);

  if( !mptr ) return NULL;

  MemTable[nb_allocated].buff=mptr;
  MemTable[nb_allocated].size=size;

  // Aligns memory block on 16byte
  mptr16 = ((void *)(((size_t)mptr + 15) & ~15));
  MemTable[nb_allocated].buff16=mptr16;
  nb_allocated++;

  return mptr16;
}

// Free a buffer
void free_16( void *ptr ) {
  
  int i,found;

  if( ptr==NULL ) return;

  i=0;found=0;
  while(!found && i<nb_allocated ) {
    found = (MemTable[i].buff16==ptr);
    if(!found) i++;
  }

  if( found ) {
    free( MemTable[i].buff );
    nb_allocated--;
//    memcpy( &(MemTable[i]) , &(MemTable[i+1]) , sizeof(MEMORY_HEADER)*(nb_allocated-i) );
	for(int j=i;j<nb_allocated;j++) MemTable[j] = MemTable[j+1]; 
  } else {
    printf("jpeg_memory warning: Trying to free unallocated memory");
  }

}

//Return the size of a buffer previoulsy allocated by malloc_16
size_t _msize_16( void *ptr ) {

  int i,found;

  if( ptr==NULL ) return 0;

  i=0;found=0;
  while(!found && i<nb_allocated ) {
    found = (MemTable[i].buff16==ptr);
    if(!found) i++;
  }

  if( found ) {
    return MemTable[i].size;
  } else {
    return 0;
  }

}

// Free all allocated buffer
void free_all_16() {

  for(int i=0;i<nb_allocated;i++)
    free( MemTable[i].buff );
  nb_allocated = 0;

}
