#include <guicomponents/quick/nodegraphcontroller.h>

#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <components/compshapes/nodeshape.h>

#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <QtCore/QJsonArray>

namespace ngs {

NodeGraphController::NodeGraphController(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _manipulator(this),
      _ng_quick(this) {
  get_dep_loader()->register_fixed_dep(_manipulator, Path());
  get_dep_loader()->register_fixed_dep(_ng_quick, Path());
}

NodeGraphController::~NodeGraphController() {

}

void NodeGraphController::dive() {
  const Dep<NodeShape>& node_shape = _ng_quick->get_last_pressed();
  dive(node_shape->get_name());
}

void NodeGraphController::dive(const QString& group_node_name) {
  dive(group_node_name.toStdString());
}

void NodeGraphController::dive(const std::string& group_node_name) {
  _manipulator->dive_into_group(group_node_name);
}

void NodeGraphController::surface_from_group() {
  _manipulator->surface_from_group();
}

void NodeGraphController::create_user_macro_node(bool centered, const QString& macro_name) {
  _manipulator->create_user_macro_node(centered, macro_name.toStdString());
}

void NodeGraphController::create_app_macro_node(bool centered, const QString& macro_name) {
  _manipulator->create_app_macro_node(centered, macro_name.toStdString());
}

// Group Nodes Creation.
void NodeGraphController::create_group_node(bool centered) {
  _manipulator->create_node(centered, EntityDID::kGroupNodeEntity);
}
void NodeGraphController::create_script_group_node(bool centered) {
  _manipulator->create_node(centered, EntityDID::kScriptGroupNodeEntity);
}
void NodeGraphController::create_browser_group_node(bool centered) {
  _manipulator->create_node(centered, EntityDID::kBrowserGroupNodeEntity);
}
void NodeGraphController::create_firebase_group_node(bool centered) {
  _manipulator->create_node(centered, EntityDID::kFirebaseGroupNodeEntity);
}
void NodeGraphController::create_mqtt_group_node(bool centered) {
  _manipulator->create_node(centered, EntityDID::kMQTTGroupNodeEntity);
}

// Create interface nodes.
void NodeGraphController::create_input_node(bool centered) {
  _manipulator->create_input_node(centered, QJsonValue(123));
}
void NodeGraphController::create_output_node(bool centered) {
  _manipulator->create_node(centered, EntityDID::kOutputNodeEntity);
}

// Create data nodes.
void NodeGraphController::create_data_node(bool centered) {
  _manipulator->create_data_node(centered, QJsonArray());
}
void NodeGraphController::create_dot_node(bool centered) {
  _manipulator->create_node(centered, EntityDID::kDotNodeEntity);
}

// Data compute nodes.
void NodeGraphController::create_merge_node(bool centered) {
  _manipulator->create_compute_node(centered, ComponentDID::kMergeNodeCompute);
}

// Firebase compute nodes.
void NodeGraphController::create_firebase_write_data_node(bool centered) {
  _manipulator->create_compute_node(centered, ComponentDID::kFirebaseWriteDataCompute);
}
void NodeGraphController::create_firebase_read_data_node(bool centered) {
  _manipulator->create_compute_node(centered, ComponentDID::kFirebaseReadDataCompute);
}

// Http compute nodes.
void NodeGraphController::create_http_node(bool centered) {
  _manipulator->create_compute_node(centered, ComponentDID::kHTTPCompute);
}

// MQTT compute nodes.
void NodeGraphController::create_mqtt_publish_node(bool centered) {
  _manipulator->create_compute_node(centered, ComponentDID::kMQTTPublishCompute);
}
void NodeGraphController::create_mqtt_subscribe_node(bool centered) {
  _manipulator->create_compute_node(centered, ComponentDID::kMQTTSubscribeCompute);
}


}