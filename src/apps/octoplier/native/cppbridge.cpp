#include <base/objectmodel/entity.h>
#include <components/compshapes/nodegraphselection.h>
#include <native/cppbridge.h>
#include <native/variantmaptreemodel.h>

#include <components/interactions/shapecanvas.h>
#include <gui/quick/nodegraphquickitem.h>
#include <gui/quick/nodegraphquickitemglobals.h>

#include <iostream>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtQml/QQmlApplicationEngine>
#include <QtQuick/QQuickView>
#include <QtWidgets/QSplashScreen>
#include <QtGui/QGuiApplication>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>


#include <components/computes/computeglobals.h>
#include <gui/widget/splashscreen.h>

namespace ngs {



CppBridge::CppBridge(QObject *parent)
    : QObject(parent),
      _process(NULL),
      _websocket(NULL),
      _use_external_process(true),
      _commands_running(false){

  _websocket  = new_ff QWebSocket();
  connect(_websocket, SIGNAL(connected()), this, SLOT(on_connected()));
  connect(_websocket, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
  connect(_websocket, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(on_ssl_error(const QList<QSslError>&)));
  connect(_websocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_error(QAbstractSocket::SocketError)));
  connect(_websocket, SIGNAL(textMessageReceived(const QString &)), this, SLOT(on_text_message_received(const QString &)));
}

CppBridge::~CppBridge() {
  replay_recording("close_browser:");
  delete_ff(_websocket);
  delete_ff(_process);
}

void CppBridge::start_browser_controller() {
  if (_use_external_process) {
    return;
  }

  delete_ff(_process);
  _process = new_ff QProcess(this);
  connect(_process, SIGNAL(started()), this, SLOT(on_browser_controller_started()));
  _process->setProgram(QString("node.exe"));
  QString folder = qApp->applicationDirPath();
  _process->setWorkingDirectory(folder);
  QStringList list("controller.js");
  _process->setArguments(list);
  _process->start();
  qDebug() << "process is trying to start";
}

void CppBridge::close_browser_controller() {
  delete_ff(_process);
}

void CppBridge::connect_to_browser_controller() {
  _websocket->open(QUrl(("ws://localhost:8082")));
}

void CppBridge::on_browser_controller_started() {
  qDebug() << "process has started";
}

void CppBridge::on_connected() {
  qDebug() << "connected to browser controller";
  on_text_message_received("ok:");
}

void CppBridge::on_disconnected() {
  qDebug() << "disconnected: trying to connect again.";
  connect_to_browser_controller();
}

void CppBridge::replay_recording(QString recording) {
  QString command = "replay_recording:" + recording;
  _commands.push_back(command);

  qDebug() << "pushed command: " << command << "\n";
  qDebug() << "command stack size: " << _commands.size();

  process_command();
}

//void CppBridge::fire_commands() {
//  // If this is the only command in the queue then queue it.
//  if (!_commands_running) {
//    qDebug() << "IIIIIIIIIII starting up command process stack";
//    _commands_running = true;
//    process_command();
//  } else {
//    for (int i=0; i<_commands.size(); ++i) {
//      qDebug() << _commands[i] << "\n";
//    }
//  }
//}

void CppBridge::start_recording() {
  _commands.push_back("open_browser:");
  _commands.push_back("goto_url:https://www.google.com");
  _commands.push_back("start_recording:");
  _commands.push_back("goto_url:https://www.google.com");
  process_command();
}

void CppBridge::stop_recording() {
  if (!_commands.empty()) {
    _commands.clear();
  }
  _commands.push_back("stop_recording:");
  process_command();
}

void CppBridge::replay_last() {
  replay_recording(_recording);
}

void CppBridge::get_recording() {
  _commands.push_back("get_recording:");
  process_command();
}

void CppBridge::on_error(QAbstractSocket::SocketError error) {
  qDebug() << "Error: " << error;
  if (error == QAbstractSocket::RemoteHostClosedError) {
    on_text_message_received("fail:");
  }
  qDebug() << "Error string: " << _websocket->errorString();
}

void CppBridge::on_ssl_error(const QList<QSslError>& errors) {
  qDebug() << "SSLError: " << errors;
  _websocket->ignoreSslErrors(errors);
}

void CppBridge::on_text_message_received(const QString & message) {
  if (_process) {
    QString controller_output(_process->readAllStandardOutput());
    qDebug() << "Controller: " << controller_output;
  }

  qDebug() << "received message: " << message;

  if (!message.startsWith("fail:")) {
    if (!_commands.empty()) {
      _commands.pop_front();
    }
  }

  qDebug() << "command stack size is now: " << _commands.size();

  // Unpack the command.
  QString code;
  QString body;
  unpack_command(message, code, body);

  // If the command succeeded, run the next command.
  if (code == "ok") {
    process_command();
  } else if (code == "fail") {
    process_command();
  } else if (code == "recording") {
    _recording = body;
    process_command();
  } else if (code == "finished_recording") {
    qDebug() << "finished recording: " + body;
    _recording = body;
  } else {
    _commands.clear();
  }
}

QString CppBridge::pack_command(const QString& code, const QString& body) {
  return code + ":" + body;
}

void CppBridge::unpack_command(const QString& command, QString& code, QString& body) {
  int pos = command.indexOf(':');
  code = command.mid(0,pos);
  body = command.mid(pos+1);
}

void CppBridge::process_command() {
  // Update running state.
  if (_commands.empty()) {
    _commands_running = false;
    return;
  } else if (_commands_running == false) {
    _commands_running = true;
  }

  // Make sure the controller is running.
  if ((!_use_external_process) && (!_process || (_process->state() != QProcess::Running))) {
    _commands.push_front("start_browser_controller:");
  }
  // Make sure the connection to the controller is running.
  else if (!_websocket->isValid()) {
    _commands.push_front("connect_to_browser_controller:");
  }

  const QString &command = _commands.front();
  qDebug() << "running command: " << command;

  if (command.startsWith("start_browser_controller:")) {
    start_browser_controller();
    _commands.pop_front();
    return process_command();
  } else if (command.startsWith("close_browser_controller:")) {
    close_browser_controller();
    _commands.pop_front();
    return process_command();
  } else if (command.startsWith("connect_to_browser_controller:")) {
    qDebug() << "trying to connect to browser";
    connect_to_browser_controller();
  } else {
    size_t num_bytes = _websocket->sendTextMessage(command);
    assert(num_bytes);
  }
}

}
