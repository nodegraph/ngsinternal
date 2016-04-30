#pragma once
#include <gui/gui_export.h>
#include <QtWidgets/QSplashScreen>

namespace ngs {

class GUI_EXPORT SplashScreen : public QSplashScreen {
Q_OBJECT
 public:
  explicit SplashScreen();
  virtual ~SplashScreen();

signals:

 public slots:

 void set_max_progress(size_t num);
 void mark_progress();
 size_t get_progress();

 void set_marker_1(size_t mark);
 bool at_marker_1();

 bool is_finished();

 protected:
  void drawContents(QPainter *painter);

 private:
 size_t _max_progress;
 size_t _progress;
 size_t _marker_1;

 // Our dimensions.
 int _width;
 int _height;

 // Our text position.
 QRect _text_rect;

 // Our progress bar brush.
 QBrush _brush;
};

}
