#include <gui/widget/eventtoinfo.h>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QWheelEvent>

namespace ngs {

MouseInfo get_mouse_info(QTouchEvent *event, int device_pixel_ratio) {
  MouseInfo info;
  QList<QTouchEvent::TouchPoint> touchPoints = event->touchPoints();
  // We only use the first touch point.
  const QTouchEvent::TouchPoint &touchPoint = touchPoints.first();
  info.screen_pos = glm::vec2(device_pixel_ratio*touchPoint.pos().x(), device_pixel_ratio*touchPoint.pos().y());
  // The touch is always considered to be a left button mouse click.
  info.left_button=true;
  info.middle_button=false;
  info.right_button=false;
  // Touch are always considered to be modifier free.
  info.control_modifier=false;
  info.alt_modifier=false;
  info.shift_modifier=false;
  return info;
}

MouseInfo get_mouse_info(QMouseEvent *event, int device_pixel_ratio) {
  MouseInfo info;
  info.screen_pos = glm::vec2(device_pixel_ratio*event->x(), device_pixel_ratio*event->y());
  info.left_button=(event->button() == Qt::LeftButton) || (event->buttons() == Qt::LeftButton);
  info.middle_button=(event->button() == Qt::MidButton) || (event->buttons() == Qt::MidButton);
  info.right_button=(event->button() == Qt::RightButton) || (event->buttons() == Qt::RightButton);
  info.control_modifier=event->modifiers() & Qt::ControlModifier;
  info.alt_modifier=event->modifiers() & Qt::AltModifier;
  info.shift_modifier=event->modifiers() & Qt::ShiftModifier;
  return info;
}

MouseInfo get_hover_info(QHoverEvent* event) {
  return get_vec2_info(glm::vec2(event->posF().x(), event->posF().y()));
}

KeyInfo get_key_info_qt(QKeyEvent* event) {
  KeyInfo info;
  info.key_code = event->key();
  info.control_modifier=event->modifiers() & Qt::ControlModifier;
  info.alt_modifier=event->modifiers() & Qt::AltModifier;
  info.shift_modifier=event->modifiers() & Qt::ShiftModifier;
  return info;
}

WheelInfo get_wheel_info(QWheelEvent* event) {
  QPoint numPixels = event->pixelDelta();
  QPoint numDegrees = event->angleDelta() / 8.0f;

  WheelInfo info;
  if (!numPixels.isNull()) {
    info.x = numPixels.x();
    info.y = numPixels.y();
  } else if (!numDegrees.isNull()) {
    info.x = (float)numDegrees.x() / 15.0f;
    info.y = (float)numDegrees.y() / 15.0f;
  }
  return info;
}

}
