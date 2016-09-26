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
    size_t num = _deps->size();
    saver.save(num);
    for (const Dep<T>& d : *_deps) {
      if (d) {
        std::string path = d->our_entity()->get_path().get_as_string();
        saver.save(path);
      }
    }
  }

  virtual void load(SimpleLoader& loader) {
    _deps->clear();
    size_t num;
    loader.load(num);
    _paths.resize(num);
    for (size_t i = 0; i < num; ++i) {
      loader.load(_paths[i]);
    }
  }

  virtual bool update_wire() {
    DepUSet<T> next_deps;
    for (size_t i = 0; i < _paths.size(); ++i) {
      Dep<T> next_dep(_fixed_dependant);
      if (_fixed_dependant->our_entity()->has_entity(_paths[i])) {
        next_dep = _fixed_dependant->get_dep<T>(_paths[i]);
        next_deps->insert(next_dep);
      }
    }
    if (*_deps == next_deps) {
      return false;
    }
    *_deps = next_deps;
    return true;
  }

 private:
  Component* _fixed_dependant;
  DepUSet<T>* _deps;
  std::vector<std::string> _paths;
};

// DynamicDeps change the dependency that they point to
// dynamically at runtime.
template<class T>
class DynamicDepWrapper : public PathedDep {
 public:

  const std::string kUnlinkedTarget="__INTERNAL__/UNLINKED"; // We assume this path never exists in the entity hierarchy.

  DynamicDepWrapper(Dep<T>* dep)
      : _dep(dep),
        _path(kUnlinkedTarget){
  }
  // Serialization.
  virtual void save(SimpleSaver& saver) const {
    bake_path();
    saver.save(_path);
  }
  virtual void load(SimpleLoader& loader) {
    loader.load(_path);
  }
  virtual bool update_wire() {
    Component* dep_owner = _dep->_fixed_dependant;
    Dep<T> next_dep(dep_owner);
    if (dep_owner->our_entity()->has_entity(_path)) {
      next_dep = dep_owner->get_dep<T>(_path);
    }
    if (*_dep == next_dep) {
      return false;
    }
    *_dep = next_dep;
    return true;
  }
  virtual void bake_path() const{
    if (*_dep) {
      Path dependency_path = _dep->get()->our_entity()->get_path();
      Path dependant_path = _dep->_fixed_dependant->our_entity()->get_path();
      dependency_path.make_relative_to(dependant_path);
      _path = dependency_path.get_as_string();
    } else {
      _path = kUnlinkedTarget;
    }
  }

 private:
  Dep<T>* _dep;
  mutable std::string _path;
};

// FixedDeps have a fixed absolute or relative path represented by a const string.
// Note even though the relative path is fixed, if the entity or component that
// owns this is moved in the entity hierarchy, it will refer to a differnt component.
template<class T>
class FixedDepWrapper : public PathedDep {
 public:
  FixedDepWrapper(Dep<T>* dep, const std::string& path)
      : _dep(dep),
        _path(path) {
  }

  // Serialization.
  // Because the path is fixed there is no need for save and load logic.
  // The path is fixed in the code.

  virtual bool update_wire() {
    Component* dep_owner = _dep->_fixed_dependant;
    Dep<T> next_dep(dep_owner);
    if (dep_owner->our_entity()->has_entity(_path)) {
      next_dep = dep_owner->get_dep<T>(_path);
    }
    if (*_dep == next_dep) {
      return false;
    }
    *_dep = next_dep;
    return true;
  }

 private:
  Dep<T>* _dep;
  const std::string _path;
};


}
