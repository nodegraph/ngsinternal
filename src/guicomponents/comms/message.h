#pragma once
#include <guicomponents/comms/comms_export.h>

#include <QtCore/QString>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtCore/QVariant>

namespace ngs {

template<class E>
typename std::underlying_type<E>::type to_underlying(E e) {
   return static_cast<typename std::underlying_type<E>::type>(e);
}


// Note we don't use the Q_DECLARE_METATYPE() or the Q_ENUM() macro because we may eventually
// move away from qml to javascript/typescript oriented gui framework, where we want our json
// message to only contain pure javascript primitive types.
// Note these enums have to be kept in sync with the typescript file, socketmessage.ts.
enum class RequestType : int {
  kUnknownRequest,

  // Chrome BG Requests.
  kClearAllCookies,
  kGetAllCookies,
  kSetAllCookies,
  kGetZoom,

  // Browser Requests.
  kShutdown,
  kCheckBrowserIsOpen,
  kResizeBrowser,
  kOpenBrowser,
  kCloseBrowser,

  // Web Page Requests.
  kBlockEvents,
  kUnblockEvents,
  kNavigateTo,
  kNavigateBack,
  kNavigateForward,
  kNavigateRefresh,
  kSwitchIFrame,

  // Page Content Set Requests.
  kGetXPath,
  kPerformMouseAction,
  kPerformTextAction,
  kPerformElementAction,
  kUpdateOveralys,
  kCacheFrame,
  kLoadCachedFrame,
  kCreateSetFromMatchValues,
  kCreateSetFromWrapType,
  kDeleteSet,
  kShiftSet,
  kExpandSet,
  kMarkSet,
  kUnmarkSet,
  kMergeMarkedSets,
  kShrinkSetToMarked,
  kShrinkSet,

  // Info Request.
  kGetCrosshairInfo,
};
//Q_DECLARE_METATYPE(RequestType);

enum class MouseActionType : int {
  kSendClick,
  kMouseOver,
  kStartMouseHover,
  kStopMouseHover,
};
//Q_DECLARE_METATYPE(MouseActionType);

enum class TextActionType : int {
  kSendText,
  kSendEnter,
};
//Q_DECLARE_METATYPE(TextActionType);

enum class ElementActionType : int {
  kGetText,
  kSelectOption,
  kScroll
};
//Q_DECLARE_METATYPE(ElementActionType);

//enum ActionType {
//  kSendClick,
//  kMouseOver,
//  kStartMouseHover,
//  kStopMouseHover,
//  kSendText,
//  kSendEnter,
//  kGetText,
//  kSelectOption,
//  kScrollDown,
//  kScrollUp,
//  kScrollRight,
//  kScrollLeft
//};

enum class InfoType : int {
  kPageIsLoading,
  kPageIsReady,
  kBgIsConnected,
  kShowWebActionMenu
};
//Q_DECLARE_METATYPE(InfoType);

enum class MessageType : int {
  kUnformedMessage,
  kRequestMessage,
  kResponseMessage,
  kInfoMessage
};
//Q_DECLARE_METATYPE(MessageType);

enum class WrapType : int {
    text,
    image,
    input,
    select,
    iframe
};
//Q_DECLARE_METATYPE(WrapType);

enum class Direction : int {
    left,
    right,
    up,
    down
};
//Q_DECLARE_METATYPE(Direction);

class COMMS_EXPORT Message: public QVariantMap {
 public:

  static const char* kRequest;
  static const char* kArgs;
  static const char* kXPath;

  static const char* kSuccess;
  static const char* kValue;

  static const char* kInfo;

  static const char* kIFrame;
  static const char* kPrevIFrame; // Sent by show web action request it's iframe doesn't match the current iframe.
  static const char* kMessageType;

  static const char* kURL;
  static const char* kWidth;
  static const char* kHeight;
  static const char* kDimensions;

  static const char* kWrapType;
  static const char* kTextValues;
  static const char* kImageValues;

  static const char* kMouseAction;
  static const char* kTextAction;
  static const char* kElementAction;

  static const char* kSetIndex;
  static const char* kOverlayIndex;

  static const char* kText;
  static const char* kOptionText;
  static const char* kPosition;

  static const char* kDirection;
  static const char* kDirections;

  static const char* kMatchCriteria;

  static const char* kMatchLeft;
  static const char* kMatchRight;
  static const char* kMatchTop;
  static const char* kMatchBottom;

  static const char* kMatchFont;
  static const char* kMatchFontSize;

  static const char* kOptionTexts;
  static const char* kOptionValues;

  static const char* kClickPos;
  static const char* kNearestRelClickPos;
  static const char* kOverlayRelClickPos;

  static const char* kAppIFramePath;
  static const char* kID;

  Message();
  Message(const QString& json); // Initialize from a json string typcially coming from another process outside the native app.
  Message(const QVariantMap& other);

  // Initializes a request message.
  Message(RequestType rt, const QVariantMap& args = QVariantMap());
  // Initializes a response message.
  Message(bool success, const QVariant& value = QVariant());
  // Initializes an info message.
  Message(InfoType it, const QVariant& value = QVariant());

  virtual ~Message();

  virtual QString to_string() const;
  virtual MessageType get_msg_type() const;

 private:
  bool check_contents();
};

}

