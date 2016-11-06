#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <guicomponents/comms/webworker.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <base/objectmodel/basefactory.h>
#include <components/computes/inputcompute.h>

#include <iostream>
#include <sstream>

#include <QtCore/QCoreApplication>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

namespace ngs {

const int WebWorker::kPollInterval = 1000;
const int WebWorker::kJitterSize = 1;

WebWorker::WebWorker(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _task_sheduler(this),
      _ng_manipulator(this),
      _show_browser(false),
      _hovering(false),
      _jitter(kJitterSize),
      _last_response_success(true){
  get_dep_loader()->register_fixed_dep(_task_sheduler, Path({}));
  get_dep_loader()->register_fixed_dep(_ng_manipulator, Path({}));

  // Setup the poll timer.
  _poll_timer.setSingleShot(false);
  _poll_timer.setInterval(kPollInterval);
  connect(&_poll_timer,SIGNAL(timeout()),this,SLOT(on_poll()));
}

WebWorker::~WebWorker() {
}

void WebWorker::open() {
  _task_sheduler->open();
}

void WebWorker::close() {
  _task_sheduler->close();
}

bool WebWorker::is_open() {
  return _task_sheduler->is_open();
}

void WebWorker::open_browser() {
  TaskContext tc(_task_sheduler);
  queue_open_browser(tc);
}

void WebWorker::close_browser() {
  TaskContext tc(_task_sheduler);
  queue_close_browser(tc);
}

void WebWorker::force_close_browser() {
  _task_sheduler->force_stack_reset();
  {
    // Make sure the browser is closed.
    TaskContext tc(_task_sheduler);
    queue_close_browser(tc);
  }

  // Wait for the tasks to fully flush out.
  while (_task_sheduler->is_busy()) {
    qApp->processEvents();
  }
}

void WebWorker::force_stack_reset() {
  _ng_manipulator->clear_ultimate_target();
  _task_sheduler->force_stack_reset();
}

bool WebWorker::is_busy_cleaning() {
  return _ng_manipulator->is_busy_cleaning();
}

void WebWorker::queue_emit_option_texts() {
  TaskContext tc(_task_sheduler);
  queue_emit_option_texts(tc);
}

void WebWorker::dive_into_firebase_group(const std::string& child_group_name, const QString& api_key, const QString& auth_domain, const QString& database_url, const QString& storage_bucket, const QString& email, const QString& password) {
  TaskContext tc(_task_sheduler);
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
void WebWorker::clean_firebase_group(const std::string& child_group_name, const QString& api_key, const QString& auth_domain, const QString& database_url, const QString& storage_bucket, const QString& email, const QString& password) {
  TaskContext tc(_task_sheduler);
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


void WebWorker::firebase_init(const QString& api_key, const QString& auth_domain, const QString& database_url, const QString& storage_bucket) {
  TaskContext tc(_task_sheduler);
  QJsonObject args;
  args.insert(Message::kApiKey, api_key);
  args.insert(Message::kAuthDomain, auth_domain);
  args.insert(Message::kDatabaseURL, database_url);
  args.insert(Message::kStorageBucket, storage_bucket);
  queue_merge_chain_state(tc, args);
  queue_firebase_init(tc);
}

void WebWorker::firebase_sign_in(const QString& email, const QString& password) {
  TaskContext tc(_task_sheduler);
  QJsonObject args;
  args.insert(Message::kEmail, email);
  args.insert(Message::kPassword, password);
  queue_merge_chain_state(tc, args);
  queue_firebase_sign_in(tc);
}

void WebWorker::dive_into_group(const std::string& child_group_name) {
  TaskContext tc(_task_sheduler);
  queue_dive_into_group(tc, child_group_name);
}

bool WebWorker::is_polling() {
  return _poll_timer.isActive();
}

void WebWorker::start_polling() {
  _poll_timer.start();
}

void WebWorker::stop_polling() {
  _poll_timer.stop();
}

void WebWorker::reset_state() {
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

void WebWorker::on_poll() {
  if (_show_browser) {
    if (!_task_sheduler->is_busy()) {
      TaskContext tc(_task_sheduler);
      queue_open_browser(tc);
      //queue_resize_browser(tc);
      // Debugging. - this makes the browser only come up once.
      _show_browser = false;
    }
  }

  if (_hovering) {
    if (!_task_sheduler->is_busy()) {
      mouse_hover_task();
    }
  }
}

// ---------------------------------------------------------------------------------
// Queue Framework Tasks.
// ---------------------------------------------------------------------------------

void WebWorker::queue_get_xpath(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::get_xpath_task,this), "queue_get_xpath");
}

void WebWorker::queue_get_crosshair_info(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::get_crosshair_info_task,this), "queue_get_crosshair_info");
}

void WebWorker::queue_merge_chain_state(TaskContext& tc, const QJsonObject& map) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::merge_chain_state_task,this, map), "queue_merge_chain_state");
}

void WebWorker::queue_build_compute_node(TaskContext& tc, ComponentDID compute_did) {
  std::stringstream ss;
  ss << "queue build compute node with did: " << (size_t)compute_did;
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::build_compute_node_task,this, compute_did), ss.str());
}

void WebWorker::queue_get_outputs(TaskContext& tc, std::function<void(const QJsonObject&)> on_get_outputs) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::get_outputs_task,this,on_get_outputs), "queue_get_outputs");
}

// ---------------------------------------------------------------------------------
// Queue Cookie Tasks.
// ---------------------------------------------------------------------------------

void WebWorker::queue_get_all_cookies(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::get_all_cookies_task, this), "queue_get_all_cookies");
}

void WebWorker::queue_clear_all_cookies(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::clear_all_cookies_task, this), "queue_clear_all_cookies");
}

void WebWorker::queue_set_all_cookies(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::set_all_cookies_task, this), "queue_set_all_cookies");
}

// ---------------------------------------------------------------------------------
// Queue Browser Tasks.
// ---------------------------------------------------------------------------------

void WebWorker::queue_open_browser(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::open_browser_task,this), "queue_open_browser");
}

void WebWorker::queue_close_browser(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::close_browser_task,this), "queue_close_browser");
}

void WebWorker::queue_is_browser_open(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::is_browser_open_task,this), "queue_check_browser_is_open");
}

void WebWorker::queue_resize_browser(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::resize_browser_task,this), "queue_resize_browser");
}

void WebWorker::queue_reset(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::reset_task, this), "reset");
}

// ---------------------------------------------------------------------------------
// Queue Page Content Tasks.
// ---------------------------------------------------------------------------------

void WebWorker::queue_block_events(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::block_events_task, this), "queue_block_events");
}

void WebWorker::queue_unblock_events(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::unblock_events_task, this), "queue_unblock_events");
}

void WebWorker::queue_wait_until_loaded(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::wait_until_loaded_task, this), "queue_wait_until_loaded");
}

// ---------------------------------------------------------------------------------
// Queue Navigate Tasks.
// ---------------------------------------------------------------------------------

void WebWorker::queue_navigate_to(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::navigate_to_task,this), "queue_navigate_to");
}

void WebWorker::queue_navigate_refresh(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::navigate_refresh_task,this), "queue_navigate_refresh");
}

void WebWorker::queue_switch_to_iframe(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::switch_to_iframe_task,this), "queue_swith_to_iframe");
}

// ---------------------------------------------------------------------------------
// Queue Set Tasks.
// ---------------------------------------------------------------------------------

void WebWorker::queue_update_overlays(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::update_overlays_task, this), "queue_update_overlays");
}

void WebWorker::queue_create_set_by_matching_values(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::create_set_by_matching_values_task,this), "queue_create_set_by_matching_values");
}

void WebWorker::queue_create_set_by_matching_type(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::create_set_by_matching_type_task,this), "queue_create_set_by_matching_type");
}

void WebWorker::queue_delete_set(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::delete_set_task,this), "queue_delete_set");
}

void WebWorker::queue_shift_set(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::shift_set_task,this), "queue_shift_set");
}

void WebWorker::queue_expand_set(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::expand_set_task,this), "queue_expand_set");
}

void WebWorker::queue_mark_set(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::mark_set_task,this), "queue_mark_set");
}

void WebWorker::queue_unmark_set(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::unmark_set_task,this), "queue_unmark_set");
}

void WebWorker::queue_merge_sets(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::merge_sets_task, this), "queue_merge_sets");
}

void WebWorker::queue_shrink_set_to_side(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::shrink_set_to_side_task, this), "queue_merge_sets");
}

void WebWorker::queue_shrink_against_marked(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::shrink_against_marked_task,this), "queue_shrink_against_marked");
}

// ---------------------------------------------------------------------------------
// Queue Perform Action Tasks.
// ---------------------------------------------------------------------------------

void WebWorker::queue_perform_mouse_action(TaskContext& tc) {
  queue_get_xpath(tc);
  queue_unblock_events(tc);
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::perform_mouse_action_task,this), "queue_perform_action_task");
  queue_block_events(tc); // After we're done interacting with the page, block events on the page.
  queue_wait_until_loaded(tc); // Our actions may have triggered asynchronous content loading in the page, so we wait for the page to be ready.
  queue_update_overlays(tc); // Our actions may have moved elements arounds, so we update our overlays.
}

void WebWorker::queue_perform_mouse_hover(TaskContext& tc) {
  queue_get_xpath(tc);
  queue_unblock_events(tc);
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::perform_hover_action_task,this), "queue_perform_hover");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_overlays(tc);
}

void WebWorker::queue_perform_post_mouse_hover(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::post_hover_task, this), "queue_perform_post_hover");
}

void WebWorker::queue_perform_text_action(TaskContext& tc) {
  queue_get_xpath(tc);
  queue_unblock_events(tc);
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::perform_text_action_task,this), "queue_perform_text_action");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_overlays(tc);
}

void WebWorker::queue_perform_element_action(TaskContext& tc) {
  queue_get_xpath(tc);
  queue_unblock_events(tc);
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::perform_element_action_task,this), "queue_perform_element_action");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_overlays(tc);
}

// ---------------------------------------------------------------------------------
// Queue other actions.
// ---------------------------------------------------------------------------------

void WebWorker::queue_start_mouse_hover(TaskContext& tc) {
  queue_get_xpath(tc);
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::start_mouse_hover_task,this), "queue_stop_mouse_hover");
}

void WebWorker::queue_stop_mouse_hover(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::stop_mouse_hover_task,this), "queue_stop_mouse_hover");
}

void WebWorker::queue_emit_option_texts(TaskContext& tc) {
  queue_get_crosshair_info(tc);
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::emit_option_texts_task,this), "select_from_dropdown3");
}

// ---------------------------------------------------------------------------------
// Queue firebase actions.
// ---------------------------------------------------------------------------------

void WebWorker::queue_firebase_init(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::firebase_init_task,this), "queue_firebase_init");
}

void WebWorker::queue_firebase_sign_in(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::firebase_sign_in_task,this), "queue_firebase_sign_in");
}

void WebWorker::queue_firebase_sign_out(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::firebase_sign_out_task,this), "queue_firebase_sign_out");
}

void WebWorker::queue_firebase_write_data(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::firebase_write_data_task,this), "queue_firebase_write_data");
}

void WebWorker::queue_firebase_read_data(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::firebase_read_data_task,this), "queue_firebase_read_data");
}

void WebWorker::queue_firebase_listen_to_changes(TaskContext& tc) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::firebase_listen_to_changes_task,this), "queue_firebase_listen_to_changes");
}

void WebWorker::queue_dive_into_group(TaskContext& tc, const std::string& child_group_name) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::dive_into_group_task,this, child_group_name), "queue_dive_into_group");
}

void WebWorker::queue_clean_group(TaskContext& tc, const std::string& child_group_name) {
  _task_sheduler->queue_task(tc, (Task)std::bind(&WebWorker::clean_firebase_group_task,this, child_group_name), "queue_clean_group");
}

// ------------------------------------------------------------------------
// Handle Incoming Messages.
// ------------------------------------------------------------------------

void WebWorker::handle_response(const Message& msg) {
  QJsonValue value = msg.value(Message::kValue);

  if (value.isObject()) {
    // Merge the values into the chain_state.
    QJsonObject obj = value.toObject();
    for (QJsonObject::const_iterator iter = obj.constBegin(); iter != obj.constEnd(); ++iter) {
      std::cerr << "inserting into chain: " << iter.key().toStdString() << " value: " << iter.value().toString().toStdString() << "\n";
      _chain_state.insert(iter.key(), iter.value());
    }
  } else if (!value.isUndefined()) {
    _chain_state.insert("value", value);
  }

  _last_response_success = msg.value(Message::kSuccess).toBool();
}

void WebWorker::handle_info(const Message& msg) {
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

void WebWorker::get_crosshair_info_task() {
  QJsonObject args;
  args.insert(Message::kClickPos, _browser_click_pos);
  Message req(RequestType::kGetCrosshairInfo,args);
  _task_sheduler->send_msg_task(req);
}

// Should be run after a response message like get_crosshair_info_task that has set_index and overlay_index.
void WebWorker::get_xpath_task() {
  QJsonObject args;
  args.insert(Message::kSetIndex, _chain_state.value(Message::kSetIndex));
  args.insert(Message::kOverlayIndex, _chain_state.value(Message::kOverlayIndex));
  Message req(RequestType::kGetXPath,args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::merge_chain_state_task(const QJsonObject& map) {
  // Merge the values into the chain_state.
  for (QJsonObject::const_iterator iter = map.constBegin(); iter != map.constEnd(); ++iter) {
    _chain_state.insert(iter.key(), iter.value());
  }
  _task_sheduler->run_next_task();
}

void WebWorker::get_outputs_task(std::function<void(const QJsonObject&)> on_get_outputs) {
  on_get_outputs(_chain_state);
  _task_sheduler->run_next_task();
}

void WebWorker::build_compute_node_task(ComponentDID compute_did) {
  Entity* node = _ng_manipulator->build_and_link_compute_node(compute_did, _chain_state);
  _ng_manipulator->set_ultimate_target(node, false);
}


// ------------------------------------------------------------------------
// Cookie Tasks.
// ------------------------------------------------------------------------

void WebWorker::get_all_cookies_task() {
  Message req(RequestType::kGetAllCookies);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::clear_all_cookies_task() {
  Message req(RequestType::kClearAllCookies);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::set_all_cookies_task() {
  Message req(RequestType::kSetAllCookies);
  _task_sheduler->send_msg_task(req);
}


// ------------------------------------------------------------------------
// Browser Tasks.
// ------------------------------------------------------------------------

void WebWorker::is_browser_open_task() {
  Message req(RequestType::kIsBrowserOpen);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::close_browser_task() {
  Message req(RequestType::kCloseBrowser);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::open_browser_task() {
  Message req(RequestType::kOpenBrowser);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::resize_browser_task() {
  QJsonObject args;
  args.insert(Message::kWidth,_chain_state.value(Message::kWidth));
  args.insert(Message::kHeight, _chain_state.value(Message::kHeight));

  Message req(RequestType::kResizeBrowser);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

// ------------------------------------------------------------------------
// Page Content Tasks.
// ------------------------------------------------------------------------

void WebWorker::block_events_task() {
  // This action implies the browser's event unblocked for a time to allow
  // some actions to be performed. After such an action we need to update
  // the overlays as elements may disappear or move around.
  Message req(RequestType::kBlockEvents);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::unblock_events_task() {
  Message req(RequestType::kUnblockEvents);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::wait_until_loaded_task() {
  Message req(RequestType::kWaitUntilLoaded);
  _task_sheduler->send_msg_task(req);
}

// ------------------------------------------------------------------------
// Browser Reset and Shutdown Tasks.
// ------------------------------------------------------------------------

void WebWorker::shutdown_task() {
  Message msg(RequestType::kShutdown);
  // Shutdown without queuing it.
  _task_sheduler->send_msg_task(msg);
  _task_sheduler->close();
}

void WebWorker::reset_browser_task() {
  // Close the browser without queuing it.
  close_browser_task();
  // Queue the reset now that all the queued task have been destroyed.
  TaskContext tc(_task_sheduler);
  queue_reset(tc);
}

// ------------------------------------------------------------------------
// Navigation Tasks.
// ------------------------------------------------------------------------

void WebWorker::navigate_to_task() {
  QJsonObject args;
  args.insert(Message::kURL, _chain_state.value(Message::kURL));
  Message req(RequestType::kNavigateTo, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::navigate_refresh_task() {
  Message req(RequestType::kNavigateRefresh);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::switch_to_iframe_task() {
  QJsonObject args;
  args.insert(Message::kIFrame, _chain_state.value(Message::kIFrame));
  Message req(RequestType::kSwitchIFrame, args);
  _task_sheduler->send_msg_task(req);
}

// ------------------------------------------------------------------------
// Set Creation/Modification Tasks.
// ------------------------------------------------------------------------

void WebWorker::update_overlays_task() {
  Message req(RequestType::kUpdateOveralys);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::create_set_by_matching_values_task() {
  QJsonObject args;
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));
  args.insert(Message::kTextValues, _chain_state.value(Message::kTextValues));
  args.insert(Message::kImageValues, _chain_state.value(Message::kImageValues));

  Message req(RequestType::kCreateSetFromMatchValues);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::create_set_by_matching_type_task() {
  QJsonObject args;
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));

  Message req(RequestType::kCreateSetFromWrapType);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::delete_set_task() {
  QJsonObject args;
  args.insert(Message::kSetIndex, _chain_state.value(Message::kSetIndex));

  Message req(RequestType::kDeleteSet);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::shift_set_task() {
  QJsonObject args;
  args.insert(Message::kSetIndex, _chain_state.value(Message::kSetIndex));
  args.insert(Message::kDirection, _chain_state.value(Message::kDirection));
  args.insert(Message::kWrapType, _chain_state.value(Message::kWrapType));

  Message req(RequestType::kShiftSet);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::expand_set_task() {
  QJsonObject args;
  args.insert(Message::kSetIndex, _chain_state.value(Message::kSetIndex));
  args.insert(Message::kDirection, _chain_state.value(Message::kDirection));
  args.insert(Message::kMatchCriteria, _chain_state.value(Message::kMatchCriteria));

  Message req(RequestType::kExpandSet);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::mark_set_task() {
  QJsonObject args;
  args.insert(Message::kSetIndex, _chain_state.value(Message::kSetIndex));

  Message req(RequestType::kMarkSet);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::unmark_set_task() {
  QJsonObject args;
  args.insert(Message::kSetIndex, _chain_state.value(Message::kSetIndex));

  Message req(RequestType::kUnmarkSet);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::merge_sets_task() {
  Message req(RequestType::kMergeMarkedSets);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::shrink_set_to_side_task() {
  QJsonObject args;
  args.insert(Message::kSetIndex, _chain_state.value(Message::kSetIndex));
  args.insert(Message::kDirection, _chain_state.value(Message::kDirection));

  Message req(RequestType::kShrinkSet);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::shrink_against_marked_task() {
  QJsonObject args;
  args.insert(Message::kSetIndex, _chain_state.value(Message::kSetIndex));
  args.insert(Message::kDirections, _chain_state.value(Message::kDirections));

  Message req(RequestType::kShrinkSetToMarked);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::perform_mouse_action_task() {
  QJsonObject args;
  args.insert(Message::kMouseAction, _chain_state.value(Message::kMouseAction));
  args.insert(Message::kXPath, _chain_state.value(Message::kXPath));
  args.insert(Message::kOverlayRelClickPos, _chain_state.value(Message::kOverlayRelClickPos));

  Message req(RequestType::kPerformMouseAction);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::perform_hover_action_task() {
  QJsonObject args;
  args.insert(Message::kMouseAction, to_underlying(MouseActionType::kMouseOver));
  args.insert(Message::kXPath, _hover_state.value(Message::kXPath));
  args.insert(Message::kOverlayRelClickPos, _hover_state.value(Message::kOverlayRelClickPos));

  Message req(RequestType::kPerformMouseAction);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::perform_text_action_task() {
  QJsonObject args;
  args.insert(Message::kTextAction, _chain_state.value(Message::kTextAction));
  args.insert(Message::kXPath, _chain_state.value(Message::kXPath));
  args.insert(Message::kText, _chain_state.value(Message::kText));

  Message req(RequestType::kPerformTextAction);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::perform_element_action_task() {
  QJsonObject args;
  args.insert(Message::kElementAction, _chain_state.value(Message::kElementAction));
  args.insert(Message::kXPath, _chain_state.value(Message::kXPath));
  args.insert(Message::kOptionText, _chain_state.value(Message::kOptionText)); // Used for selecting element from dropdowns.
  args.insert(Message::kDirection, _chain_state.value(Message::kDirection)); // Used for the scrolling directions.

  Message req(RequestType::kPerformElementAction);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::start_mouse_hover_task() {
  _hover_state = _chain_state;
  _hovering = true;
  _task_sheduler->run_next_task();
}

void WebWorker::stop_mouse_hover_task() {
  _hovering = false;
  _task_sheduler->run_next_task();
}

void WebWorker::mouse_hover_task() {
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
  TaskContext tc(_task_sheduler);
  queue_unblock_events(tc);
  queue_perform_mouse_hover(tc);
  queue_perform_post_mouse_hover(tc);
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_overlays(tc);
}

void WebWorker::post_hover_task() {
  // Stop hovering if the last hover fails.
  // This happens if the element we're hovering over disappears or webdriver switches to another iframe.
  bool success = _task_sheduler->get_last_response().value(Message::kSuccess).toBool();
  if (!success) {
    _hovering = false;
  }
  _task_sheduler->run_next_task();
}

void WebWorker::emit_option_texts_task() {
  QJsonArray vals = _chain_state.value(Message::kOptionTexts).toArray();
  QStringList options;
  for (QJsonArray::const_iterator iter = vals.constBegin(); iter != vals.constEnd(); ++iter) {
    options.push_back(iter->toString());
  }
  emit select_option_texts(options);
  _task_sheduler->run_next_task();
}

void WebWorker::reset_task() {
  reset_state();
  TaskContext tc(_task_sheduler);
  queue_open_browser(tc);
}

void WebWorker::firebase_init_task() {
  QJsonObject args;
  args.insert(Message::kApiKey, _chain_state.value(Message::kApiKey));
  args.insert(Message::kAuthDomain, _chain_state.value(Message::kAuthDomain));
  args.insert(Message::kDatabaseURL, _chain_state.value(Message::kDatabaseURL));
  args.insert(Message::kStorageBucket, _chain_state.value(Message::kStorageBucket));

  Message req(RequestType::kFirebaseInit);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::firebase_sign_in_task() {
  QJsonObject args;
  args.insert(Message::kEmail, _chain_state.value(Message::kEmail));
  args.insert(Message::kPassword, _chain_state.value(Message::kPassword));

  Message req(RequestType::kFirebaseSignIn);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::firebase_sign_out_task() {
  QJsonObject args;
  Message req(RequestType::kFirebaseSignOut);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::firebase_write_data_task() {
  QJsonObject args;
  args.insert(Message::kPath, _chain_state.value(Message::kPath));
  args.insert(Message::kValue, _chain_state.value(Message::kValue));

  Message req(RequestType::kFirebaseWriteData);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::firebase_read_data_task() {
  QJsonObject args;
  args.insert(Message::kPath, _chain_state.value(Message::kPath));

  Message req(RequestType::kFirebaseReadData);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::firebase_listen_to_changes_task() {
  QJsonObject args;
  args.insert(Message::kPath, _chain_state.value(Message::kPath));

  Message req(RequestType::kFirebaseListenToChanges);
  req.insert(Message::kArgs, args);
  _task_sheduler->send_msg_task(req);
}

void WebWorker::dive_into_group_task(const std::string& child_group_name) {
  _ng_manipulator->dive_into_group(child_group_name);
  _task_sheduler->run_next_task();
}

void WebWorker::clean_firebase_group_task(const std::string& child_group_name) {
  _ng_manipulator->clean_firebase_group(child_group_name);
  _task_sheduler->run_next_task();
}

}
