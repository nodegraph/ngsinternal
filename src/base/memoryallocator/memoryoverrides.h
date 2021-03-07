#pragma once
#include <base/memoryallocator/memoryallocator_export.h>

#include <stddef.h>
#include <new>


#if (ARCH == ARCH_LINUX) || (ARCH == ARCH_MACOS) || (ARCH == ARCH_IOS) || (ARCH == ARCH_ANDROID)

// Standard new.
void* operator new(size_t size);
// Standard array new.
void* operator new[](size_t size);
// Standard delete.
void operator delete(void *pointer) throw ();
// Standard array delete.
void operator delete[](void *pointer) throw ();

#else

// Standard new.
void* operator new(size_t size) throw ();
// Standard array new.
void* operator new[](size_t size) throw ();
// Standard delete.
void operator delete(void *pointer) throw ();
// Standard array delete.
void operator delete[](void *pointer) throw ();

// The nothrow versions.
void* operator new(size_t size, const _STD nothrow_t&) throw();
void* operator new[](size_t size, const _STD nothrow_t&) throw();
void operator delete(void *, const _STD nothrow_t&) throw();
void operator delete[](void *, const _STD nothrow_t&) throw();

//void* malloc(size_t size);
//void free(void* memblock);

#endif
