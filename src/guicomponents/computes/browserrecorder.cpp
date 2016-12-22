#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>

#include <components/computes/compute.h>
#include <guicomponents/computes/browserrecorder.h>
#include <guicomponents/computes/nodejsworker.h>

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
  get_dep_loader()->register_fixed_dep(_worker, Path());
  get_dep_loader()->register_fixed_dep(_scheduler, Path());
  get_dep_loader()->register_fixed_dep(_model, Path());
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

void BrowserRecorder::record_resize_browser() {
  check_busy();
  //int width = _model->get_work_setting(FileModel::kBrowserWidthRole).toInt();
  //int height = _model->get_work_setting(FileModel::kBrowserHeightRole).toInt();

  QJsonObject dims;
  dims.insert(Message::kWidth, 1024);
  dims.insert(Message::kHeight, 1150);

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

void BrowserRecorder::record_navigate_back() {
  check_busy()
  _worker->queue_build_compute_node(tc, ComponentDID::kNavigateBackCompute);
  finish();
}

void BrowserRecorder::record_navigate_forward() {
  check_busy()
  _worker->queue_build_compute_node(tc, ComponentDID::kNavigateForwardCompute);
  finish();
}

void BrowserRecorder::record_navigate_refresh() {
  check_busy()
  _worker->queue_build_compute_node(tc, ComponentDID::kNavigateRefreshCompute);
  finish();
}

// -----------------------------------------------------------------
// Find element by position.
// -----------------------------------------------------------------

void BrowserRecorder::record_find_text_element_by_position() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kFindElementByPositionCompute);
  finish();
}

void BrowserRecorder::record_find_image_element_by_position() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kFindElementByPositionCompute);
  finish();
}

void BrowserRecorder::record_find_input_element_by_position() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::input));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kFindElementByPositionCompute);
  finish();
}

void BrowserRecorder::record_find_select_element_by_position() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::select));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kFindElementByPositionCompute);
  finish();
}

// -----------------------------------------------------------------
// Find the first element by values.
// -----------------------------------------------------------------

void BrowserRecorder::record_find_element_by_text() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_copy_chain_property(tc, Message::kTextValues, Message::kTargetValues);
  _worker->queue_build_compute_node(tc, ComponentDID::kFindElementByValuesCompute);
  finish();
}

void BrowserRecorder::record_find_element_by_images() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_copy_chain_property(tc, Message::kImageValues, Message::kTargetValues);
  _worker->queue_build_compute_node(tc, ComponentDID::kFindElementByValuesCompute);
  finish();
}

// -----------------------------------------------------------------
// Find the first element by type.
// -----------------------------------------------------------------

void BrowserRecorder::record_find_element_by_text_type() {
  check_busy()
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kFindElementByTypeCompute);
  finish();
}

void BrowserRecorder::record_find_element_by_image_type() {
  check_busy()
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kFindElementByTypeCompute);
  finish();
}

void BrowserRecorder::record_find_element_by_input_type() {
  check_busy()
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::input));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kFindElementByTypeCompute);
  finish();
}

void BrowserRecorder::record_find_element_by_select_type() {
  check_busy()
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::select));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kFindElementByTypeCompute);
  finish();
}

// -----------------------------------------------------------------
// Shift to element by type.
// -----------------------------------------------------------------

void BrowserRecorder::record_shift_to_text_type_above() {
  check_busy()
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  args.insert(Message::kDirection, to_underlying(DirectionType::up));

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}
void BrowserRecorder::record_shift_to_text_type_below() {
  check_busy()
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  args.insert(Message::kDirection, to_underlying(DirectionType::down));

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}
void BrowserRecorder::record_shift_to_text_type_on_left() {
  check_busy()
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  args.insert(Message::kDirection, to_underlying(DirectionType::left));

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}
void BrowserRecorder::record_shift_to_text_type_on_right() {
  check_busy()
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  args.insert(Message::kDirection, to_underlying(DirectionType::right));

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}

void BrowserRecorder::record_shift_to_image_type_above() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  args.insert(Message::kDirection, to_underlying(DirectionType::up));

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}
void BrowserRecorder::record_shift_to_image_type_below() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  args.insert(Message::kDirection, to_underlying(DirectionType::down));

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}
void BrowserRecorder::record_shift_to_image_type_on_left() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  args.insert(Message::kDirection, to_underlying(DirectionType::left));

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}
void BrowserRecorder::record_shift_to_image_type_on_right() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  args.insert(Message::kDirection, to_underlying(DirectionType::right));

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}

void BrowserRecorder::record_shift_to_input_type_above() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::input));
  args.insert(Message::kDirection, to_underlying(DirectionType::up));

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}
void BrowserRecorder::record_shift_to_input_type_below() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::input));
  args.insert(Message::kDirection, to_underlying(DirectionType::down));

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}
void BrowserRecorder::record_shift_to_input_type_on_left() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::input));
  args.insert(Message::kDirection, to_underlying(DirectionType::left));

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}
void BrowserRecorder::record_shift_to_input_type_on_right() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::input));
  args.insert(Message::kDirection, to_underlying(DirectionType::right));

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}

void BrowserRecorder::record_shift_to_select_type_above() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::select));
  args.insert(Message::kDirection, to_underlying(DirectionType::up));

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}
void BrowserRecorder::record_shift_to_select_type_below() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::select));
  args.insert(Message::kDirection, to_underlying(DirectionType::down));

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}
void BrowserRecorder::record_shift_to_select_type_on_left() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::select));
  args.insert(Message::kDirection, to_underlying(DirectionType::left));

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}
void BrowserRecorder::record_shift_to_select_type_on_right() {
  check_busy()
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::select));
  args.insert(Message::kDirection, to_underlying(DirectionType::right));

  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}

// -----------------------------------------------------------------
// Shift to element by values.
// -----------------------------------------------------------------

void BrowserRecorder::record_shift_to_text_values_above() {
  check_busy()
  _worker->queue_get_element_values(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  args.insert(Message::kDirection, to_underlying(DirectionType::up));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_copy_chain_property(tc, Message::kTextValues, Message::kTargetValues);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByValuesCompute);
  finish();
}
void BrowserRecorder::record_shift_to_text_values_below(){
  check_busy()
  _worker->queue_get_element_values(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  args.insert(Message::kDirection, to_underlying(DirectionType::down));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_copy_chain_property(tc, Message::kTextValues, Message::kTargetValues);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByValuesCompute);
  finish();
}
void BrowserRecorder::record_shift_to_text_values_on_left(){
  check_busy()
  _worker->queue_get_element_values(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  args.insert(Message::kDirection, to_underlying(DirectionType::left));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_copy_chain_property(tc, Message::kTextValues, Message::kTargetValues);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByValuesCompute);
  finish();
}
void BrowserRecorder::record_shift_to_text_values_on_right(){
  check_busy()
  _worker->queue_get_element_values(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  args.insert(Message::kDirection, to_underlying(DirectionType::right));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_copy_chain_property(tc, Message::kTextValues, Message::kTargetValues);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByValuesCompute);
  finish();
}

void BrowserRecorder::record_shift_to_image_values_above(){
  check_busy()
  _worker->queue_get_element_values(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  args.insert(Message::kDirection, to_underlying(DirectionType::up));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_copy_chain_property(tc, Message::kImageValues, Message::kTargetValues);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByValuesCompute);
  finish();
}
void BrowserRecorder::record_shift_to_image_values_below(){
  check_busy()
  _worker->queue_get_element_values(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  args.insert(Message::kDirection, to_underlying(DirectionType::down));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_copy_chain_property(tc, Message::kImageValues, Message::kTargetValues);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByValuesCompute);
  finish();
}
void BrowserRecorder::record_shift_to_image_values_on_left(){
  check_busy()
  _worker->queue_get_element_values(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  args.insert(Message::kDirection, to_underlying(DirectionType::left));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_copy_chain_property(tc, Message::kImageValues, Message::kTargetValues);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByValuesCompute);
  finish();
}
void BrowserRecorder::record_shift_to_image_values_on_right(){
  check_busy()
  _worker->queue_get_element_values(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  args.insert(Message::kDirection, to_underlying(DirectionType::right));
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_copy_chain_property(tc, Message::kImageValues, Message::kTargetValues);
  _worker->queue_build_compute_node(tc, ComponentDID::kShiftElementByValuesCompute);
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

void BrowserRecorder::record_ctrl_click() {
  check_busy();
  _worker->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kMouseAction, to_underlying(MouseActionType::kSendCtrlClick));
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
  args.insert(Message::kElementAction, to_underlying(ElementActionType::kChooseOption));
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
