#include <native/scripts.h>
#include <native/treeitem.h>
#include <native/variantmaptreemodel.h>
#include <components/computes/socketmessage.h>

#include <QtWidgets/QApplication>
#include <QtGui/QGuiApplication>
//#include <QtQml/QQmlApplicationEngine>

#include <QtCore/QDebug>

#include <QtGui/QScreen>
#include <QtWidgets/QSplashScreen>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QStaticAssertFailure>
#include <QtQml/QtQml>
#include <QtQml/QQmlEngine>
//#include <QtWebKitWidgets/QWebView>
//#include <private/qquickwebview_p.h>
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
#include <gui/quick/nodegraphquickitem.h>
#include <gui/quick/nodegraphrenderer.h>
#include <gui/quick/nodegraphquickitemglobals.h>
#include <components/computes/appcommunication.h>
#include <gui/quick/fborenderer.h>
#include <gui/quick/fboworker.h>
#include <gui/widget/splashscreen.h>

#if (ARCH == ARCH_ANDROID)
#include <native/javabridge.h>
#endif

#include <cassert>
#include <iostream>


#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
#include <QtWebEngine/QtWebEngine>
#else
#include <QtWebView/QtWebView>
#endif

//int main(int argc, char *argv[]) {
//  qDebug() << "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ\n";
//  // Create our application. Note that QGUIApplication has no dependency on widgets.
//  QApplication app(argc, argv);
//  return app.exec();
//}

#if ARCH == ARCH_IOS
  #include <QtPlugin>
  Q_IMPORT_PLUGIN(QtQuick2Plugin)
  Q_IMPORT_PLUGIN(QtQuickControlsPlugin)
  Q_IMPORT_PLUGIN(QtQuick2DialogsPlugin)
  Q_IMPORT_PLUGIN(QtQuick2WindowPlugin)

  //Q_IMPORT_PLUGIN(DialogPlugin)
  //Q_IMPORT_PLUGIN(WindowPlugin)
  Q_IMPORT_PLUGIN(QmlFolderListModelPlugin)
  Q_IMPORT_PLUGIN(QmlSettingsPlugin)
  //Q_IMPORT_PLUGIN(DialogsPrivatePlugin)
  Q_IMPORT_PLUGIN(QtQuickLayoutsPlugin)
  Q_IMPORT_PLUGIN(QtQuickExtrasPlugin)
  //Q_IMPORT_PLUGIN(QtGraphicalEffectsPrivate)
  //Q_IMPORT_PLUGIN(ModelsPlugin)


  Q_IMPORT_PLUGIN(QtQuick2DialogsPrivatePlugin)
  Q_IMPORT_PLUGIN(QtQmlModelsPlugin)
  Q_IMPORT_PLUGIN(QtQuick2PrivateWidgetsPlugin)

  // WebView.=.
  Q_IMPORT_PLUGIN(QWebViewModule)
  Q_IMPORT_PLUGIN(QtGraphicalEffectsPlugin)

  Q_IMPORT_PLUGIN(QDDSPlugin)
  Q_IMPORT_PLUGIN(QICNSPlugin)
  Q_IMPORT_PLUGIN(QICOPlugin)
  Q_IMPORT_PLUGIN(QTgaPlugin)
  Q_IMPORT_PLUGIN(QTiffPlugin)
  Q_IMPORT_PLUGIN(QWbmpPlugin)
  Q_IMPORT_PLUGIN(QWebpPlugin)
  Q_IMPORT_PLUGIN(QQmlDebuggerServiceFactory)
  Q_IMPORT_PLUGIN(QQmlInspectorServiceFactory)
  Q_IMPORT_PLUGIN(QLocalClientConnectionFactory)
  Q_IMPORT_PLUGIN(QQmlNativeDebugConnectorFactory)
  Q_IMPORT_PLUGIN(QQmlProfilerServiceFactory)
  Q_IMPORT_PLUGIN(QQmlDebugServerFactory)
  Q_IMPORT_PLUGIN(QTcpServerConnectionFactory)
  Q_IMPORT_PLUGIN(QGenericEnginePlugin)


#endif

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
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
#if ARCH == ARCH_MACOS
  format.setVersion(3, 3);
  format.setProfile(QSurfaceFormat::CoreProfile);
  QSurfaceFormat::setDefaultFormat(format);
  format.setSamples(2);
#else
#if (GLES_MAJOR_VERSION <= 3)
  // This sets up the app to use opengl es.
  QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
  format.setRenderableType(QSurfaceFormat::OpenGLES);
  format.setDepthBufferSize(24);
  format.setStencilBufferSize(8);
  format.setAlphaBufferSize(8);
  format.setSamples(2);
  QSurfaceFormat::setDefaultFormat(format);
#endif
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

    FileModel* file_model = new_ff FileModel();
    //QSortFilterProxyModel file_model_proxy;
    //file_model_proxy.setSourceModel(file_model);
    //file_model_proxy.sort(0,Qt::AscendingOrder);

    // Link up our default dependencies.
    g_app_root->initialize_deps();
    g_app_root->update_deps_and_hierarchy();



    // Register a gl type.
    qRegisterMetaType<GLsizei>("GLsizei");
    qRegisterMetaType<size_t>("size_t");



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

    qmlRegisterType<SocketMessage>("SocketMessage", 1, 0, "SocketMessage");

    //qmlRegisterType<HitInfo>("com.octoplier.api", 1, 0, "HitInfo");
    qmlRegisterUncreatableType<VariantMapTreeModel>("com.octoplier.api", 1, 0, "VariantMapTreeModel", "You cannot create this type from QML.");
    //qmlRegisterUncreatableType<SystemSignals>("com.octoplier.api.systemproperties", 1, 0, "SystemProperties", "You cannot create this type QML.");

    // Load the treemodel from disk.
    QFile file(":/qml/octoplier/data/default.txt");
    file.open(QIODevice::ReadOnly);
    VariantMapTreeModel* tree_model = new_ff VariantMapTreeModel((QString)file.readAll());
    file.close();


    // Create our qml engine.
//    QQmlApplicationEngine engine;
//    g_qml_app_engine = &engine;
//    engine.addImportPath("qrc:/qml");
//    engine.addImportPath("qrc:/");
//
//    QQmlContext *context = engine.rootContext();

    QQuickView view;
    g_quick_view = &view;
#if ARCH == ARCH_WINDOWS
    view.setWidth(800);
    view.setHeight(1024);
    view.setMaximumWidth(800);
    view.setMaximumHeight(1024);
    view.setMinimumWidth(800);
    view.setMinimumHeight(1024);
#else
    view.setWidth(QGuiApplication::primaryScreen()->size().width() );
    view.setHeight(QGuiApplication::primaryScreen()->size().height() );
#endif
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setFormat(format);
    view.setColor(QColor(3,169,244,255));
    view.update();
    g_qml_engine = view.engine();
    g_qml_engine->addImportPath("qrc:/qml");
    g_qml_engine->addImportPath("qrc:/");



#if ARCH == ARCH_IOS
  // Register QML types.
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2Plugin().instance())->registerTypes("QtQuick");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2PrivateWidgetsPlugin().instance())->registerTypes("QtQuick.PrivateWidgets");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->registerTypes("QtQuick.Controls");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->registerTypes("QtQuick.Controls.Private");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->registerTypes("QtQuick.Controls.Styles");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2DialogsPlugin().instance())->registerTypes("QtQuick.Dialogs");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2DialogsPrivatePlugin().instance())->registerTypes("QtQuick.Dialogs.Private");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickLayoutsPlugin().instance())->registerTypes("QtQuick.Layouts");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2WindowPlugin().instance())->registerTypes("QtQuick.Window");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickExtrasPlugin().instance())->registerTypes("QtQuick.Extras");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQmlModelsPlugin().instance())->registerTypes("QtQml.Models");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtGraphicalEffectsPlugin().instance())->registerTypes("QtGraphicalEffects.private");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QWebViewModule().instance())->registerTypes("QtWebView");

  // Initialize engine with these QML types.

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2Plugin().instance())->initializeEngine(g_qml_engine, "QtQuick");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2PrivateWidgetsPlugin().instance())->initializeEngine(g_qml_engine, "QtQuick.PrivateWidgets");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->initializeEngine(g_qml_engine, "QtQuick.Controls");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->initializeEngine(g_qml_engine,
                                                                                                            "QtQuick.Controls.Private");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->initializeEngine(g_qml_engine,
                                                                                                            "QtQuick.Controls.Styles");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2DialogsPlugin().instance())->initializeEngine(g_qml_engine, "QtQuick.Dialogs");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2DialogsPrivatePlugin().instance())->initializeEngine(g_qml_engine, "QtQuick.Dialogs.Private");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickLayoutsPlugin().instance())->initializeEngine(g_qml_engine, "QtQuick.Layouts");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2WindowPlugin().instance())->initializeEngine(g_qml_engine, "QtQuick.Window");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickExtrasPlugin().instance())->initializeEngine(g_qml_engine, "QtQuick.Extras");
  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQmlModelsPlugin().instance())->initializeEngine(g_qml_engine, "QtQml.Models");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QWebViewModule().instance())->initializeEngine(g_qml_engine, "QtWebView");

  qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtGraphicalEffectsPlugin().instance())->initializeEngine(g_qml_engine, "QtGraphicalEffects");

#endif


    QQmlContext* context = g_qml_engine->rootContext();


    // Create helpers.
    Utils* utils = new_ff Utils(g_qml_engine);
    Scripts* scripts = new_ff Scripts();

    //qDebug() << "physical dots per inch: " << QGuiApplication::primaryScreen()->physicalDotsPerInch();
    //qDebug() << "device pixel ratio: " << QGuiApplication::primaryScreen()->devicePixelRatio();
    //qDebug() << "width: " << QGuiApplication::primaryScreen()->size().width();
    //qDebug() << "height: " << QGuiApplication::primaryScreen()->size().height();

    // Expose some c++ objects to qml.
    context->setContextProperty("pdpi", QGuiApplication::primaryScreen()->physicalDotsPerInch());
    context->setContextProperty("dpr", QGuiApplication::primaryScreen()->devicePixelRatio());
    context->setContextProperty(QStringLiteral("utils"), utils);
    context->setContextProperty(QStringLiteral("initialUrl"), Utils::url_from_input("www.google.com"));
    context->setContextProperty(QStringLiteral("scripts"), scripts);
    context->setContextProperty(QStringLiteral("treemodel"), tree_model);
    context->setContextProperty(QStringLiteral("file_model"), file_model);
    context->setContextProperty(QStringLiteral("quick_view"), g_quick_view);

    //context->setContextProperty(QStringLiteral("system_properties"),dynamic_cast<QObject*>(ng_root->get_component<SystemSignals>()));

    // Create our cpp bridge.
    g_app_comm = new_ff AppCommunication(&app);
    context->setContextProperty(QStringLiteral("app_comm"), g_app_comm);

    view.setSource(QUrl(QStringLiteral("qrc:/qml/loader.qml")));
    view.show();
    view.update();
    app.processEvents();

    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    //qApp->setQuitOnLastWindowClosed(false);

    // Get the top level qml objects.
    //QList<QObject *> root_objects = engine.rootObjects();
    //qDebug() << "number of root objects is: " << root_objects.size();
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
    delete_ff(g_app_comm);
    delete_ff(tree_model)

  }

  // Cleanup.
  delete_ff(g_app_root);

  // Shutdown.
  shutdown_memory_tracker();
  //return execReturn;
  return 0;
}

