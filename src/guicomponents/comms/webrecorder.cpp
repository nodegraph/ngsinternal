#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>

#include <components/computes/compute.h>

#include <guicomponents/comms/webrecorder.h>
#include <guicomponents/comms/webworker.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/filemodel.h>
#include <guicomponents/quick/nodegraphmanipulator.h>

#include <QtCore/QUrl>

namespace ngs {

QUrl get_proper_url(const QString& input) {
  if (input.isEmpty()) {
    return QUrl::fromUserInput("about:blank");
  }
  const QUrl result = QUrl::fromUserInput(input);
  return result.isValid() ? result : QUrl::fromUserInput("about:blank");
}

WebRecorder::WebRecorder(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _web_worker(this),
      _task_scheduler(this),
      _file_model(this) {
  get_dep_loader()->register_fixed_dep(_web_worker, Path({}));
  get_dep_loader()->register_fixed_dep(_task_scheduler, Path({}));
  get_dep_loader()->register_fixed_dep(_file_model, Path({}));
}

WebRecorder::~WebRecorder() {
}

void WebRecorder::initialize_wires() {
  Component::initialize_wires();
}

// -----------------------------------------------------------------
// // Record Browser Actions.
// -----------------------------------------------------------------

#define check_busy()   if (_task_scheduler->is_busy()) {emit web_action_ignored(); return;} TaskContext tc(_task_scheduler);
#define finish()

void WebRecorder::record_open_browser() {
  check_busy();
  _web_worker->queue_build_compute_node(tc, ComponentDID::kOpenBrowserCompute);
  finish();
}

void WebRecorder::record_close_browser() {
  check_busy();
  _web_worker->queue_build_compute_node(tc, ComponentDID::kCloseBrowserCompute);
  finish();
}

void WebRecorder::record_is_browser_open() {
  check_busy();
  _web_worker->queue_build_compute_node(tc, ComponentDID::kIsBrowserOpenCompute);
  finish();
}

void WebRecorder::record_check_browser_size() {
  check_busy();
  int width = _file_model->get_work_setting(FileModel::kBrowserWidthRole).toInt();
  int height = _file_model->get_work_setting(FileModel::kBrowserHeightRole).toInt();

  QVariantMap dims;
  dims[Message::kWidth] = width;
  dims[Message::kHeight] = height;

  QVariantMap args;
  args[Message::kDimensions] = dims;

  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kResizeBrowserCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Navigate Actions.
// -----------------------------------------------------------------

void WebRecorder::record_navigate_to(const QString& url) {
  check_busy()
  QString decorated_url = get_proper_url(url).toString();

  QVariantMap args;
  args[Message::kURL] = decorated_url;

  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kNavigateToCompute);
  finish();
}

void WebRecorder::record_switch_to_iframe() {
  check_busy()
  QVariantMap args;
  args[Message::kIFrame]= _web_worker->get_iframe_to_switch_to();
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kSwitchToIFrameCompute);
  finish();
}

void WebRecorder::record_navigate_refresh() {
  check_busy()
  _web_worker->queue_build_compute_node(tc, ComponentDID::kNavigateRefreshCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Create Set By Matching Values..
// -----------------------------------------------------------------

void WebRecorder::record_create_set_by_matching_text_values() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType]= to_underlying(WrapType::text);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromValuesCompute);
  finish();
}

void WebRecorder::record_create_set_by_matching_image_values() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::image);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromValuesCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Create Set By Type.
// -----------------------------------------------------------------

void WebRecorder::record_create_set_of_inputs() {
  check_busy()
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::input);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromTypeCompute);
  finish();
}

void WebRecorder::record_create_set_of_selects() {
  check_busy()
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::select);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromTypeCompute);
  finish();
}

void WebRecorder::record_create_set_of_images() {
  check_busy()
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::image);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromTypeCompute);
  finish();
}

void WebRecorder::record_create_set_of_text() {
  check_busy()
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::text);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromTypeCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Delete Set.
// -----------------------------------------------------------------

void WebRecorder::record_delete_set() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kDeleteSetCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Shift Sets.
// -----------------------------------------------------------------

void WebRecorder::record_shift_to_text_above() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::text);
  args[Message::kDirection] = to_underlying(Direction::up);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void WebRecorder::record_shift_to_text_below() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::text);
  args[Message::kDirection] = to_underlying(Direction::down);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void WebRecorder::record_shift_to_text_on_left() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::text);
  args[Message::kDirection] = to_underlying(Direction::left);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void WebRecorder::record_shift_to_text_on_right() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::text);
  args[Message::kDirection] = to_underlying(Direction::right);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}

void WebRecorder::record_shift_to_images_above() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::image);
  args[Message::kDirection] = to_underlying(Direction::up);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void WebRecorder::record_shift_to_images_below() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::image);
  args[Message::kDirection] = to_underlying(Direction::down);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void WebRecorder::record_shift_to_images_on_left() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::image);
  args[Message::kDirection] = to_underlying(Direction::left);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void WebRecorder::record_shift_to_images_on_right() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::image);
  args[Message::kDirection] = to_underlying(Direction::right);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}

void WebRecorder::record_shift_to_inputs_above() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::input);
  args[Message::kDirection] = to_underlying(Direction::up);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void WebRecorder::record_shift_to_inputs_below() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::input);
  args[Message::kDirection] = to_underlying(Direction::down);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void WebRecorder::record_shift_to_inputs_on_left() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::input);
  args[Message::kDirection] = to_underlying(Direction::left);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void WebRecorder::record_shift_to_inputs_on_right() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::input);
  args[Message::kDirection] = to_underlying(Direction::right);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}

void WebRecorder::record_shift_to_selects_above() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::select);
  args[Message::kDirection] = to_underlying(Direction::up);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void WebRecorder::record_shift_to_selects_below() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::select);
  args[Message::kDirection] = to_underlying(Direction::down);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void WebRecorder::record_shift_to_selects_on_left() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::select);
  args[Message::kDirection] = to_underlying(Direction::left);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void WebRecorder::record_shift_to_selects_on_right() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::select);
  args[Message::kDirection] = to_underlying(Direction::right);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}

void WebRecorder::record_shift_to_iframes_above() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::iframe);
  args[Message::kDirection] = to_underlying(Direction::up);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void WebRecorder::record_shift_to_iframes_below() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::iframe);
  args[Message::kDirection] = to_underlying(Direction::down);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void WebRecorder::record_shift_to_iframes_on_left() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::iframe);
  args[Message::kDirection] = to_underlying(Direction::left);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void WebRecorder::record_shift_to_iframes_on_right() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);
  QVariantMap args;
  args[Message::kWrapType] = to_underlying(WrapType::iframe);
  args[Message::kDirection] = to_underlying(Direction::right);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Expand Sets.
// -----------------------------------------------------------------

void WebRecorder::record_expand_above() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

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
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kExpandSetCompute);
  finish();
}

void WebRecorder::record_expand_below() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

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
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kExpandSetCompute);
  finish();
}

void WebRecorder::record_expand_left() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

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
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kExpandSetCompute);
  finish();
}

void WebRecorder::record_expand_right() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

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
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kExpandSetCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Mark Sets.
// -----------------------------------------------------------------

void WebRecorder::record_mark_set() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kMarkSetCompute);
  finish();
}

void WebRecorder::record_unmark_set() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kUnmarkSetCompute);
  finish();
}

void WebRecorder::record_merge_sets() {
  check_busy();
  _web_worker->queue_build_compute_node(tc, ComponentDID::kMergeSetsCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Shrink To One Side.
// -----------------------------------------------------------------

void WebRecorder::record_shrink_set_to_topmost() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kDirection] = to_underlying(Direction::up);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShrinkSetToSideCompute);
  finish();
}

void WebRecorder::record_shrink_set_to_bottommost() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kDirection] = to_underlying(Direction::down);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShrinkSetToSideCompute);
  finish();
}

void WebRecorder::record_shrink_set_to_leftmost() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kDirection] = to_underlying(Direction::left);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShrinkSetToSideCompute);
  finish();
}

void WebRecorder::record_shrink_set_to_rightmost() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kDirection] = to_underlying(Direction::right);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShrinkSetToSideCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Shrink Against Marked Sets.
// -----------------------------------------------------------------

void WebRecorder::record_shrink_above_of_marked() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantList dirs;
  dirs.append(to_underlying(Direction::up));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute);
  finish();
}

void WebRecorder::record_shrink_below_of_marked() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantList dirs;
  dirs.append(to_underlying(Direction::down));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute);
  finish();
}

void WebRecorder::record_shrink_above_and_below_of_marked() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantList dirs;
  dirs.append(to_underlying(Direction::up));
  dirs.append(to_underlying(Direction::down));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute);
  finish();
}

void WebRecorder::record_shrink_left_of_marked() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantList dirs;
  dirs.append(to_underlying(Direction::left));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute);
  finish();
}

void WebRecorder::record_shrink_right_of_marked() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantList dirs;
  dirs.append(to_underlying(Direction::right));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute);
  finish();
}

void WebRecorder::record_shrink_left_and_right_of_marked() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantList dirs;
  dirs.append(to_underlying(Direction::left));
  dirs.append(to_underlying(Direction::right));
  QVariantMap args;
  args[Message::kDirections] = dirs;
  _web_worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Mouse Actions.
// -----------------------------------------------------------------

void WebRecorder::record_click() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kMouseAction] = to_underlying(MouseActionType::kSendClick);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kMouseActionCompute);
  finish();
}

void WebRecorder::record_mouse_over() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kMouseAction] = to_underlying(MouseActionType::kMouseOver);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kMouseActionCompute);
  finish();
}

void WebRecorder::record_start_mouse_hover() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kStartMouseHoverActionCompute);
  finish();
}

void WebRecorder::record_stop_mouse_hover() {
  check_busy();
  _web_worker->queue_build_compute_node(tc, ComponentDID::kStopMouseHoverActionCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Text Actions.
// -----------------------------------------------------------------

void WebRecorder::record_type_text(const QString& text) {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kText] = text;
  args[Message::kTextAction] = to_underlying(TextActionType::kSendText);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kTextActionCompute);
  finish();
}

void WebRecorder::record_type_enter() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kTextAction] = to_underlying(TextActionType::kSendEnter);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kTextActionCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Element Actions.
// -----------------------------------------------------------------

void WebRecorder::record_extract_text() {
  check_busy()
  _web_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kElementAction] = to_underlying(ElementActionType::kGetText);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute);
  finish();
}

void WebRecorder::record_select_from_dropdown(const QString& option_text) {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kElementAction] = to_underlying(ElementActionType::kSelectOption);
  args[Message::kOptionText] = option_text;
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute);
  finish();
}

void WebRecorder::record_scroll_down() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kElementAction] = to_underlying(ElementActionType::kScroll);
  args[Message::kDirection] = to_underlying(Direction::down);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute);
  finish();
}

void WebRecorder::record_scroll_up() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kElementAction] = to_underlying(ElementActionType::kScroll);
  args[Message::kDirection] = to_underlying(Direction::up);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute);
  finish();
}

void WebRecorder::record_scroll_right() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kElementAction] = to_underlying(ElementActionType::kScroll);
  args[Message::kDirection] = to_underlying(Direction::right);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute);
  finish();
}

void WebRecorder::record_scroll_left() {
  check_busy();
  _web_worker->queue_get_crosshair_info(tc);

  QVariantMap args;
  args[Message::kElementAction] = to_underlying(ElementActionType::kScroll);
  args[Message::kDirection] = to_underlying(Direction::left);
  _web_worker->queue_merge_chain_state(tc, args);
  _web_worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute);
  finish();
}

}
