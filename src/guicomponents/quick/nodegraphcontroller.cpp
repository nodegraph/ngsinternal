#include <guicomponents/quick/nodegraphcontroller.h>

#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <components/compshapes/nodeshape.h>

#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

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
void NodeGraphController::create_if_group_node(bool centered) {
  _manipulator->create_node(centered, EntityDID::kIfGroupNodeEntity);
}
void NodeGraphController::create_while_group_node(bool centered) {
  _manipulator->create_node(centered, EntityDID::kWhileGroupNodeEntity);
}
void NodeGraphController::create_for_each_group_node(bool centered) {
  _manipulator->create_node(centered, EntityDID::kForEachGroupNodeEntity);
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
void NodeGraphController::create_number_input_node(bool centered) {
  _manipulator->create_input_node(centered, QJsonValue(0));
}
void NodeGraphController::create_string_input_node(bool centered) {
  _manipulator->create_input_node(centered, QJsonValue(""));
}
void NodeGraphController::create_boolean_input_node(bool centered) {
  _manipulator->create_input_node(centered, QJsonValue(false));
}
void NodeGraphController::create_array_input_node(bool centered) {
  _manipulator->create_input_node(centered, QJsonArray());
}
void NodeGraphController::create_object_input_node(bool centered) {
  _manipulator->create_input_node(centered, QJsonObject());
}


void NodeGraphController::create_output_node(bool centered) {
  _manipulator->create_node(centered, EntityDID::kOutputNodeEntity);
}

void NodeGraphController::create_accumulate_data_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kLoopComputeNodeEntity, ComponentDID::kAccumulateDataNodeCompute);
}

// Create data nodes.
void NodeGraphController::create_number_data_node(bool centered) {
  _manipulator->create_data_node(centered, 0);
}
void NodeGraphController::create_string_data_node(bool centered) {
  _manipulator->create_data_node(centered, "");
}
void NodeGraphController::create_boolean_data_node(bool centered) {
  _manipulator->create_data_node(centered, false);
}
void NodeGraphController::create_array_data_node(bool centered) {
  _manipulator->create_data_node(centered, QJsonArray());
}
void NodeGraphController::create_object_data_node(bool centered) {
  _manipulator->create_data_node(centered, QJsonObject());
}

// Create data op nodes.
void NodeGraphController::create_copy_data_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kComputeNodeEntity, ComponentDID::kCopyDataNodeCompute);
}
void NodeGraphController::create_erase_data_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kComputeNodeEntity, ComponentDID::kEraseDataNodeCompute);
}

// Create dot nodes.
void NodeGraphController::create_dot_node(bool centered) {
  _manipulator->create_node(centered, EntityDID::kDotNodeEntity);
}

// Data compute nodes.
void NodeGraphController::create_merge_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kComputeNodeEntity, ComponentDID::kMergeNodeCompute);
}

// Firebase compute nodes.
void NodeGraphController::create_firebase_write_data_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kFirebaseComputeNodeEntity, ComponentDID::kFirebaseWriteDataCompute);
}
void NodeGraphController::create_firebase_read_data_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kFirebaseComputeNodeEntity, ComponentDID::kFirebaseReadDataCompute);
}

// Http compute nodes.
void NodeGraphController::create_http_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kComputeNodeEntity, ComponentDID::kHTTPCompute);
}

// MQTT compute nodes.
void NodeGraphController::create_mqtt_publish_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kMQTTComputeNodeEntity, ComponentDID::kMQTTPublishCompute);
}
void NodeGraphController::create_mqtt_subscribe_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kMQTTComputeNodeEntity, ComponentDID::kMQTTSubscribeCompute);
}


}
