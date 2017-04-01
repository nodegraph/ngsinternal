#pragma once
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>
#include <guicomponents/quick/quick_export.h>
#include <base/objectmodel/componentids.h>
#include <base/objectmodel/dep.h>

#include <guicomponents/quick/basenodegraphmanipulator.h>
#include <guicomponents/comms/guitypes.h>
#include <string>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QJsonObject>

namespace ngs {


class BaseFactory;
class NodeSelection;
class NodeGraphQuickItem;
class TaskQueuer;
class TaskScheduler;
class Compute;
class MessageReceiver;
class FileModel;
class NodeGraphView;


// This is the implementation class for the NodeGraphManipulator.
// It is held by a raw pointer in NodeGraphManipulator to avoid dependency cycles.
// This allows us to call this from the non-gui side (eg computes) at arbitrary
// locations to update the gui side to reflect non-gui side changes.
class QUICK_EXPORT NodeGraphManipulatorImp: public QObject, public Component {
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

  bool start_waiting(const std::function<void()>& on_idle);

  virtual void initialize_wires();

  virtual void receive_message(const QString& msg);

  // Set Ultimate Targets.
  virtual void set_ultimate_targets(Entity* entity, bool force_stack_reset = false);
  virtual void set_ultimate_targets(const std::unordered_set<Entity*>& entities, bool force_stack_reset = false);
  virtual void set_inputs_as_ultimate_targets(Entity* node_entity);

  // Ultimate Target Cleaning.
  virtual void clear_ultimate_targets();
  virtual void continue_cleaning_to_ultimate_targets();
  virtual void continue_cleaning_to_ultimate_targets_on_idle();

  Q_INVOKABLE virtual bool is_busy_cleaning();
  Q_INVOKABLE virtual bool current_task_is_cancelable();
  Q_INVOKABLE virtual bool is_waiting_for_response();
  Q_INVOKABLE virtual void force_stack_reset();

  // Update current compute markers on nodes.
  virtual void set_processing_node(Entity* entity);
  virtual void clear_processing_node();
  virtual void set_error_node(const QString& error_message);
  virtual void clear_error_node();
  virtual void update_clean_marker(Entity* entity, bool clean);

  // Group logic.
  virtual void enter_group_prep(Entity* group);
  virtual void exit_group_prep(Entity* group);
  virtual void dive_into_group(const std::string& child_group_name);
  virtual void surface_from_group();

  // Update inputs and outputs configuration for the gui side.
  virtual void set_input_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering);
  virtual void set_output_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering);

  // Node creation.
  virtual Entity* create_node(bool centered, EntityDID entity_did, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_compute_node(bool centered, EntityDID entity_did, ComponentDID compute_did, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_public_macro_node(bool centered, const std::string& macro_name, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_private_macro_node(bool centered, const std::string& macro_name, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_app_macro_node(bool centered, const std::string& macro_name, const std::string& name = "", Entity* group_entity=NULL);

  virtual Entity* create_password_input_node(bool centered, const QJsonValue& value, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_input_node(bool centered, const QJsonValue& value, const std::string& name = "", Entity* group_entity=NULL);

  virtual Entity* create_password_data_node(bool centered, const QJsonValue& value, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_data_node(bool centered, const QJsonValue& value, const std::string& name = "", Entity* group_entity=NULL);

  virtual Entity* create_browser_node(bool centered, ComponentDID compute_did, const QJsonObject& chain_state = QJsonObject(), const std::string& name = "", Entity* group_entity=NULL);

  // Link a node.
  virtual void link_to_selected_node(Entity* node);

  // Modify links.
  virtual void destroy_link(Entity* input_entity);
  virtual Entity* create_link(Entity* group);
  virtual Entity* connect_plugs(Entity* input_entity, Entity* output_entity);
  virtual void copy_description_to_input_node(Entity* input_entity, Entity* input_node_entity);

  // Graph Dirtiness.
  virtual void bubble_group_dirtiness(Entity* dirty_node_entity);

  // Specialized Node Overrides.
  virtual void set_mqtt_override(const Path& node_path, const QString& topic, const QString& payload);


  virtual void send_post_value_signal(int post_type, const QString& title, const QJsonObject& obj);

  signals:
  void post_value(int post_type, QString title, QJsonObject obj);

 protected:

private slots:
  void on_auto_run_changed(bool r);
  void on_auto_run_interval_changed(int i);

  void on_idle_timer_timeout();
  void on_auto_run_timer_timeout();

 private:
  void update_auto_run_timer();

  virtual void finish_creating_node(Entity* entity, bool centered);

  void prune_clean_or_dead();

  Entity* _app_root;
  Dep<BaseFactory> _factory;
  Dep<NodeSelection> _selection;
  Dep<NodeGraphQuickItem> _ng_quick;
  Dep<TaskQueuer> _queuer;
  Dep<TaskScheduler> _scheduler;
  Dep<MessageReceiver> _msg_receiver;
  Dep<FileModel> _file_model;
  Dep<NodeGraphView> _ng_view;

  // The ultimate compute (of a node) that we are trying to clean.
  // Note that there maybe many asynchronous computes which cause each cleaning pass over the dependencies
  // to finish early (returning false). Holding this reference to the ultimate component we want to clean
  // allows us to restart the cleaning process once other asynchronous cleaning processes finish.
  std::vector<Dep<Compute> > _ultimate_targets;

  // Wait for ultimate targets to become clean.
  QTimer _idle_timer;
  std::function<void()> _on_idle;

  // Auto run timer.
  QTimer _auto_run_timer;

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

  virtual void receive_message(const QString& msg);

  // Set Ultimate Targets.
  virtual void set_ultimate_targets(Entity* entity, bool force_stack_reset);
  virtual void set_ultimate_targets(const std::unordered_set<Entity*>& entities, bool force_stack_reset);
  virtual void set_inputs_as_ultimate_targets(Entity* node_entity);

  // Ultimate Target Cleaning.
  virtual void clear_ultimate_targets();
  virtual void continue_cleaning_to_ultimate_targets();
  virtual void continue_cleaning_to_ultimate_targets_on_idle();
  virtual bool is_busy_cleaning();
  virtual bool current_task_is_cancelable();
  virtual bool is_waiting_for_response();
  virtual void force_stack_reset();

  // Update current compute markers on nodes.
  virtual void set_processing_node(Entity* entity);
  virtual void clear_processing_node();
  virtual void set_error_node(const QString& error_message); // Show error marker on the current compute node.
  virtual void clear_error_node();
  virtual void update_clean_marker(Entity* entity, bool clean);

  // Group logic.
  virtual void enter_group_prep(Entity* group);
  virtual void exit_group_prep(Entity* group);
  virtual void dive_into_group(const std::string& child_group_name);
  virtual void surface_from_group();

  // Update inputs and outputs configuration for the gui side.
  virtual void set_input_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering);
  virtual void set_output_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering);

  // Node creation.
  virtual Entity* create_node(bool centered, EntityDID entity_did, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_compute_node(bool centered, EntityDID entity_did, ComponentDID compute_did, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_public_macro_node(bool centered, const std::string& macro_name, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_private_macro_node(bool centered, const std::string& macro_name, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_app_macro_node(bool centered, const std::string& macro_name, const std::string& name = "", Entity* group_entity=NULL);

  virtual Entity* create_password_input_node(bool centered, const QJsonValue& value, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_input_node(bool centered, const QJsonValue& value, const std::string& name = "", Entity* group_entity=NULL);

  virtual Entity* create_password_data_node(bool centered, const QJsonValue& value, const std::string& name = "", Entity* group_entity=NULL);
  virtual Entity* create_data_node(bool centered, const QJsonValue& value, const std::string& name = "", Entity* group_entity=NULL);

  virtual Entity* create_browser_node(bool centered, ComponentDID compute_did, const QJsonObject& chain_state, const std::string& name = "", Entity* group_entity=NULL);

  // Link a node.
  virtual void link_to_selected_node(Entity* node);

  // Modify links.
  virtual void destroy_link(Entity* input_entity);
  virtual Entity* create_link(Entity* group);
  virtual Entity* connect_plugs(Entity* input_entity, Entity* output_entity);
  virtual void copy_description_to_input_node(Entity* input_entity, Entity* input_node_entity);

  // Graph Dirtiness.
  virtual void bubble_group_dirtiness(Entity* dirty_node_entity);

  // Specialized Node Overrides.
  virtual void set_mqtt_override(const Path& node_path, const QString& topic, const QString& payload);

  virtual void send_post_value_signal(int post_type, const QString& title, const QJsonObject& obj);
  NodeGraphManipulatorImp* get_imp() {return _imp;}

 protected:
  virtual void initialize_wires();

 private:
  NodeGraphManipulatorImp* _imp;

};




}
