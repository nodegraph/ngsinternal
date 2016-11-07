#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QMap>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include <QtNetwork/qnetworkaccessmanager.h>

class QNetworkReply;

namespace ngs {

class Message;
class TaskScheduler;
class TaskContext;
class Compute;
class BaseFactory;
class BaseNodeGraphManipulator;

// This class communicates with the nodejs process.
class COMMS_EXPORT HTTPWorker : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(HTTPWorker, HTTPWorker)
  HTTPWorker(Entity* parent);
  virtual ~HTTPWorker();

  void queue_get_outputs(TaskContext& tc, std::function<void(const QJsonObject&)> on_get_outputs);
  void queue_http_send(TaskContext& tc);
  void queue_http_get(TaskContext& tc);


  void queue_merge_chain_state(TaskContext& tc, const QJsonObject& map);
  void queue_send_get_request(TaskContext& tc, const QUrl& url);
  void queue_send_post_request(TaskContext& tc, const QUrl& url, const QVariant& value);
  void queue_send_put_request(TaskContext& tc, const QUrl& url, const QVariant& value);
  void queue_send_delete_request(TaskContext& tc, const QUrl& url);
  void queue_send_patch_request(TaskContext& tc, const QUrl& url, const QVariant& value);

 private slots:
   void finished_request(QNetworkReply* reply);

 private:
  static QByteArray get_value_as_bytes(const QVariant &value);
  void merge_chain_state_task(const QJsonObject& map);
  void get_outputs_task(std::function<void(const QJsonObject&)> on_finished_sequence);

  void http_send_task();
  void http_get_task();

  void send_get_request_task(const QUrl& url);
  void send_post_request_task(const QUrl& url, const QVariant& value);
  void send_put_request_task(const QUrl& url, const QVariant& value);
  void send_delete_request_task(const QUrl& url);
  void send_patch_request_task(const QUrl& url, const QVariant& value);



  // Our fixed dependencies.
  Dep<TaskScheduler> _task_scheduler;
  Dep<BaseNodeGraphManipulator> _ng_manipulator;

  QNetworkAccessManager _nam;
  QJsonObject _chain_state;
};

}
