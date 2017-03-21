#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <base/objectmodel/appconfig.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/computes/messagesender.h>
#include <guicomponents/computes/acceptsaveprocess.h>
#include <guicomponents/computes/javaprocess.h>
#include <guicomponents/computes/downloadmanager.h>
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
      _accept_save_process(this),
      _java_process(this),
      _download_manager(this),
      _server_port(-1),
      _server(NULL),
      _client(NULL),
      _trying_to_open(false) {
  get_dep_loader()->register_fixed_dep(_accept_save_process, Path());
  get_dep_loader()->register_fixed_dep(_java_process, Path());
  get_dep_loader()->register_fixed_dep(_download_manager, Path());

//#define USE_SSL
  // Using ssl doesn't seem to work on macos. It's not clear whether qt has a bug
  // or whether chrome is not permitting the connection.
  // Note this does work on windows however.
  // For now we keep it disabled for all platforms.

#ifndef USE_SSL
  _server  = new_ff QWebSocketServer(QStringLiteral("Smash Browser Server"), QWebSocketServer::NonSecureMode);
#else
  _server  = new_ff QWebSocketServer(QStringLiteral("Smash Browser Server"), QWebSocketServer::SecureMode);

  assert(QSslSocket::supportsSsl());
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
#endif


  if (_server->listen(QHostAddress::Any, 0)) { // 0 means that Qt will automatically find an open port for us.
    _server_port = _server->serverPort();
    std::cerr << "server listening on port: " << _server_port << "\n";

    connect(_server, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(on_accept_error(QAbstractSocket::SocketError)));
    connect(_server, SIGNAL(closed()), this, SLOT(on_closed()));
    connect(_server, SIGNAL(originAuthenticationRequired(QWebSocketCorsAuthenticator *)), this, SLOT(on_auth(QWebSocketCorsAuthenticator *)));
    connect(_server, SIGNAL(peerVerifyError(const QSslError &)), this, SLOT(on_peer_verify_error(const QSslError &)));
    connect(_server, SIGNAL(preSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator *)), this, SLOT(on_auth2(QSslPreSharedKeyAuthenticator *)));
    connect(_server, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(on_ssl_errors(const QList<QSslError>&)));
    connect(_server, SIGNAL(serverError(QWebSocketProtocol::CloseCode)), this, SLOT(on_server_error(QWebSocketProtocol::CloseCode)));
    connect(_server, SIGNAL(newConnection()), this, SLOT(on_new_connection()));
  }


}

MessageSender::~MessageSender() {
  close();
  delete_ff(_server);
}

void MessageSender::on_accept_error(QAbstractSocket::SocketError error) {
  std::cerr << "MessageSender accept error: " << error << "\n";
}

void MessageSender::on_closed() {
  std::cerr << "MessageSender on closed\n";
}

void MessageSender::on_auth(QWebSocketCorsAuthenticator *) {
  std::cerr << "MessageSender on auth required\n";
}

void MessageSender::on_peer_verify_error(const QSslError &error) {
  std::cerr << "MessageSender peer verify error: " << error.errorString().toStdString() << "\n";
}

void MessageSender::on_auth2(QSslPreSharedKeyAuthenticator *authenticator) {
  std::cerr << "MessageSender on pre shared key auth\n";
}

void MessageSender::on_new_connection() {
  std::cerr<< "got new connection !!!!\n";
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

  std::cerr << "----------------------------------------------------\n";
  std::cerr << "app is sending a message:   ----> \n";
  msg.dump();

  ReceiverType rec = msg.get_receiver_type();
  switch (rec) {
    case ReceiverType::kUnknown: {
      break;
    }
    case ReceiverType::kWebDriver: {
      _java_process->send_msg(msg);
      break;
    }
    case ReceiverType::kChrome: {
      size_t num_bytes = _client->sendTextMessage(msg.to_string());
      if (num_bytes == 0) {
        std::cerr << "Error: Unable to send msg from app to commhub. The commhub process may have terminated.\n";
      }
      break;
    }
    case ReceiverType::kPlatform: {
      if (msg[Message::kRequest].toDouble() == to_underlying(PlatformRequestType::kAcceptSaveDialog)) {
        _accept_save_process->start_process(msg.get_id());
      } else if (msg[Message::kRequest].toDouble() == to_underlying(PlatformRequestType::kDownloadVideo)) {
        //std::cerr << "starting to downloading video: " << msg.to_string().toStdString() << "\n";
        //_download_manager->download(msg.get_id(), msg[Message::kArgs].toObject());
      }
      break;
    }
    case ReceiverType::kFirebase: {
      break;
    }
  }
}

void MessageSender::on_ssl_errors(const QList<QSslError>& errors) {
  internal();
  qDebug() << "MessageSender SSLErrors: " << errors;
}

void MessageSender::on_server_error(QWebSocketProtocol::CloseCode closeCode) {
  internal();
  qDebug() << "MessageSender on server error: " << closeCode;
}

void MessageSender::close() {
  external();
  _server->close();

  // Make sure the java process is stopped as well.
  if (_java_process && _java_process->is_running()) {
    _java_process->stop_process();
  }
}

void MessageSender::open() {
  external();

  _trying_to_open = true;

  std::cerr << "MessageSender is opening for app: " << QCoreApplication::applicationName().toStdString() << "\n";

  // Make sure the java process has started.
  if (!_java_process->is_running() && QCoreApplication::applicationName() == "SmashBrowse") {
    //_process->start_process();
    _java_process->start_process(_server_port);
  }
}

void MessageSender::wait_for_chrome_connection() {
  while(!_client) {
    //std::cerr << "waiting for chrome connection\n";
    qApp->processEvents();
  }
}

}
