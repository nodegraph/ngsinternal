#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/message.h>

#include <guicomponents/computes/firebasecomputes.h>

#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <base/objectmodel/basefactory.h>
#include <components/computes/inputcompute.h>
#include <components/computes/jsonutils.h>

#include <guicomponents/computes/nodejsworker.h>
#include <guicomponents/comms/messagesender.h>

#include <iostream>
#include <sstream>

#include <QtCore/QCoreApplication>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

namespace ngs {

const int NodeJSWorker::kPollInterval = 1000;
const int NodeJSWorker::kWaitInterval = 2000;

NodeJSWorker::NodeJSWorker(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _msg_sender(this),
      _scheduler(this),
      _manipulator(this)

{
  get_dep_loader()->register_fixed_dep(_msg_sender, Path());
  get_dep_loader()->register_fixed_dep(_scheduler, Path());
  get_dep_loader()->register_fixed_dep(_manipulator, Path());

  // Setup the poll timer.
  _poll_timer.setSingleShot(false);
  _poll_timer.setInterval(kPollInterval);
  connect(&_poll_timer,SIGNAL(timeout()),this,SLOT(on_poll()));

  // Setup the wait timer.
  _wait_timer.setSingleShot(false);
  _wait_timer.setInterval(kWaitInterval);
  connect(&_wait_timer,SIGNAL(timeout()),this,SLOT(on_done_wait()));
}

NodeJSWorker::~NodeJSWorker() {
}

void NodeJSWorker::open() {
  external();
  _msg_sender->open();

  // The logic to always hover over the current element has not been fully implemented.
  //_poll_timer.start();
}

void NodeJSWorker::close() {
  external();
  _msg_sender->close();

  // The logic to always hover over the current element has not been fully implemented.
  //_poll_timer.stop();
}

bool NodeJSWorker::is_open() {
  external();
  return _msg_sender->is_open();
}

void NodeJSWorker::open_browser() {
  TaskContext tc(_scheduler);
  queue_open_browser(tc);
}

void NodeJSWorker::close_browser() {
  TaskContext tc(_scheduler);
  queue_close_browser(tc);
}

void NodeJSWorker::force_close_browser() {
  _scheduler->force_stack_reset();
  {
    // Make sure the browser is closed.
    TaskContext tc(_scheduler);
    queue_close_browser(tc);
  }

  // Wait for the tasks to fully flush out.
  while (_scheduler->is_busy()) {
    qApp->processEvents();
  }
}

void NodeJSWorker::force_stack_reset() {
  _manipulator->clear_ultimate_targets();
  _scheduler->force_stack_reset();
}

bool NodeJSWorker::is_busy_cleaning() {
  return _manipulator->is_busy_cleaning();
}

void NodeJSWorker::queue_emit_option_texts() {
  TaskContext tc(_scheduler);
  queue_emit_option_texts(tc);
}

void NodeJSWorker::firebase_init(const QString& api_key, const QString& auth_domain, const QString& database_url, const QString& storage_bucket) {
  TaskContext tc(_scheduler);
  QJsonObject args;
  args.insert(Message::kApiKey, api_key);
  args.insert(Message::kAuthDomain, auth_domain);
  args.insert(Message::kDatabaseURL, database_url);
  args.insert(Message::kStorageBucket, storage_bucket);
  queue_merge_chain_state(tc, args);
  queue_firebase_init(tc);
}

void NodeJSWorker::firebase_sign_in(const QString& email, const QString& password) {
  TaskContext tc(_scheduler);
  QJsonObject args;
  args.insert(Message::kEmail, email);
  args.insert(Message::kPassword, password);
  queue_merge_chain_state(tc, args);
  queue_firebase_sign_in(tc);
}

bool NodeJSWorker::is_polling() {
  return _poll_timer.isActive();
}

void NodeJSWorker::start_polling() {
  _poll_timer.start();
}

void NodeJSWorker::stop_polling() {
  _poll_timer.stop();
}

void NodeJSWorker::reset_state() {
    // State for message queuing.
    _chain_state = QJsonObject();
}

// -----------------------------------------------------------------
// Our Slots.
// -----------------------------------------------------------------

void NodeJSWorker::on_poll() {
    if (!_scheduler->is_busy()) {
      TaskContext tc(_scheduler);
      queue_perform_mouse_hover(tc);
    }
}

void NodeJSWorker::on_done_wait() {
  _wait_timer.stop();
  _scheduler->run_next_task();
  //_manipulator->continue_cleaning_to_ultimate_targets_on_idle();
}

// ---------------------------------------------------------------------------------
// Socket Messaging Tasks.
// ---------------------------------------------------------------------------------

void NodeJSWorker::send_msg_task(Message& msg) {
  int id = _scheduler->wait_for_response();
  msg.insert(Message::kID, id);
  _msg_sender->send_msg(msg);

  std::cerr << "sending app --> commhub: " << msg.to_string().toStdString() << "\n";
}

// ---------------------------------------------------------------------------------
// Queue Element Tasks.
// ---------------------------------------------------------------------------------

void NodeJSWorker::queue_get_current_element(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::get_current_element_info,this), "queue_get_current_element");
}

void NodeJSWorker::queue_has_current_element(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::has_current_element_info,this), "queue_has_current_element");
}

void NodeJSWorker::queue_scroll_element_into_view(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::scroll_element_into_view_task,this), "queue_scroll_element_into_view");
}

void NodeJSWorker::queue_get_crosshair_info(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::get_crosshair_info_task,this), "queue_get_crosshair_info");
}

void NodeJSWorker::queue_get_element_values(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::get_element_values_task,this), "queue_get_element_values");
}

void NodeJSWorker::queue_get_drop_down_info(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::get_drop_down_info_task,this), "queue_get_crosshair_info");
}

// ---------------------------------------------------------------------------------
// Queue Framework Tasks.
// ---------------------------------------------------------------------------------

void NodeJSWorker::queue_merge_chain_state(TaskContext& tc, const QJsonObject& map) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::merge_chain_state_task,this, map), "queue_merge_chain_state");
}

void NodeJSWorker::queue_copy_chain_property(TaskContext& tc, const QString& src_prop, const QString& dest_prop) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::copy_chain_property_task,this, src_prop, dest_prop), "queue_merge_chain_state");
}

void NodeJSWorker::queue_determine_angle_in_degress(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::determine_angle_in_degrees_task,this), "queue_determine_angle_in_degress");
}

void NodeJSWorker::queue_build_compute_node(TaskContext& tc, ComponentDID compute_did) {
  std::stringstream ss;
  ss << "queue build compute node with did: " << (size_t)compute_did;
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::build_compute_node_task,this, compute_did), ss.str());
}

void NodeJSWorker::queue_receive_chain_state(TaskContext& tc, std::function<void(const QJsonObject&)> receive_chain_state) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::receive_chain_state_task,this,receive_chain_state), "queue_get_outputs");
}

void NodeJSWorker::queue_reset(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::reset_task, this), "reset");
}

// ---------------------------------------------------------------------------------
// Queue Cookie Tasks.
// ---------------------------------------------------------------------------------

void NodeJSWorker::queue_get_all_cookies(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::get_all_cookies_task, this), "queue_get_all_cookies");
}

void NodeJSWorker::queue_clear_all_cookies(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::clear_all_cookies_task, this), "queue_clear_all_cookies");
}

void NodeJSWorker::queue_set_all_cookies(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::set_all_cookies_task, this), "queue_set_all_cookies");
}

// ---------------------------------------------------------------------------------
// Queue Browser Tasks.
// ---------------------------------------------------------------------------------

void NodeJSWorker::queue_open_browser(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::open_browser_task,this), "queue_open_browser");
}

void NodeJSWorker::queue_close_browser(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::close_browser_task,this), "queue_close_browser");
}

void NodeJSWorker::queue_is_browser_open(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::is_browser_open_task,this), "queue_check_browser_is_open");
}

void NodeJSWorker::queue_resize_browser(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::resize_browser_task,this), "queue_resize_browser");
}

void NodeJSWorker::queue_get_browser_title(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::get_browser_title_task,this), "queue_resize_browser");
}

void NodeJSWorker::queue_update_current_tab(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::update_current_tab_task,this), "queue_update_current_tab");
}

void NodeJSWorker::queue_destroy_current_tab(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::destroy_current_tab_task,this), "queue_destroy_current_tab");
}

void NodeJSWorker::queue_open_tab(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::open_tab_task,this), "queue_open_tab");
}

void NodeJSWorker::queue_download_files(TaskContext& tc) {
  queue_open_tab(tc);
  queue_update_current_tab(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::download_files_task,this), "queue_download_files");
  queue_wait(tc); // We need to wait a little for the Save-As dialog to come up.
  {
    QJsonObject args;
    args.insert(Message::kApplicationName, "chrome");
    args.insert(Message::kWindowTitle, "Save As");
    args.insert(Message::kAllMatchingWindows, false);

    QJsonArray keys;
    keys.push_back("enter");

    args.insert(Message::kKeys, keys);
    queue_merge_chain_state(tc, args);
  }
  // Now the chain_state has all the values to do our tap at the os level.
  queue_tap_keys_in_os(tc);

  // Code to deal with "Confirm Save As" dialogs when a file with the same name exists.
  // We don't currently need this as the chrome extension which does the download
  // adds a number in parentheses at the end.

  {
    QJsonObject args;
    args.insert(Message::kApplicationName, "chrome");
    args.insert(Message::kWindowTitle, "Save As");
    args.insert(Message::kAllMatchingWindows, false);

    QJsonArray keys;
    keys.push_back("tab");
    keys.push_back("enter");

    args.insert(Message::kKeys, keys);
    queue_merge_chain_state(tc, args);
  }
  // Now the chain_state has all the values to do our tap at the os level.
  queue_tap_keys_in_os(tc);
}

// ---------------------------------------------------------------------------------
// OS Level Tasks.
// ---------------------------------------------------------------------------------

void NodeJSWorker::queue_get_active_window_in_os(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::get_active_window_in_os_task,this), "queue_get_active_window_in_os");
}

void NodeJSWorker::queue_tap_keys_in_os(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::tap_keys_in_os_task,this), "queue_tap_enter_in_os");
}

// ---------------------------------------------------------------------------------
// Queue Page Content Tasks.
// ---------------------------------------------------------------------------------

void NodeJSWorker::queue_block_events(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::block_events_task, this), "queue_block_events");
}

void NodeJSWorker::queue_unblock_events(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::unblock_events_task, this), "queue_unblock_events");
}

void NodeJSWorker::queue_wait_until_loaded(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::wait_until_loaded_task, this), "queue_wait_until_loaded");
}

void NodeJSWorker::queue_wait(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::wait_task, this), "queue_wait");
}


// ---------------------------------------------------------------------------------
// Queue Navigate Tasks.
// ---------------------------------------------------------------------------------

void NodeJSWorker::queue_navigate_to(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::navigate_to_task,this), "queue_navigate_to");
}

void NodeJSWorker::queue_navigate_back(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::navigate_back_task,this), "queue_navigate_refresh");
}

void NodeJSWorker::queue_navigate_forward(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::navigate_forward_task,this), "queue_navigate_refresh");
}

void NodeJSWorker::queue_navigate_refresh(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::navigate_refresh_task,this), "queue_navigate_refresh");
}

void NodeJSWorker::queue_get_current_url(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::get_current_url_task,this), "queue_navigate_refresh");
}

// ---------------------------------------------------------------------------------
// Queue Set Tasks.
// ---------------------------------------------------------------------------------

void NodeJSWorker::queue_update_element(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::update_element_task, this), "queue_update_overlays");
}

void NodeJSWorker::queue_clear_element(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::clear_element_task, this), "queue_clear_element");
}

void NodeJSWorker::queue_find_element_by_position(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::find_element_by_position_task,this), "queue_find_element_by_values");
}

void NodeJSWorker::queue_find_element_by_values(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::find_element_by_values_task,this), "queue_find_element_by_values");
}

void NodeJSWorker::queue_find_element_by_type(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::find_element_by_type_task,this), "queue_find_element_by_type");
}

void NodeJSWorker::queue_shift_element_by_type(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::shift_element_by_type_task,this), "queue_shift_element_by_type");
}

void NodeJSWorker::queue_shift_element_by_values(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::shift_element_by_values_task,this), "queue_shift_element_by_values");
}

// ---------------------------------------------------------------------------------
// Queue Perform Action Tasks.
// ---------------------------------------------------------------------------------

void NodeJSWorker::queue_perform_mouse_action(TaskContext& tc) {
  queue_get_current_element(tc);

  queue_unblock_events(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::perform_hover_action_task,this), "perform_hover_action_task");
  queue_block_events(tc); // After we're done interacting with the page, block events on the page.
  queue_wait_until_loaded(tc); // Our actions may have triggered asynchronous content loading in the page, so we wait for the page to be ready.

  // Force wait so that the webpage can react to the mouse hover. Note this is real and makes the mouse click work 100% of the time.
  queue_wait(tc);

  queue_unblock_events(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::perform_mouse_action_task,this), "queue_perform_action_task");
  queue_block_events(tc); // After we're done interacting with the page, block events on the page.
  queue_wait_until_loaded(tc); // Our actions may have triggered asynchronous content loading in the page, so we wait for the page to be ready.

  queue_update_element(tc); // Our actions may have moved elements arounds, so we update our overlays.

  // Force wait so that the webpage can react to the mouse action. Note this is real and makes the mouse click work 100% of the time.
  queue_wait(tc);
}

void NodeJSWorker::queue_perform_mouse_hover(TaskContext& tc) {
  queue_has_current_element(tc);
  queue_unblock_events(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::perform_hover_action_task,this), "queue_perform_hover");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_element(tc);
}

void NodeJSWorker::queue_perform_text_action(TaskContext& tc) {
  queue_get_current_element(tc);
  queue_unblock_events(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::perform_text_action_task,this), "queue_perform_text_action");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_element(tc);
}

void NodeJSWorker::queue_perform_element_action(TaskContext& tc) {
  queue_get_current_element(tc);
  queue_unblock_events(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::perform_element_action_task,this), "queue_perform_element_action");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_element(tc);
}

// ---------------------------------------------------------------------------------
// Queue other actions.
// ---------------------------------------------------------------------------------

void NodeJSWorker::queue_emit_option_texts(TaskContext& tc) {
  queue_get_drop_down_info(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::emit_option_texts_task,this), "queue_emit_option_texts");
}

// ---------------------------------------------------------------------------------
// Queue firebase actions.
// ---------------------------------------------------------------------------------

void NodeJSWorker::queue_firebase_init(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::firebase_init_task,this), "queue_firebase_init");
}

void NodeJSWorker::queue_firebase_destroy(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::firebase_destroy_task,this), "queue_firebase_destroy");
}

void NodeJSWorker::queue_firebase_sign_in(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::firebase_sign_in_task,this), "queue_firebase_sign_in");
}

void NodeJSWorker::queue_firebase_sign_out(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::firebase_sign_out_task,this), "queue_firebase_sign_out");
}

void NodeJSWorker::queue_firebase_write_data(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::firebase_write_data_task,this), "queue_firebase_write_data");
}

void NodeJSWorker::queue_firebase_read_data(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::firebase_read_data_task,this), "queue_firebase_read_data");
}

void NodeJSWorker::queue_firebase_subscribe(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::firebase_subscribe_task,this), "queue_firebase_subscribe");
}

void NodeJSWorker::queue_firebase_unsubscribe(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&NodeJSWorker::firebase_unsubscribe_task,this), "queue_firebase_unsubscribe");
}

// ------------------------------------------------------------------------
// Handle Incoming Messages.
// ------------------------------------------------------------------------

void NodeJSWorker::handle_response(const Message& msg) {
  // Update the last message.
  _last_response = msg;

  QJsonValue value = msg.value(Message::kValue);

  if (value.isObject()) {
    // Merge the values into the chain_state.
    QJsonObject obj = value.toObject();
    JSONUtils::shallow_object_merge(_chain_state, obj);
  } else if (!value.isUndefined()) {
    _chain_state.insert("value", value);
  }

  // -----------------------------------------------------
  // Update our scheduler.
  // -----------------------------------------------------

  // Get the response id. This is supposed to match with the request id.
  int resp_id = msg.value(Message::kID).toInt();

  // Check for any errors.
  QString error;
  if (!msg.value(Message::kSuccess).toBool()) {
    error = msg.value(Message::kValue).toString();
    if (error.isEmpty()) {
      error = "An error has occured.";
    }
  }

  // Notify the scheduler that we're done waiting for a response.
  _scheduler->done_waiting_for_response(resp_id, error);
}

void NodeJSWorker::handle_info(const Message& msg) {
  std::cerr << "commhub --> app: info: " << msg.to_string().toStdString() << "\n";
  int info_type = msg.value(Message::kInfo).toInt();
  if (info_type == to_underlying(InfoType::kShowWebActionMenu)) {
    _global_mouse_pos = msg.value(Message::kValue).toObject().value(Message::kGlobalMousePosition).toObject();
    emit show_web_action_menu();
  } else if (info_type == to_underlying(InfoType::kFirebaseChanged)) {
    if (!msg.value(Message::kValue).isObject()) {
      return;
    }
    QJsonObject obj = msg.value(Message::kValue).toObject();
    QString data_path = obj.value(Message::kDataPath).toString();
    QJsonValue value = obj.value(Message::kValue);
    QString node_path_str = obj.value(Message::kNodePath).toString();
    Path node_path = Path::string_to_path(node_path_str.toStdString());
    _manipulator->set_firebase_override(node_path, data_path, value);
  } else {
      std::cerr << "comm->app: received info: " << msg.to_string().toStdString() << "\n";
  }
}

// ------------------------------------------------------------------------
// Element Tasks.
// ------------------------------------------------------------------------

void NodeJSWorker::get_crosshair_info_task() {
  QJsonObject args;
  args.insert(Message::kGlobalMousePosition, _global_mouse_pos);
  Message req(RequestType::kGetCrosshairInfo,args);
  send_msg_task(req);
}

void NodeJSWorker::get_element_values_task() {
  Message req(RequestType::kGetElementValues);
  send_msg_task(req);
}

void NodeJSWorker::get_drop_down_info_task() {
  Message req(RequestType::kGetDropDownInfo);
  send_msg_task(req);
}

// Should be run after a response message like get_crosshair_info_task that has set_index and overlay_index.
void NodeJSWorker::get_current_element_info() {
  QJsonObject args;
  Message req(RequestType::kGetElement,args);
  send_msg_task(req);
}

void NodeJSWorker::has_current_element_info() {
  QJsonObject args;
  Message req(RequestType::kHasElement,args);
  send_msg_task(req);
}

void NodeJSWorker::scroll_element_into_view_task() {
  QJsonObject args;
  Message req(RequestType::kScrollElementIntoView,args);
  send_msg_task(req);
}

// ------------------------------------------------------------------------
// Infrastructure Tasks.
// ------------------------------------------------------------------------

void NodeJSWorker::merge_chain_state_task(const QJsonObject& map) {
  // Merge the values into the chain_state.
  JSONUtils::shallow_object_merge(_chain_state, map);
  _scheduler->run_next_task();
}

void NodeJSWorker::copy_chain_property_task(const QString& src_prop, const QString& dest_prop) {
  _chain_state.insert(dest_prop, _chain_state[src_prop]);
  _scheduler->run_next_task();
}

void NodeJSWorker::determine_angle_in_degrees_task() {
  QJsonObject box = _chain_state["box"].toObject();
  double left = box["left"].toDouble();
  double right = box["right"].toDouble();
  double bottom = box["bottom"].toDouble();
  double top = box["top"].toDouble();
  double center_x = (left + right) / 2.0;
  double center_y = (bottom + top) / 2.0;

  QJsonObject p = _chain_state["global_mouse_position"].toObject();
  double global_x = p["x"].toDouble();
  double global_y = p["y"].toDouble();

  double diff_x = global_x - center_x;
  double diff_y = global_y - center_y;

  double theta = atan2(diff_y, diff_x);
  double angle_in_degrees = -1 * theta / 3.141592653 * 180.0; // -1 is because y increases from top to bottom, and we want users to think that 0 degress is to the right, and 90 degress is up.

  std::cerr << "---------------------------------------------------------------\n";
  std::cerr << "Box center is: " << center_x << "," << center_y << "\n";
  std::cerr << "The click pos: " << global_x << "," << global_y << "\n";
  std::cerr << "The angle in degrees is: " << angle_in_degrees << "\n";

  _chain_state.insert(Message::kAngleInDegrees, angle_in_degrees);
  _scheduler->run_next_task();
}

void NodeJSWorker::receive_chain_state_task(std::function<void(const QJsonObject&)> receive_chain_state) {
  receive_chain_state(_chain_state);
  _scheduler->run_next_task();
}

void NodeJSWorker::build_compute_node_task(ComponentDID compute_did) {
  std::cerr << "building compute node!!\n";
  Entity* node = _manipulator->create_browser_node(true, compute_did, _chain_state);
  _manipulator->link_to_closest_node(node);
  _manipulator->set_ultimate_targets(node, false);
}

void NodeJSWorker::reset_task() {
  reset_state();
  TaskContext tc(_scheduler);
  queue_open_browser(tc);
}


// ------------------------------------------------------------------------
// Cookie Tasks.
// ------------------------------------------------------------------------

void NodeJSWorker::get_all_cookies_task() {
  Message req(RequestType::kGetAllCookies);
  send_msg_task(req);
}

void NodeJSWorker::clear_all_cookies_task() {
  Message req(RequestType::kClearAllCookies);
  send_msg_task(req);
}

void NodeJSWorker::set_all_cookies_task() {
  Message req(RequestType::kSetAllCookies);
  send_msg_task(req);
}


// ------------------------------------------------------------------------
// Browser Tasks.
// ------------------------------------------------------------------------

void NodeJSWorker::is_browser_open_task() {
  Message req(RequestType::kIsBrowserOpen);
  send_msg_task(req);
}

void NodeJSWorker::close_browser_task() {
  Message req(RequestType::kCloseBrowser);
  send_msg_task(req);
}

void NodeJSWorker::open_browser_task() {
  Message req(RequestType::kOpenBrowser);
  send_msg_task(req);
}

void NodeJSWorker::resize_browser_task() {
  QJsonObject args;
  args.insert(Message::kWidth,_chain_state.value(Message::kWidth));
  args.insert(Message::kHeight, _chain_state.value(Message::kHeight));

  Message req(RequestType::kResizeBrowser);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void NodeJSWorker::get_browser_title_task() {
  Message req(RequestType::kGetBrowserTitle);
  send_msg_task(req);
}

void NodeJSWorker::update_current_tab_task() {
  Message req(RequestType::kUpdateCurrentTab);
  send_msg_task(req);
}

void NodeJSWorker::destroy_current_tab_task() {
  Message req(RequestType::kDestroyCurrentTab);
  send_msg_task(req);
}

void NodeJSWorker::open_tab_task() {
  Message req(RequestType::kOpenTab);
  send_msg_task(req);
}

void NodeJSWorker::download_files_task() {
  Message req(RequestType::kDownloadFiles);
  send_msg_task(req);
}

// ------------------------------------------------------------------------
// OS Level Tasks.
// ------------------------------------------------------------------------

void NodeJSWorker::get_active_window_in_os_task() {
  Message req(RequestType::kGetActiveWindowInOS);
  send_msg_task(req);
}

void NodeJSWorker::tap_keys_in_os_task() {
  QJsonObject args;
  args.insert(Message::kApplicationName, _chain_state.value(Message::kApplicationName));
  args.insert(Message::kWindowTitle, _chain_state.value(Message::kWindowTitle));
  args.insert(Message::kAllMatchingWindows, _chain_state.value(Message::kAllMatchingWindows));
  args.insert(Message::kKeys, _chain_state.value(Message::kKeys));

  Message req(RequestType::kTapKeysInOS);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

// ------------------------------------------------------------------------
// Page Content Tasks.
// ------------------------------------------------------------------------

void NodeJSWorker::block_events_task() {
  // This action implies the browser's event unblocked for a time to allow
  // some actions to be performed. After such an action we need to update
  // the overlays as elements may disappear or move around.
  Message req(RequestType::kBlockEvents);
  send_msg_task(req);
}

void NodeJSWorker::unblock_events_task() {
  Message req(RequestType::kUnblockEvents);
  send_msg_task(req);
}

void NodeJSWorker::wait_until_loaded_task() {
  Message req(RequestType::kWaitUntilLoaded);
  send_msg_task(req);
}

void NodeJSWorker::wait_task() {
  _wait_timer.start(kWaitInterval);
}

// ------------------------------------------------------------------------
// Browser Reset and Shutdown Tasks.
// ------------------------------------------------------------------------

void NodeJSWorker::shutdown_task() {
  Message msg(RequestType::kShutdown);
  // Shutdown without queuing it.
  send_msg_task(msg);
  close();
}

void NodeJSWorker::reset_browser_task() {
  // Close the browser without queuing it.
  close_browser_task();
  // Queue the reset now that all the queued task have been destroyed.
  TaskContext tc(_scheduler);
  queue_reset(tc);
}

// ------------------------------------------------------------------------
// Navigation Tasks.
// ------------------------------------------------------------------------

void NodeJSWorker::navigate_to_task() {
  QJsonObject args;
  args.insert(Message::kURL, _chain_state.value(Message::kURL));
  Message req(RequestType::kNavigateTo, args);
  send_msg_task(req);
}

void NodeJSWorker::navigate_back_task() {
  Message req(RequestType::kNavigateBack);
  send_msg_task(req);
}

void NodeJSWorker::navigate_forward_task() {
  Message req(RequestType::kNavigateForward);
  send_msg_task(req);
}

void NodeJSWorker::navigate_refresh_task() {
  Message req(RequestType::kNavigateRefresh);
  send_msg_task(req);
}

void NodeJSWorker::get_current_url_task() {
  Message req(RequestType::kGetCurrentURL);
  send_msg_task(req);
}

// ------------------------------------------------------------------------
// Set Creation/Modification Tasks.
// ------------------------------------------------------------------------

void NodeJSWorker::update_element_task() {
  Message req(RequestType::kUpdateElement);
  send_msg_task(req);
}

void NodeJSWorker::clear_element_task() {
  Message req(RequestType::kClearElement);
  send_msg_task(req);
}

void NodeJSWorker::find_element_by_position_task() {
  QJsonObject args;
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));
  args.insert(Message::kGlobalMousePosition, _chain_state.value(Message::kGlobalMousePosition));

  Message req(RequestType::kFindElementByPosition);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void NodeJSWorker::find_element_by_values_task() {
  QJsonObject args;
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));
  args.insert(Message::kTargetValues, _chain_state.value(Message::kTargetValues));

  Message req(RequestType::kFindElementByValues);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void NodeJSWorker::find_element_by_type_task() {
  QJsonObject args;
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));

  Message req(RequestType::kFindElementByType);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void NodeJSWorker::shift_element_by_type_task() {
  QJsonObject args;
  args.insert(Message::kAngleInDegrees, _chain_state.value(Message::kAngleInDegrees));
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));
  Message req(RequestType::kShiftElementByType);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void NodeJSWorker::shift_element_by_values_task() {
  QJsonObject args;
  args.insert(Message::kAngleInDegrees, _chain_state.value(Message::kAngleInDegrees));
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));
  args.insert(Message::kTargetValues, _chain_state.value(Message::kTargetValues));
  Message req(RequestType::kShiftElementByValues);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void NodeJSWorker::perform_mouse_action_task() {
  QJsonObject args;
  args.insert(Message::kFrameIndexPath, _chain_state.value(Message::kFrameIndexPath));
  args.insert(Message::kXPath, _chain_state.value(Message::kXPath));

  args.insert(Message::kMouseAction, _chain_state.value(Message::kMouseAction));
  args.insert(Message::kLocalMousePosition, _chain_state.value(Message::kLocalMousePosition));

  Message req(RequestType::kPerformMouseAction);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void NodeJSWorker::perform_hover_action_task() {
  QJsonObject args;
  args.insert(Message::kFrameIndexPath, _chain_state.value(Message::kFrameIndexPath));
  args.insert(Message::kXPath, _chain_state.value(Message::kXPath));

  args.insert(Message::kMouseAction, to_underlying(MouseActionType::kMouseOver));
  args.insert(Message::kLocalMousePosition, _chain_state.value(Message::kLocalMousePosition));

  Message req(RequestType::kPerformMouseAction);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void NodeJSWorker::perform_text_action_task() {
  QJsonObject args;
  args.insert(Message::kFrameIndexPath, _chain_state.value(Message::kFrameIndexPath));
  args.insert(Message::kXPath, _chain_state.value(Message::kXPath));

  args.insert(Message::kTextAction, _chain_state.value(Message::kTextAction));
  args.insert(Message::kText, _chain_state.value(Message::kText));

  Message req(RequestType::kPerformTextAction);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void NodeJSWorker::perform_element_action_task() {
  QJsonObject args;
  args.insert(Message::kFrameIndexPath, _chain_state.value(Message::kFrameIndexPath));
  args.insert(Message::kXPath, _chain_state.value(Message::kXPath));

  args.insert(Message::kElementAction, _chain_state.value(Message::kElementAction));
  args.insert(Message::kOptionText, _chain_state.value(Message::kOptionText)); // Used for selecting element from dropdowns.
  args.insert(Message::kScrollDirection, _chain_state.value(Message::kScrollDirection)); // Used for the scrolling directions.

  Message req(RequestType::kPerformElementAction);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

//void NodeJSWorker::mouse_hover_task() {
//  // Queue the tasks.
//  TaskContext tc(_scheduler);
//  queue_unblock_events(tc);
//  queue_perform_mouse_hover(tc);
//  queue_block_events(tc);
//  queue_wait_until_loaded(tc);
//  queue_update_element(tc);
//}

void NodeJSWorker::emit_option_texts_task() {
  QJsonArray vals = _chain_state.value(Message::kOptionTexts).toArray();
  QStringList options;
  for (QJsonArray::const_iterator iter = vals.constBegin(); iter != vals.constEnd(); ++iter) {
    options.push_back(iter->toString());
  }
  emit select_option_texts(options);
  _scheduler->run_next_task();
}

void NodeJSWorker::firebase_init_task() {
  QJsonObject args;
  args.insert(Message::kApiKey, _chain_state.value(Message::kApiKey));
  args.insert(Message::kAuthDomain, _chain_state.value(Message::kAuthDomain));
  args.insert(Message::kDatabaseURL, _chain_state.value(Message::kDatabaseURL));
  args.insert(Message::kStorageBucket, _chain_state.value(Message::kStorageBucket));
  args.insert(Message::kNodePath, _chain_state.value(Message::kNodePath));

  Message req(RequestType::kFirebaseInit);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void NodeJSWorker::firebase_destroy_task() {
  QJsonObject args;
  args.insert(Message::kNodePath, _chain_state.value(Message::kNodePath));

  Message req(RequestType::kFirebaseDestroy);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void NodeJSWorker::firebase_sign_in_task() {
  QJsonObject args;
  args.insert(Message::kEmail, _chain_state.value(Message::kEmail));
  args.insert(Message::kPassword, _chain_state.value(Message::kPassword));
  args.insert(Message::kNodePath, _chain_state.value(Message::kNodePath));

  Message req(RequestType::kFirebaseSignIn);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void NodeJSWorker::firebase_sign_out_task() {
  QJsonObject args;
  args.insert(Message::kNodePath, _chain_state.value(Message::kNodePath));

  Message req(RequestType::kFirebaseSignOut);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void NodeJSWorker::firebase_write_data_task() {
  QJsonObject args;
  args.insert(Message::kDataPath, _chain_state.value(Message::kDataPath));
  args.insert(Message::kValue, _chain_state.value(Message::kValue));
  args.insert(Message::kNodePath, _chain_state.value(Message::kNodePath));

  Message req(RequestType::kFirebaseWriteData);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void NodeJSWorker::firebase_read_data_task() {
  QJsonObject args;
  args.insert(Message::kDataPath, _chain_state.value(Message::kDataPath));
  args.insert(Message::kNodePath, _chain_state.value(Message::kNodePath));

  Message req(RequestType::kFirebaseReadData);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void NodeJSWorker::firebase_subscribe_task() {
  QJsonObject args;
  args.insert(Message::kDataPath, _chain_state.value(Message::kDataPath));
  args.insert(Message::kNodePath, _chain_state.value(Message::kNodePath));

  Message req(RequestType::kFirebaseSubscribe);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void NodeJSWorker::firebase_unsubscribe_task() {
  QJsonObject args;
  args.insert(Message::kDataPath, _chain_state.value(Message::kDataPath));
  args.insert(Message::kNodePath, _chain_state.value(Message::kNodePath));

  Message req(RequestType::kFirebaseUnsubscribe);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

}
