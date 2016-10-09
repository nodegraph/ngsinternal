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

class QUICK_EXPORT NodeGraphManipulator: public BaseNodeGraphManipulator {
 public:
  COMPONENT_ID(BaseNodeGraphManipulator, NodeGraphManipulator);

  NodeGraphManipulator(Entity* entity);
  virtual ~NodeGraphManipulator();

  // Builds and positions a compute node under the lowest node in the node graph.
  // If possible it will also link the latest node with the lowest.
  virtual Entity* build_and_link_compute_node(ComponentDID compute_did, const QVariantMap& chain_state);

  // Mark a node as the current compute node.
  virtual void set_compute_node(Entity* entity);

private:
  Entity* build_compute_node(ComponentDID compute_did, const QVariantMap& chain_state);
  void link(Entity* downstream);

  Dep<BaseFactory> _factory;
  Dep<NodeSelection> _node_selection;
  Dep<NodeGraphQuickItem> _ng_quick;

};

}
