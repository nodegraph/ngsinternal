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
#include <components/interactions/graphbuilder.h>
#include <components/compshapes/compshapecollective.h>
#include <entities/guientities.h>
#include <components/interactions/graphbuilder.h>
#include <guicomponents/quick/fborenderer.h>
#include <guicomponents/quick/fboworker.h>
#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/quick/nodegraphquickitemglobals.h>
#include <QtCore/QThread>
#include <QtGui/QGuiApplication>
#include <QtWidgets/QApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickView>

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


    QSurfaceFormat format;
#if ARCH == ARCH_MACOS
  format.setVersion(3, 3);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setSamples(2);
  format.setDepthBufferSize(24);
  format.setStencilBufferSize(8);
  QSurfaceFormat::setDefaultFormat(format);

#else
#if (GLES_MAJOR_VERSION <= 3)
  // This sets up the app to use opengl es.
  QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
  format.setRenderableType(QSurfaceFormat::OpenGLES);
  QSurfaceFormat::setDefaultFormat(format);
#endif
#endif

    QApplication app(argc, argv);

    // Register a gl type.
    qRegisterMetaType<GLsizei>("GLsizei");

    // Build the app root.
    g_app_root = new_ff QMLAppEntity(NULL, "app");
    g_app_root->create_internals();

    // Build the root group.
    Entity* root_group = new_ff GroupNodeEntity(g_app_root, "root");
    root_group->create_internals();

    QQuickView view;
    g_quick_view = &view;
    view.show();
    view.update();
    view.setWidth(640);
    view.setHeight(480);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setFormat(format);

    // Embed the node graph quick item into the node graph page.
    NodeGraphQuickItem* node_graph = static_cast<QMLAppEntity*>(g_app_root)->get_node_graph_quick_item();
    QQmlContext* context = view.engine()->rootContext();
    context->setContextProperty(QStringLiteral("node_graph_item"), node_graph);

    view.setSource(QUrl(QStringLiteral("qrc:/deviceitem.qml")));

    QQuickItem* page = view.rootObject();
    assert(page);
    node_graph->setParentItem(page);
    node_graph->setWidth(640);
    node_graph->setHeight(480);

    // Build a test graph.
    g_app_root->initialize_deps();
    g_app_root->update_deps_and_hierarchy();
    static_cast<QMLAppEntity*>(g_app_root)->get_graph_builder()->build_test_graph();

    execReturn = app.exec();
  }

  // Cleanup.
  delete_ff(g_app_root);

  // Shutdown.
  shutdown_memory_tracker();
  return execReturn;
}



