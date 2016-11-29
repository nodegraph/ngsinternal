#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <base/objectmodel/basefactory.h>
#include <components/computes/inputcompute.h>
#include <components/computes/jsonutils.h>

#include <guicomponents/computes/httpworker.h>

#include <iostream>
#include <sstream>

#include <QtCore/QCoreApplication>
#include <QtCore/QUrl>
#include <QtCore/QJsonDocument>
#include <QtCore/QDebug>
#include <QtQml/QQmlEngine>


#include <QtNetwork/qnetworkrequest.h>


namespace ngs {

HTTPWorker::HTTPWorker(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _scheduler(this),
      _manipulator(this) {
  get_dep_loader()->register_fixed_dep(_scheduler, Path());
  get_dep_loader()->register_fixed_dep(_manipulator, Path());
}

HTTPWorker::~HTTPWorker() {
}

void HTTPWorker::queue_get_outputs(TaskContext& tc, std::function<void(const QJsonObject&)> on_get_outputs) {
  _scheduler->queue_task(tc, (Task)std::bind(&HTTPWorker::get_outputs_task,this,on_get_outputs), "queue_get_outputs");
}

void HTTPWorker::queue_http_request(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task)std::bind(&HTTPWorker::http_request_task,this), "queue_http_request");
}






void HTTPWorker::queue_merge_chain_state(TaskContext& tc, const QJsonObject& map) {
  _scheduler->queue_task(tc, (Task) std::bind(&HTTPWorker::merge_chain_state_task, this, map), "queue_merge_chain_state");
}

void HTTPWorker::queue_send_get_request(TaskContext& tc, const QUrl& url) {
  _scheduler->queue_task(tc, (Task) std::bind(&HTTPWorker::send_get_request_task, this, url), "queue_send_get_request");
}

void HTTPWorker::queue_send_post_request(TaskContext& tc, const QUrl& url, const QByteArray &data) {
  _scheduler->queue_task(tc, (Task) std::bind(&HTTPWorker::send_post_request_task, this, url, data), "queue_send_post_request");
}

void HTTPWorker::queue_send_put_request(TaskContext& tc, const QUrl& url, const QByteArray &data) {
  _scheduler->queue_task(tc, (Task) std::bind(&HTTPWorker::send_put_request_task, this, url, data), "queue_send_put_request");
}

void HTTPWorker::queue_send_delete_request(TaskContext& tc, const QUrl& url) {
  _scheduler->queue_task(tc, (Task) std::bind(&HTTPWorker::send_delete_request_task, this, url), "queue_send_delete_request");
}

void HTTPWorker::queue_send_patch_request(TaskContext& tc, const QUrl& url, const const QByteArray &data) {
  _scheduler->queue_task(tc, (Task) std::bind(&HTTPWorker::send_patch_request_task, this, url, data), "queue_send_patch_request");
}



// ------------------------------------------------------------------------------------------
// Private Methods.
// ------------------------------------------------------------------------------------------

QByteArray HTTPWorker::get_value_as_bytes(const QVariant &value)
{
    QByteArray bytes;
    QJsonDocument doc = QJsonDocument::fromVariant(value);
    if (!doc.isNull())
        bytes = doc.toJson();
    else
        bytes = value.toByteArray();
    return bytes;
}

void HTTPWorker::merge_chain_state_task(const QJsonObject& map) {
  // Merge the values into the chain_state.
  JSONUtils::shallow_object_merge(_chain_state, map);
  _scheduler->run_next_task();
}

void HTTPWorker::get_outputs_task(std::function<void(const QJsonObject&)> on_get_outputs) {
  on_get_outputs(_chain_state);
  _scheduler->run_next_task();
}

void HTTPWorker::http_request_task() {
  QUrl url = _chain_state.value(Message::kURL).toString();
  QString json = _chain_state.value(Message::kPayload).toString();
  HTTPSendType http_send_type = static_cast<HTTPSendType>(_chain_state.value(Message::kHTTPRequestMethod).toInt());

  switch (http_send_type) {
    case HTTPSendType::kGet: {
      send_get_request_task(url);
      break;
    }
    case HTTPSendType::kPost: {
      send_post_request_task(url, json.toUtf8());
      break;
    }
    case HTTPSendType::kPut: {
      send_put_request_task(url, json.toUtf8());
      break;
    }
    case HTTPSendType::kDelete: {
      send_delete_request_task(url);
      break;
    }
    case HTTPSendType::kPatch: {
      send_patch_request_task(url, json.toUtf8());
      break;
    }
    default: {
      std::cerr << "Unknown HTTPSendType encountered.\n";
      break;
    }
  }
}

void HTTPWorker::send_get_request_task(const QUrl& url) {
  QNetworkRequest request(url);
  // We always use json.
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  int task_id = _scheduler->wait_for_response();
  QNetworkReply *reply = _nam.get(request);
  _task_ids[reply] = task_id;
  connect(reply, SIGNAL(finished()), this, SLOT(finished_request()));
}

void HTTPWorker::send_post_request_task(const QUrl& url, const QByteArray &data) {
  QNetworkRequest request(url);
  request.setRawHeader("Content-Type", "application/json");
  //request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json; charset=utf-8");
  int task_id = _scheduler->wait_for_response();
  QNetworkReply *reply = _nam.post(request, data);
  _task_ids[reply] = task_id;
  connect(reply, SIGNAL(finished()), this, SLOT(finished_request()));
}

void HTTPWorker::send_put_request_task(const QUrl& url, const QByteArray &data) {
  QNetworkRequest request(url);
  int task_id = _scheduler->wait_for_response();
  QNetworkReply *reply = _nam.put(request, data);
  _task_ids[reply] = task_id;
  connect(reply, SIGNAL(finished()), this, SLOT(finished_request()));
}

void HTTPWorker::send_delete_request_task(const QUrl& url) {
  QNetworkRequest request(url);
  int task_id = _scheduler->wait_for_response();
  QNetworkReply *reply = _nam.deleteResource(request);
  _task_ids[reply] = task_id;
  connect(reply, SIGNAL(finished()), this, SLOT(finished_request()));
}

void HTTPWorker::send_patch_request_task(const QUrl& url, const QByteArray &data) {
  QNetworkRequest request(url);
  int task_id = _scheduler->wait_for_response();
  request.setRawHeader("X-HTTP-Method-Override", "PATCH");
  QNetworkReply *reply = _nam.post(request, data);
  _task_ids[reply] = task_id;
  connect(reply, SIGNAL(finished()), this, SLOT(finished_request()));
}

void HTTPWorker::finished_request() {
  QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
  if (!reply) {
    return;
  }

  QString error;
  if (reply->error() == QNetworkReply::NoError) {
    std::cerr << "Content Type for reply: " << reply->header(QNetworkRequest::ContentTypeHeader).toString().toStdString() << "\n";

    QByteArray data = reply->readAll();
    std::cerr << "Got reply: " << data.toStdString() << "\n";

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
      std::cerr << "reply is an object\n";
      _chain_state.insert("value", doc.object());
    } else if (doc.isArray()) {
      std::cerr << "reply is an array\n";
      _chain_state.insert("value", doc.array());
    } else {
      std::cerr << "reply is a string\n";
      _chain_state.insert("value", QString(data));
    }
    // Run the next task.
    _scheduler->run_next_task();
  } else {
    // Also show the error marker on the node.
    std::cerr << "Error: http get request failed: " << reply->errorString().toStdString() << "\n";
    error = reply->errorString();
    _manipulator->set_error_node(reply->errorString());
    _manipulator->clear_ultimate_targets();
    // Reset our stack.
    _scheduler->force_stack_reset();
  }

  // Check the task id.
  assert(_task_ids.count(reply));
  int task_id = _task_ids.at(reply);
  _scheduler->done_waiting_for_response(task_id, error);
  _task_ids.erase(reply);

  // Let qt know to delete the reply later.
  reply->deleteLater();
}



}
