#pragma once
#include <guicomponents/quick/quick_export.h>
#include <base/objectmodel/dep.h>
#include <base/objectmodel/component.h>
#include <guicomponents/comms/guitypes.h>

#include <QtCore/QObject>

namespace ngs {

class BaseNodeGraphManipulator;
class NodeGraphQuickItem;

class QUICK_EXPORT NodeGraphController : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(NodeGraphController, NodeGraphController)
  NodeGraphController(Entity* parent);
  virtual ~NodeGraphController();

  Q_INVOKABLE void dive();
  Q_INVOKABLE void dive(const QString& group_node_name);
  void dive(const std::string& group_node_name);
  Q_INVOKABLE void surface_from_group();

//  // Objects which hold type info for use from the qml side.
//  JSTypeWrap js_type_wrap;
//  HintKeyWrap hint_key_wrap;
//  EnumHintValueWrap enum_hint_value_wrap;

  Q_INVOKABLE void create_public_macro_node(bool centered, const QString& macro_name);
  Q_INVOKABLE void create_private_macro_node(bool centered, const QString& macro_name);
  Q_INVOKABLE void create_app_macro_node(bool centered, const QString& macro_name);
  Q_INVOKABLE void create_main_macro_node(bool centered, const QString& macro_name);

  // Group Nodes Creation.
  Q_INVOKABLE void create_group_node(bool centered);
  Q_INVOKABLE void create_if_group_node(bool centered);
  Q_INVOKABLE void create_while_group_node(bool centered);
  Q_INVOKABLE void create_browser_group_node(bool centered);
  Q_INVOKABLE void create_mqtt_group_node(bool centered);

  // Create interface nodes.
  Q_INVOKABLE void create_password_input_node(bool centered);
  Q_INVOKABLE void create_string_input_node(bool centered);
  Q_INVOKABLE void create_number_input_node(bool centered);
  Q_INVOKABLE void create_boolean_input_node(bool centered);
  Q_INVOKABLE void create_array_input_node(bool centered);
  Q_INVOKABLE void create_object_input_node(bool centered);
  
  // Create output nodes.
  Q_INVOKABLE void create_output_node(bool centered);

  // Create data nodes.
  Q_INVOKABLE void create_password_data_node(bool centered);
  Q_INVOKABLE void create_string_data_node(bool centered);
  Q_INVOKABLE void create_number_data_node(bool centered);
  Q_INVOKABLE void create_boolean_data_node(bool centered);
  Q_INVOKABLE void create_array_data_node(bool centered);
  Q_INVOKABLE void create_object_data_node(bool centered);

  // Create data op nodes.
  Q_INVOKABLE void create_copy_data_node(bool centered);
  Q_INVOKABLE void create_erase_data_node(bool centered);

  // Script based nodes.
  Q_INVOKABLE void create_script_node(bool centered);
  Q_INVOKABLE void create_extract_property_node(bool centered);
  Q_INVOKABLE void create_embed_property_node(bool centered);
  Q_INVOKABLE void create_erase_property_node(bool centered);

  // Wait nodes.
  Q_INVOKABLE void create_wait_node(bool centered);

  // Dot nodes.
  Q_INVOKABLE void create_dot_node(bool centered);

  // Error nodes.
  Q_INVOKABLE void create_error_node(bool centered);

  // Data compute nodes.
  Q_INVOKABLE void create_merge_node(bool centered);

  // Http compute nodes.
  Q_INVOKABLE void create_http_node(bool centered);

  // MQTT compute nodes.
  Q_INVOKABLE void create_mqtt_publish_node(bool centered);
  Q_INVOKABLE void create_mqtt_subscribe_node(bool centered);

  Q_INVOKABLE void create_post_node(bool centered);


 private:
  Dep<BaseNodeGraphManipulator> _manipulator;
  Dep<NodeGraphQuickItem> _ng_quick;
};

}
