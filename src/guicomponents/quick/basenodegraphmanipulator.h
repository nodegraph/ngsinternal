#pragma once
#include <guicomponents/quick/quick_export.h>
#include <guicomponents/comms/guitypes.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/componentids.h>
#include <base/objectmodel/entityids.h>

class QJsonObject;
class QJsonValue;
class QString;

namespace ngs {

class Entity;

class BaseNodeGraphManipulator: public Component {
 public:
  COMPONENT_ID(BaseNodeGraphManipulator, InvalidComponent);

  BaseNodeGraphManipulator(Entity* entity, ComponentDID did):Component(entity, kIID(), did){}
  virtual ~BaseNodeGraphManipulator(){}

  virtual void receive_message(const QString& msg) = 0;

  // Set Ultimate Targets.
  virtual void set_ultimate_targets(Entity* entity, bool force_stack_reset = false) = 0;
  virtual void set_ultimate_targets(const std::unordered_set<Entity*>& entities, bool force_stack_reset) = 0;
  virtual void set_inputs_as_ultimate_targets(Entity* node_entity) = 0;

  // Ultimate Target Cleaning.
  virtual void clear_ultimate_targets() = 0;
  virtual void continue_cleaning_to_ultimate_targets() = 0;
  virtual void continue_cleaning_to_ultimate_targets_on_idle() = 0;
  virtual bool is_busy_cleaning() = 0;

  // Update current compute markers on nodes.
  virtual void set_processing_node(Entity* entity) = 0;
  virtual void clear_processing_node() = 0;
  virtual void set_error_node(const QString& error_message) = 0; // Show error marker on the current compute node.
  virtual void clear_error_node() = 0;
  virtual void update_clean_marker(Entity* entity, bool clean) = 0;

  // Group logic..
  virtual void clean_enter_node(Entity* group) = 0;
  virtual void clean_exit_node(Entity* group) = 0;
  virtual void dive_into_group(const std::string& child_group_name) = 0;
  virtual void surface_from_group() = 0;

  // Update inputs and outputs configuration for the gui side.
  virtual void set_input_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering) = 0;
  virtual void set_output_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering) = 0;

  // Node creation.
  virtual Entity* create_node(bool centered, EntityDID entity_did, const std::string& name = "", Entity* group_entity=NULL) = 0;
  virtual Entity* create_compute_node(bool centered, EntityDID entity_did, ComponentDID compute_did, const std::string& name = "", Entity* group_entity=NULL) = 0;
  virtual Entity* create_user_macro_node(bool centered, const std::string& macro_name, const std::string& name = "", Entity* group_entity=NULL) = 0;
  virtual Entity* create_app_macro_node(bool centered, const std::string& macro_name, const std::string& name = "", Entity* group_entity=NULL) = 0;
  virtual Entity* create_input_node(bool centered, const QJsonValue& value, const std::string& name = "", Entity* group_entity=NULL) = 0;
  virtual Entity* create_data_node(bool centered, const QJsonValue& value, const std::string& name = "", Entity* group_entity=NULL) = 0;
  virtual Entity* create_browser_node(bool centered, ComponentDID compute_did, const QJsonObject& chain_state, const std::string& name = "", Entity* group_entity=NULL) = 0;

  // Link a node.
  virtual void link_to_closest_node(Entity* node) = 0;

  // Link Manipulation.
  virtual void destroy_link(Entity* input_entity) = 0;
  virtual Entity* create_link(Entity* group) = 0;
  virtual Entity* connect_plugs(Entity* input_entity, Entity* output_entity) = 0; // Returns the entity for the link.
  virtual void copy_description_to_input_node(Entity* input_entity, Entity* input_node_entity) = 0;

  // Graph Dirtiness.
  virtual void bubble_group_dirtiness(Entity* dirty_node_entity) = 0;

  // Specialized Node Overrides.
  virtual void set_mqtt_override(const Path& node_path, const QString& topic, const QString& payload) = 0;
  virtual void set_firebase_override(const Path& node_path, const QString& data_path, const QJsonValue& value) = 0;

  virtual void send_post_value_signal(int post_type, const QString& title, const QJsonObject& value) = 0;
};

}
