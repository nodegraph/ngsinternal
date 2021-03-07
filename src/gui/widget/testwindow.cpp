

//#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>

#include <gui/widget/testwindow.h>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

#include <iostream>

void test();

namespace ngs {

TestWindow::TestWindow(QWidget * parent):
    QGLWidget(parent),
    _done(false),
    test_callback(NULL){

#if ARCH == ARCH_MACOS
  QGLFormat glFormat;
  glFormat.setVersion(3, 3);
  glFormat.setProfile(QGLFormat::CoreProfile);
  //glFormat.setSampleBuffers(true);
  QGLFormat::setDefaultFormat(glFormat);
  //glFormat.setSwapInterval(1);
  setFormat(glFormat);
#endif

//  QSurfaceFormat fmt;
//  fmt.setProfile(QSurfaceFormat::CoreProfile);
//  fmt.setRenderableType(QSurfaceFormat ::OpenGLES);
//  fmt.setVersion(2, 0);
  resize(512, 512);
}

TestWindow::~TestWindow() {
}

void TestWindow::paintGL() {
  if (_done) {
    //gpu(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    //gpu(glClearColor(0, 0, 1, 1));
    //gpu(glClear( GL_COLOR_BUFFER_BIT));

    QCoreApplication::quit();

    // We won't get here.
    swapBuffers();
    makeCurrent();
    return;
  } else {
    _done = true;
  }

  {
    //std::cerr << "OpenGL version " << glGetString(GL_VERSION)<< endl;
    //std::cerr << "GLSL version " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
  }

  try {
    test_callback();
  } catch (...) {
    std::cerr << "TestWindow has caught an exception!\n";
  }
}

}
