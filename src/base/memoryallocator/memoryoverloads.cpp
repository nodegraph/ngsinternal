#include <base/memoryallocator/memoryoverloads.h>
#include <base/memorytracker/memoryops.h>
#include <iostream>

// Overloaded new.
void* operator new(size_t size, const char *file, int line) throw () {
  return ngs::new_overload(size, file, line);
}

// Overloaded array new.
void* operator new[](size_t size, const char *file, int line) throw () {
  return ngs::new_array_overload(size, file, line);
}

// Overloaded delete. Only gets called if the corresponding custom "new" throws and exception.
void operator delete(void *pointer, const char *file, int line) throw() {
  ngs::delete_overload(pointer, file, line);
}

// Overloaded array delete. Only gets called if the corresponding custom "new" throws and exception.
void operator delete[](void *pointer, const char *file, int line) throw() {
  ngs::delete_array_overload(pointer, file, line);
}


