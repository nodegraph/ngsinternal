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
#include <QtCore/QLockFile>

#include <ngsversion.h>
#include <base/memoryallocator/taggednew.h>
#include <base/memoryallocator/bootstrap.h>
#include <base/objectmodel/entity.h>

#include <components/computes/inputcompute.h>
#include <components/computes/jsonutils.h>

#include <entities/guientities.h>
#include <components/interactions/graphbuilder.h>
#include <gui/widget/splashscreen.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/quick/fborenderer.h>
#include <guicomponents/quick/fboworker.h>
#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/quick/nodegraphview.h>
#include <guicomponents/computes/javaprocess.h>
#include <mainlib/mainlib.h>

#include <cassert>
#include <iostream>


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

void test_json_utils() {
  // Test converting QJsonValues.
  ngs::JSONUtils::test_convert_to_bool();
  ngs::JSONUtils::test_convert_to_double();
  ngs::JSONUtils::test_convert_to_string();
  ngs::JSONUtils::test_convert_to_object();
  ngs::JSONUtils::test_convert_to_array();

  // Test deep merges of QJsonValues.
  ngs::JSONUtils::test_deep_merge_object_to_object();
  ngs::JSONUtils::test_deep_merge_array_to_array();
  ngs::JSONUtils::test_deep_merge_array_to_object();
  ngs::JSONUtils::test_deep_merge_object_to_array();
}

int run_main(int argc, char *argv[]) {
  using namespace ngs;
  int execReturn = 0;



  // Startup the memory tracker.
  bootstrap_memory_tracker();
  {
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

    // Create our application. Note that QGUIApplication has no dependency on widgets.
    QGuiApplication app(argc, argv);
    QApplication::setApplicationName("YouMacro"); // This affects the user's data dir resolution.
    QApplication::setOrganizationDomain("youmacro.com");

    // Check to make sure that there are no other instance running.
    bool first_instance = true;

    QString tmpDir = QDir::tempPath();
    QLockFile lockFile(tmpDir + "/youmacro.lock");
    if(!lockFile.tryLock(100)){
      first_instance = false;
    }

    // Create the app root.
    QMLAppEntity* app_root = new_ff QMLAppEntity(NULL, "app");
    app_root->create_internals();

    // Build the root group.
    Entity* root_group = new_ff GroupNodeEntity(app_root, "root");
    root_group->create_internals();

    // Now that all the entities have been created, let's nitialize the wires.
    app_root->initialize_wires();

    // Get the NodeGraphView (QQuickView).
    NodeGraphView* view = app_root->get_node_graph_view();
    app_root->init_view(format);

    if (first_instance) {
      view->report_app_usage();

//      if (NodeGraphView::app_update_is_available()) {
//        QQmlContext* context = view->engine()->rootContext();
//        context->setContextProperty(QStringLiteral("quick_view"), view);
//        // Show our splash page with the option to update the app.
//        view->setSource(QUrl(QStringLiteral("qrc:/qml/youmacro/fullpages/SplashPageWithUpdate.qml")));
//        view->rootObject()->setProperty("ngs_version", NGS_VERSION);
//        view->rootObject()->setProperty("app_name", "YouMacro");
//        view->show();
//        view->update();
//
//        // Run the splash screen event loop.
//        execReturn = app.exec();
//
//        // If we're not updating, then start the app.
//        if (!view->update_is_starting()) {
//          {
//            // Expose some object to qml before loading our main app.
//            app_root->expose_to_qml();
//            // Show our main app.
//            view->setSource(QUrl(QStringLiteral("qrc:/qml/youmacro.qml")));
//            // Embed the node graph quick item into the node graph page.
//            app_root->embed_node_graph();
//          }
//          // Run the main event loop.
//          execReturn = app.exec();
//        }
//      } else

      {
        // Show our splash page, while loading the real qml app.
        view->setSource(QUrl(QStringLiteral("qrc:/qml/youmacro/fullpages/SplashPage.qml")));
        view->rootObject()->setProperty("ngs_version", NGS_VERSION);
        view->rootObject()->setProperty("app_name", "YouMacro");
        view->show();
        view->update();

        // Make the splash screen show up.
        app.processEvents();
        {
          // Expose some object to qml before loading our main app.
          app_root->expose_to_qml();
          // Show our main app.
          view->setSource(QUrl(QStringLiteral("qrc:/qml/youmacro.qml")));
          // Embed the node graph quick item into the node graph page.
          app_root->embed_node_graph();
        }
        // Run the main event loop.
        execReturn = app.exec();
      }
    } else {
      // Show page with warning about multiple instances.
      view->setSource(QUrl(QStringLiteral("qrc:/qml/youmacro/fullpages/AlreadyRunningPage.qml")));
      view->rootObject()->setProperty("ngs_version", NGS_VERSION);
      view->show();
      view->update();
      // Run the Qt loop.
      execReturn = app.exec();
    }

    // test_json_utils();

    // Cleanup.
    delete_ff(app_root);
  }

  // Shutdown.
  shutdown_memory_tracker();
  return execReturn;
}

