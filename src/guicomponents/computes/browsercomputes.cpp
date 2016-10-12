#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>

#include <guicomponents/comms/webworker.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/computes/browsercomputes.h>

#include <functional>

namespace ngs {

//--------------------------------------------------------------------------------

BrowserCompute::BrowserCompute(Entity* entity, ComponentDID did)
    : Compute(entity, did),
      _web_worker(this),
      _task_scheduler(this) {
  get_dep_loader()->register_fixed_dep(_web_worker, Path({}));
  get_dep_loader()->register_fixed_dep(_task_scheduler, Path({}));
}

BrowserCompute::~BrowserCompute() {
}

void BrowserCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("in");
  create_output("out");
}

void BrowserCompute::dump_map(const QVariantMap& inputs) const {
  QVariantMap::const_iterator iter;
  for (iter = inputs.begin(); iter != inputs.end(); ++iter) {
    std::cerr << iter.key().toStdString() << " : " << iter.value().toString().toStdString() << "\n";
  }
}

void BrowserCompute::pre_update_state(TaskContext& tc) {
  internal();
  QVariantMap inputs = get_inputs();
  _web_worker->queue_merge_chain_state(tc, inputs);
  // Make sure nothing is loading right now.
  // Note in general a page may start loading content at random times.
  // For examples ads may rotate and flip content.
  _web_worker->queue_wait_until_loaded(tc);
}

void BrowserCompute::post_update_state(TaskContext& tc) {
  internal();
  std::function<void(const QVariantMap&)> on_get_outputs_bound = std::bind(&BrowserCompute::on_get_outputs,this,std::placeholders::_1);
  _web_worker->queue_get_outputs(tc, on_get_outputs_bound);
}

void BrowserCompute::on_get_outputs(const QVariantMap& outputs) {
  internal();
  set_outputs(outputs);
  clean_finalize();
}

//--------------------------------------------------------------------------------

bool OpenBrowserCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_open_browser(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

bool CloseBrowserCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_close_browser(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

bool IsBrowserOpenCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_is_browser_open(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

bool ResizeBrowserCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_resize_browser(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void NavigateToCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kURL, ParamType::kQString, false);
}

bool NavigateToCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_navigate_to(tc);
  _web_worker->queue_wait_until_loaded(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

bool NavigateRefreshCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_navigate_refresh(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void SwitchToIFrameCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kIFrame, ParamType::kQString, false);
}

bool SwitchToIFrameCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_switch_to_iframe(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void CreateSetFromValuesCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kWrapType, ParamType::kWrapType, false);
  create_input(Message::kTextValues, ParamType::kQStringList, false);
  create_input(Message::kImageValues, ParamType::kQStringList, false);
}

bool CreateSetFromValuesCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_create_set_by_matching_values(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void CreateSetFromTypeCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kWrapType, ParamType::kWrapType, false);
}

bool CreateSetFromTypeCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_create_set_by_matching_type(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void DeleteSetCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
}

bool DeleteSetCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_delete_set(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void ShiftSetCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kDirection, ParamType::kInt, false);
  create_input(Message::kWrapType, ParamType::kInt, false);
}

bool ShiftSetCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_shift_set(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void MouseActionCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kOverlayIndex, ParamType::kInt, false);
  create_input(Message::kOverlayRelClickPos, ParamType::kQVariantMap, false);
  create_input(Message::kMouseAction, ParamType::kInt, false);
}

bool MouseActionCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_perform_mouse_action(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void StartMouseHoverActionCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kOverlayIndex, ParamType::kInt, false);
  create_input(Message::kPosition, ParamType::kQVariantMap, false);
}

bool StartMouseHoverActionCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_start_mouse_hover(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

bool StopMouseHoverActionCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_stop_mouse_hover(tc);
  BrowserCompute::post_update_state(tc);
  return false;
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

bool TextActionCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_perform_text_action(tc);
  BrowserCompute::post_update_state(tc);
  return false;
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

bool ElementActionCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_perform_element_action(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void ExpandSetCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kDirection, ParamType::kInt, false); // Only used when the element action is set to scroll.
  create_input(Message::kMatchCriteria, ParamType::kQVariantMap, false);
}

bool ExpandSetCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_expand_set(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void MarkSetCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
}

bool MarkSetCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_mark_set(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void UnmarkSetCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
}

bool UnmarkSetCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_unmark_set(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

bool MergeSetsCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_merge_sets(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void ShrinkSetToSideCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kDirection, ParamType::kInt, false);
}

bool ShrinkSetToSideCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_shrink_set_to_side(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void ShrinkAgainstMarkedCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kDirections, ParamType::kIntList, false);
}

bool ShrinkAgainstMarkedCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_task_scheduler);
  BrowserCompute::pre_update_state(tc);
  _web_worker->queue_shrink_against_marked(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

}
