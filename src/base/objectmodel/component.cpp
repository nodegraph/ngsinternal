#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/entity.h>
#include <base/objectmodel/dep.h>
#include <base/objectmodel/deploader.h>


#include <base/utils/simplesaver.h>

#include <cassert>
#include <cstddef>
#include <iostream>

namespace ngs {

Component::Component(Entity* entity, size_t interface_id, size_t derived_id)
    : _entity(entity),
      _iid(interface_id),
      _did(derived_id),
      _dirty(true) {
  if (_entity) {
    _entity->add(this);
  }
  _dep_loader = new_ff DepLoader();
}

// Destruction of a Component will not destroy any other depedencies or dependants.
// This is because:
// * The Entities owns the components.
// * Components are not linked only inter-entity wise, but intra-entity wise as well.
// * When components are linked intra-entity wise they reference components in other
//   entities which should not be destroyed.

Component::~Component() {

  delete_ff(_dep_loader);

  // Unregister ourself as a dependant from our dependencies.
  for (auto &iid_iter: _dependencies) {
    DepLinks copied = iid_iter.second;
    for (auto &dep_iter : copied) {
      // If the link is still alive, then disconnect from it.
      // The dep is alive if it's lockable and the raw dependency pointer is non-null.
      DepLinkPtr link = dep_iter.second.lock();
      if (link && link->dependency) {
        disconnect_from_dep(link->dependency);
      }
      // Set the link's dependant pointer to null, as we're being destroyed now.
      link->dependant = NULL;
    }
  }

  // Unregister ourself as a dependency from our dependants.
  for (auto &iid_iter: _dependants) {
    const Components copied = iid_iter.second;
    for (Component* dependant : copied) {
      dependant->disconnect_from_dep(this);
    }
  }

  // Remove ourself from our entity.
  our_entity()->remove(this);

}

void Component::initialize_wires() {
  // Assumes we're already dirty.
  _dep_loader->update_fixed_wires();
  _dep_loader->update_dynamic_wires();
}

void Component::initialize_gl_helper() {
  // Assumes we're already dirty.
  // Initialize in a depth first fashion.
  if (is_initialized_gl()) {
    return;
  }
  for (auto &iid_iter: _dependencies) {
    const DepLinks& dep_iter = iid_iter.second;
    for (auto &dep : dep_iter) {
      // The dep is alive if it's lockable and the raw dependency pointer is non-null.
      DepLinkPtr link = dep.second.lock();
      if (link && link->dependency) {
        Component* c = link->dependency;
        c->initialize_gl_helper();
      }
    }
  }
  initialize_gl();
}

Entity* Component::our_entity() const {
  return _entity;
}

const std::string& Component::get_name() const {
  return our_entity()->get_name();
}

Entity* Component::get_entity(const Path& path) const {
  return our_entity()->get_entity(path);
}

Entity* Component::has_entity(const Path& path) const {
  return our_entity()->has_entity(path);
}

void Component::destroy_entity(const Path& path) const {
  our_entity()->destroy_entity(path);
}

Entity* Component::get_app_root() const {
  return our_entity()->get_app_root();
}

Entity* Component::get_root_group() const {
  return our_entity()->get_root_group();
}

size_t Component::get_iid() const {
  return _iid;
}

size_t Component::get_did() const {
  return _did;
}

std::string Component::get_path_as_string() const {
  return our_entity()->get_path().get_as_string();
}

void Component::pre_save(SimpleSaver& saver) const {
  size_t derived_id = get_did();
  saver.save(derived_id);
}

void Component::save(SimpleSaver& saver) const {
  pre_save(saver);
  _dep_loader->save(saver);
}

void Component::load(SimpleLoader& loader) {
  _dep_loader->load(loader);
}

void Component::bake_paths() {
  _dep_loader->bake_paths();
}

DepLinkPtr Component::get_dep(Entity* e, size_t iid) {
  Component* dependency = e->get(iid);
  if (!dependency) {
    return DepLinkPtr();
  }
  return connect_to_dep(dependency);
}

DepLinkPtr Component::get_dep(const Path& path, size_t iid) {
  Entity* e = get_entity(path);
  if (!e) {
    return DepLinkPtr();
  }
  return get_dep(e, iid);
}

DepLinkPtr Component::connect_to_dep(Component* c) {
  external();

  // If this is going to create a cycle return NULL.
  if (dep_creates_cycle(c)) {
    //assert(false);
    std::cerr << "Warning: cycle was detected so ignoring connect to dep request\n";
    return DepLinkPtr();
  }

  // Otherwise.
  DepLinkPtr ptr = register_dependency(c);
  c->register_dependant(this);
  return ptr;
}

void Component::disconnect_from_dep(Component* c) {
  external();
  unregister_dependency(c);
  c->unregister_dependant(this);
}

// --------------------------------------------------------------
// Dependants.
// --------------------------------------------------------------

// Registering a dependant never makes us dirty.
// However it does make our dependant dirty.
void Component::register_dependant(Component* c) const {
  external();
  // We don't check for cycles here because if we're registering a dependant,
  // then we've already checked for cycles when we're registering the
  // dependency (the other end).
  Components& coms = _dependants[c->get_iid()];
  coms.insert(c);
}

// Unregistering a dependant never makes us dirty.
// However it does make our dependant dirty.
void Component::unregister_dependant(Component* c) const {
  external();
  Components& coms = _dependants[c->get_iid()];
  coms.erase(c);
}

// --------------------------------------------------------------
// Dependencies.
// --------------------------------------------------------------

// Returns true if there is a dependency path between the dependant and dependency.
bool Component::is_recursive_dependency(const Component* dependency) const {
  external();
  if (this == dependency) {
    return true;
  }
  for (auto &iid_iter: _dependencies) {
    for (auto &dep_iter : iid_iter.second) {
      DepLinkPtr ptr = dep_iter.second.lock();
      if (ptr && ptr->dependency) {
        if (ptr->dependency->is_recursive_dependency(dependency)) {
          return true;
        }
      }
    }
  }
  return false;
}

bool Component::dep_creates_cycle(const Component* dependency) const {
  external();
  // The proposed link between us and the dependency would create a cycle
  // if the dependency is actually dependant on us.
  if (dependency->is_recursive_dependency(this)) {
    std::cerr << "Warning the requested component would have created a cycle so a null reference is likely being returned.\n";
    //assert(false);
    return true;
  }
  return false;
}

DepLinkPtr Component::get_dep_link(Component* c) const {
  external();
  const size_t iid = c->get_iid();
  if (_dependencies.count(c->get_iid())) {
    const DepLinks &links = _dependencies.at(iid);
    if (links.count(c)) {
      DepLinkPtr ptr = links.at(c).lock();
      if(!ptr) {
        std::cerr << "Error: we shouldn't have any unlockable weak ptrs in here.\n";
        assert(false);
      }
      return ptr;
    }
  }
  return DepLinkPtr();
}

void Component::set_dep_link(Component* c, DepLinkPtr link) {
  external();
  const size_t iid = c->get_iid();
  DepLinks &links = _dependencies[iid];
  assert(links.count(c) == 0);
  links.insert({c,DepLinkWPtr(link)});

  // Newly added dependencies are dirty with respect to us so that we can update from it.
  _dirty_dependencies.insert(c);
}

DepLinkPtr Component::register_dependency(Component* c) {
  external();
  // We assume cycle check has already been performed.

  // See if we have an existing dep link.
  DepLinkPtr bp = get_dep_link(c);

  // If we do, then make sure it's dependency member is still pointing to c.
  // When the link is broken, while there are outstanding shared_ptrs the
  // dependency will get set to NULL>
  if (bp) {
    bp->dependency = c;
    return bp;
  }

  // Otherwise we need to create a dep link.
  DepLinkPtr dp(new_ff DepLink(this, c));
  set_dep_link(c, dp);
  return dp;
}

// Unregistering a dependency make us dirty.
void Component::unregister_dependency(Component* c) {
  external();
  remove_dep_link(c, c->get_iid());
}

void Component::remove_dep_link(Component* c, size_t iid) {
  external();
  // Removing a dependency can never create cycles.
  DepLinks& dep_links = _dependencies[iid];
  std::shared_ptr<DepLink> dep_link;
  if (dep_links.count(c)) {
    dep_link = dep_links.at(c).lock();
    if (dep_link) {
      // If there are outstanding shared pointers, then set the dependency to NULL.
      // The outstanding shared pointers will deference to NULL.
      dep_link->dependency = NULL;
    } else {
      // If there are no outstanding shared pointers, we can remove this entry.
      dep_links.erase(c);
    }
  }
}

// --------------------------------------------------------------
// Dirty/Clean Logic.
// --------------------------------------------------------------
void Component::propagate_dirtiness(Component* dirty_source) {
  // --------------------------------------------------------
  // **Note** We don't call start_method here.
  // This is a special case.
  // --------------------------------------------------------

  // Record the source of this dirtiness.
  if (dirty_source != this) {
#ifdef DEBUG_OBJECT_MODEL
    DepLinks& links = _dependencies[dirty_source->get_iid()];
    assert(links.count(dirty_source));
    DepLinkPtr link = links[dirty_source].lock();
    assert(link);
#endif
    _dirty_dependencies.insert(dirty_source);
  }

  // If we're already dirty there is no need to continue..
  if (_dirty) {
    return;
  }

  // Set ourselves dirty.
  _dirty = true;

  // Recurse the dirtiness to our dependants.
  for (auto &iid_iter: _dependants) {
    for (Component* dependant : iid_iter.second) {
      dependant->propagate_dirtiness(this);
    }
  }
}

void Component::clean_dependencies() {
  // If we're already clean there is nothing to do.
  if (!_dirty) {
    return;
  }

  // Otherwise first propagate cleanliness to our children.
  for (auto & iid_iter: _dependencies) {
    for (auto & dep_iter : iid_iter.second) {
      DepLinkPtr link = dep_iter.second.lock();
      if (link && link->dependency) {
          link->dependency->propagate_cleanliness();
      }
    }
  }
}

void Component::clean_self() {
  if (!_dirty) {
    return;
  }

  // Now we clean/update our internal state.
  update_state();

  // Our dependencies is now marked clean.
  // Note that all dependencies are actually clean before the
  // update_state() call. The _dirty_dependencies var allows update_state()
  // to optimize based on which dependencies were actually dirty.
  _dirty_dependencies.clear();

  // We are now clean.
  _dirty = false;
}

void Component::propagate_cleanliness() {
  clean_dependencies();
  clean_self();
}

void Component::clean_wires() {
  // Update our wires.
  update_wires();

  // Update the wires of our dependencies.
  for (auto & iid_iter: _dependencies) {
    for (auto & dep_iter : iid_iter.second) {
      DepLinkPtr link = dep_iter.second.lock();
      if (link && link->dependency) {
          link->dependency->clean_wires();
      }
    }
  }
}

void Component::dirty_state() {
  propagate_dirtiness(this);
}

void Component::clean_state() {
  propagate_cleanliness();
}



}
