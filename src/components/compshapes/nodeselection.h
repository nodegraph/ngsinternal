#pragma once
#include <components/compshapes/compshapes_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/componentids.h>
#include <base/objectmodel/entityids.h>
#include <base/objectmodel/dep.h>

#include <string>

namespace ngs {

class Entity;
class NodeShape;
class BaseFactory;

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
  void set_processing_node_entity(Entity* node);
  void set_processing_node(const Dep<NodeShape>& node);
  const Dep<NodeShape>& get_processing_node() const;
  const Dep<NodeShape>& get_last_processing_node() const;
  void clear_processing_node();


  // Tracks the node currently having an error with its compute.
  void set_error_node_entity(Entity* node);
  void set_error_node(const Dep<NodeShape>& node);
  const Dep<NodeShape>& get_error_node() const;
  void clear_error_node();

  // The selection is global throughout the system.
  void select(const Dep<NodeShape>& e);
  void deselect(const Dep<NodeShape>& e);

  void select(const DepUSet<NodeShape>& set);
  void deselect(const DepUSet<NodeShape>& set);

  bool is_selected(const Dep<NodeShape>& cs) const;
  void toggle_selected(const Dep<NodeShape>& cs);

  const DepUSet<NodeShape>& get_selected() const;
  void clear_selection();

  // Clear all references to nodes.
  void clear_all();

  // Copy and Paste.
  virtual void copy();
  virtual bool paste(Entity* group);

 protected:
  // Our state.
  virtual bool update_state();

 private:
  // Our fixed deps.
  Dep<BaseFactory> _factory;

  // We have exclusive control over the following shapes.
  // It is similar to object composition and encapsulation.
  // Except we're using our Dep dependency managers.

  // Our dynamic deps. These are not serialized.
  // Our edit and view nodes.
  Dep<NodeShape> _edit_node;
  Dep<NodeShape> _view_node;
  Dep<NodeShape> _error_node;

  // The processing node may be hidden inside a group, so it's not always the node displayed with the processing icon.
  Dep<NodeShape> _processing_node;
  Dep<NodeShape> _processing_icon_node;

  // The last processing entity is used to set the error marker.
  Dep<NodeShape> _last_processing_node;

  // The current selection.
  DepUSet<NodeShape> _selected_nodes;

  // Our copied nodes.
  std::string _raw_copy;

  // The did of the group from which the copy was performed.
  // We can only paste to other groups with the same did.
  std::unordered_set<EntityDID> _group_context_dids;

  // Lock nodes and links from being moved.
  bool _locked;

  // Mode where newly pressed nodes are accumulated into selection.
  bool _accumulate;
};

}
