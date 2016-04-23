#include <base/memorytracker/memorytracker_export.h>
#include <cstddef>

namespace ngs {

// New overrides.
MEMORYTRACKER_EXPORT void* new_override(size_t size);
MEMORYTRACKER_EXPORT void* new_array_override(size_t size);

// Delete overrides.
MEMORYTRACKER_EXPORT void delete_override(void *pointer);
MEMORYTRACKER_EXPORT void delete_array_override(void *pointer);

// New overloads.
MEMORYTRACKER_EXPORT void* new_overload(size_t size, const char *file, int line);
MEMORYTRACKER_EXPORT void* new_array_overload(size_t size, const char *file, int line);

// Delete overloads.
MEMORYTRACKER_EXPORT void delete_overload(void *pointer, const char *file, int line);
MEMORYTRACKER_EXPORT void delete_array_overload(void *pointer, const char *file, int line);

}
