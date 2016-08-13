#pragma once
#include <components/compshapes/compshapes_export.h>
#include <base/objectmodel/component.h>
#include <components/entities/componentids.h>
#include <base/objectmodel/dep.h>

#include <string>

namespace ngs {

class Entity;
class LinkableShape;

class COMPSHAPES_EXPORT NodeGraphSelection: public Component {
 public:

  COMPONENT_ID(NodeGraphSelection, NodeGraphSelection);

  NodeGraphSelection(Entity* entity);
  ~NodeGraphSelection();

  // Our state.
  virtual void update_state();

  // Edit node.
  void set_edit_node(const Dep<LinkableShape>& node);
  const Dep<LinkableShape>& get_edit_node() const;
  void clear_edit_node();

  // View node.
  void set_view_node(const Dep<LinkableShape>& node);
  const Dep<LinkableShape>& get_view_node() const;
  void clear_view_node();

  // The selection is global throughout the system.
  void select(const Dep<LinkableShape>& e);
  void deselect(const Dep<LinkableShape>& e);

  void select(const DepUSet<LinkableShape>& set);
  void deselect(const DepUSet<LinkableShape>& set);

  bool is_selected(const Dep<LinkableShape>& cs) const;
  void toggle_selected(const Dep<LinkableShape>& cs);

  const DepUSet<LinkableShape>& get_selected() const;
  void clear_selection();
  void destroy_selection();

  // Clear all references to nodes.
  void clear_all();

  // Copy and Paste.
  virtual void copy();
  virtual void paste(Entity* group);

 private:

  // We have exclusive control over the following shapes.
  // It is similar to object composition and encapsulation.
  // Except we're using our Dep dependency managers.

  // Our dynamic deps. These are not serialized.
  // Our edit and view nodes.
  Dep<LinkableShape> _edit_node;
  Dep<LinkableShape> _view_node;

  // The current selection.
  DepUSet<LinkableShape> _selected;

  // Our copied nodes.
  std::string _raw_copy;

  // Lock nodes and links from being moved.
  bool _locked;

  // Mode where newly pressed nodes are accumulated into selection.
  bool _accumulate;
};

}
