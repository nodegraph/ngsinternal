#include <base/objectmodel/appconfig.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QGuiApplication>
#include <QtCore/QStandardPaths>

namespace ngs {

const QString AppConfig::kAppDataDir = "appdata";
const QString AppConfig::kAppMacrosDir = "appmacros";
const QString AppConfig::kPublicMacrosDir = "publicmacros";
const QString AppConfig::kPrivateMacrosDir = "privatemacros";

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
  // Make sure the public macros dir exists.
  {
    QString dir = get_public_macros_dir();
    QFileInfo info(dir);
    if (!info.exists()) {
      QDir().mkpath(dir);
    }
  }
  // Make sure the private macros dir exists.
  {
    QString dir = get_public_macros_dir();
    QFileInfo info(dir);
    if (!info.exists()) {
      QDir().mkpath(dir);
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
  QString dir = get_app_bin_dir();
#if (ARCH == ARCH_WINDOWS)
  dir += "/../" + kAppMacrosDir;
#elif (ARCH == ARCH_MACOS)
  dir += "/../Resources/" + kAppMacrosDir;
#endif
  return dir;
}

QString AppConfig::get_public_macros_dir() {
  return get_user_app_dir() + "/" + kPublicMacrosDir;
}

QString AppConfig::get_private_macros_dir() {
  return get_user_app_dir() + "/" + kPrivateMacrosDir;
}

QString AppConfig::get_fallback_download_dir() {
  return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + QDir::separator() + QCoreApplication::applicationName();
}

QString AppConfig::get_smash_browse_url() {
  QString app_dir = get_app_bin_dir();
  app_dir += QString("/../html/robodownloader.html");
  return app_dir;
}

}
