#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <base/objectmodel/appconfig.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/computes/messagesender.h>
#include <guicomponents/comms/nodejsprocess.h>
#include <guicomponents/computes/acceptsaveprocess.h>
#include <guicomponents/computes/javaprocess.h>
#include <cstddef>
#include <cassert>

#include <openssl/aes.h>

#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>

#include <QtNetwork/QSslKey>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

namespace ngs {

MessageSender::MessageSender(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _process(this),
      _accept_save_process(this),
      _java_process(this),
      _port(8093),
      _server(NULL),
      _client(NULL),
      _trying_to_open(false) {
  get_dep_loader()->register_fixed_dep(_process, Path());
  get_dep_loader()->register_fixed_dep(_accept_save_process, Path());
  get_dep_loader()->register_fixed_dep(_java_process, Path());

  _server  = new_ff QWebSocketServer(QStringLiteral("Smash Browser Server"), QWebSocketServer::SecureMode);
  QSslConfiguration sslConfiguration;
  QFile certFile(QStringLiteral(":/ssl/cert.pem"));
  QFile keyFile(QStringLiteral(":/ssl/key.pem"));
  certFile.open(QIODevice::ReadOnly);
  keyFile.open(QIODevice::ReadOnly);
  QSslCertificate certificate(&certFile, QSsl::Pem);
  QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
  certFile.close();
  keyFile.close();
  sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
  sslConfiguration.setLocalCertificate(certificate);
  sslConfiguration.setPrivateKey(sslKey);
  sslConfiguration.setProtocol(QSsl::TlsV1SslV3);
  _server->setSslConfiguration(sslConfiguration);

  if (_server->listen(QHostAddress::Any, _port))
  {
      qDebug() << "Smash Browse is listening on port " << _port << ".";
      connect(_server, &QWebSocketServer::newConnection, this, &MessageSender::on_new_connection);
      connect(_server, &QWebSocketServer::sslErrors,this, &MessageSender::on_ssl_errors);
  }

//  connect(_server, SIGNAL(connected()), this, SLOT(on_connected()));
//  connect(_server, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
  connect(_server, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(on_ssl_errors(const QList<QSslError>&)));
  connect(_server, SIGNAL(serverError(QWebSocketProtocol::CloseCode)), this, SLOT(on_server_error(QWebSocketProtocol::CloseCode)));

//  connect(_server, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_error(QAbstractSocket::SocketError)));
//  connect(_server, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(on_state_changed(QAbstractSocket::SocketState)));
}

MessageSender::~MessageSender() {
  close();
  delete_ff(_server);
}

void MessageSender::on_new_connection() {
  // We're only expecting one connection.
  if (_client) {
    return;
  }
  _client = _server->nextPendingConnection();
  our_entity()->clean_wires();
}

QWebSocket* MessageSender::get_web_socket() const {
  external();
  return _client;
}

void MessageSender::clear_web_socket() {
  if (_client) {
    _client->close();
    // Client will be deleted in a callback in MessageReceiver.
  }
  _client = NULL;
}

QWebSocketServer* MessageSender::get_web_socket_server() const {
  external();
  return _server;
}

void MessageSender::send_msg(const Message& msg) const {
  external();
  Message::ReceiverType rec = msg.get_receiver_type();
  switch (rec) {
    case Message::ReceiverType::Unknown: {
      break;
    }
    case Message::ReceiverType::WebDriver: {
      _java_process->send_msg(msg);
      break;
    }
    case Message::ReceiverType::Chrome: {
      size_t num_bytes = _client->sendTextMessage(msg.to_string());
      if (num_bytes == 0) {
        std::cerr << "Error: Unable to send msg from app to commhub. The commhub process may have terminated.\n";
      }
      break;
    }
    case Message::ReceiverType::Platform: {
      if (msg[Message::kRequest].toDouble() == to_underlying(PlatformRequestType::kAcceptSaveDialog)) {
        accept_save_dialog(msg.get_id());
      }
      break;
    }
    case Message::ReceiverType::Firebase: {
      break;
    }
  }



}

void MessageSender::accept_save_dialog(int msg_id) const {
  external();
  _accept_save_process->set_msg_id(msg_id);
  _accept_save_process->start_process();
}

//void MessageSender::on_connected() {
//  internal();
//  _trying_to_open = false;
//  //qDebug() << "message sender is now connected";
//}
//
//void MessageSender::on_disconnected() {
//  internal();
//  _trying_to_open = false;
//  //qDebug() << "message sender is now disconnected";
//}

//void MessageSender::on_error(QAbstractSocket::SocketError error) {
//  internal();
//  if (error == QAbstractSocket::SocketError::SslHandshakeFailedError) {
//    // We expect this error because we're using a local ssl server.
//    return;
//  } else if (error == QAbstractSocket::RemoteHostClosedError) {
//  }
//  qDebug() << "MessageSender Error: " << error;
//  qDebug() << "WebSocket error string: " << _server->errorString();
//}

void MessageSender::on_ssl_errors(const QList<QSslError>& errors) {
  internal();
  qDebug() << "SSLErrors: " << errors;
  //_server->ignoreSslErrors(errors);
}

void MessageSender::on_server_error(QWebSocketProtocol::CloseCode closeCode) {
  internal();
  qDebug() << "on server error: " << closeCode;
}

//void MessageSender::on_state_changed(QAbstractSocket::SocketState s) {
//  internal();
//  //qDebug() << "state changed: " << s;
//}

void MessageSender::close() {
  external();

  _server->close();

  // Make sure the nodejs process is stopped as well.
  if (_process && _process->is_running()) {
    _process->stop_process();
  }

  // Make sure the nodejs process is stopped as well.
  if (_java_process && _java_process->is_running()) {
    _java_process->stop_process();
  }
}

void MessageSender::open() {
  external();

  _trying_to_open = true;

  // Make sure the nodejs process has started.
  if (!_process->is_running()) {
    _process->start_process();
    _java_process->start_process();
  }

//  const QString& nodejs_port = _process->get_nodejs_port();
//
//  // Form the websocket server's url using the port number.
//  QString url("wss://localhost:");
//  url += nodejs_port;
//
//  // Do we have ssl support?
//  //std::cerr << "ssl support: " << QSslSocket::supportsSsl() << "\n";
//
//  // Trying to ignore the ssl error doesn't seem to work..
////  QList<QSslCertificate> cert = QSslCertificate::fromPath(AppConfig::get_app_bin_dir()+"/cert.pem");
////  std::cerr << "num certs: " << cert.count() << "\n";
////  QSslError error(QSslError::SelfSignedCertificate, cert.at(0));
////  QList<QSslError> expectedSslErrors;
////  expectedSslErrors.append(error);
////  _web_socket->ignoreSslErrors(expectedSslErrors);
//
//  // The first attempt will fail but will gather the ssl errror to ignore.
//  _server->open(QUrl(url));
//
//  // Wait till it's open.
//  // However this is should success in opening on the second open attempt.
//  // The first attempt is above which is known to fail.
//  while (!is_open()) {
//    // We wait processing events until the socket connection is complete.
//    qApp->processEvents();
//
//    // After processing event we may now be open.
//    if (is_open()) {
//      break;
//    }
//
//    // Otherwise if we're not in the process of opening, try to open the socket again.
//    if (!_trying_to_open) {
//      _trying_to_open = true;
//      _server->open(QUrl(url));
//    }
//  }
}

//bool MessageSender::is_open() const{
//  external();
//  if (_server->isValid()) {
//    return true;
//  }
//  return false;
//}

}
