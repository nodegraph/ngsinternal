#include "nodegrapheditor.h"

#include <base/device/devicedebug.h>
#include <base/device/transforms/keyinfo.h>
#include <base/device/transforms/mouseinfo.h>
#include <base/device/transforms/viewparams.h>
#include <base/device/transforms/wheelinfo.h>
#include <base/glewhelper/glewhelper.h>

#include <base/objectmodel/deploader.h>

#include <components/interactions/graphbuilder.h>
#include <components/interactions/shapecanvas.h>
#include <components/interactions/groupinteraction.h>

#include <QtGui/QMouseEvent>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

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
  get_dep_loader()->register_fixed_dep(_resources, Path());
  get_dep_loader()->register_fixed_dep(_canvas, Path());
  get_dep_loader()->register_fixed_dep(_graph_builder, Path());


#if ARCH == ARCH_MACOS
  QGLFormat glFormat;
  glFormat.setVersion(3, 3);
  glFormat.setProfile(QGLFormat::CoreProfile);
  QGLFormat::setDefaultFormat(glFormat);
  setFormat(glFormat);
#endif

#if GLES_MAJOR_VERSION > 2
  // Multi sampling is not supported on GLES 2.0.
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

  if (glewGetContext()==NULL) {
    std::cerr << "starting glew 2222\n";
    start_glew(); // Initialize glew for the scene graph render thread.
  }

  get_app_root()->clean_wires();
  get_app_root()->initialize_gl();
}

void NodeGraphEditor::resizeGL(int w, int h) {
  GLsizei dpr = static_cast<GLsizei>(QGuiApplication::primaryScreen()->devicePixelRatio());
  _canvas->get_current_interaction()->resize_gl(dpr*w, dpr*h);
}

void NodeGraphEditor::paintGL() {
  if (glewGetContext()==NULL) {
    start_glew(); // Initialize glew for the scene graph render thread.
  }

  // The render thread handles all our painting.
  draw_canvas_gl();
  //_interactive_display->Render();
  swapBuffers();
}

void NodeGraphEditor::draw_canvas_gl() {
  makeCurrent();
  _canvas->clean_state();
  _canvas->draw_gl();
}

void NodeGraphEditor::mousePressEvent(QMouseEvent *event) {
  MouseInfo info = get_mouse_info(event);
  HitRegion region;
  _canvas->get_current_interaction()->pressed(info, region);
  update();
}

void NodeGraphEditor::mouseReleaseEvent(QMouseEvent* event) {
  MouseInfo info = get_mouse_info(event);
  _canvas->get_current_interaction()->released(info);
  update();
}

void NodeGraphEditor::mouseMoveEvent(QMouseEvent *event) {
  MouseInfo info = get_mouse_info(event);
  _canvas->get_current_interaction()->moved(info);
  update();
}

void NodeGraphEditor::mouseDoubleClickEvent(QMouseEvent *event) {
  MouseInfo info = get_mouse_info(event);
  update();
}

void NodeGraphEditor::wheelEvent(QWheelEvent *event) {
  WheelInfo info;
  get_wheel_info(event,info);
  _canvas->get_current_interaction()->wheel_rolled(info);
  event->accept();
  update();
}

void NodeGraphEditor::keyPressEvent(QKeyEvent* event) {
  KeyInfo info = get_key_info_qt(event);
  update();
}

void NodeGraphEditor::keyReleaseEvent(QKeyEvent * event) {
  KeyInfo info = get_key_info_qt(event);
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

