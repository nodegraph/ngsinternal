#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>

#include <components/interactions/graphbuilder.h>
#include <components/computes/compute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <components/computes/baseoutputs.h>
#include <components/computes/baseinputs.h>

#include <components/interactions/groupinteraction.h>

#include <components/compshapes/nodeshape.h>
#include <components/compshapes/linkshape.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>
#include <components/compshapes/compshapecollective.h>

#include <guicomponents/comms/appworker.h>
#include <guicomponents/comms/appcomm.h>
#include <guicomponents/comms/filemodel.h>
#include <components/interactions/shapecanvas.h>

#include <entities/entityids.h>

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
#include <sstream>

namespace ngs {

const int AppWorker::kPollInterval = 1000;
const int AppWorker::kJitterSize = 1;

AppWorker::AppWorker(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _app_comm(this),
      _factory(this),
      _node_selection(this),
      _show_browser(false),
      _hovering(false),
      _jitter(kJitterSize),
      _waiting_for_response(false),
      _next_msg_id(0),
      _connected(false) {
  get_dep_loader()->register_fixed_dep(_app_comm, Path({}));
  get_dep_loader()->register_fixed_dep(_factory, Path({}));
  get_dep_loader()->register_fixed_dep(_node_selection, Path({}));

  // Setup the poll timer.
  _poll_timer.setSingleShot(false);
  _poll_timer.setInterval(kPollInterval);
  connect(&_poll_timer,SIGNAL(timeout()),this,SLOT(on_poll()));
}

AppWorker::~AppWorker() {
}

void AppWorker::set_empty_stack_callback(std::function<void()> callback) {
  _empty_stack_callback = callback;
}

void AppWorker::initialize_wires() {
  Component::initialize_wires();

  // Create a qt signal slot connection. One time only!
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
    _waiting_for_response = false;
    _next_msg_id = 0;
    _expected_msg_id = -1;
    _last_resp = Message();
    _chain_state.clear();
    _stack.clear();

    // State for hovering.
    _hovering = false;
    _hover_state.clear();
    _jitter = kJitterSize;
}

// -----------------------------------------------------------------
// Our Slots.
// -----------------------------------------------------------------

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

void AppWorker::on_poll() {
  if (_app_comm->check_connection()) {
    if (_show_browser) {
      std::cerr << "polling open browser!\n";
      if (_stack.empty()) {
        TaskContext tc(this);
        queue_check_browser_is_open(tc);
        queue_check_browser_size(tc);
        // Debugging. - this makes the browser only come up once.
        _show_browser = false;
      }
    }
  }
  if (_hovering) {
    if (_stack.empty()) {
      mouse_hover_task();
    }
  }
}

// ---------------------------------------------------------------------------------
// Queue Framework Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_get_xpath(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::get_xpath_task,this), "queue_get_xpath");
}

void AppWorker::queue_get_crosshair_info(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::get_crosshair_info_task,this), "queue_get_crosshair_info");
}

void AppWorker::queue_merge_chain_state(TaskContext& tc, const QVariantMap& map) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::merge_chain_state_task,this, map), "queue_merge_chain_state");
}

void AppWorker::queue_build_compute_node(TaskContext& tc, ComponentDID compute_did, std::function<void(Entity*,Compute*)> on_node_built) {
  std::stringstream ss;
  ss << "queue build compute node with did: " << (size_t)compute_did;
  queue_task(tc, (AppTask)std::bind(&AppWorker::build_compute_node_task,this, compute_did, on_node_built), ss.str());
}

void AppWorker::queue_get_outputs(TaskContext& tc, std::function<void(const QVariantMap&)> on_get_outputs) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::get_outputs_task,this,on_get_outputs), "queue_get_outputs");
}

AppWorker::TaskQueue& AppWorker::get_task_queue() {
  return _stack.back();
}

void AppWorker::queue_start_sequence(TaskContext& tc) {
  tc.stack_index = _stack.size();
  _stack.push_back(TaskQueue());
  queue_task(tc, (AppTask)std::bind(&AppWorker::start_sequence_task,this), "queue_start_sequence");
}

void AppWorker::queue_finished_sequence(TaskContext& tc, std::function<void()> on_finished_sequence) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::finished_sequence_task,this,on_finished_sequence), "queue_finished_sequence");
}

// ---------------------------------------------------------------------------------
// Queue Cookie Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_get_all_cookies(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::get_all_cookies_task, this), "queue_get_all_cookies");
}

void AppWorker::queue_clear_all_cookies(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::clear_all_cookies_task, this), "queue_clear_all_cookies");
}

void AppWorker::queue_set_all_cookies(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::set_all_cookies_task, this), "queue_set_all_cookies");
}

// ---------------------------------------------------------------------------------
// Queue Browser Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_open_browser(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::open_browser_task,this), "queue_open_browser");
}

void AppWorker::queue_close_browser(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::close_browser_task,this), "queue_close_browser");
}

void AppWorker::queue_check_browser_is_open(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::check_browser_is_open_task,this), "queue_check_browser_is_open");
}

void AppWorker::queue_check_browser_size(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::check_browser_size_task,this), "queue_check_browser_size");
}

void AppWorker::queue_reset(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::reset_task, this), "reset");
}

// ---------------------------------------------------------------------------------
// Queue Page Content Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_block_events(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::block_events_task, this), "queue_block_events");
}

void AppWorker::queue_unblock_events(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::unblock_events_task, this), "queue_unblock_events");
}

void AppWorker::queue_wait_until_loaded(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::wait_until_loaded_task, this), "queue_wait_until_loaded");
}

// ---------------------------------------------------------------------------------
// Queue Navigate Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_navigate_to(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::navigate_to_task,this), "queue_navigate_to");
}

void AppWorker::queue_navigate_refresh(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::navigate_refresh_task,this), "queue_navigate_refresh");
}

void AppWorker::queue_switch_to_iframe(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::switch_to_iframe_task,this), "queue_swith_to_iframe");
}

// ---------------------------------------------------------------------------------
// Queue Set Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_update_overlays(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::update_overlays_task, this), "queue_update_overlays");
}

void AppWorker::queue_create_set_by_matching_values(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::create_set_by_matching_values_task,this), "queue_create_set_by_matching_values");
}

void AppWorker::queue_create_set_by_matching_type(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::create_set_by_matching_type_task,this), "queue_create_set_by_matching_type");
}

void AppWorker::queue_delete_set(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::delete_set_task,this), "queue_delete_set");
}

void AppWorker::queue_shift_set(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::shift_set_task,this), "queue_shift_set");
}

void AppWorker::queue_expand_set(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::expand_set_task,this), "queue_expand_set");
}

void AppWorker::queue_mark_set(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::mark_set_task,this), "queue_mark_set");
}

void AppWorker::queue_unmark_set(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::unmark_set_task,this), "queue_unmark_set");
}

void AppWorker::queue_merge_sets(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::merge_sets_task, this), "queue_merge_sets");
}

void AppWorker::queue_shrink_set_to_side(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::shrink_set_to_side_task, this), "queue_merge_sets");
}

void AppWorker::queue_shrink_against_marked(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::shrink_against_marked_task,this), "queue_shrink_against_marked");
}

// ---------------------------------------------------------------------------------
// Queue Perform Action Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_perform_mouse_action(TaskContext& tc) {
  queue_get_xpath(tc);
  queue_unblock_events(tc);
  queue_task(tc, (AppTask)std::bind(&AppWorker::perform_mouse_action_task,this), "queue_perform_action_task");
  queue_block_events(tc); // After we're done interacting with the page, block events on the page.
  queue_wait_until_loaded(tc); // Our actions may have triggered asynchronous content loading in the page, so we wait for the page to be ready.
  queue_update_overlays(tc); // Our actions may have moved elements arounds, so we update our overlays.
}

void AppWorker::queue_perform_mouse_hover(TaskContext& tc) {
  queue_get_xpath(tc);
  queue_unblock_events(tc);
  queue_task(tc, (AppTask)std::bind(&AppWorker::perform_hover_action_task,this), "queue_perform_hover");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_overlays(tc);
}

void AppWorker::queue_perform_post_mouse_hover(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::post_hover_task, this), "queue_perform_post_hover");
}

void AppWorker::queue_perform_text_action(TaskContext& tc) {
  queue_get_xpath(tc);
  queue_unblock_events(tc);
  queue_task(tc, (AppTask)std::bind(&AppWorker::perform_text_action_task,this), "queue_perform_text_action");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_overlays(tc);
}

void AppWorker::queue_perform_element_action(TaskContext& tc) {
  queue_get_xpath(tc);
  queue_unblock_events(tc);
  queue_task(tc, (AppTask)std::bind(&AppWorker::perform_element_action_task,this), "queue_perform_element_action");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_overlays(tc);
}

// ---------------------------------------------------------------------------------
// Queue other actions.
// ---------------------------------------------------------------------------------

void AppWorker::queue_start_mouse_hover(TaskContext& tc) {
  queue_get_xpath(tc);
  queue_task(tc, (AppTask)std::bind(&AppWorker::start_mouse_hover_task,this), "queue_stop_mouse_hover");
}

void AppWorker::queue_stop_mouse_hover(TaskContext& tc) {
  queue_task(tc, (AppTask)std::bind(&AppWorker::stop_mouse_hover_task,this), "queue_stop_mouse_hover");
}

void AppWorker::queue_emit_option_texts(TaskContext& tc) {
  queue_get_crosshair_info(tc);
  queue_task(tc, (AppTask)std::bind(&AppWorker::emit_option_texts_task,this), "select_from_dropdown3");
}



// ------------------------------------------------------------------------
// Task Management.
// ------------------------------------------------------------------------

void AppWorker::queue_task(TaskContext& tc, AppTask task, const std::string& about) {

  // Check to see if we can run the next worker.
  bool ok_to_run = true;

  // Make sure we're not already waiting for results.
  // We only allow one request to be in-flight at a time.
  if (_waiting_for_response) {
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

  if (get_task_queue().size() > 0) {
    ok_to_run = false;
  }

  // Push the task onto the right queue for the context. (This is not always the top queue.)
  // This is important because while a TaskContext is open another function/routine may open
  // another Sub-TaskContext. When the Sub-TaskContext falls out of context, we will be left
  // with its queue at the top of the queue stack. Now when other tasks get queued from the
  // original task context, the top queue is not the right one for it. The top one used to be
  // the right one for it before the Sub-TaskContext was created and destroyed. This is why
  // we have to the select the appropriate queue with tc.stack_index.
  _stack[tc.stack_index].push_back(task);

  // Run the worker if we're all clear to run.
  if (ok_to_run) {
    run_next_task();
  }
}

void AppWorker::run_next_task() {
  // If we're waiting for a response, then don't run the next task yet.
  if (_waiting_for_response) {
    return;
  }

  if (_stack.empty()) {
    _empty_stack_callback();
    return;
  }

  // If the queue is empty we're done.
  if (get_task_queue().empty()) {
    return;
  }

  // Pop a task and run it..
  AppTask task = get_task_queue().front();
  get_task_queue().pop_front();
  task();
}

void AppWorker::handle_response_from_nodejs(const Message& msg) {

  // Todo: Sometime we will get duplicate messages. Try to determine the cause.
  // If we get the same message (which include msg.id), ignore it.
//  if (msg == _last_resp) {
//    return;
//  }

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

  // Merge the values into the chain_state.
  QVariantMap &value = _last_resp[Message::kValue].toMap();
  for (QVariantMap::const_iterator iter = value.constBegin(); iter != value.constEnd(); ++iter) {
    _chain_state.insert(iter.key(), iter.value());
  }

  // Record the fact that we've received our response.
  _waiting_for_response = false;

  // Run the next task.
  run_next_task();
}

void AppWorker::handle_info_from_nodejs(const Message& msg) {
  std::cerr << "commhub --> app: info: " << msg.to_string().toStdString() << "\n";
  if (msg[Message::kInfo] == to_underlying(InfoType::kShowWebActionMenu)) {
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

// ------------------------------------------------------------------------
// Infrastructure Tasks.
// ------------------------------------------------------------------------

void AppWorker::send_msg_task(Message msg) {
  // Tag the request with an id. We expect a response with the same id.
  msg[Message::kID] = _next_msg_id;

  std::cerr << "app --> comhub: " << msg.to_string().toStdString() << "\n";

  // Increment the counter. Note we're ok with this overflowing and looping around.
  _next_msg_id += 1;

  // Send the request to nodejs.
  _waiting_for_response = true;
  size_t num_bytes = _app_comm->get_web_socket()->sendTextMessage(msg.to_string());
  assert(num_bytes);
}

void AppWorker::get_crosshair_info_task() {
  QVariantMap args;
  args[Message::kClickPos] = _click_pos;
  Message req(RequestType::kGetCrosshairInfo,args);
  send_msg_task(req);
}

// Should be run after a response message like get_crosshair_info_task that has set_index and overlay_index.
void AppWorker::get_xpath_task() {
  QVariantMap args;
  args[Message::kSetIndex] = _chain_state[Message::kSetIndex];
  args[Message::kOverlayIndex] = _chain_state[Message::kOverlayIndex];
  Message req(RequestType::kGetXPath,args);
  send_msg_task(req);
}

void AppWorker::merge_chain_state_task(const QVariantMap& map) {
  // Merge the values into the chain_state.
  for (QVariantMap::const_iterator iter = map.constBegin(); iter != map.constEnd(); ++iter) {
    _chain_state.insert(iter.key(), iter.value());
  }
  run_next_task();
}

void AppWorker::get_outputs_task(std::function<void(const QVariantMap&)> on_get_outputs) {
  on_get_outputs(_chain_state["outputs"].toMap());
  run_next_task();
}

void AppWorker::start_sequence_task() {
  // Make sure that we are the first task in the queue.
  //assert(get_task_queue().empty());
  run_next_task();
}

void AppWorker::finished_sequence_task(std::function<void()> on_finished_sequence) {
  // Make sure that we are the last task in the queue.
  assert(get_task_queue().empty());

  // We're done with this sequence of tasks so we pop the queue off of the stack.
  // Note each sequence of tasks is associated with a TaskContext and with one queue on the stack.
  _stack.pop_back();

  // Now call the callback which may add more queues to the stack.
  if (on_finished_sequence) {
    on_finished_sequence();
  }

  run_next_task();
}

void AppWorker::build_compute_node_task(ComponentDID compute_did, std::function<void(Entity* node, Compute* compute)> on_node_built) {
  // Create the node.
  Entity* group = _factory->get_current_group();
  std::pair<Entity*, Compute*> internals = _factory->create_compute_node2(group, compute_did);
  Entity* node = internals.first;
  Compute* compute = internals.second;

  // Initialize and update the wires.
  node->initialize_wires();
  group->clean_wires();

  // Set the values on all the inputs from the chain_state.
  QVariantMap::const_iterator iter;
  for (iter = _chain_state.begin() ; iter != _chain_state.end(); ++iter) {
    // Find the input entity.
    Path path({".","inputs"});
    path.push_back(iter.key().toStdString());
    Entity* input_entity = node->has_entity(path);
    // Skip this key if the entity doesn't exist.
    if (!input_entity) {
      continue;
    }
    // Get the compute.
    Dep<InputCompute> compute = get_dep<InputCompute>(input_entity);
    // Skip inputs which are plugs and not params.
    if (compute->is_exposed()) {
      continue;
    }

    std::cerr << "setting name: " << iter.key().toStdString() << " value: " << iter.value().toString().toStdString()
        << " type: " << iter.value().type() << " usertype: " << iter.value().userType() << "\n";
    compute->set_value(iter.value());
  }

  on_node_built(node, compute);

//  // Position and link the node we just created.
//  // We use a dummy component to avoid creating a cycle.
//  {
//    Linker* linker = new_ff Linker(get_app_root());
//    linker->link(node);
//    delete_ff(linker);
//  }
//
//  // Clean the wires in this group, as new nodes and links were created.
//  _factory->get_current_group()->clean_wires();
//
//  // Now execute it.
//  _graph_builder->clean_compute(compute->our_entity());
//
  run_next_task();
}


// ------------------------------------------------------------------------
// Cookie Tasks.
// ------------------------------------------------------------------------

void AppWorker::get_all_cookies_task() {
  Message req(RequestType::kGetAllCookies);
  send_msg_task(req);
}

void AppWorker::clear_all_cookies_task() {
  Message req(RequestType::kClearAllCookies);
  send_msg_task(req);
}

void AppWorker::set_all_cookies_task() {
  Message req(RequestType::kSetAllCookies);
  send_msg_task(req);
}


// ------------------------------------------------------------------------
// Browser Tasks.
// ------------------------------------------------------------------------

void AppWorker::open_browser_task() {
  QVariantMap args;
  args[Message::kURL] = get_smash_browse_url();

  Message req(RequestType::kOpenBrowser);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::close_browser_task() {
  Message req(RequestType::kCloseBrowser);
  send_msg_task(req);
}

void AppWorker::check_browser_is_open_task() {
  Message req(RequestType::kCheckBrowserIsOpen);
  send_msg_task(req);
}

void AppWorker::check_browser_size_task() {
  QVariantMap args;
  args[Message::kWidth] = _chain_state[Message::kWidth];
  args[Message::kHeight] = _chain_state[Message::kHeight];

  Message req(RequestType::kResizeBrowser);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

// ------------------------------------------------------------------------
// Page Content Tasks.
// ------------------------------------------------------------------------

void AppWorker::block_events_task() {
  // This action implies the browser's event unblocked for a time to allow
  // some actions to be performed. After such an action we need to update
  // the overlays as elements may disappear or move around.
  Message req(RequestType::kBlockEvents);
  send_msg_task(req);
}

void AppWorker::unblock_events_task() {
  Message req(RequestType::kUnblockEvents);
  send_msg_task(req);
}

void AppWorker::wait_until_loaded_task() {
  Message req(RequestType::kWaitUntilLoaded);
  send_msg_task(req);
}

// ------------------------------------------------------------------------
// Browser Reset and Shutdown Tasks.
// ------------------------------------------------------------------------

void AppWorker::shutdown_task() {
  Message msg(RequestType::kShutdown);
  // Shutdown without queuing it.
  send_msg_task(msg);
  _app_comm->destroy_connection();
}

void AppWorker::reset_browser_task() {
  // Close the browser without queuing it.
  close_browser_task();
  // Queue the reset now that all the queued task have been destroyed.
  TaskContext tc(this);
  queue_reset(tc);
}

// ------------------------------------------------------------------------
// Navigation Tasks.
// ------------------------------------------------------------------------

void AppWorker::navigate_to_task() {
  QVariantMap args;
  args[Message::kURL] = _chain_state[Message::kURL];
  Message req(RequestType::kNavigateTo, args);
  send_msg_task(req);
}

void AppWorker::navigate_refresh_task() {
  Message req(RequestType::kNavigateRefresh);
  send_msg_task(req);
}

void AppWorker::switch_to_iframe_task() {
  QVariantMap args;
  args[Message::kIFrame] = _chain_state[Message::kIFrame];
  Message req(RequestType::kSwitchIFrame, args);
  send_msg_task(req);
}

// ------------------------------------------------------------------------
// Set Creation/Modification Tasks.
// ------------------------------------------------------------------------

void AppWorker::update_overlays_task() {
  Message req(RequestType::kUpdateOveralys);
  send_msg_task(req);
}

void AppWorker::create_set_by_matching_values_task() {
  QVariantMap args;
  args[Message::kWrapType] = _chain_state[Message::kWrapType];
  args[Message::kTextValues] = _chain_state[Message::kTextValues];
  args[Message::kImageValues] = _chain_state[Message::kImageValues];

  Message req(RequestType::kCreateSetFromMatchValues);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::create_set_by_matching_type_task() {
  QVariantMap args;
  args[Message::kWrapType] = _chain_state[Message::kWrapType];

  Message req(RequestType::kCreateSetFromWrapType);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::delete_set_task() {
  QVariantMap args;
  args[Message::kSetIndex] = _chain_state[Message::kSetIndex];

  Message req(RequestType::kDeleteSet);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::shift_set_task() {
  QVariantMap args;
  args[Message::kSetIndex] = _chain_state[Message::kSetIndex];
  args[Message::kDirection] = _chain_state[Message::kDirection];
  args[Message::kWrapType] = _chain_state[Message::kWrapType];

  Message req(RequestType::kShiftSet);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::expand_set_task() {
  QVariantMap args;
  args[Message::kSetIndex] = _chain_state[Message::kSetIndex];
  args[Message::kDirection] = _chain_state[Message::kDirection];
  args[Message::kMatchCriteria] = _chain_state[Message::kMatchCriteria];

  Message req(RequestType::kExpandSet);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::mark_set_task() {
  QVariantMap args;
  args[Message::kSetIndex] = _chain_state[Message::kSetIndex];

  Message req(RequestType::kMarkSet);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::unmark_set_task() {
  QVariantMap args;
  args[Message::kSetIndex] = _chain_state[Message::kSetIndex];

  Message req(RequestType::kUnmarkSet);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::merge_sets_task() {
  Message req(RequestType::kMergeMarkedSets);
  send_msg_task(req);
}

void AppWorker::shrink_set_to_side_task() {
  QVariantMap args;
  args[Message::kSetIndex] = _chain_state[Message::kSetIndex];
  args[Message::kDirection] = _chain_state[Message::kDirection];

  Message req(RequestType::kShrinkSet);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::shrink_against_marked_task() {
  QVariantMap args;
  args[Message::kSetIndex] = _chain_state[Message::kSetIndex];
  args[Message::kDirections] = _chain_state[Message::kDirections];

  Message req(RequestType::kShrinkSetToMarked);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::perform_mouse_action_task() {
  QVariantMap args;
  args[Message::kMouseAction] = _chain_state[Message::kMouseAction];
  args[Message::kXPath] = _chain_state[Message::kXPath];
  args[Message::kOverlayRelClickPos] = _chain_state[Message::kOverlayRelClickPos];

  Message req(RequestType::kPerformMouseAction);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::perform_hover_action_task() {
  QVariantMap args;
  args[Message::kMouseAction] = to_underlying(MouseActionType::kMouseOver);
  args[Message::kXPath] = _hover_state[Message::kXPath];
  args[Message::kOverlayRelClickPos] = _hover_state[Message::kOverlayRelClickPos];

  Message req(RequestType::kPerformMouseAction);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::perform_text_action_task() {
  QVariantMap args;
  args[Message::kTextAction] = _chain_state[Message::kTextAction];
  args[Message::kXPath] = _chain_state[Message::kXPath];
  args[Message::kText] = _chain_state[Message::kText];

  Message req(RequestType::kPerformTextAction);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::perform_element_action_task() {
  QVariantMap args;
  args[Message::kElementAction] = _chain_state[Message::kElementAction];
  args[Message::kXPath] = _chain_state[Message::kXPath];
  args[Message::kOptionText] = _chain_state[Message::kOptionText]; // Used for selecting element from dropdowns.
  args[Message::kDirection] = _chain_state[Message::kDirection]; // Used for the scrolling directions.

  Message req(RequestType::kPerformElementAction);
  req[Message::kArgs] = args;
  send_msg_task(req);
}

void AppWorker::start_mouse_hover_task() {
  _hover_state = _chain_state;
  _hovering = true;
  run_next_task();
}

void AppWorker::stop_mouse_hover_task() {
  _hovering = false;
  run_next_task();
}

void AppWorker::mouse_hover_task() {
  // Jitter the hover position back and forth by one.
  int x = _hover_state[Message::kOverlayRelClickPos].toMap()["x"].toInt();
  int y = _hover_state[Message::kOverlayRelClickPos].toMap()["y"].toInt();
  x += _jitter;
  y += _jitter;
  _jitter *= -1;

  // Lock in the jitter.
  QVariantMap pos;
  pos["x"] = x;
  pos["y"] = y;
  _hover_state[Message::kOverlayRelClickPos] = pos;

  // Queue the tasks.
  TaskContext tc(this);
  queue_unblock_events(tc);
  queue_perform_mouse_hover(tc);
  queue_perform_post_mouse_hover(tc);
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_overlays(tc);
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

void AppWorker::emit_option_texts_task() {
  QStringList ot = _chain_state[Message::kOptionTexts].toStringList();
  emit select_option_texts(ot);
  run_next_task();
}

void AppWorker::reset_task() {
  reset_state();
  TaskContext tc(this);
  queue_check_browser_is_open(tc);
}


}
