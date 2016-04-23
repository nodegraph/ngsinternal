#include <gui/qt/qml/nodegraphquickitemglobals.h>
#include <cstddef>

namespace ngs {
  Entity* g_app_root = NULL;
  QQuickWindow* g_quick_window = NULL;
  QQmlApplicationEngine* g_qml_app_engine = NULL;
  SplashScreen* g_splash_screen = NULL;
}
