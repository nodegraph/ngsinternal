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

#include <guicomponents/comms/appconfig.h>
#include <guicomponents/comms/appworker.h>
#include <guicomponents/comms/appcomm.h>
#include <guicomponents/comms/apptaskqueue.h>
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
      _task_queue(this),
      _factory(this),
      _show_browser(false),
      _hovering(false),
      _jitter(kJitterSize) {
  get_dep_loader()->register_fixed_dep(_task_queue, Path({}));
  get_dep_loader()->register_fixed_dep(_factory, Path({}));

  // Setup the poll timer.
  _poll_timer.setSingleShot(false);
  _poll_timer.setInterval(kPollInterval);
  connect(&_poll_timer,SIGNAL(timeout()),this,SLOT(on_poll()));
}

AppWorker::~AppWorker() {
}

void AppWorker::open() {
  _task_queue->open();
}

void AppWorker::close() {
  _task_queue->close();
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
    _chain_state.clear();

    // State for hovering.
    _hovering = false;
    _hover_state.clear();
    _jitter = kJitterSize;
}

// -----------------------------------------------------------------
// Our Slots.
// -----------------------------------------------------------------

void AppWorker::on_poll() {
  if (_show_browser) {
    std::cerr << "polling open browser!\n";
    if (!_task_queue->is_busy()) {
      AppTaskContext tc(_task_queue);
      queue_check_browser_is_open(tc);
      queue_check_browser_size(tc);
      // Debugging. - this makes the browser only come up once.
      _show_browser = false;
    }
  }

  if (_hovering) {
    if (!_task_queue->is_busy()) {
      mouse_hover_task();
    }
  }
}

// ---------------------------------------------------------------------------------
// Queue Framework Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_get_xpath(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::get_xpath_task,this), "queue_get_xpath");
}

void AppWorker::queue_get_crosshair_info(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::get_crosshair_info_task,this), "queue_get_crosshair_info");
}

void AppWorker::queue_merge_chain_state(AppTaskContext& tc, const QVariantMap& map) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::merge_chain_state_task,this, map), "queue_merge_chain_state");
}

void AppWorker::queue_build_compute_node(AppTaskContext& tc, ComponentDID compute_did, std::function<void(Entity*,Compute*)> on_node_built) {
  std::stringstream ss;
  ss << "queue build compute node with did: " << (size_t)compute_did;
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::build_compute_node_task,this, compute_did, on_node_built), ss.str());
}

void AppWorker::queue_get_outputs(AppTaskContext& tc, std::function<void(const QVariantMap&)> on_get_outputs) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::get_outputs_task,this,on_get_outputs), "queue_get_outputs");
}

// ---------------------------------------------------------------------------------
// Queue Cookie Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_get_all_cookies(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::get_all_cookies_task, this), "queue_get_all_cookies");
}

void AppWorker::queue_clear_all_cookies(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::clear_all_cookies_task, this), "queue_clear_all_cookies");
}

void AppWorker::queue_set_all_cookies(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::set_all_cookies_task, this), "queue_set_all_cookies");
}

// ---------------------------------------------------------------------------------
// Queue Browser Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_open_browser(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::open_browser_task,this), "queue_open_browser");
}

void AppWorker::queue_close_browser(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::close_browser_task,this), "queue_close_browser");
}

void AppWorker::queue_check_browser_is_open(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::check_browser_is_open_task,this), "queue_check_browser_is_open");
}

void AppWorker::queue_check_browser_size(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::check_browser_size_task,this), "queue_check_browser_size");
}

void AppWorker::queue_reset(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::reset_task, this), "reset");
}

// ---------------------------------------------------------------------------------
// Queue Page Content Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_block_events(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::block_events_task, this), "queue_block_events");
}

void AppWorker::queue_unblock_events(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::unblock_events_task, this), "queue_unblock_events");
}

void AppWorker::queue_wait_until_loaded(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::wait_until_loaded_task, this), "queue_wait_until_loaded");
}

// ---------------------------------------------------------------------------------
// Queue Navigate Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_navigate_to(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::navigate_to_task,this), "queue_navigate_to");
}

void AppWorker::queue_navigate_refresh(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::navigate_refresh_task,this), "queue_navigate_refresh");
}

void AppWorker::queue_switch_to_iframe(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::switch_to_iframe_task,this), "queue_swith_to_iframe");
}

// ---------------------------------------------------------------------------------
// Queue Set Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_update_overlays(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::update_overlays_task, this), "queue_update_overlays");
}

void AppWorker::queue_create_set_by_matching_values(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::create_set_by_matching_values_task,this), "queue_create_set_by_matching_values");
}

void AppWorker::queue_create_set_by_matching_type(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::create_set_by_matching_type_task,this), "queue_create_set_by_matching_type");
}

void AppWorker::queue_delete_set(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::delete_set_task,this), "queue_delete_set");
}

void AppWorker::queue_shift_set(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::shift_set_task,this), "queue_shift_set");
}

void AppWorker::queue_expand_set(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::expand_set_task,this), "queue_expand_set");
}

void AppWorker::queue_mark_set(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::mark_set_task,this), "queue_mark_set");
}

void AppWorker::queue_unmark_set(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::unmark_set_task,this), "queue_unmark_set");
}

void AppWorker::queue_merge_sets(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::merge_sets_task, this), "queue_merge_sets");
}

void AppWorker::queue_shrink_set_to_side(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::shrink_set_to_side_task, this), "queue_merge_sets");
}

void AppWorker::queue_shrink_against_marked(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::shrink_against_marked_task,this), "queue_shrink_against_marked");
}

// ---------------------------------------------------------------------------------
// Queue Perform Action Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_perform_mouse_action(AppTaskContext& tc) {
  queue_get_xpath(tc);
  queue_unblock_events(tc);
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::perform_mouse_action_task,this), "queue_perform_action_task");
  queue_block_events(tc); // After we're done interacting with the page, block events on the page.
  queue_wait_until_loaded(tc); // Our actions may have triggered asynchronous content loading in the page, so we wait for the page to be ready.
  queue_update_overlays(tc); // Our actions may have moved elements arounds, so we update our overlays.
}

void AppWorker::queue_perform_mouse_hover(AppTaskContext& tc) {
  queue_get_xpath(tc);
  queue_unblock_events(tc);
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::perform_hover_action_task,this), "queue_perform_hover");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_overlays(tc);
}

void AppWorker::queue_perform_post_mouse_hover(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::post_hover_task, this), "queue_perform_post_hover");
}

void AppWorker::queue_perform_text_action(AppTaskContext& tc) {
  queue_get_xpath(tc);
  queue_unblock_events(tc);
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::perform_text_action_task,this), "queue_perform_text_action");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_overlays(tc);
}

void AppWorker::queue_perform_element_action(AppTaskContext& tc) {
  queue_get_xpath(tc);
  queue_unblock_events(tc);
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::perform_element_action_task,this), "queue_perform_element_action");
  queue_block_events(tc);
  queue_wait_until_loaded(tc);
  queue_update_overlays(tc);
}

// ---------------------------------------------------------------------------------
// Queue other actions.
// ---------------------------------------------------------------------------------

void AppWorker::queue_start_mouse_hover(AppTaskContext& tc) {
  queue_get_xpath(tc);
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::start_mouse_hover_task,this), "queue_stop_mouse_hover");
}

void AppWorker::queue_stop_mouse_hover(AppTaskContext& tc) {
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::stop_mouse_hover_task,this), "queue_stop_mouse_hover");
}

void AppWorker::queue_emit_option_texts(AppTaskContext& tc) {
  queue_get_crosshair_info(tc);
  _task_queue->queue_task(tc, (AppTask)std::bind(&AppWorker::emit_option_texts_task,this), "select_from_dropdown3");
}

// ------------------------------------------------------------------------
// Handle Incoming Messages.
// ------------------------------------------------------------------------

void AppWorker::handle_response(const Message& msg) {
  // Merge the values into the chain_state.
  QVariantMap &value = msg[Message::kValue].toMap();
  for (QVariantMap::const_iterator iter = value.constBegin(); iter != value.constEnd(); ++iter) {
    _chain_state.insert(iter.key(), iter.value());
  }
}

void AppWorker::handle_info(const Message& msg) {
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

void AppWorker::get_crosshair_info_task() {
  QVariantMap args;
  args[Message::kClickPos] = _click_pos;
  Message req(RequestType::kGetCrosshairInfo,args);
  _task_queue->send_msg_task(req);
}

// Should be run after a response message like get_crosshair_info_task that has set_index and overlay_index.
void AppWorker::get_xpath_task() {
  QVariantMap args;
  args[Message::kSetIndex] = _chain_state[Message::kSetIndex];
  args[Message::kOverlayIndex] = _chain_state[Message::kOverlayIndex];
  Message req(RequestType::kGetXPath,args);
  _task_queue->send_msg_task(req);
}

void AppWorker::merge_chain_state_task(const QVariantMap& map) {
  // Merge the values into the chain_state.
  for (QVariantMap::const_iterator iter = map.constBegin(); iter != map.constEnd(); ++iter) {
    _chain_state.insert(iter.key(), iter.value());
  }
  _task_queue->run_next_task();
}

void AppWorker::get_outputs_task(std::function<void(const QVariantMap&)> on_get_outputs) {
  on_get_outputs(_chain_state["outputs"].toMap());
  _task_queue->run_next_task();
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

  _task_queue->run_next_task();
}


// ------------------------------------------------------------------------
// Cookie Tasks.
// ------------------------------------------------------------------------

void AppWorker::get_all_cookies_task() {
  Message req(RequestType::kGetAllCookies);
  _task_queue->send_msg_task(req);
}

void AppWorker::clear_all_cookies_task() {
  Message req(RequestType::kClearAllCookies);
  _task_queue->send_msg_task(req);
}

void AppWorker::set_all_cookies_task() {
  Message req(RequestType::kSetAllCookies);
  _task_queue->send_msg_task(req);
}


// ------------------------------------------------------------------------
// Browser Tasks.
// ------------------------------------------------------------------------

void AppWorker::open_browser_task() {
  QVariantMap args;
  args[Message::kURL] = AppConfig::get_smash_browse_url();

  Message req(RequestType::kOpenBrowser);
  req[Message::kArgs] = args;
  _task_queue->send_msg_task(req);
}

void AppWorker::close_browser_task() {
  Message req(RequestType::kCloseBrowser);
  _task_queue->send_msg_task(req);
}

void AppWorker::check_browser_is_open_task() {
  Message req(RequestType::kCheckBrowserIsOpen);
  _task_queue->send_msg_task(req);
}

void AppWorker::check_browser_size_task() {
  QVariantMap args;
  args[Message::kWidth] = _chain_state[Message::kWidth];
  args[Message::kHeight] = _chain_state[Message::kHeight];

  Message req(RequestType::kResizeBrowser);
  req[Message::kArgs] = args;
  _task_queue->send_msg_task(req);
}

// ------------------------------------------------------------------------
// Page Content Tasks.
// ------------------------------------------------------------------------

void AppWorker::block_events_task() {
  // This action implies the browser's event unblocked for a time to allow
  // some actions to be performed. After such an action we need to update
  // the overlays as elements may disappear or move around.
  Message req(RequestType::kBlockEvents);
  _task_queue->send_msg_task(req);
}

void AppWorker::unblock_events_task() {
  Message req(RequestType::kUnblockEvents);
  _task_queue->send_msg_task(req);
}

void AppWorker::wait_until_loaded_task() {
  Message req(RequestType::kWaitUntilLoaded);
  _task_queue->send_msg_task(req);
}

// ------------------------------------------------------------------------
// Browser Reset and Shutdown Tasks.
// ------------------------------------------------------------------------

void AppWorker::shutdown_task() {
  Message msg(RequestType::kShutdown);
  // Shutdown without queuing it.
  _task_queue->send_msg_task(msg);
  _task_queue->close();
}

void AppWorker::reset_browser_task() {
  // Close the browser without queuing it.
  close_browser_task();
  // Queue the reset now that all the queued task have been destroyed.
  AppTaskContext tc(_task_queue);
  queue_reset(tc);
}

// ------------------------------------------------------------------------
// Navigation Tasks.
// ------------------------------------------------------------------------

void AppWorker::navigate_to_task() {
  QVariantMap args;
  args[Message::kURL] = _chain_state[Message::kURL];
  Message req(RequestType::kNavigateTo, args);
  _task_queue->send_msg_task(req);
}

void AppWorker::navigate_refresh_task() {
  Message req(RequestType::kNavigateRefresh);
  _task_queue->send_msg_task(req);
}

void AppWorker::switch_to_iframe_task() {
  QVariantMap args;
  args[Message::kIFrame] = _chain_state[Message::kIFrame];
  Message req(RequestType::kSwitchIFrame, args);
  _task_queue->send_msg_task(req);
}

// ------------------------------------------------------------------------
// Set Creation/Modification Tasks.
// ------------------------------------------------------------------------

void AppWorker::update_overlays_task() {
  Message req(RequestType::kUpdateOveralys);
  _task_queue->send_msg_task(req);
}

void AppWorker::create_set_by_matching_values_task() {
  QVariantMap args;
  args[Message::kWrapType] = _chain_state[Message::kWrapType];
  args[Message::kTextValues] = _chain_state[Message::kTextValues];
  args[Message::kImageValues] = _chain_state[Message::kImageValues];

  Message req(RequestType::kCreateSetFromMatchValues);
  req[Message::kArgs] = args;
  _task_queue->send_msg_task(req);
}

void AppWorker::create_set_by_matching_type_task() {
  QVariantMap args;
  args[Message::kWrapType] = _chain_state[Message::kWrapType];

  Message req(RequestType::kCreateSetFromWrapType);
  req[Message::kArgs] = args;
  _task_queue->send_msg_task(req);
}

void AppWorker::delete_set_task() {
  QVariantMap args;
  args[Message::kSetIndex] = _chain_state[Message::kSetIndex];

  Message req(RequestType::kDeleteSet);
  req[Message::kArgs] = args;
  _task_queue->send_msg_task(req);
}

void AppWorker::shift_set_task() {
  QVariantMap args;
  args[Message::kSetIndex] = _chain_state[Message::kSetIndex];
  args[Message::kDirection] = _chain_state[Message::kDirection];
  args[Message::kWrapType] = _chain_state[Message::kWrapType];

  Message req(RequestType::kShiftSet);
  req[Message::kArgs] = args;
  _task_queue->send_msg_task(req);
}

void AppWorker::expand_set_task() {
  QVariantMap args;
  args[Message::kSetIndex] = _chain_state[Message::kSetIndex];
  args[Message::kDirection] = _chain_state[Message::kDirection];
  args[Message::kMatchCriteria] = _chain_state[Message::kMatchCriteria];

  Message req(RequestType::kExpandSet);
  req[Message::kArgs] = args;
  _task_queue->send_msg_task(req);
}

void AppWorker::mark_set_task() {
  QVariantMap args;
  args[Message::kSetIndex] = _chain_state[Message::kSetIndex];

  Message req(RequestType::kMarkSet);
  req[Message::kArgs] = args;
  _task_queue->send_msg_task(req);
}

void AppWorker::unmark_set_task() {
  QVariantMap args;
  args[Message::kSetIndex] = _chain_state[Message::kSetIndex];

  Message req(RequestType::kUnmarkSet);
  req[Message::kArgs] = args;
  _task_queue->send_msg_task(req);
}

void AppWorker::merge_sets_task() {
  Message req(RequestType::kMergeMarkedSets);
  _task_queue->send_msg_task(req);
}

void AppWorker::shrink_set_to_side_task() {
  QVariantMap args;
  args[Message::kSetIndex] = _chain_state[Message::kSetIndex];
  args[Message::kDirection] = _chain_state[Message::kDirection];

  Message req(RequestType::kShrinkSet);
  req[Message::kArgs] = args;
  _task_queue->send_msg_task(req);
}

void AppWorker::shrink_against_marked_task() {
  QVariantMap args;
  args[Message::kSetIndex] = _chain_state[Message::kSetIndex];
  args[Message::kDirections] = _chain_state[Message::kDirections];

  Message req(RequestType::kShrinkSetToMarked);
  req[Message::kArgs] = args;
  _task_queue->send_msg_task(req);
}

void AppWorker::perform_mouse_action_task() {
  QVariantMap args;
  args[Message::kMouseAction] = _chain_state[Message::kMouseAction];
  args[Message::kXPath] = _chain_state[Message::kXPath];
  args[Message::kOverlayRelClickPos] = _chain_state[Message::kOverlayRelClickPos];

  Message req(RequestType::kPerformMouseAction);
  req[Message::kArgs] = args;
  _task_queue->send_msg_task(req);
}

void AppWorker::perform_hover_action_task() {
  QVariantMap args;
  args[Message::kMouseAction] = to_underlying(MouseActionType::kMouseOver);
  args[Message::kXPath] = _hover_state[Message::kXPath];
  args[Message::kOverlayRelClickPos] = _hover_state[Message::kOverlayRelClickPos];

  Message req(RequestType::kPerformMouseAction);
  req[Message::kArgs] = args;
  _task_queue->send_msg_task(req);
}

void AppWorker::perform_text_action_task() {
  QVariantMap args;
  args[Message::kTextAction] = _chain_state[Message::kTextAction];
  args[Message::kXPath] = _chain_state[Message::kXPath];
  args[Message::kText] = _chain_state[Message::kText];

  Message req(RequestType::kPerformTextAction);
  req[Message::kArgs] = args;
  _task_queue->send_msg_task(req);
}

void AppWorker::perform_element_action_task() {
  QVariantMap args;
  args[Message::kElementAction] = _chain_state[Message::kElementAction];
  args[Message::kXPath] = _chain_state[Message::kXPath];
  args[Message::kOptionText] = _chain_state[Message::kOptionText]; // Used for selecting element from dropdowns.
  args[Message::kDirection] = _chain_state[Message::kDirection]; // Used for the scrolling directions.

  Message req(RequestType::kPerformElementAction);
  req[Message::kArgs] = args;
  _task_queue->send_msg_task(req);
}

void AppWorker::start_mouse_hover_task() {
  _hover_state = _chain_state;
  _hovering = true;
  _task_queue->run_next_task();
}

void AppWorker::stop_mouse_hover_task() {
  _hovering = false;
  _task_queue->run_next_task();
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
  AppTaskContext tc(_task_queue);
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
  bool success = _task_queue->get_last_response()[Message::kSuccess].toBool();
  if (!success) {
    _hovering = false;
  }
  _task_queue->run_next_task();
}

void AppWorker::emit_option_texts_task() {
  QStringList ot = _chain_state[Message::kOptionTexts].toStringList();
  emit select_option_texts(ot);
  _task_queue->run_next_task();
}

void AppWorker::reset_task() {
  reset_state();
  AppTaskContext tc(_task_queue);
  queue_check_browser_is_open(tc);
}


}
