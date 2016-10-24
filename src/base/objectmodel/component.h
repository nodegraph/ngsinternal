#pragma once
//#define __pragma(x) _Pragma("(x)")

#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/deplink.h>
#include <base/objectmodel/depset.h>
#include <base/utils/path.h>

#include <entities/componentids.h>

#include <unordered_set>
#include <unordered_map>



#include <stdio.h>
#include <stddef.h>
#include <vcruntime.h>
#include <inttypes.h>

#include <cstdlib>
#include <vector>
#include <cassert>
#include <memory>
#include <iostream>
#include <array>
#include <algorithm>
#include <mutex>

namespace ngs {

class Entity;
class PathedDep;
class SimpleSaver;
class SimpleLoader;
class DepLoader;

enum WiresChange {
  kUnchanged,
  kChanged,
  kRequestDestruction
};

// Used to mark static methods.
// This makes marking of methods/functions symmetric with
// our other markers ... external() and internal().
static inline void is_static() {}

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
  typedef std::unordered_map<ComponentIID, DepLinks> IIDToDepLinks;
  typedef std::unordered_map<ComponentIID, Components> IIDToComponents;


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
  Component(Entity* entity, ComponentIID interface_id, ComponentDID derived_id);
  virtual ~Component();

#ifdef MULTITHREAD_COMPONENTS
  // Lockable.
  void lock() {
    _mutex.lock();
  }
  void unlock() {
    _mutex.unlock();
  }
#endif

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
  virtual ComponentIID get_iid() const;
  virtual ComponentDID get_did() const;

  virtual const char* get_iid_name() const {return "unknown iid";}
  virtual const char* get_did_name() const {return "unknown did";}

  // Wires. Propagates clean wires through dependency traversal.
  // Note: This is differnt from Entity::clean_wires as it traverses
  //       through dependencies and through entity hierarchy visitation.
  void clean_wires();
  void clean_local_wires();

  // Dirty/Clean State.
  void dirty_state();
  bool clean_state();
  bool is_state_dirty() const {return _dirty;}


  // GL.
  virtual bool is_initialized_gl() const {return is_initialized_gl_imp();}

  // Dirty/Clean Propagation.
  virtual bool clean_dependencies();
  virtual bool clean_self();
  virtual bool clean_finalize();
  void gather_dirty_components(std::vector<Component*>& comps); // Rectursively gather dirty components in order of execution needed to clean them.
  bool propagate_cleanliness();
  virtual void propagate_dirtiness(Component* dependency);
  virtual void set_self_dirty(bool dirty);

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
  std::unordered_set<Entity*> get_dependants() const {
    return get_dependants_by_iid(T::kIID());
  }

  std::unordered_set<Entity*> get_dependants_by_iid(ComponentIID iid) const {
    const Components& coms = _dependants[iid];
    std::unordered_set<Entity*> values;
    for (Component* c: coms) {
      values.insert(c->our_entity());
    }
    return values;
  }

  std::unordered_set<Entity*> get_dependants_by_did(ComponentIID iid, ComponentDID did) const {
    const Components& coms = _dependants[iid];
    std::unordered_set<Entity*> values;
    for (Component* c: coms) {
      if (c->get_did() == did) {
        values.insert(c->our_entity());
      }
    }
    return values;
  }

 protected:

  // This should be added at the start of all methods.
  void external() {dirty_state();}
  void external() const {}
  void internal() {}
  void internal() const {}


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

  DepLinkPtr get_dep(Entity* e, ComponentIID iid);
  DepLinkPtr get_dep(const Path& path, ComponentIID iid);

  bool dep_creates_cycle(const DepLinkHolder& dep) const{
    return dep_creates_cycle(dep.get_component());
  }

  DepLoader* get_dep_loader() {return _dep_loader;}

  // ----------------------------------------------------------------------------------------------------------
  // Component Wires and State.
  // ----------------------------------------------------------------------------------------------------------

  // Wires represent the imaginary wires which link us to our dependencies (components we depend on).
  // This method initializes wires from serialized data, which are fixed or relative paths to other
  // entities in the entity tree.
  // 1) This should be called once on all entities after all entities are created.
  // 2) This should also be called on all entities after some entities are destroyed or created.
  //    However for speed reasons we call this only on newly created entities.
  //    What this means is that once an entity's wire points to a destroyed entity's component,
  //    it will always stay null, even when a new entity's component is created in its place.
  //    These cases rarely occur, but when they do just call initialize_wires on those entities
  //    that need there wires to point to the new component. This logic could also live in update_wires().
  virtual void initialize_wires();

  // This method updates our wires based on our current state.
  // This method should not update our dependency components at the other end of the wire.
  // This method is orthogonal to update_deps().
  // In order to update our wires we are allowed to modify the entity hierarchy around us.
  // Just make sure to return the an appropriate value to indicate the hierarchy change performed.
  // Note: we request that we be destroyed by returning an appropriate value.
  // Note: No opengl calls are allowed when updating wires.
  // Note: The component should be marked as dirty if the wires change.
  // Note: start_method() (method which dirties the component) should not be called in derived methods.

  virtual void update_wires() {}

  // This method should return true, when it determines that this component's entity
  // does not need to exist anymore. For example if a LinkShape see it has no input or output to connect to.
  virtual bool should_destroy() {return false;}

  // Update our state from our dirty dependencies. This is called as a result of cleaning.
  // Opengl calls are allowed when updating state, but an opengl context must be current beore calling clean().
  // Note: start_method() (method which dirties the component) should not be called in derived methods.
  // Returning true implies the update is complete.
  // Returning false implies the compute is asynchronously happening somewhere off the main thread.
  // In this case there is logic to trigger the update_state at a later time.
  virtual bool update_state() {return true;}
  virtual void finalize_state() {}

  // ----------------------------------------------------------------------------------------------------------
  // Opengl Initialization.
  // ----------------------------------------------------------------------------------------------------------
  virtual void initialize_gl() {} // Called to initialize opengl resources.
  virtual void uninitialize_gl() {} // Called to release and destroy opengl resources.
  virtual void initialize_gl_helper();
  virtual bool is_initialized_gl_imp() const {return true;} // This is made public in derived classes.

 private:

  // Private Serialization methods.
  virtual void pre_save(SimpleSaver& saver) const;

  // ----------------------------------------------------------------------------------------------------------
  // Dependencies.
  // ----------------------------------------------------------------------------------------------------------

  // Dependencies.
  DepLinkPtr connect_to_dep(Component* c);
  void disconnect_from_dep(Component* c);

  // Our dep links.
  //bool has_dep_link(Component* c) const;
  DepLinkPtr get_dep_link(Component* c) const;
  void set_dep_link(Component* c, DepLinkPtr link);

  // Cycles.
  bool is_recursive_dependency(const Component* dependency, std::vector<const Component*>* path = NULL) const;
  bool dep_creates_cycle(const Component* dependency) const;

  // Registration of Dependants.
  void register_dependant(Component* c) const;
  void unregister_dependant(Component* c) const;

  // Registration of Dependencies.
  DepLinkPtr register_dependency(Component* c);
  void unregister_dependency(Component* c);
  void remove_dep_link(Component* c, ComponentIID iid);

  // Our parent entity.
  Entity* _entity; // borrowed reference.

  // ID numbers.
  ComponentIID _iid; // The id for the interface we implement.
  ComponentDID _did; // The id which represents us as a derived/implement type.

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

  // Locking.
#ifdef MULTITHREAD_COMPONENTS
  std::recursive_mutex _mutex;
#endif

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

// An invalid component which should never be created.
class InvalidComponent: public Component {
 public:
  COMPONENT_ID(InvalidComponent, InvalidComponent);
  InvalidComponent(Entity* entity):Component(entity, kIID(), kDID()) {}
};

}
