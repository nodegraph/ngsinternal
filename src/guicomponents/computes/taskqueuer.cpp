#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/appconfig.h>

#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/message.h>

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
  queue_send_msg(tc, Message(WebDriverRequestType::kCloseBrowser), false);
}

void TaskQueuer::force_close_browser() {
  _scheduler->force_stack_reset();
  {
    // Make sure the browser is closed.
    TaskContext tc(_scheduler);
    queue_send_msg(tc, Message(WebDriverRequestType::kCloseBrowser), false);
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
  queue_send_msg(tc, Message(WebDriverRequestType::kStopService));
}

void TaskQueuer::queue_emit_option_texts() {
  TaskContext tc(_scheduler);
  queue_emit_option_texts(tc);
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
    _last_results.clear();
}

// -----------------------------------------------------------------
// Our Slots.
// -----------------------------------------------------------------

void TaskQueuer::on_poll() {
    if (!_scheduler->is_busy()) {
      TaskContext tc(_scheduler);
      //queue_perform_mouse_hover(tc);
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
  _last_query = msg;
  //std::cerr << "raw msg sent by app: " << msg.to_string().toStdString() << "\n";
}


void TaskQueuer::queue_send_msg(TaskContext& tc, const Message& msg, bool cancelable) {
  _scheduler->queue_task(tc, Task(std::bind(&TaskQueuer::send_msg_task,this, msg), cancelable), "queue_send_msg_task");
}

// ---------------------------------------------------------------------------------
// Queue Element Tasks.
// ---------------------------------------------------------------------------------

void TaskQueuer::queue_scroll_element_into_view(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::scroll_element_into_view_task,this), "queue_scroll_element_into_view");
}

// ---------------------------------------------------------------------------------
// Queue Framework Tasks.
// ---------------------------------------------------------------------------------

void TaskQueuer::queue_build_compute_node(TaskContext& tc, ComponentDID compute_did, const QJsonObject& params) {
  std::stringstream ss;
  ss << "queue build compute node with did: " << (size_t)compute_did;
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::build_compute_node_task,this, compute_did, params), ss.str());
}

void TaskQueuer::queue_receive_results(TaskContext& tc, std::function<void(const std::deque<QJsonObject>&)> receive_results) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::receive_results_task,this,receive_results), "queue_receive_results");
}

void TaskQueuer::queue_reset(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::reset_task, this), "reset");
}

// ---------------------------------------------------------------------------------
// Queue Browser Tasks.
// ---------------------------------------------------------------------------------

void TaskQueuer::queue_wait_for_chrome_connection(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::wait_for_chrome_connection_task,this), "queue_wait_for_chrome_connection");
}

void TaskQueuer::queue_open_browser(TaskContext& tc) {
  queue_send_msg(tc, Message(WebDriverRequestType::kOpenBrowser));
  queue_wait_for_chrome_connection(tc);
  queue_send_msg(tc, Message(WebDriverRequestType::kOpenBrowserPost));
  queue_update_current_tab(tc);
}

void TaskQueuer::queue_update_current_tab(TaskContext& tc) {
  queue_send_msg(tc, Message(WebDriverRequestType::kUpdateCurrentTab));
  queue_send_msg(tc, Message(ChromeRequestType::kUpdateCurrentTab));
}

// ---------------------------------------------------------------------------------
// Queue Page Content Tasks.
// ---------------------------------------------------------------------------------

void TaskQueuer::queue_wait(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::wait_task, this), "queue_wait");
}

// ---------------------------------------------------------------------------------
// Queue other actions.
// ---------------------------------------------------------------------------------

void TaskQueuer::queue_emit_option_texts(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&TaskQueuer::emit_option_texts_task,this), "queue_emit_option_texts");
}

// ------------------------------------------------------------------------
// Handle Incoming Messages.
// ------------------------------------------------------------------------

void TaskQueuer::handle_response(const Message& msg) {
  // Update the last message.
  _last_response = msg;

  QJsonValue value = msg.value(Message::kValue);

  // Note that if the value is an object it gets exploded into its properties.
  // Then each of these properties is added into the chain state.
  // This allows various properties to persist between calls.

  if (value.isObject()) {
    cache_results(value.toObject());
  } else {
    std::cerr << "Error: the returned values must be objects\n";
    assert(false);
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
    _last_click_info = msg.value(Message::kValue).toObject();
    emit show_web_action_menu();
  } else {
      std::cerr << "comm->app: received info: " << msg.to_string().toStdString() << "\n";
  }
}

// ------------------------------------------------------------------------
// Element Tasks.
// ------------------------------------------------------------------------

void TaskQueuer::scroll_element_into_view_task() {
  QJsonObject args;
  Message req(ChromeRequestType::kScrollElementIntoView,args);
  send_msg_task(req);
}

// ------------------------------------------------------------------------
// Infrastructure Tasks.
// ------------------------------------------------------------------------

void TaskQueuer::receive_results_task(std::function<void(const std::deque<QJsonObject>&)> receive_results) {
  receive_results(_last_results);
  _scheduler->run_next_task();
}

void TaskQueuer::build_compute_node_task(ComponentDID compute_did, const QJsonObject& params) {
  std::cerr << "building compute node!!\n";
  Entity* node = _manipulator->create_browser_node(true, compute_did, params);
  _manipulator->link_to_closest_node(node);
  //_manipulator->set_ultimate_targets(node, false);
  _scheduler->run_next_task();
}

void TaskQueuer::reset_task() {
  reset_state();
  TaskContext tc(_scheduler);
  queue_open_browser(tc);
}

// ------------------------------------------------------------------------
// Wait and Sleep Tasks.
// ------------------------------------------------------------------------

void TaskQueuer::wait_task() {
  _wait_timer.start(kWaitInterval);
}

void TaskQueuer::wait_for_chrome_connection_task() {
  _msg_sender->wait_for_chrome_connection();
  _scheduler->run_next_task();
}

// ------------------------------------------------------------------------
// Last Results related tasks.
// ------------------------------------------------------------------------

void TaskQueuer::emit_option_texts_task() {
  const QJsonObject& last_click = get_last_click_info();
  QJsonArray vals = last_click.value(Message::kOptionTexts).toArray();
  QStringList options;
  for (QJsonArray::const_iterator iter = vals.constBegin(); iter != vals.constEnd(); ++iter) {
    options.push_back(iter->toString());
  }
  emit select_option_texts(options);
  _scheduler->run_next_task();
}

void TaskQueuer::cache_results(const QJsonObject& results) {
  _last_results.push_back(results);
  while (_last_results.size() > kMaxLastResults) {
    _last_results.pop_front();
  }
}

}
