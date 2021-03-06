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
  static const char* kTargetValue;

  static const char* kInfo;
  static const char* kOther;
  static const char* kPrune;

  static const char* kFWIndexPath;
  static const char* kFEIndexPath;
  static const char* kMessageType;
  static const char* kReceiverType;

  static const char* kURL;
  static const char* kHREF;
  static const char* kUseCurrentElement;

  static const char* kDirectory;
  static const char* kFilename;

  static const char* kMergeBestStreams;
  static const char* kFormat;
  static const char* kThumbnails;
  static const char* kMaxWidth;
  static const char* kMaxHeight;
  static const char* kMaxFilesize;

  static const char* kWidth;
  static const char* kHeight;
  static const char* kDimensions;
  static const char* kNext;

  static const char* kApplicationName;
  static const char* kAllMatchingWindows;
  static const char* kKeys;

  static const char* kTimeInMilliseconds;

  static const char* kInputExpression;
  static const char* kOutputExpression;

  static const char* kElementType;
  static const char* kElements;
  static const char* kClusters;
  static const char* kAnchors;
  static const char* kStartIndex;
  static const char* kNumIndices;

  static const char* kTextValues;
  static const char* kImageValues;
  static const char* kTargetValues;

  static const char* kElementInfo;

  static const char* kMouseAction;
  static const char* kTextAction;
  static const char* kElementAction;

  static const char* kText;
  static const char* kOptionText;
  static const char* kPosition;

  static const char* kAngle;
  static const char* kMaxWidthDifference;
  static const char* kMaxHeightDifference;
  static const char* kMaxAngleDifference;

  static const char* kScrollDirection;
  static const char* kDirections;
  static const char* kDirection;

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
  //static const char* kOutputPropertyName;

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
  Message(WebDriverRequestType rt, const QJsonObject& args = QJsonObject());
  Message(ChromeRequestType rt, const QJsonObject& args = QJsonObject());
  Message(PlatformRequestType rt, const QJsonObject& args = QJsonObject());
  Message(FirebaseRequestType rt, const QJsonObject& args = QJsonObject());
  void init_request(ReceiverType receiver_type, int request_type, const QJsonObject& args);

  // Initializes a response message.
  // The success arg is whether the app should continue sending more requests in this sequence.
  // False means some unrecoverable error has occured.
  Message(ReceiverType receiver_type, bool success, const QJsonObject& value = QJsonObject());
  // Initializes an info message.
  Message(ReceiverType receiver_type, InfoType it, const QJsonValue& value = QJsonValue());

  virtual ~Message();

  virtual QString to_string() const;

  virtual int get_id() const;
  virtual MessageType get_msg_type() const;
  virtual ReceiverType get_receiver_type() const;

  virtual void dump() const;

 private:
  bool check_contents();
};

}

