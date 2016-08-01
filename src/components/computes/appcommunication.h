#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/socketmessage.h>
#include <deque>

#include <QtCore/QProcess>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslError>

class QProcess;
class QWebSocket;

namespace ngs {


// Workaround: As of Qt 5.4 QtQuick does not expose QUrl::fromUserInput.
class COMPUTES_EXPORT Utils : public QObject {
    Q_OBJECT
public:
    Utils(QObject* parent = 0) : QObject(parent) { }
    Q_INVOKABLE static QUrl url_from_input(const QString& input);
};




class COMPUTES_EXPORT AppCommunication : public QObject {
Q_OBJECT
 public:
  static const int kPollInterval;

  explicit AppCommunication(QObject *parent = 0);
  virtual ~AppCommunication();

  // Returns false when it is busy processing a previous command.
  // When false is returned the command will be dropped.
  // You may call the method again to try again.
  bool handle_request_from_app(const SocketMessage& sm);
  Q_INVOKABLE bool handle_request_from_app(const QString& json_text); // Called from qml.
  Q_INVOKABLE QString get_app_dir();
  Q_INVOKABLE QString get_smash_browse_url();

  Q_INVOKABLE bool is_polling();
  Q_INVOKABLE void start_polling();
  Q_INVOKABLE void stop_polling();
  Q_INVOKABLE void open_browser();
  Q_INVOKABLE void close_browser();

signals:
  // Fired on completion of a command.
  void command_finished(const SocketMessage& sm);

 private slots:
  // Slots for our timer.
  void on_poll();

  // Slots for handling nodejs process.
  void on_nodejs_started();
  void on_nodejs_error(QProcess::ProcessError error);

  // Slots for handling socket connection to the nodejs process.
  void on_connected();
  void on_disconnected();
  void on_error(QAbstractSocket::SocketError error);
  void on_ssl_error(const QList<QSslError>& errors);
  void on_state_changed(QAbstractSocket::SocketState);
  void on_text_message_received(const QString & message);


 private:
  // Nodejs process.
  void start_nodejs();
  bool nodejs_is_running();
  void stop_nodejs();

  // Nodejs socket connection state.
  void connect_to_nodejs();
  bool nodejs_is_connected();

  // Browser open/close state.
  void check_browser_is_open();

  // Handles requests originating from clicking items in the browser's context menu.
  void handle_request_from_nodejs(const SocketMessage& sm);

  // Handles responses from the browser's content script, after completing actions/tasks.
  void handle_response_from_nodejs(const SocketMessage& sm);

  //Our nodejs process.
  bool _use_external_process;
  QProcess* _process;
  QWebSocket* _websocket;

  //Command execution state.
  SocketMessage _current_cmd;
  bool _waiting_for_results;

  // Poll timer.
  QTimer _poll_timer;
};

}
