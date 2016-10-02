#pragma once
#include <gui/gui_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>

#include <QtOpenGL/QGLWidget>

namespace ngs {
  class Entity;
  class ShapeCanvas;
  class Resources;
  class GraphBuilder;

class GUI_EXPORT NodeGraphEditor : public QGLWidget, public Component {
Q_OBJECT
 public:

  COMPONENT_ID(NodeGraphEditor, NodeGraphEditor)

  NodeGraphEditor(Entity* entity, QWidget* parent = NULL, const QGLWidget* shared_widget = NULL);
  virtual ~NodeGraphEditor();

  void draw_canvas_gl(); // public for now, so that RenderThread can call it.

  virtual void build_test_graph();

 public slots:
  void on_timer();

 protected:
  // Qt overrides.
  virtual void initializeGL();
  virtual void resizeGL(int w, int h);
  virtual void paintGL();
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseDoubleClickEvent(QMouseEvent *event);
  virtual void wheelEvent(QWheelEvent *event);
  virtual void keyPressEvent(QKeyEvent *event);
  virtual void keyReleaseEvent(QKeyEvent * event);
  virtual void closeEvent(QCloseEvent * event);
  //virtual void resizeEvent(QResizeEvent *event);
  //virtual void paintEvent(QPaintEvent *event);

 private:
  // Our fixed deps.
  Dep<Resources> _resources;
  Dep<ShapeCanvas> _canvas;
  Dep<GraphBuilder> _graph_builder;

  bool _initialized_gl;
};

}
