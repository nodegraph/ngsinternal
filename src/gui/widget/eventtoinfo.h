#include <base/device/transforms/keyinfo.h>
#include <base/device/transforms/mouseinfo.h>
#include <base/device/transforms/wheelinfo.h>

class QTouchEvent;
class QMouseEvent;
class QHoverEvent;
class QKeyEvent;
class QWheelEvent;


namespace ngs {

  MouseInfo get_mouse_info(QTouchEvent *event, int device_pixel_ratio, size_t index = 0);
  MouseInfo get_mouse_info(QMouseEvent *event, int device_pixel_ratio);

  MouseInfo get_hover_info(QHoverEvent* event);

  KeyInfo get_key_info_qt(QKeyEvent* event);
  WheelInfo get_wheel_info(QWheelEvent* event);

}
