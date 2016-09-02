#pragma once
#include <guicomponents/comms/comms_export.h>

#include <QtCore/QString>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtCore/QVariant>

namespace ngs {

// Note these enums have to be kept in sync with the typescript file, socketmessage.ts.
enum RequestType {
    kUnknownRequest,
    // Browser Requests.
    kBlockEvents,
    kShutdown,
    kCheckBrowserIsOpen,
    kResizeBrowser,
    kOpenBrowser,
    kCloseBrowser,
    kNavigateTo,
    kNavigateBack,
    kNavigateForward,
    kNavigateRefresh,
    kPerformAction,
};

enum ActionType {
    kSendClick,
    kSendText,
    kSendEnter,
    kGetText,
    kSelectOption,
    kScrollDown,
    kScrollUp,
    kScrollRight,
    kScrollLeft
};

class COMMS_EXPORT Message: public QObject, public QJsonObject {
  Q_OBJECT
 public:

  static const char* kRequest;
  static const char* kArgs;
  static const char* kXPath;

  static const char* kSuccess;
  static const char* kValue;

  static const char* kURL;
  static const char* kWidth;
  static const char* kHeight;

  Message();
  Message(const QString& json); // Initialize from a json string.
  Message(RequestType rt, const QJsonObject& args = QJsonObject(), const QString& xpath = ""); // Initializes a request message.
  Message(bool success, const QJsonValue& value = QJsonValue()); // Initializes a response message.
  Message(const Message& other);
  virtual ~Message();

  virtual QString to_string() const;
  virtual bool is_request() const;

 private:
  void merge_request_object(const QJsonObject& obj);
  void merge_response_object(const QJsonObject& obj);
};

}
