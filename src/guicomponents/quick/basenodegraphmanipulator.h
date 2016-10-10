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

  // Show the processing marker on a node.
  virtual void set_compute_node(Entity* entity) = 0;
  virtual void clear_compute_node() = 0;
  virtual void set_error_node() = 0; // Show error marker on the current compute node.
  virtual void clear_error_node() = 0;
  virtual void update_clean_marker(Entity* entity, bool clean) = 0;

  // Builds and positions a compute node under the lowest node in the node graph.
  // If possible it will also link the latest node with the lowest.
  virtual Entity* build_and_link_compute_node(ComponentDID compute_did, const QVariantMap& chain_state) = 0;

  // Update inputs and outputs configuration for the gui side.
  virtual void set_input_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering) = 0;
  virtual void set_output_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering) = 0;


};

}
