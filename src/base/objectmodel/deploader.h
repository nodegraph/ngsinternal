#pragma once
#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/depset.h>
#include <base/objectmodel/depwrappers.h>

#include <base/memoryallocator/taggednew.h>

#include <vector>
#include <string>

namespace ngs {

template <class T>
class Dep;

template <class T>
class FixedDepWrapper;

template <class T>
class DynamicDepWrapper;

template <class T>
class DynamicDepUSetWrapper;

class SimpleSaver;
class SimpleLoader;

class Component;
class PathedDep;

class DepLoader {
public:
  DepLoader();
  ~DepLoader();

  template <class T>
  void register_fixed_dep(Dep<T>& dep, const Path& path) {
    FixedDepWrapper<T>* w = new_ff FixedDepWrapper<T>(&dep, path);
    _fixed_deps.push_back(w);
  }

  template <class T>
  void register_dynamic_dep(Dep<T>& dep) {
    DynamicDepWrapper<T>* w = new_ff DynamicDepWrapper<T>(&dep);
    _dynamic_deps.push_back(w);
  }

  template <class T>
  void register_dynamic_dep_set(Component* fixed_dependant, DepUSet<T>& dep_set) {
    DynamicDepUSetWrapper<T>* w = new_ff DynamicDepUSetWrapper<T>(fixed_dependant, &dep_set);
    _set_deps.push_back(w);
  }

  // Updates return true if something actually changes in the update.
  void update_fixed_wires();
  void update_dynamic_wires();

  void save(SimpleSaver& saver) const;
  void load(SimpleLoader& loader);
  void bake_paths();

 private:
  std::vector<PathedDep*> _fixed_deps;
  std::vector<PathedDep*> _dynamic_deps;
  std::vector<PathedDep*> _set_deps;
};




}
