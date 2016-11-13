#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>
#include <guicomponents/comms/browserworker.h>

#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/computes/browsercomputes.h>
#include <guicomponents/computes/enterbrowsergroupcompute.h>

#include <functional>

namespace ngs {

//--------------------------------------------------------------------------------

BrowserCompute::BrowserCompute(Entity* entity, ComponentDID did)
    : Compute(entity, did),
      _worker(this),
      _scheduler(this),
      _enter(this) {
  get_dep_loader()->register_fixed_dep(_worker, Path({}));
  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
  get_dep_loader()->register_fixed_dep(_enter, Path({"..","group_context"}));
}

BrowserCompute::~BrowserCompute() {
}

void BrowserCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("in", QJsonObject());
  create_output("out");
}

void BrowserCompute::init_hints(QJsonObject& m) {
  add_hint(m, "in", HintType::kJSType, to_underlying(JSType::kObject));
  add_hint(m, "in", HintType::kDescription, "The main object that flows through this node. This cannot be set manually.");
}

void BrowserCompute::dump_map(const QJsonObject& inputs) const {
  for(QJsonObject::const_iterator iter = inputs.begin(); iter != inputs.end(); ++iter) {
    std::cerr << iter.key().toStdString() << " : " << iter.value().toString().toStdString() << "\n";
  }
}

void BrowserCompute::pre_update_state(TaskContext& tc) {
  internal();
  QJsonObject inputs = _inputs->get_input_values();
  _worker->queue_merge_chain_state(tc, inputs);
  // Make sure nothing is loading right now.
  // Note in general a page may start loading content at random times.
  // For examples ads may rotate and flip content.
  _worker->queue_wait_until_loaded(tc);
}

void BrowserCompute::post_update_state(TaskContext& tc) {
  internal();
  std::function<void(const QJsonObject&)> callback = std::bind(&BrowserCompute::receive_chain_state,this,std::placeholders::_1);
  _worker->queue_receive_chain_state(tc, callback);
}

void BrowserCompute::receive_chain_state(const QJsonObject& chain_state) {
  internal();
  clean_finalize();

  // This copies the incoming data, to our output.
  // Derived classes will in add in extra data, extracted from the web.
  QJsonValue incoming = _inputs->get_input_value("in");
  set_output("out", incoming);
}

//--------------------------------------------------------------------------------

bool OpenBrowserCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_open_browser(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

bool CloseBrowserCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_close_browser(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

bool IsBrowserOpenCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_is_browser_open(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

bool ResizeBrowserCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_resize_browser(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void NavigateToCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kURL, "", false);
}

const QJsonObject NavigateToCompute::_hints = NavigateToCompute::init_hints();
QJsonObject NavigateToCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kURL, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kURL, HintType::kDescription, "The url the browser should to navigate to.");
  return m;
}

bool NavigateToCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_navigate_to(tc);
  _worker->queue_wait_until_loaded(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

bool NavigateRefreshCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_navigate_refresh(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void SwitchToIFrameCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kIFrame, "", false);
}

const QJsonObject SwitchToIFrameCompute::_hints = SwitchToIFrameCompute::init_hints();
QJsonObject SwitchToIFrameCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kIFrame, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kIFrame, HintType::kDescription,
           "The path to the iframe that subsequent nodes shall act on. IFrame paths are made up of iframe indexes separated by a forward slash. For example: 1/2/3");
  return m;
}

bool SwitchToIFrameCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_switch_to_iframe(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void CreateSetFromValuesCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kWrapType, 0, false);
  QJsonArray string_arr;
  string_arr.push_back("example");
  create_input(Message::kTextValues, string_arr, false);
  create_input(Message::kImageValues, string_arr, false);
}

const QJsonObject CreateSetFromValuesCompute::_hints = CreateSetFromValuesCompute::init_hints();
QJsonObject CreateSetFromValuesCompute::init_hints() {
  QJsonObject m;
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

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_create_set_by_matching_values(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void CreateSetFromTypeCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();

  create_input(Message::kWrapType, 0, false);

}

const QJsonObject CreateSetFromTypeCompute::_hints = CreateSetFromTypeCompute::init_hints();
QJsonObject CreateSetFromTypeCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kWrapType, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kWrapType, HintType::kEnum, to_underlying(EnumHint::kWrapType));
  add_hint(m, Message::kWrapType, HintType::kDescription, "The element type to use when creating the set.");
  return m;
}

bool CreateSetFromTypeCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_create_set_by_matching_type(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void DeleteSetCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, 0, false);
}

const QJsonObject DeleteSetCompute::_hints = DeleteSetCompute::init_hints();
QJsonObject DeleteSetCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kSetIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kSetIndex, HintType::kDescription, "The zero based index that identifies the element set to delete.");
  return m;
}

bool DeleteSetCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_delete_set(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void ShiftSetCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, 0, false);
  create_input(Message::kDirection, 0, false);
  create_input(Message::kWrapType, 0, false);
}

const QJsonObject ShiftSetCompute::_hints = ShiftSetCompute::init_hints();
QJsonObject ShiftSetCompute::init_hints() {
  QJsonObject m;
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

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_shift_set(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void MouseActionCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, 0, false);
  create_input(Message::kOverlayIndex, 0, false);
  create_input(Message::kMouseAction, 0, false);
  QJsonObject pos;
  pos.insert("x", 0);
  pos.insert("y", 0);
  create_input(Message::kOverlayRelClickPos, pos, false);
}

const QJsonObject MouseActionCompute::_hints = MouseActionCompute::init_hints();
QJsonObject MouseActionCompute::init_hints() {
  QJsonObject m;
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

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_perform_mouse_action(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void StartMouseHoverActionCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, 0, false);
  create_input(Message::kOverlayIndex, 0, false);
  QJsonObject pos;
  pos.insert("x", 0);
  pos.insert("y", 0);
  create_input(Message::kOverlayRelClickPos, pos, false);
}

const QJsonObject StartMouseHoverActionCompute::_hints = StartMouseHoverActionCompute::init_hints();
QJsonObject StartMouseHoverActionCompute::init_hints() {
  QJsonObject m;
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

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_start_mouse_hover(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

bool StopMouseHoverActionCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_stop_mouse_hover(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void TextActionCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, 0, false);
  create_input(Message::kOverlayIndex, 0, false);
  create_input(Message::kTextAction, 0, false);
  create_input(Message::kText, "", false); // Only used when the text action is set to send text.
}

const QJsonObject TextActionCompute::_hints = TextActionCompute::init_hints();
QJsonObject TextActionCompute::init_hints() {
  QJsonObject m;
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

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_perform_text_action(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void ElementActionCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, 0, false);
  create_input(Message::kOverlayIndex, 0, false);
  create_input(Message::kElementAction, 0, false);
  create_input(Message::kOptionText, "", false); // Only used when the element action is set to select option from dropdown.
  create_input(Message::kDirection, 0, false); // Only used when the element action is set to scroll.
  create_input(Message::kTextDataName, "extracted_text", false); // Only used when the element action is set to get_text.
}

const QJsonObject ElementActionCompute::_hints = ElementActionCompute::init_hints();
QJsonObject ElementActionCompute::init_hints() {
  QJsonObject m;
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

  add_hint(m, Message::kTextDataName, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kTextDataName, HintType::kDescription, "The name used to reference the extracted text data.");

  return m;
}

void ElementActionCompute::receive_chain_state(const QJsonObject& chain_state) {
  internal();
  clean_finalize();

  // This copies the incoming data, to our output.
  // Derived classes will in add in extra data, extracted from the web.
  QJsonValue incoming = _inputs->get_input_value("in");
  QString text_data_name = _inputs->get_input_value(Message::kTextDataName).toString();
  if (incoming.isObject()) {
    QJsonObject obj = incoming.toObject();
    obj.insert(text_data_name, chain_state.value("value"));
    set_output("out", obj);
  } else {
    set_output("out", incoming);
  }
}

bool ElementActionCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_perform_element_action(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void ExpandSetCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, 0, false);
  create_input(Message::kDirection, 0, false); // Only used when the element action is set to scroll.
  QJsonObject match_criteria;
  match_criteria.insert(Message::kMatchLeft, true);
  match_criteria.insert(Message::kMatchRight, false);
  match_criteria.insert(Message::kMatchTop, false);
  match_criteria.insert(Message::kMatchBottom, false);
  match_criteria.insert(Message::kMatchFont, true);
  match_criteria.insert(Message::kMatchFontSize, true);
  create_input(Message::kMatchCriteria, match_criteria, false);
}

const QJsonObject ExpandSetCompute::_hints = ExpandSetCompute::init_hints();
QJsonObject ExpandSetCompute::init_hints() {
  QJsonObject m;
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

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_expand_set(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void MarkSetCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, 0, false);
}

const QJsonObject MarkSetCompute::_hints = MarkSetCompute::init_hints();
QJsonObject MarkSetCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kSetIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kSetIndex, HintType::kDescription, "The zero based index that identifies the element set to mark.");
  return m;
}

bool MarkSetCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_mark_set(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void UnmarkSetCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, 0, false);
}

const QJsonObject UnmarkSetCompute::_hints = UnmarkSetCompute::init_hints();
QJsonObject UnmarkSetCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kSetIndex, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kSetIndex, HintType::kDescription, "The zero based index that identifies the element set to unmark.");
  return m;
}

bool UnmarkSetCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_unmark_set(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

bool MergeSetsCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_merge_sets(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void ShrinkSetToSideCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, 0, false);
  create_input(Message::kDirection, 0, false);
}

const QJsonObject ShrinkSetToSideCompute::_hints = ShrinkSetToSideCompute::init_hints();
QJsonObject ShrinkSetToSideCompute::init_hints() {
  QJsonObject m;
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

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_shrink_set_to_side(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void ShrinkAgainstMarkedCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, 0, false);
  QJsonArray number_arr;
  number_arr.push_back(0);
  create_input(Message::kDirections, number_arr, false);
}

const QJsonObject ShrinkAgainstMarkedCompute::_hints = ShrinkAgainstMarkedCompute::init_hints();
QJsonObject ShrinkAgainstMarkedCompute::init_hints() {
  QJsonObject m;
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

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_shrink_against_marked(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

// -------------------------------------------------------------------------------------------------------------------------
// Firebase
// -------------------------------------------------------------------------------------------------------------------------

void FirebaseSignInCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kEmail, "user@user.com", false);
  create_input(Message::kPassword, "password", false);
}

const QJsonObject FirebaseSignInCompute::_hints = FirebaseSignInCompute::init_hints();
QJsonObject FirebaseSignInCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kEmail, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kEmail, HintType::kDescription, "Email address for firebase authentication.");

  add_hint(m, Message::kPassword, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kPassword, HintType::kDescription, "Password for firebase authentication.");

  return m;
}

bool FirebaseSignInCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  //BrowserCompute::pre_update_state(tc);
  QJsonObject inputs = _inputs->get_input_values();
  _worker->queue_merge_chain_state(tc, inputs);

  _worker->queue_firebase_sign_in(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

bool FirebaseSignOutCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  //BrowserCompute::pre_update_state(tc);
  QJsonObject inputs = _inputs->get_input_values();
  _worker->queue_merge_chain_state(tc, inputs);

  _worker->queue_firebase_sign_out(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void FirebaseWriteDataCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kPath, "some/path", false);
  create_input(Message::kValue, "hello there", false);
}

const QJsonObject FirebaseWriteDataCompute::_hints = FirebaseWriteDataCompute::init_hints();
QJsonObject FirebaseWriteDataCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kPath, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kPath, HintType::kDescription, "Firebase data path.");

  add_hint(m, Message::kValue, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kValue, HintType::kDescription, "The data to write at the path.");

  return m;
}

bool FirebaseWriteDataCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  //BrowserCompute::pre_update_state(tc);
  QJsonObject inputs = _inputs->get_input_values();
  _worker->queue_merge_chain_state(tc, inputs);

  _worker->queue_firebase_write_data(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void FirebaseReadDataCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kPath, "some/path", false);
  create_input(Message::kDataName, "data", false);
}

const QJsonObject FirebaseReadDataCompute::_hints = FirebaseReadDataCompute::init_hints();
QJsonObject FirebaseReadDataCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kPath, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kPath, HintType::kDescription, "Firebase data path.");

  add_hint(m, Message::kDataName, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kDataName, HintType::kDescription, "The name given to the data read from Firebase. This will be merged into the data flowing through \"in\".");

  return m;
}

void FirebaseReadDataCompute::receive_chain_state(const QJsonObject& chain_state) {
  internal();
  clean_finalize();

  // This copies the incoming data, to our output.
  // Derived classes will in add in extra data, extracted from the web.
  QJsonValue incoming = _inputs->get_input_value("in");
  QString data_name = _inputs->get_input_value(Message::kDataName).toString();
  if (incoming.isObject()) {
    QJsonObject obj = incoming.toObject();
    obj.insert(data_name, chain_state.value("value"));
    set_output("out", obj);
  } else {
    set_output("out", incoming);
  }
}

bool FirebaseReadDataCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  //BrowserCompute::pre_update_state(tc);
  QJsonObject inputs = _inputs->get_input_values();
  _worker->queue_merge_chain_state(tc, inputs);

  _worker->queue_firebase_read_data(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void FirebaseListenToChangesCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kPath, "some/path", false);
}

const QJsonObject FirebaseListenToChangesCompute::_hints = FirebaseListenToChangesCompute::init_hints();
QJsonObject FirebaseListenToChangesCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kPath, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kPath, HintType::kDescription, "Firebase data path.");

  return m;
}

bool FirebaseListenToChangesCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  //BrowserCompute::pre_update_state(tc);
  QJsonObject inputs = _inputs->get_input_values();
  _worker->queue_merge_chain_state(tc, inputs);

  _worker->queue_firebase_listen_to_changes(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

}
