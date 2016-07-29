#pragma once
#include <components/computes/computes_export.h>

#include <QtCore/QString>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>

namespace ngs {

class COMPUTES_EXPORT SocketMessage: public QObject
{
  Q_OBJECT
public:
  SocketMessage();
  SocketMessage(const SocketMessage& other);
  SocketMessage(const QString& json_text);
  SocketMessage(const QJsonObject& json_obj);
  ~SocketMessage();

  bool is_request() const {return _is_request;}
  const QString& get_json_text() const {return _json_text;}
  const QJsonObject& get_json_obj() const {return _json_obj;}


private:
  QString _json_text; // The string which parses into a json document.
  QJsonDocument _json_doc; // The json document object.
  QJsonObject _json_obj; // The json object with all the key value pairs.

  bool _is_request; // The socket message can only be a request or a response.
};

}
