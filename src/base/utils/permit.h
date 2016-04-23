#pragma once
#include <base/utils/utils_export.h>

namespace ngs {

template <class LOCKABLE>
class Permit  {
public:
  explicit Permit(LOCKABLE* lockable)
      : _lockable(lockable) {
    _lockable->lock();
  }

  ~Permit() {
    _lockable->unlock();
  }

  bool is_for(LOCKABLE* lockable) const {
    return (lockable==_lockable);
  }

  LOCKABLE* get_lockable() {
    return _lockable;
  }

private:
    Permit();
    Permit(Permit const&);
    Permit& operator=(Permit const&);

    LOCKABLE* _lockable;
};



}
