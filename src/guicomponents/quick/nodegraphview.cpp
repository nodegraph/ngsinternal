#include <guicomponents/quick/nodegraphview.h>
#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/appconfig.h>

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
      Component(entity, kIID(), kDID()) {

  // Set the app icon.
  QIcon icon(":images/octopus_blue.png");
  setIcon(icon);
  setTitle(QGuiApplication::applicationName());
}

NodeGraphView::~NodeGraphView() {
}

QSGTexture* NodeGraphView::create_texture_from_id(uint id, const QSize &size, CreateTextureOptions options) const {
  return createTextureFromId(id, size, options);
}

bool NodeGraphView::app_update_is_available() {
  QString dir = AppConfig::get_app_bin_dir();
  dir += "\\..";
  QString program = dir + "\\maintenancetool.exe";

  QProcess process;
  process.setWorkingDirectory(dir);
  process.setProgram(program);
  QStringList args;
  args.push_back("--checkupdates");
  process.setArguments(args);
  process.start();
  process.waitForFinished();
  if (process.error() != QProcess::UnknownError) {
    std::cerr << "app update query error: " << process.error() << "\n";
    return false;
  }

  QByteArray data = process.readAllStandardOutput();
  std::string output = data.toStdString();
  //std::cerr << "got back data: " << output << "\n";

  const size_t pos1 = output.find("<updates>");
  const size_t pos2 = output.find("</updates>");
  if ((pos1 == std::string::npos) || (pos2 == std::string::npos)) {
    return false;
  }
  return true;
}

void NodeGraphView::start_app_update() {
  QString dir = AppConfig::get_app_bin_dir();
  dir += "\\..";

  QStringList args("--updater");
  bool success = QProcess::startDetached(dir+"\\maintenancetool.exe", args);
  qApp->quit();
}

}
