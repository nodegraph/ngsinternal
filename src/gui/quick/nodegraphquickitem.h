#pragma once
#include <gui/gui_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>
#include <components/entities/componentids.h>
#include <base/device/transforms/mouseinfo.h>

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
class GraphBuilder;

class GUI_EXPORT NodeGraphQuickItem : public QQuickItem, public Component
{
Q_OBJECT
 public:

  COMPONENT_ID(NodeGraphQuickItem, NodeGraphQuickItem)

  static const int kLongPressTimeThreshold; // in milli seconds.
  static const float kLongPressDistanceThreshold; // in object space.
  static const int kDoublePressTimeThreshold; // max time between release events for a double tap/click.

  static const QString kLinkableFile;
  static const QString kLinkableDir;

  static const int kMinimumIdleLength; // in milli seconds

  NodeGraphQuickItem(QQuickItem *parent = 0);
  virtual ~NodeGraphQuickItem();

  virtual void initialize_fixed_deps();

  // Testing.
  Q_INVOKABLE void build_test_graph();

  // Node Creation.
  Q_INVOKABLE void create_group_node(bool centered);
  Q_INVOKABLE void create_input_node(bool centered);
  Q_INVOKABLE void create_output_node(bool centered);
  Q_INVOKABLE void create_dot_node(bool centered);
  Q_INVOKABLE void create_mock_node(bool centered);

  // Group Navigation.
  Q_INVOKABLE void dive();
  Q_INVOKABLE void surface();

  // Selection.
  Q_INVOKABLE void toggle_selection_under_long_press();
  Q_INVOKABLE void select_all();
  Q_INVOKABLE void deselect_all();
  Q_INVOKABLE void destroy_selection();

  // Framing.
  Q_INVOKABLE void frame_all();
  Q_INVOKABLE void frame_selected();

  // Serialization.
  Q_INVOKABLE QString get_linkable_file();
  Q_INVOKABLE void make_save_point();
  Q_INVOKABLE void save();
  Q_INVOKABLE void load();

  Q_INVOKABLE void shutdown();

  // Copy and Paste.
  Q_INVOKABLE void copy();
  Q_INVOKABLE void cut();
  Q_INVOKABLE void paste(bool centered);

  // Explode and Collapse.
  Q_INVOKABLE void restore_default_node_graph();
  Q_INVOKABLE void collapse_to_group();
  Q_INVOKABLE void explode_group();

 public Q_SLOTS:
  void cleanup();

 private Q_SLOTS:
  void handle_window_changed(QQuickWindow *win);

 public Q_SLOTS:

  //void on_switch_to_mode(int mode);

  void on_long_press();
  void on_save();
  void on_load();


 signals:

 // Our signals.
 void node_graph_context_menu_requested(bool center);
 void group_node_context_menu_requested();
 void node_context_menu_requested();

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

  void finish_creating_node(Entity* e, bool centered);


  const Dep<GroupInteraction>& get_current_interaction();

  // Our fixed deps.
  Dep<FBOWorker> _fbo_worker;
  Dep<NodeGraphSelection> _selection;
  Dep<ShapeCanvas> _canvas;
  Dep<BaseFactory> _factory;
  Dep<GraphBuilder> _graph_builder;
  BaseEntityInstancer* _entity_instancer;

  // Used to detect long press taps/clicks.
  QTimer _long_press_timer;

  // Info about our last press, used when handling long presses.
  MouseInfo _last_press;
  Dep<CompShape> _last_pressed_shape;

  std::string _last_save_raw;

  QTimer _idle_timer;
  QTime _idle_length;

  // Pinching mode.
  bool _pinch_mode;
};

}

