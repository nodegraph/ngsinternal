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

#define check_busy()   if (is_busy()) {emit web_action_ignored(); return;}

const int AppWorker::kPollInterval = 1000;
const int AppWorker::kJitterSize = 1;

QUrl get_proper_url(const QString& input) {
  if (input.isEmpty()) {
    return QUrl::fromUserInput("about:blank");
  }
  const QUrl result = QUrl::fromUserInput(input);
  return result.isValid() ? result : QUrl::fromUserInput("about:blank");
}

class Linker: public Component {
 public:
  COMPONENT_ID(InvalidComponent, InvalidComponent);
  Linker(Entity* entity):Component(entity, kIID(), kDID()) {}
  // upstream and downstream are the output and input entities respectively.
  void link(Entity* downstream) {
    // Get the factory.
    Dep<BaseFactory> factory = get_dep<BaseFactory>(get_app_root());
    Entity* current_group = factory->get_current_group();

    // Get the current comp shape collective.
    Dep<CompShapeCollective> collective = get_dep<CompShapeCollective>(current_group);

    // Find the lowest node that's where we'll link to.
    Dep<CompShape> upstream_node = collective->get_lowest();
    if (!upstream_node) {
      return;
    }
    Entity* upstream = upstream_node->our_entity();

    // -------------------------------------------------------------------------
    // At this point we have both the upstream and downstream nodes to connect.
    // -------------------------------------------------------------------------

    std::cerr << "we now have both upstream and downstream\n";

    // Position our node below the upstream.
    Dep<NodeShape> downstream_node = get_dep<NodeShape>(downstream);
    glm::vec2 pos = upstream_node->get_pos();
    pos.y -= 700;
    downstream_node->set_pos(pos);

    // Get the exposed outputs from the upstream node.
    Dep<BaseOutputs> outputs = get_dep<BaseOutputs>(upstream);
    const std::unordered_map<std::string, Entity*>& exposed_outputs = outputs->get_exposed();

    // Get the exposed inputs from the downstream node.
    Dep<BaseInputs> inputs = get_dep<BaseInputs>(downstream);
    const std::unordered_map<std::string, Entity*>& exposed_inputs = inputs->get_exposed();

    // If we have exposed outputs and inputs then lets connect the first of each of them.
    if (!exposed_outputs.empty() && !exposed_inputs.empty()) {
      Entity* output = exposed_outputs.begin()->second;
      Entity* input = exposed_inputs.begin()->second;

      // Link the computes.
      Dep<OutputCompute> output_compute = get_dep<OutputCompute>(output);
      Dep<InputCompute> input_compute = get_dep<InputCompute>(input);
      input_compute->link_output_compute(output_compute);

      // Create a link.
      Entity* links_folder = current_group->get_entity(Path({".","links"}));
      Entity* link = factory->instance_entity(links_folder, "link", EntityDID::kLinkEntity);
      link->create_internals();

      // Link the link, input and output shapes.
      Dep<OutputShape> output_shape = get_dep<OutputShape>(output);
      Dep<InputShape> input_shape = get_dep<InputShape>(input);
      Dep<LinkShape> link_shape= get_dep<LinkShape>(link);
      link_shape->start_moving();
      link_shape->link_input_shape(input_shape);
      link_shape->link_output_shape(output_shape);
      link_shape->finished_moving();
    }
  }
};

AppWorker::AppWorker(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _app_comm(this),
      _file_model(this),
      _graph_builder(this),
      _factory(this),
      _compute(this),
      _node_selection(this),
      _show_browser(false),
      _hovering(false),
      _jitter(kJitterSize),
      _waiting_for_results(false),
      _next_msg_id(0),
      _connected(false) {
  get_dep_loader()->register_fixed_dep(_app_comm, Path({}));
  get_dep_loader()->register_fixed_dep(_file_model, Path({}));
  get_dep_loader()->register_fixed_dep(_graph_builder, Path({}));
  get_dep_loader()->register_fixed_dep(_factory, Path({}));
  get_dep_loader()->register_fixed_dep(_node_selection, Path({}));

  // Setup the poll timer.
  _poll_timer.setSingleShot(false);
  _poll_timer.setInterval(kPollInterval);
  connect(&_poll_timer,SIGNAL(timeout()),this,SLOT(on_poll()));
}

AppWorker::~AppWorker() {
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

void AppWorker::clean_compute(Dep<Compute>& compute) {
  external();
  _compute = compute;
  _compute->clean_state();
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
    _chain_state.clear();
    _queue.clear();

    // State for hovering.
    _hovering = false;
    _hover_state.clear();
    _jitter = kJitterSize;
}

//void AppWorker::send_json(const QString& json) {
//  Message msg(json);
//  send_msg(msg);
//}
//
//void AppWorker::send_map(const QVariantMap& map) {
//  Message msg(map);
//  send_msg(msg);
//}

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
      if (_queue.empty()) {
        queue_check_browser_is_open();
        queue_check_browser_size();
        // Debugging. - this makes the browser only come up once.
        _show_browser = false;
      }
    }
  }
  if (_hovering) {
    if (_queue.empty()) {
      mouse_hover_task();
    }
  }
}

// -----------------------------------------------------------------
// // Record Browser Actions.
// -----------------------------------------------------------------

void AppWorker::record_open_browser() {
  check_busy();
  queue_build_compute_node(ComponentDID::kOpenBrowserCompute);
}

void AppWorker::record_close_browser() {
  check_busy();
  queue_build_compute_node(ComponentDID::kCloseBrowserCompute);
}

void AppWorker::record_check_browser_is_open() {
  check_busy();
  queue_build_compute_node(ComponentDID::kCheckBrowserIsOpenCompute);
}

void AppWorker::record_check_browser_size() {
  int width = _file_model->get_work_setting(FileModel::kBrowserWidthRole).toInt();
  int height = _file_model->get_work_setting(FileModel::kBrowserHeightRole).toInt();

  QVariantMap dims;
  dims[Message::kWidth] = width;
  dims[Message::kHeight] = height;

  QVariantMap args;
  args[Message::kDimensions] = dims;

  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kCheckBrowserSizeCompute);
}

// -----------------------------------------------------------------
// Record Navigate Actions.
// -----------------------------------------------------------------

void AppWorker::record_navigate_to(const QString& url) {
  check_busy()
  QString decorated_url = get_proper_url(url).toString();

  QVariantMap args;
  args[Message::kURL] = decorated_url;

  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kNavigateToCompute);
}

void AppWorker::record_switch_to_iframe() {
  check_busy()
  queue_get_crosshair_info();
  queue_build_compute_node(ComponentDID::kSwitchToIFrameCompute);
}

void AppWorker::record_navigate_refresh() {
  check_busy()
  queue_build_compute_node(ComponentDID::kNavigateRefreshCompute);
}

// -----------------------------------------------------------------
// Record Create Set By Matching Values..
// -----------------------------------------------------------------

void AppWorker::record_create_set_by_matching_text_values() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::text);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kCreateSetFromValuesCompute);
}

void AppWorker::record_create_set_by_matching_image_values() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::image);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kCreateSetFromValuesCompute);
}

// -----------------------------------------------------------------
// Record Create Set By Type.
// -----------------------------------------------------------------

void AppWorker::record_create_set_of_inputs() {
  check_busy()
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::input);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kCreateSetFromTypeCompute);
}

void AppWorker::record_create_set_of_selects() {
  check_busy()
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::select);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kCreateSetFromTypeCompute);
}

void AppWorker::record_create_set_of_images() {
  check_busy()
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::image);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kCreateSetFromTypeCompute);
}

void AppWorker::record_create_set_of_text() {
  check_busy()
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::text);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kCreateSetFromTypeCompute);
}

// -----------------------------------------------------------------
// Record Delete Set.
// -----------------------------------------------------------------

void AppWorker::record_delete_set() {
  check_busy()
  queue_get_crosshair_info();
  queue_build_compute_node(ComponentDID::kDeleteSetCompute);
}

// -----------------------------------------------------------------
// Record Shift Sets.
// -----------------------------------------------------------------

void AppWorker::record_shift_to_text_above() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::text);
  args[Message::kDirection].setValue(Direction::up);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}
void AppWorker::record_shift_to_text_below() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::text);
  args[Message::kDirection].setValue(Direction::down);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}
void AppWorker::record_shift_to_text_on_left() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::text);
  args[Message::kDirection].setValue(Direction::left);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}
void AppWorker::record_shift_to_text_on_right() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::text);
  args[Message::kDirection].setValue(Direction::right);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}

void AppWorker::record_shift_to_images_above() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::image);
  args[Message::kDirection].setValue(Direction::up);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}
void AppWorker::record_shift_to_images_below() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::image);
  args[Message::kDirection].setValue(Direction::down);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}
void AppWorker::record_shift_to_images_on_left() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::image);
  args[Message::kDirection].setValue(Direction::left);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}
void AppWorker::record_shift_to_images_on_right() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::image);
  args[Message::kDirection].setValue(Direction::right);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}

void AppWorker::record_shift_to_inputs_above() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::input);
  args[Message::kDirection].setValue(Direction::up);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}
void AppWorker::record_shift_to_inputs_below() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::input);
  args[Message::kDirection].setValue(Direction::down);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}
void AppWorker::record_shift_to_inputs_on_left() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::input);
  args[Message::kDirection].setValue(Direction::left);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}
void AppWorker::record_shift_to_inputs_on_right() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::input);
  args[Message::kDirection].setValue(Direction::right);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}

void AppWorker::record_shift_to_selects_above() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::select);
  args[Message::kDirection].setValue(Direction::up);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}
void AppWorker::record_shift_to_selects_below() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::select);
  args[Message::kDirection].setValue(Direction::down);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}
void AppWorker::record_shift_to_selects_on_left() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::select);
  args[Message::kDirection].setValue(Direction::left);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}
void AppWorker::record_shift_to_selects_on_right() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::select);
  args[Message::kDirection].setValue(Direction::right);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}

void AppWorker::record_shift_to_iframes_above() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::iframe);
  args[Message::kDirection].setValue(Direction::up);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}
void AppWorker::record_shift_to_iframes_below() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::iframe);
  args[Message::kDirection].setValue(Direction::down);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}
void AppWorker::record_shift_to_iframes_on_left() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::iframe);
  args[Message::kDirection].setValue(Direction::left);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}
void AppWorker::record_shift_to_iframes_on_right() {
  check_busy()
  queue_get_crosshair_info();
  QVariantMap args;
  args[Message::kWrapType].setValue(WrapType::iframe);
  args[Message::kDirection].setValue(Direction::right);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShiftSetCompute);
}

// -----------------------------------------------------------------
// Record Expand Sets.
// -----------------------------------------------------------------

void AppWorker::record_expand_above() {
  check_busy();
  queue_get_crosshair_info();

  QVariantMap match_criteria;
  match_criteria[Message::kMatchLeft] = true;
  match_criteria[Message::kMatchRight] = false;
  match_criteria[Message::kMatchTop] = false;
  match_criteria[Message::kMatchBottom] = false;
  match_criteria[Message::kMatchFont] = true;
  match_criteria[Message::kMatchFontSize] = true;

  QVariantMap args;
  args[Message::kMatchCriteria] = match_criteria;
  args[Message::kDirection].setValue(Direction::up);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kExpandSetCompute);
}

void AppWorker::record_expand_below() {
  check_busy();
  queue_get_crosshair_info();

  QVariantMap match_criteria;
  match_criteria[Message::kMatchLeft] = true;
  match_criteria[Message::kMatchRight] = false;
  match_criteria[Message::kMatchTop] = false;
  match_criteria[Message::kMatchBottom] = false;
  match_criteria[Message::kMatchFont] = true;
  match_criteria[Message::kMatchFontSize] = true;

  QVariantMap args;
  args[Message::kMatchCriteria] = match_criteria;
  args[Message::kDirection].setValue(Direction::down);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kExpandSetCompute);
}

void AppWorker::record_expand_left() {
  check_busy();
  queue_get_crosshair_info();

  QVariantMap match_criteria;
  match_criteria[Message::kMatchLeft] = false;
  match_criteria[Message::kMatchRight] = false;
  match_criteria[Message::kMatchTop] = true;
  match_criteria[Message::kMatchBottom] = false;
  match_criteria[Message::kMatchFont] = true;
  match_criteria[Message::kMatchFontSize] = true;

  QVariantMap args;
  args[Message::kMatchCriteria] = match_criteria;
  args[Message::kDirection].setValue(Direction::left);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kExpandSetCompute);
}

void AppWorker::record_expand_right() {
  check_busy();
  queue_get_crosshair_info();

  QVariantMap match_criteria;
  match_criteria[Message::kMatchLeft] = false;
  match_criteria[Message::kMatchRight] = false;
  match_criteria[Message::kMatchTop] = true;
  match_criteria[Message::kMatchBottom] = false;
  match_criteria[Message::kMatchFont] = true;
  match_criteria[Message::kMatchFontSize] = true;

  QVariantMap args;
  args[Message::kMatchCriteria] = match_criteria;
  args[Message::kDirection].setValue(Direction::right);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kExpandSetCompute);
}

// -----------------------------------------------------------------
// Record Mark Sets.
// -----------------------------------------------------------------

void AppWorker::record_mark_set() {
  check_busy();
  queue_get_crosshair_info();
  queue_build_compute_node(ComponentDID::kMarkSetCompute);
}

void AppWorker::record_unmark_set() {
  check_busy();
  queue_get_crosshair_info();
  queue_build_compute_node(ComponentDID::kUnmarkSetCompute);
}

void AppWorker::record_merge_sets() {
  check_busy();
  queue_build_compute_node(ComponentDID::kMergeSetsCompute);
}

// -----------------------------------------------------------------
// Record Shrink To One Side.
// -----------------------------------------------------------------

void AppWorker::record_shrink_set_to_topmost() {
  check_busy();
  queue_get_crosshair_info();

  QVariantMap args;
  args[Message::kDirection].setValue(Direction::up);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShrinkSetToSideCompute);
}

void AppWorker::record_shrink_set_to_bottommost() {
  check_busy();
  queue_get_crosshair_info();

  QVariantMap args;
  args[Message::kDirection].setValue(Direction::down);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShrinkSetToSideCompute);
}

void AppWorker::record_shrink_set_to_leftmost() {
  check_busy();
  queue_get_crosshair_info();

  QVariantMap args;
  args[Message::kDirection].setValue(Direction::left);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShrinkSetToSideCompute);
}

void AppWorker::record_shrink_set_to_rightmost() {
  check_busy();
  queue_get_crosshair_info();

  QVariantMap args;
  args[Message::kDirection].setValue(Direction::right);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kShrinkSetToSideCompute);
}

// -----------------------------------------------------------------
// Record Shrink Against Marked Sets.
// -----------------------------------------------------------------

void AppWorker::record_shrink_above_of_marked() {
  check_busy();
  queue_get_crosshair_info();

  QVariantList dirs;
  dirs.append(QVariant::fromValue(Direction::up));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  queue_build_compute_node(ComponentDID::kShrinkAgainstMarkedCompute);
}

void AppWorker::record_shrink_below_of_marked() {
  check_busy();
  queue_get_crosshair_info();

  QVariantList dirs;
  dirs.append(QVariant::fromValue(Direction::down));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  queue_build_compute_node(ComponentDID::kShrinkAgainstMarkedCompute);
}

void AppWorker::record_shrink_above_and_below_of_marked() {
  check_busy();
  queue_get_crosshair_info();

  QVariantList dirs;
  dirs.append(QVariant::fromValue(Direction::up));
  dirs.append(QVariant::fromValue(Direction::down));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  queue_build_compute_node(ComponentDID::kShrinkAgainstMarkedCompute);
}

void AppWorker::record_shrink_left_of_marked() {
  check_busy();
  queue_get_crosshair_info();

  QVariantList dirs;
  dirs.append(QVariant::fromValue(Direction::left));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  queue_build_compute_node(ComponentDID::kShrinkAgainstMarkedCompute);
}

void AppWorker::record_shrink_right_of_marked() {
  check_busy();
  queue_get_crosshair_info();

  QVariantList dirs;
  dirs.append(QVariant::fromValue(Direction::right));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  queue_build_compute_node(ComponentDID::kShrinkAgainstMarkedCompute);
}

void AppWorker::record_shrink_left_and_right_of_marked() {
  check_busy();
  queue_get_crosshair_info();

  QVariantList dirs;
  dirs.append(QVariant::fromValue(Direction::left));
  dirs.append(QVariant::fromValue(Direction::right));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  queue_build_compute_node(ComponentDID::kShrinkAgainstMarkedCompute);
}

// -----------------------------------------------------------------
// Record Mouse Actions.
// -----------------------------------------------------------------

void AppWorker::record_click() {
  check_busy();
  queue_get_crosshair_info();

  QVariantMap args;
  args[Message::kMouseAction].setValue(MouseActionType::kSendClick);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kMouseActionCompute);
}

void AppWorker::record_mouse_over() {
  check_busy();
  queue_get_crosshair_info();

  QVariantMap args;
  args[Message::kMouseAction].setValue(MouseActionType::kMouseOver);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kMouseActionCompute);
}

void AppWorker::record_start_mouse_hover() {
  check_busy();
  queue_get_crosshair_info();
  queue_build_compute_node(ComponentDID::kStartMouseHoverActionCompute);
}

void AppWorker::record_stop_mouse_hover() {
  check_busy();
  queue_build_compute_node(ComponentDID::kStopMouseHoverActionCompute);
}

// -----------------------------------------------------------------
// Record Text Actions.
// -----------------------------------------------------------------

void AppWorker::record_type_text(const QString& text) {
  check_busy();
  queue_get_crosshair_info();

  QVariantMap args;
  args[Message::kText] = text;
  args[Message::kTextAction].setValue(TextActionType::kSendText);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kTextActionCompute);
}

void AppWorker::record_type_enter() {
  check_busy();
  queue_get_crosshair_info();

  QVariantMap args;
  args[Message::kTextAction].setValue(TextActionType::kSendEnter);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kTextActionCompute);
}

// -----------------------------------------------------------------
// Record Element Actions.
// -----------------------------------------------------------------

void AppWorker::record_extract_text() {
  check_busy()
  queue_get_crosshair_info();

  QVariantMap args;
  args[Message::kElementAction].setValue(ElementActionType::kGetText);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kElementActionCompute);
}

void AppWorker::record_select_from_dropdown(const QString& option_text) {
  check_busy();
  queue_get_crosshair_info();

  QVariantMap args;
  args[Message::kElementAction].setValue(ElementActionType::kSelectOption);
  args[Message::kOptionText] = option_text;
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kElementActionCompute);
}

void AppWorker::record_scroll_down() {
  check_busy();
  queue_get_crosshair_info();

  QVariantMap args;
  args[Message::kElementAction].setValue(ElementActionType::kScroll);
  args[Message::kDirection].setValue(Direction::down);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kElementActionCompute);
}

void AppWorker::record_scroll_up() {
  check_busy();
  queue_get_crosshair_info();

  QVariantMap args;
  args[Message::kElementAction].setValue(ElementActionType::kScroll);
  args[Message::kDirection].setValue(Direction::up);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kElementActionCompute);
}

void AppWorker::record_scroll_right() {
  check_busy();
  queue_get_crosshair_info();

  QVariantMap args;
  args[Message::kElementAction].setValue(ElementActionType::kScroll);
  args[Message::kDirection].setValue(Direction::right);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kElementActionCompute);
}

void AppWorker::record_scroll_left() {
  check_busy();
  queue_get_crosshair_info();

  QVariantMap args;
  args[Message::kElementAction].setValue(ElementActionType::kScroll);
  args[Message::kDirection].setValue(Direction::left);
  queue_merge_chain_state(args);
  queue_build_compute_node(ComponentDID::kElementActionCompute);
}

// ---------------------------------------------------------------------------------
// Queue Framework Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_get_xpath() {
  queue_task((AppTask)std::bind(&AppWorker::get_xpath_task,this), "queue_get_xpath");
}

void AppWorker::queue_get_crosshair_info() {
  queue_task((AppTask)std::bind(&AppWorker::get_crosshair_info_task,this), "queue_get_crosshair_info");
}

void AppWorker::queue_merge_chain_state(const QVariantMap& map) {
  queue_task((AppTask)std::bind(&AppWorker::merge_chain_state_task,this, map), "queue_merge_chain_state");
}

void AppWorker::queue_build_compute_node(ComponentDID compute_did) {
  std::stringstream ss;
  ss << "queue build compute node with did: " << (size_t)compute_did;
  queue_task((AppTask)std::bind(&AppWorker::build_compute_node_task,this, compute_did), ss.str());
}

void AppWorker::queue_finished(std::function<void(const QVariantMap&)> finalize_update) {
  queue_task((AppTask)std::bind(&AppWorker::finished_task,this,finalize_update), "queue_finished");
}

// ---------------------------------------------------------------------------------
// Queue Cookie Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_get_all_cookies() {
  queue_task((AppTask)std::bind(&AppWorker::get_all_cookies_task, this), "queue_get_all_cookies");
}

void AppWorker::queue_clear_all_cookies() {
  queue_task((AppTask)std::bind(&AppWorker::clear_all_cookies_task, this), "queue_clear_all_cookies");
}

void AppWorker::queue_set_all_cookies() {
  queue_task((AppTask)std::bind(&AppWorker::set_all_cookies_task, this), "queue_set_all_cookies");
}

// ---------------------------------------------------------------------------------
// Queue Browser Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_open_browser() {
  queue_task((AppTask)std::bind(&AppWorker::open_browser_task,this), "queue_open_browser");
}

void AppWorker::queue_close_browser() {
  queue_task((AppTask)std::bind(&AppWorker::close_browser_task,this), "queue_close_browser");
}

void AppWorker::queue_check_browser_is_open() {
  queue_task((AppTask)std::bind(&AppWorker::check_browser_is_open_task,this), "queue_check_browser_is_open");
}

void AppWorker::queue_check_browser_size() {
  queue_task((AppTask)std::bind(&AppWorker::check_browser_size_task,this), "queue_check_browser_size");
}

void AppWorker::queue_reset() {
  queue_task((AppTask)std::bind(&AppWorker::reset_task, this), "reset");
}

// ---------------------------------------------------------------------------------
// Queue Page Content Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_block_events() {
  queue_task((AppTask)std::bind(&AppWorker::block_events_task, this), "reset");
}

void AppWorker::queue_unblock_events() {
  queue_task((AppTask)std::bind(&AppWorker::unblock_events_task, this), "reset");
}

// ---------------------------------------------------------------------------------
// Queue Navigate Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_navigate_to() {
  queue_task((AppTask)std::bind(&AppWorker::navigate_to_task,this), "queue_navigate_to");
}

void AppWorker::queue_navigate_refresh() {
  queue_task((AppTask)std::bind(&AppWorker::navigate_refresh_task,this), "queue_navigate_refresh");
}

void AppWorker::queue_switch_to_iframe() {
  queue_task((AppTask)std::bind(&AppWorker::switch_to_iframe_task,this), "queue_swith_to_iframe");
}

// ---------------------------------------------------------------------------------
// Queue Set Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_update_overlays() {
  queue_task((AppTask)std::bind(&AppWorker::update_overlays_task, this), "queue_update_overlays");
}

void AppWorker::queue_create_set_by_matching_values() {
  queue_task((AppTask)std::bind(&AppWorker::create_set_by_matching_values_task,this), "queue_create_set_by_matching_values");
}

void AppWorker::queue_create_set_by_matching_type() {
  queue_task((AppTask)std::bind(&AppWorker::create_set_by_matching_type_task,this), "queue_create_set_by_matching_type");
}

void AppWorker::queue_delete_set() {
  queue_task((AppTask)std::bind(&AppWorker::delete_set_task,this), "queue_delete_set");
}

void AppWorker::queue_shift_set() {
  queue_task((AppTask)std::bind(&AppWorker::shift_set_task,this), "queue_shift_set");
}

void AppWorker::queue_expand_set() {
  queue_task((AppTask)std::bind(&AppWorker::expand_set_task,this), "queue_expand_set");
}

void AppWorker::queue_mark_set() {
  queue_task((AppTask)std::bind(&AppWorker::mark_set_task,this), "queue_mark_set");
}

void AppWorker::queue_unmark_set() {
  queue_task((AppTask)std::bind(&AppWorker::unmark_set_task,this), "queue_unmark_set");
}

void AppWorker::queue_merge_sets() {
  queue_task((AppTask)std::bind(&AppWorker::merge_sets_task, this), "queue_merge_sets");
}

void AppWorker::queue_shrink_set_to_side() {
  queue_task((AppTask)std::bind(&AppWorker::shrink_set_to_side_task, this), "queue_merge_sets");
}

void AppWorker::queue_shrink_against_marked() {
  queue_task((AppTask)std::bind(&AppWorker::shrink_against_marked_task,this), "queue_shrink_against_marked");
}

// ---------------------------------------------------------------------------------
// Queue Perform Action Tasks.
// ---------------------------------------------------------------------------------

void AppWorker::queue_perform_mouse_action() {
  queue_get_xpath();
  queue_unblock_events();
  queue_task((AppTask)std::bind(&AppWorker::perform_mouse_action_task,this), "queue_perform_action_task");
  queue_block_events();
}

void AppWorker::queue_perform_mouse_hover() {
  queue_get_xpath();
  queue_unblock_events();
  queue_task((AppTask)std::bind(&AppWorker::perform_hover_action_task,this), "queue_perform_hover");
  queue_block_events();
}

void AppWorker::queue_perform_post_mouse_hover() {
  queue_task((AppTask)std::bind(&AppWorker::post_hover_task, this), "queue_perform_post_hover");
}

void AppWorker::queue_perform_text_action() {
  queue_get_xpath();
  queue_unblock_events();
  queue_task((AppTask)std::bind(&AppWorker::perform_text_action_task,this), "queue_perform_text_action");
  queue_block_events();
}

void AppWorker::queue_perform_element_action() {
  queue_get_xpath();
  queue_unblock_events();
  queue_task((AppTask)std::bind(&AppWorker::perform_element_action_task,this), "queue_perform_element_action");
  queue_block_events();
}

// ---------------------------------------------------------------------------------
// Queue other actions.
// ---------------------------------------------------------------------------------

void AppWorker::queue_start_mouse_hover() {
  queue_get_xpath();
  queue_task((AppTask)std::bind(&AppWorker::start_mouse_hover_task,this), "queue_stop_mouse_hover");
}

void AppWorker::queue_stop_mouse_hover() {
  queue_task((AppTask)std::bind(&AppWorker::stop_mouse_hover_task,this), "queue_stop_mouse_hover");
}

void AppWorker::queue_emit_option_texts() {
  queue_get_crosshair_info();
  queue_task((AppTask)std::bind(&AppWorker::emit_option_texts_task,this), "select_from_dropdown3");
}



// ------------------------------------------------------------------------
// Task Management.
// ------------------------------------------------------------------------

void AppWorker::queue_task(AppTask task, const std::string& about) {
  const AppTask test = task;
  _queue.push_back(test);
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

  // Merge the values into the chain_state.
  QVariantMap &value = _last_resp[Message::kValue].toMap();
  for (QVariantMap::const_iterator iter = value.constBegin(); iter != value.constEnd(); ++iter) {
    _chain_state.insert(iter.key(), iter.value());
  }

  // Run the next task.
  run_next_task();
}

void AppWorker::handle_info_from_nodejs(const Message& msg) {
  std::cerr << "commhub --> app: info: " << msg.to_string().toStdString() << "\n";
  if (msg[Message::kInfo] == QVariant::fromValue(InfoType::kShowWebActionMenu)) {
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
  _waiting_for_results = true;
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

void AppWorker::finished_task(std::function<void(const QVariantMap&)> finalize_update) {
  finalize_update(_chain_state["outputs"].toMap());
  // If the compute is still dirty, it's because one of the dependencies has an
  // asynchronous update_state() method. In this case just run the cleaning processing again.
  if (_compute) {
    if (!_compute->is_state_dirty()) {
      //
    } else {
      _compute->clean_state();
    }
  }
  run_next_task();
}

void AppWorker::build_compute_node_task(ComponentDID compute_did) {
  // Create the node.
  Entity* group = _factory->get_current_group();
  Entity* node = _factory->instance_entity(group, "click", EntityDID::kComputeNodeEntity);
  node->create_internals();

  // Add the compute to the node.
  Compute* compute = static_cast<Compute*>(_factory->instance_component(node, compute_did));
  compute->create_inputs_outputs();

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
    compute->set_value(iter.value());
  }

  // Position and link the node we just created.
  // We use a dummy component to avoid creating a cycle.
  {
    Linker* linker = new_ff Linker(get_app_root());
    linker->link(node);
    delete_ff(linker);
  }

  // Clean the wires in this group, as new nodes and links were created.
  _factory->get_current_group()->clean_wires();

  // Now execute it.
  compute->clean_state();

  run_next_task();
}


// ------------------------------------------------------------------------
// Cookie Tasks.
// ------------------------------------------------------------------------

void AppWorker::get_all_cookies_task() {
  check_busy()
  Message req(RequestType::kGetAllCookies);
  send_msg_task(req);
}

void AppWorker::clear_all_cookies_task() {
  check_busy()
  Message req(RequestType::kClearAllCookies);
  send_msg_task(req);
}

void AppWorker::set_all_cookies_task() {
  check_busy()
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
  {
    Message req(RequestType::kBlockEvents);
    send_msg_task(req);
  }
  {
    Message req(RequestType::kUpdateOveralys);
    send_msg_task(req);
  }
}

void AppWorker::unblock_events_task() {
  Message req(RequestType::kUnblockEvents);
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
  queue_reset();
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
  check_busy()
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
  args[Message::kMouseAction].setValue(MouseActionType::kMouseOver);
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
  queue_unblock_events();
  queue_perform_mouse_hover();
  queue_perform_post_mouse_hover();
  queue_block_events();
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
  queue_check_browser_is_open();
}


}
