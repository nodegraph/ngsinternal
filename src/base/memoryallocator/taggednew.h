#pragma once

#include "memoryoverloads.h"
#include "memoryoverrides.h"

/*
 TRACK_NEW -- calls the constructor after allocating memory with our overloaded
 "new" operator which tracks allocations.

 TRACK_ALLOC -- only allocates memory.  Use it in situations where you would typically
 just use malloc. Internally however it just uses the overloaded "new" operator, to track
 memory so it can just be deleted like other allocations.

 TRACK_DELETE -- calls our overriden destroy operator after calling the destructor on the pointer.

 TRACK_DELETE_ARRAY -- similar to the above but for arrays.

 Note: -- that all destroys go thru our overriden destroy operators.
 -- but there are different overriden and overloaded news.

 */


#ifdef DEBUG_MEMORY_ALLOCATION
  // Enable memory allocation tracking.
  #define TRACK_NEW new(__FILE__, __LINE__)
  #define TRACK_ALLOC(size_bytes) ::operator new(size_bytes,__FILE__,__LINE__)
  #define TRACK_DELETE(PTR) delete(PTR); PTR=NULL;
  #define TRACK_DELETE2(PTR) delete(PTR);
  #define TRACK_DELETE_ARRAY(PTR) delete[](PTR); PTR=NULL;
#else
  // Disable memory allocation tracking.
  #define TRACK_NEW new
  #define TRACK_ALLOC(size_bytes) ::operator new(size_bytes)
  #define TRACK_DELETE(PTR) delete(PTR); PTR=NULL;
  #define TRACK_DELETE2(PTR) delete(PTR);
  #define TRACK_DELETE_ARRAY(PTR) delete[](PTR); PTR=NULL;
#endif

#define new_ff TRACK_NEW
#define alloc_ff TRACK_ALLOC

#define delete_ff(PTR) TRACK_DELETE(PTR)
#define delete2_ff(PTR) TRACK_DELETE2(PTR)
#define delete_array_ff TRACK_DELETE_ARRAY

