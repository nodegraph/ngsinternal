#pragma once
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>
#include <guicomponents/quick/quick_export.h>
#include <base/objectmodel/componentids.h>
#include <base/objectmodel/dep.h>

#include <guicomponents/quick/basenodegraphmanipulator.h>
#include <string>

#include <QtCore/QObject>
#include <QtCore/QTimer>

namespace ngs {


class BaseFactory;
class NodeSelection;
class NodeGraphQuickItem;
class TaskScheduler;
class Compute;


// This is the implementation class for the NodeGraphManipulator.
// It is held by a raw pointer in NodeGraphManipulator to avoid dependency cycles.
// This allows us to call this from the non-gui side (eg computes) at arbitrary
// locations to update the gui side to reflect non-gui side changes.
class NodeGraphManipulatorImp: public QObject, public Component {
Q_OBJECT
 public:
  // Note that components with and IID of kIInvalidComponent, should not be
  // created with the app root as its parent as there may be other invalid components
  // which also get created later resulting in collisions.
  // Invalid components are always created under a null entity.
  // Note that an invalid component usually needs a reference to the app root entity
  // in order create Dep<>s to other components.
  COMPONENT_ID(InvalidComponent, InvalidComponent);
  NodeGraphManipulatorImp(Entity* app_root);

  bool start_waiting(std::function<void()> on_idle);

  virtual void initialize_wires();

  // Set Ultimate Targets.
  virtual void set_ultimate_targets(Entity* entity, bool force_stack_reset = false);
  virtual void set_ultimate_targets(const std::unordered_set<Entity*>& entities, bool force_stack_reset = false);
  virtual void set_inputs_as_ultimate_targets(Entity* node_entity);

  // Ultimate Target Cleaning.
  virtual void clear_ultimate_targets();
  virtual void continue_cleaning_to_ultimate_targets();
  virtual void continue_cleaning_to_ultimate_targets_on_idle();
  virtual bool is_busy_cleaning();

  // Update current compute markers on nodes.
  virtual void set_processing_node(Entity* entity);
  virtual void clear_processing_node();
  virtual void set_error_node(const QString& error_message);
  virtual void clear_error_node();
  virtual void update_clean_marker(Entity* entity, bool clean);

  // Group logic.
  virtual void clean_enter_group(Entity* group);
  virtual void clean_exit_group(Entity* group);
  virtual void dive_into_group(const std::string& child_group_name);
  virtual void surface_from_group();

  // Update inputs and outputs configuration for the gui side.
  virtual void set_input_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering);
  virtual void set_output_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering);

  // Node creation.
  virtual Entity* create_node(bool centered, EntityDID entity_did, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_compute_node(bool centered, ComponentDID compute_did, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_user_macro_node(bool centered, const std::string& macro_name, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_app_macro_node(bool centered, const std::string& macro_name, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_input_node(bool centered, const QJsonValue& value, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_data_node(bool centered, const QJsonValue& value, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_browser_node(bool centered, ComponentDID compute_did, const QJsonObject& chain_state, const std::string& name = "", Entity* group_entity=NULL);

  // Link a node.
  virtual void link_to_closest_node(Entity* node);

  // Modify links.
  virtual void destroy_link(Entity* input_entity);
  virtual Entity* create_link(Entity* group);
  virtual Entity* connect_plugs(Entity* input_entity, Entity* output_entity);
  virtual void copy_description_to_input_node(Entity* input_entity, Entity* input_node_entity);

  // Graph Dirtiness.
  virtual void bubble_group_dirtiness();
  virtual void synchronize_graph_dirtiness(Entity* group_entity);
  virtual void dirty_compute(const Path& path);

  // Specialized Node Overrides.
  virtual void set_mqtt_override(const Path& node_path, const QString& topic, const QString& payload);
  virtual void set_firebase_override(const Path& node_path, const QString& data_path, const QJsonValue& value);

private slots:
  void on_idle_timer_timeout();

 private:
  virtual void synchronize_group_dirtiness(Entity* group_entity);
  virtual void dirty_group_from_internals(Entity* group_entity);
  virtual void dirty_internals_from_group(Entity* group_entity);

  virtual void finish_creating_node(Entity* entity, bool centered);

  void prune_clean_or_dead();

  Entity* _app_root;
  Dep<BaseFactory> _factory;
  Dep<NodeSelection> _selection;
  Dep<NodeGraphQuickItem> _ng_quick;
  Dep<TaskScheduler> _scheduler;

  // The ultimate compute (of a node) that we are trying to clean.
  // Note that there maybe many asynchronous computes which cause each cleaning pass over the dependencies
  // to finish early (returning false). Holding this reference to the ultimate component we want to clean
  // allows us to restart the cleaning process once other asynchronous cleaning processes finish.
  DepUSet<Compute> _ultimate_targets;

  // Wait for ultimate targets to become clean.
  QTimer _idle_timer;
  std::function<void()> _on_idle;
};


// ---------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------


// This class wraps an internal Component held by a raw pointer.
// This allows it to break dependency cycles.
class QUICK_EXPORT NodeGraphManipulator : public BaseNodeGraphManipulator {
 public:
  COMPONENT_ID(BaseNodeGraphManipulator, NodeGraphManipulator)

  NodeGraphManipulator(Entity* entity);
  virtual ~NodeGraphManipulator();

  // Set Ultimate Targets.
  virtual void set_ultimate_targets(Entity* entity, bool force_stack_reset);
  virtual void set_ultimate_targets(const std::unordered_set<Entity*>& entities, bool force_stack_reset);
  virtual void set_inputs_as_ultimate_targets(Entity* node_entity);

  // Ultimate Target Cleaning.
  virtual void clear_ultimate_targets();
  virtual void continue_cleaning_to_ultimate_targets();
  virtual void continue_cleaning_to_ultimate_targets_on_idle();
  virtual bool is_busy_cleaning();

  // Update current compute markers on nodes.
  virtual void set_processing_node(Entity* entity);
  virtual void clear_processing_node();
  virtual void set_error_node(const QString& error_message); // Show error marker on the current compute node.
  virtual void clear_error_node();
  virtual void update_clean_marker(Entity* entity, bool clean);

  // Group logic.
  virtual void clean_enter_group(Entity* group);
  virtual void clean_exit_group(Entity* group);
  virtual void dive_into_group(const std::string& child_group_name);
  virtual void surface_from_group();

  // Update inputs and outputs configuration for the gui side.
  virtual void set_input_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering);
  virtual void set_output_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering);

  // Node creation.
  virtual Entity* create_node(bool centered, EntityDID entity_did, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_compute_node(bool centered, ComponentDID compute_did, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_user_macro_node(bool centered, const std::string& macro_name, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_app_macro_node(bool centered, const std::string& macro_name, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_input_node(bool centered, const QJsonValue& value, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_data_node(bool centered, const QJsonValue& value, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_browser_node(bool centered, ComponentDID compute_did, const QJsonObject& chain_state, const std::string& name = "", Entity* group_entity=NULL);

  // Link a node.
  virtual void link_to_closest_node(Entity* node);

  // Modify links.
  virtual void destroy_link(Entity* input_entity);
  virtual Entity* create_link(Entity* group);
  virtual Entity* connect_plugs(Entity* input_entity, Entity* output_entity);
  virtual void copy_description_to_input_node(Entity* input_entity, Entity* input_node_entity);

  // Graph Dirtiness.
  virtual void bubble_group_dirtiness();
  virtual void synchronize_graph_dirtiness(Entity* group_entity);
  virtual void dirty_compute(const Path& path);

  // Specialized Node Overrides.
  virtual void set_mqtt_override(const Path& node_path, const QString& topic, const QString& payload);
  virtual void set_firebase_override(const Path& node_path, const QString& data_path, const QJsonValue& value);
 protected:
  virtual void initialize_wires();

 private:
  NodeGraphManipulatorImp* _imp;

};




}
