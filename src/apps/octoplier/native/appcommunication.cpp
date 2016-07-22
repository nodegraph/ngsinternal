#include <native/appcommunication.h>
#include <native/utils.h>
#include <base/memoryallocator/taggednew.h>

#include <cstddef>
#include <cassert>

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>


#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtWebSockets/QWebSocket>

namespace ngs {

const int AppCommunication::kPollInterval = 1000;

AppCommunication::AppCommunication(QObject *parent)
    : QObject(parent),
      _process(NULL),
      _websocket(NULL),
      _use_external_process(false),
      _waiting_for_results(false){

  // Setup the websocket.
  _websocket  = new_ff QWebSocket();
  connect(_websocket, SIGNAL(connected()), this, SLOT(on_connected()));
  connect(_websocket, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
  connect(_websocket, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(on_ssl_error(const QList<QSslError>&)));
  connect(_websocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_error(QAbstractSocket::SocketError)));
  connect(_websocket, SIGNAL(textMessageReceived(const QString &)), this, SLOT(on_text_message_received(const QString &)));
  connect(_websocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(on_state_changed(QAbstractSocket::SocketState)));

  // Start the poll timer.
  _poll_timer.setSingleShot(false);
  _poll_timer.setInterval(kPollInterval);
  connect(&_poll_timer,SIGNAL(timeout()),this,SLOT(on_poll()));
  _poll_timer.start();
}

AppCommunication::~AppCommunication() {
  handle_request_from_app(SocketMessage("{request: 'close_browser'}"));
  delete_ff(_websocket);

  _process->kill();
  delete_ff(_process);
}

QString AppCommunication::get_app_dir() {
  return QCoreApplication::applicationDirPath();
}

QString AppCommunication::get_smash_browse_url() {
  QString app_dir = get_app_dir();
  app_dir += QString("/smashbrowse.html");
  return app_dir;
}

bool AppCommunication::handle_request_from_app(const QString& json_text) {
  SocketMessage sm(json_text);
  return handle_request_from_app(sm);
}

bool AppCommunication::handle_request_from_app(const SocketMessage& sm) {
  // Make sure nodejs is running.
  if (!nodejs_is_running()) {
    return false;
  }

  // Make sure we're connected to nodejs.
  if (!nodejs_is_connected()) {
    return false;
  }

  // Make sure not waiting for results from a previous command.
  if (_waiting_for_results) {
    return false;
  }

  // Send the request to nodejs.
  _waiting_for_results = true;
  size_t num_bytes = _websocket->sendTextMessage(sm.get_json_text());
  assert(num_bytes);
  return true;
}

void AppCommunication::on_poll() {
  // Read any output from the nodejs process.
  if (_process) {
    QDebug debug = qDebug();
    debug.noquote();

    //debug << "nodejs state: " << _process->state();

    QString output(_process->readAllStandardOutput());
    if (!output.isEmpty()) {
      debug << "nodejs out: " << output;
    }

    QString error(_process->readAllStandardError());
    if (!error.isEmpty()) {
      debug << "nodejs err: " << error;
    }
  }

  // Try to get nodejs running and connected.
  if (!nodejs_is_running()) {
    // Start the nodejs process.
    start_nodejs();
  } else if (!nodejs_is_connected()) {
    // Connect to the nodejs process.
    connect_to_nodejs();
  }
}

void AppCommunication::on_nodejs_started() {
  qDebug() << "process has started";
}

void AppCommunication::on_nodejs_error(QProcess::ProcessError error) {
  qDebug() << "process error: " << error << "\n";
}

void AppCommunication::on_connected() {
  qDebug() << "nodejs is now connected";
}

void AppCommunication::on_disconnected() {
  qDebug() << "nodejs is now disconnected";
}

void AppCommunication::on_error(QAbstractSocket::SocketError error) {
  qDebug() << "Error: " << error;
  if (error == QAbstractSocket::RemoteHostClosedError) {
  }
  qDebug() << "Error websocket error string: " << _websocket->errorString();
}

void AppCommunication::on_ssl_error(const QList<QSslError>& errors) {
  qDebug() << "SSLError: " << errors;
  _websocket->ignoreSslErrors(errors);
}

void AppCommunication::on_state_changed(QAbstractSocket::SocketState s) {
  qDebug() << "state changed: " << s;
}

void AppCommunication::on_text_message_received(const QString & message) {
  SocketMessage sm(message);
  const QJsonObject& obj = sm.get_json_obj();
  if (sm.is_request()) {
    handle_request_from_nodejs(sm);
  } else {
    handle_response_from_nodejs(sm);
  }
}

// -----------------------------------------------------------------
// Private Methods.
// -----------------------------------------------------------------

void AppCommunication::start_nodejs() {
  if (_use_external_process) {
    return;
  }
  // Create a process.
  delete_ff(_process);
  _process = new_ff QProcess(this);

  // Connect to signals.
  connect(_process, SIGNAL(started()), this, SLOT(on_nodejs_started()));
  connect(_process, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(on_nodejs_error(QProcess::ProcessError)));

  // Set the executable.
  _process->setProgram(QString("node.exe"));

  // Set the working directory.
  QString folder = qApp->applicationDirPath();
  _process->setWorkingDirectory(folder);

  // Set the arguments.
  QStringList list("controller.js");
  _process->setArguments(list);
  _process->start();

  qDebug() << "starting process again!";
}

bool AppCommunication::nodejs_is_running() {
  if (_use_external_process) {
    return true;
  }
  if (_process) {
    if (_process->state() == QProcess::Running) {
      return true;
    }
  }
  return false;
}

void AppCommunication::stop_nodejs() {
  delete_ff(_process);
}

void AppCommunication::connect_to_nodejs() {
  _websocket->open(QUrl(QStringLiteral("ws://localhost:8082")));
}

bool AppCommunication::nodejs_is_connected() {
  if (_websocket->isValid()) {
    return true;
  }
  return false;
}

void AppCommunication::handle_request_from_nodejs(const SocketMessage& sm) {
  // Do stuff like create nodes in the node graph, in response to
  // messages from nodejs.
  qDebug() << "app received request from nodejs: " << sm.get_json_text();

  // Hack to fix up urls coming from the extension.
  if (sm.get_json_obj().value("request").toString() == "navigate_to") {
    QJsonObject obj = sm.get_json_obj();
    QString url = obj["url"].toString();
    url = Utils::url_from_input(url).toString();
    obj["url"] = url;
    SocketMessage adjusted(obj);
    handle_request_from_app(adjusted);
  } else {
    handle_request_from_app(sm);
  }

  // Request from nodejs get recorded as nodes in the node graph.

}

void AppCommunication::handle_response_from_nodejs(const SocketMessage& sm) {
  qDebug() << "app received response from nodejs: " << sm.get_json_text();
  _waiting_for_results = false;
  emit command_finished(sm);
}

}
