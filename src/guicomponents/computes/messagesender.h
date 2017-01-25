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

namespace ngs {

class AcceptSaveProcess;
class JavaProcess;
class Message;

// This class communicates with the nodejs process.
class GUICOMPUTES_EXPORT MessageSender : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(MessageSender, MessageSender)

  explicit MessageSender(Entity* parent);
  virtual ~MessageSender();

  void open();
  void close();
  void wait_for_chrome_connection();

  // Note the sending and receiving of messages of QWebSocket has been split into
  // two parts to keep the dependencies cleanly separated, without hacks.
  void send_msg(const Message& msg) const;
  void accept_save_dialog(int msg_id) const;

 private slots:
  void on_new_connection();
  //void on_connected();
  //void on_disconnected();
  //void on_error(QAbstractSocket::SocketError error);
  void on_ssl_errors(const QList<QSslError>& errors);
  void on_server_error(QWebSocketProtocol::CloseCode closeCode);
  //void on_state_changed(QAbstractSocket::SocketState);

 public:
  QWebSocket* get_web_socket() const;
  void clear_web_socket();
  QWebSocketServer* get_web_socket_server() const; // The MessageSender will use this to grab the web socket.

 private:

  // Our fixed dependencies.
  Dep<AcceptSaveProcess> _accept_save_process;
  Dep<JavaProcess> _java_process;

  int _port;
  mutable QWebSocketServer* _server; // The main web socket server.
  QWebSocket* _client; // We only expect one client to connect to the server.
  bool _trying_to_open;

  friend class Grabber; // This is temporary component that needs access to get_web_socket().
};

}

