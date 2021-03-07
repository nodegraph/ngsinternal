#pragma once
#include <guicomponents/comms/comms_export.h>
#include <guicomponents/comms/cryptologic.h>
#include <guicomponents/comms/messagetypes.h>
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

  COMPONENT_ID(LicenseChecker, LicenseChecker)

  static const QString kLicenseFile;
  static const long long num_trial_days = 15;

  explicit LicenseChecker(Entity* parent);
  virtual ~LicenseChecker();

  // Properties that can be bound in QML.
  Q_PROPERTY(bool has_valid_pro_license READ has_valid_pro_license NOTIFY has_valid_pro_license_changed)

  // Serialization.
  Q_INVOKABLE void save() const;
  Q_INVOKABLE void load();

  // Trigger the license check cycle of method calls/callbacks.
  Q_INVOKABLE void set_lite_edition();
  Q_INVOKABLE void set_pro_edition();
  Q_INVOKABLE void set_license(const QString& license);
  Q_INVOKABLE LicenseEdition get_edition() const;
  Q_INVOKABLE QString get_license() const;

  Q_INVOKABLE void check_license();
  Q_INVOKABLE bool license_is_valid() {return _license_is_valid;}
  Q_INVOKABLE bool has_valid_pro_license() {return _license_is_valid && edition_is_pro();}
  Q_INVOKABLE bool edition_is_pro() {return (_edition == LicenseEdition::kPro);}

 signals:
  void license_checked(bool); // emitted when we determine we have a valid license.
  void has_valid_pro_license_changed();

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
  LicenseEdition _edition;
  std::string _license;
  bool _license_is_valid;
};

}
