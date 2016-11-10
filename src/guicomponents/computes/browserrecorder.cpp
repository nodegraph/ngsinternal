#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>

#include <components/computes/compute.h>
#include <guicomponents/computes/browserrecorder.h>
#include <guicomponents/comms/browserworker.h>

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

BrowserRecorder::BrowserRecorder(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _worker(this),
      _scheduler(this),
      _model(this) {
  get_dep_loader()->register_fixed_dep(_worker, Path({}));
  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
  get_dep_loader()->register_fixed_dep(_model, Path({}));
}

BrowserRecorder::~BrowserRecorder() {
}

void BrowserRecorder::initialize_wires() {
  Component::initialize_wires();
}

// -----------------------------------------------------------------
// // Record Browser Actions.
// -----------------------------------------------------------------

#define check_busy()   if (_scheduler->is_busy()) {emit web_action_ignored(); return;} TaskContext tc(_scheduler);
#define finish()

void BrowserRecorder::record_open_browser() {
  check_busy();
  _worker->queue_build_compute_node(tc, ComponentDID::kOpenBrowserCompute);
  finish();
}

void BrowserRecorder::record_close_browser() {
  check_busy();
  _worker->queue_build_compute_node(tc, ComponentDID::kCloseBrowserCompute);
  finish();
}

void BrowserRecorder::record_is_browser_open() {
  check_busy();
  _worker->queue_build_compute_node(tc, ComponentDID::kIsBrowserOpenCompute);
  finish();
}

void BrowserRecorder::record_check_browser_size() {
  check_busy();
  int width = _model->get_work_setting(FileModel::kBrowserWidthRole).toInt();
  int height = _model->get_work_setting(FileModel::kBrowserHeightRole).toInt();

  QJsonObject dims;
  dims.insert(Message::kWidth, width);
  dims.insert(Message::kHeight, height);

  QJsonObject args;
  args.insert(Message::kDimensions, dims);

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kResizeBrowserCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Navigate Actions.
// -----------------------------------------------------------------

void BrowserRecorder::record_navigate_to(const QString& url) {
  check_busy()
  QString decorated_url = get_proper_url(url).toString();

  QJsonObject args;
  args.insert(Message::kURL, decorated_url);

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kNavigateToCompute);
  finish();
}

void BrowserRecorder::record_switch_to_iframe() {
  check_busy()
  QJsonObject args;
  args.insert(Message::kIFrame, _worker->get_iframe_to_switch_to());
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kSwitchToIFrameCompute);
  finish();
}

void BrowserRecorder::record_navigate_refresh() {
  check_busy()
  _worker->queue_build_compute_node(tc, ComponentDID::kNavigateRefreshCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Create Set By Matching Values..
// -----------------------------------------------------------------

void BrowserRecorder::record_create_set_by_matching_text_values() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromValuesCompute);
  finish();
}

void BrowserRecorder::record_create_set_by_matching_image_values() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromValuesCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Create Set By Type.
// -----------------------------------------------------------------

void BrowserRecorder::record_create_set_of_inputs() {
  check_busy()
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::input));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromTypeCompute);
  finish();
}

void BrowserRecorder::record_create_set_of_selects() {
  check_busy()
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::select));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromTypeCompute);
  finish();
}

void BrowserRecorder::record_create_set_of_images() {
  check_busy()
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromTypeCompute);
  finish();
}

void BrowserRecorder::record_create_set_of_text() {
  check_busy()
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kCreateSetFromTypeCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Delete Set.
// -----------------------------------------------------------------

void BrowserRecorder::record_delete_set() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  _worker->queue_build_compute_node(tc, ComponentDID::kDeleteSetCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Shift Sets.
// -----------------------------------------------------------------

void BrowserRecorder::record_shift_to_text_above() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  args.insert(Message::kDirection, to_underlying(DirectionType::up));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void BrowserRecorder::record_shift_to_text_below() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  args.insert(Message::kDirection, to_underlying(DirectionType::down));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void BrowserRecorder::record_shift_to_text_on_left() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  args.insert(Message::kDirection, to_underlying(DirectionType::left));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void BrowserRecorder::record_shift_to_text_on_right() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  args.insert(Message::kDirection, to_underlying(DirectionType::right));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}

void BrowserRecorder::record_shift_to_images_above() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  args.insert(Message::kDirection, to_underlying(DirectionType::up));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void BrowserRecorder::record_shift_to_images_below() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  args.insert(Message::kDirection, to_underlying(DirectionType::down));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void BrowserRecorder::record_shift_to_images_on_left() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  args.insert(Message::kDirection, to_underlying(DirectionType::left));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void BrowserRecorder::record_shift_to_images_on_right() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  args.insert(Message::kDirection, to_underlying(DirectionType::right));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}

void BrowserRecorder::record_shift_to_inputs_above() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::input));
  args.insert(Message::kDirection, to_underlying(DirectionType::up));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void BrowserRecorder::record_shift_to_inputs_below() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::input));
  args.insert(Message::kDirection, to_underlying(DirectionType::down));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void BrowserRecorder::record_shift_to_inputs_on_left() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::input));
  args.insert(Message::kDirection, to_underlying(DirectionType::left));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void BrowserRecorder::record_shift_to_inputs_on_right() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::input));
  args.insert(Message::kDirection, to_underlying(DirectionType::right));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}

void BrowserRecorder::record_shift_to_selects_above() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::select));
  args.insert(Message::kDirection, to_underlying(DirectionType::up));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void BrowserRecorder::record_shift_to_selects_below() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::select));
  args.insert(Message::kDirection, to_underlying(DirectionType::down));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void BrowserRecorder::record_shift_to_selects_on_left() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::select));
  args.insert(Message::kDirection, to_underlying(DirectionType::left));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void BrowserRecorder::record_shift_to_selects_on_right() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::select));
  args.insert(Message::kDirection, to_underlying(DirectionType::right));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}

void BrowserRecorder::record_shift_to_iframes_above() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::iframe));
  args.insert(Message::kDirection, to_underlying(DirectionType::up));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void BrowserRecorder::record_shift_to_iframes_below() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::iframe));
  args.insert(Message::kDirection, to_underlying(DirectionType::down));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void BrowserRecorder::record_shift_to_iframes_on_left() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::iframe));
  args.insert(Message::kDirection, to_underlying(DirectionType::left));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}
void BrowserRecorder::record_shift_to_iframes_on_right() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::iframe));
  args.insert(Message::kDirection, to_underlying(DirectionType::right));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftSetCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Expand Sets.
// -----------------------------------------------------------------

void BrowserRecorder::record_expand_above() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonObject match_criteria;
  match_criteria.insert(Message::kMatchLeft, true);
  match_criteria.insert(Message::kMatchRight, false);
  match_criteria.insert(Message::kMatchTop, false);
  match_criteria.insert(Message::kMatchBottom, false);
  match_criteria.insert(Message::kMatchFont, true);
  match_criteria.insert(Message::kMatchFontSize, true);

  QJsonObject args;
  args.insert(Message::kMatchCriteria, match_criteria);
  args.insert(Message::kDirection, to_underlying(DirectionType::up));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kExpandSetCompute);
  finish();
}

void BrowserRecorder::record_expand_below() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonObject match_criteria;
  match_criteria.insert(Message::kMatchLeft, true);
  match_criteria.insert(Message::kMatchRight, false);
  match_criteria.insert(Message::kMatchTop, false);
  match_criteria.insert(Message::kMatchBottom, false);
  match_criteria.insert(Message::kMatchFont, true);
  match_criteria.insert(Message::kMatchFontSize, true);

  QJsonObject args;
  args.insert(Message::kMatchCriteria, match_criteria);
  args.insert(Message::kDirection, to_underlying(DirectionType::down));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kExpandSetCompute);
  finish();
}

void BrowserRecorder::record_expand_left() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonObject match_criteria;
  match_criteria.insert(Message::kMatchLeft, false);
  match_criteria.insert(Message::kMatchRight, false);
  match_criteria.insert(Message::kMatchTop, true);
  match_criteria.insert(Message::kMatchBottom, false);
  match_criteria.insert(Message::kMatchFont, true);
  match_criteria.insert(Message::kMatchFontSize, true);

  QJsonObject args;
  args.insert(Message::kMatchCriteria, match_criteria);
  args.insert(Message::kDirection, to_underlying(DirectionType::left));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kExpandSetCompute);
  finish();
}

void BrowserRecorder::record_expand_right() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonObject match_criteria;
  match_criteria.insert(Message::kMatchLeft, false);
  match_criteria.insert(Message::kMatchRight, false);
  match_criteria.insert(Message::kMatchTop, true);
  match_criteria.insert(Message::kMatchBottom, false);
  match_criteria.insert(Message::kMatchFont, true);
  match_criteria.insert(Message::kMatchFontSize, true);

  QJsonObject args;
  args.insert(Message::kMatchCriteria, match_criteria);
  args.insert(Message::kDirection, to_underlying(DirectionType::right));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kExpandSetCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Mark Sets.
// -----------------------------------------------------------------

void BrowserRecorder::record_mark_set() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);
  _worker->queue_build_compute_node(tc, ComponentDID::kMarkSetCompute);
  finish();
}

void BrowserRecorder::record_unmark_set() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);
  _worker->queue_build_compute_node(tc, ComponentDID::kUnmarkSetCompute);
  finish();
}

void BrowserRecorder::record_merge_sets() {
  check_busy();
  _worker->queue_build_compute_node(tc, ComponentDID::kMergeSetsCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Shrink To One Side.
// -----------------------------------------------------------------

void BrowserRecorder::record_shrink_set_to_topmost() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kDirection, to_underlying(DirectionType::up));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShrinkSetToSideCompute);
  finish();
}

void BrowserRecorder::record_shrink_set_to_bottommost() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kDirection, to_underlying(DirectionType::down));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShrinkSetToSideCompute);
  finish();
}

void BrowserRecorder::record_shrink_set_to_leftmost() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kDirection, to_underlying(DirectionType::left));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShrinkSetToSideCompute);
  finish();
}

void BrowserRecorder::record_shrink_set_to_rightmost() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kDirection, to_underlying(DirectionType::right));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShrinkSetToSideCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Shrink Against Marked Sets.
// -----------------------------------------------------------------

void BrowserRecorder::record_shrink_above_of_marked() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonArray dirs;
  dirs.push_back(to_underlying(DirectionType::up));
  QJsonObject args;
  args.insert(Message::kDirections, dirs);
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute);
  finish();
}

void BrowserRecorder::record_shrink_below_of_marked() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonArray dirs;
  dirs.push_back(to_underlying(DirectionType::down));
  QJsonObject args;
  args.insert(Message::kDirections, dirs);
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute);
  finish();
}

void BrowserRecorder::record_shrink_above_and_below_of_marked() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonArray dirs;
  dirs.push_back(to_underlying(DirectionType::up));
  dirs.push_back(to_underlying(DirectionType::down));
  QJsonObject args;
  args.insert(Message::kDirections, dirs);
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute);
  finish();
}

void BrowserRecorder::record_shrink_left_of_marked() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonArray dirs;
  dirs.push_back(to_underlying(DirectionType::left));
  QJsonObject args;
  args.insert(Message::kDirections, dirs);
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute);
  finish();
}

void BrowserRecorder::record_shrink_right_of_marked() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonArray dirs;
  dirs.push_back(to_underlying(DirectionType::right));
  QJsonObject args;
  args.insert(Message::kDirections, dirs);
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute);
  finish();
}

void BrowserRecorder::record_shrink_left_and_right_of_marked() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonArray dirs;
  dirs.push_back(to_underlying(DirectionType::left));
  dirs.push_back(to_underlying(DirectionType::right));
  QJsonObject args;
  args.insert(Message::kDirections, dirs);
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShrinkAgainstMarkedCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Mouse Actions.
// -----------------------------------------------------------------

void BrowserRecorder::record_click() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kMouseAction, to_underlying(MouseActionType::kSendClick));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kMouseActionCompute);
  finish();
}

void BrowserRecorder::record_mouse_over() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kMouseAction, to_underlying(MouseActionType::kMouseOver));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kMouseActionCompute);
  finish();
}

void BrowserRecorder::record_start_mouse_hover() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);
  _worker->queue_build_compute_node(tc, ComponentDID::kStartMouseHoverActionCompute);
  finish();
}

void BrowserRecorder::record_stop_mouse_hover() {
  check_busy();
  _worker->queue_build_compute_node(tc, ComponentDID::kStopMouseHoverActionCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Text Actions.
// -----------------------------------------------------------------

void BrowserRecorder::record_type_text(const QString& text) {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kText, text);
  args.insert(Message::kTextAction, to_underlying(TextActionType::kSendText));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kTextActionCompute);
  finish();
}

void BrowserRecorder::record_type_enter() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kTextAction, to_underlying(TextActionType::kSendEnter));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kTextActionCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Element Actions.
// -----------------------------------------------------------------

void BrowserRecorder::record_extract_text() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kElementAction, to_underlying(ElementActionType::kGetText));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute);
  finish();
}

void BrowserRecorder::record_select_from_dropdown(const QString& option_text) {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kElementAction, to_underlying(ElementActionType::kSelectOption));
  args.insert(Message::kOptionText, option_text);
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute);
  finish();
}

void BrowserRecorder::record_scroll_down() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kElementAction, to_underlying(ElementActionType::kScroll));
  args.insert(Message::kDirection, to_underlying(DirectionType::down));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute);
  finish();
}

void BrowserRecorder::record_scroll_up() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kElementAction, to_underlying(ElementActionType::kScroll));
  args.insert(Message::kDirection, to_underlying(DirectionType::up));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute);
  finish();
}

void BrowserRecorder::record_scroll_right() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kElementAction, to_underlying(ElementActionType::kScroll));
  args.insert(Message::kDirection, to_underlying(DirectionType::right));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute);
  finish();
}

void BrowserRecorder::record_scroll_left() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kElementAction, to_underlying(ElementActionType::kScroll));
  args.insert(Message::kDirection, to_underlying(DirectionType::left));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kElementActionCompute);
  finish();
}

}
