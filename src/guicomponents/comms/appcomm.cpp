#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <guicomponents/comms/appcomm.h>
#include <guicomponents/comms/appworker.h>

#include <cstddef>
#include <cassert>

#include <openssl/aes.h>

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QGuiApplication>
#include <QtCore/QStandardPaths>

#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QProcess>
#include <QtWebSockets/QWebSocket>

#include <iostream>

namespace ngs {

AppComm::AppComm(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _process(NULL),
      _websocket(NULL),
      _nodejs_port_regex("port:(\\d+)"){

  // Setup the websocket.
  _websocket  = new_ff QWebSocket();
  connect(_websocket, SIGNAL(connected()), this, SLOT(on_connected()));
  connect(_websocket, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
  connect(_websocket, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(on_ssl_error(const QList<QSslError>&)));
  connect(_websocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_error(QAbstractSocket::SocketError)));
  connect(_websocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(on_state_changed(QAbstractSocket::SocketState)));

}

AppComm::~AppComm() {
  delete_ff(_websocket);

  if (_process) {
    _process->kill();
    delete_ff(_process);
  }
}

QWebSocket* AppComm::get_web_socket() {
  return _websocket;
}

bool AppComm::check_connection() {
  // Read any output from the nodejs process.
  if (_process) {
    QDebug debug = qDebug();
    debug.noquote();

    //debug << "nodejs state: " << _process->state();
    QString output(_process->readAllStandardOutput());

    // Look for the nodejs port number.
    if (_nodejs_port.isEmpty()) {
      int pos = _nodejs_port_regex.indexIn(output);
      if (pos >= 0) {
        QStringList list = _nodejs_port_regex.capturedTexts();
        assert(list.size() == 2);
        _nodejs_port = list[1];
      }
    }

    // Dump any std output from the process.
    if (!output.isEmpty()) {
      debug << "------------------nodejs out--------------------\n";
      debug << output;
      debug << "------------------------------------------------\n";
    }

    // Dump any std errors from the process.
    QString error(_process->readAllStandardError());
    if (!error.isEmpty()) {
      debug << "------------------nodejs err--------------------\n";
      debug << error;
      debug << "------------------------------------------------\n";
    }
  }

  // Try to get nodejs running and connected.
  if (!nodejs_is_running()) {
    // Start the nodejs process.
    start_nodejs();
    return false;
  } else if (!nodejs_is_connected()) {
    // Connect to the nodejs process.
    connect_to_nodejs();
    return false;
  }
  return true;
}

void AppComm::on_nodejs_started() {
  //qDebug() << "process has started";
}

void AppComm::on_nodejs_error(QProcess::ProcessError error) {
  qDebug() << "process error: " << error << "\n";
}

void AppComm::on_connected() {
  //qDebug() << "nodejs is now connected";
  emit nodejs_connected();
}

void AppComm::on_disconnected() {
  //qDebug() << "nodejs is now disconnected";
}

void AppComm::on_error(QAbstractSocket::SocketError error) {
  if (error == QAbstractSocket::SocketError::SslHandshakeFailedError) {
    // We expect this error because we're using a local ssl server.
    return;
  } else if (error == QAbstractSocket::RemoteHostClosedError) {
  }

  qDebug() << "AppComm Error: " << error;
  qDebug() << "WebSocket error string: " << _websocket->errorString();
}

void AppComm::on_ssl_error(const QList<QSslError>& errors) {
  //qDebug() << "SSLError: " << errors;
  _websocket->ignoreSslErrors(errors);
}

void AppComm::on_state_changed(QAbstractSocket::SocketState s) {
  //qDebug() << "state changed: " << s;
}

// -----------------------------------------------------------------
// Private Methods.
// -----------------------------------------------------------------

void AppComm::start_nodejs() {
  if (nodejs_is_running()) {
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
  QString folder = AppWorker::get_app_bin_dir();
  _process->setWorkingDirectory(folder);

  // Set the arguments.
  QStringList list("chb.js");
  list.append(AppWorker::get_user_data_dir());
  _process->setArguments(list);
  _process->start();

  qDebug() << "starting process again!";
}

bool AppComm::nodejs_is_running() {
  if (_process) {
    if (_process->state() == QProcess::Running) {
      return true;
    }
  }
  return false;
}

void AppComm::stop_nodejs() {
  delete_ff(_process);
}

void AppComm::connect_to_nodejs() {
  if (_nodejs_port.isEmpty()) {
    return;
  }

  // Form the websocket server's url using the port number.
  QString url("wss://localhost:");
  url += _nodejs_port;

  // Open the port.
  _websocket->open(QUrl(url));
}

bool AppComm::nodejs_is_connected() {
  if (_websocket->isValid()) {
    return true;
  }
  return false;
}

}
