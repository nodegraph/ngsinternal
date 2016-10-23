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

  // Asynchronous Component Cleaning.
  virtual void set_ultimate_target(Entity* entity, bool force_stack_reset = false) = 0;
  virtual void clear_ultimate_target() = 0;
  virtual void continue_cleaning_to_ultimate_target() = 0;
  virtual bool is_busy_cleaning() = 0;

  // Show the processing marker on a node.
  virtual void set_processing_node(Entity* entity) = 0;
  virtual void clear_processing_node() = 0;
  virtual void set_error_node() = 0; // Show error marker on the current compute node.
  virtual void clear_error_node() = 0;
  virtual void update_clean_marker(Entity* entity, bool clean) = 0;

  // Builds and positions a compute node under the lowest node in the node graph.
  // If possible it will also link the latest node with the lowest.
  virtual Entity* build_and_link_compute_node(ComponentDID compute_did, const QVariantMap& chain_state) = 0;

  // Update inputs and outputs configuration for the gui side.
  virtual void set_input_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering) = 0;
  virtual void set_output_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering) = 0;

  virtual void destroy_link(Entity* input_entity) = 0;
  virtual Entity* create_link() = 0;
  virtual Entity* connect_plugs(Entity* input_entity, Entity* output_entity) = 0; // Returns the entity for the link.
};

}
