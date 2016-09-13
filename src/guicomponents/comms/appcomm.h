#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>

#include <QtCore/QProcess>
#include <QtCore/QObject>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslError>

class QWebSocket;

namespace ngs {

class FileModel;

// This class communicates with the nodejs process.
class COMMS_EXPORT AppComm : public QObject, Component {
Q_OBJECT
 public:
  COMPONENT_ID(AppComm, AppComm)

  explicit AppComm(Entity* parent);
  virtual ~AppComm();

  QWebSocket* get_web_socket();
  bool check_connection();

  // Nodejs process.
  Q_INVOKABLE bool nodejs_is_connected();
  Q_INVOKABLE bool nodejs_is_running(); // Whether the nodejs process is running.

signals:
  void nodejs_connected();

 private slots:
  // Slots for handling nodejs process.
  void on_nodejs_started();
  void on_nodejs_error(QProcess::ProcessError error);

  // Slots for handling socket connection to the nodejs process.
  void on_connected();
  void on_disconnected();
  void on_error(QAbstractSocket::SocketError error);
  void on_ssl_error(const QList<QSslError>& errors);
  void on_state_changed(QAbstractSocket::SocketState);

 private:
  // Our fixed dependencies.

  // Nodejs process.
  void start_nodejs();
  void stop_nodejs();

  // Nodejs socket connection state.
  void connect_to_nodejs();

  //Our nodejs process.
  QProcess* _process;
  QWebSocket* _websocket;

  // The nodejs server's port number for us, the app.
  QString _nodejs_port;
  QRegExp _nodejs_port_regex;
};

}

