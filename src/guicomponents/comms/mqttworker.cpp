#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <guicomponents/comms/mqttworker.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/message.h>
#include <components/computes/compute.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>
#include <guicomponents/computes/mqttcomputes.h>


namespace ngs {

MQTTWorker::MQTTWorker(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _scheduler(this),
      _manipulator(this),
      _current_client(NULL),
      _current_task_id(-1){
  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
  get_dep_loader()->register_fixed_dep(_manipulator, Path({}));

  _timer.setSingleShot(true);
  _timer.setInterval(1000); // We allow 1 second to wait to connect.
  connect(&_timer,SIGNAL(timeout()),this,SLOT(on_time_out()));
}

MQTTWorker::~MQTTWorker() {
  // Qt should destroy our mqtt clients, as we parented them underneath us.
}

bool MQTTWorker::is_subscribed(const Compute* compute, const std::string& topic) const {
  if (!_listeners.count(topic)) {
    return false;
  }
  Path node_path = compute->our_entity()->get_path();
  const std::unordered_set<Path, PathHasher>& paths = _listeners.at(topic);
  if (paths.count(node_path)) {
    return true;
  }
  return false;
}

void MQTTWorker::dive_into_lockable_group(const std::string& child_group_name, const QHostAddress& host_address, const quint16 port, const QString& username, const QString& password) {
  TaskContext tc(_scheduler);
  queue_connect_task(tc, host_address, port, username, password);
  queue_dive_into_lockable_group(tc, child_group_name);
}

void MQTTWorker::clean_lockable_group(const std::string& child_group_name, const QHostAddress& host_address, const quint16 port, const QString& username, const QString& password) {
  TaskContext tc(_scheduler);
  queue_connect_task(tc, host_address, port, username, password);
  queue_clean_lockable_group(tc, child_group_name);
}

void MQTTWorker::queue_connect_task(TaskContext& tc, const QHostAddress& host_address, quint16 port,
                                    const QString& username, const QString& password) {
  _scheduler->queue_task(tc, (Task) std::bind(&MQTTWorker::connect_task, this, host_address, port, username, password), "queue_connect_task");
}

void MQTTWorker::queue_publish_task(TaskContext& tc, const QString& topic, const QString& message) {
  _scheduler->queue_task(tc, (Task) std::bind(&MQTTWorker::publish_task, this, topic, message), "queue_publish_task");
}

void MQTTWorker::queue_subscribe_task(TaskContext& tc, const QString& topic, const Path& node_path) {
  _scheduler->queue_task(tc, (Task) std::bind(&MQTTWorker::subscribe_task, this, topic, node_path), "queue_subscribe_task");
}

void MQTTWorker::queue_unsubscribe_task(TaskContext& tc, const QString& topic) {
  _scheduler->queue_task(tc, (Task) std::bind(&MQTTWorker::unsubscribe_task, this, topic), "queue_send_delete_request");
}

void MQTTWorker::queue_dive_into_lockable_group(TaskContext& tc, const std::string& child_group_name) {
  _scheduler->queue_task(tc, (Task)std::bind(&MQTTWorker::dive_into_lockable_group_task, this, child_group_name), "queue_dive_into_lockable_group");
}

void MQTTWorker::queue_clean_lockable_group(TaskContext& tc, const std::string& child_group_name) {
  _scheduler->queue_task(tc, (Task)std::bind(&MQTTWorker::clean_lockable_group_task, this, child_group_name), "queue_clean_lockable_group");
}

void MQTTWorker::queue_finished_compute(TaskContext& tc, BaseMQTTCompute* compute) {
  _scheduler->queue_task(tc, (Task)std::bind(&MQTTWorker::finished_compute_task, this, compute), "queue_finished_compute");
}

// ------------------------------------------------------------------------------------------
// Client Management.
// ------------------------------------------------------------------------------------------

std::string MQTTWorker::get_key(const QHostAddress& host_address, const quint16 port) {
  QString key = host_address.toString() + QString::number(port);
  return key.toStdString();
}

QMQTT::Client* MQTTWorker::get_client(const QHostAddress& host_address, const quint16 port) {
  std::string key = get_key(host_address, port);
  if (_clients.count(key)) {
    return _clients.at(key);
  }
  QMQTT::Client* client = new_ff QMQTT::Client(host_address, port, NULL);
  client->setWillRetain(true);
  connect(client, SIGNAL(connected()), this, SLOT(on_connected()));
  connect(client, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
  connect(client, SIGNAL(published(const QMQTT::Message&)), this, SLOT(on_published(const QMQTT::Message&)));
  connect(client, SIGNAL(subscribed(const QString&)), this, SLOT(on_subscribed(const QString&)));
  connect(client, SIGNAL(unsubscribed(const QString&)), this, SLOT(on_unsubscribed(const QString&)));
  connect(client, SIGNAL(received(const QMQTT::Message&)), this, SLOT(on_received(const QMQTT::Message&)));
  _clients[key] = client;

  return client;
}

// ------------------------------------------------------------------------------------------
// Tasks.
// ------------------------------------------------------------------------------------------

void MQTTWorker::connect_task(const QHostAddress& host_address, quint16 port, const QString& username, const QString& password) {
  _current_client = get_client(host_address, port); //QHostAddress::LocalHost
  assert(_current_client);
  if (!username.isEmpty()) {
    _current_client->setUsername(username);
    _current_client->setPassword(password);
  } else {
    _current_client->setUsername("");
    _current_client->setPassword("");
  }
  if (!_current_client->isConnectedToHost()) {
    _current_task_id = _scheduler->wait_for_response();
    _current_client->connectToHost();
    // Start timer.
    _timer.start();
  } else {
    _scheduler->run_next_task();
  }
}

void MQTTWorker::publish_task(const QString& topic, const QString& message) {
  _current_task_id = _scheduler->wait_for_response();
  QMQTT::Message msg(_current_task_id, topic, message.toUtf8());
  _current_client->publish(msg);
}

void MQTTWorker::subscribe_task(const QString& topic, const Path& node_path) {
  _listeners[topic.toStdString()].insert(node_path);
  _current_task_id = _scheduler->wait_for_response();
  _current_client->subscribe(topic, 0);
}

void MQTTWorker::unsubscribe_task(const QString& topic) {
  _current_task_id = _scheduler->wait_for_response();
  _current_client->unsubscribe(topic);
}

void MQTTWorker::on_time_out() {
  QString error = "Unable to connect to MQTT broker. Make sure the host address, username or password is correct on the group.";
  _scheduler->done_waiting_for_response(_current_task_id, error);
}

void MQTTWorker::on_connected() {
  _timer.stop();
  QString error;
  _scheduler->done_waiting_for_response(_current_task_id, error);
}

void MQTTWorker::on_disconnected() {
  _timer.stop();
  QString error = "Disconnected from MQTT broker. Make sure the host address, username or password is correct on the group.";
  _scheduler->done_waiting_for_response(_current_task_id, error);
}

void MQTTWorker::on_published(const QMQTT::Message& message) {
  std::cerr << "mqtt worker on_published\n";
  QString error;
  _scheduler->done_waiting_for_response(_current_task_id, error);
}

void MQTTWorker::on_subscribed(const QString& topic) {
  std::cerr << "mqtt worker on_subscribed\n";
  QString error;
  _scheduler->done_waiting_for_response(_current_task_id, error);
}

void MQTTWorker::on_unsubscribed(const QString& topic) {
  std::cerr << "mqtt worker on_unsubscribed\n";
  QString error;
  _scheduler->done_waiting_for_response(_current_task_id, error);
}

void MQTTWorker::on_received(const QMQTT::Message& message) {
  std::cerr << "mqtt worker on_received\n";
  // Merge the payload of the message into the chain state.
  QString payload = QString::fromUtf8(message.payload());
  QString topic = message.topic();
  if (_listeners.count(topic.toStdString())) {
    const std::unordered_set<Path, PathHasher>& paths = _listeners.at(topic.toStdString());
    std::unordered_set<Path, PathHasher>::const_iterator iter;
    for (iter = paths.begin(); iter != paths.end(); ++iter) {
      const Path& node_path = *iter;
      _manipulator->set_mqtt_override(node_path, topic, payload);
    }
  }
}

void MQTTWorker::dive_into_lockable_group_task(const std::string& child_group_name) {
  std::cerr << "dive_into_lockable_group_task\n";
  _manipulator->dive_into_lockable_group(child_group_name);
  _scheduler->run_next_task();
}

void MQTTWorker::clean_lockable_group_task(const std::string& child_group_name) {
  _manipulator->clean_lockable_group(child_group_name);
  _scheduler->run_next_task();
}

void MQTTWorker::finished_compute_task(BaseMQTTCompute* compute) {
  compute->on_finished_compute();
}

}
