#pragma once
#include <gui/qt/gui_qt_export.h>

class QQuickWindow;
class QQmlApplicationEngine;

namespace ngs {

class SplashScreen;
class Entity;


// This is used by our custom NodeGraphQuickItem class and sub-components.
// It allows to feed this variable to the internal components without us
// passing it through as method arguments.
GUI_QT_EXPORT extern Entity* g_app_root;
GUI_QT_EXPORT extern QQuickWindow* g_quick_window;
GUI_QT_EXPORT extern QQmlApplicationEngine* g_qml_app_engine;
GUI_QT_EXPORT extern SplashScreen* g_splash_screen;
}
