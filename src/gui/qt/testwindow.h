#pragma once
#include <gui/qt/gui_qt_export.h>
#include <QtOpenGL/QGLWidget>

namespace ngs {

class GUI_QT_EXPORT TestWindow : public QGLWidget {
 public:
  TestWindow(QWidget * parent = 0);
  virtual ~TestWindow();

  void (*test_callback)();

  virtual void paintGL();

 private:
  bool _done;

};


}
