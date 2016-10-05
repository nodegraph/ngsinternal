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

#include <guicomponents/comms/apprecorder.h>
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



AppRecorder::AppRecorder(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _app_worker(this),
      _graph_builder(this),
      _factory(this),
      _file_model(this),
      _compute(this) {
  get_dep_loader()->register_fixed_dep(_app_worker, Path({}));
  get_dep_loader()->register_fixed_dep(_graph_builder, Path({}));
  get_dep_loader()->register_fixed_dep(_factory, Path({}));
  get_dep_loader()->register_fixed_dep(_file_model, Path({}));

  _on_node_built = std::bind(&AppRecorder::on_node_built, this, std::placeholders::_1, std::placeholders::_2);
  _on_finished_sequence = std::bind(&AppRecorder::on_finished_sequence, this);
}

AppRecorder::~AppRecorder() {
}

void AppRecorder::clean_compute(Entity* compute_entity) {
  external();
  _compute = get_dep<Compute>(compute_entity);
  _compute->clean_state();
}

void AppRecorder::clean_compute(Dep<Compute>& compute) {
  external();
  _compute = compute;
  _compute->clean_state();
}

void AppRecorder::continue_cleaning_compute() {
  external();
  if (!_compute) {
    return;
  }
  if (!_compute->is_state_dirty()) {
    return;
  }
  _compute->clean_state();
}

void AppRecorder::on_node_built(Entity* node, Compute* compute) {
  std::cerr << "AppRec on node build: " << node << "," << compute << "\n";
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
  clean_compute(compute->our_entity());
}

void AppRecorder::on_finished_sequence() {
  std::cerr << "AppRect on finished sequence!\n";
  continue_cleaning_compute();
}

// -----------------------------------------------------------------
// // Record Browser Actions.
// -----------------------------------------------------------------

#define check_busy()   if (_app_worker->is_busy()) {emit web_action_ignored(); return;} TaskContext tc(_app_worker.get(), _on_finished_sequence);
#define finish()

void AppRecorder::record_open_browser() {
  check_busy();
  _app_worker->queue_build_compute_node(tc, ComponentDID::kOpenBrowserCompute, _on_node_built);
  finish();
}

void AppRecorder::record_close_browser() {
  check_busy();
  _app_worker->queue_build_compute_node(tc, ComponentDID::kCloseBrowserCompute, _on_node_built);
  finish();
}

void AppRecorder::record_check_browser_is_open() {
  check_busy();
  _app_worker->queue_build_compute_node(tc, ComponentDID::kCheckBrowserIsOpenCompute, _on_node_built);
  finish();
}

void AppRecorder::record_check_browser_size() {
  check_busy();
  int width = _file_model->get_work_setting(FileModel::kBrowserWidthRole).toInt();
  int height = _file_model->get_work_setting(FileModel::kBrowserHeightRole).toInt();

  QVariantMap dims;
  dims[Message::kWidth] = width;
  dims[Message::kHeight] = height;

  QVariantMap args;
  args[Message::kDimensions] = dims;

  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kCheckBrowserSizeCompute, _on_node_built);
  finish();
}

// -----------------------------------------------------------------
// Record Navigate Actions.
// -----------------------------------------------------------------

void AppRecorder::record_navigate_to(const QString& url) {
  check_busy()
  QString decorated_url = get_proper_url(url).toString();

  QVariantMap args;
  args[Message::kURL] = decorated_url;

  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kNavigateToCompute, _on_node_built);
  finish();
}

void AppRecorder::record_switch_to_iframe() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kSwitchToIFrameCompute, _on_node_built);
  finish();
}

void AppRecorder::record_navigate_refresh() {
  check_busy()
  _app_worker->queue_build_compute_node(tc, ComponentDID::kNavigateRefreshCompute, _on_node_built);
  finish();
}

// -----------------------------------------------------------------
// Record Create Set By Matching Values..
// -----------------------------------------------------------------

void AppRecorder::record_create_set_by_matching_text_values() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType]= to_underlying(WrapType::text);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromValuesCompute, _on_node_built);
  finish();
}

void AppRecorder::record_create_set_by_matching_image_values() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::image);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromValuesCompute, _on_node_built);
  finish();
}

// -----------------------------------------------------------------
// Record Create Set By Type.
// -----------------------------------------------------------------

void AppRecorder::record_create_set_of_inputs() {
  check_busy()
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::input);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromTypeCompute, _on_node_built);
  finish();
}

void AppRecorder::record_create_set_of_selects() {
  check_busy()
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::select);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromTypeCompute, _on_node_built);
  finish();
}

void AppRecorder::record_create_set_of_images() {
  check_busy()
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::image);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromTypeCompute, _on_node_built);
  finish();
}

void AppRecorder::record_create_set_of_text() {
  check_busy()
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::text);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromTypeCompute, _on_node_built);
  finish();
}

// -----------------------------------------------------------------
// Record Delete Set.
// -----------------------------------------------------------------

void AppRecorder::record_delete_set() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kDeleteSetCompute, _on_node_built);
  finish();
}

// -----------------------------------------------------------------
// Record Shift Sets.
// -----------------------------------------------------------------

void AppRecorder::record_shift_to_text_above() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::text);
  args[Message::kDirection] = to_underlying(Direction::up);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}
void AppRecorder::record_shift_to_text_below() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::text);
  args[Message::kDirection] = to_underlying(Direction::down);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}
void AppRecorder::record_shift_to_text_on_left() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::text);
  args[Message::kDirection] = to_underlying(Direction::left);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}
void AppRecorder::record_shift_to_text_on_right() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::text);
  args[Message::kDirection] = to_underlying(Direction::right);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}

void AppRecorder::record_shift_to_images_above() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::image);
  args[Message::kDirection] = to_underlying(Direction::up);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}
void AppRecorder::record_shift_to_images_below() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::image);
  args[Message::kDirection] = to_underlying(Direction::down);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}
void AppRecorder::record_shift_to_images_on_left() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::image);
  args[Message::kDirection] = to_underlying(Direction::left);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}
void AppRecorder::record_shift_to_images_on_right() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::image);
  args[Message::kDirection] = to_underlying(Direction::right);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}

void AppRecorder::record_shift_to_inputs_above() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::input);
  args[Message::kDirection] = to_underlying(Direction::up);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}
void AppRecorder::record_shift_to_inputs_below() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::input);
  args[Message::kDirection] = to_underlying(Direction::down);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}
void AppRecorder::record_shift_to_inputs_on_left() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::input);
  args[Message::kDirection] = to_underlying(Direction::left);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}
void AppRecorder::record_shift_to_inputs_on_right() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::input);
  args[Message::kDirection] = to_underlying(Direction::right);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}

void AppRecorder::record_shift_to_selects_above() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::select);
  args[Message::kDirection] = to_underlying(Direction::up);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}
void AppRecorder::record_shift_to_selects_below() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::select);
  args[Message::kDirection] = to_underlying(Direction::down);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}
void AppRecorder::record_shift_to_selects_on_left() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::select);
  args[Message::kDirection] = to_underlying(Direction::left);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}
void AppRecorder::record_shift_to_selects_on_right() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::select);
  args[Message::kDirection] = to_underlying(Direction::right);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}

void AppRecorder::record_shift_to_iframes_above() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::iframe);
  args[Message::kDirection] = to_underlying(Direction::up);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}
void AppRecorder::record_shift_to_iframes_below() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::iframe);
  args[Message::kDirection] = to_underlying(Direction::down);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}
void AppRecorder::record_shift_to_iframes_on_left() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::iframe);
  args[Message::kDirection] = to_underlying(Direction::left);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}
void AppRecorder::record_shift_to_iframes_on_right() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::iframe);
  args[Message::kDirection] = to_underlying(Direction::right);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute, _on_node_built);
  finish();
}

// -----------------------------------------------------------------
// Record Expand Sets.
// -----------------------------------------------------------------

void AppRecorder::record_expand_above() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap match_criteria;
  match_criteria[Message::kMatchLeft] = true;
  match_criteria[Message::kMatchRight] = false;
  match_criteria[Message::kMatchTop] = false;
  match_criteria[Message::kMatchBottom] = false;
  match_criteria[Message::kMatchFont] = true;
  match_criteria[Message::kMatchFontSize] = true;

  QVariantMap args;
  args[Message::kMatchCriteria] = match_criteria;
  args[Message::kDirection] = to_underlying(Direction::up);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kExpandSetCompute, _on_node_built);
  finish();
}

void AppRecorder::record_expand_below() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap match_criteria;
  match_criteria[Message::kMatchLeft] = true;
  match_criteria[Message::kMatchRight] = false;
  match_criteria[Message::kMatchTop] = false;
  match_criteria[Message::kMatchBottom] = false;
  match_criteria[Message::kMatchFont] = true;
  match_criteria[Message::kMatchFontSize] = true;

  QVariantMap args;
  args[Message::kMatchCriteria] = match_criteria;
  args[Message::kDirection] = to_underlying(Direction::down);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kExpandSetCompute, _on_node_built);
  finish();
}

void AppRecorder::record_expand_left() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap match_criteria;
  match_criteria[Message::kMatchLeft] = false;
  match_criteria[Message::kMatchRight] = false;
  match_criteria[Message::kMatchTop] = true;
  match_criteria[Message::kMatchBottom] = false;
  match_criteria[Message::kMatchFont] = true;
  match_criteria[Message::kMatchFontSize] = true;

  QVariantMap args;
  args[Message::kMatchCriteria] = match_criteria;
  args[Message::kDirection] = to_underlying(Direction::left);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kExpandSetCompute, _on_node_built);
  finish();
}

void AppRecorder::record_expand_right() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap match_criteria;
  match_criteria[Message::kMatchLeft] = false;
  match_criteria[Message::kMatchRight] = false;
  match_criteria[Message::kMatchTop] = true;
  match_criteria[Message::kMatchBottom] = false;
  match_criteria[Message::kMatchFont] = true;
  match_criteria[Message::kMatchFontSize] = true;

  QVariantMap args;
  args[Message::kMatchCriteria] = match_criteria;
  args[Message::kDirection] = to_underlying(Direction::right);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kExpandSetCompute, _on_node_built);
  finish();
}

// -----------------------------------------------------------------
// Record Mark Sets.
// -----------------------------------------------------------------

void AppRecorder::record_mark_set() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kMarkSetCompute, _on_node_built);
  finish();
}

void AppRecorder::record_unmark_set() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kUnmarkSetCompute, _on_node_built);
  finish();
}

void AppRecorder::record_merge_sets() {
  check_busy();
  _app_worker->queue_build_compute_node(tc, ComponentDID::kMergeSetsCompute, _on_node_built);
  finish();
}

// -----------------------------------------------------------------
// Record Shrink To One Side.
// -----------------------------------------------------------------

void AppRecorder::record_shrink_set_to_topmost() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kDirection] = to_underlying(Direction::up);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShrinkSetToSideCompute, _on_node_built);
  finish();
}

void AppRecorder::record_shrink_set_to_bottommost() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kDirection] = to_underlying(Direction::down);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShrinkSetToSideCompute, _on_node_built);
  finish();
}

void AppRecorder::record_shrink_set_to_leftmost() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kDirection] = to_underlying(Direction::left);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShrinkSetToSideCompute, _on_node_built);
  finish();
}

void AppRecorder::record_shrink_set_to_rightmost() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kDirection] = to_underlying(Direction::right);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShrinkSetToSideCompute, _on_node_built);
  finish();
}

// -----------------------------------------------------------------
// Record Shrink Against Marked Sets.
// -----------------------------------------------------------------

void AppRecorder::record_shrink_above_of_marked() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantList dirs;
  dirs.append(to_underlying(Direction::up));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute, _on_node_built);
  finish();
}

void AppRecorder::record_shrink_below_of_marked() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantList dirs;
  dirs.append(to_underlying(Direction::down));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute, _on_node_built);
  finish();
}

void AppRecorder::record_shrink_above_and_below_of_marked() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantList dirs;
  dirs.append(to_underlying(Direction::up));
  dirs.append(to_underlying(Direction::down));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute, _on_node_built);
  finish();
}

void AppRecorder::record_shrink_left_of_marked() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantList dirs;
  dirs.append(to_underlying(Direction::left));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute, _on_node_built);
  finish();
}

void AppRecorder::record_shrink_right_of_marked() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantList dirs;
  dirs.append(to_underlying(Direction::right));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute, _on_node_built);
  finish();
}

void AppRecorder::record_shrink_left_and_right_of_marked() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantList dirs;
  dirs.append(to_underlying(Direction::left));
  dirs.append(to_underlying(Direction::right));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  _app_worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute, _on_node_built);
  finish();
}

// -----------------------------------------------------------------
// Record Mouse Actions.
// -----------------------------------------------------------------

void AppRecorder::record_click() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kMouseAction] = to_underlying(MouseActionType::kSendClick);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kMouseActionCompute, _on_node_built);
  finish();
}

void AppRecorder::record_mouse_over() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kMouseAction] = to_underlying(MouseActionType::kMouseOver);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kMouseActionCompute, _on_node_built);
  finish();
}

void AppRecorder::record_start_mouse_hover() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kStartMouseHoverActionCompute, _on_node_built);
  finish();
}

void AppRecorder::record_stop_mouse_hover() {
  check_busy();
  _app_worker->queue_build_compute_node(tc, ComponentDID::kStopMouseHoverActionCompute, _on_node_built);
  finish();
}

// -----------------------------------------------------------------
// Record Text Actions.
// -----------------------------------------------------------------

void AppRecorder::record_type_text(const QString& text) {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kText] = text;
  args[Message::kTextAction] = to_underlying(TextActionType::kSendText);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kTextActionCompute, _on_node_built);
  finish();
}

void AppRecorder::record_type_enter() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kTextAction] = to_underlying(TextActionType::kSendEnter);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kTextActionCompute, _on_node_built);
  finish();
}

// -----------------------------------------------------------------
// Record Element Actions.
// -----------------------------------------------------------------

void AppRecorder::record_extract_text() {
  check_busy()
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kElementAction] = to_underlying(ElementActionType::kGetText);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute, _on_node_built);
  finish();
}

void AppRecorder::record_select_from_dropdown(const QString& option_text) {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kElementAction] = to_underlying(ElementActionType::kSelectOption);
  args[Message::kOptionText] = option_text;
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute, _on_node_built);
  finish();
}

void AppRecorder::record_scroll_down() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kElementAction] = to_underlying(ElementActionType::kScroll);
  args[Message::kDirection] = to_underlying(Direction::down);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute, _on_node_built);
  finish();
}

void AppRecorder::record_scroll_up() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kElementAction] = to_underlying(ElementActionType::kScroll);
  args[Message::kDirection] = to_underlying(Direction::up);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute, _on_node_built);
  finish();
}

void AppRecorder::record_scroll_right() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kElementAction] = to_underlying(ElementActionType::kScroll);
  args[Message::kDirection] = to_underlying(Direction::right);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute, _on_node_built);
  finish();
}

void AppRecorder::record_scroll_left() {
  check_busy();
  _app_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kElementAction] = to_underlying(ElementActionType::kScroll);
  args[Message::kDirection] = to_underlying(Direction::left);
  _app_worker->queue_merge_chain_state(tc, args);
  _app_worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute, _on_node_built);
  finish();
}

}
