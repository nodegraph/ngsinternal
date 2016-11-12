#pragma once
#include <guicomponents/quick/quick_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>

class QJsonObject;
class QString;

namespace ngs {

class Entity;

class BaseNodeGraphManipulator: public Component {
 public:
  COMPONENT_ID(BaseNodeGraphManipulator, InvalidComponent);

  BaseNodeGraphManipulator(Entity* entity, ComponentDID did):Component(entity, kIID(), did){}
  virtual ~BaseNodeGraphManipulator(){}

  // Set Ultimate Targets.
  virtual void set_ultimate_targets(Entity* entity, bool force_stack_reset = false) = 0;
  virtual void set_ultimate_targets(const std::unordered_set<Entity*>& entities, bool force_stack_reset) = 0;
  virtual void set_inputs_as_ultimate_targets(Entity* node_entity) = 0;

  // Ultimate Target Cleaning.
  virtual void clear_ultimate_targets() = 0;
  virtual void continue_cleaning_to_ultimate_targets() = 0;
  virtual bool is_busy_cleaning() = 0;

  // Update current compute markers on nodes.
  virtual void set_processing_node(Entity* entity) = 0;
  virtual void clear_processing_node() = 0;
  virtual void set_error_node(const QString& error_message) = 0; // Show error marker on the current compute node.
  virtual void clear_error_node() = 0;
  virtual void update_clean_marker(Entity* entity, bool clean) = 0;

  // Lockable Groups.
  virtual void dive_into_lockable_group(const std::string& child_group_name) = 0;
  virtual void surface_from_lockable_group() = 0;

  // Builds and positions a compute node under the lowest node in the node graph.
  // If possible it will also link the latest node with the lowest.
  virtual Entity* build_and_link_compute_node(ComponentDID compute_did, const QJsonObject& chain_state) = 0;

  // Update inputs and outputs configuration for the gui side.
  virtual void set_input_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering) = 0;
  virtual void set_output_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering) = 0;

  // Link Manipulation.
  virtual void destroy_link(Entity* input_entity) = 0;
  virtual Entity* create_link() = 0;
  virtual Entity* connect_plugs(Entity* input_entity, Entity* output_entity) = 0; // Returns the entity for the link.

  // Graph Dirtiness.
  virtual void bubble_group_dirtiness() = 0;
  virtual void synchronize_graph_dirtiness(Entity* group_entity) = 0;
  virtual void dirty_compute(const Path& path) = 0;

  // Specialized Overrides.
  virtual void set_mqtt_override(const Path& node_path, const QString& topic, const QString& payload) = 0;
};

}
