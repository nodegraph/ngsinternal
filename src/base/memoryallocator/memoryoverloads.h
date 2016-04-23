#pragma once
#include <base/memoryallocator/memoryallocator_export.h>
#include <cstddef>

// Overloaded new.
MEMORYALLOCATOR_EXPORT void* operator new(size_t size, const char *file, int line) throw ();

// Overloaded array new.
MEMORYALLOCATOR_EXPORT void* operator new[](size_t size, const char *file, int line) throw ();

// Overloaded delete. Only gets called if the corresponding custom "new" throws an exception.
MEMORYALLOCATOR_EXPORT void operator delete(void *pointer, const char *file, int line) throw();

// Overloaded array delete. Only gets called if the corresponding custom "new" throws an exception.
MEMORYALLOCATOR_EXPORT void operator delete[](void *pointer, const char *file, int line) throw();

