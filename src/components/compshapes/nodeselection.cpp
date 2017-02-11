#include <base/device/transforms/keyinfo.h>
#include <base/device/transforms/viewparams.h>
#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/entity.h>
#include <base/objectmodel/entityids.h>
#include <components/interactions/groupinteraction.h>

#include <base/utils/path.h>
#include <base/utils/permit.h>
#include <base/objectmodel/basefactory.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <components/compshapes/compshapecollective.h>
#include <components/compshapes/linkshape.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/nodeselection.h>
#include <components/compshapes/nodeshape.h>
#include <components/computes/compute.h>
#include <components/computes/inputcompute.h>

#include <base/objectmodel/entityids.h>
#include <base/objectmodel/deploader.h>

#include <sstream>
#include <unordered_map>

namespace ngs {

NodeSelection::NodeSelection(Entity* entity)
    : Component(entity, kIID(), kDID()),
      _factory(this),
      _edit_node(this),
      _view_node(this),
      _processing_node(this),
      _processing_icon_node(this),
      _error_node(this),
      _last_processing_node(this),
      _locked(false),
      _accumulate(false) {
  get_dep_loader()->register_fixed_dep(_factory, Path());
}

NodeSelection::~NodeSelection() {
}

bool NodeSelection::update_state() {
  internal();
  if (!_edit_node) {
    _edit_node.reset();
  }
  if (!_view_node) {
    _view_node.reset();
  }
  if (!_processing_node) {
      _processing_node.reset();
  }
  if (!_error_node) {
      _error_node.reset();
  }

  DepUSet<NodeShape>::iterator iter = _selected_nodes.begin();
  while(iter != _selected_nodes.end()) {
    if (!*iter) {
      iter = _selected_nodes.erase(iter);
    } else {
      ++iter;
    }
  }
  return true;
}

void NodeSelection::set_edit_node(const Dep<NodeShape>& node) {
  external();
  clear_edit_node();
  node->show_edit_marker(true);
  _edit_node = node;
}

void NodeSelection::set_view_node(const Dep<NodeShape>& node) {
  external();
  clear_view_node();
  node->show_view_marker(true);
  _view_node = node;
}

void NodeSelection::set_processing_node_entity(Entity* node) {
  external();
  Dep<NodeShape> node_shape = get_dep<NodeShape>(node);
  set_processing_node(node_shape);
}

void NodeSelection::set_error_node_entity(Entity* node) {
  Dep<NodeShape> node_shape = get_dep<NodeShape>(node);
  set_error_node(node_shape);
}

void NodeSelection::set_processing_node(const Dep<NodeShape>& node) {
  external();
  if (!node) {
    return;
  }
  clear_processing_node();

  // Determine the node to show the processing marker on.
  // The processing node may not be in the current group but nested deep inside another group.
  // In this case we travel up the hierarchy looking for a group node in our current group.
  Entity* node_entity = node->our_entity();
  Entity* node_parent = node_entity->get_parent();
  // If the node_entity is null, we're at the app_root, and we never show the root group,
  // so in this case there is no node to mark.
  if (node_entity) {
    Entity* current_group = _factory->get_current_group();
    Entity* root_group = get_app_root();
    while(node_parent && (node_parent != root_group) && (node_parent != current_group)) {
      node_entity = node_parent;
      node_parent = node_entity->get_parent();
    }

    // Show the icon on the right node.
    _processing_icon_node  = get_dep<NodeShape>(node_entity);
    _processing_icon_node->show_processing_marker(true);
  }

  // Record the main processing node.
  _processing_node = node;
  _last_processing_node = node;
}

void NodeSelection::set_error_node(const Dep<NodeShape>& node) {
  external();
  clear_error_node();
  node->show_error_marker(true);
  _error_node = node;
}

const Dep<NodeShape>& NodeSelection::get_edit_node() const {
  external();
  return _edit_node;
}

const Dep<NodeShape>& NodeSelection::get_view_node() const {
  external();
  return _view_node;
}

const Dep<NodeShape>& NodeSelection::get_processing_node() const {
  external();
  return _processing_node;
}

const Dep<NodeShape>& NodeSelection::get_last_processing_node() const {
  external();
  return _last_processing_node;
}

const Dep<NodeShape>& NodeSelection::get_error_node() const {
  external();
  return _error_node;
}

void NodeSelection::clear_edit_node() {
  external();
  if (_edit_node) {
    _edit_node->show_edit_marker(false);
  }
  _edit_node.reset();
}

void NodeSelection::clear_view_node() {
  external();
  if (_view_node) {
    _view_node->show_view_marker(false);
  }
  _view_node.reset();
}

void NodeSelection::clear_processing_node() {
  external();
  if (_processing_icon_node) {
    _processing_icon_node->show_processing_marker(false);
  }
  _processing_icon_node.reset();
  _processing_node.reset();
}

void NodeSelection::clear_error_node() {
  external();
  if (_error_node) {
    _error_node->show_error_marker(false);
  }
  _error_node.reset();
}

void NodeSelection::select(const Dep<NodeShape>& e) {
  external();
  if (!e) {
    return;
  }
  // Make ourself be a dependant on the dependency.
  Dep<NodeShape> dep(this);
  dep = e;
  dep->select(true);
  _selected_nodes.insert(dep);
}

void NodeSelection::deselect(const Dep<NodeShape>& e) {
  external();
  if (e) {
    e->select(false);
  }
  _selected_nodes.erase(e);
}

void NodeSelection::select(const DepUSet<NodeShape>& set) {
  external();
  for (const Dep<NodeShape>& cs : set) {
    select(cs);
  }
}

void NodeSelection::deselect(const DepUSet<NodeShape>& set) {
  external();
  for (const Dep<NodeShape>& cs: set) {
    deselect(cs);
  }
}

bool NodeSelection::is_selected(const Dep<NodeShape>& e) const{
  external();
  if (_selected_nodes.count(e)) {
    return true;
  }
  return false;
}
void NodeSelection::toggle_selected(const Dep<NodeShape>& e) {
  external();
  if (is_selected(e)) {
    deselect(e);
  } else {
    select(e);
  }
}

const DepUSet<NodeShape>& NodeSelection::get_selected() const{
  external();
  return _selected_nodes;
}

void NodeSelection::clear_selection() {
  external();
  // We don't call the deselect() method which erase elements one by one as it's slow.
  // We clear everything at once.
  for (const Dep<NodeShape>& d: _selected_nodes) {
    if (d) {
      d->select(false);
    }
  }
  _selected_nodes.clear();
}

void NodeSelection::destroy_selection() {
  external();

  for(const Dep<NodeShape>& cs: _selected_nodes) {
    if (!cs) {
      continue;
    }
    Entity* e = cs->our_entity();
    Entity* group = e->get_parent();

    // Nodes that are not visible can never be selected. However just in case we skip invisible nodes.
    if (!cs->is_visible()) {
      continue;
    }

    // Nodes in certain groups cannot be destroyed.
    if (group->get_did() == EntityDID::kIfGroupNodeEntity) {
      const std::string& name = e->get_name();
      if ( (name == "in") || (name == "out") ) {
        continue;
      }
    }
    if (group->get_did() == EntityDID::kForEachGroupNodeEntity) {
      const std::string& name = e->get_name();
      if ( (name == "in") || (name == "out") || (name == "element")) {
        continue;
      }
    }
    if (group->get_did() == EntityDID::kWhileGroupNodeEntity) {
      const std::string& name = e->get_name();
      if ( (name == "in") || (name == "out") ) {
        continue;
      }
    }

    // Otherwise we destroy it.
    delete_ff(e);
  }
  _selected_nodes.clear();
  // Shoud any of our cached Dep<> references, reference a destroyed node they
  // will become null.
}

// Clear all references to nodes.
void NodeSelection::clear_all() {
  external();
  clear_selection();
  clear_edit_node();
  clear_view_node();
}

void NodeSelection::copy() {
  external();
  if (_selected_nodes.empty()) {
    return;
  }
  // Determine the group entity.
  Dep<NodeShape> ns = *_selected_nodes.begin();
  Entity* node = ns->our_entity();
  Entity* group = node->get_parent();

  // Record the copy group did. We will only paste to groups with the same did.
  _group_context_dids = group->get_group_context_dids();

  // Convert the comp shapes to their entities.
  std::unordered_set<Entity*> selected_entities;
  for (const auto &s: _selected_nodes) {
    selected_entities.insert(s->our_entity());
  }

  // Serialize the selected to a string.
  _raw_copy = group->copy_to_string(selected_entities);
}

bool NodeSelection::paste(Entity* group) {
  external();
  if (_raw_copy.empty()) {
    return false;
  }

  // The group context dids for the target location.
  std::unordered_set<EntityDID> target_dids = group->get_group_context_dids();

  // Make sure the target location has all the needed context dids.
  bool context_mismatch = false;
  for (auto did: _group_context_dids) {
    if (target_dids.count(did) == 0) {
      context_mismatch = true;
    }
  }
  if (context_mismatch) {
    std::cerr << "Error: the target group for the paste operation did not have appropriate surrounding group contexts\n";
    for (auto did: _group_context_dids) {
      std::cerr << "source: " << to_underlying(did) << "\n";
    }
    for (auto did: target_dids) {
      std::cerr << "dest: " << to_underlying(did) << "\n";
    }
    return false;
  }

  group->paste_from_string(_raw_copy);
  return true;
}

}
