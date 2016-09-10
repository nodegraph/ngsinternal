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

QVariantMap AppComm::build_msg_from_json(const QString& json){
  return Message(json);
}

QVariantMap AppComm::build_copied_msg(const Message& other){
  return Message(other);
}

QVariantMap AppComm::build_request_msg(const QString& iframe, RequestType rt, const QVariantMap& args, const QString& xpath) {
  return Message(iframe, rt, args, xpath);
}

QVariantMap AppComm::build_response_msg(const QString& iframe, bool success, const QVariant& value){
  return Message(iframe, success, value);
}

QVariantMap AppComm::build_info_msg(const QString& iframe, InfoType it){
  return Message(iframe, it);
}



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
  qDebug() << "hub --> app: " << json;
  Message msg(json);

  MessageType type = msg.get_msg_type();
  if (type == MessageType::kRequestMessage) {
    handle_request_from_nodejs(msg);
  } else if (type == MessageType::kResponseMessage) {
    handle_response_from_nodejs(msg);
  } else if (type == MessageType::kInfoMessage) {
    handle_info_from_nodejs(msg);
  } else {
    std::cerr << "Error: Got message of unknown type!\n";
  }
}

// -----------------------------------------------------------------
// Private Methods.
// -----------------------------------------------------------------

void AppComm::open_browser() {
  Message msg(_iframe, RequestType::kOpenBrowser);

  QJsonObject args;
  args[Message::kURL] = get_smash_browse_url();

  msg[Message::kArgs] = args;
  handle_request_from_app(msg);
}

void AppComm::close_browser() {
  Message msg(_iframe, RequestType::kCloseBrowser);
  handle_request_from_app(msg);
}

void AppComm::get_all_cookies() {
  QString iframe = _show_menu_msg[Message::kIFrame].toString();
  Message msg(iframe, RequestType::kGetAllCookies);
  handle_request_from_app(msg);
}

void AppComm::clear_all_cookies() {
  QString iframe = _show_menu_msg[Message::kIFrame].toString();
  Message msg(iframe, RequestType::kClearAllCookies);
  handle_request_from_app(msg);
}

void AppComm::set_all_cookies() {
  QString iframe = _show_menu_msg[Message::kIFrame].toString();
  Message msg(iframe, RequestType::kSetAllCookies);
  handle_request_from_app(msg);
}

void AppComm::update_overlays() {
  QString iframe = _show_menu_msg[Message::kIFrame].toString();
  Message msg(iframe, RequestType::kUpdateOveralys);
  handle_request_from_app(msg);
}

int AppComm::get_set_index() {
  return _show_menu_msg[Message::kArgs].toMap()[Message::kSetIndex].toInt();
}
int AppComm::get_overlay_index() {
  return _show_menu_msg[Message::kArgs].toMap()[Message::kOverlayIndex].toInt();
}
QStringList AppComm::get_option_texts() {
  return _show_menu_msg[Message::kArgs].toMap()[Message::kOptionTexts].toStringList();
}

void AppComm::navigate_to(const QString& url) {
  // Fix the url.
  QString decorated_url = Utils::url_from_input(url).toString();

  // Create the args.
  QVariantMap args;
  args[Message::kURL] = decorated_url;

  // Send the message.
  Message msg("", RequestType::kNavigateTo, args);
  handle_request_from_app(msg);
}

void AppComm::navigate_refresh() {
  // Send the message.
  Message msg("", RequestType::kNavigateRefresh);
  handle_request_from_app(msg);
}

void AppComm::create_set_by_matching_text() {
  QVariantMap args;
  args[Message::kWrapType] = WrapType::text;
  args[Message::kMatchValues] = _show_menu_msg[Message::kArgs].toMap()[Message::kTextValues];

  Message req(_iframe, RequestType::kCreateSetFromMatchValues);
  req[Message::kArgs] = args;

  handle_request_from_app(req);
}

void AppComm::create_set_by_matching_images() {
  QVariantMap args;
  args[Message::kWrapType] = WrapType::image;
  args[Message::kMatchValues] = _show_menu_msg[Message::kArgs].toMap()[Message::kImageValues];

  Message req(_iframe, RequestType::kCreateSetFromMatchValues);
  req[Message::kArgs] = args;

  handle_request_from_app(req);
}

void AppComm::create_set_of_inputs() {
  QVariantMap args;
  args[Message::kWrapType] = WrapType::input;

  Message req(_iframe, RequestType::kCreateSetFromWrapType);
  req[Message::kArgs] = args;

  handle_request_from_app(req);
}

void AppComm::create_set_of_selects() {
  QVariantMap args;
  args[Message::kWrapType] = WrapType::select;

  Message req(_iframe, RequestType::kCreateSetFromWrapType);
  req[Message::kArgs] = args;

  handle_request_from_app(req);
}

void AppComm::create_set_of_images() {
  QVariantMap args;
  args[Message::kWrapType] = WrapType::image;

  Message req(_iframe, RequestType::kCreateSetFromWrapType);
  req[Message::kArgs] = args;

  handle_request_from_app(req);
}

void AppComm::create_set_of_text() {
  QVariantMap args;
  args[Message::kWrapType] = WrapType::text;

  Message req(_iframe, RequestType::kCreateSetFromWrapType);
  req[Message::kArgs] = args;

  handle_request_from_app(req);
}

void AppComm::delete_set() {
  if (get_set_index()<0) {
    return;
  }

  QVariantMap args;
  args[Message::kSetIndex] = get_set_index();

  Message req(_iframe, RequestType::kDeleteSet);
  req[Message::kArgs] = args;

  handle_request_from_app(req);
}

void AppComm::shift_set(WrapType wrap_type, Direction dir) {
  if (get_set_index()<0) {
      return;
    }

  QVariantMap args;
  args[Message::kSetIndex] = get_set_index();
  args[Message::kDirection] = dir;
  args[Message::kWrapType] = wrap_type;

  Message req(_iframe, RequestType::kShiftSet);
  req[Message::kArgs] = args;

  handle_request_from_app(req);
}

void AppComm::shift_to_text_above() {
  shift_set(WrapType::text, Direction::up);
}
void AppComm::shift_to_text_below() {
  shift_set(WrapType::text, Direction::down);
}
void AppComm::shift_to_text_on_left() {
  shift_set(WrapType::text, Direction::left);
}
void AppComm::shift_to_text_on_right() {
  shift_set(WrapType::text, Direction::right);
}

void AppComm::shift_to_images_above() {
  shift_set(WrapType::image, Direction::up);
}
void AppComm::shift_to_images_below() {
  shift_set(WrapType::image, Direction::down);
}
void AppComm::shift_to_images_on_left() {
  shift_set(WrapType::image, Direction::left);
}
void AppComm::shift_to_images_on_right() {
  shift_set(WrapType::image, Direction::right);
}

void AppComm::shift_to_inputs_above() {
  shift_set(WrapType::input, Direction::up);
}
void AppComm::shift_to_inputs_below() {
  shift_set(WrapType::input, Direction::down);
}
void AppComm::shift_to_inputs_on_left() {
  shift_set(WrapType::input, Direction::left);
}
void AppComm::shift_to_inputs_on_right() {
  shift_set(WrapType::input, Direction::right);
}

void AppComm::shift_to_selects_above() {
  shift_set(WrapType::select, Direction::up);
}
void AppComm::shift_to_selects_below() {
  shift_set(WrapType::select, Direction::down);
}
void AppComm::shift_to_selects_on_left() {
  shift_set(WrapType::select, Direction::left);
}
void AppComm::shift_to_selects_on_right() {
  shift_set(WrapType::select, Direction::right);
}

void AppComm::expand(Direction dir, const QVariantMap& match_criteria) {
  if (get_set_index()<0) {
      return;
    }

  QVariantMap args;
  args[Message::kSetIndex] = get_set_index();
  args[Message::kDirection] = dir;
  args[Message::kMatchCriteria] = match_criteria;

  Message req(_iframe, RequestType::kExpandSet);
  req[Message::kArgs] = args;
  handle_request_from_app(req);
}

void AppComm::expand_above() {
  QVariantMap match_criteria;
  match_criteria[Message::kMatchLeft] = true;
  match_criteria[Message::kMatchRight] = false;
  match_criteria[Message::kMatchTop] = false;
  match_criteria[Message::kMatchBottom] = false;
  match_criteria[Message::kMatchFont] = true;
  match_criteria[Message::kMatchFontSize] = true;
  expand(Direction::up, match_criteria);
}

void AppComm::expand_below() {
  QVariantMap match_criteria;
  match_criteria[Message::kMatchLeft] = true;
  match_criteria[Message::kMatchRight] = false;
  match_criteria[Message::kMatchTop] = false;
  match_criteria[Message::kMatchBottom] = false;
  match_criteria[Message::kMatchFont] = true;
  match_criteria[Message::kMatchFontSize] = true;
  expand(Direction::down, match_criteria);
}

void AppComm::expand_left() {
  QVariantMap match_criteria;
  match_criteria[Message::kMatchLeft] = false;
  match_criteria[Message::kMatchRight] = false;
  match_criteria[Message::kMatchTop] = true;
  match_criteria[Message::kMatchBottom] = false;
  match_criteria[Message::kMatchFont] = true;
  match_criteria[Message::kMatchFontSize] = true;
  expand(Direction::left, match_criteria);
}

void AppComm::expand_right() {
  QVariantMap match_criteria;
  match_criteria[Message::kMatchLeft] = false;
  match_criteria[Message::kMatchRight] = false;
  match_criteria[Message::kMatchTop] = true;
  match_criteria[Message::kMatchBottom] = false;
  match_criteria[Message::kMatchFont] = true;
  match_criteria[Message::kMatchFontSize] = true;
  expand(Direction::right, match_criteria);
}

void AppComm::mark_set() {
  if (get_set_index()<0) {
      return;
    }

  QVariantMap args;
  args[Message::kSetIndex] = get_set_index();

  Message req(_iframe, RequestType::kMarkSet);
  req[Message::kArgs] = args;
  handle_request_from_app(req);
}
void AppComm::unmark_set() {
  if (get_set_index()<0) {
      return;
    }

  QVariantMap args;
  args[Message::kSetIndex] = get_set_index();

  Message req(_iframe, RequestType::kUnmarkSet);
  req[Message::kArgs] = args;
  handle_request_from_app(req);
}
void AppComm::merge_sets() {
  Message req(_iframe, RequestType::kMergeMarkedSets);
  handle_request_from_app(req);
}

void AppComm::shrink_set_to_side(Direction dir) {
  if (get_set_index()<0) {
      return;
    }

  QVariantMap args;
  args[Message::kSetIndex] = get_set_index();
  args[Message::kDirection] = dir;

  Message req(_iframe, RequestType::kShrinkSet);
  req[Message::kArgs] = args;
  handle_request_from_app(req);
}

void AppComm::shrink_set_to_topmost() {
  shrink_set_to_side(Direction::up);
}
void AppComm::shrink_set_to_bottommost() {
  shrink_set_to_side(Direction::down);
}
void AppComm::shrink_set_to_leftmost() {
  shrink_set_to_side(Direction::left);
}
void AppComm::shrink_set_to_rightmost() {
  shrink_set_to_side(Direction::right);
}

void AppComm::shrink_against_marked(const QVariantList& dirs) {
  if (get_set_index()<0) {
      return;
    }

  QVariantMap args;
  args[Message::kSetIndex] = get_set_index();
  args[Message::kDirections] = dirs;

  Message req(_iframe, RequestType::kShrinkSetToMarked);
  req[Message::kArgs] = args;
  handle_request_from_app(req);
}
void AppComm::shrink_above_of_marked() {
  QVariantList dirs;
  dirs.append(Direction::up);
  shrink_against_marked(dirs);
}
void AppComm::shrink_below_of_marked() {
  QVariantList dirs;
  dirs.append(Direction::down);
  shrink_against_marked(dirs);
}
void AppComm::shrink_above_and_below_of_marked() {
  QVariantList dirs;
  dirs.append(Direction::up);
  dirs.append(Direction::down);
  shrink_against_marked(dirs);
}
void AppComm::shrink_left_of_marked() {
  QVariantList dirs;
  dirs.append(Direction::left);
  shrink_against_marked(dirs);
}
void AppComm::shrink_right_of_marked() {
  QVariantList dirs;
  dirs.append(Direction::right);
  shrink_against_marked(dirs);
}
void AppComm::shrink_left_and_right_of_marked() {
  QVariantList dirs;
  dirs.append(Direction::left);
  dirs.append(Direction::right);
  shrink_against_marked(dirs);
}

void AppComm::perform_action(ActionType action) {
  if (get_set_index()<0) {
      return;
    }

  QVariantMap args;
  args[Message::kSetIndex] = get_set_index();
  args[Message::kOverlayIndex] = 0;
  args[Message::kAction] = action;

  Message req(_iframe, RequestType::kPerformAction);
  req[Message::kArgs] = args;
  handle_request_from_app(req);
}
void AppComm::perform_action(const QVariantMap& args) {
  Message req(_iframe, RequestType::kPerformAction);
  req[Message::kArgs] = args;
  handle_request_from_app(req);
}
void AppComm::perform_click() {
  perform_action(ActionType::kSendClick);
}
void AppComm::type_text(const QString& text) {
  if (get_set_index()<0) {
      return;
    }

  QVariantMap args;
  args[Message::kSetIndex] = get_set_index();
  args[Message::kOverlayIndex] = 0;
  args[Message::kAction] = ActionType::kSendText;
  args[Message::kText] = text;
  perform_action(args);
}
void AppComm::type_enter() {
  perform_action(ActionType::kSendEnter);
}
void AppComm::extract_text() {
  perform_action(ActionType::kGetText);
}
void AppComm::select_from_dropdown(const QString& option_text) {
  if (get_set_index()<0) {
      return;
    }

  QVariantMap args;
  args[Message::kSetIndex] = get_set_index();
  args[Message::kOverlayIndex] = 0;
  args[Message::kAction] = ActionType::kSelectOption;
  args[Message::kOptionText] = option_text;
  perform_action(args);
}


void AppComm::scroll_down() {
  perform_action(ActionType::kScrollDown);
}
void AppComm::scroll_up() {
  perform_action(ActionType::kScrollUp);
}
void AppComm::scroll_right() {
  perform_action(ActionType::kScrollRight);
}
void AppComm::scroll_left() {
  perform_action(ActionType::kScrollLeft);
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
  Message msg(_iframe, RequestType::kCheckBrowserIsOpen);
  handle_request_from_app(msg);
}

void AppComm::check_browser_size() {
  Message msg(_iframe, RequestType::kResizeBrowser);

  int width = _file_model->get_work_setting(FileModel::kBrowserWidthRole).toInt();
  int height = _file_model->get_work_setting(FileModel::kBrowserHeightRole).toInt();

  QJsonObject args;
  args[Message::kWidth] = width;
  args[Message::kHeight] = height;

  msg[Message::kArgs] = args;
  handle_request_from_app(msg);
}

void AppComm::handle_request_from_nodejs(const Message& sm) {

  RequestType type = static_cast<RequestType>(sm.value(Message::kRequest).toInt());

  if (type == RequestType::kShowAppMenu) {
    qDebug() << "emitting show menu from native side\n";
    _show_menu_msg = sm;
    emit show_web_action_menu(sm);
  } else {
    handle_request_from_app(sm);
  }

  // Request from nodejs get recorded as nodes in the node graph.

}

void AppComm::handle_response_from_nodejs(const Message& msg) {
  _waiting_for_results = false;
  emit command_finished(msg);
}

void AppComm::handle_info_from_nodejs(const Message& msg) {

}

}
