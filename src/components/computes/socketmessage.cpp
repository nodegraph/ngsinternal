#include <components/computes/socketmessage.h>

namespace ngs {

SocketMessage::SocketMessage()
    : _json_text(""),
      _json_doc(),
      _json_obj(),
      _is_request(false) {
}

SocketMessage::SocketMessage(const SocketMessage& other):
  _json_text(other._json_text),
  _json_doc(other._json_doc),
  _json_obj(other._json_obj),
  _is_request(other._is_request){
}

SocketMessage::SocketMessage(const QString& json_text):
        _json_text(json_text),
        _json_doc(QJsonDocument::fromJson(json_text.toUtf8())),
        _json_obj(_json_doc.object()),
        _is_request(false)
{
  const QJsonObject::iterator iter = _json_obj.find("request");
  if (iter != _json_obj.end()) {
    _is_request = true;
  }
}

SocketMessage::SocketMessage(const QJsonObject& json_obj)
    : _json_doc(),
      _json_obj(json_obj),
      _is_request(false) {
  _json_doc.setObject(_json_obj);
  QByteArray bytes = _json_doc.toJson(QJsonDocument::Compact);
  _json_text = QString(bytes);
}

SocketMessage::~SocketMessage() {
}

}
