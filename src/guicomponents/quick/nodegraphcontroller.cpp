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

void NodeGraphController::create_public_macro_node(bool centered, const QString& macro_name) {
  _manipulator->create_public_macro_node(centered, macro_name.toStdString());
}

void NodeGraphController::create_private_macro_node(bool centered, const QString& macro_name) {
  _manipulator->create_private_macro_node(centered, macro_name.toStdString());
}

void NodeGraphController::create_app_macro_node(bool centered, const QString& macro_name) {
  _manipulator->create_app_macro_node(centered, macro_name.toStdString());
}

void NodeGraphController::create_main_macro_node(bool centered, const QString& macro_name) {
  Entity* macro = _manipulator->create_app_macro_node(centered, macro_name.toStdString(), "main_macro");
  Entity* macro_in = macro->get_entity(Path({".","inputs","in"}));
  Entity* macro_out = macro->get_entity(Path({".","outputs","out"}));
  Entity *upstream = get_entity(Path({"root","in","outputs","out"}));
  Entity *downstream = get_entity(Path({"root","out","inputs","in"}));

  _manipulator->create_link(macro_in, upstream);
  _manipulator->create_link(downstream, macro_out);
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
void NodeGraphController::create_browser_group_node(bool centered) {
  _manipulator->create_node(centered, EntityDID::kBrowserGroupNodeEntity);
}
void NodeGraphController::create_mqtt_group_node(bool centered) {
  _manipulator->create_node(centered, EntityDID::kMQTTGroupNodeEntity);
}

// Create interface nodes.
void NodeGraphController::create_password_input_node(bool centered) {
  _manipulator->create_password_input_node(centered, QJsonValue(""));
}
void NodeGraphController::create_string_input_node(bool centered) {
  _manipulator->create_input_node(centered, QJsonValue(""));
}
void NodeGraphController::create_number_input_node(bool centered) {
  _manipulator->create_input_node(centered, QJsonValue(0));
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

// Create data nodes.
void NodeGraphController::create_password_data_node(bool centered) {
  _manipulator->create_password_data_node(centered, "");
}
void NodeGraphController::create_string_data_node(bool centered) {
  _manipulator->create_data_node(centered, "");
}
void NodeGraphController::create_number_data_node(bool centered) {
  _manipulator->create_data_node(centered, 0);
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

// Create script based nodes.
void NodeGraphController::create_script_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kComputeNodeEntity, ComponentDID::kScriptNodeCompute);
}

void NodeGraphController::create_extract_property_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kComputeNodeEntity, ComponentDID::kExtractPropertyNodeCompute);
}

void NodeGraphController::create_embed_property_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kComputeNodeEntity, ComponentDID::kEmbedPropertyNodeCompute);
}

void NodeGraphController::create_erase_property_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kComputeNodeEntity, ComponentDID::kErasePropertyNodeCompute);
}

// Create wait nodes.
void NodeGraphController::create_wait_node(bool centered) {
  _manipulator->create_node(centered, EntityDID::kWaitNodeEntity);
}

// Create dot nodes.
void NodeGraphController::create_dot_node(bool centered) {
  _manipulator->create_node(centered, EntityDID::kDotNodeEntity);
}

void NodeGraphController::create_error_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kComputeNodeEntity, ComponentDID::kErrorNodeCompute);
}

// Data compute nodes.
void NodeGraphController::create_merge_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kComputeNodeEntity, ComponentDID::kMergeNodeCompute);
}

// Http compute nodes.
void NodeGraphController::create_http_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kComputeNodeEntity, ComponentDID::kHTTPCompute);
}

// MQTT compute nodes.
void NodeGraphController::create_mqtt_publish_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kComputeNodeEntity, ComponentDID::kMQTTPublishCompute);
}
void NodeGraphController::create_mqtt_subscribe_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kComputeNodeEntity, ComponentDID::kMQTTSubscribeCompute);
}

void NodeGraphController::create_post_node(bool centered) {
  _manipulator->create_compute_node(centered, EntityDID::kComputeNodeEntity, ComponentDID::kPostNodeCompute);
}


}
