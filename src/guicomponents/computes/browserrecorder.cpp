#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>

#include <components/computes/compute.h>
#include <guicomponents/computes/browserrecorder.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/filemodel.h>
#include <guicomponents/computes/taskqueuer.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <QtCore/QUrl>

namespace ngs {

QUrl get_proper_url(const QString& input) {
  if (input.isEmpty()) {
    return QUrl::fromUserInput("about:blank");
  }
  const QUrl result = QUrl::fromUserInput(input);
  return result.isValid() ? result : QUrl::fromUserInput("about:blank");
}

BrowserRecorder::BrowserRecorder(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _queuer(this),
      _scheduler(this),
      _model(this),
      _manipulator(this) {
  get_dep_loader()->register_fixed_dep(_queuer, Path());
  get_dep_loader()->register_fixed_dep(_scheduler, Path());
  get_dep_loader()->register_fixed_dep(_model, Path());
  get_dep_loader()->register_fixed_dep(_manipulator, Path());
}

BrowserRecorder::~BrowserRecorder() {
}

void BrowserRecorder::initialize_wires() {
  Component::initialize_wires();
}

// -----------------------------------------------------------------
// // Record Browser Actions.
// -----------------------------------------------------------------

#define start() \
  /* Return if the scheduler is busy. */\
  if (_scheduler->is_busy()) {\
    emit web_action_ignored();\
    return;\
  }\

#define finish()

void BrowserRecorder::record_open_browser() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kOpenBrowserCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_close_browser() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kCloseBrowserCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_release_browser() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kReleaseBrowserCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_is_browser_open() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kIsBrowserOpenCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_resize_browser() {
  start();
  QJsonObject dims;
  dims.insert(Message::kWidth, 1024);
  dims.insert(Message::kHeight, 1150);

  QJsonObject params;
  params.insert(Message::kDimensions, dims);

  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kResizeBrowserCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_get_browser_size() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kGetBrowserSizeCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_get_title() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kGetTitleCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_destroy_current_tab() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kCloseTabCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_open_tab() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kOpenTabCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_accept_save_dialog() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kAcceptSaveDialogCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_download_video() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kDownloadVideosCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_download_image() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kDownloadImagesCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

// -----------------------------------------------------------------
// Record Navigate Actions.
// -----------------------------------------------------------------

void BrowserRecorder::record_navigate_to(const QString& url) {
  start()
  QString decorated_url = get_proper_url(url).toString();

  QJsonObject params;
  params.insert(Message::kURL, decorated_url);

  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kNavigateToCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_navigate_back() {
  start()
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kNavigateBackCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_navigate_forward() {
  start()
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kNavigateForwardCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_navigate_refresh() {
  start()
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kNavigateRefreshCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_get_url() {
  start()
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kGetURLCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

// -----------------------------------------------------------------
// Record Mouse Actions.
// -----------------------------------------------------------------

void BrowserRecorder::record_click() {
  start();
  QJsonObject params;
  {
    const QJsonObject info = _queuer->get_last_click_info();
    params.insert(Message::kMouseAction, info.value(Message::kLocalMousePosition));
    params.insert(Message::kMouseAction, to_underlying(MouseActionType::kSendClick));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kMouseActionCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_ctrl_click() {
  start();
  QJsonObject params;
  {
    const QJsonObject info = _queuer->get_last_click_info();
    params.insert(Message::kMouseAction, info.value(Message::kLocalMousePosition));
    params.insert(Message::kMouseAction, to_underlying(MouseActionType::kSendCtrlClick));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kMouseActionCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_mouse_over() {
  start();
  QJsonObject params;
  {
    const QJsonObject info = _queuer->get_last_click_info();
    params.insert(Message::kMouseAction, info.value(Message::kLocalMousePosition));
    params.insert(Message::kMouseAction, to_underlying(MouseActionType::kMouseOver));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kMouseActionCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

// -----------------------------------------------------------------
// Record Text Actions.
// -----------------------------------------------------------------

void BrowserRecorder::record_type_text(const QString& text) {
  start();
  QJsonObject params;
  {
    params.insert(Message::kText, text);
    params.insert(Message::kTextAction, to_underlying(TextActionType::kSendText));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kTextActionCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_type_enter() {
  start();
  QJsonObject params;
  {
    params.insert(Message::kTextAction, to_underlying(TextActionType::kSendEnter));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kTextActionCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_type_password(const QString& text) {
  start();
  QJsonObject params;
  {
    params.insert(Message::kText, text);
    params.insert(Message::kTextAction, to_underlying(TextActionType::kSendText));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kPasswordActionCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

// -----------------------------------------------------------------
// Record Element Actions.
// -----------------------------------------------------------------

void BrowserRecorder::record_select_from_dropdown(const QString& option_text) {
  start();
  QJsonObject params;
  {
    params.insert(Message::kElementAction, to_underlying(ElementActionType::kChooseOption));
    params.insert(Message::kOptionText, option_text);
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kElementActionCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

// -----------------------------------------------------------------
// Record Element Scroll Actions.
// -----------------------------------------------------------------

void BrowserRecorder::record_scroll_down() {
  start();
  QJsonObject params;
  {
    params.insert(Message::kScrollDirection, to_underlying(DirectionType::kDown));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kElementScrollCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_scroll_up() {
  start();
  QJsonObject params;
  {
    params.insert(Message::kScrollDirection, to_underlying(DirectionType::kUp));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kElementScrollCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_scroll_right() {
  start();
  QJsonObject params;
  {
    params.insert(Message::kScrollDirection, to_underlying(DirectionType::kRight));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kElementScrollCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_scroll_left() {
  start();
  QJsonObject params;
  {
    params.insert(Message::kScrollDirection, to_underlying(DirectionType::kLeft));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kElementScrollCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

// -----------------------------------------------------------------
// Set Element.
// -----------------------------------------------------------------
void BrowserRecorder::record_get_all_elements() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kGetAllElementsCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}













void BrowserRecorder::record_isolate_by_type() {
  start();
  QJsonObject params;
  {
    // Initial values for the type.
    ElementType type = ElementType::kInput;

    // Get the image and text value from the last click info.
    const QJsonObject info = _queuer->get_last_click_info();
    QString image = info.value("image").toString();
    QString text = info.value("text").toString();
    QString tag_name = info.value("tag_name").toString();

    // If either the image or text values are non empty then we can easily predict
    // the desired parameter values for the node.
    if (!image.isEmpty()) {
      type = ElementType::kImage;
    } else if (!text.isEmpty()) {
      type = ElementType::kText;
    } else if (tag_name == "input") {
      type = ElementType::kInput;
    } else if (tag_name == "select") {
      type = ElementType::kSelect;
    }
    params.insert(Message::kElementType, to_underlying(type));
    params.insert(Message::kTargetValue, "");
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kIsolateByTypeAndValueNodeCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_isolate_by_value() {
  start();
  QJsonObject params;
  {
    // Initial values for type and value.
    ElementType type = ElementType::kInput;
    QString value = "";

    // Get the image and text value from the last click info.
    const QJsonObject info = _queuer->get_last_click_info();
    QString image = info.value("image").toString();
    QString text = info.value("text").toString();
    QString tag_name = info.value("tag_name").toString();

    // If either the image or text values are non empty then we can easily predict
    // the desired parameter values for the node.
    if (!image.isEmpty()) {
      type = ElementType::kImage;
      value = image;
    } else if (!text.isEmpty()) {
      type = ElementType::kText;
      value = text;
    } else if (tag_name == "input") {
      type = ElementType::kInput;
      value = "";
    } else if (tag_name == "select") {
      type = ElementType::kSelect;
      value = "";
    }
    params.insert(Message::kElementType, to_underlying(type));
    params.insert(Message::kTargetValue, value);
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kIsolateByTypeAndValueNodeCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_isolate_by_position() {
  start();
  QJsonObject params;
  {
    const QJsonObject info = _queuer->get_last_click_info();
    params.insert(Message::kGlobalMousePosition, info.value(Message::kGlobalMousePosition));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kIsolateByPositionNodeCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_isolate_by_dimensions() {
  start();
  QJsonObject params;
  {
    const QJsonObject info = _queuer->get_last_click_info();
    double left = info.value("box").toObject().value("left").toDouble();
    double right = info.value("box").toObject().value("right").toDouble();
    double top = info.value("box").toObject().value("top").toDouble();
    double bottom = info.value("box").toObject().value("bottom").toDouble();

    double width = right - left;
    double height = bottom - top;

    params.insert(Message::kWidth, width);
    params.insert(Message::kHeight, height);
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kIsolateByDimensionsNodeCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_isolate_by_viewport() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kIsolateByViewportNodeCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_sort_by_distance_to_anchors() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kSortByDistanceToAnchorsNodeCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_isolate_leftmost() {
  start();
  QJsonObject params;
  {
    params.insert(Message::kDirection, to_underlying(DirectionType::kLeft));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kIsolateByBoundaryNodeCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_isolate_rightmost() {
  start();
  QJsonObject params;
  {
    params.insert(Message::kDirection, to_underlying(DirectionType::kRight));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kIsolateByBoundaryNodeCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_isolate_topmost() {
  start();
  QJsonObject params;
  {
    params.insert(Message::kDirection, to_underlying(DirectionType::kUp));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kIsolateByBoundaryNodeCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_isolate_bottommost() {
  start();
  QJsonObject params;
  {
    params.insert(Message::kDirection, to_underlying(DirectionType::kDown));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kIsolateByBoundaryNodeCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_isolate_on_left() {
  start();
  QJsonObject params;
  {
    params.insert(Message::kDirection, to_underlying(DirectionType::kLeft));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kIsolateToOneSideNodeCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_isolate_on_right() {
  start();
  QJsonObject params;
  {
    params.insert(Message::kDirection, to_underlying(DirectionType::kRight));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kIsolateToOneSideNodeCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_isolate_on_top() {
  start();
  QJsonObject params;
  {
    params.insert(Message::kDirection, to_underlying(DirectionType::kUp));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kIsolateToOneSideNodeCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_isolate_on_bottom() {
  start();
  QJsonObject params;
  {
    params.insert(Message::kDirection, to_underlying(DirectionType::kDown));
  }
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kIsolateToOneSideNodeCompute, params);
  _manipulator->link_to_closest_node(node);
  finish();
}


void BrowserRecorder::record_isolate_by_index() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kIsolateByIndexNodeCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_join_elements() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kJoinElementsNodeCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_prune_elements() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kPruneElementsNodeCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_prune_duplicates() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kPruneDuplicatesNodeCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

void BrowserRecorder::record_highlight_elements() {
  start();
  Entity* node = _manipulator->create_browser_node(true, ComponentDID::kHighlightElementsCompute);
  _manipulator->link_to_closest_node(node);
  finish();
}

}
