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

LicenseChecker::LicenseChecker(Entity *parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _network_manager(new_ff QNetworkAccessManager(this)){
  assert(QSslSocket::supportsSsl());
  connect(_network_manager, SIGNAL(finished(QNetworkReply*)), SLOT(on_license_check_reply(QNetworkReply*)));

  //check_license("lite", "3AA4B57A-B8EC4445-B0D2437B-11568F59");
}

LicenseChecker::~LicenseChecker() {
  delete_ff(_network_manager);
}


void LicenseChecker::check_license(const QString& edition, const QString& license) {
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

void LicenseChecker::on_web_reply(QNetworkReply* reply) {
  if (reply->error() > 0) {
    std::cerr << "license check error: " << reply->errorString().toStdString() << "\n";
    emit invalid_license();
    return;
  }



  // Construct an object from the json string.
  QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();

  // Check the success property.
  if (obj["success"] == QJsonValue::Undefined || !obj["success"].toBool()) {
    emit invalid_license();
    return;
  }

  // Grab the purchase object.
  if (obj["purchase"] == QJsonValue::Undefined) {
    emit invalid_license();
    return;
  }
  QJsonObject purchase = obj["purchase"].toObject();

  // Check the refunded property.
  if (purchase["refunded"] == QJsonValue::Undefined || purchase["refunded"].toBool()) {
    emit invalid_license();
    return;
  }

  // Check the chargebacked property.
  if (purchase["chargebacked"] == QJsonValue::Undefined || purchase["chargebacked"].toBool()) {
    emit invalid_license();
    return;
  }

  std::cerr << "license check success: " << reply->readAll().toStdString() << "\n";
  emit valid_license();
}

}
