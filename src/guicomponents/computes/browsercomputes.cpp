#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>
#include <guicomponents/comms/appworker.h>
#include <guicomponents/computes/browsercomputes.h>

#include <functional>

namespace ngs {

//--------------------------------------------------------------------------------

BrowserCompute::BrowserCompute(Entity* entity, ComponentDID did)
    : Compute(entity, did),
      _app_worker(this) {
  get_dep_loader()->register_fixed_dep(_app_worker, Path({}));
}

BrowserCompute::~BrowserCompute() {
}

void BrowserCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("in");
  create_output("out");
}

void BrowserCompute::pre_update_state() {
  internal();
  QVariantMap inputs = get_inputs();
  _app_worker->queue_merge_chain_state(inputs);
}

void BrowserCompute::post_update_state() {
  internal();
  std::function<void(const QVariantMap&)> finalize_update = std::bind(&BrowserCompute::on_finalize_update,this,std::placeholders::_1);
  _app_worker->queue_finished(finalize_update);
}

void BrowserCompute::on_finalize_update(const QVariantMap& outputs) {
  set_outputs(outputs);
  clean_finalize();
}

//QVariantMap BrowserCompute::get_map(const std::string& input_name) const {
//  external();
//  return _inputs.at(input_name)->get_output("out").toMap();
//}
//
//QString BrowserCompute::get_string(const std::string& input_name) const {
//  external();
//  return _inputs.at(input_name)->get_output("out").toString();
//}
//
//QStringList BrowserCompute::get_string_list(const std::string& input_name) const {
//  external();
//  return _inputs.at(input_name)->get_output("out").toStringList();
//}
//
//int BrowserCompute::get_int(const std::string& input_name) const {
//  external();
//  return _inputs.at(input_name)->get_output("out").toInt();
//}
//
//float BrowserCompute::get_float(const std::string& input_name) const {
//  external();
//  return _inputs.at(input_name)->get_output("out").toFloat();
//}
//
//WrapType BrowserCompute::get_wrap_type(const std::string& input_name) const {
//  external();
//  return static_cast<WrapType>(_inputs.at(input_name)->get_output("out").toInt());
//}
//
//ActionType BrowserCompute::get_action_type(const std::string& input_name) const {
//  external();
//  return static_cast<ActionType>(_inputs.at(input_name)->get_output("out").toInt());
//}
//
//Direction BrowserCompute::get_direction(const std::string& input_name) const {
//  external();
//  return static_cast<Direction>(_inputs.at(input_name)->get_output("out").toInt());
//}

//--------------------------------------------------------------------------------

void OpenBrowserCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_open_browser();
  BrowserCompute::post_update_state();
}

void CloseBrowserCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_close_browser();
  BrowserCompute::post_update_state();
}

void CheckBrowserIsOpenCompute::update_state(){
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_check_browser_is_open();
  BrowserCompute::post_update_state();
}

void CheckBrowserSizeCompute::update_state(){
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_check_browser_size();
  BrowserCompute::post_update_state();
}

void NavigateToCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kURL, ParamType::kQString, false);
}

void NavigateToCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_navigate_to();
  BrowserCompute::post_update_state();
}

void NavigateRefreshCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_navigate_refresh();
  BrowserCompute::post_update_state();
}

void SwitchToIFrameCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kIFrame, ParamType::kQString, false);
}

void SwitchToIFrameCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_switch_to_iframe();
  BrowserCompute::post_update_state();
}

void CreateSetFromValuesCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kWrapType, ParamType::kWrapType, false);
  create_input(Message::kTextValues, ParamType::kQStringList, false);
  create_input(Message::kImageValues, ParamType::kQStringList, false);
}

void CreateSetFromValuesCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_create_set_by_matching_values();
  BrowserCompute::post_update_state();
}

void CreateSetFromTypeCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kWrapType, ParamType::kWrapType, false);
}

void CreateSetFromTypeCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_create_set_by_matching_type();
  BrowserCompute::post_update_state();
}

void DeleteSetCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
}

void DeleteSetCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_delete_set();
  BrowserCompute::post_update_state();
}

void ShiftSetCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kDirection, ParamType::kInt, false);
  create_input(Message::kWrapType, ParamType::kInt, false);
}

void ShiftSetCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_shift_set();
  BrowserCompute::post_update_state();
}

void MouseActionCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kOverlayIndex, ParamType::kInt, false);
  create_input(Message::kPosition, ParamType::kQVariantMap, false);
  create_input(Message::kMouseAction, ParamType::kInt, false);
}

void MouseActionCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_perform_mouse_action();
  BrowserCompute::post_update_state();
}

void StartMouseHoverActionCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kOverlayIndex, ParamType::kInt, false);
  create_input(Message::kPosition, ParamType::kQVariantMap, false);
}

void StartMouseHoverActionCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_start_mouse_hover();
  BrowserCompute::post_update_state();
}

void StopMouseHoverActionCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_stop_mouse_hover();
  BrowserCompute::post_update_state();
}

void TextActionCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kOverlayIndex, ParamType::kInt, false);
  create_input(Message::kPosition, ParamType::kQVariantMap, false);
  create_input(Message::kTextAction, ParamType::kInt, false);
  create_input(Message::kText, ParamType::kQString, false); // Only used when the text action is set to send text.
}

void TextActionCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_perform_text_action();
  BrowserCompute::post_update_state();
}

void ElementActionCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kOverlayIndex, ParamType::kInt, false);
  create_input(Message::kPosition, ParamType::kQVariantMap, false);
  create_input(Message::kElementAction, ParamType::kInt, false);
  create_input(Message::kOptionText, ParamType::kQString, false); // Only used when the element action is set to select option from dropdown.
  create_input(Message::kDirection, ParamType::kInt, false); // Only used when the element action is set to scroll.
}

void ElementActionCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_perform_element_action();
  BrowserCompute::post_update_state();
}

void ExpandSetCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kDirection, ParamType::kInt, false); // Only used when the element action is set to scroll.
  create_input(Message::kMatchCriteria, ParamType::kQVariantMap, false);
}

void ExpandSetCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_expand_set();
  BrowserCompute::post_update_state();
}

void MarkSetCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
}

void MarkSetCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_mark_set();
  BrowserCompute::post_update_state();
}

void UnmarkSetCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
}

void UnmarkSetCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_unmark_set();
  BrowserCompute::post_update_state();
}

void MergeSetsCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_merge_sets();
  BrowserCompute::post_update_state();
}

void ShrinkSetToSideCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kDirection, ParamType::kInt, false);
}

void ShrinkSetToSideCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_shrink_set_to_side();
  BrowserCompute::post_update_state();
}

void ShrinkAgainstMarkedCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kDirections, ParamType::kIntList, false);
}

void ShrinkAgainstMarkedCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_shrink_against_marked();
  BrowserCompute::post_update_state();
}

}
