#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <base/objectmodel/basefactory.h>
#include <components/computes/inputcompute.h>
#include <guicomponents/comms/httpworker.h>

#include <iostream>
#include <sstream>

#include <QtCore/QCoreApplication>
#include <QtCore/QUrl>
#include <QtCore/QDebug>
#include <QtQml/QQmlEngine>

#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>

namespace ngs {

HTTPWorker::HTTPWorker(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _task_scheduler(this),
      _ng_manipulator(this) {
  get_dep_loader()->register_fixed_dep(_task_scheduler, Path({}));
  get_dep_loader()->register_fixed_dep(_ng_manipulator, Path({}));
}

HTTPWorker::~HTTPWorker() {
}

void HTTPWorker::queue_get_outputs(TaskContext& tc, std::function<void(const QJsonObject&)> on_get_outputs) {
  _task_scheduler->queue_task(tc, (Task)std::bind(&HTTPWorker::get_outputs_task,this,on_get_outputs), "queue_get_outputs");
}

void HTTPWorker::queue_http_send(TaskContext& tc) {

}

void HTTPWorker::queue_http_get(TaskContext& tc) {

}





void HTTPWorker::queue_merge_chain_state(TaskContext& tc, const QJsonObject& map) {
  _task_scheduler->queue_task(tc, (Task) std::bind(&HTTPWorker::merge_chain_state_task, this, map), "queue_merge_chain_state");
}

void HTTPWorker::queue_send_get_request(TaskContext& tc, const QUrl& url) {
  _task_scheduler->queue_task(tc, (Task) std::bind(&HTTPWorker::send_get_request_task, this, url), "queue_send_get_request");
}

void HTTPWorker::queue_send_post_request(TaskContext& tc, const QUrl& url, const QVariant& value) {
  _task_scheduler->queue_task(tc, (Task) std::bind(&HTTPWorker::send_post_request_task, this, url, value), "queue_send_post_request");
}

void HTTPWorker::queue_send_put_request(TaskContext& tc, const QUrl& url, const QVariant& value) {
  _task_scheduler->queue_task(tc, (Task) std::bind(&HTTPWorker::send_put_request_task, this, url, value), "queue_send_put_request");
}

void HTTPWorker::queue_send_delete_request(TaskContext& tc, const QUrl& url) {
  _task_scheduler->queue_task(tc, (Task) std::bind(&HTTPWorker::send_delete_request_task, this, url), "queue_send_delete_request");
}

void HTTPWorker::queue_send_patch_request(TaskContext& tc, const QUrl& url, const QVariant& value) {
  _task_scheduler->queue_task(tc, (Task) std::bind(&HTTPWorker::send_patch_request_task, this, url, value), "queue_send_patch_request");
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
  for (QJsonObject::const_iterator iter = map.constBegin(); iter != map.constEnd(); ++iter) {
    _chain_state.insert(iter.key(), iter.value());
  }
  _task_scheduler->run_next_task();
}

void HTTPWorker::get_outputs_task(std::function<void(const QJsonObject&)> on_get_outputs) {
  on_get_outputs(_chain_state);
  _task_scheduler->run_next_task();
}

void HTTPWorker::http_send_task() {
  QUrl url = _chain_state.value(Message::kURL).toString();
  QString expr = _chain_state.value(Message::kValue).toString();
  HTTPSendType http_send_type = static_cast<HTTPSendType>(_chain_state.value(Message::kHTTPRequestType).toInt());

  QJSEngine engine;
  QJsonValue value;
  QString error;
  if (Compute::eval_js(engine, expr, value, error)) {
    switch(http_send_type) {
      case HTTPSendType::kPost: {
        send_post_request_task(url, value);
        break;
      }
      case HTTPSendType::kPut: {
        send_put_request_task(url, value);
        break;
      }
      case HTTPSendType::kDelete: {
        send_delete_request_task(url);
        break;
      }
      case HTTPSendType::kPatch: {
        send_patch_request_task(url, value);
        break;
      }
      default: {
        std::cerr << "Unknown HTTPSendType encountered.\n";
        break;
      }
    }
  }
}

void HTTPWorker::http_get_task() {
  QUrl url = _chain_state.value(Message::kURL).toString();
  QString data_name = _chain_state.value(Message::kDataName).toString();
  send_get_request_task(url);
}


void HTTPWorker::send_get_request_task(const QUrl& url) {
  QNetworkRequest request(url);
  // We always use json.
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  QNetworkReply *reply = _nam.get(request);
  connect(reply, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished_request(QNetworkReply*)));
}

void HTTPWorker::send_post_request_task(const QUrl& url, const QVariant& value) {
  QNetworkRequest request(url);
  QNetworkReply *reply = _nam.post(request, get_value_as_bytes(value));
  connect(reply, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished_request(QNetworkReply*)));
}

void HTTPWorker::send_put_request_task(const QUrl& url, const QVariant& value) {
  QNetworkRequest request(url);
  QNetworkReply *reply = _nam.put(request, get_value_as_bytes(value));
  connect(reply, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished_request(QNetworkReply*)));
}

void HTTPWorker::send_delete_request_task(const QUrl& url) {
  QNetworkRequest request(url);
  QNetworkReply *reply = _nam.deleteResource(request);
  connect(reply, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished_request(QNetworkReply*)));
}

void HTTPWorker::send_patch_request_task(const QUrl& url, const QVariant& value) {
  QNetworkRequest request(url);
  request.setRawHeader("X-HTTP-Method-Override", "PATCH");
  QNetworkReply *reply = _nam.post(request, get_value_as_bytes(value));
  connect(reply, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished_request(QNetworkReply*)));
}

void HTTPWorker::finished_request(QNetworkReply* reply) {
  if (!reply) {
    return;
  }
  if (reply->error() == QNetworkReply::NoError) {
    std::cerr << "Content Type for reply: " << reply->header(QNetworkRequest::ContentTypeHeader).toString().toStdString() << "\n";
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
      merge_chain_state_task(doc.object());
    } else {
      _chain_state.insert("value", QString(data));
    }
    // Run the next task.
    _task_scheduler->run_next_task();
  } else {
    // Also show the error marker on the node.
    std::cerr << "Error: http get request failed: " << reply->errorString().toStdString() << "\n";
    _ng_manipulator->set_error_node(reply->errorString());
    _ng_manipulator->clear_ultimate_target();
    // Reset our stack.
    _task_scheduler->force_stack_reset();
  }
  reply->deleteLater();
}



}
