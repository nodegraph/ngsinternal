#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>
#include <guicomponents/computes/nodejsworker.h>
#include <guicomponents/comms/commtypes.h>

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
  get_dep_loader()->register_fixed_dep(_worker, Path());
  get_dep_loader()->register_fixed_dep(_scheduler, Path());
}

BrowserCompute::~BrowserCompute() {
}

void BrowserCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);

  EntityConfig c = config;
  c.expose_plug = true;
  c.unconnected_value = QJsonObject();

  create_input("in", c);
  create_output("out", c);
}

void BrowserCompute::init_hints(QJsonObject& m) {
  add_hint(m, "in", HintKey::kDescriptionHint, "The main object that flows through this node. This cannot be set manually.");
}

void BrowserCompute::dump_map(const QJsonObject& inputs) const {
  for(QJsonObject::const_iterator iter = inputs.begin(); iter != inputs.end(); ++iter) {
    std::cerr << iter.key().toStdString() << " : " << iter.value().toString().toStdString() << "\n";
  }
}

void BrowserCompute::update_wires() {
  // This caches the dep, and will only dirty this component when it changes.
  _enter = find_enter_node();
}

Entity* BrowserCompute::find_group_context() const {
  Entity* e = our_entity();
  while(e) {
    if (e->get_did() == EntityDID::kBrowserGroupNodeEntity) {
      return e;
    }
    e = e->get_parent();
  }
  assert(false);
  return NULL;
}

Dep<EnterBrowserGroupCompute> BrowserCompute::find_enter_node() {
  Entity* group = find_group_context();
  Entity* enter = group->get_child("enter");
  assert(enter);
  return get_dep<EnterBrowserGroupCompute>(enter);
}

void BrowserCompute::pre_update_state(TaskContext& tc) {
  internal();
  // Make sure the browser is open.
  _worker->queue_open_browser(tc);

  // Merge chain state.
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

void NavigateToCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);

  EntityConfig c = config;
  c.expose_plug = false;
  c.unconnected_value = "";
  create_input(Message::kURL, c);
}

const QJsonObject NavigateToCompute::_hints = NavigateToCompute::init_hints();
QJsonObject NavigateToCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);
  add_hint(m, Message::kURL, HintKey::kDescriptionHint, "The url the browser should to navigate to.");
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

bool NavigateBackCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_navigate_back(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

bool NavigateForwardCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_navigate_forward(tc);
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

void FindElementByValuesCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);

  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 0;
    create_input(Message::kWrapType, c);
  }
  {
    QJsonArray string_arr;
    string_arr.push_back("example");

    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = string_arr;
    create_input(Message::kTargetValues, c);
  }
}

const QJsonObject FindElementByValuesCompute::_hints = FindElementByValuesCompute::init_hints();
QJsonObject FindElementByValuesCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kWrapType, HintKey::kEnumHint, to_underlying(EnumHintValue::kWrapType));
  add_hint(m, Message::kWrapType, HintKey::kDescriptionHint, "The type of the elements to put into the set.");

  add_hint(m, Message::kTargetValues, HintKey::kDescriptionHint, "The texts or image urls used to find elements.");
  add_hint(m, Message::kTargetValues, HintKey::kElementJSTypeHint, to_underlying(JSType::kString));
  return m;
}


bool FindElementByValuesCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_find_element_by_values(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void FindElementByTypeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);

  EntityConfig c = config;
  c.expose_plug = false;
  c.unconnected_value = 0;

  create_input(Message::kWrapType, c);

}

const QJsonObject FindElementByTypeCompute::_hints = FindElementByTypeCompute::init_hints();
QJsonObject FindElementByTypeCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kWrapType, HintKey::kEnumHint, to_underlying(EnumHintValue::kWrapType));
  add_hint(m, Message::kWrapType, HintKey::kDescriptionHint, "The element type to use when creating the set.");
  return m;
}

bool FindElementByTypeCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_find_element_by_type(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

// ---------------------------------------------------------------------------------------------------

void ShiftElementByTypeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);

  EntityConfig c = config;
  c.expose_plug = false;
  c.unconnected_value = 0;

  create_input(Message::kDirection, c);
  create_input(Message::kWrapType, c);
}

const QJsonObject ShiftElementByTypeCompute::_hints = ShiftElementByTypeCompute::init_hints();
QJsonObject ShiftElementByTypeCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kDirection, HintKey::kEnumHint, to_underlying(EnumHintValue::kDirectionType));
  add_hint(m, Message::kDirection, HintKey::kDescriptionHint, "The direction in which to shift the set elements to.");

  add_hint(m, Message::kWrapType, HintKey::kEnumHint, to_underlying(EnumHintValue::kWrapType));
  add_hint(m, Message::kWrapType, HintKey::kDescriptionHint, "The type of elements to shift to.");

  return m;
}

bool ShiftElementByTypeCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_shift_element_by_type(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

// ---------------------------------------------------------------------------------------------------

void ShiftElementByValuesCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);

  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 0;

    create_input(Message::kDirection, c);
    create_input(Message::kWrapType, c);\
  }

  {
    QJsonArray string_arr;
    string_arr.push_back("example");

    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = string_arr;
    create_input(Message::kTargetValues, c);
  }


}

const QJsonObject ShiftElementByValuesCompute::_hints = ShiftElementByValuesCompute::init_hints();
QJsonObject ShiftElementByValuesCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kDirection, HintKey::kEnumHint, to_underlying(EnumHintValue::kDirectionType));
  add_hint(m, Message::kDirection, HintKey::kDescriptionHint, "The direction in which to shift the set elements to.");

  add_hint(m, Message::kWrapType, HintKey::kEnumHint, to_underlying(EnumHintValue::kWrapType));
  add_hint(m, Message::kWrapType, HintKey::kDescriptionHint, "The type of elements to shift to.");

  add_hint(m, Message::kTargetValues, HintKey::kDescriptionHint, "The texts or image urls used to find elements.");
  return m;
}

bool ShiftElementByValuesCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_shift_element_by_values(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}


// ---------------------------------------------------------------------------------------------------

void MouseActionCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);

  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "";
    create_input(Message::kMouseAction, c);
  }
  {
    QJsonObject pos;
    pos.insert("x", 0);
    pos.insert("y", 0);

    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = pos;

    create_input(Message::kLocalMousePosition, c);
  }
}

const QJsonObject MouseActionCompute::_hints = MouseActionCompute::init_hints();
QJsonObject MouseActionCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kLocalMousePosition, HintKey::kElementJSTypeHint, to_underlying(JSType::kNumber));
  add_hint(m, Message::kLocalMousePosition, HintKey::kDescriptionHint, "The position to perform our action at, relative to the element itself.");

  add_hint(m, Message::kMouseAction, HintKey::kEnumHint, to_underlying(EnumHintValue::kMouseActionType));
  add_hint(m, Message::kMouseAction, HintKey::kDescriptionHint, "The type of mouse mouse action to perform.");
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

void StartMouseHoverActionCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);

  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 0;
  }
  {
    QJsonObject pos;
    pos.insert("x", 0);
    pos.insert("y", 0);

    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = pos;

    create_input(Message::kLocalMousePosition, c);
  }
}

const QJsonObject StartMouseHoverActionCompute::_hints = StartMouseHoverActionCompute::init_hints();
QJsonObject StartMouseHoverActionCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kLocalMousePosition, HintKey::kElementJSTypeHint, to_underlying(JSType::kNumber));
  add_hint(m, Message::kLocalMousePosition, HintKey::kDescriptionHint, "The position to perform our hover at, relative to the element itself.");
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

void TextActionCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 0;
    create_input(Message::kTextAction, c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "";
    create_input(Message::kText, c);  // Only used when the text action is set to send text.
  }
}

const QJsonObject TextActionCompute::_hints = TextActionCompute::init_hints();
QJsonObject TextActionCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kTextAction, HintKey::kEnumHint, to_underlying(EnumHintValue::kTextActionType));
  add_hint(m, Message::kTextAction, HintKey::kDescriptionHint, "The type of text action to perform.");

  add_hint(m, Message::kText, HintKey::kDescriptionHint, "The text to type.");
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

void ElementActionCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 0;

    create_input(Message::kElementAction, c);
    create_input(Message::kDirection, c);  // Only used when the element action is set to scroll.
  }
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "";

    create_input(Message::kOptionText, c); // Only used when the element action is set to select option from dropdown.

    c.unconnected_value = "extracted_text";
    create_input(Message::kTextDataName, c); // Only used when the element action is set to get_text.
  }
}

const QJsonObject ElementActionCompute::_hints = ElementActionCompute::init_hints();
QJsonObject ElementActionCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kElementAction, HintKey::kEnumHint, to_underlying(EnumHintValue::kElementActionType));
  add_hint(m, Message::kElementAction, HintKey::kDescriptionHint, "The type of element action to perform.");

  add_hint(m, Message::kOptionText, HintKey::kDescriptionHint, "The text to select from dropdowns.");

  add_hint(m, Message::kDirection, HintKey::kEnumHint, to_underlying(EnumHintValue::kDirectionType));
  add_hint(m, Message::kDirection, HintKey::kDescriptionHint, "The direction to scroll.");

  add_hint(m, Message::kTextDataName, HintKey::kDescriptionHint, "The name used to reference the extracted text data.");

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

}
