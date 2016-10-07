#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>
#include <QtCore/QObject>

namespace ngs {

class AppComm;
class FileModel;
class GraphBuilder;
class Compute;

// This class communicates with the nodejs process.
class COMMS_EXPORT AppConfig : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(AppConfig, AppConfig)

  explicit AppConfig(Entity* parent);
  virtual ~AppConfig();

  // Paths to resources.
  static QString get_app_bin_dir();
  static QString get_user_data_dir();
  Q_INVOKABLE static QString get_smash_browse_url();

};

}
