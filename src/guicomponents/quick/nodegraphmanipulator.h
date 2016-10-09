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
// It is held by a raw pointer in NodeGraphManipulator to avoid dependency cycles.
// This allows us to call this from the non-gui side (eg computes) at arbitrary
// locations to update the gui side to reflect non-gui side changes.
class Imp: public Component {
 public:
  // Note that components with and IID of kIInvalidComponent, should not be
  // created with the app root as its parent as there may be other invalid components
  // which also get created later resulting in collisions.
  // Invalid components are always created under a null entity.
  // Note that an invalid component usually needs a reference to the app root entity
  // in order create Dep<>s to other components.
  COMPONENT_ID(InvalidComponent, InvalidComponent);
  Imp(Entity* app_root);

  virtual void initialize_wires();

  // Update current compute markers on nodes.
  void set_compute_node(Entity* entity);
  void clear_compute_node();

  // Builds and positions a compute node under the lowest node in the node graph.
  // If possible it will also link the latest node with the lowest.
  Entity* build_and_link_compute_node(ComponentDID compute_did, const QVariantMap& chain_state);

 private:
  Entity* build_compute_node(ComponentDID compute_did, const QVariantMap& chain_state);
  void link(Entity* downstream);

  Entity* _app_root;
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

  virtual void initialize_wires();

  virtual void set_compute_node(Entity* entity);
  virtual void clear_compute_node();

  virtual Entity* build_and_link_compute_node(ComponentDID compute_did, const QVariantMap& chain_state);

private:
  Imp* _imp;
};

}
