#pragma once
#include <guicomponents/comms/comms_export.h>
#include <guicomponents/comms/messagetypes.h>
#include <base/utils/enumutil.h>

#include <QtCore/QJsonObject>

class QString;
class QJsonValue;

namespace ngs {

class COMMS_EXPORT Message: public QJsonObject {
 public:

  static const char* kRequest;
  static const char* kArgs;
  static const char* kXPath;

  static const char* kSuccess;
  static const char* kValue;

  static const char* kInfo;

  static const char* kFrameIndexPath;
  static const char* kMessageType;

  static const char* kURL;
  static const char* kWidth;
  static const char* kHeight;
  static const char* kDimensions;
  static const char* kNext;

  static const char* kWrapType;
  static const char* kTextValues;
  static const char* kImageValues;
  static const char* kTargetValues;

  static const char* kMouseAction;
  static const char* kTextAction;
  static const char* kElementAction;

  static const char* kTextDataName;

  static const char* kText;
  static const char* kOptionText;
  static const char* kPosition;

  static const char* kDirection;
  static const char* kDirections;

  static const char* kAllowInternalElements;

  static const char* kMatchCriteria;

  static const char* kMatchLeft;
  static const char* kMatchRight;
  static const char* kMatchTop;
  static const char* kMatchBottom;

  static const char* kMatchFont;
  static const char* kMatchFontSize;

  static const char* kOptionTexts;
  static const char* kOptionValues;

  static const char* kGlobalMousePosition;
  static const char* kLocalMousePosition;

  static const char* kID;

  static const char* kDataPath;
  static const char* kNodePath;
  static const char* kListenForChanges;

  static const char* kEmail;
  static const char* kPassword;

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

