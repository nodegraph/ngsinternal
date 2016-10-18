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

  create_input(Message::kURL, ParamType::kString, false);
  add_hint(Message::kURL, HintType::kDescription, "The url the browser should to navigate to.");
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

  create_input(Message::kIFrame, ParamType::kString, false);
  add_hint(Message::kIFrame, HintType::kDescription, "The path to the iframe that subsequent actions shall act on.");
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

  create_input(Message::kWrapType, ParamType::kInt, false);
  add_hint(Message::kWrapType, HintType::kEnum, to_underlying(EnumHint::kWrapType));
  add_hint(Message::kWrapType, HintType::kDescription, "The element type to use when creating the set.");

  create_input(Message::kTextValues, ParamType::kStringList, false);
  add_hint(Message::kTextValues, HintType::kDescription, "The set of text values used to find text elements.");

  create_input(Message::kImageValues, ParamType::kStringList, false);
  add_hint(Message::kImageValues, HintType::kDescription, "The set of overlapping images used to find image elements.");
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

  create_input(Message::kWrapType, ParamType::kInt, false);
  add_hint(Message::kWrapType, HintType::kEnum, to_underlying(EnumHint::kWrapType));
  add_hint(Message::kWrapType, HintType::kDescription, "The element type to use when creating the set.");
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
  add_hint(Message::kSetIndex, HintType::kDescription, "The 0 based index that identifies the element set to delete.");
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
  add_hint(Message::kSetIndex, HintType::kDescription, "The 0 based index that identifies the element set to shift.");

  create_input(Message::kDirection, ParamType::kInt, false);
  add_hint(Message::kDirection, HintType::kEnum, to_underlying(EnumHint::kDirectionType));
  add_hint(Message::kSetIndex, HintType::kDescription, "The direction in which to shift the set elements to.");

  create_input(Message::kWrapType, ParamType::kInt, false);
  add_hint(Message::kWrapType, HintType::kEnum, to_underlying(EnumHint::kWrapType));
  add_hint(Message::kWrapType, HintType::kDescription, "The type of elements to shift to.");
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
  add_hint(Message::kSetIndex, HintType::kDescription, "The 0 based index that identifies the element set which contains the element to act on.");

  create_input(Message::kOverlayIndex, ParamType::kInt, false);
  add_hint(Message::kOverlayIndex, HintType::kDescription, "The 0 based index that identifies the element to act on inside the set.");

  create_input(Message::kOverlayRelClickPos, ParamType::kMap, false);
  add_hint(Message::kOverlayRelClickPos, HintType::kDescription, "The position to perform our action at, relative to the element itself.");

  create_input(Message::kMouseAction, ParamType::kInt, false);
  add_hint(Message::kMouseAction, HintType::kEnum, to_underlying(EnumHint::kMouseActionType));
  add_hint(Message::kMouseAction, HintType::kDescription, "The type of mouse mouse action to perform.");
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
  add_hint(Message::kSetIndex, HintType::kDescription, "The 0 based index that identifies the element set which contains the element to act on.");

  create_input(Message::kOverlayIndex, ParamType::kInt, false);
  add_hint(Message::kOverlayIndex, HintType::kDescription, "The 0 based index that identifies the element to hover on inside the set.");

  create_input(Message::kOverlayRelClickPos, ParamType::kMap, false);
  add_hint(Message::kOverlayRelClickPos, HintType::kDescription, "The position to perform our hover at, relative to the element itself.");
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
  add_hint(Message::kSetIndex, HintType::kDescription, "The 0 based index that identifies the element set which contains the element to type on.");

  create_input(Message::kOverlayIndex, ParamType::kInt, false);
  add_hint(Message::kOverlayIndex, HintType::kDescription, "The 0 based index that identifies the element to type on inside the set.");

  create_input(Message::kTextAction, ParamType::kInt, false);
  add_hint(Message::kTextAction, HintType::kEnum, to_underlying(EnumHint::kTextActionType));
  add_hint(Message::kTextAction, HintType::kDescription, "The type of text action to perform.");

  create_input(Message::kText, ParamType::kString, false); // Only used when the text action is set to send text.
  add_hint(Message::kText, HintType::kDescription, "The text to type.");
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
  add_hint(Message::kSetIndex, HintType::kDescription, "The 0 based index that identifies the element set which contains the element to act on.");

  create_input(Message::kOverlayIndex, ParamType::kInt, false);
  add_hint(Message::kOverlayIndex, HintType::kDescription, "The 0 based index that identifies the element to act on inside the set.");

  create_input(Message::kElementAction, ParamType::kInt, false);
  add_hint(Message::kElementAction, HintType::kEnum, to_underlying(EnumHint::kElementActionType));
  add_hint(Message::kElementAction, HintType::kDescription, "The type of element action to perform.");

  create_input(Message::kOptionText, ParamType::kString, false); // Only used when the element action is set to select option from dropdown.
  add_hint(Message::kOptionText, HintType::kDescription, "The text to select from dropdowns.");

  create_input(Message::kDirection, ParamType::kInt, false); // Only used when the element action is set to scroll.
  add_hint(Message::kDirection, HintType::kEnum, to_underlying(EnumHint::kDirectionType));
  add_hint(Message::kDirection, HintType::kDescription, "The direction to scroll.");
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
  add_hint(Message::kSetIndex, HintType::kDescription, "The 0 based index that identifies the element set to expand.");

  create_input(Message::kDirection, ParamType::kInt, false); // Only used when the element action is set to scroll.
  add_hint(Message::kDirection, HintType::kEnum, to_underlying(EnumHint::kDirectionType));
  add_hint(Message::kDirection, HintType::kDescription, "The direction in which to expand the set.");

  create_input(Message::kMatchCriteria, ParamType::kMap, false);
  add_hint(Message::kMatchCriteria, HintType::kDescription, "The match criteria used when expanding the set.");
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
  add_hint(Message::kSetIndex, HintType::kDescription, "The 0 based index that identifies the element set to mark.");
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
  add_hint(Message::kSetIndex, HintType::kDescription, "The 0 based index that identifies the element set to unmark.");
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
  add_hint(Message::kSetIndex, HintType::kDescription, "The 0 based index that identifies the element set to shrink on one side.");

  create_input(Message::kDirection, ParamType::kInt, false);
  add_hint(Message::kDirection, HintType::kEnum, to_underlying(EnumHint::kDirectionType));
  add_hint(Message::kDirection, HintType::kDescription, "The side of the set from which to perform the shrink.");
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
  add_hint(Message::kSetIndex, HintType::kDescription, "The 0 based index that identifies the element set to shrink against.");

  create_input(Message::kDirections, ParamType::kIntList, false);
  add_hint(Message::kDirections, HintType::kEnum, to_underlying(EnumHint::kDirectionType));
  add_hint(Message::kDirections, HintType::kDescription, "The sides of the marked sets used to intersect the set index, identified by set_index.");
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
