#pragma once
#include <base/utils/utils_export.h>

#include <mutex>
#include <vector>
#include <set>

namespace ngs {

template <class T> class Permit;

// Reserves the smallest positive unsigned integer for you to use as an id.
// This is often used to eliminate the time spent hashing, and looking through buckets.
// We currently generate 32 bit integers because these ids are sent over to the gpu as vertex attributes.
class UTILS_EXPORT IDGenerator {
 public:
  IDGenerator();
  ~IDGenerator();

  // Lockable.
  void lock() {
    _mutex.lock();
  }
  void unlock() {
    _mutex.unlock();
  }

  unsigned int create_id(Permit<IDGenerator>& holder);
  void destroy_id(Permit<IDGenerator>& holder,unsigned int);

 private:
  void check_permit(Permit<IDGenerator>& holder);

  // num_ids is the number of ids in play.
  // It is one greater that the max id.
  unsigned int _num_ids;

  // Note some id's may have destroyed.
  // They are tracked here so that they can be reused.
  std::vector<unsigned int> _destroyed; // recently destroyed

  // Locking.
  std::recursive_mutex _mutex;
};

}
