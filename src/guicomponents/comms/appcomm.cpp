#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <guicomponents/comms/appcomm.h>
#include <guicomponents/comms/filemodel.h>


#include <cstddef>
#include <cassert>

#include <openssl/aes.h>

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QGuiApplication>
#include <QtCore/QStandardPaths>

#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtWebSockets/QWebSocket>

#include <iostream>

namespace ngs {

QUrl Utils::url_from_input(const QString& input)
{
    if (input.isEmpty()) {
        return QUrl::fromUserInput("about:blank");
    }
    const QUrl result = QUrl::fromUserInput(input);
    return result.isValid() ? result : QUrl::fromUserInput("about:blank");
}



const int AppComm::kPollInterval = 1000;


QString AppComm::get_app_bin_dir() {
  return QCoreApplication::applicationDirPath();
}

QString AppComm::get_user_data_dir() {
  return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

AppComm::AppComm(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _file_model(this),
      _process(NULL),
      _websocket(NULL),
      _waiting_for_results(false),
      _show_browser(false),
      _nodejs_port_regex("port:(\\d+)"){

  get_dep_loader()->register_fixed_dep(_file_model, "");

  // Setup the websocket.
  _websocket  = new_ff QWebSocket();
  connect(_websocket, SIGNAL(connected()), this, SLOT(on_connected()));
  connect(_websocket, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
  connect(_websocket, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(on_ssl_error(const QList<QSslError>&)));
  connect(_websocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_error(QAbstractSocket::SocketError)));
  connect(_websocket, SIGNAL(textMessageReceived(const QString &)), this, SLOT(on_json_received(const QString &)));
  connect(_websocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(on_state_changed(QAbstractSocket::SocketState)));

  // Setup the poll timer.
  _poll_timer.setSingleShot(false);
  _poll_timer.setInterval(kPollInterval);
  connect(&_poll_timer,SIGNAL(timeout()),this,SLOT(on_poll()));
}

AppComm::~AppComm() {
  delete_ff(_websocket);

  if (_process) {
    _process->kill();
    delete_ff(_process);
  }
}

bool AppComm::is_polling() {
  return _poll_timer.isActive();
}

void AppComm::start_polling() {
  _poll_timer.start();
}

void AppComm::stop_polling() {
  _poll_timer.stop();
}

QString AppComm::get_smash_browse_url() {
  QString app_dir = get_app_bin_dir();
  app_dir += QString("/../html/smashbrowse.html");
  return app_dir;
}

//QVariantMap AppComm::build_message(const QString& json){
//  return Message(json);
//}

QVariantMap AppComm::build_message(const QString& iframe, Message::RequestType rt, const QJsonObject& args, const QString& xpath) {
  return Message(iframe, rt, args, xpath);
}

//QVariantMap AppComm::build_message(const QString& iframe, bool success, const QJsonValue& value){
//  return Message(iframe, success, value);
//}
//
//QVariantMap AppComm::build_message(const Message& other){
//  return Message(other);
//}

bool AppComm::handle_request_from_app(const QString& json) {
  // Make sure nodejs is running.
  if (!nodejs_is_running()) {
    return false;
  }

  // Make sure we're connected to nodejs.
  if (!nodejs_is_connected()) {
    return false;
  }

//  // Make sure not waiting for results from a previous command.
//  if (_waiting_for_results) {
//    return false;
//  }

  // Send the request to nodejs.
  _waiting_for_results = true;
  size_t num_bytes = _websocket->sendTextMessage(json);
  assert(num_bytes);
  return true;
}

bool AppComm::handle_request_from_app(const Message& msg) {
  return handle_request_from_app(msg.to_string());
}

bool AppComm::handle_request_from_app(const QVariantMap& map) {
  //const Message& msg = reinterpret_cast<const Message&>(map);
  //return handle_request_from_app(msg);

  Message msg(map);
  return handle_request_from_app(msg);
}

void AppComm::on_poll() {
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
  } else if (!nodejs_is_connected()) {
    // Connect to the nodejs process.
    connect_to_nodejs();
  } else if (_show_browser) {
    check_browser_is_open();
    check_browser_size();
    _show_browser = false;
  }
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

void AppComm::on_json_received(const QString & json) {
  Message msg(json);
  qDebug() << "hub[" << msg[Message::kIFrame].toString() << "] --> app: " << msg.to_string();

  Message::MessageType type = msg.get_msg_type();
  if (type == Message::MessageType::KRequestMessage) {
    handle_request_from_nodejs(msg);
  } else if (type == Message::MessageType::KResponseMessage) {
    handle_response_from_nodejs(msg);
  } else if (type == Message::MessageType::KInfoMessage) {
    handle_info_from_nodejs(msg);
  } else {
    std::cerr << "Error: Got message of unknown type!\n";
  }
}

// -----------------------------------------------------------------
// Private Methods.
// -----------------------------------------------------------------

void AppComm::open_browser() {
  Message msg(_iframe, Message::RequestType::KOpenBrowser);

  QJsonObject args;
  args[Message::kURL] = get_smash_browse_url();

  msg[Message::kArgs] = args;
  handle_request_from_app(msg);
}

void AppComm::close_browser() {
  Message msg(_iframe, Message::RequestType::KCloseBrowser);
  handle_request_from_app(msg);
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
  QString folder = get_app_bin_dir();
  _process->setWorkingDirectory(folder);

  // Set the arguments.
  QStringList list("chb.js");
  list.append(get_user_data_dir());
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

void AppComm::check_browser_is_open() {
  Message msg(_iframe, Message::RequestType::KCheckBrowserIsOpen);
  handle_request_from_app(msg);
}

void AppComm::check_browser_size() {
  Message msg(_iframe, Message::RequestType::KResizeBrowser);

  int width = _file_model->get_work_setting(FileModel::kBrowserWidthRole).toInt();
  int height = _file_model->get_work_setting(FileModel::kBrowserHeightRole).toInt();

  QJsonObject args;
  args[Message::kWidth] = width;
  args[Message::kHeight] = height;

  msg[Message::kArgs] = args;
  handle_request_from_app(msg);
}

void AppComm::handle_request_from_nodejs(const Message& sm) {

  Message::RequestType type = static_cast<Message::RequestType>(sm.value(Message::kRequest).toInt());

  if (type == Message::RequestType::KNavigateTo) {
    // Hack to pretty up urls coming from the extension.
    // Extract the url from the message.
    QString url = sm[Message::kArgs].toMap()[Message::kURL].toString();
    // Fix the url.
    url = Utils::url_from_input(url).toString();

    // Create a new Request.
    Message updated(sm);
    QVariantMap args = sm[Message::kArgs].toMap();
    args[Message::kURL] = url;
    updated[Message::kArgs] = args;

    // Send it off to the app.
    handle_request_from_app(updated);
  } else if (type == Message::RequestType::KShowAppMenu) {
    qDebug() << "emitting show menu from native side\n";
    emit show_web_action_menu(sm);
  } else {
    handle_request_from_app(sm);
  }

  // Request from nodejs get recorded as nodes in the node graph.

}

void AppComm::handle_response_from_nodejs(const Message& msg) {
  qDebug() << "app received response from nodejs: " << msg.to_string();
  _waiting_for_results = false;
  emit command_finished(msg);
}

void AppComm::handle_info_from_nodejs(const Message& msg) {

}

}
