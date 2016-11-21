#pragma once
#include <guicomponents/quick/quick_export.h>

#include <base/device/devicebasictypesgl.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>
#include <entities/componentids.h>
#include <base/device/transforms/mouseinfo.h>
#include <guicomponents/comms/filemodel.h>
#include <guicomponents/comms/qmltypes.h>

#include <QtQuick/QQuickItem>
//#include <QtCore/QThread>
#include <QtCore/QTime>
#include <QtCore/QTimer>

namespace ngs {

class FBOWorker;
class GroupInteraction;
class ShapeCanvas;
class NodeSelection;
class NodeShape;
class BaseFactory;
class BaseEntityInstancer;
class LicenseChecker;
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

  // Objects which hold type info for use from the qml side.
  JSTypeWrap js_type_wrap;
  HintKeyWrap hint_key_wrap;
  EnumHintValueWrap enum_hint_value_wrap;

  Q_INVOKABLE void create_user_macro_node(bool centered, const QString& macro_name);
  Q_INVOKABLE void create_app_macro_node(bool centered, const QString& macro_name);

  // Group Nodes Creation.
  Q_INVOKABLE void create_group_node(bool centered);
  Q_INVOKABLE void create_script_group_node(bool centered);
  Q_INVOKABLE void create_browser_group_node(bool centered);
  Q_INVOKABLE void create_firebase_group_node(bool centered);
  Q_INVOKABLE void create_mqtt_group_node(bool centered);

  // Create interface nodes.
  Q_INVOKABLE void create_input_node(bool centered);
  Q_INVOKABLE void create_output_node(bool centered);

  // Create data nodes.
  Q_INVOKABLE void create_data_node(bool centered);
  Q_INVOKABLE void create_dot_node(bool centered);

  // Data compute nodes.
  Q_INVOKABLE void create_merge_node(bool centered);

  // Firebase compute nodes.
  Q_INVOKABLE void create_firebase_write_data_node(bool centered);
  Q_INVOKABLE void create_firebase_read_data_node(bool centered);

  // Http compute nodes.
  Q_INVOKABLE void create_http_node(bool centered);

  // MQTT compute nodes.
  Q_INVOKABLE void create_mqtt_publish_node(bool centered);
  Q_INVOKABLE void create_mqtt_subscribe_node(bool centered);


 private:
  Dep<BaseNodeGraphManipulator> _manipulator;
  Dep<NodeGraphQuickItem> _ng_quick;
};


class QUICK_EXPORT NodeGraphQuickItem : public QQuickItem, public Component
{
Q_OBJECT
 public:

  COMPONENT_ID(NodeGraphQuickItem, NodeGraphQuickItem)

  static const int kLongPressTimeThreshold; // in milli seconds.
  static const float kLongPressDistanceThreshold; // in object space.
  static const int kDoublePressTimeThreshold; // max time between release events for a double tap/click.


  static const int kMinimumIdleLength; // in milli seconds

  NodeGraphQuickItem(Entity* parent);
  virtual ~NodeGraphQuickItem();

  Q_INVOKABLE QString get_ngs_version() const;
  Q_INVOKABLE size_t get_num_nodes() const;

  const Dep<NodeShape>& get_last_pressed() const {return _last_node_shape;}
  const MouseInfo& get_last_press_info() const { return _last_press;}
  const Dep<GroupInteraction>& get_current_interaction() const;

  // View and Edit.
  Q_INVOKABLE void view_node();
  Q_INVOKABLE void edit_node();
  Q_INVOKABLE void dirty_node();
  Q_INVOKABLE void clean_node();

  Q_INVOKABLE void set_editable_inputs(const QString& node_path, const QJsonObject& values);
  Q_INVOKABLE void set_input_exposure(const QString& node_path, const QJsonObject& values);

  // Group Navigation.
  void dive_into_group(const std::string& child_group_name);
  Q_INVOKABLE void surface();
  Q_INVOKABLE void surface_to_root();

  // Selection.
  Q_INVOKABLE void toggle_selection_under_long_press();
  Q_INVOKABLE void select_last_press();
  Q_INVOKABLE void deselect_last_press();
  Q_INVOKABLE void select_all();
  Q_INVOKABLE void deselect_all();
  Q_INVOKABLE void destroy_selection();

  // Framing.
  Q_INVOKABLE void frame_all();
  Q_INVOKABLE void frame_selected();

  // Serialization.
  Q_INVOKABLE void save();
  Q_INVOKABLE void load();

  // Macros.
  Q_INVOKABLE bool macro_exists(const QString& macro_name) const;
  Q_INVOKABLE void publish(const QString& macro_name);
  Q_INVOKABLE QStringList get_user_macro_names() const;
  Q_INVOKABLE QStringList get_app_macro_names() const;


  // Copy and Paste.
  Q_INVOKABLE void copy();
  Q_INVOKABLE void cut();
  Q_INVOKABLE void paste(bool centered);

  // Explode and Collapse.
  Q_INVOKABLE void collapse_to_group();
  Q_INVOKABLE void explode_group();

  // Lock Graph.
  Q_INVOKABLE void lock_links(bool locked);
  Q_INVOKABLE bool links_are_locked() const;


  Q_INVOKABLE void view_node_poke();
  Q_INVOKABLE void edit_node_poke();

 public Q_SLOTS:
  void cleanup();

 private Q_SLOTS:
  void handle_window_changed(QQuickWindow *win);

 public Q_SLOTS:
  void on_long_press();

 signals:

 // Our signals.
 void node_graph_context_menu_requested(bool center);
 void group_node_context_menu_requested();
 void node_context_menu_requested();

 void view_node_outputs(const QString& node_path, const QJsonObject& results);
 void edit_node_inputs(const QString& node_path, const QJsonObject& values, const QJsonObject& hints, const QJsonObject& exposed_settings);

 void mark_progress();

 void set_error_message(const QString& message);
 void show_error_page();

 protected:
 // Our state.
 virtual bool update_state();

  virtual QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*);
  virtual void releaseResources();

  // Geometry change overrides.
  virtual void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);

  // Focus overrides.
  virtual void focusInEvent(QFocusEvent * event);
  virtual void focusOutEvent(QFocusEvent * event);

  // Mouse overrides.
  virtual void mouseDoubleClickEvent(QMouseEvent * event);
  virtual void mouseMoveEvent(QMouseEvent * event);
  virtual void mousePressEvent(QMouseEvent * event);
  virtual void mouseReleaseEvent(QMouseEvent * event);// saves node graph
  virtual void wheelEvent(QWheelEvent *event);

  // Hover overrides.
  virtual void hoverMoveEvent(QHoverEvent * event);

  // Key overrides.
  virtual void keyPressEvent(QKeyEvent * event);
  virtual void keyReleaseEvent(QKeyEvent * event);

  // Touch overrides
  virtual void touchEvent(QTouchEvent * event);// saves node graph

 private:

  void popup_context_menu();
  void finish_creating_node(Entity* e, bool centered);

  // Our fixed deps.
  Dep<FBOWorker> _fbo_worker;
  Dep<NodeSelection> _selection;
  Dep<ShapeCanvas> _canvas;
  Dep<BaseFactory> _factory;
  Dep<FileModel> _file_model;
  Dep<LicenseChecker> _license_checker;
  Dep<BaseNodeGraphManipulator> _manipulator;

  // Used to detect long press.
  QTimer _long_press_timer;

  // Info about our last press, used when handling long presses.
  MouseInfo _last_press;
  Dep<NodeShape> _last_node_shape;

  // Pinch Zoom.
  glm::vec2 _original_pinch_center;
  glm::vec2 _original_pinch_a;
  glm::vec2 _original_pinch_b;

  // In OpenGL pixels are always in device pixels.
  GLsizei _device_pixel_ratio;

  // Whether the node graph is locked.
  bool _link_locked;


};

}

