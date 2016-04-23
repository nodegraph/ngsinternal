#include <base/memoryallocator/taggednew.h>
#include <base/memoryallocator/bootstrap.h>

#include <base/device/devicebasictypesgl.h>
#include <base/device/transforms/glmhelper.h>
#include <base/objectmodel/entity.h>
#include <base/objectmodel/basefactory.h>
#include <components/interactions/shapecanvas.h>
#include <base/utils/fileutil.h>
#include <components/compshapes/compshapecollective.h>
#include <components/interactions/groupinteraction.h>
#include <components/compshapes/compshapecollective.h>
#include <components/entities/guientities.h>
#include <components/interactions/graphbuilder.h>
#include <gui/qt/qml/nodegraphquickitem.h>
#include <gui/qt/qml/nodegraphquickitemglobals.h>

// Qt.
#include <QtCore/QThread>
#include <QtGui/QGuiApplication>
#include <QtWidgets/QApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickView>

#include <gui/qt/qml/fborenderer.h>
#include <gui/qt/qml/fboworker.h>
#include <gui/qt/qml/nodegraphquickitem.h>

#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
#include <QtWebEngine/QtWebEngine>
#endif

#include <QtWebView/QtWebView>

// System.
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
  using namespace ngs;

  int execReturn = 0;

  // Startup the memory tracker.
  bootstrap_memory_tracker();
  {
    // Register a gl type.
    qRegisterMetaType<GLsizei>("GLsizei");

    // Build the app root.
    g_app_root = new_ff QMLAppEntity(NULL, "app");
    g_app_root->create_internals();

    // Build the root group.
    Entity* root_group = new_ff GroupNodeEntity(g_app_root, "root");
    root_group->create_internals();

    // App the FBOWorker and FBORender components to the app root.
    new_ff FBORenderer(g_app_root);
    new_ff FBOWorker(g_app_root);

    // Link up our default dependencies.
    g_app_root->initialize_deps();
    g_app_root->update_deps_and_hierarchy();

    QApplication app(argc, argv);
#if (GLES_MAJOR_VERSION <= 3)
  // This sets up the app to use opengl es.
  QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
  QSurfaceFormat format;
  format.setRenderableType(QSurfaceFormat::OpenGLES);
  QSurfaceFormat::setDefaultFormat(format);
#endif
#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
    QtWebEngine::initialize();
#else
    QtWebView::initialize();
#endif


    //QGuiApplication app(argc, argv);
    qmlRegisterType<NodeGraphQuickItem>("SceneGraphRendering", 1, 0, "NodeGraphQuickItem");

    // Create our qml engine.
    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();

    // Load our qml doc.
    engine.load(QUrl(QStringLiteral("qrc:/deviceitem.qml")));

    // Get the top level qml objects.
    QList<QObject *> root_objects = engine.rootObjects();
    qDebug() << "number of root objects is: " << root_objects.size();
    //static_cast<QQuickWindow *> (engine.rootObjects().first());

    execReturn = app.exec();
  }

  // Cleanup.
  delete_ff(g_app_root);

  // Shutdown.
  shutdown_memory_tracker();
  return execReturn;
}



