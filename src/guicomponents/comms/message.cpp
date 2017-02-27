#include <guicomponents/comms/message.h>
#include <guicomponents/comms/messagetypestrings.h>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>

#include <iostream>
#include <cassert>

namespace ngs {

const char* Message::kRequest = "request";
const char* Message::kArgs = "args";
const char* Message::kXPath = "xpath";

const char* Message::kSuccess = "success";
const char* Message::kValue = "value";

const char* Message::kInfo = "info";

const char* Message::kFWIndexPath = "fw_index_path";
const char* Message::kFEIndexPath = "fe_index_path";
const char* Message::kMessageType = "msg_type";
const char* Message::kReceiverType = "receiver_type";

const char* Message::kURL = "url";
const char* Message::kHREF = "href";
const char* Message::kUseCurrentElement = "use_current_element";

const char* Message::kDirectory = "directory";
const char* Message::kFilename = "filename";

const char* Message::kMaxWidth = "max_width";
const char* Message::kMaxHeight = "max_height";
const char* Message::kMaxFilesize = "max_filesize";

const char* Message::kWidth = "width";
const char* Message::kHeight = "height";
const char* Message::kDimensions = "dimensions";
const char* Message::kNext = "next";

const char* Message::kApplicationName = "application_name";
const char* Message::kAllMatchingWindows = "all_matching_windows";
const char* Message::kKeys = "keys";

const char* Message::kTimeInMilliseconds = "time_in_milliseconds";

const char* Message::kWrapType = "wrap_type";
const char* Message::kTextValues = "text_values";
const char* Message::kImageValues = "image_values";
const char* Message::kTargetValues = "target_values";

const char* Message::kMouseAction = "mouse_action";
const char* Message::kTextAction = "text_action";
const char* Message::kElementAction = "element_action";

const char* Message::kText = "text";
const char* Message::kOptionText = "option_text";
const char* Message::kPosition = "position";

const char* Message::kAngle = "angle";
const char* Message::kMaxWidthDifference = "max_width_difference";
const char* Message::kMaxHeightDifference = "max_height_difference";
const char* Message::kMaxAngleDifference = "max_angle_difference";

const char* Message::kScrollDirection = "scroll_direction";
const char* Message::kDirections = "directions";
const char* Message::kAllowInternalElements = "allow_internal_elements";

const char* Message::kMatchCriteria = "match_criteria";
const char* Message::kMatchLeft = "match_left";
const char* Message::kMatchRight = "match_right";
const char* Message::kMatchTop = "match_top";
const char* Message::kMatchBottom = "match_bottom";
const char* Message::kMatchFont = "match_font";
const char* Message::kMatchFontSize = "match_font_size";

// Selection Dropdowns.
const char* Message::kOptionTexts = "option_texts";
const char* Message::kOptionValues = "option_values";

const char* Message::kGlobalMousePosition = "global_mouse_position";
const char* Message::kLocalMousePosition = "local_mouse_position";

const char* Message::kID = "id";

const char* Message::kDataPath = "data_path";
const char* Message::kNodePath = "node_path";
const char* Message::kListenForChanges = "listen_for_changes";

const char* Message::kEmail = "email";
const char* Message::kPassword = "password";

const char* Message::kApiKey = "apiKey";
const char* Message::kAuthDomain = "authDomain";
const char* Message::kDatabaseURL = "databaseURL";
const char* Message::kStorageBucket = "storageBucket";

const char* Message::kHTTPRequestMethod = "request_method";
const char* Message::kPayload = "payload";
//const char* Message::kOutputPropertyName = "output_property_name";

const char* Message::kTopic = "topic";
const char* Message::kMessage = "message";

const char* Message::kUsername = "username";
const char* Message::kHostName = "host_name";
const char* Message::kHostAddress = "host_address";
const char* Message::kPort = "port";
const char* Message::kClientID = "client_id";

Message::Message()
    : QJsonObject() {
}

Message::Message(const QString& json) {
  QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
  QJsonObject obj = doc.object();
  *this = obj;
  assert(check_contents());
}

Message::Message(WebDriverRequestType rt, const QJsonObject& args) {
  init_request(ReceiverType::WebDriver, to_underlying(rt),args);
}

Message::Message(ChromeRequestType rt, const QJsonObject& args) {
  init_request(ReceiverType::Chrome, to_underlying(rt),args);
}

Message::Message(PlatformRequestType rt, const QJsonObject& args) {
  init_request(ReceiverType::Platform, to_underlying(rt),args);
}

Message::Message(FirebaseRequestType rt, const QJsonObject& args) {
  init_request(ReceiverType::Firebase, to_underlying(rt),args);
}

void Message::init_request(ReceiverType receiver_type, int request_type, const QJsonObject& args) {
  insert(Message::kID, -1);
  insert(Message::kReceiverType, to_underlying(receiver_type));
  insert(Message::kMessageType, to_underlying(MessageType::kRequestMessage));

  insert(Message::kRequest, request_type);
  insert(Message::kArgs, args);

  assert(check_contents());
}

Message::Message(ReceiverType receiver_type, bool success, const QJsonValue& value) {
  insert(Message::kID, -1);
  insert(Message::kReceiverType, to_underlying(receiver_type));
  insert(Message::kMessageType, to_underlying(MessageType::kResponseMessage));

  insert(Message::kSuccess, success);
  insert(Message::kValue, value);

  assert(check_contents());
}

Message::Message(ReceiverType receiver_type, InfoType it, const QJsonValue& value) {
  insert(Message::kID, -1);
  insert(Message::kReceiverType, to_underlying(receiver_type));
  insert(Message::kMessageType, to_underlying(MessageType::kResponseMessage));

  insert(Message::kInfo, to_underlying(it));
  insert(Message::kValue, value);

  assert(check_contents());
}

Message::Message(const QJsonObject& other) {
  QJsonObject* me = this;
  *me = other;
  assert(check_contents());
}

Message::~Message() {
}

bool Message::check_contents() {
  if (!contains(Message::kID)) {
    std::cerr << "Error: message is missing id.\n";
    return false;
  }
  if (!contains(Message::kMessageType)) {
    std::cerr << "Error: message is missing msg_type.\n";
    return false;
  }
  if (!contains(Message::kReceiverType)) {
      std::cerr << "Error: message is missing receiver_type.\n";
      return false;
    }

  MessageType msg_type = static_cast<MessageType>(value(Message::kMessageType).toInt());
  switch(msg_type) {
    case MessageType::kUnformedMessage:
      break;
    case MessageType::kRequestMessage:
      if (!contains(Message::kRequest)) {
        std::cerr << "Error: request message is missing request.\n";
        return false;
      }
      if (!contains(Message::kArgs)) {
        std::cerr << "Error: request message is missing args.\n";
        return false;
      }
      break;
    case MessageType::kResponseMessage:
      if (!contains(Message::kSuccess)) {
        std::cerr << "Error: response message is missing success.\n";
        return false;
      }
      if (!contains(Message::kValue)) {
        std::cerr << "Error: response message is missing value.\n";
        return false;
      }
      break;
    case MessageType::kInfoMessage:
      if (!contains(Message::kInfo)) {
        std::cerr << "Error: info message is missing info.\n";
        return false;
      }
      break;
  }
  return true;
}

QString Message::to_string() const {
  QJsonDocument doc;
  doc.setObject(*this);
  QByteArray bytes = doc.toJson(QJsonDocument::Compact);
  return QString(bytes);
}

int Message::get_id() const {
  return value(Message::kID).toInt();
}

MessageType Message::get_msg_type() const {
  return static_cast<MessageType>(value(Message::kMessageType).toInt());
}

ReceiverType Message::get_receiver_type() const {
  return static_cast<ReceiverType>(value(Message::kReceiverType).toInt());
}

void Message::dump() const {
  std::cerr << "id: " << get_id() << "\n";

  MessageType msg_type = static_cast<MessageType>(value(Message::kMessageType).toInt());
  std::cerr << "msg_type: " << message_type_to_string(msg_type) << "\n";

  ReceiverType receiver_type = static_cast<ReceiverType>(value(Message::kReceiverType).toInt());
  std::cerr << "receiver type: " << receiver_type_to_string(receiver_type) << "\n";

  const_iterator iter = begin();
  while (iter != end()) {

    if (iter.key() == "receiver_type") {
    } else if (iter.key() == Message::kID) {
    } else if (iter.key() == Message::kMessageType) {
    } else if (iter.key() == Message::kRequest) {
      std::cerr << "request: ";
      switch(receiver_type) {
        case ReceiverType::Unknown: {
          std::cerr << "unknown" << "\n";
        } break;
        case ReceiverType::WebDriver: {
          WebDriverRequestType t = static_cast<WebDriverRequestType>(iter.value().toInt());
          std::cerr << web_driver_request_type_to_string(t) << "\n";
        } break;
        case ReceiverType::Chrome: {
          ChromeRequestType t = static_cast<ChromeRequestType>(iter.value().toInt());
          std::cerr << chrome_request_type_to_string(t) << "\n";
        } break;
        case ReceiverType::Platform: {
          PlatformRequestType t = static_cast<PlatformRequestType>(iter.value().toInt());
          std::cerr << platform_request_type_to_string(t) << "\n";
        } break;
        case ReceiverType::Firebase: {
          FirebaseRequestType t = static_cast<FirebaseRequestType>(iter.value().toInt());
          std::cerr << firebase_request_type_to_string(t) << "\n";
        } break;
        default: {
          assert(false);
        } break;
      }
    }  else if (iter.key() == Message::kInfo) {
      InfoType t = static_cast<InfoType>(iter.value().toInt());
      std::cerr << "info: " << info_type_to_string(t) << "\n";
    } else if (iter.key() == Message::kSuccess) {
      std::cerr << "success: " << iter.value().toBool() << "\n";
    } else if (iter.key() == Message::kValue) {
      if (iter.value().isObject() || iter.value().isArray()) {
        QJsonDocument doc(iter.value().toObject());
        std::cerr << "value: " << doc.toJson().toStdString() << "\n";
      } else {
        std::cerr << "value: " << iter.value().toString().toStdString() << "\n";
      }
    } else if (iter.key() == Message::kArgs) {
      if (iter.value().isObject()) {
        QJsonDocument doc(iter.value().toObject());
        std::cerr << "args: " << doc.toJson().toStdString() << "\n";
      } else if (iter.value().isArray()) {
        QJsonDocument doc(iter.value().toArray());
        std::cerr << "args: " << doc.toJson().toStdString() << "\n";
      } else {
        std::cerr << "Error: args should always be an object.\n";
      }
    } else {
      std::cerr << "found unexpected property: " << iter.key().toStdString() << "\n";
    }

    iter++;
  }
  std::cerr << "--------------------------\n";
}

}
