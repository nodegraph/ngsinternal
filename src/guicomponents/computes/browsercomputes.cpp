#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <components/computes/jsonutils.h>
#include <components/computes/inputnodecompute.h>

#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>
#include <guicomponents/comms/guitypes.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/computes/browsercomputes.h>
#include <guicomponents/computes/taskqueuer.h>

#include <functional>

namespace ngs {

//--------------------------------------------------------------------------------

BrowserCompute::BrowserCompute(Entity* entity, ComponentDID did)
    : Compute(entity, did),
      _queuer(this),
      _scheduler(this),
      _browser_width(this),
      _browser_height(this) {
  get_dep_loader()->register_fixed_dep(_queuer, Path());
  get_dep_loader()->register_fixed_dep(_scheduler, Path());

  _response_value_properties_to_merge.push_back(Message::kValue);
}

BrowserCompute::~BrowserCompute() {
}

void BrowserCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  create_main_input(config);
  create_main_output(config);
}

void BrowserCompute::init_hints(QJsonObject& m) {
  add_main_input_hint(m);
}

void BrowserCompute::dump_map(const QJsonObject& inputs) const {
  for(QJsonObject::const_iterator iter = inputs.begin(); iter != inputs.end(); ++iter) {
    std::cerr << iter.key().toStdString() << " : " << iter.value().toString().toStdString() << "\n";
  }
}

void BrowserCompute::update_wires() {
  // This caches the dep, and will only dirty this component when it changes.
  find_dep_nodes();
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

void BrowserCompute::find_dep_nodes() {
  Entity* group = find_group_context();

  Entity* width = group->get_child("browser_width");
  Entity* height = group->get_child("browser_height");
  _browser_width = get_dep<InputNodeCompute>(width);
  _browser_height = get_dep<InputNodeCompute>(height);
}

void BrowserCompute::pre_update_state(TaskContext& tc) {
  internal();

  // Make sure the browser is open.
  _queuer->queue_open_browser(tc);

  // Make sure the browser is of the right size.
  double width = _browser_width->get_output("out").toDouble();
  double height = _browser_height->get_output("out").toDouble();
  QJsonObject size_obj;
  size_obj.insert(Message::kWidth, width);
  size_obj.insert(Message::kHeight, height);
  _queuer->queue_overwrite_chain_state(tc, size_obj);
  _queuer->queue_resize_browser(tc);

  // Overwrite the chain state with our input values.
  QJsonObject inputs = _inputs->get_input_values();
  _queuer->queue_overwrite_chain_state(tc, inputs);

  // Make sure nothing is loading right now.
  // Note in general a page may start loading content at random times.
  // For examples ads may rotate and flip content.
  _queuer->queue_wait_until_loaded(tc);
}

void BrowserCompute::handle_response(TaskContext& tc) {
  std::function<void(const QJsonObject&)> callback = std::bind(&BrowserCompute::on_response,this,std::placeholders::_1);
  _queuer->queue_receive_chain_state(tc, callback);
}

void BrowserCompute::handle_finished(TaskContext& tc) {
  std::function<void(const QJsonObject&)> callback = std::bind(&BrowserCompute::on_finished,this,std::placeholders::_1);
  _queuer->queue_receive_chain_state(tc, callback);
}

void BrowserCompute::post_update_state(TaskContext& tc) {
  internal();
  _queuer->queue_scroll_element_into_view(tc);
  _queuer->queue_update_element(tc);
  _queuer->queue_update_current_tab(tc);
  _queuer->queue_wait_until_loaded(tc); // Make sure everything is loaded before updating the frame offsets, otherwise the frame offsets won't distribute properly.
  _queuer->queue_update_frame_offsets(tc);
  handle_finished(tc);
}

void BrowserCompute::on_response(const QJsonObject& chain_state) {
  internal();

  // Be default we copy the value property from the chain state
  // and paste it into our output object which is just a copy
  // of our input object.

  // Initialize our main output value to be the main input value.
  QJsonObject obj = _inputs->get_main_input_object();

  // Copy properties from the response value into our output value.
  for (const std::string& prop: _response_value_properties_to_merge) {
    QJsonValue value = chain_state.value(prop.c_str());
    obj.insert(prop.c_str(), value);
  }

  // Set our main output.
  set_main_output(obj);
}

void BrowserCompute::on_finished(const QJsonObject& chain_state) {
  clean_finalize();
}

//--------------------------------------------------------------------------------

bool OpenBrowserCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  //pre_update_state(tc);
  _queuer->queue_open_browser(tc);
  handle_response(tc);
  post_update_state(tc);
  return false;
}

bool CloseBrowserCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  pre_update_state(tc);
  {
    // Determine whether we are nested recusively inside muliple browser groups.
    // We do this by counting the number surrounding browser groups.
    Entity* parent = our_entity()->get_parent();
    size_t count = 0;
    while(parent) {
      if (parent->get_did() == EntityDID::kBrowserGroupNodeEntity) {
        count++;
      }
      parent = parent->get_parent();
    }
    std::cerr << "NNNNNNNNNNNNNNNNNNNNNNNNNNN num browser groups: " << count << "\n";
    // If we're not nested, then we can close the browser.
    if (count <= 1) {
      std::cerr << "closing browser!!!!!!!!!!!!\n";
      _queuer->queue_close_browser(tc);
    }
  }
  //post_update_state(tc);
  return true;
}

bool ReleaseBrowserCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  pre_update_state(tc);
  _queuer->queue_release_browser(tc);
  _queuer->queue_open_browser(tc);
  handle_response(tc);
  post_update_state(tc);
  return false;
}

bool IsBrowserOpenCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  pre_update_state(tc);
  _queuer->queue_is_browser_open(tc);
  handle_response(tc);
  post_update_state(tc);
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
  pre_update_state(tc);
  _queuer->queue_resize_browser(tc);
  handle_response(tc);
  post_update_state(tc);
  return false;
}

void GetBrowserSizeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);
}

const QJsonObject GetBrowserSizeCompute::_hints = GetBrowserSizeCompute::init_hints();
QJsonObject GetBrowserSizeCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);
  return m;
}

void GetBrowserSizeCompute::on_response(const QJsonObject& chain_state) {
  internal();
  clean_finalize();

  // Be default we copy the value property from the chain state
  // and paste it into our output object which is just a copy
  // of our input object.

  // Our main output is intially a copy of the main input object.
  QJsonObject obj = _inputs->get_main_input_object();

  // Get the value property from the chain state.
  QJsonValue width = chain_state.value(Message::kWidth);
  QJsonValue height = chain_state.value(Message::kHeight);
  QJsonObject result;
  result.insert("width", width);
  result.insert("height", height);

  // Copy the value property.
  obj.insert(Message::kValue, result);

  // Set our main output.
  set_main_output(obj);
}

bool GetBrowserSizeCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  pre_update_state(tc);
  _queuer->queue_get_browser_size(tc);
  handle_response(tc);
  post_update_state(tc);
  return false;
}

//void GetActiveTabTitleCompute::receive_chain_state(const QJsonObject& chain_state) {
//  internal();
//  clean_finalize();
//
//  // Grab the title from the chain state.
//  QString title = chain_state[Message::kTitle].toString();
//  // Grab the incoming object.
//  QJsonObject obj = _inputs->get_main_input_object();
//  obj.insert(Message::kValue, title);
//  set_main_output(obj);
//}

bool GetActiveTabTitleCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  pre_update_state(tc);
  _queuer->queue_get_active_tab_title(tc);
  handle_response(tc);
  post_update_state(tc);
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
  pre_update_state(tc);
  _queuer->queue_destroy_current_tab(tc);
  handle_response(tc);
  post_update_state(tc);
  return false;
}

bool OpenTabCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  pre_update_state(tc);
  _queuer->queue_open_tab(tc);
  handle_response(tc);
  post_update_state(tc);
  return false;
}

bool AcceptSaveDialogCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  pre_update_state(tc);
  _queuer->queue_accept_save_dialog(tc);
  handle_response(tc);
  post_update_state(tc);
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
    c.unconnected_value = false;
    create_input(Message::kUseCurrentElement, c);
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
  add_hint(m, Message::kUseCurrentElement, GUITypes::HintKey::DescriptionHint, "Whether to download the video from the current element or the page.");
  add_hint(m, Message::kMaxWidth, GUITypes::HintKey::DescriptionHint, "The maximum video width (in pixels) to download. Zero will download the largest.");
  add_hint(m, Message::kMaxHeight, GUITypes::HintKey::DescriptionHint, "The maximum video height (in pixels) to download. Zero will download the largest.");
  add_hint(m, Message::kMaxFilesize, GUITypes::HintKey::DescriptionHint, "The maximum video filesize (in megabytes) to download. Zero will download the largest.");
  return m;
}

bool DownloadVideoCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  pre_update_state(tc);

  QJsonObject in = _inputs->get_input_object("in");
  QJsonArray elements = in.value("elements").toArray();

  if (elements.size() > 0) {
    for (int i=0; i<elements.size(); ++i) {
      QJsonObject args;
      args.insert(Message::kHREF, elements.at(i).toObject().value(Message::kHREF));
      _queuer->queue_merge_chain_state(tc, args);
      _queuer->queue_download_video(tc);
    }
  } else {
    // Set the url chain property to the current url.
    _queuer->queue_get_current_url(tc);
    _queuer->queue_copy_chain_property(tc, Message::kValue, Message::kURL);
    _queuer->queue_download_video(tc);
  }

  handle_response(tc);
  post_update_state(tc);
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
  pre_update_state(tc);
  _queuer->queue_navigate_to(tc);
  _queuer->queue_wait_until_loaded(tc);
  handle_response(tc);
  post_update_state(tc);
  return false;
}

bool NavigateBackCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  pre_update_state(tc);
  _queuer->queue_navigate_back(tc);
  handle_response(tc);
  post_update_state(tc);
  return false;
}

bool NavigateForwardCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  pre_update_state(tc);
  _queuer->queue_navigate_forward(tc);
  handle_response(tc);
  post_update_state(tc);
  return false;
}

bool NavigateRefreshCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  pre_update_state(tc);
  _queuer->queue_navigate_refresh(tc);
  handle_response(tc);
  post_update_state(tc);
  return false;
}

bool GetCurrentURLCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  pre_update_state(tc);
  _queuer->queue_get_current_url(tc);
  handle_response(tc);
  post_update_state(tc);
  return false;
}


bool GetAllElementsCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  pre_update_state(tc);
  _queuer->queue_get_all_elements(tc);
  handle_response(tc);
  post_update_state(tc);
  return false;
}

void HighlightElementsCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = QJsonObject();
    create_input(Message::kElementInfo, c);
  }
}

const QJsonObject HighlightElementsCompute::_hints = HighlightElementsCompute::init_hints();
QJsonObject HighlightElementsCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);
  add_hint(m, Message::kWrapType, GUITypes::HintKey::DescriptionHint, "Object containing values identifying the element to make current.");
  return m;
}


bool HighlightElementsCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  pre_update_state(tc);
  _queuer->queue_highlight_elements(tc);
  handle_response(tc);
  post_update_state(tc);
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
  pre_update_state(tc);
  _queuer->queue_perform_mouse_action(tc);
  handle_response(tc);
  post_update_state(tc);
  return false;
}

void MouseActionCompute::post_update_state(TaskContext& tc) {
  internal();
  _queuer->queue_block_events(tc); // After we're done interacting with the page, block events on the page.
  _queuer->queue_wait_until_loaded(tc); // Our actions may have triggered asynchronous content loading in the page, so we wait for the page to be ready.

  _queuer->queue_update_element(tc); // Our actions may have moved elements arounds, so we update our overlays.

  // Force wait so that the webpage can react to the mouse action. Note this is real and makes the mouse click work 100% of the time.
  _queuer-> queue_wait(tc);

  // Do the base logic.
  BrowserCompute::post_update_state(tc);
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
  pre_update_state(tc);
  _queuer->queue_perform_text_action(tc);
  handle_response(tc);
  post_update_state(tc);
  return false;
}

void TextActionCompute::post_update_state(TaskContext& tc) {
  _queuer->queue_block_events(tc);
  _queuer->queue_wait_until_loaded(tc);
  _queuer->queue_update_element(tc);

  // Do the base logic.
  BrowserCompute::post_update_state(tc);
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
  }
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "";

    create_input(Message::kOptionText, c); // Only used when the element action is set to choose an option from a drop down field.
  }
}

const QJsonObject ElementActionCompute::_hints = ElementActionCompute::init_hints();
QJsonObject ElementActionCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kElementAction, GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::ElementActionType));
  add_hint(m, Message::kElementAction, GUITypes::HintKey::DescriptionHint, "The type of element action to perform.");

  add_hint(m, Message::kOptionText, GUITypes::HintKey::DescriptionHint, "The text to select from dropdowns.");

  return m;
}

bool ElementActionCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  pre_update_state(tc);
  _queuer->queue_perform_element_action(tc);
  handle_response(tc);
  post_update_state(tc);
  return false;
}

void ElementActionCompute::post_update_state(TaskContext& tc) {
  internal();

  // Special
  _queuer->queue_block_events(tc);
  _queuer->queue_wait_until_loaded(tc);

  // Do the base logic.
  BrowserCompute::post_update_state(tc);
}

void ElementScrollCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 0;
    create_input(Message::kScrollDirection, c);  // Only used when the element action is set to scroll.
  }
}

const QJsonObject ElementScrollCompute::_hints = ElementScrollCompute::init_hints();
QJsonObject ElementScrollCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);

  add_hint(m, Message::kScrollDirection, GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::DirectionType));
  add_hint(m, Message::kScrollDirection, GUITypes::HintKey::DescriptionHint, "The direction in which to scroll.");

  return m;
}

bool ElementScrollCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  pre_update_state(tc);
  _queuer->queue_perform_element_scroll(tc);
  handle_response(tc);
  post_update_state(tc);
  return false;
}

void ElementScrollCompute::post_update_state(TaskContext& tc) {
  internal();

  // Special
  _queuer->queue_block_events(tc);
  _queuer->queue_wait_until_loaded(tc);

  // Do most of the logic from the base class.
  //_queuer->queue_scroll_element_into_view(tc);
  _queuer->queue_update_element(tc);
  _queuer->queue_update_current_tab(tc);
  _queuer->queue_wait_until_loaded(tc); // Make sure everything is loaded before updating the frame offsets, otherwise the frame offsets won't distribute properly.
  _queuer->queue_update_frame_offsets(tc);
  handle_finished(tc);
}

}
