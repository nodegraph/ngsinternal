#pragma once
#include <guicomponents/quick/quick_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>

#include <QtCore/QVariant>

namespace ngs {

class Entity;

class BaseNodeGraphManipulator: public Component {
 public:
  COMPONENT_ID(BaseNodeGraphManipulator, InvalidComponent);

  BaseNodeGraphManipulator(Entity* entity, ComponentDID did):Component(entity, kIID(), did){}
  virtual ~BaseNodeGraphManipulator(){}

  // Builds and positions a compute node under the lowest node in the node graph.
  // If possible it will also link the latest node with the lowest.
  virtual Entity* build_and_link_compute_node(ComponentDID compute_did, const QVariantMap& chain_state) = 0;

  // Show the processing marker on a node.
  virtual void set_compute_node(Entity* entity) = 0;
  virtual void clear_compute_node() = 0;

};

}
