#pragma once
#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/deplink.h>
#include <base/objectmodel/depset.h>
#include <base/utils/path.h>

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <memory>
#include <iostream>
#include <array>
#include <algorithm>

namespace ngs {

class Entity;
class PathedDep;
class SimpleSaver;
class SimpleLoader;
class DepLoader;

enum HierarchyUpdate {
  kUnchanged,
  kChanged,
  kRequestDestruction,
  kNumHierarchyUpdates
};

// Components will often depend on other components to perform their job.
// Here are some rules for derived Components, in order to maintain proper dependency relationships.
// - They can give out pointers to internal components which they depend on through members.
//   This is because we already depend on the component which is giving out its internal dependencies.
// - Components can accept pointers to external components through their members.
//   However they can only be used during the execution of the method, as they
//   have not been registered as dependants on those external components.
// - If a methods accepts a Component pointer through a method and stores it in
//   a member variable, it should be added as a dependency by calling add_dependency.
//   In this case, it's better to have the method accept an Entity pointer instead.
class OBJECTMODEL_EXPORT Component {
 public:

  typedef std::unordered_set<Component*> Components;
  typedef std::unordered_map<Component*, std::weak_ptr<DepLink> > DepLinks;
  typedef std::unordered_map<size_t, DepLinks> IIDToDepLinks;
  typedef std::unordered_map<size_t, Components> IIDToComponents;

//  template <class T>
//  struct ConvertToDep {
//    Dep<T> operator()(std::pair<Component*, std::weak_ptr<DepLink> >& pair) const {
//      DepLinkPtr base_link = pair.second.lock();
//      if (!base_link) {
//        std::cerr << "Error: dangling weak pointers should have removed on the last unregister call\n";
//        assert(false);
//        return Dep<T>(this);
//      }
//      return Dep<T>(base_link);
//    }
//  };

  struct ConvertToEntity {
    Entity* operator()(const Component* c) {
      std::cerr << "converting component: " << c << " with name: " << c->get_name()
          << " to: " << c->our_entity() << "\n";
      return c->our_entity();
    }
  };


  // Construction/Destruction.
  // Although the interface id is available from a virtual method on the component,
  // this is called in the initializer list of derived components and so the virtual
  // method will not fully resolve to the dervied components get_iid() method.
  // This is why the interface_id is also an argument here.
  Component(Entity* entity, size_t interface_id, size_t derived_id);
  virtual ~Component();

  // The entity which own us.
  Entity* our_entity() const;
  Entity* get_entity(const Path& path) const;
  Entity* has_entity(const Path& path) const;
  void destroy_entity(const Path& path) const;
  Entity* get_app_root() const;
  Entity* get_root_group() const;
  std::string get_path_as_string() const;
  const std::string& get_name() const;

  // Our ids.
  virtual size_t get_iid() const;
  virtual size_t get_did() const;

  // Dirty/Clean State.
  void dirty();
  void clean();
  bool is_dirty() const {return _dirty;}

  // Dirty/Clean Propagation.
  void propagate_cleanliness();
  void propagate_dirtiness(Component* dependency);

  // Serialization.
  virtual void save(SimpleSaver& saver) const;
  virtual void load(SimpleLoader& loader);
  virtual void bake_paths();

  bool dep_is_dirty(DepLinkHolder& dep) {
    if (_dirty_dependencies.count(dep.get_component())) {
      return true;
    }
    return false;
  }

//  // Get all the dependencies for a given interface T.
//  template <class T>
//  std::unordered_set<Dep<T> > get_deps() const {
//    const DepLinks& links = _dependencies[T::get_interface_id()];
//    std::unordered_set<Dep<T> > values;
//    std::transform(links.begin(), links.end(), values.begin(), ConvertToDep<T>());
//    return values;
//  }

  // Get all of our dependant entities for a given interface T.
  // Note that we don't give out the components directly as this could create a cycle.
  // The calling component should instead call get_dep(entity) instead.
  template <class T>
  std::vector<Entity*> get_dependants() const {
    return get_dependants_by_iid(T::kIID());
  }

  std::vector<Entity*> get_dependants_by_iid(size_t iid) const {
    const Components& coms = _dependants[iid];
    std::vector<Entity*> values;
    values.resize(coms.size());
    std::transform(coms.begin(), coms.end(), values.begin(), ConvertToEntity());
    return values;
  }

  std::vector<Entity*> get_dependants_by_did(size_t iid, size_t did) const {
    const Components& coms = _dependants[iid];
    std::vector<Entity*> values;
    for (Component* c: coms) {
      if (c->get_did() == did) {
        values.push_back(c->our_entity());
      }
    }
    return values;
  }

 protected:

  // This should be added at the start of all methods.
  void start_method() {dirty();}
  void start_method() const {}

  // ------------------------------------------------
  // Our 4 get_dep implementations.
  // ------------------------------------------------
  template <class T>
  Dep<T> get_dep(Entity* e) {
    return Dep<T>(get_dep(e, T::kIID()));
  }

  template <class T>
  Dep<T> get_dep(const Path& path) {
    return Dep<T>(get_dep(path, T::kIID()));
  }

  DepLinkPtr get_dep(Entity* e, size_t iid);
  DepLinkPtr get_dep(const Path& path, size_t iid);

  bool dep_creates_cycle(const DepLinkHolder& dep) const{
    return dep_creates_cycle(dep.get_component());
  }

  DepLoader* get_dep_loader() {return _dep_loader;}

  // Dependency Initialization.
  // No opengl calls are allowed when updating deps.
  virtual void initialize_fixed_deps();
  virtual void initialize_dynamic_deps();
  virtual void uninitialize_deps() {}

  // Update the dependencies that we depend on, based on our current state.
  // This should not create or destroy other entities around our entity.
  // Returns true if deps were changed.
  // Multiple update_deps passes over components happen, until all components in our system return false.
  virtual bool update_deps() {return false;}

  // Update the entity hierarchy around us, based on our current state.
  // In order to keep this orthogonal to update_deps, this should try not to update any deps of any components.
  // However there may be cases where this may be unavoidable.
  // Returns the update performed. Components may request destruction as well.
  virtual HierarchyUpdate update_hierarchy() {return kUnchanged;}

  // Update our state from our dirty dependencies. This is called as a result of cleaning.
  // Opengl calls are allowed when updating state, but an opengl context must be current beore calling clean().
  virtual void update_state() {}

  // Opengl Initialization.
  virtual void initialize_gl() {} // Called to initialize opengl resources.
  virtual void uninitialize_gl() {} // Called to release and destroy opengl resources.
  virtual bool is_initialized_gl() {return true;}
  virtual void initialize_gl_helper();

 private:

//  // Private Serialization methods.
  virtual void pre_save(SimpleSaver& saver) const;

  // Dependencies.
  DepLinkPtr connect_to_dep(Component* c);
  void disconnect_from_dep(Component* c);

  // Our dep links.
  //bool has_dep_link(Component* c) const;
  DepLinkPtr get_dep_link(Component* c) const;
  void set_dep_link(Component* c, DepLinkPtr link);

  // Cycles.
  bool is_recursive_dependency(const Component* dependency) const;
  bool dep_creates_cycle(const Component* dependency) const;

  // Registration of Dependants.
  void register_dependant(Component* c) const;
  void unregister_dependant(Component* c) const;

  // Registration of Dependencies.
  DepLinkPtr register_dependency(Component* c);
  void unregister_dependency(Component* c);
  void remove_dep_link(Component* c, size_t iid);

  // Our parent entity.
  Entity* _entity; // borrowed reference.

  // ID numbers.
  size_t _iid; // The id for the interface we implement.
  size_t _did; // The id which represents us as a derived/implement type.

  // Our dirty state.
  bool _dirty;

  // Our dependants which depend on us for information.
  // Note changing our dependants doesn't make our content dirty because
  // we don't allow usage of upstream dependant components from within.
  // We only propagate dirtiness to them.
  mutable IIDToComponents _dependants;

  // Our dependencies on which we depend for information.
  IIDToDepLinks _dependencies;

  // Our dependencies which were dirtied.
  Components _dirty_dependencies;

  // Our dep loader.
  DepLoader* _dep_loader;

  // The Dependency class needs access to our private dependency methods.
  friend class Entity;
  friend class DepLink;

  template<class T>
  friend class Dep;

  template<class T>
  friend class FixedDepWrapper;

  template<class T>
  friend class DynamicDepWrapper;

  template<class T>
  friend class DynamicDepUSetWrapper;
};

}
