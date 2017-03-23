#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/componentids.h>
#include <base/objectmodel/dep.h>

#include <QtCore/QProcess>
#include <QtCore/QObject>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslError>
#include <QtWebSockets/qwebsocketprotocol.h>

class QWebSocket;
class QWebSocketServer;
class QWebSocketCorsAuthenticator;
class QSslPreSharedKeyAuthenticator;

namespace ngs {

class DownloadManager;
class AcceptSaveProcess;
class JavaProcess;
class Message;

class GUICOMPUTES_EXPORT MessageSender : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(MessageSender, MessageSender)

  static const size_t kMaxMsgSizeToOutput;

  explicit MessageSender(Entity* parent);
  virtual ~MessageSender();

  void open();
  void close();
  void wait_for_chrome_connection();

  void send_msg(const Message& msg) const;

 private slots:
  void on_accept_error(QAbstractSocket::SocketError);
  void on_closed();
  void on_auth(QWebSocketCorsAuthenticator * auth);
  void on_peer_verify_error(const QSslError &error);
  void on_auth2(QSslPreSharedKeyAuthenticator *authenticator);
  void on_new_connection();
  void on_ssl_errors(const QList<QSslError>& errors);
  void on_server_error(QWebSocketProtocol::CloseCode closeCode);

 public:
  QWebSocket* get_web_socket() const;
  void clear_web_socket();
  QWebSocketServer* get_web_socket_server() const; // The MessageSender will use this to grab the web socket.

 private:

  // Our fixed dependencies.
  Dep<AcceptSaveProcess> _accept_save_process;
  Dep<JavaProcess> _java_process;
  Dep<DownloadManager> _download_manager;

  int _server_port;
  mutable QWebSocketServer* _server; // The main web socket server.
  QWebSocket* _client; // We only expect one client to connect to the server.
  bool _trying_to_open;

  friend class Grabber; // This is temporary component that needs access to get_web_socket().
};

}

