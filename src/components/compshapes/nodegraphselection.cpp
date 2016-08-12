#include <base/device/transforms/keyinfo.h>
#include <base/device/transforms/viewparams.h>
#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/entity.h>
#include <components/interactions/groupinteraction.h>
#include <components/interactions/canvas.h>

#include <base/utils/path.h>
#include <components/compshapes/compshape.h>

#include <base/utils/permit.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <components/compshapes/compshapecollective.h>
#include <components/compshapes/nodegraphselection.h>
#include <components/compshapes/linkshape.h>
#include <components/compshapes/inputshape.h>
#include <components/computes/compute.h>
#include <components/entities/entityids.h>
#include <components/computes/inputcompute.h>
#include <sstream>
#include <unordered_map>

namespace ngs {

NodeGraphSelection::NodeGraphSelection(Entity* entity)
    : Component(entity, kIID(), kDID()),
      _edit_node(this),
      _view_node(this),
      _locked(false),
      _accumulate(false){
}

NodeGraphSelection::~NodeGraphSelection() {
}

void NodeGraphSelection::update_state() {
  if (!_edit_node) {
    _edit_node.reset();
  }
  if (!_view_node) {
    _view_node.reset();
  }

  DepUSet<CompShape>::iterator iter = _selected.begin();
  while(iter != _selected.end()) {
    if (!*iter) {
      iter = _selected.erase(iter);
    } else {
      ++iter;
    }
  }
}

void NodeGraphSelection::set_edit_node(const Dep<CompShape>& node) {
  start_method();
  if (_edit_node) {
    _edit_node->edit(false);
  }
  node->edit(true);
  _edit_node = node;
}

void NodeGraphSelection::set_view_node(const Dep<CompShape>& node) {
  start_method();
  if (_view_node) {
    _view_node->view(false);
  }
  node->view(true);
  _view_node = node;
}

const Dep<CompShape>& NodeGraphSelection::get_edit_node() const {
  start_method();
  return _edit_node;
}

const Dep<CompShape>& NodeGraphSelection::get_view_node() const {
  start_method();
  return _view_node;
}

void NodeGraphSelection::clear_edit_node() {
  start_method();
  if (_edit_node) {
    _edit_node->edit(false);
  }
  _edit_node.reset();
}

void NodeGraphSelection::clear_view_node() {
  start_method();
  if (_view_node) {
    _view_node->view(false);
  }
  _view_node.reset();
}

void NodeGraphSelection::select(const Dep<CompShape>& e) {
  start_method();
  if (!e) {
    return;
  }
  // Make ourself be a dependant on the dependency.
  Dep<CompShape> dep(this);
  dep = e;
  dep->select(true);
  _selected.insert(dep);
}

void NodeGraphSelection::deselect(const Dep<CompShape>& e) {
  start_method();
  if (e) {
    e->select(false);
  }
  _selected.erase(e);
}

void NodeGraphSelection::select(const DepUSet<CompShape>& set) {
  start_method();
  for (const Dep<CompShape>& cs : set) {
    select(cs);
  }
}

void NodeGraphSelection::deselect(const DepUSet<CompShape>& set) {
  start_method();
  for (const Dep<CompShape>& cs: set) {
    deselect(cs);
  }
}

bool NodeGraphSelection::is_selected(const Dep<CompShape>& e) const{
  start_method();
  if (_selected.count(e)) {
    return true;
  }
  return false;
}
void NodeGraphSelection::toggle_selected(const Dep<CompShape>& e) {
  start_method();
  if (is_selected(e)) {
    deselect(e);
  } else {
    select(e);
  }
}

const DepUSet<CompShape>& NodeGraphSelection::get_selected() const{
  return _selected;
}

void NodeGraphSelection::clear_selection() {
  start_method();
  // We don't call the deselect() method which erase elements one by one as it's slow.
  // We clear everything at once.
  for (const Dep<CompShape>& d: _selected) {
    if (d) {
      d->select(false);
    }
  }
  _selected.clear();
}

void NodeGraphSelection::destroy_selection() {
  for(const Dep<CompShape>& cs: _selected) {
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

    // If we have a selected link shape we want to destroy all the associated
    // connections as well.
    if (cs->get_did() == kLinkShape) {
      Dep<LinkShape> link_shape(cs, true);
      const Dep<InputShape>& input_shape = link_shape->get_input_shape();
      if (input_shape) {
        Dep<InputCompute> input_compute = get_dep<InputCompute>(input_shape->our_entity());
        // Unlink components.
        input_compute->unlink_output_compute();
      }
      // Destroy entities. This automatically disconnect many components.
      delete2_ff(link_shape->our_entity());
    } else {
      delete2_ff(cs->our_entity());
    }
  }
  _selected.clear();
}

// Clear all references to nodes.
void NodeGraphSelection::clear_all() {
  clear_selection();
  clear_edit_node();
  clear_view_node();
}

void NodeGraphSelection::clear_selected_links() {
  DepUSet<CompShape> copy = _selected;
  for (const auto &s: copy) {
    if (s->get_did() == kLinkShape) {
      deselect(s);
    }
  }
}

void NodeGraphSelection::copy() {
  if (_selected.empty()) {
    return;
  }
  Dep<CompShape> cs = *_selected.begin();
  Entity* node = cs->our_entity();
  Entity* group = node->get_parent();

  // Convert the comp shapes to their entities.
  std::unordered_set<Entity*> selected_entities;
  for (const auto &s: _selected) {
    selected_entities.insert(s->our_entity());
  }

  _raw_copy = group->copy_to_string(selected_entities);
}

void NodeGraphSelection::paste(Entity* group) {
  if (_raw_copy.empty()) {
    return;
  }
  group->paste_from_string(_raw_copy);
}

}
