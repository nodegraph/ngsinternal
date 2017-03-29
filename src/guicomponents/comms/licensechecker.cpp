#include <base/memoryallocator/taggednew.h>
#include <guicomponents/comms/licensechecker.h>
#include <guicomponents/comms/messagetypes.h>
#include <base/objectmodel/deploader.h>

#include <cstddef>
#include <cassert>

#include <openssl/aes.h>

#include <QtCore/QDebug>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QCoreApplication>
#include <QtWebSockets/QWebSocket>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSsl>

#include <iostream>
#include <sstream>

namespace ngs {

//This is the routine to check the license in javascript.
/*
function check_license(edition, license) {
  var product_id
  if (edition == "pro") {
    product_id = "tgctL"
  } else {
    product_id = "QbCCX"
  }
  request({
    url: 'https://api.gumroad.com/v2/licenses/verify',
    qs: {product_permalink: product_id, license_key: license},
    method: 'POST'
  }, function(error, response, body) {
    if(error) {
      send_to_app({response: false, value: "invalid_license"})
    } else {
      //console.log(response.statusCode, body);
      var obj = JSON.parse(body)
      if ((obj.success == true) && (obj.purchase.refunded == false) && (obj.purchase.chargebacked == false)) {
        send_to_app({response: true, value: "valid_license"})
      } else {
        send_to_app({response: false, value: "invalid_license"})
      }
    }
  });
}
*/

const QString LicenseChecker::kLicenseFile = "license.dat"; // Stores info about our license.

LicenseChecker::LicenseChecker(Entity *parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _crypto_logic(this),
      _network_manager(new_ff QNetworkAccessManager(this)),
      _date_regex("(\\d\\d\\d\\d)-(\\d\\d)-(\\d\\d)"),
      _edition(LicenseEdition::kLite),
      _license_is_valid(false) {
  get_dep_loader()->register_fixed_dep(_crypto_logic, Path());

  assert(QSslSocket::supportsSsl());
  connect(_network_manager, SIGNAL(finished(QNetworkReply*)), SLOT(on_reply_from_web(QNetworkReply*)));
  //check_license("lite", "3AA4B57A-B8EC4445-B0D2437B-11568F59");

#ifdef SKIP_LICENSE_CHECK
  _edition = LicenseEdition::kLite;
  _license = "bogus";
  _license_is_valid = true;
#endif
}

LicenseChecker::~LicenseChecker() {
  delete_ff(_network_manager);
}

void LicenseChecker::save() const{
  external();
  // Save out the crypto config.
  {
    std::stringstream ss;
    SimpleSaver saver(ss);
    int e = to_underlying(_edition);
    saver.save(e);
    saver.save(_license);
    _crypto_logic->write_file(kLicenseFile, ss.str());
  }
}

void LicenseChecker::load() {
  external();
  if (!_crypto_logic->file_exists(kLicenseFile)) {
    _edition = LicenseEdition::kLite;
    _license = "unspecified";
    _license_is_valid = true;
    return;
  }

  // Load the crypto file.
  QByteArray contents = _crypto_logic->load_file(kLicenseFile);
  Bits* bits = create_bits_from_raw(contents.data(), contents.size());
  SimpleLoader loader(bits);

  // Extract the nonce and salt.
  int e;
  loader.load(e);
  _edition = static_cast<LicenseEdition>(e);
  loader.load(_license);
  _license_is_valid = false;
}

void LicenseChecker::set_lite_edition() {
  _edition = LicenseEdition::kLite;
}

void LicenseChecker::set_pro_edition() {
  _edition = LicenseEdition::kPro;
}

void LicenseChecker::set_license(const QString& license) {
  _license = license.toStdString();
}

LicenseEdition LicenseChecker::get_edition() const {
  return _edition;
}

QString LicenseChecker::get_license() const {
  return _license.c_str();
}

#ifdef SKIP_LICENSE_CHECK

void LicenseChecker::check_license() {
  // Note any edition and license will be ok.
  _license_is_valid = true;
  emit license_checked(true);
  emit has_valid_pro_license_changed();
}

void LicenseChecker::on_reply_from_web(QNetworkReply* reply) {
}

#else

void LicenseChecker::check_license() {
  // Note use QCoreApplication::applicationName to determine which app we're checking the license for.

  if (_edition == LicenseEdition::kLite) {
    _license_is_valid = true;
    emit license_checked(true);
    emit has_valid_pro_license_changed();
    return;
  }

  // The logic below pertains to the pro license.

  // Create our request.
  QNetworkRequest request(QUrl("https://api.gumroad.com/v2/licenses/verify"));
  QSslConfiguration config = QSslConfiguration::defaultConfiguration();
  config.setProtocol(QSsl::AnyProtocol);
  request.setSslConfiguration(config);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  // Determine product id.
  QString product_id;

#if (ARCH == ARCH_WINDOWS)
  if (QCoreApplication::applicationName() == "SmashBrowse") {
    product_id = "tgctL";
  }
#elif (ARCH == ARCH_MACOS)
  if (QCoreApplication::applicationName() == "SmashBrowse") {
    product_id = "RMzG";
  }
#endif

  // Generate query object in json.
  QString query = "{\"product_permalink\": \"" + product_id + "\", \"license_key\": \"" + _license.c_str() + "\"}";

  // Post query.
  QNetworkReply *reply = _network_manager->post(request, query.toUtf8());
  reply->ignoreSslErrors();
}

void LicenseChecker::on_reply_from_web(QNetworkReply* reply) {
  if (reply->error() > 0) {
    //std::cerr << "license check error: " << reply->errorString().toStdString() << "\n";
    emit license_checked(false);
    emit has_valid_pro_license_changed();
    return;
  }

  // Construct an object from the json string.
  QByteArray bytes = reply->readAll();
  QJsonDocument doc = QJsonDocument::fromJson(bytes);
  QJsonObject obj = doc.object();

  // Check the success property.
  if (obj["success"] == QJsonValue::Undefined || !obj["success"].toBool()) {
    emit license_checked(false);
    emit has_valid_pro_license_changed();
    return;
  }

  // Grab the purchase object.
  if (obj["purchase"] == QJsonValue::Undefined) {
    emit license_checked(false);
    emit has_valid_pro_license_changed();
    return;
  }
  QJsonObject purchase = obj["purchase"].toObject();

  // Check the refunded property.
  if (purchase["refunded"] == QJsonValue::Undefined || purchase["refunded"].toBool()) {
    emit license_checked(false);
    emit has_valid_pro_license_changed();
    return;
  }

  // Check the chargebacked property.
  if (purchase["chargebacked"] == QJsonValue::Undefined || purchase["chargebacked"].toBool()) {
    emit license_checked(false);
    emit has_valid_pro_license_changed();
    return;
  }

  // Check if the trial/lite software has expired.
  if (_edition == LicenseEdition::kLite) {
    if (purchase["created_at"] == QJsonValue::Undefined) {
      emit license_checked(false);
      emit has_valid_pro_license_changed();
      return;
    }
    int pos = _date_regex.indexIn(purchase["created_at"].toString());
    if (pos >= 0) {
      QStringList list = _date_regex.capturedTexts();
      assert(list.size() == 4);
      // list[0] contains the full match. The other elements contain the submatches.
      int year = list[1].toInt();
      int month = list[2].toInt();
      int day = list[3].toInt();
      QDate install_date(year,month,day);
      QDate current_date = QDate::currentDate();
      qint64 num_days_used = current_date.toJulianDay() - install_date.toJulianDay();
      if (num_days_used > num_trial_days) {
        emit license_checked(false);
        emit has_valid_pro_license_changed();
        return;
      }
    }
  }

  // Cache the valid license state.
  _license_is_valid = true;
  emit license_checked(true);
  emit has_valid_pro_license_changed();
}
#endif

}
