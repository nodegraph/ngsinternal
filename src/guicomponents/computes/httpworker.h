#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QMap>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>

namespace ngs {

class Message;
class TaskScheduler;
class TaskContext;
class Compute;
class BaseFactory;
class BaseNodeGraphManipulator;

class GUICOMPUTES_EXPORT HTTPWorker : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(HTTPWorker, HTTPWorker)
  HTTPWorker(Entity* parent);
  virtual ~HTTPWorker();

  void queue_get_outputs(TaskContext& tc, std::function<void(const QJsonObject&)> on_get_outputs);
  void queue_http_request(TaskContext& tc);


  void queue_merge_chain_state(TaskContext& tc, const QJsonObject& map);
  void queue_send_get_request(TaskContext& tc, const QUrl& url);
  void queue_send_post_request(TaskContext& tc, const QUrl& url, const QByteArray &data);
  void queue_send_put_request(TaskContext& tc, const QUrl& url, const QByteArray &data);
  void queue_send_delete_request(TaskContext& tc, const QUrl& url);
  void queue_send_patch_request(TaskContext& tc, const QUrl& url, const QByteArray &data);

 private slots:
   void finished_request();

 private:
  static QByteArray get_value_as_bytes(const QVariant &value);
  void merge_chain_state_task(const QJsonObject& map);
  void get_outputs_task(std::function<void(const QJsonObject&)> on_finished_sequence);

  // Main http request task.
  void http_request_task();

  // More specific http request tasks.
  void send_get_request_task(const QUrl& url);
  void send_post_request_task(const QUrl& url, const QByteArray &data);
  void send_put_request_task(const QUrl& url, const QByteArray &data);
  void send_delete_request_task(const QUrl& url);
  void send_patch_request_task(const QUrl& url, const QByteArray &data);



  // Our fixed dependencies.
  Dep<TaskScheduler> _scheduler;
  Dep<BaseNodeGraphManipulator> _manipulator;

  // Used for sanity checking task ids with the TaskScheduler.
  std::unordered_map<QNetworkReply*, int> _task_ids;

  QNetworkAccessManager _nam;
  QJsonObject _chain_state;
};

}
