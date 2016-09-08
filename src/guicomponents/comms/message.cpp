#include <guicomponents/comms/message.h>
#include <iostream>

namespace ngs {

const char* Message::kRequest = "request";
const char* Message::kArgs = "args";
const char* Message::kXPath = "xpath";

const char* Message::kSuccess = "success";
const char* Message::kValue = "value";

const char* Message::kInfo = "info";

const char* Message::kIFrame = "iframe";
const char* Message::kMessageType = "msg_type";

const char* Message::kURL = "url";
const char* Message::kWidth = "width";
const char* Message::kHeight = "height";

Message::Message()
    : QVariantMap() {
}

Message::Message(const QString& json) {
  QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
  QJsonObject obj = doc.object();
  QVariantMap map = obj.toVariantMap();

  operator[](Message::kIFrame) = map[Message::kIFrame];
  operator[](Message::kMessageType) = map[Message::kMessageType];

  if (obj.keys().contains(Message::kRequest)) {
    merge_request_object(map);
  } if (obj.keys().contains(Message::kInfo)) {
    merge_info_object(map);
  } else {
    merge_response_object(map);
  }
}

Message::Message(const QString& iframe, RequestType rt, const QJsonObject& args, const QString& xpath) {
  operator[](Message::kIFrame) = iframe;
  operator[](Message::kMessageType) = static_cast<int>(Message::MessageType::KRequestMessage);

  operator[](Message::kRequest) = static_cast<int>(rt);
  operator[](Message::kArgs) = args;
  operator[](Message::kXPath) = xpath;
}

Message::Message(const QString& iframe, bool success, const QJsonValue& value) {
  operator[](Message::kIFrame) = iframe;
  operator[](Message::kMessageType) = static_cast<int>(Message::MessageType::KResponseMessage);

  operator[](Message::kSuccess) = success;
  operator[](Message::kValue) = value;
}

Message::Message(const QVariantMap& other) {
  operator[](Message::kIFrame) = other[Message::kIFrame];
  operator[](Message::kMessageType) = other[Message::kMessageType];

  MessageType type = static_cast<MessageType>(operator[](Message::kMessageType).toInt());
  switch (type) {
    case MessageType::KRequestMessage:
      merge_request_object(other);
      break;
    case MessageType::KResponseMessage:
      merge_response_object(other);
      break;
    case MessageType::KInfoMessage:
      merge_info_object(other);
      break;
    default:
      break;
  }
}

Message::~Message() {
}

void Message::merge_request_object(const QVariantMap& obj) {
  if (obj.keys().contains(Message::kRequest)) {
    operator[](Message::kRequest) = obj[Message::kRequest];
  }
  if (obj.keys().contains(Message::kArgs)) {
    operator[](Message::kArgs) = obj[Message::kArgs];
  }
  if (obj.keys().contains(Message::kXPath)) {
    operator[](Message::kXPath) = obj[Message::kXPath];
  }
}

void Message::merge_info_object(const QVariantMap& obj) {
  if (obj.keys().contains(Message::kInfo)) {
    operator[](Message::kInfo) = obj[Message::kInfo];
  }
}

void Message::merge_response_object(const QVariantMap& obj) {
  if (obj.keys().contains(Message::kSuccess)) {
    operator[](Message::kSuccess) = obj[Message::kSuccess];
  }
  if (obj.keys().contains(Message::kValue)) {
    operator[](Message::kValue) = obj[Message::kValue];
  }
}

QString Message::to_string() const {
  QJsonDocument doc;
  QJsonObject obj = QJsonObject::fromVariantMap(*this);
  doc.setObject(obj);
  QByteArray bytes = doc.toJson(QJsonDocument::Compact);
  return QString(bytes);
}

Message::MessageType Message::get_msg_type() const {
  return static_cast<Message::MessageType>(operator[](Message::kMessageType).toInt());
}

}
