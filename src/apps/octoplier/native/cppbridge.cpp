#include <base/objectmodel/entity.h>
#include <components/compshapes/nodegraphselection.h>
#include <native/cppbridge.h>
#include <native/variantmaptreemodel.h>

#include <components/interactions/shapecanvas.h>
#include <gui/quick/nodegraphquickitem.h>
#include <gui/quick/nodegraphquickitemglobals.h>

#include <iostream>
#include <QtCore/QDebug>
#include <QtQml/QQmlApplicationEngine>
#include <QtQuick/QQuickView>
#include <QtWidgets/QSplashScreen>
#include <QtGui/QGuiApplication>

#include <components/computes/computeglobals.h>
#include <gui/widget/splashscreen.h>

namespace ngs {



CppBridge::CppBridge(QObject *parent)
    : QObject(parent) {
}

void CppBridge::post_init() {
  qDebug() << "RRRRRRRRRRRunning cpp bridge post init !!!!!!!!\n";

  // Find the NodePoolAPI.
//  QList<QObject *> root_objects = g_qml_app_engine->rootObjects();
//  g_node_pool_api = root_objects[0]->findChild<QObject*>(QStringLiteral("node_pool_api_object"));

  g_node_pool_api = g_quick_view->rootObject()->findChild<QObject*>(QStringLiteral("node_pool_api_object"));

  if (g_node_pool_api) {
    qDebug() << "found node pool api";
  } else {
    qDebug() << "Error: could not find node pool api";
  }

  if (g_splash_screen) {
    g_splash_screen->hide();
    delete_ff(g_splash_screen);
    qDebug() << "Deleted splash screen yo!";
  }
}

void CppBridge::set_max_load_progress(size_t max) {
  if (g_splash_screen) {
    g_splash_screen->set_max_progress(max);
  }
  //qApp->processEvents();
}

void CppBridge::mark_load_progress() {
  if (g_splash_screen) {
    g_splash_screen->mark_progress();
  }
  //qApp->processEvents();
}

size_t CppBridge::get_load_progress() {
  if (g_splash_screen) {
    return g_splash_screen->get_progress();
  }
  return 0;
}

void CppBridge::set_marker_1(size_t mark) {
  if (g_splash_screen) {
    g_splash_screen->set_marker_1(mark);
  }
  //qApp->processEvents();
}

bool CppBridge::at_marker_1() {
  if (g_splash_screen) {
    return g_splash_screen->at_marker_1();
  }
  return false;
}

bool CppBridge::is_finished_loading() {
  if (!g_splash_screen) {
    return false;
  }
  return g_splash_screen->is_finished();
}

void CppBridge::process_events() {
  qDebug() << "cppbridge processing events!!!";
  qApp->processEvents();
}

void CppBridge::on_test_message(const QString& message) {
  qDebug() << "c++ side received message: " << message << "\n";
}

void CppBridge::on_test_1(const QVariant& value) {
  //_tree_model->init(value);
}

void CppBridge::on_move_root(int child_row) {
  //_tree_model->push_root(child_row);
}

void CppBridge::reset_input_method() {
//  qApp->inputMethod()->commit();
//  qApp->inputMethod()->reset();
//  qApp->inputMethod()->update(Qt::ImCurrentSelection);


  qApp->inputMethod()->hide();
//  qApp->inputMethod()->show();
//  qApp->inputMethod()->invokeAction(QInputMethod::Click, 0);
  qApp->inputMethod()->hide();

  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();

  qApp->inputMethod()->hide();

  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();

  qApp->inputMethod()->hide();

  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
  qApp->processEvents();
}

void CppBridge::press_down_key(QObject* obj) {
  QKeyEvent *event = new QKeyEvent ( QEvent::KeyPress, Qt::Key_Close, Qt::NoModifier);
  qApp->postEvent(obj, event);
}

void CppBridge::release_down_key(QObject* obj) {
  QKeyEvent *event = new QKeyEvent ( QEvent::KeyRelease, Qt::Key_Close, Qt::NoModifier);
  qApp->postEvent(obj, event);
}

}
