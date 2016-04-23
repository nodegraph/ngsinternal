
// Only uncomment this for debugging purposes.
// This slows down the memory tracker by orders of magnitude.
//#define RECORD_CALL_STACKS

#include <base/memorytracker/memorytracker.h>

#ifdef RECORD_CALL_STACKS
#include <base/memorytracker/callstack.h>
#endif

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

namespace ngs {

// Global MemoryTracker.
MemoryTracker* g_memory_tracker=NULL;

// ---------------------------------------------------------------------------
// AllocationInfo.
// ---------------------------------------------------------------------------

AllocationInfo::AllocationInfo()
    : _ordering(0),
      _pointer(0),
      _alloc_size(0),
      _is_array(false),
      _filename(""),
      _line_number(-1),
      _call_stack("") {
}

AllocationInfo::AllocationInfo(size_t ordering, void* pointer, size_t allocSize, bool isArray)
    : _ordering(ordering),
      _pointer(pointer),
      _alloc_size(allocSize),
      _is_array(isArray),
      _filename(""),
      _line_number(-1),
      _call_stack("") {
}
AllocationInfo::AllocationInfo(size_t ordering, void* pointer, size_t allocSize, bool isArray, const std::string& filename, int lineNumber)
    : _ordering(ordering),
      _pointer(pointer),
      _alloc_size(allocSize),
      _is_array(isArray),
      _filename(filename),
      _line_number(lineNumber),
      _call_stack("") {
}
AllocationInfo::AllocationInfo(size_t ordering, void* pointer, size_t allocSize, bool isArray, const std::string& filename, int lineNumber,
                               const std::string& callStack)
    : _ordering(ordering),
      _pointer(pointer),
      _alloc_size(allocSize),
      _is_array(isArray),
      _filename(filename),
      _line_number(lineNumber),
      _call_stack(callStack) {
}

bool AllocationInfo::operator<(const AllocationInfo& right) const {
  return this->_ordering < right._ordering;
}

// ---------------------------------------------------------------------------
// MemoryTracker.
// ---------------------------------------------------------------------------

const std::string MemoryTracker::kExternalFile("_unknown_file_");
std::recursive_mutex MemoryTracker::_mutex;

MemoryTracker::MemoryTracker()
    : _recording(false),_ordering(0) {
}

MemoryTracker::~MemoryTracker() {
}

void MemoryTracker::check_permit(Permit<MemoryTracker>& permit) {
  // Make sure the holder has locked us.
  if (!permit.is_for(this)) {
    assert(false);
  }
}

void MemoryTracker::start(Permit<MemoryTracker>& permit) {
  check_permit(permit);
	_recording = true;
}

void MemoryTracker::stop(Permit<MemoryTracker>& permit) {
  check_permit(permit);
	_recording = false;
}

bool MemoryTracker::is_recording() {
	return _recording;
}

void MemoryTracker::record_allocation(Permit<MemoryTracker>& permit, void* pointer, size_t size, const char* filename, unsigned int line_number,
                                      bool is_array) {
  check_permit(permit);

	if (!is_recording()) {
		return;
	}

	// Turn memory tracking off to avoid infinite recursion.
	StopRecording r(permit,this);
	{
		if (allocation_exists(pointer)) {

		  if(_pointer_to_info[pointer]._filename == MemoryTracker::kExternalFile) {
		    // An external library has likely called free instead of delete on memory it allocated with new.
		  } else {
		    std::cerr << "Error: MemoryTracker detected a duplicate allocation on memory which was already allocated.\n";
	      dump_allocation(pointer);
	      const AllocationInfo& info = _pointer_to_info[pointer];
		    assert(false);
		  }
			return;
		}

		// To halt the program at a certain allocation, uncomment the following.
		//if (mOrdering == 52) {
		//	assert(false);
		//}

#ifdef RECORD_CALL_STACKS
		call_stack stack;
		std::string trace = stack.as_string();

		// Update the allocation info.
		AllocationInfo info(_ordering++,pointer,size, is_array, filename, line_number, trace);
		_pointer_to_info[pointer]=info;
#else
		// Update the allocation info.
		AllocationInfo info(_ordering++, pointer, size, is_array, filename,
				line_number);
		_pointer_to_info[pointer] = info;
#endif

	}
}

void MemoryTracker::remove_allocation(Permit<MemoryTracker>& permit, void* pointer, bool is_array) {
  check_permit(permit);

	if (!is_recording()) {
		return;
	}

	// Turn memory tracking off to avoid infinite recursion.
	StopRecording r(permit, this);
	{

		// Deletion on NULL are fine, ignore them.
		if (pointer == NULL) {
			return;
		}
		// If the pointer doesn't exist it may have been previously deleted.
		// This indicates memory is not being managed properly. Make sure to clean it up.
		// This should only happen from external libraries.
		// External libraries may create an object with new_ but then use free instead of delete_.
		if (!allocation_exists(pointer)) {
//      std::cerr << "\nNodeGraph Warning: MemoryTracker detected a delete on memory which is not currently allocated: " << pointer << "\n";
//      assert(false);
//			call_stack stack;
//			std::string trace = stack.as_string();
//			std::cerr << "Stack Trace:\n";
//			std::cerr << trace << "\n";
			return;
		}

		// Check to see if the array-ness of the deallocation matches the allocation.
		PointerToInfo::iterator iter = _pointer_to_info.find(pointer);
		if (is_array != iter->second._is_array) {
      std::cerr << "\nNodeGraph Warning: MemoryTracker detected and improper delete, which didn't match the array'ness of the allocation.\n";
		}

		// Update the allocation info.
		_pointer_to_info.erase(iter);
	}
}

bool MemoryTracker::allocation_exists(void* pointer) {
  if (_pointer_to_info.count(pointer)) {
    return true;
  }
  return false;
}

void MemoryTracker::dump_allocation(void* pointer) {
  const AllocationInfo& info = _pointer_to_info[pointer];
  std::cerr
      << " pointer: " << pointer
      << " filename: " << info._filename
      << " line: " << info._line_number
      << " is_array: " << info._is_array << "\n";
}

void MemoryTracker::dump_allocations(Permit<MemoryTracker>& permit) {
  check_permit(permit);

	StopRecording r(permit, this);
	{
		//Shove the allocation infos into a vector.
		std::vector<AllocationInfo> infos;
		PointerToInfo::iterator iter = _pointer_to_info.begin();
		PointerToInfo::iterator end = _pointer_to_info.end();
		for (; iter != end; iter++) {
			AllocationInfo& alloc = iter->second;
			infos.push_back(alloc);
		}

		// Now sort them.
		std::sort(infos.begin(), infos.end());

		// Now print it out.
		if (infos.size()) {
			std::cerr << "\n--------------------------------------------------------------\n";
			std::cerr << "Your program is leaking the following memory allocations: \n";
			std::cerr << "These are the leaked allocations from your own custom libraries:\n";
			size_t numExternalAllocs = 0;
			for (size_t i = 0; i < infos.size(); i++) {
				AllocationInfo& alloc = infos[i];
				if (alloc._filename == kExternalFile) {
					++numExternalAllocs;
					continue;
				}
				std::cerr << "Ordering: " << alloc._ordering << " Pointer: "
						<< alloc._pointer << " Filename: " << alloc._filename
						<< " Line: " << alloc._line_number << " Size: "
						<< alloc._alloc_size << "\n";
#ifdef RECORD_CALL_STACKS
				std::cerr
				<< "----------------------------- CALL STACK -----------------------------\n";
				std::cerr << alloc._call_stack;
				std::cerr
				<< "----------------------------------------------------------------------\n";
#endif
			}
			if (numExternalAllocs == infos.size()) {
				std::cerr << " -- no leaks found in your own custom libraries.\n";
			}

			std::cerr << "The number of leaked allocations from external libraries is: " << numExternalAllocs << "\n";
		} else {
			std::cerr << "All memory has been accounted for.\n";
		}

	}
}

size_t MemoryTracker::get_num_allocations(Permit<MemoryTracker>& permit) {
  check_permit(permit);
	return _pointer_to_info.size();
}


}
