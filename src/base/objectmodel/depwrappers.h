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

  virtual void initialize() {
    for (size_t i = 0; i < _paths.size(); ++i) {
      Entity* target = _fixed_dependant->has_entity(_paths[i]);
      if (target) {
        _deps->insert(_fixed_dependant->get_dep<T>(target));
      }
    }
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

  const std::string kUnlinkedTarget="__INTERNAL__/UNLINKED";

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
  virtual void initialize() {
    if (_path == kUnlinkedTarget) {
      return;
    }
    Entity* target = _dep->_fixed_dependant->has_entity(_path);
    if (target) {
    	Component* c = _dep->_fixed_dependant;
      *_dep = c->get_dep<T>(target);
    } else {
      _path = kUnlinkedTarget;
    }
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

// FixedDeps have fixed absolute or relative paths,
// represented with a const string.
template<class T>
class FixedDepWrapper : public PathedDep {
 public:
  FixedDepWrapper(Dep<T>* dep, const std::string& path)
      : _dep(dep),
        _path(path) {
  }

  // Serialization.
  virtual void initialize() {
    Entity* target = _dep->_fixed_dependant->has_entity(_path);
    if (target) {
    	Component* c = _dep->_fixed_dependant;
      *_dep = c->get_dep<T>(target);
    }
  }

 private:
  Dep<T>* _dep;
  const std::string _path;
};


}
