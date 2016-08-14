#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/basefactory.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/entity.h>
#include <base/objectmodel/lowerhierarchychange.h>
#include <base/objectmodel/upperhierarchychange.h>
#include <base/utils/path.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>

#include <boost/lexical_cast.hpp>
#include <components/entities/factory.h>

namespace ngs {

Entity::Entity(Entity* parent, const std::string& name)
    : _parent(NULL),
      _name(name) {
  if (parent) {
    // Note the adding ourself as a child will also set our _parent pointer.
    parent->add_orphan_child(this);
  }
  // All entities have an entity change component.
  // This allows other components to depend on changes to this entity.
  // These entity level changes are just the addition and removal of our child entities.
  new_ff LowerHierarchyChange(this);
  new_ff UpperHierarchyChange(this);
}

Entity::~Entity() {
  // The order of deletion of child entities is random.
  destroy_all_children();

  // The order of deletion of components within an entity is random.
  // 1) Components should not destroy any sibling components.
  // 2) Components should not to use any sibling components during destruction.
  IIDToComponentMap copy = _components;
  for (auto &iter: copy) {
    delete_ff(iter.second);
  }
  assert(_components.empty());

  // Remove ourself from our parent.
  if (_parent) {
    _parent->remove_child(this);
  }
  _parent = NULL;
}

Entity* Entity::get_parent() const {
  return _parent;
}

const std::string& Entity::get_name() const {
  return _name;
}

void Entity::add(Component* c) {
  assert(c);
  size_t iid = c->get_iid();
  assert(!_components.count(iid));
  _components[iid] = c;
  c->_entity = this;
}

void Entity::remove(Component* c) {
  assert(c);
  size_t iid = c->get_iid();
  assert(_components.count(iid));
  _components.erase(iid);
  c->_entity = NULL;
}

Component* Entity::get(size_t iid) const {
  if (!_components.count(iid)) {
    return NULL;
  }
  return _components.at(iid);
}

void Entity::rename(const std::string& next_name) {
  Entity* parent = get_parent();
  if (parent) {
    parent->rename_child(get_name(), next_name);
  } else {
    set_name(next_name);
  }
  // A change has occurred to this entity.
  get<LowerHierarchyChange>()->dirty();
}

void Entity::add_orphan_child(Entity* child) {
  // Make sure the child is not currently parented.
  assert(child->get_parent() == NULL);
  child->_parent = this;

  // Make sure the child's name is unique.
  if (has_child_name(child->get_name())) {
    std::string unique_name = child->get_name();
    make_child_name_unique(unique_name);
    child->set_name(unique_name);
  }
  // Insert the child under the unique name.
  _children[child->get_name()] = child;
  // A change has occurred to this entity.
  get<LowerHierarchyChange>()->dirty();
}

void Entity::take_child(Entity* child) {
  if (child->get_parent()) {
    child->get_parent()->remove_child(child);
  }
  add_orphan_child(child);
}

//const std::string& Entity::get_remapped_name(const std::string& old_name) const {
//  if (_name_remapping.count(old_name)) {
//    return _name_remapping.at(old_name);
//  }
//  return old_name;
//}

Entity* Entity::get_child(const std::string& name) const {
  if (!has_child_name(name)) {
    return NULL;
  }
  return _children.at(name);
}

void Entity::remove_child(Entity* child) {
  // Make sure we actually have this child.
  if (!has_child_name(child->get_name())) {
    assert(false);
    return;
  }
  // Remove the child.
  _children.erase(child->get_name());
  child->_parent = NULL;
  // A change has occurred to this entity.
  get<LowerHierarchyChange>()->dirty();
}

void Entity::reparent_child(Entity* child, Entity* next_parent) {
  remove_child(child);
  next_parent->add_orphan_child(child);
}

void Entity::destroy_all_children() {
  // The order of deletion of child entities is random.
  // Deleting children will modify our children's set so we make a copy.
  NameToChildMap copy = _children;
  for (auto &e: copy) {
    delete_ff(e.second);
  }
  assert(_children.empty());
}

void Entity::destroy_all_children_except(const std::unordered_set<size_t>& dids) {
  // The order of deletion of child entities is random.
  // Deleting children will modify our children's set so we make a copy.
  NameToChildMap copy = _children;
  for (auto &e : copy) {
    if (!dids.count(e.second->get_did())) {
      delete_ff(e.second);
    }
  }
}

void Entity::rename_child(const std::string& prev_name, const std::string& next_name) {
  if (!has_child_name(prev_name)) {
    return;
  }
  Entity* e = _children.at(prev_name);
  remove_child(e);
  e->set_name(next_name);
  add_orphan_child(e);
  // A change has occurred to this entity.
  get<LowerHierarchyChange>()->dirty();
}

const Entity::NameToChildMap& Entity::get_children() const {
  return _children;
}

Entity* Entity::get_app_root() const {
  if (!get_parent()) {
    return const_cast<Entity*>(this);
  }
  return get_parent()->get_app_root();
}

bool Entity::is_app_root() const {
  if (!get_parent()) {
    return true;
  }
  return false;
}

Entity* Entity::get_root_group() const {
  return get_app_root()->get_entity("./root");
}

bool Entity::is_root_group() const {
  if (this == get_root_group()) {
    return true;
  }
  return false;
}

void Entity::initialize_fixed_deps() {
  for (auto &iter: _components) {
    iter.second->initialize_fixed_deps();
  }
  for (auto &iter: _children) {
    iter.second->initialize_fixed_deps();
  }
}

void Entity::initialize_dynamic_deps() {
  for (auto &iter: _components) {
    iter.second->initialize_dynamic_deps();
  }
  for (auto &iter: _children) {
    iter.second->initialize_dynamic_deps();
  }
}

void Entity::initialize_deps() {
  initialize_fixed_deps();
  initialize_dynamic_deps();
}

void Entity::uninitialize_deps() {
  for (auto &iter: _components) {
    iter.second->uninitialize_deps();
  }
  for (auto &iter: _children) {
    iter.second->uninitialize_deps();
  }
}

void Entity::update_deps() {
  // Encoding the proper update order among the components
  // is tedious to encode. So we currently rely on multiple
  // passes until the deps no longer change.
  static const size_t kMaxUpdateRuns = 5;
  size_t count = 0;
  while(update_deps_helper()) {
    ++count;
    if (count>kMaxUpdateRuns) {
      assert(false);
    }
  }
}

bool Entity::update_deps_helper() {
  bool changed = false;
  for (auto &iter: _components) {
    if (iter.second->update_deps()) {
      changed = true;
    }
  }
  for (auto &iter: _children) {
    if (iter.second->update_deps_helper()) {
      changed = true;
    }
  }
  return changed;
}

void Entity::update_hierarchy() {
  // Encoding the proper update order among the components
  // is tedious to encode. So we currently rely on multiple
  // passes until the hierarchy no longer change.
  static const size_t kMaxUpdateRuns = 5;
  size_t count = 0;
  while(update_hierarchy_helper()) {
    ++count;
    if (count>kMaxUpdateRuns) {
      assert(false);
    }
  }
}

bool Entity::update_hierarchy_helper() {
  bool changed = false;
  for (auto &iter: _components) {
    HierarchyUpdate t = iter.second->update_hierarchy();
    if (t == kRequestDestruction) {
      std::cerr << "Destroying due to destruction request\n";
      delete2_ff(this);
      return true;
    } else if (t != kUnchanged) {
      changed = true;
    }
  }
  // Because our children can destroy them selves, we need to restart
  // the loop if anything returns anything other than kUnchanged.
  {
    bool repeat = false;
    do {
      repeat = false;
      for (auto &iter : _children) {
        if (iter.second->update_hierarchy_helper()) {
          repeat = true;
          break;
        }
      }
    } while (repeat);
  }
  // All children will report kUnchanged at this point.

  // Now return our changed state.
  return changed;
}

void Entity::update_deps_and_hierarchy() {
  static const size_t kMaxUpdateRuns = 5;
  size_t count = 0;
  while(true) {
    ++count;
    if (count>kMaxUpdateRuns) {
      assert(false);
    }
    bool hierarchy_changed = update_hierarchy_helper();
    bool deps_changed = update_deps_helper();
    if ((!hierarchy_changed) && (!deps_changed)) {
      break;
    }
  }
}

void Entity::initialize_gl() {
  for (auto &iter: _components) {
    iter.second->initialize_gl_helper();
  }
  for (auto &iter: _children) {
    iter.second->initialize_gl();
  }
}

void Entity::uninitialize_gl() {
  for (auto &iter: _components) {
    iter.second->uninitialize_gl();
  }
  for (auto &iter: _children) {
    iter.second->uninitialize_gl();
  }
}

void Entity::bake_paths() {
  for (auto &iter: _components) {
    iter.second->bake_paths();
  }
  for (auto &iter: _children) {
    iter.second->bake_paths();
  }
}

void Entity::pre_save(SimpleSaver& saver) const {
  size_t derived_id = get_did();
  saver.save(derived_id);
  std::string name = get_name();
  saver.save(name);
}

void Entity::save_components(SimpleSaver& saver) const {
  size_t num = _components.size();
  saver.save(num);
  for (auto &iter: _components) {
    iter.second->save(saver);
  }
}

void Entity::save_children(SimpleSaver& saver) const {
  size_t num_children = _children.size();
  saver.save(num_children);
  for (auto iter: _children) {
    iter.second->save(saver);
  }
}

void Entity::save(SimpleSaver& saver) const {
  pre_save(saver);
  save_components(saver);
  save_children(saver);
}

void Entity::load_components(SimpleLoader& loader) {
  BaseComponentInstancer* ci = get_app_root()->get<BaseFactory>()->get_component_instancer();
  size_t num_components;
  loader.load(num_components);
  for (size_t i=0; i<num_components; ++i) {
    // Peek at the derived id.
    size_t derived_id;
    loader.load(derived_id);
    // Does this component already exist.
    Component* c = get(ci->get_loadable_iid(derived_id));
    // Make sure it has the same derived id.
    if (c && c->get_did() != derived_id) {
      //assert(false);
      std::cerr << "warning: replacing a component with did: " << c->get_did()
          << "with another with did: " << derived_id << "at entity path: " << get_path().get_as_string() << "\n";
      delete_ff(c);
    }
    // Otherwise we create one.
    if (!c) {
      c = ci->instance(this, derived_id);
      assert(c);
    }
    // Now load it up.
    c->load(loader);
  }
}

void Entity::load(SimpleLoader& loader) {
  load_helper(loader);
  // Now that everything has been pasted we need to update our deps and hierarchy.
  initialize_fixed_deps();
  initialize_dynamic_deps();
  update_hierarchy();
  update_deps();
}

void Entity::load_helper(SimpleLoader& loader) {
  // Load components.
  load_components(loader);
  // Load child entities.
  paste_with_merging(loader);
}

void Entity::paste(SimpleLoader& loader) {
  // There shouldn't be any components to load.
  // Load child entities.
  paste_without_merging(loader);
  // Now that everything has been pasted we need to update our deps and hierarchy.
  initialize_fixed_deps();
  initialize_dynamic_deps();
  update_hierarchy();
  update_deps();
}

void Entity::set_name(const std::string& name) {
  _name = name;
}

bool Entity::has_child_name(const std::string& name) const {
  if (_children.count(name)) {
    return true;
  }
  return false;
}

void Entity::make_child_name_unique(std::string& name) const {
  if (!_children.count(name)) {
    return;
  }

  size_t last_index = name.find_last_not_of("0123456789");
  std::string suffix = name.substr(last_index + 1);
  std::string prefix = name.substr(0,last_index+1);
  size_t number=0;
  try {
    number = boost::lexical_cast<size_t>(suffix);
  } catch (...) {
    number = 1;
  }

  bool exists = true;
  while(exists) {
    name = prefix + boost::lexical_cast<std::string>(number);
    if (!_children.count(name)) {
      exists = false;
    }
    number+=1;
  }
}

// Note paths always omit the name of the top most node,
// as there is only one unique one to use.
Path Entity::get_path() const {
  Path path;
  if (_parent) {
    path = _parent->get_path();
    path.push_back(get_name());
    return path;
  }
  // Note the top most node's name is omitted.
  return path;
}

// Note paths always omit the name of the top most node,
// as there is only one unique one to use.
Entity* Entity::get_entity_helper(const Path& path) const {
  Entity* e = const_cast<Entity*>(this);
  if (path.is_absolute()) {
    e = get_app_root();
  }
  for (const std::string& element: path.get_elements()) {
    if (element == Path::kCurrentDir) {
      continue;
    } else if (element == Path::kParentDir) {
      e = e->get_parent();
      if (!e) {
        return NULL;
      }
    } else if (!e->has_child_name(element)) {
      return NULL;
    } else {
      e = e->get_child(element);
    }
  }
  return e;
}

Entity* Entity::has_entity(const Path& path) const {
  Entity* e = get_entity_helper(path);
  return e;
}

Entity* Entity::get_entity(const Path& path) const {
  Entity* e = get_entity_helper(path);
  assert(e);
  return e;
}

void Entity::destroy_entity(const Path& path) const {
  Entity* e = get_entity_helper(path);
  delete_ff(e);
}


std::string Entity::copy_to_string(std::unordered_set<Entity*>& children) const{
  if (children.empty()) {
    return "";
  }

  std::stringstream ss;
  SimpleSaver saver(ss);

  copy(saver,children);
  return ss.str();
}

void Entity::paste_from_string(const std::string& raw) {
  clear_last_pasted();

  Bits* bits = create_bits_from_string(raw);
  SimpleLoader loader(bits);
  paste(loader);
}

const std::unordered_set<Entity*>& Entity::get_last_pasted() {
  return _last_pasted;
}

void Entity::clear_last_pasted() {
  _last_pasted.clear();
}

// Saves out only the specified children of this entity.
void Entity::copy(SimpleSaver& saver, const std::unordered_set<Entity*>& children) const {
  if (children.empty()) {
    return;
  }
  // Save the number of entities.
  size_t num_nodes = children.size();
  saver.save(num_nodes);

  // Save the entities.
  for (Entity* e : children) {
    e->save(saver);
  }
}

void Entity::paste_without_merging(SimpleLoader& loader) {
  // We create a novel namespace group so that we can load without name collisions.
  // Then we move them back into this entity, renaming the entities if there are collisions.
  // The references between components are based on pointers and not names,
  // so we don't need to fix up any references.
  Entity* folder = new_ff Entity(this, "__internal_folder__");
  folder->paste_with_merging(loader);

  folder->initialize_fixed_deps();
  folder->initialize_dynamic_deps();
  folder->update_hierarchy();
  folder->update_deps();
  folder->bake_paths();


  // Record the set of entities that we've pasted.
  std::unordered_set<Entity*> entities_loaded;
  std::unordered_set<Entity*> links_loaded;

  // Move and rename entities back into this entity.
  const NameToChildMap copy = folder->get_children();
  for (auto &iter: copy) {
    // Here we have a slight creeping of specialized logic, for the
    // GroupNodeEntity's links, inputs, and outputs folders.
    // Skip the inputs and outputs folders.
    // We rely on GroupNodeShape::update_hierarchy to create them for us.
    if ((iter.first == "inputs") || (iter.first == "outputs") ){
      continue;
    }
    // For the links folder we paste_with_out_merging the contents of the folder.
    else if (iter.first == "links") {
      Entity* src_links_folder = iter.second;
      Entity* target_links_folder = get_child("links");
      const NameToChildMap src_links = src_links_folder->get_children();
      for (auto &iter2: src_links) {
        Entity* child = iter2.second;
        target_links_folder->take_child(child);
        links_loaded.insert(child);
      }
    }
    // Otherwise we move over the child.
    else {
      Entity* child = iter.second;
      take_child(child);
      entities_loaded.insert(child);
    }
  }

  // Bake the current dep paths.
  for (Entity* e: entities_loaded) {
    e->bake_paths();
  }
  for (Entity* e: links_loaded) {
    e->bake_paths();
  }

  // Record the entites that we pasted.
  _last_pasted = entities_loaded;

  // Cleanup.
  delete_ff(folder);
}

void Entity::paste_with_merging(SimpleLoader& loader) {
  // Load the number of entities.
  size_t num_entities;
  loader.load(num_entities);

  // Record the set of entities that we've pasted.
  std::unordered_set<Entity*> entities_loaded;

  // Load the entities.
  for (size_t i=0; i<num_entities; ++i) {
    Entity* e = paste_entity_with_merging(loader);
    entities_loaded.insert(e);
  }

  // Find and collect any internal nodes which were not specified in the file
  // and are not used by the inputs and outputs.
  // Note the input and output nodes share the same namespace as the internal nodes.
  std::unordered_set<Entity*> entities_to_destroy;
  for (auto &e: get_children()) {
    if (entities_loaded.count(e.second) == 0) {
      entities_to_destroy.insert(e.second);
    }
  }

  // Delete nodes not specified in the file.
  for (Entity* c: entities_to_destroy) {
    delete_ff(c);
  }
}

Entity* Entity::paste_entity_with_merging(SimpleLoader& loader) {
  // Peek at the file.
  size_t did;
  loader.load(did);
  std::string name;
  loader.load(name);

  // Does a child by this name already exist inside this entity.
  Entity* entity = get_child(name);

  // If the entities exists with the wrong type, then delete it.
  if (entity && entity->get_did()!=did) {
    delete_ff(entity);
    entity=NULL;
  }

  // If the entity is missing, create one.
  if (!entity) {
    BaseEntityInstancer* ei = get_app_root()->get<BaseFactory>()->get_entity_instancer();
    entity = ei->instance(this, name, did);
  }
  entity->load_helper(loader);

  assert(entity);
  return entity;
}

}
