#include <base/memoryallocator/bootstrap.h>
#include <base/memorytracker/memorytracker.h>
#include <cassert>

namespace ngs {

#if (BUILD_TYPE == RELEASE_TYPE) || (ARCH == ARCH_ANDROID)

void bootstrap_memory_tracker() {
}

void shutdown_memory_tracker() {
}

#else

void bootstrap_memory_tracker() {
  assert(!g_memory_tracker);
  // Create the memory tracker. *** Memory tracker must use raw new_ as it cannot track itself recursively. ***
  void* mem = malloc(sizeof(MemoryTracker));
  g_memory_tracker = new (mem) MemoryTracker();  // Note don't use new_ff here.
  Permit<MemoryTracker> permit(g_memory_tracker);
  g_memory_tracker->start(permit);
}

void shutdown_memory_tracker() {
  assert(g_memory_tracker);
  Permit<MemoryTracker> permit(g_memory_tracker);
  g_memory_tracker->dump_allocations(permit);
  g_memory_tracker->stop(permit);
  g_memory_tracker->~MemoryTracker();
  free(g_memory_tracker);
  g_memory_tracker = NULL;
}

#endif

}
