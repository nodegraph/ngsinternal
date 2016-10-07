#include <guicomponents/comms/appconfig.h>

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QGuiApplication>
#include <QtCore/QStandardPaths>

namespace ngs {

AppConfig::AppConfig(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()) {}

AppConfig::~AppConfig() {}

QString AppConfig::get_app_bin_dir() {
  return QCoreApplication::applicationDirPath();
}

QString AppConfig::get_user_data_dir() {
  return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString AppConfig::get_smash_browse_url() {
  QString app_dir = get_app_bin_dir();
  app_dir += QString("/../html/smashbrowse.html");
  return app_dir;
}

}
