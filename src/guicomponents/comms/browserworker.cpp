#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <base/objectmodel/basefactory.h>
#include <components/computes/inputcompute.h>

#include <guicomponents/comms/browserworker.h>
#include <guicomponents/comms/messagesender.h>

#include <iostream>
#include <sstream>

#include <QtCore/QCoreApplication>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

namespace ngs {

const int BrowserWorker::kPollInterval = 1000;
const int BrowserWorker::kJitterSize = 1;

BrowserWorker::BrowserWorker(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _msg_sender(this),
      _scheduler(this),
      _manipulator(this),
      _hovering(false),
      _jitter(kJitterSize)

{
  get_dep_loader()->register_fixed_dep(_msg_sender, Path({}));
  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
  get_dep_loader()->register_fixed_dep(_manipulator, Path({}));

  // Setup the poll timer.
  _poll_timer.setSingleShot(false);
  _poll_timer.setInterval(kPollInterval);
  connect(&_poll_timer,SIGNAL(timeout()),this,SLOT(on_poll()));
}

BrowserWorker::~BrowserWorker() {
}

void BrowserWorker::open() {
  external();
  _msg_sender->open();
}

void BrowserWorker::close() {
  external();
  _msg_sender->close();
}

bool BrowserWorker::is_open() {
  external();
  return _msg_sender->is_open();
}

void BrowserWorker::open_browser() {
  TaskContext tc(_scheduler);
  queue_open_browser(tc);
}

void BrowserWorker::close_browser() {
  TaskContext tc(_scheduler);
  queue_close_browser(tc);
}

void BrowserWorker::force_close_browser() {
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

void BrowserWorker::force_stack_reset() {
  _manipulator->clear_ultimate_target();
  _scheduler->force_stack_reset();
}

bool BrowserWorker::is_busy_cleaning() {
  return _manipulator->is_busy_cleaning();
}

void BrowserWorker::queue_emit_option_texts() {
  TaskContext tc(_scheduler);
  queue_emit_option_texts(tc);
}

void BrowserWorker::dive_into_firebase_group(const std::string& child_group_name, const QString& api_key, const QString& auth_domain, const QString& database_url, const QString& storage_bucket, const QString& email, const QString& password) {
  TaskContext tc(_scheduler);
  // Initialize the firebase wrapper.
  {
    QJsonObject args;
    args.insert(Message::kApiKey, api_key);
    args.insert(Message::kAuthDomain, auth_domain);
    args.insert(Message::kDatabaseURL, database_url);
    args.insert(Message::kStorageBucket, storage_bucket);
    queue_merge_chain_state(tc, args);
    queue_firebase_init(tc);
  }
  // Sign into a firebase account.
  {
    QJsonObject args;
    args.insert(Message::kEmail, email);
    args.insert(Message::kPassword, password);
    queue_merge_chain_state(tc, args);
    queue_firebase_sign_in(tc);
  }
  // Now dive into the group.
  // If any of the above steps fail, we won't be able to dive into the group, which is what we want.
  {
    queue_dive_into_group(tc, child_group_name);
  }
}
void BrowserWorker::clean_firebase_group(const std::string& child_group_name, const QString& api_key, const QString& auth_domain, const QString& database_url, const QString& storage_bucket, const QString& email, const QString& password) {
  TaskContext tc(_scheduler);
  // Initialize the firebase wrapper.
  {
    QJsonObject args;
    args.insert(Message::kApiKey, api_key);
    args.insert(Message::kAuthDomain, auth_domain);
    args.insert(Message::kDatabaseURL, database_url);
    args.insert(Message::kStorageBucket, storage_bucket);
    queue_merge_chain_state(tc, args);
    queue_firebase_init(tc);
  }
  // Sign into a firebase account.
  {
    QJsonObject args;
    args.insert(Message::kEmail, email);
    args.insert(Message::kPassword, password);
    queue_merge_chain_state(tc, args);
    queue_firebase_sign_in(tc);
  }
  // Now clean the group.
  {
    queue_clean_group(tc, child_group_name);
  }
}


void BrowserWorker::firebase_init(const QString& api_key, const QString& auth_domain, const QString& database_url, const QString& storage_bucket) {
  TaskContext tc(_scheduler);
  QJsonObject args;
  args.insert(Message::kApiKey, api_key);
  args.insert(Message::kAuthDomain, auth_domain);
  args.insert(Message::kDatabaseURL, database_url);
  args.insert(Message::kStorageBucket, storage_bucket);
  queue_merge_chain_state(tc, args);
  queue_firebase_init(tc);
}

void BrowserWorker::firebase_sign_in(const QString& email, const QString& password) {
  TaskContext tc(_scheduler);
  QJsonObject args;
  args.insert(Message::kEmail, email);
  args.insert(Message::kPassword, password);
  queue_merge_chain_state(tc, args);
  queue_firebase_sign_in(tc);
}

void BrowserWorker::dive_into_group(const std::string& child_group_name) {
  TaskContext tc(_scheduler);
  queue_dive_into_group(tc, child_group_name);
}

bool BrowserWorker::is_polling() {
  return _poll_timer.isActive();
}

void BrowserWorker::start_polling() {
  _poll_timer.start();
}

void BrowserWorker::stop_polling() {
  _poll_timer.stop();
}

void BrowserWorker::reset_state() {
    // State for message queuing.
    _chain_state = QJsonObject();

    // State for hovering.
    _hovering = false;
    _hover_state = QJsonObject();
    _jitter = kJitterSize;
}

// -----------------------------------------------------------------
// Our Slots.
// -----------------------------------------------------------------

void BrowserWorker::on_poll() {
  if (_hovering) {
    if (!_scheduler->is_busy()) {
      mouse_hover_task();
    }
  }
}

// ---------------------------------------------------------------------------------
// Socket Messaging Tasks.
// ---------------------------------------------------------------------------------

void BrowserWorker::send_msg_task(Message& msg) {
  int id = _scheduler->wait_for_response();
  msg.insert(Message::kID, id);
  _msg_sender->send_msg(msg);

  std::cerr << "sending app --> commhub: " << msg.to_string().toStdString() << "\n";
}

// ---------------------------------------------------------------------------------
// Queue Framework Tasks.
// ---------------------------------------------------------------------------------

void BrowserWorker::queue_get_xpath(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::get_xpath_task,this), "queue_get_xpath");
}

void BrowserWorker::queue_get_crosshair_info(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::get_crosshair_info_task,this), "queue_get_crosshair_info");
}

void BrowserWorker::queue_merge_chain_state(TaskContext& tc, const QJsonObject& map) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::merge_chain_state_task,this, map), "queue_merge_chain_state");
}

void BrowserWorker::queue_build_compute_node(TaskContext& tc, ComponentDID compute_did) {
  std::stringstream ss;
  ss << "queue build compute node with did: " << (size_t)compute_did;
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::build_compute_node_task,this, compute_did), ss.str());
}

void BrowserWorker::queue_get_outputs(TaskContext& tc, std::function<void(const QJsonObject&)> on_get_outputs) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::get_outputs_task,this,on_get_outputs), "queue_get_outputs");
}

// ---------------------------------------------------------------------------------
// Queue Cookie Tasks.
// ---------------------------------------------------------------------------------

void BrowserWorker::queue_get_all_cookies(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::get_all_cookies_task, this), "queue_get_all_cookies");
}

void BrowserWorker::queue_clear_all_cookies(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::clear_all_cookies_task, this), "queue_clear_all_cookies");
}

void BrowserWorker::queue_set_all_cookies(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::set_all_cookies_task, this), "queue_set_all_cookies");
}

// ---------------------------------------------------------------------------------
// Queue Browser Tasks.
// ---------------------------------------------------------------------------------

void BrowserWorker::queue_open_browser(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::open_browser_task,this), "queue_open_browser");
}

void BrowserWorker::queue_close_browser(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::close_browser_task,this), "queue_close_browser");
}

void BrowserWorker::queue_is_browser_open(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::is_browser_open_task,this), "queue_check_browser_is_open");
}

void BrowserWorker::queue_resize_browser(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::resize_browser_task,this), "queue_resize_browser");
}

void BrowserWorker::queue_reset(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::reset_task, this), "reset");
}

// ---------------------------------------------------------------------------------
// Queue Page Content Tasks.
// ---------------------------------------------------------------------------------

void BrowserWorker::queue_block_events(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::block_events_task, this), "queue_block_events");
}

void BrowserWorker::queue_unblock_events(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::unblock_events_task, this), "queue_unblock_events");
}

void BrowserWorker::queue_wait_until_loaded(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::wait_until_loaded_task, this), "queue_wait_until_loaded");
}

// ---------------------------------------------------------------------------------
// Queue Navigate Tasks.
// ---------------------------------------------------------------------------------

void BrowserWorker::queue_navigate_to(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::navigate_to_task,this), "queue_navigate_to");
}

void BrowserWorker::queue_navigate_refresh(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::navigate_refresh_task,this), "queue_navigate_refresh");
}

void BrowserWorker::queue_switch_to_iframe(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::switch_to_iframe_task,this), "queue_swith_to_iframe");
}

// ---------------------------------------------------------------------------------
// Queue Set Tasks.
// ---------------------------------------------------------------------------------

void BrowserWorker::queue_update_overlays(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::update_overlays_task, this), "queue_update_overlays");
}

void BrowserWorker::queue_create_set_by_matching_values(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::create_set_by_matching_values_task,this), "queue_create_set_by_matching_values");
}

void BrowserWorker::queue_create_set_by_matching_type(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::create_set_by_matching_type_task,this), "queue_create_set_by_matching_type");
}

void BrowserWorker::queue_delete_set(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::delete_set_task,this), "queue_delete_set");
}

void BrowserWorker::queue_shift_set(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::shift_set_task,this), "queue_shift_set");
}

void BrowserWorker::queue_expand_set(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::expand_set_task,this), "queue_expand_set");
}

void BrowserWorker::queue_mark_set(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::mark_set_task,this), "queue_mark_set");
}

void BrowserWorker::queue_unmark_set(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::unmark_set_task,this), "queue_unmark_set");
}

void BrowserWorker::queue_merge_sets(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::merge_sets_task, this), "queue_merge_sets");
}

void BrowserWorker::queue_shrink_set_to_side(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::shrink_set_to_side_task, this), "queue_merge_sets");
}

void BrowserWorker::queue_shrink_against_marked(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::shrink_against_marked_task,this), "queue_shrink_against_marked");
}

// ---------------------------------------------------------------------------------
// Queue Perform Action Tasks.
// ---------------------------------------------------------------------------------

void BrowserWorker::queue_perform_mouse_action(TaskContext& tc) {
  queue_get_xpath(tc);
  queue_unblock_events(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::perform_mouse_action_task,this), "queue_perform_action_task");
  queue_block_events(tc); // After we're done interacting with the page, block events on the page.
  queue_wait_until_loaded(tc); // Our actions may have triggered asynchronous content loading in the page, so we wait for the page to be ready.
  queue_update_overlays(tc); // Our actions may have moved elements arounds, so we update our overlays.
}

void BrowserWorker::queue_perform_mouse_hover(TaskContext& tc) {
  queue_get_xpath(tc);
  queue_unblock_events(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::perform_hover_action_task,this), "queue_perform_hover");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_overlays(tc);
}

void BrowserWorker::queue_perform_post_mouse_hover(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::post_hover_task, this), "queue_perform_post_hover");
}

void BrowserWorker::queue_perform_text_action(TaskContext& tc) {
  queue_get_xpath(tc);
  queue_unblock_events(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::perform_text_action_task,this), "queue_perform_text_action");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_overlays(tc);
}

void BrowserWorker::queue_perform_element_action(TaskContext& tc) {
  queue_get_xpath(tc);
  queue_unblock_events(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::perform_element_action_task,this), "queue_perform_element_action");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_overlays(tc);
}

// ---------------------------------------------------------------------------------
// Queue other actions.
// ---------------------------------------------------------------------------------

void BrowserWorker::queue_start_mouse_hover(TaskContext& tc) {
  queue_get_xpath(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::start_mouse_hover_task,this), "queue_stop_mouse_hover");
}

void BrowserWorker::queue_stop_mouse_hover(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::stop_mouse_hover_task,this), "queue_stop_mouse_hover");
}

void BrowserWorker::queue_emit_option_texts(TaskContext& tc) {
  queue_get_crosshair_info(tc);
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::emit_option_texts_task,this), "select_from_dropdown3");
}

// ---------------------------------------------------------------------------------
// Queue firebase actions.
// ---------------------------------------------------------------------------------

void BrowserWorker::queue_firebase_init(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::firebase_init_task,this), "queue_firebase_init");
}

void BrowserWorker::queue_firebase_sign_in(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::firebase_sign_in_task,this), "queue_firebase_sign_in");
}

void BrowserWorker::queue_firebase_sign_out(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::firebase_sign_out_task,this), "queue_firebase_sign_out");
}

void BrowserWorker::queue_firebase_write_data(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::firebase_write_data_task,this), "queue_firebase_write_data");
}

void BrowserWorker::queue_firebase_read_data(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::firebase_read_data_task,this), "queue_firebase_read_data");
}

void BrowserWorker::queue_firebase_listen_to_changes(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::firebase_listen_to_changes_task,this), "queue_firebase_listen_to_changes");
}

void BrowserWorker::queue_dive_into_group(TaskContext& tc, const std::string& child_group_name) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::dive_into_group_task,this, child_group_name), "queue_dive_into_group");
}

void BrowserWorker::queue_clean_group(TaskContext& tc, const std::string& child_group_name) {
  _scheduler->queue_task(tc, (Task)std::bind(&BrowserWorker::clean_firebase_group_task,this, child_group_name), "queue_clean_group");
}

// ------------------------------------------------------------------------
// Handle Incoming Messages.
// ------------------------------------------------------------------------

void BrowserWorker::handle_response(const Message& msg) {
  // Update the last message.
  _last_response = msg;

  QJsonValue value = msg.value(Message::kValue);

  if (value.isObject()) {
    // Merge the values into the chain_state.
    QJsonObject obj = value.toObject();
    Compute::shallow_object_merge(_chain_state, obj);
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

void BrowserWorker::handle_info(const Message& msg) {
  std::cerr << "commhub --> app: info: " << msg.to_string().toStdString() << "\n";
  if (msg.value(Message::kInfo).toInt() == to_underlying(InfoType::kShowWebActionMenu)) {
    _browser_click_pos = msg.value(Message::kValue).toObject().value(Message::kClickPos).toObject();
    _iframe_to_switch_to = msg.value(Message::kIFrame).toString();
    if (msg.value(Message::kValue).toObject().contains(Message::kPrevIFrame)) {
      QString prev_iframe = msg.value(Message::kValue).toObject().value(Message::kPrevIFrame).toString();
      emit show_iframe_menu();
    } else {
      emit show_web_action_menu();
    }
  } else {
      std::cerr << "comm->app: received info: " << msg.to_string().toStdString() << "\n";
  }
}

// ------------------------------------------------------------------------
// Infrastructure Tasks.
// ------------------------------------------------------------------------

void BrowserWorker::get_crosshair_info_task() {
  QJsonObject args;
  args.insert(Message::kClickPos, _browser_click_pos);
  Message req(RequestType::kGetCrosshairInfo,args);
  send_msg_task(req);
}

// Should be run after a response message like get_crosshair_info_task that has set_index and overlay_index.
void BrowserWorker::get_xpath_task() {
  QJsonObject args;
  args.insert(Message::kSetIndex, _chain_state.value(Message::kSetIndex));
  args.insert(Message::kOverlayIndex, _chain_state.value(Message::kOverlayIndex));
  Message req(RequestType::kGetXPath,args);
  send_msg_task(req);
}

void BrowserWorker::merge_chain_state_task(const QJsonObject& map) {
  // Merge the values into the chain_state.
  Compute::shallow_object_merge(_chain_state, map);
  _scheduler->run_next_task();
}

void BrowserWorker::get_outputs_task(std::function<void(const QJsonObject&)> on_get_outputs) {
  on_get_outputs(_chain_state);
  _scheduler->run_next_task();
}

void BrowserWorker::build_compute_node_task(ComponentDID compute_did) {
  Entity* node = _manipulator->build_and_link_compute_node(compute_did, _chain_state);
  _manipulator->set_ultimate_target(node, false);
}


// ------------------------------------------------------------------------
// Cookie Tasks.
// ------------------------------------------------------------------------

void BrowserWorker::get_all_cookies_task() {
  Message req(RequestType::kGetAllCookies);
  send_msg_task(req);
}

void BrowserWorker::clear_all_cookies_task() {
  Message req(RequestType::kClearAllCookies);
  send_msg_task(req);
}

void BrowserWorker::set_all_cookies_task() {
  Message req(RequestType::kSetAllCookies);
  send_msg_task(req);
}


// ------------------------------------------------------------------------
// Browser Tasks.
// ------------------------------------------------------------------------

void BrowserWorker::is_browser_open_task() {
  Message req(RequestType::kIsBrowserOpen);
  send_msg_task(req);
}

void BrowserWorker::close_browser_task() {
  Message req(RequestType::kCloseBrowser);
  send_msg_task(req);
}

void BrowserWorker::open_browser_task() {
  Message req(RequestType::kOpenBrowser);
  send_msg_task(req);
}

void BrowserWorker::resize_browser_task() {
  QJsonObject args;
  args.insert(Message::kWidth,_chain_state.value(Message::kWidth));
  args.insert(Message::kHeight, _chain_state.value(Message::kHeight));

  Message req(RequestType::kResizeBrowser);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

// ------------------------------------------------------------------------
// Page Content Tasks.
// ------------------------------------------------------------------------

void BrowserWorker::block_events_task() {
  // This action implies the browser's event unblocked for a time to allow
  // some actions to be performed. After such an action we need to update
  // the overlays as elements may disappear or move around.
  Message req(RequestType::kBlockEvents);
  send_msg_task(req);
}

void BrowserWorker::unblock_events_task() {
  Message req(RequestType::kUnblockEvents);
  send_msg_task(req);
}

void BrowserWorker::wait_until_loaded_task() {
  Message req(RequestType::kWaitUntilLoaded);
  send_msg_task(req);
}

// ------------------------------------------------------------------------
// Browser Reset and Shutdown Tasks.
// ------------------------------------------------------------------------

void BrowserWorker::shutdown_task() {
  Message msg(RequestType::kShutdown);
  // Shutdown without queuing it.
  send_msg_task(msg);
  close();
}

void BrowserWorker::reset_browser_task() {
  // Close the browser without queuing it.
  close_browser_task();
  // Queue the reset now that all the queued task have been destroyed.
  TaskContext tc(_scheduler);
  queue_reset(tc);
}

// ------------------------------------------------------------------------
// Navigation Tasks.
// ------------------------------------------------------------------------

void BrowserWorker::navigate_to_task() {
  QJsonObject args;
  args.insert(Message::kURL, _chain_state.value(Message::kURL));
  Message req(RequestType::kNavigateTo, args);
  send_msg_task(req);
}

void BrowserWorker::navigate_refresh_task() {
  Message req(RequestType::kNavigateRefresh);
  send_msg_task(req);
}

void BrowserWorker::switch_to_iframe_task() {
  QJsonObject args;
  args.insert(Message::kIFrame, _chain_state.value(Message::kIFrame));
  Message req(RequestType::kSwitchIFrame, args);
  send_msg_task(req);
}

// ------------------------------------------------------------------------
// Set Creation/Modification Tasks.
// ------------------------------------------------------------------------

void BrowserWorker::update_overlays_task() {
  Message req(RequestType::kUpdateOveralys);
  send_msg_task(req);
}

void BrowserWorker::create_set_by_matching_values_task() {
  QJsonObject args;
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));
  args.insert(Message::kTextValues, _chain_state.value(Message::kTextValues));
  args.insert(Message::kImageValues, _chain_state.value(Message::kImageValues));

  Message req(RequestType::kCreateSetFromMatchValues);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::create_set_by_matching_type_task() {
  QJsonObject args;
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));

  Message req(RequestType::kCreateSetFromWrapType);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::delete_set_task() {
  QJsonObject args;
  args.insert(Message::kSetIndex, _chain_state.value(Message::kSetIndex));

  Message req(RequestType::kDeleteSet);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::shift_set_task() {
  QJsonObject args;
  args.insert(Message::kSetIndex, _chain_state.value(Message::kSetIndex));
  args.insert(Message::kDirection, _chain_state.value(Message::kDirection));
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));

  Message req(RequestType::kShiftSet);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::expand_set_task() {
  QJsonObject args;
  args.insert(Message::kSetIndex, _chain_state.value(Message::kSetIndex));
  args.insert(Message::kDirection, _chain_state.value(Message::kDirection));
  args.insert(Message::kMatchCriteria, _chain_state.value(Message::kMatchCriteria));

  Message req(RequestType::kExpandSet);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::mark_set_task() {
  QJsonObject args;
  args.insert(Message::kSetIndex, _chain_state.value(Message::kSetIndex));

  Message req(RequestType::kMarkSet);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::unmark_set_task() {
  QJsonObject args;
  args.insert(Message::kSetIndex, _chain_state.value(Message::kSetIndex));

  Message req(RequestType::kUnmarkSet);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::merge_sets_task() {
  Message req(RequestType::kMergeMarkedSets);
  send_msg_task(req);
}

void BrowserWorker::shrink_set_to_side_task() {
  QJsonObject args;
  args.insert(Message::kSetIndex, _chain_state.value(Message::kSetIndex));
  args.insert(Message::kDirection, _chain_state.value(Message::kDirection));

  Message req(RequestType::kShrinkSet);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::shrink_against_marked_task() {
  QJsonObject args;
  args.insert(Message::kSetIndex, _chain_state.value(Message::kSetIndex));
  args.insert(Message::kDirections, _chain_state.value(Message::kDirections));

  Message req(RequestType::kShrinkSetToMarked);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::perform_mouse_action_task() {
  QJsonObject args;
  args.insert(Message::kMouseAction, _chain_state.value(Message::kMouseAction));
  args.insert(Message::kXPath, _chain_state.value(Message::kXPath));
  args.insert(Message::kOverlayRelClickPos, _chain_state.value(Message::kOverlayRelClickPos));

  Message req(RequestType::kPerformMouseAction);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::perform_hover_action_task() {
  QJsonObject args;
  args.insert(Message::kMouseAction, to_underlying(MouseActionType::kMouseOver));
  args.insert(Message::kXPath, _hover_state.value(Message::kXPath));
  args.insert(Message::kOverlayRelClickPos, _hover_state.value(Message::kOverlayRelClickPos));

  Message req(RequestType::kPerformMouseAction);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::perform_text_action_task() {
  QJsonObject args;
  args.insert(Message::kTextAction, _chain_state.value(Message::kTextAction));
  args.insert(Message::kXPath, _chain_state.value(Message::kXPath));
  args.insert(Message::kText, _chain_state.value(Message::kText));

  Message req(RequestType::kPerformTextAction);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::perform_element_action_task() {
  QJsonObject args;
  args.insert(Message::kElementAction, _chain_state.value(Message::kElementAction));
  args.insert(Message::kXPath, _chain_state.value(Message::kXPath));
  args.insert(Message::kOptionText, _chain_state.value(Message::kOptionText)); // Used for selecting element from dropdowns.
  args.insert(Message::kDirection, _chain_state.value(Message::kDirection)); // Used for the scrolling directions.

  Message req(RequestType::kPerformElementAction);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::start_mouse_hover_task() {
  _hover_state = _chain_state;
  _hovering = true;
  _scheduler->run_next_task();
}

void BrowserWorker::stop_mouse_hover_task() {
  _hovering = false;
  _scheduler->run_next_task();
}

void BrowserWorker::mouse_hover_task() {
  // Jitter the hover position back and forth by one.
  int x = _hover_state.value(Message::kOverlayRelClickPos).toObject().value("x").toInt();
  int y = _hover_state.value(Message::kOverlayRelClickPos).toObject().value("y").toInt();
  x += _jitter;
  y += _jitter;
  _jitter *= -1;

  // Lock in the jitter.
  QJsonObject pos;
  pos.insert("x", x);
  pos.insert("y", y);
  _hover_state.insert(Message::kOverlayRelClickPos, pos);

  // Queue the tasks.
  TaskContext tc(_scheduler);
  queue_unblock_events(tc);
  queue_perform_mouse_hover(tc);
  queue_perform_post_mouse_hover(tc);
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_overlays(tc);
}

void BrowserWorker::post_hover_task() {
  // Stop hovering if the last hover fails.
  // This happens if the element we're hovering over disappears or webdriver switches to another iframe.
  bool success = _last_response.value(Message::kSuccess).toBool();
  if (!success) {
    _hovering = false;
  }
  _scheduler->run_next_task();
}

void BrowserWorker::emit_option_texts_task() {
  QJsonArray vals = _chain_state.value(Message::kOptionTexts).toArray();
  QStringList options;
  for (QJsonArray::const_iterator iter = vals.constBegin(); iter != vals.constEnd(); ++iter) {
    options.push_back(iter->toString());
  }
  emit select_option_texts(options);
  _scheduler->run_next_task();
}

void BrowserWorker::reset_task() {
  reset_state();
  TaskContext tc(_scheduler);
  queue_open_browser(tc);
}

void BrowserWorker::firebase_init_task() {
  QJsonObject args;
  args.insert(Message::kApiKey, _chain_state.value(Message::kApiKey));
  args.insert(Message::kAuthDomain, _chain_state.value(Message::kAuthDomain));
  args.insert(Message::kDatabaseURL, _chain_state.value(Message::kDatabaseURL));
  args.insert(Message::kStorageBucket, _chain_state.value(Message::kStorageBucket));

  Message req(RequestType::kFirebaseInit);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::firebase_sign_in_task() {
  QJsonObject args;
  args.insert(Message::kEmail, _chain_state.value(Message::kEmail));
  args.insert(Message::kPassword, _chain_state.value(Message::kPassword));

  Message req(RequestType::kFirebaseSignIn);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::firebase_sign_out_task() {
  QJsonObject args;
  Message req(RequestType::kFirebaseSignOut);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::firebase_write_data_task() {
  QJsonObject args;
  args.insert(Message::kPath, _chain_state.value(Message::kPath));
  args.insert(Message::kValue, _chain_state.value(Message::kValue));

  Message req(RequestType::kFirebaseWriteData);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::firebase_read_data_task() {
  QJsonObject args;
  args.insert(Message::kPath, _chain_state.value(Message::kPath));

  Message req(RequestType::kFirebaseReadData);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::firebase_listen_to_changes_task() {
  QJsonObject args;
  args.insert(Message::kPath, _chain_state.value(Message::kPath));

  Message req(RequestType::kFirebaseListenToChanges);
  req.insert(Message::kArgs, args);
  send_msg_task(req);
}

void BrowserWorker::dive_into_group_task(const std::string& child_group_name) {
  _manipulator->dive_into_group(child_group_name);
  _scheduler->run_next_task();
}

void BrowserWorker::clean_firebase_group_task(const std::string& child_group_name) {
  _manipulator->clean_firebase_group(child_group_name);
  _scheduler->run_next_task();
}

}
