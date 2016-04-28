

#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>

#include <gui/qt/testwindow.h>
#include <QtCore/QDebug>
#include <QtGui/QOpenGLFunctions>

#include <iostream>

void test();

namespace ngs {

TestWindow::TestWindow(QWidget * parent):
    QGLWidget(parent),
    _done(false){

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
    swapBuffers();
    return;
  } else {
    _done = true;
  }

  std::cerr << "xxxxxxxxxxxxxx\n";
  std::cerr << "glGetString: " << glGetString << "\n";

  {
    std::cerr << "OpenGL version " << glGetString(GL_VERSION)<< endl;
    std::cerr << "GLSL version " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
  }
//  {
//    QOpenGLFunctions gl;
//    gl.initializeOpenGLFunctions();
//
//    const GLubyte * version = gl.glGetString(GL_VERSION);
//    std::cerr << "ccccccccccccccc\n";
//    if (version == 0) {
//      std::cerr << "error getting the gl version string\n";
//    } else {
//      std::cerr << "qt reported gl version: " << version << "\n";
//    }
//  }

  {
    const GLubyte * version = glGetString(GL_VERSION);
    std::cerr << "qqqqqqqqqqqqqqqqq\n";
    if (version == 0) {
      std::cerr << "error getting the gl version string\n";
    } else {
      std::cerr << "version: " << version << "\n";
    }
  }

  //  QString versionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
//  qDebug() << "Driver Version String:" << versionString;
//  qDebug() << "Current Context:" << format();

  std::cerr << "yyyyy\n";
  try {
    test_callback();
  } catch (...) {
    std::cerr << "wwwwwww we caught an exception\n";
  }
  std::cerr << "zzzzz\n";
}

}
