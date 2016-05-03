#include <base/memoryallocator/memoryoverrides.h>
#include <base/memorytracker/memoryops.h>
#include <iostream>


#if (ARCH == ARCH_LINUX) || (ARCH == ARCH_MACOS) || (ARCH == ARCH_IOS) || (ARCH == ARCH_ANDROID)
// Standard new.
void* operator new(size_t size) {
  return ngs::new_override(size);
}
// Standard array new.
void* operator new[](size_t size) {
  return ngs::new_array_override(size);
}
// Standard delete.
void operator delete(void *pointer) throw () {
    ngs::delete_override(pointer);
}
// Standard array delete.
void operator delete[](void *pointer) throw () {
    ngs::delete_array_override(pointer);
}
#else
// Standard new.
void* operator new(size_t size) throw (){
  return ngs::new_override(size);
}
// Standard array new.
void* operator new[](size_t size) throw () {
  return ngs::new_array_override(size);
}
// Standard delete.
void operator delete(void *pointer) throw () {
  ngs::delete_override(pointer);
}
// Standard array delete.
void operator delete[](void *pointer) throw () {
  ngs::delete_array_override(pointer);
}

// Standard no throw new.
void* operator new(size_t size, const _STD nothrow_t&) throw() {
  return ngs::new_override(size);
}
// Standard no throw array new.
void* operator new[](size_t size, const _STD nothrow_t&) throw() {
  return ngs::new_array_override(size);
}
// Standard no throw delete.
void operator delete(void* pointer, const _STD nothrow_t&) throw() {
  ngs::delete_override(pointer);
}
// Standard no throw array delete.
void operator delete[](void* pointer, const _STD nothrow_t&) throw() {
  ngs::delete_array_override (pointer);
}


#endif

