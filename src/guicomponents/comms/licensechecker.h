#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>

#include <QtCore/QObject>

class QNetworkAccessManager;
class QNetworkReply;

namespace ngs {

class COMMS_EXPORT LicenseChecker : public QObject, public Component{
Q_OBJECT
 public:

  COMPONENT_ID(LicenseChecker, LicenseChecker)

  explicit LicenseChecker(Entity* parent);
  virtual ~LicenseChecker();

  // Trigger the license check cycle of method calls/callbacks.
  Q_INVOKABLE void check_license(const QString& edition, const QString& license);

 signals:
  void valid_license(); // emitted when we determine we have a valid license.
  void invalid_license(); // emitted when we determine we have an invalid license.

 private slots:
  void on_web_reply(QNetworkReply* reply);

 private:
  QNetworkAccessManager *_network_manager;
};

}
