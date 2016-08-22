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
  void license_checked(bool); // emitted when we determine we have a valid license.

 private slots:
  void on_reply_from_web(QNetworkReply* reply);

 private:
  QNetworkAccessManager *_network_manager;
};

}
