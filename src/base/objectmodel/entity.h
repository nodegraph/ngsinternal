#pragma once
#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/entityids.h>
#include <base/objectmodel/entityconfig.h>

#include <cassert>
#include <string>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <QtCore/QJsonValue>

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
  static EntityIID kIID() {
    return EntityIID::kIInvalidEntity;
  }
  virtual EntityIID get_iid() const {
    return kIID();
  }

  // Our derived id.
  static EntityDID kDID() {
    return EntityDID::kInvalidEntity;
  }
  virtual EntityDID get_did() const {
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
  typedef std::unordered_map<ComponentIID, Component*> IIDToComponentMap;
  typedef std::unordered_map<std::string, Entity*> NameToChildMap;

  // Constructors.
  Entity(Entity* parent, const std::string& name);
  virtual ~Entity();

  // Creates internal components. The ids argument can contain ids for sub entities or components.
  // It is up to the derived entity classes what the ids actually mean.
  virtual void create_internals(const EntityConfig& configs = EntityConfig()) {}

  // Our parent.
  Entity* get_parent() const;

  // Our group context.
  std::unordered_set<EntityDID> get_group_context_dids() const;
  bool has_group_related_did() const;
  bool has_macro_related_did() const;
  size_t get_num_nodes() const;

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
  void destroy_all_children_except(const std::unordered_set<EntityDID>& dids);
  void rename_child(const std::string& prev_name, const std::string& next_name);
  const NameToChildMap& get_children() const;
  bool is_topologically_fixed() const;

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

  Entity* get_root_group() const;
  bool is_root_group() const;

  // Initialize GL.
  void initialize_gl();
  void uninitialize_gl();

  // Helper routines for multi threading.
  void collect_components(std::vector<Component*>& comps) const;

  // Wires represent an imaginary line which connects to components that are our dependencies.
  // Initializes serialized component wires in entity tree below us and on us.
  // Serialized component wires are wires which actually get saved on disk.
  // Serialized component wires have an associated absolute or relative path to point to other component dependencies.
  // This should only be called once after an entity is constructed.
  // Note: it should be called after all entities are created others the wires may
  //      refer to something that has not been created yet.
  // Also: it should be called on newly created nodes to initialize their wires and allow
  //       them to embed properly into the graph.
  void initialize_wires();

  void helper(const std::vector<Component*>& comps, std::pair<size_t,size_t>& range);
  void initialize_wires_mt();

  // Gathers all non-serialized component wires in entity tree below us and and on us.
  // Non-serialized component wires are those that are gathered dynamically by components.
  void clean_wires();

  // This returns true if an entity wants to be destroyed.
  bool should_destroy();

  // Cleans out all entities that want to be destroyed from the entity tree.
  void clean_dead_entities();

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

  bool has_comp_with_iid(ComponentIID iid) const;
  bool has_comp_with_did(ComponentIID iid, ComponentDID did) const;
 protected:
  Component* get(ComponentIID iid) const;

  template<class T>
  T* get() const {
    return static_cast<T*>(get(T::kIID()));
  }

  Entity* get_entity_helper(const Path& path) const;

 public:

  // ---------------------------------------------------------------------
  // Copy and Paste related.
  // ---------------------------------------------------------------------

  // Copy a selection of entities.
  virtual void copy(SimpleSaver& saver, const std::unordered_set<Entity*>& selection) const;

  // Paste.
  virtual void paste_without_merging(SimpleLoader& loader);
  void paste_with_merging(SimpleLoader& loader);


  // Any name collisions will result in updating the existing node,
  // instead of creating a novel node with a slightly altered name.
  Entity* paste_entity_with_merging(SimpleLoader& loader);

 private:
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
  friend DepLinkPtr Component::get_dep(Entity*, ComponentIID);

  friend class GroupNodeEntity;
};



}
