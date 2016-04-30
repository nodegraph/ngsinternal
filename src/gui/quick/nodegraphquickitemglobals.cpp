#include <gui/quick/nodegraphquickitemglobals.h>
#include <cstddef>

namespace ngs {
  Entity* g_app_root = NULL;
  QQuickView* g_quick_view = NULL;
  QQmlEngine* g_qml_engine = NULL;
  SplashScreen* g_splash_screen = NULL;
}
