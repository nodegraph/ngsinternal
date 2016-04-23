#include <base/memoryallocator/taggednew.h>
#include <base/memoryallocator/bootstrap.h>
#include <base/memorytracker/memorytracker.h>

#include <iostream>
#include <cassert>

int main(int argc, char **argv) {
  using namespace ngs;

  bootstrap_memory_tracker();
  Permit<MemoryTracker> permit(g_memory_tracker);

  size_t numAllocations = g_memory_tracker->get_num_allocations(permit);

  // Single new.
  int* alloc = new int;
  numAllocations++;
  std::cerr << "num allocations is: " << numAllocations << "\n";
  std::cerr << "trackers allocation is: " << g_memory_tracker->get_num_allocations(permit) << "\n";
  assert(g_memory_tracker->get_num_allocations(permit) == numAllocations);

  // Single delete.
  delete (alloc);
  numAllocations--;
  assert(g_memory_tracker->get_num_allocations(permit) == numAllocations);

  // Array new.
  alloc = new int[100];
  numAllocations++;
  assert(g_memory_tracker->get_num_allocations(permit) == numAllocations);

  // Array delete.
  delete[] (alloc);
  numAllocations--;
  assert(g_memory_tracker->get_num_allocations(permit) == numAllocations);

  // -------------------------------------------------------------------------------

  // Overridden single new.
  alloc = new_ff int;
  numAllocations++;
  assert(g_memory_tracker->get_num_allocations(permit) == numAllocations);

  // Overridden single delete.
  delete_ff(alloc)
  ;
  assert(alloc==NULL);
  numAllocations--;
  assert(g_memory_tracker->get_num_allocations(permit) == numAllocations);

  // Overridden array new
  alloc = new_ff int[100];
  numAllocations++;
  assert(g_memory_tracker->get_num_allocations(permit) == numAllocations);

  // Overridden array delete.
  delete_array_ff(alloc)
  ;
  assert(alloc==NULL);
  numAllocations--;
  assert(g_memory_tracker->get_num_allocations(permit) == numAllocations);

  // Cleanup.
  shutdown_memory_tracker();
  return 0;
}

