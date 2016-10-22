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
  create_input("in", QVariantMap());
  create_output("out");
  create_input("use_script", false, JSType::kBoolean, false);
  create_input("script", "", JSType::kString, false);
}

void BrowserCompute::init_hints(QVariantMap& m) {
  add_hint(m, "use_script", HintType::kJSType, to_underlying(JSType::kBoolean));
  add_hint(m, "use_script", HintType::kDescription, "Whether to use a script to modify the input parameters on this node.");
  add_hint(m, "script", HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, "script", HintType::kDescription, "The script to modify the input parameters on this node.");
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
  create_input(Message::kURL, "", JSType::kString, false);
}

const QVariantMap NavigateToCompute::_hints = NavigateToCompute::init_hints();
QVariantMap NavigateToCompute::init_hints() {
  QVariantMap m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kURL, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kURL, HintType::kDescription, "The url the browser should to navigate to.");
  return m;
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
  create_input(Message::kIFrame, "", JSType::kString, false);
}

const QVariantMap SwitchToIFrameCompute::_hints = SwitchToIFrameCompute::init_hints();
QVariantMap SwitchToIFrameCompute::init_hints() {
  QVariantMap m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kIFrame, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kIFrame, HintType::kDescription,
           "The path to the iframe that subsequent nodes shall act on. IFrame paths are made up of iframe indexes separated by a forward slash. For example: 1/2/3");
  return m;
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
  create_input(Message::kWrapType, 0, JSType::kNumber, false);
  create_input(Message::kTextValues, QStringList(), JSType::kArray, false);
  create_input(Message::kImageValues, QStringList(), JSType::kArray, false);
}

const QVariantMap CreateSetFromValuesCompute::_hints = CreateSetFromValuesCompute::init_hints();
QVariantMap CreateSetFromValuesCompute::init_hints() {
  QVariantMap m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kWrapType, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kWrapType, HintType::kEnum, to_underlying(EnumHint::kWrapType));
  add_hint(m, Message::kWrapType, HintType::kDescription, "The element type to use when creating the set.");

  add_hint(m, Message::kTextValues, HintType::kJSType, to_underlying(JSType::kArray));
  add_hint(m, Message::kTextValues, HintType::kDescription, "The set of text values used to find text elements.");
  add_hint(m, Message::kTextValues, HintType::kResizable, true);
  add_hint(m, Message::kTextValues, HintType::kElementJSType, to_underlying(JSType::kString));

  add_hint(m, Message::kImageValues, HintType::kJSType, to_underlying(JSType::kArray));
  add_hint(m, Message::kImageValues, HintType::kDescription, "The set of overlapping images used to find image elements.");
  add_hint(m, Message::kImageValues, HintType::kResizable, true);
  add_hint(m, Message::kImageValues, HintType::kElementJSType, to_underlying(JSType::kString));
  return m;
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

  create_input(Message::kWrapType, 0, JSType::kNumber, false);

}

const QVariantMap CreateSetFromTypeCompute::_hints = CreateSetFromTypeCompute::init_hints();
QVariantMap CreateSetFromTypeCompute::init_hints() {
  QVariantMap m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kWrapType, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kWrapType, HintType::kEnum, to_underlying(EnumHint::kWrapType));
  add_hint(m, Message::kWrapType, HintType::kDescription, "The element type to use when creating the set.");
  return m;
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
  create_input(Message::kSetIndex, 0, JSType::kNumber, false);
}

const QVariantMap DeleteSetCompute::_hints = DeleteSetCompute::init_hints();
QVariantMap DeleteSetCompute::init_hints() {
  QVariantMap m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kSetIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kSetIndex, HintType::kDescription, "The zero based index that identifies the element set to delete.");
  return m;
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
  create_input(Message::kSetIndex, 0, JSType::kNumber, false);
  create_input(Message::kDirection, 0, JSType::kNumber, false);
  create_input(Message::kWrapType, 0, JSType::kNumber, false);
}

const QVariantMap ShiftSetCompute::_hints = ShiftSetCompute::init_hints();
QVariantMap ShiftSetCompute::init_hints() {
  QVariantMap m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kSetIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kSetIndex, HintType::kDescription, "The zero based index that identifies the element set to shift.");

  add_hint(m, Message::kDirection, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kDirection, HintType::kEnum, to_underlying(EnumHint::kDirectionType));
  add_hint(m, Message::kDirection, HintType::kDescription, "The direction in which to shift the set elements to.");

  add_hint(m, Message::kWrapType, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kWrapType, HintType::kEnum, to_underlying(EnumHint::kWrapType));
  add_hint(m, Message::kWrapType, HintType::kDescription, "The type of elements to shift to.");
  return m;
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
  create_input(Message::kSetIndex, 0, JSType::kNumber, false);
  create_input(Message::kOverlayIndex, 0, JSType::kNumber, false);
  create_input(Message::kMouseAction, 0, JSType::kNumber, false);
  QVariantMap pos;
  pos["x"] = 0;
  pos["y"] = 0;
  create_input(Message::kOverlayRelClickPos, pos, JSType::kObject, false);
}

const QVariantMap MouseActionCompute::_hints = MouseActionCompute::init_hints();
QVariantMap MouseActionCompute::init_hints() {
  QVariantMap m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kSetIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kSetIndex, HintType::kDescription, "The zero based index that identifies the element set which contains the element to act on.");

  add_hint(m, Message::kOverlayIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kOverlayIndex, HintType::kDescription, "The zero based index that identifies the element to act on inside the set.");

  add_hint(m, Message::kOverlayRelClickPos, HintType::kJSType, to_underlying(JSType::kObject));
  add_hint(m, Message::kOverlayRelClickPos, HintType::kElementJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kOverlayRelClickPos, HintType::kDescription, "The position to perform our action at, relative to the element itself.");

  add_hint(m, Message::kMouseAction, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kMouseAction, HintType::kEnum, to_underlying(EnumHint::kMouseActionType));
  add_hint(m, Message::kMouseAction, HintType::kDescription, "The type of mouse mouse action to perform.");
  return m;
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
  create_input(Message::kSetIndex, 0, JSType::kNumber, false);
  create_input(Message::kOverlayIndex, 0, JSType::kNumber, false);
  QVariantMap pos;
  pos["x"] = 0;
  pos["y"] = 0;
  create_input(Message::kOverlayRelClickPos, pos, JSType::kObject, false);
}

const QVariantMap StartMouseHoverActionCompute::_hints = StartMouseHoverActionCompute::init_hints();
QVariantMap StartMouseHoverActionCompute::init_hints() {
  QVariantMap m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kSetIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kSetIndex, HintType::kDescription, "The zero based index that identifies the element set which contains the element to act on.");

  add_hint(m, Message::kOverlayIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kOverlayIndex, HintType::kDescription, "The zero based index that identifies the element to hover on inside the set.");

  add_hint(m, Message::kOverlayRelClickPos, HintType::kJSType, to_underlying(JSType::kObject));
  add_hint(m, Message::kOverlayRelClickPos, HintType::kElementJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kOverlayRelClickPos, HintType::kDescription, "The position to perform our hover at, relative to the element itself.");
  return m;
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
  create_input(Message::kSetIndex, 0, JSType::kNumber, false);
  create_input(Message::kOverlayIndex, 0, JSType::kNumber, false);
  create_input(Message::kTextAction, 0, JSType::kNumber, false);
  create_input(Message::kText, "", JSType::kString, false); // Only used when the text action is set to send text.
}

const QVariantMap TextActionCompute::_hints = TextActionCompute::init_hints();
QVariantMap TextActionCompute::init_hints() {
  QVariantMap m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kSetIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kSetIndex, HintType::kDescription, "The zero based index that identifies the element set which contains the element to type on.");

  add_hint(m, Message::kOverlayIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kOverlayIndex, HintType::kDescription, "The zero based index that identifies the element to type on inside the set.");

  add_hint(m, Message::kTextAction, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kTextAction, HintType::kEnum, to_underlying(EnumHint::kTextActionType));
  add_hint(m, Message::kTextAction, HintType::kDescription, "The type of text action to perform.");

  add_hint(m, Message::kText, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kText, HintType::kDescription, "The text to type.");
  return m;
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
  create_input(Message::kSetIndex, 0, JSType::kNumber, false);
  create_input(Message::kOverlayIndex, 0, JSType::kNumber, false);
  create_input(Message::kElementAction, 0, JSType::kNumber, false);
  create_input(Message::kOptionText, "", JSType::kString, false); // Only used when the element action is set to select option from dropdown.
  create_input(Message::kDirection, 0, JSType::kNumber, false); // Only used when the element action is set to scroll.
}

const QVariantMap ElementActionCompute::_hints = ElementActionCompute::init_hints();
QVariantMap ElementActionCompute::init_hints() {
  QVariantMap m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kSetIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kSetIndex, HintType::kDescription, "The zero based index that identifies the element set which contains the element to act on.");

  add_hint(m, Message::kOverlayIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kOverlayIndex, HintType::kDescription, "The zero based index that identifies the element to act on inside the set.");

  add_hint(m, Message::kElementAction, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kElementAction, HintType::kEnum, to_underlying(EnumHint::kElementActionType));
  add_hint(m, Message::kElementAction, HintType::kDescription, "The type of element action to perform.");

  add_hint(m, Message::kOptionText, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kOptionText, HintType::kDescription, "The text to select from dropdowns.");

  add_hint(m, Message::kDirection, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kDirection, HintType::kEnum, to_underlying(EnumHint::kDirectionType));
  add_hint(m, Message::kDirection, HintType::kDescription, "The direction to scroll.");
  return m;
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
  create_input(Message::kSetIndex, 0, JSType::kNumber, false);
  create_input(Message::kDirection, 0, JSType::kNumber, false); // Only used when the element action is set to scroll.
  QVariantMap match_criteria;
  match_criteria[Message::kMatchLeft] = true;
  match_criteria[Message::kMatchRight] = false;
  match_criteria[Message::kMatchTop] = false;
  match_criteria[Message::kMatchBottom] = false;
  match_criteria[Message::kMatchFont] = true;
  match_criteria[Message::kMatchFontSize] = true;
  create_input(Message::kMatchCriteria, match_criteria, JSType::kObject, false);
}

const QVariantMap ExpandSetCompute::_hints = ExpandSetCompute::init_hints();
QVariantMap ExpandSetCompute::init_hints() {
  QVariantMap m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kSetIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kSetIndex, HintType::kDescription, "The zero based index that identifies the element set to expand.");

  add_hint(m, Message::kDirection, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kDirection, HintType::kEnum, to_underlying(EnumHint::kDirectionType));
  add_hint(m, Message::kDirection, HintType::kDescription, "The direction in which to expand the set.");

  add_hint(m, Message::kMatchCriteria, HintType::kJSType, to_underlying(JSType::kObject));
  add_hint(m, Message::kMatchCriteria, HintType::kDescription, "The match criteria used when expanding the set.");
  return m;
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
  create_input(Message::kSetIndex, 0, JSType::kNumber, false);
}

const QVariantMap MarkSetCompute::_hints = MarkSetCompute::init_hints();
QVariantMap MarkSetCompute::init_hints() {
  QVariantMap m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kSetIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kSetIndex, HintType::kDescription, "The zero based index that identifies the element set to mark.");
  return m;
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
  create_input(Message::kSetIndex, 0, JSType::kNumber, false);
}

const QVariantMap UnmarkSetCompute::_hints = UnmarkSetCompute::init_hints();
QVariantMap UnmarkSetCompute::init_hints() {
  QVariantMap m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kSetIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kSetIndex, HintType::kDescription, "The zero based index that identifies the element set to unmark.");
  return m;
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
  create_input(Message::kSetIndex, 0, JSType::kNumber, false);
  create_input(Message::kDirection, 0, JSType::kNumber, false);
}

const QVariantMap ShrinkSetToSideCompute::_hints = ShrinkSetToSideCompute::init_hints();
QVariantMap ShrinkSetToSideCompute::init_hints() {
  QVariantMap m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kSetIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kSetIndex, HintType::kDescription, "The zero based index that identifies the element set to shrink on one side.");

  add_hint(m, Message::kDirection, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kDirection, HintType::kEnum, to_underlying(EnumHint::kDirectionType));
  add_hint(m, Message::kDirection, HintType::kDescription, "The side of the set from which to perform the shrink.");
  return m;
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
  create_input(Message::kSetIndex, 0, JSType::kNumber, false);
  create_input(Message::kDirections, QVariantList(), JSType::kArray, false);
}

const QVariantMap ShrinkAgainstMarkedCompute::_hints = ShrinkAgainstMarkedCompute::init_hints();
QVariantMap ShrinkAgainstMarkedCompute::init_hints() {
  QVariantMap m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kSetIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kSetIndex, HintType::kDescription, "The zero based index that identifies the element set to shrink against.");

  add_hint(m, Message::kDirections, HintType::kJSType, to_underlying(JSType::kArray));
  add_hint(m, Message::kDirections, HintType::kElementJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kDirections, HintType::kElementEnum, to_underlying(EnumHint::kDirectionType));
  add_hint(m, Message::kDirections, HintType::kResizable, true);
  add_hint(m, Message::kDirections, HintType::kDescription, "The sides of the marked sets used to intersect the set of interest.");
  return m;
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
