#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <components/computes/jsonutils.h>
#include <components/computes/inputnodecompute.h>

#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>
#include <guicomponents/comms/guitypes.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/filemodel.h>
#include <guicomponents/computes/browsercomputes.h>
#include <guicomponents/computes/taskqueuer.h>
#include <guicomponents/computes/messagesender.h>
#include <guicomponents/computes/downloadmanager.h>

#include <functional>

#include <QtCore/QDir>

namespace ngs {

//--------------------------------------------------------------------------------

BrowserCompute::BrowserCompute(Entity* entity, ComponentDID did)
    : Compute(entity, did),
      _queuer(this),
      _scheduler(this),
      _msg_sender(this),
      _browser_width(this),
      _browser_height(this),
      _release(this){
  get_dep_loader()->register_fixed_dep(_queuer, Path());
  get_dep_loader()->register_fixed_dep(_scheduler, Path());
  get_dep_loader()->register_fixed_dep(_msg_sender, Path());
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

QJsonObject BrowserCompute::get_params() const {
  QJsonObject params = _inputs->get_input_values();

  // Strip irrelevant properties from in.
  {
    QJsonObject in = params.value(Compute::kMainInputName).toObject();
    QJsonObject stripped_in;
    for (QJsonObject::iterator iter = in.begin(); iter != in.end(); iter++) {
      if ((iter.key().toStdString() == Message::kValue) ||
          (iter.key().toStdString() == Message::kElements) ||
          (iter.key().toStdString() == Message::kClusters)) {
        stripped_in.insert(iter.key(), iter.value());
      }
    }
    // Qt 5.8 seems to have a bug where if you try to insert a child object,
    // where the child object has had properties removed beforehand, it will crash here.
    // So we only add child objects which were created by adding in properties.
    params.insert(Compute::kMainInputName, stripped_in);
  }
  return params;
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
  Entity* release = group->get_child("release_on_exit");

  _browser_width = get_dep<InputNodeCompute>(width);
  _browser_height = get_dep<InputNodeCompute>(height);
  if (release) {
    _release = get_dep<InputNodeCompute>(release);
  }
}

void BrowserCompute::queue_start_update(TaskContext& tc) {
  internal();

  // Make sure the browser is open.
  _queuer->queue_open_browser(tc);

  // Make sure the browser is of the right size.
  {
    double width = _browser_width->get_output("out").toDouble();
    double height = _browser_height->get_output("out").toDouble();
    QJsonObject args;
    args.insert(Message::kWidth, width);
    args.insert(Message::kHeight, height);
    Message req(WebDriverRequestType::kSetBrowserSize, args);
    _queuer->queue_send_msg(tc, req);
  }

  // Make sure nothing is loading right now.
  // Note in general a page may start loading content at random times.
  // For examples ads may rotate and flip content.
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kWaitUntilLoaded));
}

void BrowserCompute::queue_on_results(TaskContext& tc) {
  auto f = [this]() {
    on_results(_queuer->get_last_results());
    _scheduler->run_next_task();
  };
  _scheduler->queue_task(tc, Task(f), "queuing on_response handler");
}

void BrowserCompute::queue_on_finished(TaskContext& tc) {
  auto f = [this](){
    on_finished(_queuer->get_last_results());
    _scheduler->run_next_task();
  };
  _scheduler->queue_task(tc, Task(f), "queuing on_finished handler");
}

void BrowserCompute::queue_end_update(TaskContext& tc) {
  internal();
  //_queuer->queue_scroll_element_into_view(tc);
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kUpdateElementHighlights));
  _queuer->queue_update_current_tab(tc);
  // Make sure everything is loaded before updating the frame offsets, otherwise the frame offsets won't distribute properly.
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kWaitUntilLoaded));
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kUpdateFrameOffsets));
  queue_on_finished(tc);
}

void BrowserCompute::on_results(const std::deque<QJsonObject>& last_results) {
  internal();

  // Be default we copy the value property from the chain state
  // and paste it into our output object which is just a copy
  // of our input object.

  // Initialize our main output value to be the main input value.
  QJsonObject out = _inputs->get_main_input_object();

  // Merge the last result into our output.
  JSONUtils::shallow_object_merge(out, last_results.back());

  // Set our main output.
  set_main_output(out);
}

void BrowserCompute::on_finished(const std::deque<QJsonObject>& last_results) {
  clean_finalize();
}

//--------------------------------------------------------------------------------

bool OpenBrowserCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  //pre_update_state(tc);
  _queuer->queue_open_browser(tc);
  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

bool CloseBrowserCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  queue_start_update(tc);
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
      _queuer->queue_send_msg(tc, Message(WebDriverRequestType::kCloseBrowser), false);
    }
  }
  //queue_end_update(tc);
  return true;
}

bool ReleaseBrowserCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  queue_start_update(tc);
  {
    _queuer->queue_send_msg(tc, Message(ChromeRequestType::kUnblockEvents));
    _queuer->queue_send_msg(tc, Message(ChromeRequestType::kReleaseBrowser));
    _queuer->queue_send_msg(tc, Message(WebDriverRequestType::kReleaseBrowser));
    {
      auto f = [this](){
        _msg_sender->clear_web_socket();
        _scheduler->run_next_task();
      };
      _scheduler->queue_task(tc, Task(f), "clear web socket");
    }
    _queuer->queue_open_browser(tc);
  }
  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

bool IsBrowserOpenCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  queue_start_update(tc);
  {
    _queuer->queue_send_msg(tc, Message(WebDriverRequestType::kIsBrowserOpen));
  }
  queue_on_results(tc);
  queue_end_update(tc);
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
  queue_start_update(tc);

  QJsonObject params = get_params();
  Message req(WebDriverRequestType::kSetBrowserSize, params);
  _queuer->queue_send_msg(tc, req);

  queue_on_results(tc);
  queue_end_update(tc);
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
  queue_start_update(tc);
  {
    _queuer->queue_send_msg(tc, Message(WebDriverRequestType::kGetBrowserSize));
  }
  queue_on_results(tc);
  queue_end_update(tc);
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

bool GetTitleCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  queue_start_update(tc);
  {
    _queuer->queue_send_msg(tc, Message(ChromeRequestType::kGetActiveTabTitle));
  }
  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

void CloseTabCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);
}

const QJsonObject CloseTabCompute::_hints = CloseTabCompute::init_hints();
QJsonObject CloseTabCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);
  return m;
}

bool CloseTabCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  queue_start_update(tc);
  {
    _queuer->queue_send_msg(tc, Message(WebDriverRequestType::kDestroyCurrentTab));
  }
  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

void OpenTabCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "https://www.google.com";
    create_input(Message::kURL, c);
  }
}

const QJsonObject OpenTabCompute::_hints = OpenTabCompute::init_hints();
QJsonObject OpenTabCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);
  add_hint(m, Message::kURL, GUITypes::HintKey::DescriptionHint, "The url to open in the new tab.");
  return m;
}

bool OpenTabCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  queue_start_update(tc);
  {
    QJsonObject params = get_params();
    _queuer->queue_send_msg(tc, Message(ChromeRequestType::kOpenTab, params));
  }
  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

bool AcceptSaveDialogCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  queue_start_update(tc);
  _queuer->queue_send_msg(tc, Message(PlatformRequestType::kAcceptSaveDialog));
  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

DownloadVideosCompute::DownloadVideosCompute(Entity* entity):
    BrowserCompute(entity, kDID()),
    _download_manager(this) {
  get_dep_loader()->register_fixed_dep(_download_manager, Path());
}

void DownloadVideosCompute::create_inputs_outputs(const EntityConfig& config) {
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

const QJsonObject DownloadVideosCompute::_hints = DownloadVideosCompute::init_hints();
QJsonObject DownloadVideosCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);
  add_hint(m, Message::kDirectory, GUITypes::HintKey::DescriptionHint, "The relative or absolute directory to save the video to. Leave empty to use the default directory.");
  add_hint(m, Message::kMaxWidth, GUITypes::HintKey::DescriptionHint, "The maximum video width (in pixels) to download. Zero will download the largest.");
  add_hint(m, Message::kMaxHeight, GUITypes::HintKey::DescriptionHint, "The maximum video height (in pixels) to download. Zero will download the largest.");
  add_hint(m, Message::kMaxFilesize, GUITypes::HintKey::DescriptionHint, "The maximum video filesize (in megabytes) to download. Zero will download the largest.");
  return m;
}

bool DownloadVideosCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  queue_start_update(tc);

  QJsonObject inputs = get_params();
  QJsonArray elements = inputs.value(Compute::kMainInputName).toObject().value(Message::kElements).toArray();

  if (elements.size() > 0) {
    std::cerr << "Downloading from element href links.\n";

    auto download_from_links = [this, inputs, elements](){
      for (int i=0; i<elements.size(); ++i) {
        QJsonObject args;
        QJsonObject elem = elements[i].toObject();

        QString url = elem.value(Message::kHREF).toString();
        QString dir = inputs.value(Message::kDirectory).toString();
        int max_width = inputs.value(Message::kMaxWidth).toInt();
        int max_height = inputs.value(Message::kMaxHeight).toInt();
        int max_filesize = inputs.value(Message::kMaxFilesize).toInt();
        _download_manager->download(url, dir, max_width, max_height, max_filesize);
      }
      _scheduler->run_next_task();
    };
    _scheduler->queue_task(tc, Task(download_from_links), "download_from_links");
  } else {
    std::cerr << "Downloading from page.\n";
    Message req(WebDriverRequestType::kGetCurrentURL);
    _queuer->queue_send_msg(tc, req);

    auto download_from_page = [this, inputs](){
      QJsonObject args;
      const std::deque<QJsonObject> &results = _queuer->get_last_results();

      QString url = results.back().value(Message::kValue).toString();
      QString dir = inputs.value(Message::kDirectory).toString();
      int max_width = inputs.value(Message::kMaxWidth).toInt();
      int max_height = inputs.value(Message::kMaxHeight).toInt();
      int max_filesize = inputs.value(Message::kMaxFilesize).toInt();
      _download_manager->download(url, dir, max_width, max_height, max_filesize);

      _scheduler->run_next_task();
    };

    _scheduler->queue_task(tc, Task(download_from_page), "download_from_page");
  }

  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

DownloadImagesCompute::DownloadImagesCompute(Entity* entity):
    BrowserCompute(entity, kDID()),
    _file_model(this) {
  get_dep_loader()->register_fixed_dep(_file_model, Path());
}

void DownloadImagesCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BrowserCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "";
    create_input(Message::kDirectory, c);
  }
}

const QJsonObject DownloadImagesCompute::_hints = DownloadImagesCompute::init_hints();
QJsonObject DownloadImagesCompute::init_hints() {
  QJsonObject m;
  BrowserCompute::init_hints(m);
  add_hint(m, Message::kDirectory, GUITypes::HintKey::DescriptionHint, "The relative or absolute directory to save the image to. Leave empty to use the default directory.");
  return m;
}

bool DownloadImagesCompute::update_state(){
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  queue_start_update(tc);

  // Tweak the download directory on c++ because we don't access to the default download dir on the java side yet.
  QString path = _inputs->get_input_value(Message::kDirectory).toString();
  QDir dir(path);
  if (dir.isRelative()) {
    path = _file_model->get_default_download_dir() + QDir::separator() + dir.path();
  } else {
    path = dir.path();
  }

  QJsonObject params = get_params();
  // Overwrite the directory param.
  params.insert(Message::kDirectory, path);
  Message req(WebDriverRequestType::kDownloadImages, params);
  _queuer->queue_send_msg(tc, req, true, "download images");

  queue_on_results(tc);
  queue_end_update(tc);
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
  queue_start_update(tc);

  QJsonObject params = get_params();
  Message req(WebDriverRequestType::kNavigateTo, params);
  _queuer->queue_send_msg(tc, req);

  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kWaitUntilLoaded));
  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

bool NavigateBackCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  queue_start_update(tc);

  Message req(WebDriverRequestType::kNavigateBack);
  _queuer->queue_send_msg(tc, req);

  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

bool NavigateForwardCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  queue_start_update(tc);

  Message req(WebDriverRequestType::kNavigateForward);
  _queuer->queue_send_msg(tc, req);

  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

bool NavigateRefreshCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  queue_start_update(tc);

  Message req(WebDriverRequestType::kNavigateRefresh);
  _queuer->queue_send_msg(tc, req);

  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

bool GetURLCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  queue_start_update(tc);

  Message req(WebDriverRequestType::kGetCurrentURL);
  _queuer->queue_send_msg(tc, req);

  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}


bool GetAllElementsCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  queue_start_update(tc);
  {
    _queuer->queue_send_msg(tc, Message(ChromeRequestType::kGetAllElements));
  }
  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

bool HighlightElementsCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  queue_start_update(tc);
  {
    QJsonObject params = get_params();
    Message req(ChromeRequestType::kHighlightElements, params);
    _queuer->queue_send_msg(tc, req);
  }
  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

bool ClearElementHighlightsCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  queue_start_update(tc);
  {
    QJsonObject params = get_params();
    Message req(ChromeRequestType::kClearElementHighlights, params);
    _queuer->queue_send_msg(tc, req);
  }
  queue_on_results(tc);
  queue_end_update(tc);
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
  queue_start_update(tc);

  MouseActionType type = static_cast<MouseActionType>(_inputs->get_input_value(Message::kMouseAction).toInt());
  if (type != MouseActionType::kMouseOver) {
    _queuer->queue_send_msg(tc, Message(ChromeRequestType::kUnblockEvents));
    queue_perform_hover(tc);
    // After we're done interacting with the page, block events on the page.
    _queuer->queue_send_msg(tc, Message(ChromeRequestType::kBlockEvents));

    // Our hover may have triggered asynchronous content loading in the page, so we wait for the page to be ready.
    _queuer->queue_send_msg(tc, Message(ChromeRequestType::kWaitUntilLoaded));

    // Force wait so that the webpage can react to the mouse hover. Note this is real and makes the mouse click work 100% of the time.
    // For example this allows the page to display an animated video under mouse hover.
    _queuer->queue_wait(tc);
    _queuer->queue_send_msg(tc, Message(ChromeRequestType::kUnblockEvents));
    queue_perform_action(tc);
  } else {
    _queuer->queue_send_msg(tc, Message(ChromeRequestType::kUnblockEvents));
    queue_perform_action(tc);
    _queuer->queue_send_msg(tc, Message(ChromeRequestType::kBlockEvents));
    _queuer->queue_send_msg(tc, Message(ChromeRequestType::kWaitUntilLoaded));
  }

  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

void MouseActionCompute::queue_perform_hover(TaskContext& tc) {
  QJsonObject params = get_params();
  params.insert(Message::kMouseAction, to_underlying(MouseActionType::kMouseOver));
  Message req(WebDriverRequestType::kPerformMouseAction, params);
  _queuer->queue_send_msg(tc, req);
}

void MouseActionCompute::queue_perform_action(TaskContext& tc) {
  QJsonObject params = get_params();
  Message req(WebDriverRequestType::kPerformMouseAction, params);
  _queuer->queue_send_msg(tc, req);
}

void MouseActionCompute::queue_end_update(TaskContext& tc) {
  internal();

  // After we're done interacting with the page, block events on the page.
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kBlockEvents));
  // Our actions may have triggered asynchronous content loading in the page, so we wait for the page to be ready.
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kWaitUntilLoaded));
  // Our actions may have moved elements arounds, so we update our overlays.
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kUpdateElementHighlights));
  // Force wait so that the webpage can react to the mouse action. Note this is real and makes the mouse click work 100% of the time.
  _queuer->queue_wait(tc);

  // Do the base logic.
  BrowserCompute::queue_end_update(tc);
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
  queue_start_update(tc);

  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kUnblockEvents));

  QJsonObject params = get_params();
  Message req(WebDriverRequestType::kPerformTextAction, params);
  _queuer->queue_send_msg(tc, req);

  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

void TextActionCompute::queue_end_update(TaskContext& tc) {
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kBlockEvents));
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kWaitUntilLoaded));
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kUpdateElementHighlights));

  // Do the base logic.
  BrowserCompute::queue_end_update(tc);
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
  queue_start_update(tc);

  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kUnblockEvents));

  QJsonObject params = get_params();
  Message req(WebDriverRequestType::kPerformElementAction, params);
  _queuer->queue_send_msg(tc, req);

  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

void ElementActionCompute::queue_end_update(TaskContext& tc) {
  internal();

  // Special
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kBlockEvents));
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kWaitUntilLoaded));

  // Do the base logic.
  BrowserCompute::queue_end_update(tc);
}

// ----------------------------------------------------------------------------------

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
  queue_start_update(tc);

  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kUnblockEvents));

  QJsonObject params = get_params();
  Message req(ChromeRequestType::kScrollElement, params);
  _queuer->queue_send_msg(tc, req);

  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

void ElementScrollCompute::queue_end_update(TaskContext& tc) {
  internal();

  // Special
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kBlockEvents));
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kWaitUntilLoaded));

  // Do most of the logic from the base class.
  //_queuer->queue_scroll_element_into_view(tc);
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kUpdateElementHighlights));
  _queuer->queue_update_current_tab(tc);
  // Make sure everything is loaded before updating the frame offsets, otherwise the frame offsets won't distribute properly.
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kWaitUntilLoaded));
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kUpdateFrameOffsets));
  queue_on_finished(tc);
}

// ----------------------------------------------------------------------------------

bool ElementScrollIntoViewCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  TaskContext tc(_scheduler);
  queue_start_update(tc);

  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kUnblockEvents));

  QJsonObject params = get_params();
  Message req(ChromeRequestType::kScrollElementIntoView, params);
  _queuer->queue_send_msg(tc, req);

  queue_on_results(tc);
  queue_end_update(tc);
  return false;
}

void ElementScrollIntoViewCompute::queue_end_update(TaskContext& tc) {
  internal();

  // Special
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kBlockEvents));
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kWaitUntilLoaded));

  // Do most of the logic from the base class.
  //_queuer->queue_scroll_element_into_view(tc);
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kUpdateElementHighlights));
  _queuer->queue_update_current_tab(tc);
  // Make sure everything is loaded before updating the frame offsets, otherwise the frame offsets won't distribute properly.
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kWaitUntilLoaded));
  _queuer->queue_send_msg(tc, Message(ChromeRequestType::kUpdateFrameOffsets));
  queue_on_finished(tc);
}


}
