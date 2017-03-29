#include <guicomponents/quick/nodegraphview.h>
#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/appconfig.h>
#include <entities/guientities.h>

#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/comms/filemodel.h>

#include <QtGui/QGuiApplication>
#include <QtQml/QtQml>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickItem>
#include <QtGui/QScreen>

namespace ngs {

// Using the QQmlApplicationEngine would of allowed us to declare a ApplicationWindow on the QML side.
// This would have been nicer but, it doesn't allow us to set the opengl context and version settings.
// Hence we use a QQuickView as the ApplicationWindow and then load QML components into that.

NodeGraphView::NodeGraphView(Entity* entity)
    : QQuickView(NULL),
      Component(entity, kIID(), kDID()),
      _update_is_starting(false) {

  // Set the app icon.
  QIcon icon(":images/robot_blue.png");
  setIcon(icon);
  setTitle(QGuiApplication::applicationName());
}

NodeGraphView::~NodeGraphView() {
}

QSGTexture* NodeGraphView::create_texture_from_id(uint id, const QSize &size, CreateTextureOptions options) const {
  return createTextureFromId(id, size, options);
}

void get_dir_and_program(QString& dir, QString& program) {
  dir = AppConfig::get_app_bin_dir();

#if (ARCH == ARCH_WINDOWS)
  // We're inside the bin dir.
  dir += "/..";
  program = dir + "/maintenancetool.exe";
#elif (ARCH == ARCH_MACOS)
  // We're inside the .app/Contents/MacOS dir.
  dir += "/../../..";
  program = dir + "/maintenancetool.app/Contents/MacOS/maintenancetool";
#endif

  std::cerr << "app bin dir is: "  <<  dir.toStdString() << "\n";
  std::cerr << "app program is: " << program.toStdString() << "\n";
}

bool NodeGraphView::app_update_is_available() {
  QString dir;
  QString program;
  get_dir_and_program(dir, program);

  QProcess process;
  process.setWorkingDirectory(dir);
  process.setProgram(program);
  QStringList args;
  args.push_back("--checkupdates");
  process.setArguments(args);

  std::cerr << "checking for app updates\n";
  process.start();
  process.waitForFinished();
  if (process.error() != QProcess::UnknownError) {
    std::cerr << "checking for app updates error: " << process.error() << "\n";
    return false;
  }

  QByteArray data = process.readAllStandardOutput();
  std::string output = data.toStdString();
  std::cerr << "app update data: " << output << "\n";

  const size_t pos1 = output.find("<updates>");
  const size_t pos2 = output.find("</updates>");
  if ((pos1 == std::string::npos) || (pos2 == std::string::npos)) {
    return false;
  }
  return true;
}

void NodeGraphView::close_splash_page() {
  qApp->quit();
}

void NodeGraphView::start_app_update() {
  _update_is_starting = true;

  QString dir;
  QString program;
  get_dir_and_program(dir, program);

  QStringList args("--updater");
  bool success = QProcess::startDetached(program, args);
  qApp->exit();
}

bool NodeGraphView::update_is_starting() {
  return _update_is_starting;
}



}
