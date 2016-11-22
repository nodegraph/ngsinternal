#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/componentids.h>
#include <base/objectmodel/dep.h>

#include <QtCore/QProcess>
#include <QtCore/QObject>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslError>

class QWebSocket;

namespace ngs {

class NodeJSProcess;
class Message;

// This class communicates with the nodejs process.
class COMMS_EXPORT MessageSender : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(MessageSender, MessageSender)

  explicit MessageSender(Entity* parent);
  virtual ~MessageSender();

  void open();
  void close();
  bool is_open() const;

  // Note the sending and receiving of messages of QWebSocket has been split into
  // two parts to keep the dependencies cleanly separated, without hacks.
  void send_msg(const Message& msg) const;

 private slots:
  void on_connected();
  void on_disconnected();
  void on_error(QAbstractSocket::SocketError error);
  void on_ssl_error(const QList<QSslError>& errors);
  void on_state_changed(QAbstractSocket::SocketState);

 private:

  QWebSocket* get_web_socket() const; // The MessageSender will use this to grab the web socket.

  // Our fixed dependencies.
  Dep<NodeJSProcess> _process; // The node js process we want to connect to.

  mutable QWebSocket* _web_socket;
  bool _trying_to_open;

  friend class Grabber; // This is temporary component that needs access to get_web_socket().
};

}

