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
      _edit_node(this),
      _view_node(this),
      _processing_node(this),
      _locked(false),
      _accumulate(false){
}

NodeSelection::~NodeSelection() {
}

void NodeSelection::update_state() {
  internal();
  if (!_edit_node) {
    _edit_node.reset();
  }
  if (!_view_node) {
    _view_node.reset();
  }

  DepUSet<NodeShape>::iterator iter = _selected.begin();
  while(iter != _selected.end()) {
    if (!*iter) {
      iter = _selected.erase(iter);
    } else {
      ++iter;
    }
  }
}

void NodeSelection::set_edit_node(const Dep<NodeShape>& node) {
  external();
  if (_edit_node) {
    _edit_node->edit(false);
  }
  node->edit(true);
  _edit_node = node;
}

void NodeSelection::set_view_node(const Dep<NodeShape>& node) {
  external();
  if (_view_node) {
    _view_node->view(false);
  }
  node->view(true);
  _view_node = node;
}

void NodeSelection::set_processing_node(const Dep<NodeShape>& node) {
  external();
  if (_processing_node) {
    _processing_node->processing(false);
  }
  node->processing(true);
  _processing_node = node;
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

void NodeSelection::clear_edit_node() {
  external();
  if (_edit_node) {
    _edit_node->edit(false);
  }
  _edit_node.reset();
}

void NodeSelection::clear_view_node() {
  external();
  if (_view_node) {
    _view_node->view(false);
  }
  _view_node.reset();
}

void NodeSelection::clear_processing_node() {
  external();
  if (_processing_node) {
    _processing_node->view(false);
  }
  _processing_node.reset();
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
  _selected.insert(dep);
}

void NodeSelection::deselect(const Dep<NodeShape>& e) {
  external();
  if (e) {
    e->select(false);
  }
  _selected.erase(e);
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
  if (_selected.count(e)) {
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
  return _selected;
}

void NodeSelection::clear_selection() {
  external();
  // We don't call the deselect() method which erase elements one by one as it's slow.
  // We clear everything at once.
  for (const Dep<NodeShape>& d: _selected) {
    if (d) {
      d->select(false);
    }
  }
  _selected.clear();
}

void NodeSelection::destroy_selection() {
  external();
  for(const Dep<NodeShape>& cs: _selected) {
    if (!cs) {
      continue;
    }

    // Deselect it.
    cs->select(false);

    // If we're destroying the edit node, clean up the reference to it.
    if (cs == _edit_node) {
      clear_edit_node();
    }
    // If we're destroying the view node, clean up the reference to it.
    if (cs == _view_node) {
      clear_view_node();
    }

    Entity* e = cs->our_entity();
    delete_ff(e);
  }
  _selected.clear();
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
  if (_selected.empty()) {
    return;
  }
  // Determine the group entity.
  Dep<NodeShape> ns = *_selected.begin();
  Entity* node = ns->our_entity();
  Entity* group = node->get_parent();

  // Convert the comp shapes to their entities.
  std::unordered_set<Entity*> selected_entities;
  for (const auto &s: _selected) {
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
