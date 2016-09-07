#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>
#include <guicomponents/comms/message.h>

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

class FileModel;

// Workaround: As of Qt 5.4 QtQuick does not expose QUrl::fromUserInput.
class COMMS_EXPORT Utils : public QObject {
    Q_OBJECT
public:
    Utils(QObject* parent = 0) : QObject(parent) { }
    Q_INVOKABLE static QUrl url_from_input(const QString& input);
};

// This class communicates with the nodejs process.
class COMMS_EXPORT AppComm : public QObject, Component {
Q_OBJECT
 public:
  COMPONENT_ID(AppComm, AppComm)

  static const int kPollInterval;

  static QString get_app_bin_dir();
  static QString get_user_data_dir();

  explicit AppComm(Entity* parent);
  virtual ~AppComm();

  // Returns false when it is busy processing a previous command.
  // When false is returned the command will be dropped.
  // You may call the method again to try again.
  Q_INVOKABLE bool handle_request_from_app(const Message& sm);
  Q_INVOKABLE bool handle_request_from_app(const QString& json); // Called from qml.
  Q_INVOKABLE QString get_smash_browse_url();

  // Polling to keep browser open and of the right size.
  Q_INVOKABLE bool is_polling();
  Q_INVOKABLE void start_polling();
  Q_INVOKABLE void stop_polling();

  // Nodejs process.
  Q_INVOKABLE bool nodejs_is_connected();
  Q_INVOKABLE bool nodejs_is_running(); // Whether the nodejs process is running.

  // Show or hide browser as part of polling.
  Q_INVOKABLE void show_browser() {_show_browser = true;}
  Q_INVOKABLE void hide_browser() {_show_browser = false;}

  // Open or close browser outside of polling.
  Q_INVOKABLE void open_browser();
  Q_INVOKABLE void close_browser();

signals:
  // Fired on completion of a command.
  void command_finished(const Message& msg);
  void nodejs_connected();

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
  void on_json_received(const QString & json);

 private:

  // Our fixed dependencies.
  Dep<FileModel> _file_model;

  // Nodejs process.
  void start_nodejs();
  void stop_nodejs();

  // Nodejs socket connection state.
  void connect_to_nodejs();

  // Check browser state.
  void check_browser_is_open();
  void check_browser_size();

  // Handles requests originating from clicking items in the browser's context menu.
  void handle_request_from_nodejs(const Message& sm);

  // Handles responses from the browser's content script, after completing actions/tasks.
  void handle_response_from_nodejs(const Message& sm);
  void handle_info_from_nodejs(const Message& msg);

  //Our nodejs process.
  QProcess* _process;
  QWebSocket* _websocket;

  //Command execution state.
  bool _waiting_for_results;

  // Poll timer.
  QTimer _poll_timer;

  // Browser.
  bool _show_browser;

  // The nodejs server's port number for us, the app.
  QString _nodejs_port;
  QRegExp _nodejs_port_regex;

  QString _iframe;
};

}

