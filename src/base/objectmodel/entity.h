#pragma once
#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/component.h>
//#include <base/utils/path.h>

#include <cassert>
#include <string>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace ngs {

class Path;

class BaseFactory;
class BaseEntityInstancer;
class BaseComponentInstancer;

class SimpleSaver;
class SimpleLoader;

// -------------------------------------------------------------------------
// The Entity Class.
// -------------------------------------------------------------------------

/// - The Entity class represents an abstract entity which can act as
///   a singular unit.
///   + For example in games, AI agents, players and collidable
///     objects are often represented as entities.
/// - The Entity class does not use inheritance to provide functional variation.
///   + Using inheritance hierarchies to inherit or override methods doesn't
///     allow proper separation of new logic into a reusable unit.
///   + Overriding methods is tightly coupled to the inheritance hierarchy.
///   + Hence designing and maintaining complex hierarchies can become
///     problematic.
/// - The Entity class uses composition instead of inheritance to provide variation.
///   + Composition allows collecting arbitrary objects and using them internally.
///   + The Component class represents objects which are composed by the Entity class.
///   + Acquiring new logic no longer involves carefully crafting or
///     modifying the inheritance hierarchy.
///   + Acquiring new logic is now orthogonal to the inheritance hierarchy.

// Entities are bags which hold Components which should be allocated and
// deallocated together.
// A good example is a Node and its Input/OutputPlugs. Anyone one of these can be
// allocated and deallocated and it will still leave the node graph in a valid state.
// A bad example is a LinkHead and LinkTail, which represents the head and tail of
// a link. When on of these is destroyed, it leaves the node graph in an invalid state.

class OBJECTMODEL_EXPORT Entity {
 public:

  // Our interface id.
  static size_t kIID() {
    return -1;
  }
  virtual size_t get_iid() const {
    return kIID();
  }

  // Our derived id.
  static size_t kDID() {
    return -1;
  }
  virtual size_t get_did() const {
    return kDID();
  }

  // Our derived id name.
  static const char* kDIDName() {
    return "entity";
  }
  virtual const char* get_did_name() const {
    return kDIDName();
  }

  // The maximum number of component interfaces we can have.
  typedef std::unordered_map<size_t, Component*> IIDToComponentMap;
  typedef std::unordered_map<std::string, Entity*> NameToChildMap;

  // Constructors.
  Entity(Entity* parent, const std::string& name);
  virtual ~Entity();

  // Creates internal components and children.
  virtual void create_internals() {}

  // Our parent.
  Entity* get_parent() const;

  // Our name.
  const std::string& get_name() const;
  void rename(const std::string& next_name);
  void set_name(const std::string& name); // Todo: remove this.

  // Our identifying path.
  Path get_path() const;

  // Our children.
  virtual void add_orphan_child(Entity* child);
  virtual void take_child(Entity* child);
  Entity* get_child(const std::string& name) const;
  virtual void remove_child(Entity* child);
  void reparent_child(Entity* child, Entity* next_parent);
  void destroy_all_children();
  void rename_child(const std::string& prev_name, const std::string& next_name);
  const NameToChildMap& get_children() const;



  // Our children's names.
  bool has_child_name(const std::string& name) const;
  void make_child_name_unique(std::string& name) const;

  // Get entities by an absolute or relative path.
  Entity* has_entity(const Path& path) const;
  Entity* get_entity(const Path& path) const;
  void destroy_entity(const Path& path) const;

  // Get common entities.
  Entity* get_app_root() const; // Gets the top most entity.
  bool is_app_root() const; // Are we the top most entity.

  // Initialize deps.
  void initialize_deps();
  void uninitialize_deps();

  // Initialize GL.
  void initialize_gl();
  void uninitialize_gl();

  // Update hierarchy and deps.
  void update_deps_and_hierarchy();

  // Serialization.
  virtual void save(SimpleSaver& saver) const;
  virtual void pre_save(SimpleSaver& saver) const;
  virtual void save_components(SimpleSaver& saver) const;
  virtual void save_children(SimpleSaver& saver) const;

  virtual void load(SimpleLoader& loader);
  virtual void load_helper(SimpleLoader& loader);
  virtual void paste(SimpleLoader& loader);
  virtual void load_components(SimpleLoader& loader);

  virtual void bake_paths();

  // Adds a component.
  // You can retrieve it later by its interface ID.
  void add(Component* c);
  void remove(Component* c);
  Component* back();
  void pop();

  // -----------------------------------------------------------

  template<class ... T>
  class HasComponents;

  template<class B>
  struct HasComponents<B> {
    static const bool value(const Entity* e) {
      return e->get<B>();
    }
  };

  template<class H, class ... T>
  struct HasComponents<H, T...> {
    static const bool value(const Entity* e) {
//      if (!e->get<H>()) {
//        std::cerr << "could not find: " << H::kIID() << "," << H::kDID() << "\n";
//      }
      return e->get<H>() && HasComponents<T...>::value(e);
    }
  };

  template<class ... T>
  bool has() const {
    return HasComponents<T...>::value(this);
  }

  // -----------------------------------------------------------
  // These are hacks which expose the internal derived types of
  // the components. They are helpful when refactoring to cleaner
  // code. These will eventually be removed.
  // -----------------------------------------------------------

//  template<class B, class D>
//  D* get_derived_component() const {
//    Component* c = get<B>();
//    if (!c) {
//      return NULL;
//    }
//    if (c->get_did() == D::kDerivedID()) {
//        return static_cast<D*>(c);
//    }
//    return NULL;
//  }

  // ---------------------------------------------------------------------
  // Copy and Paste related.
  // ---------------------------------------------------------------------

  // Copy/Paste Logic.
  std::string copy_to_string(std::unordered_set<Entity*>& selected) const;
  void paste_from_string(const std::string& raw);

  // Last pasted entities.
  const std::unordered_set<Entity*>& get_last_pasted();
  void clear_last_pasted();

 protected:
  Component* get(size_t interface_id) const;

  template<class T>
  T* get() const {
    return static_cast<T*>(get(T::kIID()));
  }

  Entity* get_entity_helper(const Path& path) const;

 private:

  // Initialize our dependencies, upon creation or loading.
  void initialize_fixed_deps();
  void initialize_dynamic_deps();

  // Updates the dependencies that our components depend on.
  // See the Component class for more details.
  void update_deps();
  bool update_deps_helper();

  // Updates the hierarchy around us.
  // See the Component class for more details.
  void update_hierarchy();
  bool update_hierarchy_helper();

  // ---------------------------------------------------------------------
  // Copy and Paste related.
  // ---------------------------------------------------------------------

  // Copy a selection of entities.
  void copy(SimpleSaver& saver, const std::unordered_set<Entity*>& selection) const;

  void paste_without_merging(SimpleLoader& loader);
  void paste_with_merging(SimpleLoader& loader);

  // Any name collisions will result in updating the existing node,
  // instead of creating a novel node with a slightly altered name.
  Entity* paste_entity_with_merging(SimpleLoader& loader);

  // ---------------------------------------------------------------------
  // Entity Hierarchy related members.
  // ---------------------------------------------------------------------

  // Our parent;
  Entity* _parent;

  // Our name;
  std::string _name;

  // Our children. All entities can be accessed by a path.
  NameToChildMap _children;

  // ---------------------------------------------------------------------
  // Component related members.
  // ---------------------------------------------------------------------

  // Our components. These can be thought of as our interfaces.
  IIDToComponentMap _components;

  // ---------------------------------------------------------------------
  // Copy and Paste related members.
  // ---------------------------------------------------------------------

  // This info is needed to select the newly created nodes.
  std::unordered_set<Entity*> _last_pasted;

  // This is the only method that can call our "get<>" method.
  friend DepLinkPtr Component::get_dep(Entity*, size_t);
};



}
