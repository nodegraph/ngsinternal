#pragma once
#include <guicomponents/comms/comms_export.h>

#include <QtCore/QString>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtCore/QVariant>

namespace ngs {

// Note in order for the enums to be visible in Qt they have to start with a capital letter.
// Note these enums have to be kept in sync with the typescript file, socketmessage.ts.
enum RequestType {
  kUnknownRequest,
  kShowAppMenu,

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
  kNavigateTo,
  kNavigateBack,
  kNavigateForward,
  kNavigateRefresh,
  kSwitchIFrame,

  // Page Content Set Requests.
  kPerformAction,
  kUpdateOveralys,
  kCreateSetFromMatchValues,
  kCreateSetFromWrapType,
  kDeleteSet,
  kShiftSet,
  kExpandSet,
  kMarkSet,
  kUnmarkSet,
  kMergeMarkedSets,
  kShrinkSetToMarked,
  kShrinkSet
};

enum ActionType {
  kSendClick,
  kMouseOver,
  kSendText,
  kSendEnter,
  kGetText,
  kSelectOption,
  kScrollDown,
  kScrollUp,
  kScrollRight,
  kScrollLeft
};

enum InfoType {
  kPageIsLoading,
  kPageIsReady,
  kBgIsConnected
};

enum MessageType {
  kUnformedMessage,
  kRequestMessage,
  kResponseMessage,
  kInfoMessage
};

enum WrapType {
    text,
    image,
    input,
    select
};

enum Direction {
    left,
    right,
    up,
    down
};

class COMMS_EXPORT Message: public QVariantMap {
 public:

  static const char* kRequest;
  static const char* kArgs;
  static const char* kXPath;

  static const char* kSuccess;
  static const char* kValue;

  static const char* kInfo;

  static const char* kIFrame;
  static const char* kMessageType;

  static const char* kURL;
  static const char* kWidth;
  static const char* kHeight;

  static const char* kWrapType;
  static const char* kMatchValues;

  static const char* kTextValues;
  static const char* kImageValues;

  static const char* kSetIndex;
  static const char* kOverlayIndex;
  static const char* kAction;
  static const char* kText;
  static const char* kOptionText;

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

  Message();
  Message(const QString& json); // Initialize from a json string.
  Message(const QVariantMap& other);

  // Initializes a request message.
  Message(const QString& iframe, RequestType rt, const QVariantMap& args = QVariantMap(), const QString& xpath = "");
  // Initializes a response message.
  Message(const QString& iframe, bool success, const QVariant& value = QVariant());
  // Initializes an info message.
  Message(const QString& iframe, InfoType it);

  virtual ~Message();

  virtual QString to_string() const;
  virtual MessageType get_msg_type() const;

 private:
  bool check_contents();
};

}
