#pragma once
#include <guicomponents/comms/comms_export.h>

#include <QtCore/QString>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtCore/QVariant>

namespace ngs {

class COMMS_EXPORT Message: public QVariantMap {
  Q_GADGET
 public:

  // Note in order for the enums to be visible in Qt they have to start with a capital letter.
  // Note these enums have to be kept in sync with the typescript file, socketmessage.ts.
  enum class RequestType {
    KUnknownRequest,
    KShowAppMenu,

    // Chrome BG Requests.
    KClearAllCookies,
    KGetAllCookies,
    KSetAllCookies,
    KGetZoom,

    // Browser Requests.
    KShutdown,
    KCheckBrowserIsOpen,
    KResizeBrowser,
    KOpenBrowser,
    KCloseBrowser,

    // Web Page Requests.
    KBlockEvents,
    KNavigateTo,
    KNavigateBack,
    KNavigateForward,
    KNavigateRefresh,

    // Page Content Set Requests.
    KPerformAction,
    KUpdateOveralys,
    KCreateSetFromMatchValues,
    KCreateSetFromWrapType,
    KDeleteSet,
    KShiftSet,
    KExpandSet,
    KMarkSet,
    KUnmarkSet,
    KMergeMarkedSets,
    KShrinkSetToMarked,
    KShrinkSet
  };
  Q_ENUM(RequestType);

  enum class ActionType {
    KSendClick,
    KMouseOver,
    KSendText,
    KSendEnter,
    KGetText,
    KSelectOption,
    KScrollDown,
    KScrollUp,
    KScrollRight,
    KScrollLeft
  };
  Q_ENUM(ActionType);

  enum class InfoType {
    KPageIsLoading,
    KPageIsReady,
    KBgIsConnected
  };
  Q_ENUM(InfoType);

  enum class MessageType {
    KUnformedMessage,
    KRequestMessage,
    KResponseMessage,
    KInfoMessage
  };
  Q_ENUM(MessageType);


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

  Message();
  Message(const QString& json); // Initialize from a json string.
  Message(const QString& iframe, RequestType rt, const QJsonObject& args = QJsonObject(), const QString& xpath = ""); // Initializes a request message.
  Message(const QString& iframe, bool success, const QJsonValue& value = QJsonValue()); // Initializes a response message.
  Message(const QVariantMap& other);
  virtual ~Message();

  Message& operator =(const Message& other) {*this = other; return *this;}

  virtual QString to_string() const;
  virtual MessageType get_msg_type() const;

 private:
  void merge_request_object(const QVariantMap& obj);
  void merge_response_object(const QVariantMap& obj);
  void merge_info_object(const QVariantMap& obj);
};

Q_DECLARE_METATYPE(Message)

}
