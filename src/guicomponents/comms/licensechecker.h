#pragma once
#include <guicomponents/comms/comms_export.h>
#include <guicomponents/comms/cryptologic.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/componentids.h>
#include <base/objectmodel/dep.h>

#include <QtCore/QObject>

class QNetworkAccessManager;
class QNetworkReply;

namespace ngs {

class CryptoLogic;

class COMMS_EXPORT LicenseChecker : public QObject, public Component{
Q_OBJECT
 public:

  static const QString kLicenseFile;
  static const long long num_trial_days = 15;

  COMPONENT_ID(LicenseChecker, LicenseChecker)

  explicit LicenseChecker(Entity* parent);
  virtual ~LicenseChecker();

  Q_INVOKABLE void save() const;
  Q_INVOKABLE void load();


  // Trigger the license check cycle of method calls/callbacks.
  Q_INVOKABLE void set_edition(const QString& edition);
  Q_INVOKABLE void set_license(const QString& license);
  Q_INVOKABLE QString get_edition() const;
  Q_INVOKABLE QString get_license() const;

  Q_INVOKABLE void check_license();
  Q_INVOKABLE bool license_is_valid() {return _license_is_valid;}

 signals:
  void license_checked(bool); // emitted when we determine we have a valid license.

 private slots:
  void on_reply_from_web(QNetworkReply* reply);

 private:
  // Our dependencies.
  Dep<CryptoLogic> _crypto_logic;

  // Use to make http request to gumroad.
  QNetworkAccessManager *_network_manager;

  // Used when parsing response from gumroad.
  const QRegExp _date_regex;

  // Our licensing data.
  std::string _edition;
  std::string _license;
  bool _license_is_valid;
};

}
