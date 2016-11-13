#include <guicomponents/comms/message.h>
#include <iostream>
#include <cassert>

namespace ngs {

const char* Message::kRequest = "request";
const char* Message::kArgs = "args";
const char* Message::kXPath = "xpath";

const char* Message::kSuccess = "success";
const char* Message::kValue = "value";

const char* Message::kInfo = "info";

const char* Message::kIFrame = "iframe";
const char* Message::kPrevIFrame = "prev_iframe";
const char* Message::kMessageType = "msg_type";

const char* Message::kURL = "url";
const char* Message::kWidth = "width";
const char* Message::kHeight = "height";
const char* Message::kDimensions = "dimensions";

const char* Message::kWrapType = "wrap_type";
const char* Message::kTextValues = "text_values";
const char* Message::kImageValues = "image_values";

const char* Message::kMouseAction = "mouse_action";
const char* Message::kTextAction = "text_action";
const char* Message::kElementAction = "element_action";

const char* Message::kSetIndex = "set_index";
const char* Message::kOverlayIndex = "overlay_index";

const char* Message::kTextDataName = "text_data_name";

const char* Message::kText = "text";
const char* Message::kOptionText = "option_text";
const char* Message::kPosition = "position";

const char* Message::kDirection = "direction";
const char* Message::kDirections = "directions";

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

const char* Message::kClickPos = "click_pos";
const char* Message::kNearestRelClickPos = "nearest_rel_click_pos";
const char* Message::kOverlayRelClickPos = "overlay_rel_click_pos";
const char* Message::kAppIFramePath = "-1";

const char* Message::kID = "id";

const char* Message::kDataPath = "data_path";
const char* Message::kNodePath = "node_path";
const char* Message::kListenForChanges = "listen_for_changes";

const char* Message::kEmail = "email";
const char* Message::kPassword = "password";
const char* Message::kDataName = "data_name";

const char* Message::kApiKey = "apiKey";
const char* Message::kAuthDomain = "authDomain";
const char* Message::kDatabaseURL = "databaseURL";
const char* Message::kStorageBucket = "storageBucket";

const char* Message::kHTTPRequestMethod = "request_method";
const char* Message::kPayload = "payload";
const char* Message::kOutputPropertyName = "output_property_name";

const char* Message::kTopic = "topic";
const char* Message::kMessage = "message";

const char* Message::kUsername = "username";
const char* Message::kHostAddress = "host_address";
const char* Message::kPort = "port";

Message::Message()
    : QJsonObject() {
}

Message::Message(const QString& json) {
  QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
  QJsonObject obj = doc.object();
  *this = obj;
  assert(check_contents());
}

Message::Message(RequestType rt, const QJsonObject& args) {
  insert(Message::kID, -1);
  insert(Message::kIFrame, Message::kAppIFramePath);
  insert(Message::kMessageType, static_cast<int>(MessageType::kRequestMessage));

  insert(Message::kRequest, static_cast<int>(rt));
  insert(Message::kArgs, args);

  assert(check_contents());
}

Message::Message(bool success, const QJsonValue& value) {
  insert(Message::kID, -1);
  insert(Message::kIFrame, Message::kAppIFramePath);
  insert(Message::kMessageType, static_cast<int>(MessageType::kResponseMessage));

  insert(Message::kSuccess, success);
  insert(Message::kValue, value);

  assert(check_contents());
}

Message::Message(InfoType it, const QJsonValue& value) {
  insert(Message::kID, -1);
  insert(Message::kIFrame, Message::kAppIFramePath);
  insert(Message::kMessageType, static_cast<int>(MessageType::kResponseMessage));

  insert(Message::kInfo, static_cast<int>(it));
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
  if (!contains(Message::kIFrame)) {
    std::cerr << "Error: message is missing iframe.\n";
    return false;
  }
  if (!contains(Message::kMessageType)) {
    std::cerr << "Error: message is missing msg_type.\n";
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

MessageType Message::get_msg_type() const {
  return static_cast<MessageType>(value(Message::kMessageType).toInt());
}

}
