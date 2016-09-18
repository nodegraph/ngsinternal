#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <guicomponents/comms/appworker.h>
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

const int AppWorker::kPollInterval = 1000;
const int AppWorker::kJitterSize = 1;

QUrl get_proper_url(const QString& input) {
  if (input.isEmpty()) {
    return QUrl::fromUserInput("about:blank");
  }
  const QUrl result = QUrl::fromUserInput(input);
  return result.isValid() ? result : QUrl::fromUserInput("about:blank");
}

AppWorker::AppWorker(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _app_comm(this),
      _file_model(this),
      _show_browser(true),
      _hovering(false),
      _jitter(kJitterSize),
      _waiting_for_results(false),
      _next_msg_id(0),
      _connected(false){
  get_dep_loader()->register_fixed_dep(_app_comm, "");
  get_dep_loader()->register_fixed_dep(_file_model, "");

  // Setup the poll timer.
  _poll_timer.setSingleShot(false);
  _poll_timer.setInterval(kPollInterval);
  connect(&_poll_timer,SIGNAL(timeout()),this,SLOT(on_poll()));
}

AppWorker::~AppWorker() {
}
void AppWorker::initialize_fixed_deps() {
  std::cerr << "AppWorker initialize_fixed_deps state\n";
  Component::initialize_fixed_deps();

  if (!_connected) {
    connect(_app_comm->get_web_socket(), SIGNAL(textMessageReceived(const QString &)), this, SLOT(on_text_received(const QString &)));
    _connected = true;
  }
}

QString AppWorker::get_app_bin_dir() {
  return QCoreApplication::applicationDirPath();
}

QString AppWorker::get_user_data_dir() {
  return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString AppWorker::get_smash_browse_url() {
  QString app_dir = get_app_bin_dir();
  app_dir += QString("/../html/smashbrowse.html");
  return app_dir;
}

bool AppWorker::is_polling() {
  return _poll_timer.isActive();
}

void AppWorker::start_polling() {
  _poll_timer.start();
}

void AppWorker::stop_polling() {
  _poll_timer.stop();
}

void AppWorker::reset_state() {
    // State for message queuing.
    _waiting_for_results = false;
    _next_msg_id = 0;
    _expected_msg_id = -1;
    _last_resp = Message();
    _queue.clear();

    // State for hovering.
    _hovering = false;
    _hover_args.clear();
    _jitter = kJitterSize;
}

void AppWorker::send_json(const QString& json) {
  Message msg(json);
  send_msg(msg);
}

void AppWorker::send_map(const QVariantMap& map) {
  Message msg(map);
  send_msg(msg);
}

void AppWorker::send_msg(Message& msg) {
  queue_task(std::bind(&AppWorker::send_msg_task, this, msg), "send_msg");
}

void AppWorker::send_msg_task(Message msg) {
  // Tag the request with an id. We expect a response with the same id.
  msg[Message::kID] = _next_msg_id;

  std::cerr << "app --> comhub: " << msg.to_string().toStdString() << "\n";

  // Increment the counter. Note we're ok with this overflowing and looping around.
  _next_msg_id += 1;

  // Send the request to nodejs.
  _waiting_for_results = true;
  size_t num_bytes = _app_comm->get_web_socket()->sendTextMessage(msg.to_string());
  assert(num_bytes);
}

void AppWorker::queue_task(AppTask task, const std::string& about) {
  _queue.push_back(task);
  std::cerr << "pushing: " << about << " queue is now of size: " << _queue.size() << "\n";

  // Check to see if we can run the next worker.
  bool ok_to_run = true;

  if (_queue.size() > 1) {
	  ok_to_run = false;
  }

  // Make sure we're not already waiting for results.
  // We only allow one request to be in-flight at a time.
  if (_waiting_for_results) {
    ok_to_run = false;
  }

  // Make sure nodejs is running.
  if (!_app_comm->nodejs_is_running()) {
    std::cerr << "Error: nodejs is not running.\n";
    ok_to_run = false;
  }

  // Make sure we're connected to nodejs.
  if (!_app_comm->nodejs_is_connected()) {
    std::cerr << "Error: nodejs is not connected.\n";
    ok_to_run = false;
  }

  // Run the worker if we're all clear to run.
  if (ok_to_run) {
	  std::cerr << "running task.\n";
    _queue[0]();
  }
}

void AppWorker::on_text_received(const QString & text) {

  std::cerr << "app_worker got text: " << text.toStdString() << "\n";

//  // Multiple message may arrive concatenated in the json string, so we split them.
//  QRegExp regex("(\\{[^\\{\\}]*\\})");
//  QStringList splits;
//  int pos = 0;
//  while ((pos = regex.indexIn(text, pos)) != -1) {
//      splits << regex.cap(1);
//      pos += regex.matchedLength();
//  }

  // Loop over each message string.
  //for (int i=0; i<splits.size(); ++i) {
    //std::cerr << "hub --> app-" << i << ": " << splits[i].toStdString() << "\n";
    //Message msg(splits[i]);
    Message msg(text);
    MessageType type = msg.get_msg_type();
    if (type == MessageType::kRequestMessage) {
      // We shouldn't be getting request message from nodejs.
      std::cerr << "Error: App should not be receiving request messages.\n";
      assert(false);
    } else if (type == MessageType::kResponseMessage) {
      handle_response_from_nodejs(msg);
    } else if (type == MessageType::kInfoMessage) {
      handle_info_from_nodejs(msg);
    } else {
      std::cerr << "Error: Got message of unknown type!\n";
    }
  //}
}

// -----------------------------------------------------------------
// Browser Actions.
// -----------------------------------------------------------------

void AppWorker::open_browser() {
  Message msg(RequestType::kOpenBrowser);

  QVariantMap args;
  args[Message::kURL] = get_smash_browse_url();

  msg[Message::kArgs] = args;
  queue_task(std::bind(&AppWorker::send_msg_task, this, msg), "open_browser");
}

void AppWorker::close_browser() {
  Message msg(RequestType::kCloseBrowser);
  queue_task(std::bind(&AppWorker::send_msg_task, this, msg), "close_browser");
}

void test() {
  std::cerr << "running test func!\n";
}

void AppWorker::check_browser_is_open() {
  Message msg(RequestType::kCheckBrowserIsOpen);
  queue_task(std::bind(&AppWorker::send_msg_task, this, msg), "check_browser_is_open");
}

void AppWorker::check_browser_size() {
  Message msg(RequestType::kResizeBrowser);

  int width = _file_model->get_work_setting(FileModel::kBrowserWidthRole).toInt();
  int height = _file_model->get_work_setting(FileModel::kBrowserHeightRole).toInt();

  QJsonObject args;
  args[Message::kWidth] = width;
  args[Message::kHeight] = height;

  msg[Message::kArgs] = args;
  queue_task(std::bind(&AppWorker::send_msg_task, this, msg), "check_browser_size");
}

void AppWorker::shutdown() {
  Message msg(RequestType::kShutdown);
  //queue_task(std::bind(&AppWorker::send_msg_task, this, msg));
  // We force the shutdown task to run right away.
  send_msg_task(msg);
  _app_comm->destroy_connection();
}

void AppWorker::reset() {
  reset_state();
  close_browser();
  check_browser_is_open();
}

// -----------------------------------------------------------------
// Test Features.
// -----------------------------------------------------------------

#define check_busy()   if (is_busy()) {emit web_action_ignored(); return;}

void AppWorker::get_all_cookies() {
  check_busy()
  Message msg(RequestType::kGetAllCookies);
  queue_task(std::bind(&AppWorker::send_msg_task, this, msg), "get_all_cookies");
}

void AppWorker::clear_all_cookies() {
  check_busy()
  Message msg(RequestType::kClearAllCookies);
  queue_task(std::bind(&AppWorker::send_msg_task, this, msg), "clear_all_cookies");
}

void AppWorker::set_all_cookies() {
  check_busy()
  Message msg(RequestType::kSetAllCookies);
  queue_task(std::bind(&AppWorker::send_msg_task, this, msg), "set_all_cookies");
}

void AppWorker::update_overlays() {
  check_busy()
  Message msg(RequestType::kUpdateOveralys);
  queue_task(std::bind(&AppWorker::send_msg_task, this, msg), "update_overlays");
}

// -----------------------------------------------------------------
// Navigation.
// -----------------------------------------------------------------

void AppWorker::navigate_to(const QString& url) {
  check_busy()

  // Fix the url.
  QString decorated_url = get_proper_url(url).toString();

  // Create the args.
  QVariantMap args;
  args[Message::kURL] = decorated_url;

  // Send the message.
  Message msg(RequestType::kNavigateTo, args);
  queue_task(std::bind(&AppWorker::send_msg_task, this, msg), "navigate_to");
}

void AppWorker::switch_to_iframe() {
  check_busy()
  QVariantMap args;
  args[Message::kIFrame] = _iframe;

  Message msg(RequestType::kSwitchIFrame, args);
  queue_task(std::bind(&AppWorker::send_msg_task, this, msg), "switch_to_iframe");
}

void AppWorker::navigate_refresh() {
  check_busy()
  // Send the message.
  Message msg(RequestType::kNavigateRefresh);
  queue_task(std::bind(&AppWorker::send_msg_task, this, msg), "navigate_refresh");
}

// -----------------------------------------------------------------
// Create set by matching values.
// -----------------------------------------------------------------

void AppWorker::create_set_by_matching_text() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "create_set_by_matching_text1");
  queue_task(std::bind(&AppWorker::create_set_by_matching_text_task,this), "create_set_by_matching_text2");
}

void AppWorker::create_set_by_matching_images() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "create_set_by_matching_images1");
  queue_task(std::bind(&AppWorker::create_set_by_matching_images_task,this), "create_set_by_matching_images2");
}

// -----------------------------------------------------------------
// Create set by type.
// -----------------------------------------------------------------

void AppWorker::create_set_of_inputs() {
  check_busy()
  QVariantMap args;
  args[Message::kWrapType] = WrapType::input;

  Message req(RequestType::kCreateSetFromWrapType);
  req[Message::kArgs] = args;

  queue_task(std::bind(&AppWorker::send_msg_task, this, req), "create_set_of_inputs");
}

void AppWorker::create_set_of_selects() {
  check_busy()
  QVariantMap args;
  args[Message::kWrapType] = WrapType::select;

  Message req(RequestType::kCreateSetFromWrapType);
  req[Message::kArgs] = args;

  queue_task(std::bind(&AppWorker::send_msg_task, this, req), "create_set_of_selects");
}

void AppWorker::create_set_of_images() {
  check_busy()
  QVariantMap args;
  args[Message::kWrapType] = WrapType::image;

  Message req(RequestType::kCreateSetFromWrapType);
  req[Message::kArgs] = args;

  queue_task(std::bind(&AppWorker::send_msg_task, this, req), "create_set_of_images");
}

void AppWorker::create_set_of_text() {
  check_busy()
  QVariantMap args;
  args[Message::kWrapType] = WrapType::text;

  Message req(RequestType::kCreateSetFromWrapType);
  req[Message::kArgs] = args;

  queue_task(std::bind(&AppWorker::send_msg_task, this, req), "create_set_of_text");
}

// -----------------------------------------------------------------
// Delete set.
// -----------------------------------------------------------------

void AppWorker::delete_set() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "delete_set1");
  queue_task(std::bind(&AppWorker::delete_set_task,this), "delete_set2");
}

// -----------------------------------------------------------------
// Shift sets
// -----------------------------------------------------------------

void AppWorker::shift_to_text_above() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_text_above1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::text, Direction::up>,this), "shift_to_text_above2");
}
void AppWorker::shift_to_text_below() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_text_below1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::text, Direction::down>,this), "shift_to_text_below2");
}
void AppWorker::shift_to_text_on_left() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_text_on_left1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::text, Direction::left>,this), "shift_to_text_on_left2");
}
void AppWorker::shift_to_text_on_right() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_text_on_right1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::text, Direction::right>,this), "shift_to_text_on_right2");
}

void AppWorker::shift_to_images_above() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_images_above1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::image, Direction::up>,this), "shift_to_images_above2");
}
void AppWorker::shift_to_images_below() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_images_below1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::image, Direction::down>,this), "shift_to_images_below2");
}
void AppWorker::shift_to_images_on_left() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_images_on_left1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::image, Direction::left>,this), "shift_to_images_on_left2");
}
void AppWorker::shift_to_images_on_right() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_images_on_right1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::image, Direction::right>,this), "shift_to_images_on_right2");
}

void AppWorker::shift_to_inputs_above() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_inputs_above1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::input, Direction::up>,this), "shift_to_inputs_above2");
}
void AppWorker::shift_to_inputs_below() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_inputs_below1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::input, Direction::down>,this), "shift_to_inputs_below2");
}
void AppWorker::shift_to_inputs_on_left() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_inputs_on_left1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::input, Direction::left>,this), "shift_to_inputs_on_left2");
}
void AppWorker::shift_to_inputs_on_right() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_inputs_on_right1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::input, Direction::right>,this), "shift_to_inputs_on_right2");
}

void AppWorker::shift_to_selects_above() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_selects_above1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::select, Direction::up>,this), "shift_to_selects_above2");
}
void AppWorker::shift_to_selects_below() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_selects_below1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::select, Direction::down>,this), "shift_to_selects_below2");
}
void AppWorker::shift_to_selects_on_left() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_selects_on_left1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::select, Direction::left>,this), "shift_to_selects_on_left2");
}
void AppWorker::shift_to_selects_on_right() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_selects_on_right1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::select, Direction::right>,this), "shift_to_selects_on_right2");
}

void AppWorker::shift_to_iframes_above() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_iframes_above1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::iframe, Direction::up>,this), "shift_to_iframes_above2");
}
void AppWorker::shift_to_iframes_below() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_iframes_below1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::iframe, Direction::down>,this), "shift_to_iframes_below2");
}
void AppWorker::shift_to_iframes_on_left() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_iframes_on_left1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::iframe, Direction::left>,this), "shift_to_iframes_on_left2");
}
void AppWorker::shift_to_iframes_on_right() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shift_to_iframes_on_right1");
  queue_task(std::bind(&AppWorker::shift_set_task<WrapType::iframe, Direction::right>,this), "shift_to_iframes_on_right2");
}

// -----------------------------------------------------------------
// Expand sets
// -----------------------------------------------------------------

void AppWorker::expand_above() {
  check_busy()
  QVariantMap match_criteria;
  match_criteria[Message::kMatchLeft] = true;
  match_criteria[Message::kMatchRight] = false;
  match_criteria[Message::kMatchTop] = false;
  match_criteria[Message::kMatchBottom] = false;
  match_criteria[Message::kMatchFont] = true;
  match_criteria[Message::kMatchFontSize] = true;
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "expand_above1");
  queue_task(std::bind(&AppWorker::expand_task,this, Direction::up, match_criteria), "expand_above2");
}

void AppWorker::expand_below() {
  check_busy()
  QVariantMap match_criteria;
  match_criteria[Message::kMatchLeft] = true;
  match_criteria[Message::kMatchRight] = false;
  match_criteria[Message::kMatchTop] = false;
  match_criteria[Message::kMatchBottom] = false;
  match_criteria[Message::kMatchFont] = true;
  match_criteria[Message::kMatchFontSize] = true;
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "expand_below1");
  queue_task(std::bind(&AppWorker::expand_task,this, Direction::down, match_criteria), "expand_below2");
}

void AppWorker::expand_left() {
  check_busy()
  QVariantMap match_criteria;
  match_criteria[Message::kMatchLeft] = false;
  match_criteria[Message::kMatchRight] = false;
  match_criteria[Message::kMatchTop] = true;
  match_criteria[Message::kMatchBottom] = false;
  match_criteria[Message::kMatchFont] = true;
  match_criteria[Message::kMatchFontSize] = true;
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "expand_left1");
  queue_task(std::bind(&AppWorker::expand_task,this, Direction::left, match_criteria), "expand_left2");
}

void AppWorker::expand_right() {
  check_busy()
  QVariantMap match_criteria;
  match_criteria[Message::kMatchLeft] = false;
  match_criteria[Message::kMatchRight] = false;
  match_criteria[Message::kMatchTop] = true;
  match_criteria[Message::kMatchBottom] = false;
  match_criteria[Message::kMatchFont] = true;
  match_criteria[Message::kMatchFontSize] = true;
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "expand_right1");
  queue_task(std::bind(&AppWorker::expand_task,this, Direction::right, match_criteria), "expand_right2");
}

// -----------------------------------------------------------------
// Mark sets
// -----------------------------------------------------------------

void AppWorker::mark_set() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "mark_set1");
  queue_task(std::bind(&AppWorker::mark_set_task,this), "mark_set2");
}

void AppWorker::unmark_set() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "unmark_set1");
  queue_task(std::bind(&AppWorker::unmark_set_task,this), "unmark_set2");
}

void AppWorker::merge_sets() {
  check_busy()
  Message req(RequestType::kMergeMarkedSets);
  queue_task(std::bind(&AppWorker::send_msg_task, this, req), "merge_sets");
}

// -----------------------------------------------------------------
// Shrink to one side.
// -----------------------------------------------------------------

void AppWorker::shrink_set_to_topmost() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shrink_set_to_topmost1");
  queue_task(std::bind(&AppWorker::shrink_set_to_side_task,this,Direction::up), "shrink_set_to_topmost2");
}
void AppWorker::shrink_set_to_bottommost() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shrink_set_to_bottommost1");
  queue_task(std::bind(&AppWorker::shrink_set_to_side_task,this,Direction::down), "shrink_set_to_bottommost2");
}
void AppWorker::shrink_set_to_leftmost() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shrink_set_to_leftmost1");
  queue_task(std::bind(&AppWorker::shrink_set_to_side_task,this,Direction::left), "shrink_set_to_leftmost2");
}
void AppWorker::shrink_set_to_rightmost() {
  check_busy()
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shrink_set_to_rightmost1");
  queue_task(std::bind(&AppWorker::shrink_set_to_side_task,this,Direction::right), "shrink_set_to_rightmost2");
}

void AppWorker::shrink_above_of_marked() {
  check_busy()
  QVariantList dirs;
  dirs.append(Direction::up);
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shrink_above_of_marked1");
  queue_task(std::bind(&AppWorker::shrink_against_marked_task,this,dirs), "shrink_above_of_marked2");
}
void AppWorker::shrink_below_of_marked() {
  check_busy()
  QVariantList dirs;
  dirs.append(Direction::down);
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shrink_below_of_marked1");
  queue_task(std::bind(&AppWorker::shrink_against_marked_task,this,dirs), "shrink_below_of_marked2");
}
void AppWorker::shrink_above_and_below_of_marked() {
  check_busy()
  QVariantList dirs;
  dirs.append(Direction::up);
  dirs.append(Direction::down);
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shrink_above_and_below_of_marked1");
  queue_task(std::bind(&AppWorker::shrink_against_marked_task,this,dirs), "shrink_above_and_below_of_marked2");
}
void AppWorker::shrink_left_of_marked() {
  check_busy()
  QVariantList dirs;
  dirs.append(Direction::left);
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shrink_left_of_marked1");
  queue_task(std::bind(&AppWorker::shrink_against_marked_task,this,dirs), "shrink_left_of_marked2");
}
void AppWorker::shrink_right_of_marked() {
  check_busy()
  QVariantList dirs;
  dirs.append(Direction::right);
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shrink_right_of_marked1");
  queue_task(std::bind(&AppWorker::shrink_against_marked_task,this,dirs), "shrink_right_of_marked2");
}
void AppWorker::shrink_left_and_right_of_marked() {
  check_busy()
  QVariantList dirs;
  dirs.append(Direction::left);
  dirs.append(Direction::right);
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "shrink_left_and_right_of_marked1");
  queue_task(std::bind(&AppWorker::shrink_against_marked_task,this,dirs), "shrink_left_and_right_of_marked2");
}

// -----------------------------------------------------------------
// Block/Unblock events in the browser content scripts.
// -----------------------------------------------------------------

void AppWorker::block_events() {
  Message req(RequestType::kBlockEvents);
  queue_task(std::bind(&AppWorker::send_msg_task,this,req), "block_events");
}

void AppWorker::unblock_events() {
  Message req(RequestType::kUnblockEvents);
  queue_task(std::bind(&AppWorker::send_msg_task,this,req), "unblock_events");
}

// -----------------------------------------------------------------
// Perform Web Actions.
// -----------------------------------------------------------------

void AppWorker::click() {
  check_busy()
  QVariantMap extra_args;

  unblock_events();
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "click1");
  queue_task(std::bind(&AppWorker::perform_action_task, this, ActionType::kSendClick, extra_args), "click2");
  block_events();
}
void AppWorker::mouse_over() {
  check_busy()
  QVariantMap extra_args;

  unblock_events();
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "mouse_over1");
  queue_task(std::bind(&AppWorker::perform_action_task,this, ActionType::kMouseOver, extra_args), "mouse_over2");
  block_events();
}
void AppWorker::start_mouse_hover() {
  check_busy()
  unblock_events();
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "start_mouse_hover1");
  queue_task(std::bind(&AppWorker::start_hover_task,this), "start_mouse_hover2");
  //queue_task(std::bind(&AppWorker::hover_task,this), "start_mouse_hover3");
  block_events();
}
void AppWorker::stop_mouse_hover() {
  check_busy()
  _hovering = false;
}
void AppWorker::type_text(const QString& text) {
  check_busy()
  QVariantMap extra_args;
  extra_args[Message::kText] = text;

  unblock_events();
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "type_text1");
  queue_task(std::bind(&AppWorker::perform_action_task,this,ActionType::kSendText,extra_args), "type_text2");
  block_events();
}
void AppWorker::type_enter() {
  check_busy()
  QVariantMap extra_args;

  unblock_events();
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "type_enter1");
  queue_task(std::bind(&AppWorker::perform_action_task,this,ActionType::kSendEnter,extra_args), "type_enter2");
  block_events();
}
void AppWorker::extract_text() {
  check_busy()
  QVariantMap extra_args;

  unblock_events();
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "extract_text1");
  queue_task(std::bind(&AppWorker::perform_action_task,this,ActionType::kGetText,extra_args), "extract_text2");
  block_events();
}
void AppWorker::get_option_texts() {
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "get_option_texts1");
  queue_task(std::bind(&AppWorker::emit_option_texts_task,this), "get_option_texts2");
}
void AppWorker::select_from_dropdown(const QString& option_text) {
  check_busy()
  QVariantMap extra_args;
  extra_args[Message::kOptionText] = option_text;

  unblock_events();
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "select_from_dropdown1");
  queue_task(std::bind(&AppWorker::perform_action_task,this,ActionType::kSelectOption,extra_args), "select_from_dropdown2");
  block_events();
}

void AppWorker::scroll_down() {
  check_busy()
  QVariantMap extra_args;

  unblock_events();
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "scroll_down1");
  queue_task(std::bind(&AppWorker::perform_action_task,this,ActionType::kScrollDown,extra_args), "scroll_down2");
  block_events();
}
void AppWorker::scroll_up() {
  check_busy()
  QVariantMap extra_args;

  unblock_events();
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "scroll_up1");
  queue_task(std::bind(&AppWorker::perform_action_task,this,ActionType::kScrollUp,extra_args), "scroll_up2");
  block_events();
}
void AppWorker::scroll_right() {
  check_busy()
  QVariantMap extra_args;

  unblock_events();
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "scroll_right1");
  queue_task(std::bind(&AppWorker::perform_action_task,this,ActionType::kScrollRight,extra_args), "scroll_right2");
  block_events();
}
void AppWorker::scroll_left() {
  check_busy()
  QVariantMap extra_args;

  unblock_events();
  queue_task(std::bind(&AppWorker::get_crosshair_info_task,this), "scroll_left1");
  queue_task(std::bind(&AppWorker::perform_action_task,this,ActionType::kScrollLeft,extra_args), "scroll_left2");
  block_events();
}


// -----------------------------------------------------------------
// Tasks. Our members which get bound into tasks.
// -----------------------------------------------------------------


void AppWorker::get_crosshair_info_task() {
  QVariantMap args;
  args[Message::kClickPos] = _click_pos;
  Message req(RequestType::kGetCrosshairInfo,args);
  send_msg_task(req);
}

void AppWorker::create_set_by_matching_text_task() {
  QVariantMap args;
  args[Message::kWrapType] = WrapType::text;
  args[Message::kMatchValues] = _last_resp[Message::kValue].toMap()[Message::kTextValues];

  Message req(RequestType::kCreateSetFromMatchValues);
  req[Message::kArgs] = args;

  send_msg_task(req);
}

void AppWorker::create_set_by_matching_images_task() {
  QVariantMap args;
  args[Message::kWrapType] = WrapType::image;
  args[Message::kMatchValues] = _last_resp[Message::kValue].toMap()[Message::kImageValues];

  Message req(RequestType::kCreateSetFromMatchValues);
  req[Message::kArgs] = args;

  send_msg_task(req);
}

void AppWorker::delete_set_task() {
  int set_index = _last_resp[Message::kValue].toMap()[Message::kSetIndex].toInt();
  if (set_index<0) {
    run_next_task();
    return;
  }

  QVariantMap args;
  args[Message::kSetIndex] = set_index;

  Message req(RequestType::kDeleteSet);
  req[Message::kArgs] = args;

  send_msg_task(req);
}

template <WrapType WRAP_TYPE, Direction DIR>
void AppWorker::shift_set_task() {
  int set_index = _last_resp[Message::kValue].toMap()[Message::kSetIndex].toInt();
  if (set_index < 0) {
    run_next_task();
    return;
  }

  QVariantMap args;
  args[Message::kSetIndex] = set_index;
  args[Message::kDirection] = DIR;
  args[Message::kWrapType] = WRAP_TYPE;

  Message req(RequestType::kShiftSet);
  req[Message::kArgs] = args;

  send_msg_task(req);
}

void AppWorker::expand_task(Direction dir, const QVariantMap& match_criteria) {
  int set_index = _last_resp[Message::kValue].toMap()[Message::kSetIndex].toInt();
  if (set_index < 0) {
    run_next_task();
    return;
  }

  QVariantMap args;
  args[Message::kSetIndex] = set_index;
  args[Message::kDirection] = dir;
  args[Message::kMatchCriteria] = match_criteria;

  Message req(RequestType::kExpandSet);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::mark_set_task() {
  int set_index = _last_resp[Message::kValue].toMap()[Message::kSetIndex].toInt();
  if (set_index < 0) {
    run_next_task();
    return;
  }

  QVariantMap args;
  args[Message::kSetIndex] = set_index;

  Message req(RequestType::kMarkSet);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::unmark_set_task() {
  int set_index = _last_resp[Message::kValue].toMap()[Message::kSetIndex].toInt();
  if (set_index < 0) {
    run_next_task();
    return;
  }

  QVariantMap args;
  args[Message::kSetIndex] = set_index;

  Message req(RequestType::kUnmarkSet);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::shrink_set_to_side_task(Direction dir) {
  int set_index = _last_resp[Message::kValue].toMap()[Message::kSetIndex].toInt();
  if (set_index < 0) {
    run_next_task();
    return;
  }

  QVariantMap args;
  args[Message::kSetIndex] = set_index;
  args[Message::kDirection] = dir;

  Message req(RequestType::kShrinkSet);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::shrink_against_marked_task(QVariantList dirs) {
  int set_index = _last_resp[Message::kValue].toMap()[Message::kSetIndex].toInt();
  if (set_index < 0) {
    run_next_task();
    return;
  }

  QVariantMap args;
  args[Message::kSetIndex] = set_index;
  args[Message::kDirections] = dirs;

  Message req(RequestType::kShrinkSetToMarked);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::start_hover_task() {
  std::cerr << "start hover task running\n";
  _hover_args = _last_resp[Message::kValue].toMap();
  _hovering = true;
  std::cerr << "running next task\n";
  run_next_task();
}

void AppWorker::hover_task() {
  // Jitter the hover position back and forth by one.
  int x = _hover_args[Message::kOverlayRelClickPos].toMap()["x"].toInt();
  int y = _hover_args[Message::kOverlayRelClickPos].toMap()["y"].toInt();
  x += _jitter;
  y += _jitter;
  _jitter *= -1;

  // Lock in the jitter.
  QVariantMap pos;
  pos["x"] = x;
  pos["y"] = y;
  _hover_args[Message::kOverlayRelClickPos] = pos;

  // Queue the tasks.
  unblock_events();
  queue_task(std::bind(&AppWorker::perform_action_task,this, ActionType::kMouseOver, _hover_args), "hover_task");
  queue_task(std::bind(&AppWorker::post_hover_task, this), "post_hover_task");
  block_events();
}

void AppWorker::post_hover_task() {
  // Stop hovering if the last hover fails.
  // This happens if the element we're hovering over disappears or webdriver switches to another iframe.
  bool success = _last_resp[Message::kSuccess].toBool();
  if (!success) {
    _hovering = false;
  }
  run_next_task();
}

void AppWorker::perform_action_task(ActionType action, QVariantMap extra_args) {
  int set_index = _last_resp[Message::kValue].toMap()[Message::kSetIndex].toInt();
  if (set_index < 0) {
    run_next_task();
    return;
  }

  int overlay_index = _last_resp[Message::kValue].toMap()[Message::kOverlayIndex].toInt();
  QString xpath = _last_resp[Message::kValue].toMap()[Message::kXPath].toString();

  QVariantMap args;
  args[Message::kSetIndex] = set_index;
  args[Message::kOverlayIndex] = overlay_index;
  args[Message::kXPath] = xpath;
  args[Message::kAction] = action;

  // Get other settings from the last response.
  if (action == kSendClick || action == kMouseOver) {
    args[Message::kOverlayRelClickPos] = _last_resp[Message::kValue].toMap()[Message::kOverlayRelClickPos];
  }

  // Merge the extra args. Note these actually override the above key value pairs.
  QVariantMap::iterator iter;
  for (iter = extra_args.begin(); iter != extra_args.end(); ++iter) {
    args[iter.key()] = iter.value();
  }


  Message req(RequestType::kPerformAction);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::emit_option_texts_task() {
  QStringList ot = _last_resp[Message::kValue].toMap()[Message::kOptionTexts].toStringList();
  emit select_option_texts(ot);
  run_next_task();
}

// -----------------------------------------------------------------
// Handle Messages.
// -----------------------------------------------------------------

void AppWorker::run_next_task() {
  // Unblock the scheduler.
  _waiting_for_results = false;

  // Otherwise we got the expected response id.
  // Pop the queue.
  if (_queue.size()) {
    _queue.pop_front();
  }

  // Call the next handler.
  if (_queue.size()) {
    _queue[0]();
  }
}

void AppWorker::handle_response_from_nodejs(const Message& msg) {

  // Todo: Sometime we will get duplicate messages. Try to determine the cause.
  // If we get the same message (which include msg.id), ignore it.
  if (msg == _last_resp) {
    return;
  }

  // Get the response id. This is supposed to match with the request id.
  int resp_id = msg[Message::kID].toInt();

  // Determine the request id.
  int req_id = _next_msg_id -1;

  // Check the ids.
  if (resp_id != req_id) {
    std::cerr << "Error: response id: " << resp_id << " did not match request id: " << req_id << "\n";
  } else {
    std::cerr << "Success: response id: " << resp_id << " matches request id: " << req_id << "\n";
  }

  assert(resp_id == req_id);

  // Update the last message.
  _last_resp = msg;

  // Run the next task.
  run_next_task();
}

void AppWorker::handle_info_from_nodejs(const Message& msg) {
  std::cerr << "commhub --> app: info: " << msg.to_string().toStdString() << "\n";
  if (msg[Message::kInfo] == InfoType::kShowWebActionMenu) {
    _click_pos = msg[Message::kValue].toMap()[Message::kClickPos].toMap();
    std::cerr << "got click x,y: " << _click_pos["x"].toInt() << ", " << _click_pos["y"].toInt() << "\n";

    _iframe = msg[Message::kIFrame].toString();
    if (msg[Message::kValue].toMap().count(Message::kPrevIFrame)) {
      QString prev_iframe = msg[Message::kValue].toMap().value(Message::kPrevIFrame).toString();
      emit show_iframe_menu();
    } else {
      emit show_web_action_menu();
    }
  } else {
      std::cerr << "comm->app: received info: " << msg.to_string().toStdString() << "\n";
  }
}

void AppWorker::on_poll() {
  if (_app_comm->check_connection()) {
    if (_show_browser) {
      std::cerr << "polling open browser!\n";
      check_browser_is_open();
      check_browser_size();
      // Debugging. - this makes the browser only come up once.
      _show_browser = false;
    }
  }
  if (_hovering) {
    if (_queue.empty()) {
      hover_task();
    }
  }
}

}
