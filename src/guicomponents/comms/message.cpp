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

const char* Message::kWrapType = "wrap_type";
const char* Message::kMatchValues = "match_values";

const char* Message::kTextValues = "text_values";
const char* Message::kImageValues = "image_values";

const char* Message::kSetIndex = "set_index";
const char* Message::kOverlayIndex = "overlay_index";
const char* Message::kAction = "action";
const char* Message::kText = "text";
const char* Message::kOptionText = "option_text";

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

const char* Message::kAppIFramePath = "-1";

Message::Message()
    : QVariantMap() {
}

Message::Message(const QString& json) {
  QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
  QJsonObject obj = doc.object();
  *this = obj.toVariantMap();
  assert(check_contents());
}

Message::Message(RequestType rt, const QVariantMap& args, const QString& xpath) {
  operator[](Message::kIFrame) = Message::kAppIFramePath;
  operator[](Message::kMessageType) = static_cast<int>(MessageType::kRequestMessage);

  operator[](Message::kRequest) = static_cast<int>(rt);
  operator[](Message::kArgs) = args;
  operator[](Message::kXPath) = xpath;

  assert(check_contents());
}

Message::Message(bool success, const QVariant& value) {
  operator[](Message::kIFrame) = Message::kAppIFramePath;
  operator[](Message::kMessageType) = static_cast<int>(MessageType::kResponseMessage);

  operator[](Message::kSuccess) = success;
  operator[](Message::kValue) = value;

  assert(check_contents());
}

Message::Message(InfoType it) {
  operator[](Message::kIFrame) = Message::kAppIFramePath;
  operator[](Message::kMessageType) = static_cast<int>(MessageType::kResponseMessage);

  operator[](Message::kInfo) = static_cast<int>(it);

  assert(check_contents());
}

Message::Message(const QVariantMap& other) {
  QVariantMap* me = this;
  *me = other;
  assert(check_contents());
}

Message::~Message() {
}

bool Message::check_contents() {
  if (!keys().contains(Message::kIFrame)) {
    return false;
  }
  if (!keys().contains(Message::kMessageType)) {
    return false;
  }

  MessageType msg_type = static_cast<MessageType>(operator[](Message::kMessageType).toInt());
  switch(msg_type) {
    case MessageType::kUnformedMessage:
      break;
    case MessageType::kRequestMessage:
      if (!keys().contains(Message::kRequest)) {
        return false;
      }
      if (!keys().contains(Message::kArgs)) {
        return false;
      }
      if (!keys().contains(Message::kXPath)) {
        return false;
      }
      break;
    case MessageType::kResponseMessage:
      if (!keys().contains(Message::kSuccess)) {
        return false;
      }
      if (!keys().contains(Message::kValue)) {
        return false;
      }
      break;
    case MessageType::kInfoMessage:
      if (!keys().contains(Message::kInfo)) {
        return false;
      }
      break;
  }
  return true;
}

QString Message::to_string() const {
  QJsonDocument doc;
  QJsonObject obj = QJsonObject::fromVariantMap(*this);
  doc.setObject(obj);
  QByteArray bytes = doc.toJson(QJsonDocument::Compact);
  return QString(bytes);
}

MessageType Message::get_msg_type() const {
  return static_cast<MessageType>(operator[](Message::kMessageType).toInt());
}

}
