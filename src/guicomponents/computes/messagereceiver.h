#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/componentids.h>
#include <base/objectmodel/dep.h>

#include <QtCore/QProcess>
#include <QtCore/QObject>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslError>

class QWebSocket;

namespace ngs {

class TaskQueuer;

class GUICOMPUTES_EXPORT MessageReceiver : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(MessageReceiver, MessageReceiver)

  static const size_t kMaxMsgSizeToOutput;

  explicit MessageReceiver(Entity* parent);
  virtual ~MessageReceiver();

 public slots:
  void on_connected();
  void on_disconnected();
  void on_error(QAbstractSocket::SocketError error);
  void on_ssl_error(const QList<QSslError>& errors);
  void on_state_changed(QAbstractSocket::SocketState);
  void on_text_received(const QString & text);

 protected:
  // Our state.
  virtual void update_wires();

 private:
  // Debug methods.
  void separate_messages(const QString& text);

  // Our fixed dependencies.
  Dep<TaskQueuer> _queuer;

  QWebSocket* _web_socket; // Borrowed reference.
  bool _connected; // Whether our slot is connected to receive text from the socket.
};

}

