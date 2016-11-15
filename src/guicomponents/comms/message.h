#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/utils/enumutil.h>

#include <QtCore/QString>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>

#include <QtCore/QJsonValue>
#include <QtCore/QJsonObject>

namespace ngs {


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
  kIsBrowserOpen,
  kResizeBrowser,
  kOpenBrowser,
  kCloseBrowser,

  // Web Page Requests.
  kBlockEvents,
  kUnblockEvents,
  kWaitUntilLoaded,
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

  // Firebase.
  kFirebaseInit,
  kFirebaseSignIn,
  kFirebaseSignOut,
  kFirebaseWriteData,
  kFirebaseReadData,
  kFirebaseSubscribe,
  kFirebaseUnsubscribe
};

enum class MouseActionType : int {
  kSendClick,
  kMouseOver,
  kStartMouseHover,
  kStopMouseHover,
};

enum class TextActionType : int {
  kSendText,
  kSendEnter,
};

enum class ElementActionType : int {
  kGetText,
  kSelectOption,
  kScroll
};

enum class InfoType : int {
  kPageIsLoading,
  kPageIsReady,
  kBgIsConnected,
  kShowWebActionMenu,
  kFirebaseChanged,
};

enum class MessageType : int {
  kUnformedMessage,
  kRequestMessage,
  kResponseMessage,
  kInfoMessage
};

enum class WrapType : int {
  text,
  image,
  input,
  select,
  iframe
};

enum class DirectionType : int {
  left,
  right,
  up,
  down
};

enum class HTTPSendType: int {
  kGet,
  kPost,
  kPut,
  kDelete,
  kPatch
};

class COMMS_EXPORT Message: public QJsonObject {
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

  static const char* kTextDataName;

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

  static const char* kDataPath;
  static const char* kNodePath;
  static const char* kListenForChanges;

  static const char* kEmail;
  static const char* kPassword;
  static const char* kDataName;

  static const char* kApiKey;
  static const char* kAuthDomain;
  static const char* kDatabaseURL;
  static const char* kStorageBucket;

  static const char* kHTTPRequestMethod;
  static const char* kPayload;
  static const char* kOutputPropertyName;

  static const char* kTopic;
  static const char* kMessage;

  static const char* kUsername;
  static const char* kHostName;
  static const char* kHostAddress;
  static const char* kPort;
  static const char* kClientID;

  Message();
  Message(const QString& json); // Initialize from a json string typcially coming from another process outside the native app.
  Message(const QJsonObject& other);

  // Initializes a request message.
  Message(RequestType rt, const QJsonObject& args = QJsonObject());
  // Initializes a response message.
  // The success arg is whether the app should continue sending more requests in this sequence.
  // False means some unrecoverable error has occured.
  Message(bool success, const QJsonValue& value = QJsonValue());
  // Initializes an info message.
  Message(InfoType it, const QJsonValue& value = QJsonValue());

  virtual ~Message();

  virtual QString to_string() const;
  virtual MessageType get_msg_type() const;

 private:
  bool check_contents();
};

}

