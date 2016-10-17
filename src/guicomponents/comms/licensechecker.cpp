#include <base/memoryallocator/taggednew.h>
#include <guicomponents/comms/licensechecker.h>

#include <cstddef>
#include <cassert>

#include <openssl/aes.h>

#include <QtCore/QDebug>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtWebSockets/QWebSocket>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSsl>

#include <iostream>

namespace ngs {

//This is the routine to check the license in nodejs/javascript.
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

LicenseChecker::LicenseChecker(Entity *parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _network_manager(new_ff QNetworkAccessManager(this)),
      _license_is_valid(false),
      _date_regex("(\\d\\d\\d\\d)-(\\d\\d)-(\\d\\d)") {
  assert(QSslSocket::supportsSsl());
  connect(_network_manager, SIGNAL(finished(QNetworkReply*)), SLOT(on_reply_from_web(QNetworkReply*)));
  //check_license("lite", "3AA4B57A-B8EC4445-B0D2437B-11568F59");

#ifdef SKIP_LICENSE_CHECK
  _license_is_valid = true;
#endif
}

LicenseChecker::~LicenseChecker() {
  delete_ff(_network_manager);
}

#ifdef SKIP_LICENSE_CHECK

void LicenseChecker::check_license(const QString& edition, const QString& license) {
  _license_is_valid = true;
  emit license_checked(true);
}

void LicenseChecker::on_reply_from_web(QNetworkReply* reply) {
}

#else

void LicenseChecker::check_license(const QString& edition, const QString& license) {
  _check_edition = edition;

  // Create our request.
  QNetworkRequest request(QUrl("https://api.gumroad.com/v2/licenses/verify"));
  QSslConfiguration config = QSslConfiguration::defaultConfiguration();
  config.setProtocol(QSsl::AnyProtocol);
  request.setSslConfiguration(config);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  // Determine product id.
  QString product_id;
  if (edition == "pro") {
    product_id = "tgctL";
  } else {
    product_id = "QbCCX";
  }

  // Generate query object in json.
  QString query = "{\"product_permalink\": \"" + product_id + "\", \"license_key\": \"" + license + "\"}";

  // Post query.
  QNetworkReply *reply = _network_manager->post(request, query.toUtf8());
  reply->ignoreSslErrors();
}

void LicenseChecker::on_reply_from_web(QNetworkReply* reply) {
  if (reply->error() > 0) {
    //std::cerr << "license check error: " << reply->errorString().toStdString() << "\n";
    emit license_checked(false);
    return;
  }

  // Construct an object from the json string.
  QByteArray bytes = reply->readAll();
  QJsonDocument doc = QJsonDocument::fromJson(bytes);
  QJsonObject obj = doc.object();

  // Check the success property.
  if (obj["success"] == QJsonValue::Undefined || !obj["success"].toBool()) {
    emit license_checked(false);
    return;
  }

  // Grab the purchase object.
  if (obj["purchase"] == QJsonValue::Undefined) {
    emit license_checked(false);
    return;
  }
  QJsonObject purchase = obj["purchase"].toObject();

  // Check the refunded property.
  if (purchase["refunded"] == QJsonValue::Undefined || purchase["refunded"].toBool()) {
    emit license_checked(false);
    return;
  }

  // Check the chargebacked property.
  if (purchase["chargebacked"] == QJsonValue::Undefined || purchase["chargebacked"].toBool()) {
    emit license_checked(false);
    return;
  }

  // Check if the trial/lite software has expired.
  if (_check_edition == "lite") {
    if (purchase["created_at"] == QJsonValue::Undefined) {
      emit license_checked(false);
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
        return;
      }
    }
  }

  // Cache the valid license state.
  _license_is_valid = true;
  emit license_checked(true);
}
#endif

}
