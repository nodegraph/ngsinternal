#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/appconfig.h>

#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/message.h>

#include <guicomponents/computes/firebasecomputes.h>

#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <base/objectmodel/basefactory.h>
#include <components/computes/inputcompute.h>
#include <components/computes/jsonutils.h>

#include <guicomponents/computes/messagesender.h>
#include <guicomponents/computes/taskqueuer.h>
#include <guicomponents/computes/javaprocess.h>

#include <iostream>
#include <sstream>

#include <QtCore/QCoreApplication>
#include <QtCore/QUrl>
#include <QtCore/QDebug>
#include <QtCore/QDir>

namespace ngs {

const int TaskQueuer::kPollInterval = 1000;
const int TaskQueuer::kWaitInterval = 2000;

TaskQueuer::TaskQueuer(Entity* parent)
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

TaskQueuer::~TaskQueuer() {
}

void TaskQueuer::open() {
  external();
  _msg_sender->open();

  // The logic to always hover over the current element has not been fully implemented.
  //_poll_timer.start();
}

void TaskQueuer::close() {
  external();
  _msg_sender->close();

  // The logic to always hover over the current element has not been fully implemented.
  //_poll_timer.stop();
}

//bool NodeJSWorker::is_open() {
//  external();
//  return _msg_sender->is_open();
//}

void TaskQueuer::open_browser() {
  TaskContext tc(_scheduler);
  queue_open_browser(tc);
}

void TaskQueuer::close_browser() {
  TaskContext tc(_scheduler);
  queue_close_browser(tc);
}

void TaskQueuer::force_close_browser() {
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

void TaskQueuer::force_stack_reset() {
  _manipulator->clear_ultimate_targets();
  _scheduler->force_stack_reset();
}

bool TaskQueuer::is_busy_cleaning() {
  return _manipulator->is_busy_cleaning();
}

bool TaskQueuer::current_task_is_cancelable() {
  return _manipulator->current_task_is_cancelable();
}

bool TaskQueuer::is_waiting_for_response() {
  return _scheduler->is_waiting_for_response();
}

void TaskQueuer::queue_stop_service() {
  TaskContext tc(_scheduler);
  queue_stop_service(tc);
}

void TaskQueuer::queue_emit_option_texts() {
  TaskContext tc(_scheduler);
  queue_emit_option_texts(tc);
}

void TaskQueuer::firebase_init(const QString& api_key, const QString& auth_domain, const QString& database_url, const QString& storage_bucket) {
  TaskContext tc(_scheduler);
  QJsonObject args;
  args.insert(Message::kApiKey, api_key);
  args.insert(Message::kAuthDomain, auth_domain);
  args.insert(Message::kDatabaseURL, database_url);
  args.insert(Message::kStorageBucket, storage_bucket);
  queue_merge_chain_state(tc, args);
  queue_firebase_init(tc);
}

void TaskQueuer::firebase_sign_in(const QString& email, const QString& password) {
  TaskContext tc(_scheduler);
  QJsonObject args;
  args.insert(Message::kEmail, email);
  args.insert(Message::kPassword, password);
  queue_merge_chain_state(tc, args);
  queue_firebase_sign_in(tc);
}

bool TaskQueuer::is_polling() {
  return _poll_timer.isActive();
}

void TaskQueuer::start_polling() {
  _poll_timer.start();
}

void TaskQueuer::stop_polling() {
  _poll_timer.stop();
}

void TaskQueuer::reset_state() {
    // State for message queuing.
    _chain_state = QJsonObject();
}

// -----------------------------------------------------------------
// Our Slots.
// -----------------------------------------------------------------

void TaskQueuer::on_poll() {
    if (!_scheduler->is_busy()) {
      TaskContext tc(_scheduler);
      queue_perform_mouse_hover(tc);
    }
}

void TaskQueuer::on_done_wait() {
  _wait_timer.stop();
  _scheduler->run_next_task();
  //_manipulator->continue_cleaning_to_ultimate_targets_on_idle();
}

// ---------------------------------------------------------------------------------
// Socket Messaging Tasks.
// ---------------------------------------------------------------------------------

void TaskQueuer::send_msg_task(Message& msg) {
  int id = _scheduler->wait_for_response();
  msg.insert(Message::kID, id);
  _msg_sender->send_msg(msg);
  std::cerr << "sending app --> commhub: " << msg.to_string().toStdString() << "\n";
}

// ---------------------------------------------------------------------------------
// Queue Element Tasks.
// ---------------------------------------------------------------------------------

void TaskQueuer::queue_get_current_element(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::get_current_element_info,this), "queue_get_current_element");
}

void TaskQueuer::queue_has_current_element(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::has_current_element_info,this), "queue_has_current_element");
}

void TaskQueuer::queue_scroll_element_into_view(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::scroll_element_into_view_task,this), "queue_scroll_element_into_view");
}

void TaskQueuer::queue_get_crosshair_info(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::get_crosshair_info_task,this), "queue_get_crosshair_info");
}

void TaskQueuer::queue_get_element_values(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::get_element_values_task,this), "queue_get_element_values");
}

void TaskQueuer::queue_get_drop_down_info(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::get_drop_down_info_task,this), "queue_get_crosshair_info");
}

// ---------------------------------------------------------------------------------
// Queue Framework Tasks.
// ---------------------------------------------------------------------------------

void TaskQueuer::queue_merge_chain_state(TaskContext& tc, const QJsonObject& map) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::merge_chain_state_task,this, map), "queue_merge_chain_state");
}

void TaskQueuer::queue_copy_chain_property(TaskContext& tc, const QString& src_prop, const QString& dest_prop) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::copy_chain_property_task,this, src_prop, dest_prop), "queue_merge_chain_state");
}

void TaskQueuer::queue_determine_angle_in_degress(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::determine_angle_in_degrees_task,this), "queue_determine_angle_in_degress");
}

void TaskQueuer::queue_build_compute_node(TaskContext& tc, ComponentDID compute_did) {
  std::stringstream ss;
  ss << "queue build compute node with did: " << (size_t)compute_did;
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::build_compute_node_task,this, compute_did), ss.str());
}

void TaskQueuer::queue_receive_chain_state(TaskContext& tc, std::function<void(const QJsonObject&)> receive_chain_state) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::receive_chain_state_task,this,receive_chain_state), "queue_get_outputs");
}

void TaskQueuer::queue_reset(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::reset_task, this), "reset");
}

// ---------------------------------------------------------------------------------
// Queue Cookie Tasks.
// ---------------------------------------------------------------------------------

void TaskQueuer::queue_get_all_cookies(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::get_all_cookies_task, this), "queue_get_all_cookies");
}

void TaskQueuer::queue_clear_all_cookies(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::clear_all_cookies_task, this), "queue_clear_all_cookies");
}

void TaskQueuer::queue_set_all_cookies(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::set_all_cookies_task, this), "queue_set_all_cookies");
}

// ---------------------------------------------------------------------------------
// Web Driver Service Tasks.
// ---------------------------------------------------------------------------------

void TaskQueuer::queue_start_service(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::start_service_task,this), "queue_start_service");
}

void TaskQueuer::queue_stop_service(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::stop_service_task,this), "queue_stop_service");
}

// ---------------------------------------------------------------------------------
// Queue Browser Tasks.
// ---------------------------------------------------------------------------------

void TaskQueuer::queue_wait_for_chrome_connection(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::wait_for_chrome_connection_task,this), "queue_wait_for_chrome_connection");
}

void TaskQueuer::queue_open_browser(TaskContext& tc) {
  _scheduler->queue_task(tc, Task(std::bind(&TaskQueuer::open_browser_task,this), false), "queue_open_browser");
  queue_wait_for_chrome_connection(tc);
}

void TaskQueuer::queue_close_browser(TaskContext& tc) {
  _scheduler->queue_task(tc, Task(std::bind(&TaskQueuer::close_browser_task,this), false), "queue_close_browser");
}

void TaskQueuer::queue_release_browser(TaskContext& tc) {
  _scheduler->queue_task(tc, Task(std::bind(&TaskQueuer::release_browser_task,this), false), "queue_close_browser");
}

void TaskQueuer::queue_is_browser_open(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::is_browser_open_task,this), "queue_check_browser_is_open");
}

void TaskQueuer::queue_resize_browser(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::resize_browser_task,this), "queue_resize_browser");
}

void TaskQueuer::queue_get_active_tab_title(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::get_active_tab_title_task,this), "queue_resize_browser");
}

void TaskQueuer::queue_update_current_tab(TaskContext& tc) {
  queue_update_current_tab_in_browser_controller(tc);
  queue_update_current_tab_in_chrome_extension(tc);
}

void TaskQueuer::queue_update_current_tab_in_browser_controller(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::update_current_tab_in_browser_controller_task,this), "queue_update_current_tab");
}

void TaskQueuer::queue_update_current_tab_in_chrome_extension(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::update_current_tab_in_chrome_extension_task,this), "queue_update_current_tab");
}

void TaskQueuer::queue_destroy_current_tab(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::destroy_current_tab_task,this), "queue_destroy_current_tab");
}

void TaskQueuer::queue_open_tab(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::open_tab_task,this), "queue_open_tab");
}

void TaskQueuer::queue_download_video(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::download_video_task,this), "queue_download_files");
}

void TaskQueuer::queue_accept_save_dialog(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::accept_save_dialog_task,this), "queue_accept_save_dialog");
}

// ---------------------------------------------------------------------------------
// Queue Page Content Tasks.
// ---------------------------------------------------------------------------------

void TaskQueuer::queue_block_events(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::block_events_task, this), "queue_block_events");
}

void TaskQueuer::queue_unblock_events(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::unblock_events_task, this), "queue_unblock_events");
}

void TaskQueuer::queue_wait_until_loaded(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::wait_until_loaded_task, this), "queue_wait_until_loaded");
}

void TaskQueuer::queue_wait(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::wait_task, this), "queue_wait");
}


// ---------------------------------------------------------------------------------
// Queue Navigate Tasks.
// ---------------------------------------------------------------------------------

void TaskQueuer::queue_navigate_to(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::navigate_to_task,this), "queue_navigate_to");
}

void TaskQueuer::queue_navigate_back(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::navigate_back_task,this), "queue_navigate_refresh");
}

void TaskQueuer::queue_navigate_forward(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::navigate_forward_task,this), "queue_navigate_refresh");
}

void TaskQueuer::queue_navigate_refresh(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::navigate_refresh_task,this), "queue_navigate_refresh");
}

void TaskQueuer::queue_get_current_url(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::get_current_url_task,this), "queue_navigate_refresh");
}

// ---------------------------------------------------------------------------------
// Queue Set Tasks.
// ---------------------------------------------------------------------------------

void TaskQueuer::queue_update_element(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::update_element_task, this), "queue_update_overlays");
}

void TaskQueuer::queue_clear_element(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::clear_element_task, this), "queue_clear_element");
}

void TaskQueuer::queue_find_element_by_position(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::find_element_by_position_task,this), "queue_find_element_by_values");
}

void TaskQueuer::queue_find_element_by_values(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::find_element_by_values_task,this), "queue_find_element_by_values");
}

void TaskQueuer::queue_find_element_by_type(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::find_element_by_type_task,this), "queue_find_element_by_type");
}

void TaskQueuer::queue_shift_element_by_type(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::shift_element_by_type_task,this), "queue_shift_element_by_type");
}

void TaskQueuer::queue_shift_element_by_values(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::shift_element_by_values_task,this), "queue_shift_element_by_values");
}

void TaskQueuer::queue_shift_element_by_type_along_rows(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::shift_element_by_type_along_rows_task,this), "queue_shift_element_by_type_along_rows");
}

void TaskQueuer::queue_shift_element_by_values_along_rows(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::shift_element_by_values_along_rows_task,this), "queue_shift_element_by_values_along_rows");
}

// ---------------------------------------------------------------------------------
// Queue Perform Action Tasks.
// ---------------------------------------------------------------------------------

void TaskQueuer::queue_perform_mouse_action(TaskContext& tc) {
  queue_get_current_element(tc);

  queue_unblock_events(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::perform_hover_action_task,this), "perform_hover_action_task");
  queue_block_events(tc); // After we're done interacting with the page, block events on the page.
  queue_wait_until_loaded(tc); // Our actions may have triggered asynchronous content loading in the page, so we wait for the page to be ready.

  // Force wait so that the webpage can react to the mouse hover. Note this is real and makes the mouse click work 100% of the time.
  queue_wait(tc);

  queue_unblock_events(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::perform_mouse_action_task,this), "queue_perform_action_task");
  queue_block_events(tc); // After we're done interacting with the page, block events on the page.
  queue_wait_until_loaded(tc); // Our actions may have triggered asynchronous content loading in the page, so we wait for the page to be ready.

  queue_update_element(tc); // Our actions may have moved elements arounds, so we update our overlays.

  // Force wait so that the webpage can react to the mouse action. Note this is real and makes the mouse click work 100% of the time.
  queue_wait(tc);
}

void TaskQueuer::queue_perform_mouse_hover(TaskContext& tc) {
  queue_has_current_element(tc);
  queue_unblock_events(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::perform_hover_action_task,this), "queue_perform_hover");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_element(tc);
}

void TaskQueuer::queue_perform_text_action(TaskContext& tc) {
  queue_get_current_element(tc);
  queue_unblock_events(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::perform_text_action_task,this), "queue_perform_text_action");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_element(tc);
}

void TaskQueuer::queue_perform_element_action(TaskContext& tc) {
  queue_get_current_element(tc);
  queue_unblock_events(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::perform_element_action_task,this), "queue_perform_element_action");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_element(tc);
}

// ---------------------------------------------------------------------------------
// Queue other actions.
// ---------------------------------------------------------------------------------

void TaskQueuer::queue_emit_option_texts(TaskContext& tc) {
  queue_get_drop_down_info(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::emit_option_texts_task,this), "queue_emit_option_texts");
}

// ---------------------------------------------------------------------------------
// Queue firebase actions.
// ---------------------------------------------------------------------------------

void TaskQueuer::queue_firebase_init(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::firebase_init_task,this), "queue_firebase_init");
}

void TaskQueuer::queue_firebase_destroy(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::firebase_destroy_task,this), "queue_firebase_destroy");
}

void TaskQueuer::queue_firebase_sign_in(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::firebase_sign_in_task,this), "queue_firebase_sign_in");
}

void TaskQueuer::queue_firebase_sign_out(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::firebase_sign_out_task,this), "queue_firebase_sign_out");
}

void TaskQueuer::queue_firebase_write_data(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::firebase_write_data_task,this), "queue_firebase_write_data");
}

void TaskQueuer::queue_firebase_read_data(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::firebase_read_data_task,this), "queue_firebase_read_data");
}

void TaskQueuer::queue_firebase_subscribe(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::firebase_subscribe_task,this), "queue_firebase_subscribe");
}

void TaskQueuer::queue_firebase_unsubscribe(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::firebase_unsubscribe_task,this), "queue_firebase_unsubscribe");
}

// ------------------------------------------------------------------------
// Handle Incoming Messages.
// ------------------------------------------------------------------------

void TaskQueuer::handle_response(const Message& msg) {
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

  // When the success value of a ResponseMessage is false,
  // an error will be shown in the gui and all processing will stop.
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

void TaskQueuer::handle_info(const Message& msg) {
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

void TaskQueuer::get_crosshair_info_task() {
  QJsonObject args;
  args.insert(Message::kGlobalMousePosition, _global_mouse_pos);
  Message req(ChromeRequestType::kGetCrosshairInfo,args);
  send_msg_task(req);
}

void TaskQueuer::get_element_values_task() {
  Message req(ChromeRequestType::kGetElementValues);
  send_msg_task(req);
}

void TaskQueuer::get_drop_down_info_task() {
  Message req(ChromeRequestType::kGetDropDownInfo);
  send_msg_task(req);
}

void TaskQueuer::get_current_element_info() {
  QJsonObject args;
  Message req(ChromeRequestType::kGetElement,args);
  send_msg_task(req);
}

void TaskQueuer::has_current_element_info() {
  QJsonObject args;
  Message req(ChromeRequestType::kHasElement,args);
  send_msg_task(req);
}

void TaskQueuer::scroll_element_into_view_task() {
  QJsonObject args;
  Message req(ChromeRequestType::kScrollElementIntoView,args);
  send_msg_task(req);
}

// ------------------------------------------------------------------------
// Infrastructure Tasks.
// ------------------------------------------------------------------------

void TaskQueuer::merge_chain_state_task(const QJsonObject& map) {
  // Merge the values into the chain_state.
  JSONUtils::shallow_object_merge(_chain_state, map);
  _scheduler->run_next_task();
}

void TaskQueuer::copy_chain_property_task(const QString& src_prop, const QString& dest_prop) {
  _chain_state.insert(dest_prop, _chain_state[src_prop]);
  _scheduler->run_next_task();
}

void TaskQueuer::determine_angle_in_degrees_task() {
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
  double degrees = -1 * theta / 3.141592653 * 180.0; // -1 is because y increases from top to bottom, and we want users to think that 0 degress is to the right, and 90 degress is up.

  std::cerr << "---------------------------------------------------------------\n";
  std::cerr << "Box center is: " << center_x << "," << center_y << "\n";
  std::cerr << "The click pos: " << global_x << "," << global_y << "\n";
  std::cerr << "The angle in degrees is: " << degrees << "\n";

  _chain_state.insert(Message::kAngle, degrees);
  _scheduler->run_next_task();
}

void TaskQueuer::receive_chain_state_task(std::function<void(const QJsonObject&)> receive_chain_state) {
  receive_chain_state(_chain_state);
  _scheduler->run_next_task();
}

void TaskQueuer::build_compute_node_task(ComponentDID compute_did) {
  std::cerr << "building compute node!!\n";
  Entity* node = _manipulator->create_browser_node(true, compute_did, _chain_state);
  _manipulator->link_to_closest_node(node);
  _manipulator->set_ultimate_targets(node, false);
}

void TaskQueuer::reset_task() {
  reset_state();
  TaskContext tc(_scheduler);
  queue_open_browser(tc);
}


// ------------------------------------------------------------------------
// Cookie Tasks.
// ------------------------------------------------------------------------

void TaskQueuer::get_all_cookies_task() {
  Message req(ChromeRequestType::kGetAllCookies);
  send_msg_task(req);
}

void TaskQueuer::clear_all_cookies_task() {
  Message req(ChromeRequestType::kClearAllCookies);
  send_msg_task(req);
}

void TaskQueuer::set_all_cookies_task() {
  Message req(ChromeRequestType::kSetAllCookies);
  send_msg_task(req);
}


// ------------------------------------------------------------------------
// Browser Tasks.
// ------------------------------------------------------------------------

void TaskQueuer::is_browser_open_task() {
  Message req(WebDriverRequestType::kIsBrowserOpen);
  send_msg_task(req);
}

void TaskQueuer::close_browser_task() {
  Message req(WebDriverRequestType::kCloseBrowser);
  send_msg_task(req);
}

void TaskQueuer::release_browser_task() {
  Message req(WebDriverRequestType::kReleaseBrowser);
  send_msg_task(req);
  _msg_sender->clear_web_socket();
}

void TaskQueuer::start_service_task() {
  Message req(WebDriverRequestType::kStartService);
  send_msg_task(req);
}

void TaskQueuer::stop_service_task() {
  Message req(WebDriverRequestType::kStopService);
  send_msg_task(req);
}

void TaskQueuer::wait_for_chrome_connection_task() {
  _msg_sender->wait_for_chrome_connection();
  _scheduler->run_next_task();
}

void TaskQueuer::open_browser_task() {
  Message req(WebDriverRequestType::kOpenBrowser);
  send_msg_task(req);
}

void TaskQueuer::resize_browser_task() {
  QJsonObject args;
  args.insert(Message::kWidth,_chain_state.value(Message::kWidth));
  args.insert(Message::kHeight, _chain_state.value(Message::kHeight));

  Message req(WebDriverRequestType::kResizeBrowser);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::get_active_tab_title_task() {
  Message req(ChromeRequestType::kGetActiveTabTitle);
  send_msg_task(req);
}

void TaskQueuer::update_current_tab_in_browser_controller_task() {
  Message req(WebDriverRequestType::kUpdateCurrentTab);
  send_msg_task(req);
}

void TaskQueuer::update_current_tab_in_chrome_extension_task() {
  Message req(ChromeRequestType::kUpdateCurrentTab);
  send_msg_task(req);
}

void TaskQueuer::destroy_current_tab_task() {
  Message req(WebDriverRequestType::kDestroyCurrentTab);
  send_msg_task(req);
}

void TaskQueuer::open_tab_task() {
  Message req(ChromeRequestType::kOpenTab);
  send_msg_task(req);
}

void TaskQueuer::download_video_task() {
  QJsonObject args;
  // Note this task uses the URL property even though it is not a paremeter
  // on the DownloadVideoCompute node. This is so that we can always set it
  // to the current URL.
  args.insert(Message::kURL, _chain_state.value(Message::kURL));
  args.insert(Message::kDirectory, _chain_state.value(Message::kDirectory));
  args.insert(Message::kMaxWidth, _chain_state.value(Message::kMaxWidth));
  args.insert(Message::kMaxHeight, _chain_state.value(Message::kMaxHeight));
  args.insert(Message::kMaxFilesize, _chain_state.value(Message::kMaxFilesize));

  Message req(PlatformRequestType::kDownloadVideo, args);
  send_msg_task(req);
}

void TaskQueuer::accept_save_dialog_task() {
  Message req(PlatformRequestType::kAcceptSaveDialog);
  send_msg_task(req);
}

// ------------------------------------------------------------------------
// Page Content Tasks.
// ------------------------------------------------------------------------

void TaskQueuer::block_events_task() {
  // This action implies the browser's event unblocked for a time to allow
  // some actions to be performed. After such an action we need to update
  // the overlays as elements may disappear or move around.
  Message req(ChromeRequestType::kBlockEvents);
  send_msg_task(req);
}

void TaskQueuer::unblock_events_task() {
  Message req(ChromeRequestType::kUnblockEvents);
  send_msg_task(req);
}

void TaskQueuer::wait_until_loaded_task() {
  Message req(ChromeRequestType::kWaitUntilLoaded);
  send_msg_task(req);
}

void TaskQueuer::wait_task() {
  _wait_timer.start(kWaitInterval);
}

// ------------------------------------------------------------------------
// Browser Reset and Shutdown Tasks.
// ------------------------------------------------------------------------

void TaskQueuer::reset_browser_task() {
  // Close the browser without queuing it.
  close_browser_task();
  // Queue the reset now that all the queued task have been destroyed.
  TaskContext tc(_scheduler);
  queue_reset(tc);
}

// ------------------------------------------------------------------------
// Navigation Tasks.
// ------------------------------------------------------------------------

void TaskQueuer::navigate_to_task() {
  QJsonObject args;
  args.insert(Message::kURL, _chain_state.value(Message::kURL));
  Message req(WebDriverRequestType::kNavigateTo, args);
  send_msg_task(req);
}

void TaskQueuer::navigate_back_task() {
  Message req(WebDriverRequestType::kNavigateBack);
  send_msg_task(req);
}

void TaskQueuer::navigate_forward_task() {
  Message req(WebDriverRequestType::kNavigateForward);
  send_msg_task(req);
}

void TaskQueuer::navigate_refresh_task() {
  Message req(WebDriverRequestType::kNavigateRefresh);
  send_msg_task(req);
}

void TaskQueuer::get_current_url_task() {
  Message req(WebDriverRequestType::kGetCurrentURL);
  send_msg_task(req);
}

// ------------------------------------------------------------------------
// Set Creation/Modification Tasks.
// ------------------------------------------------------------------------

void TaskQueuer::update_element_task() {
  Message req(ChromeRequestType::kUpdateElement);
  send_msg_task(req);
}

void TaskQueuer::clear_element_task() {
  Message req(ChromeRequestType::kClearElement);
  send_msg_task(req);
}

void TaskQueuer::find_element_by_position_task() {
  QJsonObject args;
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));
  args.insert(Message::kGlobalMousePosition, _chain_state.value(Message::kGlobalMousePosition));

  Message req(ChromeRequestType::kFindElementByPosition);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::find_element_by_values_task() {
  QJsonObject args;
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));
  args.insert(Message::kTargetValues, _chain_state.value(Message::kTargetValues));

  Message req(ChromeRequestType::kFindElementByValues);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::find_element_by_type_task() {
  QJsonObject args;
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));

  Message req(ChromeRequestType::kFindElementByType);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::shift_element_by_type_task() {
  QJsonObject args;
  args.insert(Message::kAngle, _chain_state.value(Message::kAngle));
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));
  args.insert(Message::kMaxWidthDifference, _chain_state.value(Message::kMaxWidthDifference));
  args.insert(Message::kMaxHeightDifference, _chain_state.value(Message::kMaxHeightDifference));
  args.insert(Message::kMaxAngleDifference, _chain_state.value(Message::kMaxAngleDifference));
  Message req(ChromeRequestType::kShiftElementByType);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::shift_element_by_values_task() {
  QJsonObject args;
  args.insert(Message::kAngle, _chain_state.value(Message::kAngle));
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));
  args.insert(Message::kTargetValues, _chain_state.value(Message::kTargetValues));
  args.insert(Message::kMaxWidthDifference, _chain_state.value(Message::kMaxWidthDifference));
  args.insert(Message::kMaxHeightDifference, _chain_state.value(Message::kMaxHeightDifference));
  args.insert(Message::kMaxAngleDifference, _chain_state.value(Message::kMaxAngleDifference));
  Message req(ChromeRequestType::kShiftElementByValues);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::shift_element_by_type_along_rows_task() {
  QJsonObject args;
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));
  args.insert(Message::kMaxWidthDifference, _chain_state.value(Message::kMaxWidthDifference));
  args.insert(Message::kMaxHeightDifference, _chain_state.value(Message::kMaxHeightDifference));
  args.insert(Message::kMaxAngleDifference, _chain_state.value(Message::kMaxAngleDifference));
  Message req(ChromeRequestType::kShiftElementByTypeAlongRows);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::shift_element_by_values_along_rows_task() {
  QJsonObject args;
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));
  args.insert(Message::kTargetValues, _chain_state.value(Message::kTargetValues));
  args.insert(Message::kMaxWidthDifference, _chain_state.value(Message::kMaxWidthDifference));
  args.insert(Message::kMaxHeightDifference, _chain_state.value(Message::kMaxHeightDifference));
  args.insert(Message::kMaxAngleDifference, _chain_state.value(Message::kMaxAngleDifference));
  Message req(ChromeRequestType::kShiftElementByValuesAlongRows);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::perform_mouse_action_task() {
  QJsonObject args;
  args.insert(Message::kFrameIndexPath, _chain_state.value(Message::kFrameIndexPath));
  args.insert(Message::kXPath, _chain_state.value(Message::kXPath));

  args.insert(Message::kMouseAction, _chain_state.value(Message::kMouseAction));
  args.insert(Message::kLocalMousePosition, _chain_state.value(Message::kLocalMousePosition));

  Message req(WebDriverRequestType::kPerformMouseAction);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::perform_hover_action_task() {
  QJsonObject args;
  args.insert(Message::kFrameIndexPath, _chain_state.value(Message::kFrameIndexPath));
  args.insert(Message::kXPath, _chain_state.value(Message::kXPath));

  args.insert(Message::kMouseAction, to_underlying(MouseActionType::kMouseOver));
  args.insert(Message::kLocalMousePosition, _chain_state.value(Message::kLocalMousePosition));

  Message req(WebDriverRequestType::kPerformMouseAction);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::perform_text_action_task() {
  QJsonObject args;
  args.insert(Message::kFrameIndexPath, _chain_state.value(Message::kFrameIndexPath));
  args.insert(Message::kXPath, _chain_state.value(Message::kXPath));

  args.insert(Message::kTextAction, _chain_state.value(Message::kTextAction));
  args.insert(Message::kText, _chain_state.value(Message::kText));

  Message req(WebDriverRequestType::kPerformTextAction);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::perform_element_action_task() {
  QJsonObject args;
  args.insert(Message::kFrameIndexPath, _chain_state.value(Message::kFrameIndexPath));
  args.insert(Message::kXPath, _chain_state.value(Message::kXPath));

  args.insert(Message::kElementAction, _chain_state.value(Message::kElementAction));
  args.insert(Message::kOptionText, _chain_state.value(Message::kOptionText)); // Used for selecting element from dropdowns.
  args.insert(Message::kScrollDirection, _chain_state.value(Message::kScrollDirection)); // Used for the scrolling directions.

  Message req(WebDriverRequestType::kPerformElementAction);
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

void TaskQueuer::emit_option_texts_task() {
  QJsonArray vals = _chain_state.value(Message::kOptionTexts).toArray();
  QStringList options;
  for (QJsonArray::const_iterator iter = vals.constBegin(); iter != vals.constEnd(); ++iter) {
    options.push_back(iter->toString());
  }
  emit select_option_texts(options);
  _scheduler->run_next_task();
}

void TaskQueuer::firebase_init_task() {
  QJsonObject args;
  args.insert(Message::kApiKey, _chain_state.value(Message::kApiKey));
  args.insert(Message::kAuthDomain, _chain_state.value(Message::kAuthDomain));
  args.insert(Message::kDatabaseURL, _chain_state.value(Message::kDatabaseURL));
  args.insert(Message::kStorageBucket, _chain_state.value(Message::kStorageBucket));
  args.insert(Message::kNodePath, _chain_state.value(Message::kNodePath));

  Message req(FirebaseRequestType::kFirebaseInit);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::firebase_destroy_task() {
  QJsonObject args;
  args.insert(Message::kNodePath, _chain_state.value(Message::kNodePath));

  Message req(FirebaseRequestType::kFirebaseDestroy);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::firebase_sign_in_task() {
  QJsonObject args;
  args.insert(Message::kEmail, _chain_state.value(Message::kEmail));
  args.insert(Message::kPassword, _chain_state.value(Message::kPassword));
  args.insert(Message::kNodePath, _chain_state.value(Message::kNodePath));

  Message req(FirebaseRequestType::kFirebaseSignIn);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::firebase_sign_out_task() {
  QJsonObject args;
  args.insert(Message::kNodePath, _chain_state.value(Message::kNodePath));

  Message req(FirebaseRequestType::kFirebaseSignOut);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::firebase_write_data_task() {
  QJsonObject args;
  args.insert(Message::kDataPath, _chain_state.value(Message::kDataPath));
  args.insert(Message::kValue, _chain_state.value(Message::kValue));
  args.insert(Message::kNodePath, _chain_state.value(Message::kNodePath));

  Message req(FirebaseRequestType::kFirebaseWriteData);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::firebase_read_data_task() {
  QJsonObject args;
  args.insert(Message::kDataPath, _chain_state.value(Message::kDataPath));
  args.insert(Message::kNodePath, _chain_state.value(Message::kNodePath));

  Message req(FirebaseRequestType::kFirebaseReadData);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::firebase_subscribe_task() {
  QJsonObject args;
  args.insert(Message::kDataPath, _chain_state.value(Message::kDataPath));
  args.insert(Message::kNodePath, _chain_state.value(Message::kNodePath));

  Message req(FirebaseRequestType::kFirebaseSubscribe);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void TaskQueuer::firebase_unsubscribe_task() {
  QJsonObject args;
  args.insert(Message::kDataPath, _chain_state.value(Message::kDataPath));
  args.insert(Message::kNodePath, _chain_state.value(Message::kNodePath));

  Message req(FirebaseRequestType::kFirebaseUnsubscribe);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

}
