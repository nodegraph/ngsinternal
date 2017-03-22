#pragma once
#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/componentids.h>

#include <QtCore/QString>

namespace ngs {

class OBJECTMODEL_EXPORT AppConfig : public Component {
 public:

  static const QString kAppDataDir;
  static const QString kAppMacrosDir;
  static const QString kPublicMacrosDir;
  static const QString kPrivateMacrosDir;

  COMPONENT_ID(AppConfig, AppConfig)

  explicit AppConfig(Entity* parent);
  virtual ~AppConfig();

  // Paths to resources.
  static QString get_app_bin_dir(); // The binary directory of the app, usually inside system folder on windows.
  static QString get_user_app_dir(); // The main user directory on the os.
  static QString get_app_data_dir(); // The app data dir inside the user dir.
  static QString get_app_macros_dir(); // The macros dir inside of the app data dir.
  static QString get_public_macros_dir(); // The public macros dir inside of the app data dir.
  static QString get_private_macros_dir(); // The public macros dir inside of the app data dir.
  static QString get_fallback_download_dir();

  static QString get_smash_browse_url();

};

}
