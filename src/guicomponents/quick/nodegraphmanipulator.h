#pragma once
#include <base/objectmodel/component.h>
#include <guicomponents/quick/quick_export.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>

#include <guicomponents/quick/basenodegraphmanipulator.h>
#include <string>

#include <QtCore/QVariantMap>

namespace ngs {

class BaseFactory;
class NodeSelection;
class NodeGraphQuickItem;

// This is the implementation class for the NodeGraphManipulator.
// This is Imp instance is held by a raw pointer in NodeGraphManipulator
// to avoid dependency cycles.
// This allows us to call this from the non-gui side (eg computes) to update
// the gui side to reflect non-gui side changes.
class Imp: public Component {
 public:
  COMPONENT_ID(InvalidComponent, InvalidComponent);
  Imp(Entity* e);

  // Update current compute markers on nodes.
  void set_compute_node(Entity* entity);
  void clear_compute_node();

  // Builds and positions a compute node under the lowest node in the node graph.
  // If possible it will also link the latest node with the lowest.
  Entity* build_and_link_compute_node(ComponentDID compute_did, const QVariantMap& chain_state);

 private:
  Entity* build_compute_node(ComponentDID compute_did, const QVariantMap& chain_state);
  void link(Entity* downstream);

  Dep<BaseFactory> _factory;
  Dep<NodeSelection> _node_selection;
  Dep<NodeGraphQuickItem> _ng_quick;
};


// This class wraps Imp
class QUICK_EXPORT NodeGraphManipulator: public BaseNodeGraphManipulator {
 public:
  COMPONENT_ID(BaseNodeGraphManipulator, NodeGraphManipulator);

  NodeGraphManipulator(Entity* entity);
  virtual ~NodeGraphManipulator();

  virtual void set_compute_node(Entity* entity);
  virtual void clear_compute_node();

  virtual Entity* build_and_link_compute_node(ComponentDID compute_did, const QVariantMap& chain_state);

private:
  Imp* _imp;
};

}
