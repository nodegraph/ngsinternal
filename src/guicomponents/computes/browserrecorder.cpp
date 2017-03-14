#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>

#include <components/computes/compute.h>
#include <guicomponents/computes/browserrecorder.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/filemodel.h>
#include <guicomponents/computes/taskqueuer.h>
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
      _queuer(this),
      _scheduler(this),
      _model(this) {
  get_dep_loader()->register_fixed_dep(_queuer, Path());
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

#define start() \
  /* Return if the scheduler is busy. */\
  if (_scheduler->is_busy()) {\
    emit web_action_ignored();\
    return;\
  }\
  /* Create our task context. */\
  TaskContext tc(_scheduler);\
  /* Clear the chain state so that any unset values
   * will take on default values of the node we're creating.*/\
  _queuer->queue_clear_chain_state(tc);


#define finish()

void BrowserRecorder::record_open_browser() {
  start();
  _queuer->queue_build_compute_node(tc, ComponentDID::kOpenBrowserCompute);
  finish();
}

void BrowserRecorder::record_close_browser() {
  start();
  _queuer->queue_build_compute_node(tc, ComponentDID::kCloseBrowserCompute);
  finish();
}

void BrowserRecorder::record_release_browser() {
  start();
  _queuer->queue_build_compute_node(tc, ComponentDID::kReleaseBrowserCompute);
  finish();
}

void BrowserRecorder::record_is_browser_open() {
  start();
  _queuer->queue_build_compute_node(tc, ComponentDID::kIsBrowserOpenCompute);
  finish();
}

void BrowserRecorder::record_resize_browser() {
  start();
  QJsonObject dims;
  dims.insert(Message::kWidth, 1024);
  dims.insert(Message::kHeight, 1150);

  QJsonObject args;
  args.insert(Message::kDimensions, dims);

  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kResizeBrowserCompute);
  finish();
}

void BrowserRecorder::record_get_browser_size() {
  start();
  _queuer->queue_build_compute_node(tc, ComponentDID::kGetBrowserSizeCompute);
  finish();
}

void BrowserRecorder::record_get_browser_title() {
  start();
  _queuer->queue_build_compute_node(tc, ComponentDID::kGetActiveTabTitleCompute);
  finish();
}

void BrowserRecorder::record_destroy_current_tab() {
  start();
  _queuer->queue_build_compute_node(tc, ComponentDID::kDestroyCurrentTabCompute);
  finish();
}

void BrowserRecorder::record_open_tab() {
  start();
  _queuer->queue_build_compute_node(tc, ComponentDID::kOpenTabCompute);
  finish();
}

void BrowserRecorder::record_accept_save_dialog() {
  start();
  _queuer->queue_build_compute_node(tc, ComponentDID::kAcceptSaveDialogCompute);
  finish();
}

void BrowserRecorder::record_download_video(bool use_current_element) {
  start();

  QJsonObject args;
  args.insert(Message::kUseCurrentElement, use_current_element);

  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kDownloadVideoCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Navigate Actions.
// -----------------------------------------------------------------

void BrowserRecorder::record_navigate_to(const QString& url) {
  start()
  QString decorated_url = get_proper_url(url).toString();

  QJsonObject args;
  args.insert(Message::kURL, decorated_url);

  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kNavigateToCompute);
  finish();
}

void BrowserRecorder::record_navigate_back() {
  start()
  _queuer->queue_build_compute_node(tc, ComponentDID::kNavigateBackCompute);
  finish();
}

void BrowserRecorder::record_navigate_forward() {
  start()
  _queuer->queue_build_compute_node(tc, ComponentDID::kNavigateForwardCompute);
  finish();
}

void BrowserRecorder::record_navigate_refresh() {
  start()
  _queuer->queue_build_compute_node(tc, ComponentDID::kNavigateRefreshCompute);
  finish();
}

void BrowserRecorder::record_get_current_url() {
  start()
  _queuer->queue_build_compute_node(tc, ComponentDID::kGetCurrentURLCompute);
  finish();
}

// -----------------------------------------------------------------
// Find element by position.
// -----------------------------------------------------------------

void BrowserRecorder::record_find_text_element_by_position() {
  start()
  _queuer->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kFindElementByPositionCompute);
  finish();
}

void BrowserRecorder::record_find_image_element_by_position() {
  start()
  _queuer->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kFindElementByPositionCompute);
  finish();
}

void BrowserRecorder::record_find_input_element_by_position() {
  start()
  _queuer->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::input));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kFindElementByPositionCompute);
  finish();
}

void BrowserRecorder::record_find_select_element_by_position() {
  start()
  _queuer->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::select));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kFindElementByPositionCompute);
  finish();
}

// -----------------------------------------------------------------
// Find the first element by values.
// -----------------------------------------------------------------

void BrowserRecorder::record_find_element_by_text() {
  start()
  _queuer->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_copy_chain_property(tc, Message::kTextValues, Message::kTargetValues);
  _queuer->queue_build_compute_node(tc, ComponentDID::kFindElementByValuesCompute);
  finish();
}

void BrowserRecorder::record_find_element_by_images() {
  start()
  _queuer->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_copy_chain_property(tc, Message::kImageValues, Message::kTargetValues);
  _queuer->queue_build_compute_node(tc, ComponentDID::kFindElementByValuesCompute);
  finish();
}

// -----------------------------------------------------------------
// Find the first element by type.
// -----------------------------------------------------------------

void BrowserRecorder::record_find_element_by_text_type() {
  start()
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kFindElementByTypeCompute);
  finish();
}

void BrowserRecorder::record_find_element_by_image_type() {
  start()
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kFindElementByTypeCompute);
  finish();
}

void BrowserRecorder::record_find_element_by_input_type() {
  start()
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::input));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kFindElementByTypeCompute);
  finish();
}

void BrowserRecorder::record_find_element_by_select_type() {
  start()
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::select));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kFindElementByTypeCompute);
  finish();
}

// -----------------------------------------------------------------
// Shift by type.
// -----------------------------------------------------------------

void BrowserRecorder::record_shift_to_text_type() {
  start()
  _queuer->queue_get_current_element(tc);
  _queuer->queue_get_crosshair_info(tc);
  _queuer->queue_determine_angle_in_degress(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}

void BrowserRecorder::record_shift_to_image_type() {
  start()
  _queuer->queue_get_current_element(tc);
  _queuer->queue_get_crosshair_info(tc);
  _queuer->queue_determine_angle_in_degress(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}

void BrowserRecorder::record_shift_to_input_type() {
  start()
  _queuer->queue_get_current_element(tc);
  _queuer->queue_get_crosshair_info(tc);
  _queuer->queue_determine_angle_in_degress(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::input));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}

void BrowserRecorder::record_shift_to_select_type() {
  start()
  _queuer->queue_get_current_element(tc);
  _queuer->queue_get_crosshair_info(tc);
  _queuer->queue_determine_angle_in_degress(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::select));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeCompute);
  finish();
}

// -----------------------------------------------------------------
// Shift by type along rows.
// -----------------------------------------------------------------

void BrowserRecorder::record_shift_to_text_type_along_rows() {
  start()
  _queuer->queue_get_current_element(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeAlongRowsCompute);
  finish();
}

void BrowserRecorder::record_shift_to_image_type_along_rows() {
  start()
  _queuer->queue_get_current_element(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeAlongRowsCompute);
  finish();
}

void BrowserRecorder::record_shift_to_input_type_along_rows() {
  start()
  _queuer->queue_get_current_element(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::input));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeAlongRowsCompute);
  finish();
}

void BrowserRecorder::record_shift_to_select_type_along_rows() {
  start()
  _queuer->queue_get_current_element(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::select));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeAlongRowsCompute);
  finish();
}

// -----------------------------------------------------------------
// Shift by type along columns.
// -----------------------------------------------------------------

void BrowserRecorder::record_shift_to_text_type_along_columns() {
  start()
  _queuer->queue_get_current_element(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeAlongColumnsCompute);
  finish();
}

void BrowserRecorder::record_shift_to_image_type_along_columns() {
  start()
  _queuer->queue_get_current_element(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeAlongColumnsCompute);
  finish();
}

void BrowserRecorder::record_shift_to_input_type_along_columns() {
  start()
  _queuer->queue_get_current_element(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::input));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeAlongColumnsCompute);
  finish();
}

void BrowserRecorder::record_shift_to_select_type_along_columns() {
  start()
  _queuer->queue_get_current_element(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::select));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByTypeAlongColumnsCompute);
  finish();
}

// -----------------------------------------------------------------
// Shift to element by values.
// -----------------------------------------------------------------

void BrowserRecorder::record_shift_to_text_values() {
  start()
  _queuer->queue_get_current_element(tc);
  _queuer->queue_get_crosshair_info(tc);
  _queuer->queue_determine_angle_in_degress(tc);
  _queuer->queue_get_element_values(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_copy_chain_property(tc, Message::kTextValues, Message::kTargetValues);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByValuesCompute);
  finish();
}

void BrowserRecorder::record_shift_to_image_values(){
  start()
  _queuer->queue_get_current_element(tc);
  _queuer->queue_get_crosshair_info(tc);
  _queuer->queue_determine_angle_in_degress(tc);
  _queuer->queue_get_element_values(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_copy_chain_property(tc, Message::kImageValues, Message::kTargetValues);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByValuesCompute);
  finish();
}

// -----------------------------------------------------------------
// Shift by values along rows.
// -----------------------------------------------------------------

void BrowserRecorder::record_shift_to_text_values_along_rows() {
  start()
  _queuer->queue_get_current_element(tc);
  _queuer->queue_get_element_values(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_copy_chain_property(tc, Message::kTextValues, Message::kTargetValues);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByValuesAlongRowsCompute);
  finish();
}

void BrowserRecorder::record_shift_to_image_values_along_rows(){
  start()
  _queuer->queue_get_current_element(tc);
  _queuer->queue_get_element_values(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_copy_chain_property(tc, Message::kImageValues, Message::kTargetValues);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByValuesAlongRowsCompute);
  finish();
}

// -----------------------------------------------------------------
// Shift by values along columns.
// -----------------------------------------------------------------

void BrowserRecorder::record_shift_to_text_values_along_columns() {
  start()
  _queuer->queue_get_current_element(tc);
  _queuer->queue_get_element_values(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::text));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_copy_chain_property(tc, Message::kTextValues, Message::kTargetValues);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByValuesAlongColumnsCompute);
  finish();
}

void BrowserRecorder::record_shift_to_image_values_along_columns(){
  start()
  _queuer->queue_get_current_element(tc);
  _queuer->queue_get_element_values(tc);
  QJsonObject args;
  args.insert(Message::kWrapType, to_underlying(WrapType::image));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_copy_chain_property(tc, Message::kImageValues, Message::kTargetValues);
  _queuer->queue_build_compute_node(tc, ComponentDID::kShiftElementByValuesAlongColumnsCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Mouse Actions.
// -----------------------------------------------------------------

void BrowserRecorder::record_click() {
  start();
  _queuer->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kMouseAction, to_underlying(MouseActionType::kSendClick));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kMouseActionCompute);
  finish();
}

void BrowserRecorder::record_ctrl_click() {
  start();
  _queuer->queue_get_crosshair_info(tc);
  QJsonObject args;
  args.insert(Message::kMouseAction, to_underlying(MouseActionType::kSendCtrlClick));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kMouseActionCompute);
  finish();
}

void BrowserRecorder::record_mouse_over() {
  start();
  _queuer->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kMouseAction, to_underlying(MouseActionType::kMouseOver));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kMouseActionCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Text Actions.
// -----------------------------------------------------------------

void BrowserRecorder::record_type_text(const QString& text) {
  start();
  _queuer->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kText, text);
  args.insert(Message::kTextAction, to_underlying(TextActionType::kSendText));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kTextActionCompute);
  finish();
}

void BrowserRecorder::record_type_enter() {
  start();
  _queuer->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kTextAction, to_underlying(TextActionType::kSendEnter));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kTextActionCompute);
  finish();
}

void BrowserRecorder::record_type_password(const QString& text) {
  start();
  _queuer->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kText, text);
  args.insert(Message::kTextAction, to_underlying(TextActionType::kSendText));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kPasswordActionCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Element Actions.
// -----------------------------------------------------------------

void BrowserRecorder::record_extract_text() {
  start()
  _queuer->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kElementAction, to_underlying(ElementActionType::kGetText));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kElementActionCompute);
  finish();
}

void BrowserRecorder::record_select_from_dropdown(const QString& option_text) {
  start();
  _queuer->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kElementAction, to_underlying(ElementActionType::kChooseOption));
  args.insert(Message::kOptionText, option_text);
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kElementActionCompute);
  finish();
}

// -----------------------------------------------------------------
// Record Element Scroll Actions.
// -----------------------------------------------------------------

void BrowserRecorder::record_scroll_down() {
  start();
  _queuer->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kScrollDirection, to_underlying(DirectionType::down));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kElementScrollCompute);
  finish();
}

void BrowserRecorder::record_scroll_up() {
  start();
  _queuer->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kScrollDirection, to_underlying(DirectionType::up));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kElementScrollCompute);
  finish();
}

void BrowserRecorder::record_scroll_right() {
  start();
  _queuer->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kScrollDirection, to_underlying(DirectionType::right));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kElementScrollCompute);
  finish();
}

void BrowserRecorder::record_scroll_left() {
  start();
  _queuer->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kScrollDirection, to_underlying(DirectionType::left));
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kElementScrollCompute);
  finish();
}

// -----------------------------------------------------------------
// Set Element.
// -----------------------------------------------------------------
void BrowserRecorder::record_set_element() {
  start();
  _queuer->queue_get_crosshair_info(tc);

  QJsonObject args;
  args.insert(Message::kElementInfo, "");
  _queuer->queue_merge_chain_state(tc, args);
  _queuer->queue_build_compute_node(tc, ComponentDID::kSetElementCompute);
  finish();
}

}
