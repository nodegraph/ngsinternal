#include <base/device/transforms/keyinfo.h>
#include <base/device/transforms/viewparams.h>
#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/entity.h>
#include <components/interactions/groupinteraction.h>

#include <base/utils/path.h>
#include <base/utils/permit.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <components/compshapes/compshapecollective.h>
#include <components/compshapes/linkshape.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/nodeselection.h>
#include <components/compshapes/nodeshape.h>
#include <components/computes/compute.h>
#include <entities/entityids.h>
#include <components/computes/inputcompute.h>
#include <sstream>
#include <unordered_map>

namespace ngs {

NodeSelection::NodeSelection(Entity* entity)
    : Component(entity, kIID(), kDID()),
      _edit_node(this),
      _view_node(this),
      _processing_node(this),
      _error_node(this),
      _last_processing_node(this),
      _locked(false),
      _accumulate(false){
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
  node->show_processing_marker(true);
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
  if (_processing_node) {
    _processing_node->show_processing_marker(false);
  }
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

  // Convert the comp shapes to their entities.
  std::unordered_set<Entity*> selected_entities;
  for (const auto &s: _selected_nodes) {
    selected_entities.insert(s->our_entity());
  }

  // Serialize the selected to a string.
  _raw_copy = group->copy_to_string(selected_entities);
}

void NodeSelection::paste(Entity* group) {
  external();
  if (_raw_copy.empty()) {
    return;
  }
  group->paste_from_string(_raw_copy);
}

}
