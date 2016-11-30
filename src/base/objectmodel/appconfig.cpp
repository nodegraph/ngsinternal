#include <base/objectmodel/appconfig.h>

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QGuiApplication>
#include <QtCore/QStandardPaths>

namespace ngs {

const QString AppConfig::kAppDataDir = "app_data";
const QString AppConfig::kAppMacrosDir = "app_macros";
const QString AppConfig::kUserMacrosDir = "user_macros";

AppConfig::AppConfig(Entity* parent)
    : Component(parent, kIID(), kDID()) {
  // Make sure the app data dir exists.
  {
    QString app_data_dir = get_app_data_dir();
    qDebug() << "app data dir is: " << app_data_dir << "\n";
    QFileInfo info(app_data_dir);
    if (!info.exists()) {
      QDir().mkpath(app_data_dir);
    }
  }
  // Make sure the macros dir exists.
  {
    QString app_macros_dir = get_app_macros_dir();
    QFileInfo info(app_macros_dir);
    if (!info.exists()) {
      QDir().mkpath(app_macros_dir);
    }
  }
  // Make sure the user macros dir exists.
  {
    QString user_macros_dir = get_user_macros_dir();
    QFileInfo info(user_macros_dir);
    if (!info.exists()) {
      QDir().mkpath(user_macros_dir);
    }
  }
}

AppConfig::~AppConfig() {}

QString AppConfig::get_app_bin_dir() {
  return QCoreApplication::applicationDirPath();
}

QString AppConfig::get_user_app_dir() {
  return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString AppConfig::get_app_data_dir() {
  return get_user_app_dir() + "/" + kAppDataDir;
}

QString AppConfig::get_app_macros_dir() {
  return get_user_app_dir() + "/" + kAppMacrosDir;
}

QString AppConfig::get_user_macros_dir() {
  return get_user_app_dir() + "/" + kUserMacrosDir;
}

QString AppConfig::get_smash_browse_url() {
  QString app_dir = get_app_bin_dir();
  app_dir += QString("/../html/smashbrowse.html");
  return app_dir;
}

}
