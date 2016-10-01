#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>
#include <guicomponents/comms/appworker.h>
#include <guicomponents/computes/browsercomputes.h>

#include <functional>

namespace ngs {

//--------------------------------------------------------------------------------

BrowserCompute::BrowserCompute(Entity* entity, size_t did)
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
  _app_worker->queue_chain_state_merge(inputs);
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

void CreateSetFromValuesCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kWrapType, ParamType::kWrapType, false);
  create_input(Message::kMatchValues, ParamType::kQStringList, false);
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

void ClickActionCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kOverlayIndex, ParamType::kInt, false);
  create_input(Message::kPosition, ParamType::kQVariantMap, false);
}

void ClickActionCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_get_xpath();
  _app_worker->queue_click();
  BrowserCompute::post_update_state();
}

void MouseOverActionCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kOverlayIndex, ParamType::kInt, false);
  create_input(Message::kPosition, ParamType::kQVariantMap, false);
}

void MouseOverActionCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_get_xpath();
  _app_worker->queue_mouse_over();
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
  _app_worker->queue_get_xpath();
  _app_worker->queue_start_mouse_hover();
  BrowserCompute::post_update_state();
}

void StopMouseHoverActionCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kOverlayIndex, ParamType::kInt, false);
  create_input(Message::kPosition, ParamType::kQVariantMap, false);
}

void StopMouseHoverActionCompute::update_state() {
  internal();
  BrowserCompute::pre_update_state();
  _app_worker->queue_get_xpath();
  _app_worker->queue_stop_mouse_hover();
  BrowserCompute::post_update_state();
}

}
