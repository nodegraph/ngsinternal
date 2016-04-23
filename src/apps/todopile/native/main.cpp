#include <native/app.h>
#include <native/scripts.h>
#include <native/treeitem.h>
#include <native/variantmaptreemodel.h>
#include <native/utils.h>

#include <QtWidgets/QApplication>
//#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>

#include <QtCore/QDebug>

#include <QtGui/QScreen>
#include <QtWidgets/QSplashScreen>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QStaticAssertFailure>
#include <QtQml/QtQml>
//#include <QtWidgets/QSplashScreen>
//#include <QtCore/QTimer>


#include <QtQuick/QQuickWindow>
#include <QtGui/QPainter>

#include <base/memoryallocator/taggednew.h>
#include <base/memoryallocator/bootstrap.h>
#include <base/objectmodel/entity.h>

#include <components/computes/computeglobals.h>

#include <components/entities/guientities.h>
#include <components/interactions/graphbuilder.h>
#include <gui/qt/qml/nodegraphquickitem.h>
#include <gui/qt/qml/nodegraphrenderer.h>
#include <gui/qt/qml/nodegraphquickitemglobals.h>
#include <native/cppbridge.h>
#include <gui/qt/qml/fborenderer.h>
#include <gui/qt/qml/fboworker.h>
#include <gui/qt/splashscreen.h>

#if (ARCH == ARCH_ANDROID)
#include <native/javabridge.h>
#endif

#include <cassert>
#include <iostream>


#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
#include <QtWebEngine/QtWebEngine>
#endif

#include <QtWebView/QtWebView>

//int main(int argc, char *argv[]) {
//  qDebug() << "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ\n";
//  // Create our application. Note that QGUIApplication has no dependency on widgets.
//  QApplication app(argc, argv);
//  return app.exec();
//}

int main(int argc, char *argv[]) {

//  QSurfaceFormat format;
//  format.setDepthBufferSize(24);
//  QSurfaceFormat::setDefaultFormat(format);


  using namespace ngs;

  int execReturn = 0;

  // Startup the memory tracker.
  bootstrap_memory_tracker();
  {

    // Create our application. Note that QGUIApplication has no dependency on widgets.
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

//    QSurfaceFormat fmt;
//
//    if (ARCH == ARCH_ANDROID) {
//      fmt.setProfile(QSurfaceFormat::CoreProfile);
//      fmt.setRenderableType(QSurfaceFormat ::OpenGLES);
//      fmt.setVersion(3, 0); // Nexus 5 has gles 3.0, not 3.1.
//    } else {
//      //fmt.setRenderableType(QSurfaceFormat ::OpenGL);
//      //fmt.setVersion(4, 5); // 4.5 should be available on desktops
//    }
//    QSurfaceFormat::setDefaultFormat(fmt);

    // Create the app root.
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





    // Register a gl type.
    qRegisterMetaType<GLsizei>("GLsizei");
    qRegisterMetaType<size_t>("size_t");


    // Show our splash screen.
    //g_splash_screen = new_ff SplashScreen();
    //g_splash_screen->show();


//    brush.setColor(QColor(0xFFFFFFFF));
//    painter.setBrush(brush);
//    painter.drawRect(100, 100, 32, 30);
//    painter.setBrush(Qt::NoBrush);
//
//    /* display text */
//    pen.setColor(0xFF000000);
//    painter.setPen(pen);
//    font.setFamily((QFontDatabase::applicationFontFamilies(0).at(0).toLocal8Bit().constData()));
//    font.setPointSize(20);
//    //font.setPixelSize(20); //alternative
//    painter.setFont(font);@




    //QGuiApplication app(argc, argv);
    qmlRegisterType<NodeGraphQuickItem>("NodeGraphRendering", 1, 0, "NodeGraphQuickItem");
    qmlRegisterType<NodeGraphRenderer>("NodeGraphTesting", 1, 0, "NodeGraphRenderer");

    //qmlRegisterType<HitInfo>("com.todopile.api", 1, 0, "HitInfo");
    qmlRegisterUncreatableType<VariantMapTreeModel>("com.todopile.api", 1, 0, "VariantMapTreeModel", "You cannot create this type from QML.");
    //qmlRegisterUncreatableType<SystemSignals>("com.todopile.api.systemproperties", 1, 0, "SystemProperties", "You cannot create this type QML.");

    // Load the treemodel from disk.
    QFile file(":/qml/todopile/testdata/default.txt");
    file.open(QIODevice::ReadOnly);
    VariantMapTreeModel* tree_model = new_ff VariantMapTreeModel((QString)file.readAll());
    file.close();

    // Create our qml engine.
    QQmlApplicationEngine engine;
    g_qml_app_engine = &engine;
    engine.addImportPath("qrc:/qml");
    engine.addImportPath("qrc:/");

    QQmlContext *context = engine.rootContext();

    // Create helpers.
    Utils* utils = new_ff Utils(&engine);
    Scripts* scripts = new_ff Scripts();

    qDebug() << "physical dots per inch: " << QGuiApplication::primaryScreen()->physicalDotsPerInch() << "\n";
    qDebug() << "device pixel ratio: " << QGuiApplication::primaryScreen()->devicePixelRatio() << "\n";
    qDebug() << "width: " << QGuiApplication::primaryScreen()->size().width() << "\n";
    qDebug() << "height: " << QGuiApplication::primaryScreen()->size().height() << "\n";

    // Expose some c++ objects to qml.
    context->setContextProperty("pdpi", QGuiApplication::primaryScreen()->physicalDotsPerInch());
    context->setContextProperty("dpr", QGuiApplication::primaryScreen()->devicePixelRatio());
    context->setContextProperty(QStringLiteral("utils"), utils);
    context->setContextProperty(QStringLiteral("initialUrl"), Utils::url_from_input("www.google.com"));
    context->setContextProperty(QStringLiteral("scripts"), scripts);
    context->setContextProperty(QStringLiteral("treemodel"), tree_model);

    //context->setContextProperty(QStringLiteral("system_properties"),dynamic_cast<QObject*>(ng_root->get_component<SystemSignals>()));

    // Create our cpp bridge.
    CppBridge* cpp_bridge = NULL;
    cpp_bridge = new_ff CppBridge(&app);
    context->setContextProperty(QStringLiteral("cpp_bridge"), cpp_bridge);

    // Load our qml doc.
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    // Get the top level qml objects.
    QList<QObject *> root_objects = engine.rootObjects();
    qDebug() << "number of root objects is: " << root_objects.size();
//    QQuickWindow *window = static_cast<QQuickWindow *>(engine.rootObjects().first());

    // Set our multisampling settings.
    // Currently this doesn't seem to do anything.
//    QSurfaceFormat format = window->format();
//    format.setSamples(16);
//    window->setFormat(format);



    // Find the web view.
//    QObject* web_view = root_objects[0]->findChild<QObject*>(QStringLiteral("web_view_object"));
//    if (web_view) {
//      qDebug() << "found web view";
//    }

    // Get our ng page which is create from loading the QML above.
//    QObject* obj = root_objects[0]->findChild<QObject*>(QStringLiteral("node_graph_page_object"));
//    NodeGraphQuickItem* ng_page = dynamic_cast<NodeGraphQuickItem*>(obj);



#if (ARCH == ARCH_ANDROID)
    // Create our java bridge.
    JavaBridge *java_bridge = new_ff JavaBridge(&app);
    context->setContextProperty(QLatin1String("java_bridge"), java_bridge);
#endif

//      // Find the node context menu.
//      {
//        QObject* node_context_menu = root_objects[0]->findChild<QObject*>(QStringLiteral("nodeContextMenu"));
//        if (node_context_menu) {
//          QObject::connect(node_context_menu, SIGNAL(triggered()),
//                           &myClass, SLOT(cppSlot(QString)));
//        }
//      }


    // Create some nodes.
    //App::load_demo(ng_root);
    //ng_page->load();

    execReturn = app.exec();

    delete_ff(scripts);
    delete_ff(utils);
    delete_ff(cpp_bridge);
    delete_ff(tree_model)

  }

  // Cleanup.
  delete_ff(g_app_root);

  // Shutdown.
  shutdown_memory_tracker();
  return execReturn;
}

