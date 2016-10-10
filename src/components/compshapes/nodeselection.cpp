#include <base/device/transforms/keyinfo.h>
#include <base/device/transforms/viewparams.h>
#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/entity.h>
#include <components/interactions/groupinteraction.h>
#include <components/interactions/canvas.h>

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
      _edit_node_shape(this),
      _view_node_shape(this),
      _compute_node_shape(this),
      _error_node_shape(this),
      _locked(false),
      _accumulate(false){
}

NodeSelection::~NodeSelection() {
}

void NodeSelection::update_state() {
  internal();
  if (!_edit_node_shape) {
    _edit_node_shape.reset();
  }
  if (!_view_node_shape) {
    _view_node_shape.reset();
  }
  if (!_compute_node_shape) {
    _compute_node_shape.reset();
  }

  DepUSet<NodeShape>::iterator iter = _selected_node_shapes.begin();
  while(iter != _selected_node_shapes.end()) {
    if (!*iter) {
      iter = _selected_node_shapes.erase(iter);
    } else {
      ++iter;
    }
  }
}

void NodeSelection::set_edit_node(const Dep<NodeShape>& node) {
  external();
  if (_edit_node_shape) {
    _edit_node_shape->show_edit_marker(false);
  }
  node->show_edit_marker(true);
  _edit_node_shape = node;
}

void NodeSelection::set_view_node(const Dep<NodeShape>& node) {
  external();
  if (_view_node_shape) {
    _view_node_shape->show_view_marker(false);
  }
  node->show_view_marker(true);
  _view_node_shape = node;
}

void NodeSelection::set_compute_node_entity(Entity* node) {
  Dep<NodeShape> node_shape = get_dep<NodeShape>(node);
  set_compute_node(node_shape);
}

void NodeSelection::set_error_node_entity(Entity* node) {
  Dep<NodeShape> node_shape = get_dep<NodeShape>(node);
  set_error_node(node_shape);
}

void NodeSelection::set_compute_node(const Dep<NodeShape>& node) {
  external();
  if (_compute_node_shape) {
    _compute_node_shape->show_compute_marker(false);
  }
  node->show_compute_marker(true);
  _compute_node_shape = node;
}

void NodeSelection::set_error_node(const Dep<NodeShape>& node) {
  external();
  if (_error_node_shape) {
    _error_node_shape->show_error_marker(false);
  }
  node->show_error_marker(true);
  _error_node_shape = node;
}

const Dep<NodeShape>& NodeSelection::get_edit_node() const {
  external();
  return _edit_node_shape;
}

const Dep<NodeShape>& NodeSelection::get_view_node() const {
  external();
  return _view_node_shape;
}

const Dep<NodeShape>& NodeSelection::get_compute_node() const {
  external();
  return _compute_node_shape;
}

const Dep<NodeShape>& NodeSelection::get_error_node() const {
  external();
  return _error_node_shape;
}

void NodeSelection::clear_edit_node() {
  external();
  if (_edit_node_shape) {
    _edit_node_shape->show_edit_marker(false);
  }
  _edit_node_shape.reset();
}

void NodeSelection::clear_view_node() {
  external();
  if (_view_node_shape) {
    _view_node_shape->show_view_marker(false);
  }
  _view_node_shape.reset();
}

void NodeSelection::clear_compute_node() {
  external();
  if (_compute_node_shape) {
    _compute_node_shape->show_compute_marker(false);
  }
  _compute_node_shape.reset();
}

void NodeSelection::clear_error_node() {
  external();
  if (_error_node_shape) {
    _error_node_shape->show_error_marker(false);
  }
  _error_node_shape.reset();
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
  _selected_node_shapes.insert(dep);
}

void NodeSelection::deselect(const Dep<NodeShape>& e) {
  external();
  if (e) {
    e->select(false);
  }
  _selected_node_shapes.erase(e);
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
  if (_selected_node_shapes.count(e)) {
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
  return _selected_node_shapes;
}

void NodeSelection::clear_selection() {
  external();
  // We don't call the deselect() method which erase elements one by one as it's slow.
  // We clear everything at once.
  for (const Dep<NodeShape>& d: _selected_node_shapes) {
    if (d) {
      d->select(false);
    }
  }
  _selected_node_shapes.clear();
}

void NodeSelection::destroy_selection() {
  external();
  for(const Dep<NodeShape>& cs: _selected_node_shapes) {
    if (!cs) {
      continue;
    }

    // Deselect it.
    cs->select(false);

    // If we're destroying the edit node, clean up the reference to it.
    if (cs == _edit_node_shape) {
      clear_edit_node();
    }
    // If we're destroying the view node, clean up the reference to it.
    if (cs == _view_node_shape) {
      clear_view_node();
    }
    if (cs == _compute_node_shape) {
      clear_compute_node();
    }

    Entity* e = cs->our_entity();
    delete_ff(e);
  }
  _selected_node_shapes.clear();
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
  if (_selected_node_shapes.empty()) {
    return;
  }
  // Determine the group entity.
  Dep<NodeShape> ns = *_selected_node_shapes.begin();
  Entity* node = ns->our_entity();
  Entity* group = node->get_parent();

  // Convert the comp shapes to their entities.
  std::unordered_set<Entity*> selected_entities;
  for (const auto &s: _selected_node_shapes) {
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
