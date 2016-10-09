#pragma once
#include <components/compshapes/compshapes_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>

#include <string>

namespace ngs {

class Entity;
class NodeShape;

class COMPSHAPES_EXPORT NodeSelection: public Component {
 public:

  COMPONENT_ID(NodeSelection, NodeSelection);

  NodeSelection(Entity* entity);
  ~NodeSelection();

  // Tracks the node currently being edited.
  void set_edit_node(const Dep<NodeShape>& node);
  const Dep<NodeShape>& get_edit_node() const;
  void clear_edit_node();

  // Tracks the node currently being viewed.
  void set_view_node(const Dep<NodeShape>& node);
  const Dep<NodeShape>& get_view_node() const;
  void clear_view_node();

  // Tracks the node currently performing its compute.
  void set_compute_node_entity(Entity* node);
  void set_compute_node(const Dep<NodeShape>& node);
  const Dep<NodeShape>& get_compute_node() const;
  void clear_compute_node();

  // The selection is global throughout the system.
  void select(const Dep<NodeShape>& e);
  void deselect(const Dep<NodeShape>& e);

  void select(const DepUSet<NodeShape>& set);
  void deselect(const DepUSet<NodeShape>& set);

  bool is_selected(const Dep<NodeShape>& cs) const;
  void toggle_selected(const Dep<NodeShape>& cs);

  const DepUSet<NodeShape>& get_selected() const;
  void clear_selection();
  void destroy_selection();

  // Clear all references to nodes.
  void clear_all();

  // Copy and Paste.
  virtual void copy();
  virtual void paste(Entity* group);

 protected:
  // Our state.
  virtual void update_state();

 private:

  // We have exclusive control over the following shapes.
  // It is similar to object composition and encapsulation.
  // Except we're using our Dep dependency managers.

  // Our dynamic deps. These are not serialized.
  // Our edit and view nodes.
  Dep<NodeShape> _edit_node_shape;
  Dep<NodeShape> _view_node_shape;
  Dep<NodeShape> _compute_node_shape;

  // The current selection.
  DepUSet<NodeShape> _selected_node_shapes;

  // Our copied nodes.
  std::string _raw_copy;

  // Lock nodes and links from being moved.
  bool _locked;

  // Mode where newly pressed nodes are accumulated into selection.
  bool _accumulate;
};

}
