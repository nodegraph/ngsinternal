#include "nodegrapheditor.h"

//#include <base/memoryallocator/taggednew.h>

//#include <base/device/program/pipeline.h>

#include <base/device/devicedebug.h>
//#include <base/device/pipelinesetups/quadpipelinesetup.h>
//#include <base/device/transforms/glmhelper.h>
#include <base/device/transforms/keyinfo.h>
#include <base/device/transforms/mouseinfo.h>
#include <base/device/transforms/viewparams.h>
#include <base/device/transforms/wheelinfo.h>
//#include <base/objectmodel/component.h>
//#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
//#include <iostream>

//#include <glm/gtc/matrix_transform.hpp>
//
//#include <base/utils/simplesaver.h>
//#include <base/utils/simpleloader.h>
//#include <components/resources/resources.h>

//// To create a test node graph.
//#include <components/compshapes/nodeshape.h>
//
//
//#include <base/utils/fileutil.h>
//#include <base/utils/idgenerator.h>
//#include <base/utils/path.h>
//#include <components/compshapes/compshapecollective.h>
//#include <components/compshapes/nodegraphselection.h>
//#include <components/computes/compute.h>
#include <components/interactions/graphbuilder.h>
#include <components/interactions/shapecanvas.h>
#include <components/interactions/groupinteraction.h>

#include <QtGui/QMouseEvent>


namespace {
using namespace ngs;

MouseInfo get_mouse_info(QMouseEvent *event) {
  MouseInfo info;
  info.screen_pos = glm::vec2(event->x(),event->y());
  info.left_button=(event->button() == Qt::LeftButton) || (event->buttons() == Qt::LeftButton);
  info.middle_button=(event->button() == Qt::MidButton) || (event->buttons() == Qt::MidButton);
  info.right_button=(event->button() == Qt::RightButton) || (event->buttons() == Qt::RightButton);
  info.control_modifier=event->modifiers() & Qt::ControlModifier;
  info.alt_modifier=event->modifiers() & Qt::AltModifier;
  info.shift_modifier=event->modifiers() & Qt::ShiftModifier;
  return info;
}

KeyInfo get_key_info_qt(QKeyEvent* event) {
  KeyInfo info;
  info.key_code = event->key();
  info.control_modifier=event->modifiers() & Qt::ControlModifier;
  info.alt_modifier=event->modifiers() & Qt::AltModifier;
  info.shift_modifier=event->modifiers() & Qt::ShiftModifier;
  return info;
}

void get_wheel_info(QWheelEvent* event, WheelInfo& info) {
  QPoint numPixels = event->pixelDelta();
  QPoint numDegrees = event->angleDelta() / 8.0f;

  if (!numPixels.isNull()) {
    info.x = numPixels.x();
    info.y = numPixels.y();
  } else if (!numDegrees.isNull()) {
    info.x = (float)numDegrees.x() / 15.0f;
    info.y = (float)numDegrees.y() / 15.0f;
  }
}

}

namespace ngs {

NodeGraphEditor::NodeGraphEditor(Entity* entity, QWidget* parent, const QGLWidget* shared_widget)
    : Component(entity, kIID(), kDID()),
      QGLWidget(parent, shared_widget),
      _resources(this),
      _canvas(this),
      _graph_builder(this),
      _initialized_gl(false) {
  get_dep_loader()->register_fixed_dep(_resources, "");
  get_dep_loader()->register_fixed_dep(_canvas, "");
  get_dep_loader()->register_fixed_dep(_graph_builder, "");

#if GLES_MAJOR_VERSION > 2
  // Multi sampling not supported on GLES 2.0.
  // Enable multi-sampling.
  QGLFormat glf = QGLFormat::defaultFormat();
  glf.setSampleBuffers(true);
  glf.setSamples(2);
  QGLFormat::setDefaultFormat(glf);
  QGLWidget::setFormat(glf);
#endif

  setAutoBufferSwap(false);
  setMouseTracking(true);
  setFocusPolicy(Qt::WheelFocus);

  resize(950, 950);

  std::cerr << "alpha size: " << format().alphaBufferSize() << " enabled: " << format().alpha() << "\n";
  std::cerr << "depth size: " << format().depthBufferSize() << " enabled: " << format().depth() << "\n";
}

NodeGraphEditor::~NodeGraphEditor() {
}

void NodeGraphEditor::build_test_graph() {
  _graph_builder->build_test_graph();
}

void NodeGraphEditor::on_timer() {
  makeCurrent();
  paintGL();
}

void NodeGraphEditor::initializeGL() {
  // This is main entry point for initialize gl resources in our system.
  std::cerr << "initialize gl called\n";

#if !((ARCH == ARCH_ANDROID) || (GLES_USE_ANGLE == 1))
  if (glewGetContext()==NULL) {
    std::cerr << "starting glew 2222\n";
    start_glew(); // Initialize glew for the scene graph render thread.
  }
#endif

  get_app_root()->initialize_deps();
  get_app_root()->update_deps_and_hierarchy();
  get_app_root()->initialize_gl();
}

void NodeGraphEditor::resizeGL(int w, int h) {
  _canvas->get_current_interaction()->resize_gl(w, h);
}

void NodeGraphEditor::paintGL() {
#if !((ARCH == ARCH_ANDROID) || (GLES_USE_ANGLE == 1))
  if (glewGetContext()==NULL) {
    start_glew(); // Initialize glew for the scene graph render thread.
  }
#endif

  // The render thread handles all our painting.
  draw_canvas_gl();
  //_interactive_display->Render();
  swapBuffers();
}

void NodeGraphEditor::draw_canvas_gl() {
  makeCurrent();
  _canvas->clean();
  _canvas->draw_gl();
}

void NodeGraphEditor::mousePressEvent(QMouseEvent *event) {
  MouseInfo info = get_mouse_info(event);
  _canvas->clean();
  _canvas->get_current_interaction()->pressed(info);
  update();
}

void NodeGraphEditor::mouseReleaseEvent(QMouseEvent* event) {
  MouseInfo info = get_mouse_info(event);
  _canvas->clean();
  _canvas->get_current_interaction()->released(info);
  update();
}

void NodeGraphEditor::mouseMoveEvent(QMouseEvent *event) {
  MouseInfo info = get_mouse_info(event);
  _canvas->clean();
  _canvas->get_current_interaction()->moved(info);
  update();
}

void NodeGraphEditor::mouseDoubleClickEvent(QMouseEvent *event) {
  MouseInfo info = get_mouse_info(event);
  _canvas->clean();
  update();
}

void NodeGraphEditor::wheelEvent(QWheelEvent *event) {
  WheelInfo info;
  get_wheel_info(event,info);
  _canvas->clean();
  _canvas->get_current_interaction()->wheel_rolled(info);
  event->accept();
  update();
}

void NodeGraphEditor::keyPressEvent(QKeyEvent* event) {
  KeyInfo info = get_key_info_qt(event);
  _canvas->clean();
  update();
}

void NodeGraphEditor::keyReleaseEvent(QKeyEvent * event) {
  KeyInfo info = get_key_info_qt(event);
  _canvas->clean();
  update();
}

void NodeGraphEditor::closeEvent(QCloseEvent *evt)
{
    QGLWidget::closeEvent(evt);
}

//void NodeGraphEditor::resizeEvent(QResizeEvent * event)
//{
//  if (!_initialized_gl) {
//    glInit();
//    _initialized_gl=true;
//  }
//
//  QSize size = event->size();
//  int width = size.width();
//  int height = size.height();
//  resizeGL(width,height);
//}

//void NodeGraphEditor::paintEvent(QPaintEvent *event)
//{
//  paintGL();
//}

}

