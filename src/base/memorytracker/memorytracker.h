#pragma once

//#define WINVER 0x0501
//#define _WIN32_WINNT 0x0501

#include <base/memorytracker/memorytracker_export.h>
#include <base/utils/permit.h>

#include <unordered_map>
#include <thread>
#include <mutex>
#include <string>

namespace ngs {

// Stores debug information about any "new" memory allocations.
struct AllocationInfo {
  AllocationInfo();
  AllocationInfo(size_t ordering, void* pointer, size_t alloc_size, bool is_rray);
  AllocationInfo(size_t ordering, void* pointer, size_t alloc_size, bool is_array, const std::string& filename, int line_number);
  AllocationInfo(size_t ordering, void* pointer, size_t alloc_size, bool is_array, const std::string& filename, int line_number, const std::string& call_stack);

  bool operator<(const AllocationInfo& right) const;

  size_t _ordering;
  void* _pointer;

  size_t _alloc_size;
  std::string _filename;
  std::string _call_stack;
  int _line_number;
  bool _is_array;
};


class MEMORYTRACKER_EXPORT MemoryTracker {
 public:
  MemoryTracker();
  virtual ~MemoryTracker();

  // A dummy name used for memory allocations in external libraries.
  static const std::string kExternalFile;

  // Implement the Lockable interface.
  void lock() {
    _mutex.lock();
  }
  void unlock() {
    _mutex.unlock();
  }

  // Recording is disable while manipulating objects in memory to record the allocation info.
  void start(Permit<MemoryTracker>& permit);
  void stop(Permit<MemoryTracker>& permit);

  // Memory Allocations.
  void record_allocation(Permit<MemoryTracker>& permit, void* pointer, size_t size, const char* filename, unsigned int line_number, bool is_array);
  void remove_allocation(Permit<MemoryTracker>& permit, void* pointer, bool is_array);

  // Debugging Routines.
  size_t get_num_allocations(Permit<MemoryTracker>& permit);
  void dump_allocations(Permit<MemoryTracker>& permit);

 private:

  void check_permit(Permit<MemoryTracker>& permit);

  // Helper class to temporarily disable recording while modifying
  // objects to record or remove allocation info.
  class StopRecording {
   public:
    StopRecording(Permit<MemoryTracker>& permit, MemoryTracker* tracker):_tracker(tracker), _permit(&permit){
      _tracker->stop(*_permit);
    }
    ~StopRecording() {
      _tracker->start(*_permit);
    }
    MemoryTracker* _tracker;
    Permit<MemoryTracker>* _permit;
  };

  // Private Helpers.
  bool is_recording();
  void clear() {_pointer_to_info.clear();}
  bool allocation_exists(void* pointer);
  void dump_allocation(void* pointer);

  // Information about our memory allocations.
  typedef std::unordered_map<void*, AllocationInfo> PointerToInfo;
  PointerToInfo _pointer_to_info;

  // Whether we are enabled or not.
  bool _recording;

  // Memory allocations are tagged with a number.
  size_t _ordering;

  // Clients must hold the mutex before using this object.
  static std::recursive_mutex _mutex;
};

// Our global memory tracker.
extern MEMORYTRACKER_EXPORT MemoryTracker* g_memory_tracker;
}

