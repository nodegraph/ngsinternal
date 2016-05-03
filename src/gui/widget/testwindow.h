#pragma once
#include <gui/gui_export.h>
#include <QtOpenGL/QGLWidget>

namespace ngs {

class GUI_EXPORT TestWindow : public QGLWidget {
Q_OBJECT
 public:
  TestWindow(QWidget * parent = 0);
  virtual ~TestWindow();

  void (*test_callback)();

  virtual void paintGL();

 private:
  bool _done;

};


}
