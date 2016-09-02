#include <guicomponents/comms/socketmessage.h>
#include <iostream>

namespace ngs {

const char* Message::kRequest = "request";
const char* Message::kArgs = "args";
const char* Message::kXPath = "xpath";

const char* Message::kSuccess = "success";
const char* Message::kValue = "value";

const char* Message::kURL = "url";
const char* Message::kWidth = "width";
const char* Message::kHeight = "height";

Message::Message()
    : QJsonObject() {
}

Message::Message(const QString& json) {
  QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
  QJsonObject obj = doc.object();
  if (obj.keys().contains(Message::kRequest)) {
    merge_request_object(obj);
  } else {
    merge_response_object(obj);
  }
}

Message::Message(RequestType rt, const QJsonObject& args, const QString& xpath) {
  operator[](Message::kRequest) = rt;
  operator[](Message::kArgs) = args;
  operator[](Message::kXPath) = xpath;
}

Message::Message(bool success, const QJsonValue& value) {
  operator[](Message::kSuccess) = success;
  operator[](Message::kValue) = value;
}

Message::Message(const Message& other) {
  if (other.keys().contains(Message::kRequest)) {
    merge_request_object(other);
  } else {
    merge_response_object(other);
  }
}

Message::~Message() {
}

void Message::merge_request_object(const QJsonObject& obj) {
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

void Message::merge_response_object(const QJsonObject& obj) {
  if (obj.keys().contains(Message::kSuccess)) {
    operator[](Message::kSuccess) = obj[Message::kSuccess];
  }
  if (obj.keys().contains(Message::kValue)) {
    operator[](Message::kValue) = obj[Message::kValue];
  }
}

QString Message::to_string() const {
  QJsonDocument doc;
  doc.setObject(*this);
  QByteArray bytes = doc.toJson(QJsonDocument::Compact);
  return QString(bytes);
}

bool Message::is_request() const {
  if (keys().contains(Message::kRequest)) {
    return true;
  }
  return false;
}

}
