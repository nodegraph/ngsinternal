#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>
#include <guicomponents/comms/guitypes.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/computes/browsercomputes.h>
#include <guicomponents/computes/enterbrowsergroupcompute.h>
#include <guicomponents/computes/taskqueuer.h>

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
  add_hint(m, "in", GUITypes::HintKey::DescriptionHint, "The main object that flows through this node. This cannot be set manually.");
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
  _worker->queue_scroll_element_into_view(tc);
  _worker->queue_update_current_tab(tc);
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

bool ReleaseBrowserCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_release_browser(tc);
  _worker->queue_open_browser(tc);
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

void ResizeBrowserCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);

  EntityConfig c = config;
  c.expose_plug = false;
  c.unconnected_value = 1024;
  create_input(Message::kWidth, c);

  c.unconnected_value = 1150;
  create_input(Message::kHeight, c);
}

const QJsonObject ResizeBrowserCompute::_hints = ResizeBrowserCompute::init_hints();
QJsonObject ResizeBrowserCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kWidth, GUITypes::HintKey::DescriptionHint, "The desired width of the browser.");
  add_hint(m, Message::kHeight, GUITypes::HintKey::DescriptionHint, "The desired height of the browser.");

  return m;
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

void GetActiveTabTitleCompute::receive_chain_state(const QJsonObject& chain_state) {
  internal();
  clean_finalize();

  // Grab the title from the chain state.
  QString title = chain_state["value"].toString();
  std::cerr << "Got browser title: " << title.toStdString() << "\n";
  // Grab the incoming object.
  QJsonValue incoming = _inputs->get_input_value("in");
  // Add the url into the incoming object under the key, "value".
  QJsonObject obj = incoming.toObject();
  obj.insert("value", title);
  set_output("out", obj);
}

bool GetActiveTabTitleCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_get_active_tab_title(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void DestroyCurrentTabCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);
}

const QJsonObject DestroyCurrentTabCompute::_hints = DestroyCurrentTabCompute::init_hints();
QJsonObject DestroyCurrentTabCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);
  return m;
}

bool DestroyCurrentTabCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_destroy_current_tab(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

bool OpenTabCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_open_tab(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

bool AcceptSaveDialogCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_accept_save_dialog(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void DownloadVideoCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);

  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "";
    create_input(Message::kDirectory, c);
  }

  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 0;
    create_input(Message::kMaxWidth, c);
  }

  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 0;
    create_input(Message::kMaxHeight, c);
  }

  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 0;
    create_input(Message::kMaxFilesize, c);
  }

}

const QJsonObject DownloadVideoCompute::_hints = DownloadVideoCompute::init_hints();
QJsonObject DownloadVideoCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kDirectory, GUITypes::HintKey::DescriptionHint, "The directory to save the video to. Leave empty to use the default directory.");
  add_hint(m, Message::kMaxWidth, GUITypes::HintKey::DescriptionHint, "The maximum video width (in pixels) to download. Zero will download the largest.");
  add_hint(m, Message::kMaxHeight, GUITypes::HintKey::DescriptionHint, "The maximum video height (in pixels) to download. Zero will download the largest.");
  add_hint(m, Message::kMaxFilesize, GUITypes::HintKey::DescriptionHint, "The maximum video filesize (in megabytes) to download. Zero will download the largest.");
  return m;
}

bool DownloadVideoCompute::update_state(){
  internal();
  BrowserCompute::update_state();


  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);

  // Set the url chain property to the current url.
  _worker->queue_get_current_url(tc);
  _worker->queue_copy_chain_property(tc, Message::kValue, Message::kURL);

  // Download the video.
  _worker->queue_download_video(tc);

  // The video filename will be inserted into the value property.
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
  add_hint(m, Message::kURL, GUITypes::HintKey::DescriptionHint, "The url the browser should to navigate to.");
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

bool GetCurrentURLCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_get_current_url(tc);
  BrowserCompute::post_update_state(tc);
  return false;
}

void GetCurrentURLCompute::receive_chain_state(const QJsonObject& chain_state) {
  internal();
  clean_finalize();

  // Grab the url from the chain state.
  QString url = chain_state["value"].toString();
  std::cerr << "Got final url: " << url.toStdString() << "\n";
  // Grab the incoming object.
  QJsonValue incoming = _inputs->get_input_value("in");
  // Add the url into the incoming object under the key, "value".
  QJsonObject obj = incoming.toObject();
  obj.insert("value", url);
  set_output("out", obj);
}


void FindElementByPositionCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);

  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 0;
    create_input(Message::kWrapType, c);
  }

  {
    QJsonObject pos;
    pos.insert("x", 0);
    pos.insert("y", 0);

    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = pos;

    create_input(Message::kGlobalMousePosition, c);
  }
}

const QJsonObject FindElementByPositionCompute::_hints = FindElementByPositionCompute::init_hints();
QJsonObject FindElementByPositionCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kWrapType, GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::WrapType));
  add_hint(m, Message::kWrapType, GUITypes::HintKey::DescriptionHint, "The type of the elements to put into the set.");

  add_hint(m, Message::kGlobalMousePosition, GUITypes::HintKey::ElementJSTypeHint, to_underlying(GUITypes::JSType::Number));
  add_hint(m, Message::kGlobalMousePosition, GUITypes::HintKey::DescriptionHint, "The position to look for the element at.");

  return m;
}


bool FindElementByPositionCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  BrowserCompute::pre_update_state(tc);
  _worker->queue_find_element_by_position(tc);
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

  add_hint(m, Message::kWrapType, GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::WrapType));
  add_hint(m, Message::kWrapType, GUITypes::HintKey::DescriptionHint, "The type of the elements to put into the set.");

  add_hint(m, Message::kTargetValues, GUITypes::HintKey::DescriptionHint, "The texts or image urls used to find elements.");
  add_hint(m, Message::kTargetValues, GUITypes::HintKey::ElementJSTypeHint, to_underlying(GUITypes::JSType::String));
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

  add_hint(m, Message::kWrapType, GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::WrapType));
  add_hint(m, Message::kWrapType, GUITypes::HintKey::DescriptionHint, "The element type to use when creating the set.");
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

  create_input(Message::kAngleInDegrees, c);
  create_input(Message::kWrapType, c);
}

const QJsonObject ShiftElementByTypeCompute::_hints = ShiftElementByTypeCompute::init_hints();
QJsonObject ShiftElementByTypeCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kAngleInDegrees, GUITypes::HintKey::DescriptionHint, "The direction in degrees in which to shift the element to. (0 is right, 90 is up, 180 is left, 270 is down)");

  add_hint(m, Message::kWrapType, GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::WrapType));
  add_hint(m, Message::kWrapType, GUITypes::HintKey::DescriptionHint, "The type of elements to shift to.");

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

    create_input(Message::kAngleInDegrees, c);
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

  add_hint(m, Message::kAngleInDegrees, GUITypes::HintKey::DescriptionHint, "The direction in degrees in which to shift the element to. (0 is right, 90 is up, 180 is left, 270 is down)");

  add_hint(m, Message::kWrapType, GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::WrapType));
  add_hint(m, Message::kWrapType, GUITypes::HintKey::DescriptionHint, "The type of elements to shift to.");

  add_hint(m, Message::kTargetValues, GUITypes::HintKey::DescriptionHint, "The texts or image urls used to find elements.");
  add_hint(m, Message::kTargetValues, GUITypes::HintKey::ElementJSTypeHint, to_underlying(GUITypes::JSType::String));
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

  add_hint(m, Message::kLocalMousePosition, GUITypes::HintKey::ElementJSTypeHint, to_underlying(GUITypes::JSType::Number));
  add_hint(m, Message::kLocalMousePosition, GUITypes::HintKey::DescriptionHint, "The position to perform our action at, relative to the element itself.");

  add_hint(m, Message::kMouseAction, GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::MouseActionType));
  add_hint(m, Message::kMouseAction, GUITypes::HintKey::DescriptionHint, "The type of mouse mouse action to perform.");
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

  add_hint(m, Message::kTextAction, GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::TextActionType));
  add_hint(m, Message::kTextAction, GUITypes::HintKey::DescriptionHint, "The type of text action to perform.");

  add_hint(m, Message::kText, GUITypes::HintKey::DescriptionHint, "The password to type.");
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

const QJsonObject PasswordActionCompute::_hints = PasswordActionCompute::init_hints();
QJsonObject PasswordActionCompute::init_hints() {
  QJsonObject m = TextActionCompute::init_hints();
  add_hint(m, Message::kText, GUITypes::HintKey::PasswordHint, true);
  return m;
}

void ElementActionCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 0;

    create_input(Message::kElementAction, c);
    create_input(Message::kScrollDirection, c);  // Only used when the element action is set to scroll.
  }
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "";

    create_input(Message::kOptionText, c); // Only used when the element action is set to choose an option from a drop down field.

    c.unconnected_value = "extracted_text";
    create_input(Message::kTextDataName, c); // Only used when the element action is set to get_text.
  }
}

const QJsonObject ElementActionCompute::_hints = ElementActionCompute::init_hints();
QJsonObject ElementActionCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kElementAction, GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::ElementActionType));
  add_hint(m, Message::kElementAction, GUITypes::HintKey::DescriptionHint, "The type of element action to perform.");

  add_hint(m, Message::kOptionText, GUITypes::HintKey::DescriptionHint, "The text to select from dropdowns.");

  add_hint(m, Message::kScrollDirection, GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::DirectionType));
  add_hint(m, Message::kScrollDirection, GUITypes::HintKey::DescriptionHint, "The direction to scroll.");

  add_hint(m, Message::kTextDataName, GUITypes::HintKey::DescriptionHint, "The name used to reference the extracted text data.");

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
