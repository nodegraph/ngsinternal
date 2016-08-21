#include <QtWidgets/QApplication>
#include <QtGui/QGuiApplication>
//#include <QtQml/QQmlApplicationEngine>
#include <QtCore/QDebug>

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
#include <QtQuick/QQuickView>
#include <QtGui/QPainter>

#include <ngsversion.h>
#include <base/memoryallocator/taggednew.h>
#include <base/memoryallocator/bootstrap.h>
#include <base/objectmodel/entity.h>

#include <components/computes/computeglobals.h>
#include <components/computes/inputcompute.h>

#include <entities/guientities.h>
#include <components/interactions/graphbuilder.h>
#include <gui/widget/splashscreen.h>
#include <guicomponents/comms/appcomm.h>
#include <guicomponents/comms/socketmessage.h>
#include <guicomponents/quick/fborenderer.h>
#include <guicomponents/quick/fboworker.h>
#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/quick/nodegraphquickitemglobals.h>
#include <guicomponents/quick/nodegraphrenderer.h>

#if (ARCH == ARCH_ANDROID)
#include <native/javabridge.h>
#endif

#include <cassert>
#include <iostream>


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



static void copy_dir(const QDir &src, const QDir &target) {
    // Make sure the target dir exists.
    target.mkpath(".");

    // Loop through the source dir.
    QFileInfoList infos = src.entryInfoList(QDir::Dirs | QDir::Files |  QDir::NoDot | QDir::NoDotDot | QDir::Hidden | QDir::System);

    foreach (const QFileInfo &info, infos){
      if (info.isDir()) {
        // We have a sub directory.
        QDir sub_src(src.filePath(info.filePath()));
        QDir sub_tgt(target.filePath(info.fileName()));
        copy_dir(sub_src, sub_tgt);
      } else {
        // We have a file.
        QFile src_file(info.filePath());
        QString tgt_file(target.filePath(info.fileName()));
        src_file.copy(tgt_file);
      }
    }
}


int main(int argc, char *argv[]) {
  using namespace ngs;
  int execReturn = 0;

  // Startup the memory tracker.
  bootstrap_memory_tracker();
  {

    // Create our application. Note that QGUIApplication has no dependency on widgets.
    QGuiApplication app(argc, argv);
    QApplication::setApplicationName("smashbrowse"); // This affects the user's data dir resolution.
    QApplication::setOrganizationDomain("smashbrowse.com");

    // Screen stats.
    //qDebug() << "physical dots per inch: " << QGuiApplication::primaryScreen()->physicalDotsPerInch();
    //qDebug() << "device pixel ratio: " << QGuiApplication::primaryScreen()->devicePixelRatio();
    //qDebug() << "width: " << QGuiApplication::primaryScreen()->size().width();
    //qDebug() << "height: " << QGuiApplication::primaryScreen()->size().height();

    // Setup our custom opengl settings.
    // We always use opengles no matter if we are on mobile or desktop.
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

    // Copy the chromeextension over to the user data dir, so that we can change some files.
    {
      QDir ext_src(AppComm::get_app_bin_dir() + "/../chromeextension");
      QDir ext_tgt(AppComm::get_user_data_dir() + "/chromeextension");
      copy_dir(ext_src, ext_tgt);
    }

    // Using the QQmlApplicationEngine would of allowed us to declare a ApplicationWindow on the QML side.
    // This would have been nicer but, it doesn't allow us to set the opengl context and version settings.
    // Hence we use a QQuickView as the ApplicationWindow and then load QML components into that.
    QQuickView view;
    g_quick_view = &view;

    // Set the app icon.
    QIcon icon(":images/octopus_blue.png");
    g_quick_view->setIcon(icon);
    g_quick_view->setTitle("Smash Browse");

    // Set the view size.
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


    g_qml_engine = view.engine();
    g_qml_engine->addImportPath("qrc:/qml");
    g_qml_engine->addImportPath("qrc:/");
    QQmlContext* context = g_qml_engine->rootContext();

    // Create the app root.
    g_app_root = new_ff QMLAppEntity(NULL, "app");
    g_app_root->create_internals();

    // Build the root group.
    Entity* root_group = new_ff GroupNodeEntity(g_app_root, "root");
    root_group->create_internals();

    // Register gl types.
    //qRegisterMetaType<GLsizei>("GLsizei");
    qRegisterMetaType<size_t>("size_t"); // Used by some of our c++ quick item classes. (eg. NodeGraphQuickItem::get_num_nodes())

    // Register our types.
    qmlRegisterType<SocketMessage>("SocketMessage", 1, 0, "SocketMessage");
    qmlRegisterUncreatableType<InputCompute>("InputCompute", 1, 0, "InputCompute", "You cannot create this type from QML.");

    // Expose some app level components to QML.
    FileModel* file_model = static_cast<QMLAppEntity*>(g_app_root)->get_file_model();
    NodeGraphQuickItem* node_graph = static_cast<QMLAppEntity*>(g_app_root)->get_node_graph_quick_item();
    g_app_comm = static_cast<QMLAppEntity*>(g_app_root)->get_app_comm();

    context->setContextProperty(QStringLiteral("file_model"), file_model);
    context->setContextProperty(QStringLiteral("node_graph_item"), node_graph);
    context->setContextProperty(QStringLiteral("app_comm"), g_app_comm);

    // Expose other useful objects to qml.
    context->setContextProperty("pdpi", QGuiApplication::primaryScreen()->physicalDotsPerInch());
    context->setContextProperty("dpr", QGuiApplication::primaryScreen()->devicePixelRatio());
    context->setContextProperty(QStringLiteral("quick_view"), g_quick_view);
#if (ARCH == ARCH_ANDROID)
    // Create our java bridge.
    JavaBridge *java_bridge = new_ff JavaBridge(&app);
    context->setContextProperty(QLatin1String("java_bridge"), java_bridge);
#endif

    // Show our splash page, while loading the real qml app.
    view.setSource(QUrl(QStringLiteral("qrc:/qml/smashbrowse/pages/SplashPage.qml")));
    view.rootObject()->setProperty("ngs_version", NGS_VERSION);
    view.show();
    view.update();
    app.processEvents();

    // Show our main app.
    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    // Embed the node graph quick item into the node graph page.
    QQuickItem* node_graph_page = view.rootObject()->findChild<QQuickItem*>("node_graph_page", Qt::FindChildrenRecursively);
    node_graph->setParentItem(node_graph_page);
    node_graph->setWidth(node_graph_page->width());
    node_graph->setHeight(node_graph_page->height());

    // Run the Qt loop.
    execReturn = app.exec();
  }

  // Cleanup.
  delete_ff(g_app_root);

  // Shutdown.
  shutdown_memory_tracker();
  return execReturn;
}

