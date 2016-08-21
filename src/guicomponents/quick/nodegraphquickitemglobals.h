#pragma once
#include <guicomponents/quick/quick_export.h>

class QQuickView;
class QQmlEngine;

namespace ngs {

class SplashScreen;
class Entity;


// This is used by our custom NodeGraphQuickItem class and sub-components.
// It allows to feed this variable to the internal components without us
// passing it through as method arguments.
QUICK_EXPORT extern Entity* g_app_root;
QUICK_EXPORT extern QQuickView* g_quick_view;
QUICK_EXPORT extern QQmlEngine* g_qml_engine;
}
