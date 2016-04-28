#include <base/memorytracker/memoryops.h>
#include <base/memorytracker/memorytracker.h>
#include <cstdlib>

namespace ngs {

// ----------------------------------------------------------------------------
// New overrides.
// Our replacements to override the standard "new" operator.
// ----------------------------------------------------------------------------
void* new_override(size_t size) {
  // If memory tracking enabled.
  if (g_memory_tracker) {
    Permit<MemoryTracker> permit(g_memory_tracker);
    void *pointer = static_cast<void *>(malloc(size));
    g_memory_tracker->record_allocation(permit, pointer, size, MemoryTracker::kExternalFile.c_str(), 0, false);
    return (pointer);
  }
  // If memory tracking is disabled.
  else {
    void *pointer = static_cast<void *>(malloc(size));
    return (pointer);
  }
  return NULL;
}
void* new_array_override(size_t size) {
  // If memory tracking enabled.
  if (g_memory_tracker) {
    Permit<MemoryTracker> permit(g_memory_tracker);
    void *pointer = static_cast<void *>(malloc(size));
    g_memory_tracker->record_allocation(permit, pointer, size, MemoryTracker::kExternalFile.c_str(), 0, true);
    return (pointer);
  }
  // If memory tracking is disabled.
  else {
    void *pointer = static_cast<void *>(malloc(size));
    return (pointer);
  }
  return NULL;
}

// ----------------------------------------------------------------------------
// Delete overrides.
// Our replacements to override the standard delete operator.
// ----------------------------------------------------------------------------
void delete_override(void *pointer) {
  // If memory tracking enabled.
  if (g_memory_tracker) {
    Permit<MemoryTracker> permit(g_memory_tracker);
    g_memory_tracker->remove_allocation(permit, pointer, false);
    free(pointer);
  }
  // If memory tracking is disabled.
  else {
    free(pointer);
  }

}
void delete_array_override(void *pointer) {
  // If memory tracking enabled.
  if (g_memory_tracker) {
    Permit<MemoryTracker> permit(g_memory_tracker);
    g_memory_tracker->remove_allocation(permit, pointer, true);
    free(pointer);
  }
  // If memory tracking is disabled.
  else {
    free(pointer);
  }
}

// ----------------------------------------------------------------------------
// New overloads.
// Our replacements for the "new" operator with file and line arguments.
// ----------------------------------------------------------------------------
void* new_overload(size_t size, const char *file, int line) {
  // If memory tracking enabled.
  if (g_memory_tracker) {
    Permit<MemoryTracker> permit(g_memory_tracker);
    void *pointer = static_cast<void *>(malloc(size));
    g_memory_tracker->record_allocation(permit, pointer, size, file, line, false);
    return (pointer);
  }
  // If memory tracking is disabled.
  else {
    void *pointer = static_cast<void *>(malloc(size));
    return (pointer);
  }
}
void* new_array_overload(size_t size, const char *file, int line) {
  // If memory tracking enabled.
  if (g_memory_tracker) {
    Permit<MemoryTracker> permit(g_memory_tracker);
    void *pointer = static_cast<void *>(malloc(size));
    g_memory_tracker->record_allocation(permit, pointer, size, file, line, true);
    return (pointer);
  }
  // If memory tracking is disabled.
  else {
    void *pointer = static_cast<void *>(malloc(size));
    return (pointer);
  }
}

// ----------------------------------------------------------------------------
// Delete overloads.
// Our replacements for a delete operator with file and line arguments.
// Note this only gets called if the constructor throws an exception on new.
// ----------------------------------------------------------------------------
void delete_overload(void *pointer, const char *file, int line) {
  // Use the overloaded standard delete operator.
  delete_override(pointer);
}
void delete_array_overload(void *pointer, const char *file, int line) {
  // Use the overloaded standard delete operator.
  delete_array_override(pointer);
}

}
