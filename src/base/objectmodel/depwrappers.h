#pragma once
#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/dep.h>
#include <base/objectmodel/entity.h>
#include <base/objectmodel/patheddeps.h>


#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>

#include <base/memoryallocator/taggednew.h>

namespace ngs {

template <class T>
class DynamicDepUSetWrapper: public PathedDep {
 public:
  DynamicDepUSetWrapper(Component* fixed_dependant, DepUSet<T>* deps)
      : _fixed_dependant(fixed_dependant),
        _deps(deps) {
  }
  virtual ~DynamicDepUSetWrapper(){}

  virtual void save(SimpleSaver& saver) const {
    size_t num = _paths.size();
    saver.save(num);
    for (size_t i=0; i<num; ++i) {
      _paths[i].save(saver);
    }
  }

  virtual void load(SimpleLoader& loader) {
    size_t num;
    loader.load(num);
    _paths.resize(num);
    for (size_t i = 0; i < num; ++i) {
      _paths[i].load(loader);
    }
  }

  virtual void update_wire() {
    _deps->clear();
    for (size_t i = 0; i < _paths.size(); ++i) {
      if (_fixed_dependant->our_entity()->has_entity(_paths[i])) {
        _deps->insert(_fixed_dependant->get_dep<T>(_paths[i]));
      }
    }
  }

  virtual void bake_path() const {
    _paths.resize(_deps->size());
    DepUSet<T>::const_iterator iter;
    size_t count = 0;
    for (iter = _deps->begin(); iter != _deps->end(); ++iter) {
      if (*iter) {
        Dep<T> dep = *iter;
        _paths[count] = dep.get()->our_entity()->get_path();
        Path owner_path = dep->_fixed_dependant->our_entity()->get_path();
        _paths[count].make_relative_to(owner_path);
      } else {
        _paths[count].invalidate();
      }
    }
  }

 private:
  Component* _fixed_dependant;
  DepUSet<T>* _deps;
  mutable std::vector<Path> _paths;
};

// DynamicDeps change the dependency that they point to
// dynamically at runtime.
template<class T>
class DynamicDepWrapper : public PathedDep {
 public:
  DynamicDepWrapper(Dep<T>* dep)
      : _dep(dep),
        _path(){
  }
  // Serialization.
  virtual void save(SimpleSaver& saver) const {
    bake_path();
    _path.save(saver);
  }
  virtual void load(SimpleLoader& loader) {
    _path.load(loader);
  }
  virtual void update_wire() {
    Component* dep_owner = _dep->_fixed_dependant;
    if (dep_owner->our_entity()->has_entity(_path)) {
      *_dep = dep_owner->get_dep<T>(_path);
    } else {
      (*_dep).reset();
    }
  }
  virtual void bake_path() const{
    if (*_dep) {
      _path = _dep->get()->our_entity()->get_path();
      Path owner_path = _dep->_fixed_dependant->our_entity()->get_path();
      _path.make_relative_to(owner_path);
    } else {
      _path.invalidate();
    }
  }

 private:
  Dep<T>* _dep;
  mutable Path _path;
};

// FixedDeps have a fixed absolute or relative path represented by a const string.
// Note even though the relative path is fixed, if the entity or component that
// owns this is moved in the entity hierarchy, it will refer to a differnt component.
template<class T>
class FixedDepWrapper : public PathedDep {
 public:
  FixedDepWrapper(Dep<T>* dep, const Path& path)
      : _dep(dep),
        _path(path) {
  }

  // Serialization.
  // Because the path is fixed there is no need for save and load logic.
  // The path is fixed in the code.

  virtual void update_wire() {
    Component* dep_owner = _dep->_fixed_dependant;
    if (dep_owner->our_entity()->has_entity(_path)) {
      *_dep = dep_owner->get_dep<T>(_path);
    } else {
      (*_dep).reset();
    }
  }

 private:
  Dep<T>* _dep;
  const Path _path;
};


}
