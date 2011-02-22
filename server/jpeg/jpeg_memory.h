// 
// File:        jpeg_memory.h
// Description: Small memory manager (16 bytes aligned for MMX/SSE code)
// Program:     Simple jpeg coding/decoding library
// Author:      JL Pons 2009
//

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
