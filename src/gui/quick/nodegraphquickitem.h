#pragma once
#include <gui/gui_export.h>

#include <base/device/devicebasictypesgl.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>
#include <components/entities/componentids.h>
#include <base/device/transforms/mouseinfo.h>
#include <gui/quick/filemodel.h>

// Qt includes.
#include <QtQuick/QQuickItem>
//#include <QtCore/QThread>
#include <QtCore/QTime>
#include <QtCore/QTimer>

namespace ngs {

class FBOWorker;
class GroupInteraction;
class ShapeCanvas;
class NodeGraphSelection;
class CompShape;
class BaseFactory;
class BaseEntityInstancer;


class GUI_EXPORT NodeGraphQuickItem : public QQuickItem, public Component
{
Q_OBJECT
 public:

  COMPONENT_ID(NodeGraphQuickItem, NodeGraphQuickItem)

  static const int kLongPressTimeThreshold; // in milli seconds.
  static const float kLongPressDistanceThreshold; // in object space.
  static const int kDoublePressTimeThreshold; // max time between release events for a double tap/click.


  static const int kMinimumIdleLength; // in milli seconds

  NodeGraphQuickItem(QQuickItem *parent = 0);
  virtual ~NodeGraphQuickItem();

  virtual void initialize_fixed_deps();

  // Node Creation.
  Q_INVOKABLE void create_group_node(bool centered);
  Q_INVOKABLE void create_input_node(bool centered);
  Q_INVOKABLE void create_output_node(bool centered);
  Q_INVOKABLE void create_dot_node(bool centered);
  Q_INVOKABLE void create_mock_node(bool centered);

  // Browser Node Creation.
  Q_INVOKABLE void create_open_browser_node(bool centered);
  Q_INVOKABLE void create_close_browser_node(bool centered);
  Q_INVOKABLE void create_create_set_from_values_node(bool centered);
  Q_INVOKABLE void create_create_set_from_type_node(bool centered);

  // View and Edit.
  Q_INVOKABLE void view_node();
  Q_INVOKABLE void edit_node();

  // Group Navigation.
  Q_INVOKABLE void dive();
  Q_INVOKABLE void surface();
  Q_INVOKABLE void surface_to_root();

  // Selection.
  Q_INVOKABLE void toggle_selection_under_long_press();
  Q_INVOKABLE void select_all();
  Q_INVOKABLE void deselect_all();
  Q_INVOKABLE void destroy_selection();

  // Framing.
  Q_INVOKABLE void frame_all();
  Q_INVOKABLE void frame_selected();

  // Serialization.
  Q_INVOKABLE void save();
  Q_INVOKABLE void load();

  // Copy and Paste.
  Q_INVOKABLE void copy();
  Q_INVOKABLE void cut();
  Q_INVOKABLE void paste(bool centered);

  // Explode and Collapse.
  Q_INVOKABLE void collapse_to_group();
  Q_INVOKABLE void explode_group();

  // Lock Graph.
  Q_INVOKABLE void lock_links(bool locked);
  Q_INVOKABLE bool links_are_locked();

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

 void view_node_outputs(const QString& name, const QVariantMap& results);
 void edit_node_inputs(const QString& name, const QVariantList& inputs);

 void mark_progress();

 protected:
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


  const Dep<GroupInteraction>& get_current_interaction();

  // Our fixed deps.
  Dep<FBOWorker> _fbo_worker;
  Dep<NodeGraphSelection> _selection;
  Dep<ShapeCanvas> _canvas;
  Dep<BaseFactory> _factory;
  Dep<FileModel> _file_model;

  // Cached helper.
  BaseEntityInstancer* _entity_instancer;

  // Used to detect long press.
  QTimer _long_press_timer;

  // Info about our last press, used when handling long presses.
  MouseInfo _last_press;
  Dep<CompShape> _last_pressed_shape;

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

