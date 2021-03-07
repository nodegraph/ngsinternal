#include <gui/widget/splashscreen.h>
#include <QtWidgets/QApplication>
#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QScreen>
#include <QtWidgets/QStyleOptionProgressBar>

#include <iostream>
#include <cmath>

namespace ngs {

SplashScreen::SplashScreen():
    QSplashScreen(),
    _max_progress(-1),
    _progress(0),
    _marker_1(0){

  // Determine the screen size to fill with our splash screen.
  QScreen *screen = QApplication::screens().at(0);

#if (ARCH == ARCH_WINDOWS)
  _width = 800;
  _height = 1024;
#else
  _width = screen->size().width();
  _height = screen->size().height();
#endif

  // Load our logo.
  QPixmap logo;
  logo.load(":/images/robot_white.png");

  // Our scaled logo.
  int min = std::min(_width, _height);
  min /= 4;
  QPixmap logo_scaled = logo.scaled(min,min);

  // Our splash image.
  QPixmap splash(QSize(_width,_height));
  QPainter painter(&splash);
  QColor app_color(3,169,244,255);
  painter.fillRect(0,0,_width,_height,app_color);
  int logo_x = (_width-logo_scaled.width())/2.0f;
  int logo_y = (_height-logo_scaled.height())/2.0f;
  painter.drawPixmap(logo_x, logo_y, logo_scaled);
  setPixmap(splash);

  // Our font.
  QFont splashFont;
  splashFont.setFamily("Arial");
  splashFont.setBold(true);
  splashFont.setPixelSize(_height/10);
  //splashFont.setStretch(125);
  setFont(splashFont);

  // Our progress bar brush.
  _brush.setColor(Qt::white);
  _brush.setStyle(Qt::SolidPattern);

  // Our text rect positioning.
  int text_height = _height/10.0f;
  int text_y = _height * 0.7f;
  int text_margin = _width/50.0f;
  int max_text_width = _width - (2*text_margin);
  _text_rect = QRect(text_margin, text_y, max_text_width, text_height);

}

SplashScreen::~SplashScreen() {
}

void SplashScreen::set_max_progress(size_t num) {
  qDebug() << "zzzzzzzzzzz " << num;
  _max_progress = num;
}

void SplashScreen::mark_progress() {
  ++_progress;
  float frac = 100.0f*(float)_progress/(float)_max_progress;
  int percentage = round(frac);
  QString message = QString::number(percentage);

  qDebug() << "progress,max: " << _progress << "," << _max_progress;
  qDebug() << "SplashScreen progress: " << percentage << " , " << frac;

  showMessage(message, Qt::AlignBottom|Qt::AlignHCenter, Qt::white);

  repaint();

  if (percentage == 100) {
    deleteLater();
  }
}

size_t SplashScreen::get_progress() {
  return _progress;
}

void SplashScreen::set_marker_1(size_t mark) {
  _marker_1 = mark;
}

bool SplashScreen::at_marker_1() {
  if (_progress == _marker_1) {
    return true;
  }
  return false;
}

bool SplashScreen::is_finished() {
  if (_progress == _max_progress) {
    return true;
  }
  return false;
}



void SplashScreen::drawContents(QPainter *painter)
{
  //QSplashScreen::drawContents(painter);

  // Draw the text.
  painter->setPen(Qt::white);
  painter->setFont(font());

  std::cerr << "drawing message: " << message().toStdString() << "\n";
  painter->drawText(_text_rect, Qt::AlignBottom|Qt::AlignHCenter, message());

  // Draw the progress bar.
  int progress_height = _height/50.0f;
  int progress_y = _height * 0.85f;
  int margin = _width/50.0f;
  int max_progress_width = _width - (2*margin);
  int progress_width = (float)max_progress_width * ((float)_progress/(float)_max_progress);
  painter->fillRect(margin, progress_y, progress_width, progress_height, _brush);

//  // Set style for progressbar...
//  QStyleOptionProgressBarV2 pbstyle;
//  pbstyle.initFrom(this);
//  pbstyle.state = QStyle::State_Enabled;
//  pbstyle.textVisible = false;
//  pbstyle.minimum = 0;
//  pbstyle.maximum = _max_progress;
//  pbstyle.progress = _progress;
//  pbstyle.invertedAppearance = false;
//
//  //qDebug() << "progress loc: " << margin << "," << progress_y << "," << progress_width << "," << progress_height << "\n";
//  pbstyle.rect = QRect(margin, progress_y, progress_width, progress_height); // Where is it.
//
//  // Draw it...
//  style()->drawControl(QStyle::CE_ProgressBar, &pbstyle, painter, this);
}

}
