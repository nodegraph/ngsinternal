#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <guicomponents/comms/appconfig.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/comms/messagesender.h>
#include <guicomponents/comms/nodejsprocess.h>

#include <cstddef>
#include <cassert>

#include <openssl/aes.h>

#include <QtWebSockets/QWebSocket>
#include <QtCore/QCoreApplication>

namespace ngs {

MessageSender::MessageSender(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _process(this),
      _web_socket(NULL),
      _trying_to_open(false) {
  get_dep_loader()->register_fixed_dep(_process, Path({}));

  _web_socket  = new_ff QWebSocket();
  connect(_web_socket, SIGNAL(connected()), this, SLOT(on_connected()));
  connect(_web_socket, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
  connect(_web_socket, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(on_ssl_error(const QList<QSslError>&)));
  connect(_web_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_error(QAbstractSocket::SocketError)));
  connect(_web_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(on_state_changed(QAbstractSocket::SocketState)));
}

MessageSender::~MessageSender() {
  close();
  delete_ff(_web_socket);
}

QWebSocket* MessageSender::get_web_socket() const {
  external();
  return _web_socket;
}

void MessageSender::send_msg(const Message& msg) const {
  external();
  size_t num_bytes = _web_socket->sendTextMessage(msg.to_string());
  if(num_bytes == 0) {
    std::cerr << "Error: Unable to send msg from app to commhub. The commhub process may have terminated.\n";
  }
}

void MessageSender::on_connected() {
  internal();
  _trying_to_open = false;
  //qDebug() << "message sender is now connected";
}

void MessageSender::on_disconnected() {
  internal();
  _trying_to_open = false;
  //qDebug() << "message sender is now disconnected";
}

void MessageSender::on_error(QAbstractSocket::SocketError error) {
  internal();
  if (error == QAbstractSocket::SocketError::SslHandshakeFailedError) {
    // We expect this error because we're using a local ssl server.
    return;
  } else if (error == QAbstractSocket::RemoteHostClosedError) {
  }
  qDebug() << "MessageSender Error: " << error;
  qDebug() << "WebSocket error string: " << _web_socket->errorString();
}

void MessageSender::on_ssl_error(const QList<QSslError>& errors) {
  internal();
  //qDebug() << "SSLError: " << errors;
  _web_socket->ignoreSslErrors(errors);
}

void MessageSender::on_state_changed(QAbstractSocket::SocketState s) {
  internal();
  //qDebug() << "state changed: " << s;
}

void MessageSender::close() {
  external();

  _web_socket->close();

  // Make sure the nodejs process is stopped as well.
  if (_process && _process->is_running()) {
    _process->stop_process();
  }
}

void MessageSender::open() {
  external();

  _trying_to_open = true;

  // Make sure the nodejs process has started.
  if (!_process->is_running()) {
    _process->start_process();
  }

  const QString& nodejs_port = _process->get_nodejs_port();

  // Form the websocket server's url using the port number.
  QString url("wss://localhost:");
  url += nodejs_port;

  // Do we have ssl support?
  //std::cerr << "ssl support: " << QSslSocket::supportsSsl() << "\n";

  // Trying to ignore the ssl error doesn't seem to work..
//  QList<QSslCertificate> cert = QSslCertificate::fromPath(AppConfig::get_app_bin_dir()+"/cert.pem");
//  std::cerr << "num certs: " << cert.count() << "\n";
//  QSslError error(QSslError::SelfSignedCertificate, cert.at(0));
//  QList<QSslError> expectedSslErrors;
//  expectedSslErrors.append(error);
//  _web_socket->ignoreSslErrors(expectedSslErrors);

  // The first attempt will fail but will gather the ssl errror to ignore.
  _web_socket->open(QUrl(url));

  // Wait till it's open.
  // However this is should success in opening on the second open attempt.
  // The first attempt is above which is known to fail.
  while (!is_open()) {
    // We wait processing events until the socket connection is complete.
    qApp->processEvents();

    // After processing event we may now be open.
    if (is_open()) {
      break;
    }

    // Otherwise if we're not in the process of opening, try to open the socket again.
    if (!_trying_to_open) {
      _trying_to_open = true;
      _web_socket->open(QUrl(url));
    }
  }
}

bool MessageSender::is_open() const{
  external();
  if (_web_socket->isValid()) {
    return true;
  }
  return false;
}

}
