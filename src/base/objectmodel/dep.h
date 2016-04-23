#pragma once
#include <base/objectmodel/component.h>
#include <base/objectmodel/deplink.h>

namespace ngs {

template<class T>
class Dep: public DepLinkHolder {
 public:
  // If the fixed dependant is null, we won't actually be connected to the dependency.

  template<class D>
  Dep(const Dep<D>& other)
      : _fixed_dependant(other._fixed_dependant),
        _link(other._link) {
    // Make sure that type D is a more derived type of T.
    // This will catch inappropriate types at compile time.
    T* p = (D*)0;
  }

  template<class D>
  Dep(const Dep<D>& other, bool enable_cast)
      : _fixed_dependant(other._fixed_dependant),
        _link(other._link) {
    // Note that it doesn't matter whether enable_cast is true or false.
    // This will always do a reinterpret-style case.
  }

  Dep(Component* dependant, Component* dependency)
      : _fixed_dependant(dependant),
        _link() {
  }
  Dep(Component* dependant)
      : _fixed_dependant(dependant),
        _link() {
  }
  Dep(DepLinkPtr& ptr):
    _fixed_dependant(ptr?ptr->dependant:NULL),
    _link(ptr){
  }
  Dep(DepLinkPtr ptr):
    _fixed_dependant(ptr?ptr->dependant:NULL),
    _link(ptr){
  }
  Dep(const Dep<T>& other)
      : _fixed_dependant(other._fixed_dependant),
        _link(other._link) {
  }
  virtual ~Dep() {
  }

  T* get() const {
    // Return the dependency from the link.
    if (_link) {
      size_t id =T::kIID();
      return static_cast<T*>(_link->dependency);
    }
    return NULL;
  }

  virtual const DepLinkPtr& get_link() const {
    return _link;
  }

  void reset() {
    // This doesn't nullify the dependency in the existing DepLink.
    // It just ignores the existing DepLink, and resets the _link shared_ptr member.
    // This is because other Deps may be using the dependency.
    // Only the Component can set the dependencies to null when it is
    // removing dependencies. Other than that when all the shared_ptrs
    // are destroyed the link will get destroyed, along with its
    // record in the Component object.
    _link.reset();
  }

  Dep& operator=(const Dep<T>& other) {
    // If other has non null dependency.
    if (other) {
      // If the two dependants match, our dependant is already connected to the dependency.
      // If our dependant is null, we don't to actually connect to the dependency.
      // So in these cases we just copy the link.
      if ((_fixed_dependant == other._link->dependant) || (_fixed_dependant == NULL)) {
        _link = other._link;
      } else {
        // Otherwise, create a new link.
        // We dynamically get the interface id (iid) in order to allow us to predeclare
        // the T type when declaring Dep<T> members in a class declaration (header).
        size_t iid = other._link->dependency->get_iid();
        _link = _fixed_dependant->get_dep(other._link->dependency->our_entity(), iid);
      }
    } else {
      // Otherwise other has a null dependency. So make sure we do as well.
      reset();
    }
    return *this;
  }

  T* operator->() const {
    if (_link) {
      return static_cast<T*>(_link->dependency);
    }
    return NULL;
  }

  operator bool() const {
    if (_link && _link->dependency) {
      return true;
    }
    return false;
  }

  // Operators customized for stl containers.

  bool operator<(const Dep<T>& other) const {
    return (_link->dependency_hash < other._link->dependency_hash);
  }

  bool operator==(const Dep<T>& other) const {
    if ((!_link) && (!other._link)) {
      return true;
    }
    if (!_link) {
      return false;
    }
    if (!other._link) {
      return false;
    }
    return (_link->dependency_hash == other._link->dependency_hash);
  }

 protected:
  Component* const _fixed_dependant;
  DepLinkPtr _link; // The duplicate dependency in the link can change to null, so this cannot be used as a hash.


  template <class B>
  friend struct DepHash;

  template <class D>
  friend class Dep;

  template <class D>
  friend class FixedDepWrapper;

  template <class D>
  friend class DynamicDepWrapper;
};

template <class T>
struct DepHash {
   size_t operator() (const Dep<T> &x) const {
     return reinterpret_cast<size_t>(x._link->dependency_hash);
   }
};

}
