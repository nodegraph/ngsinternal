#pragma once
#include <gui/qt/gui_qt_export.h>

class QQuickView;
class QQmlEngine;

namespace ngs {

class SplashScreen;
class Entity;


// This is used by our custom NodeGraphQuickItem class and sub-components.
// It allows to feed this variable to the internal components without us
// passing it through as method arguments.
GUI_QT_EXPORT extern Entity* g_app_root;
GUI_QT_EXPORT extern QQuickView* g_quick_view;
GUI_QT_EXPORT extern QQmlEngine* g_qml_engine;
GUI_QT_EXPORT extern SplashScreen* g_splash_screen;
}
