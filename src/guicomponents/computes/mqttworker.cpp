#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <guicomponents/computes/mqttworker.h>
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

  // Timer setup.
  _timer.setSingleShot(true);
  _timer.setInterval(1000); // We allow 1 second to wait to connect.
  connect(&_timer,SIGNAL(timeout()),this,SLOT(on_time_out()));
}

MQTTWorker::~MQTTWorker() {
  // Qt should destroy our mqtt clients, as we parented them underneath us.
}

bool MQTTWorker::is_connected(const Config& config) {
  QMQTT::Client* client = get_client(config);
  return client->isConnectedToHost();
}

bool MQTTWorker::is_subscribed(const Compute* compute, const std::string& topic) const {
  if (!_subscribers.count(topic)) {
    return false;
  }
  Path node_path = compute->get_path();
  const std::unordered_set<Path, PathHasher>& paths = _subscribers.at(topic);
  if (paths.count(node_path)) {
    return true;
  }
  return false;
}

void MQTTWorker::queue_connect_task(TaskContext& tc, const Config& config) {
  _scheduler->queue_task(tc, (Task) std::bind(&MQTTWorker::connect_task, this, config), "queue_connect_task");
}

void MQTTWorker::queue_publish_task(TaskContext& tc, const QString& topic, const QString& message) {
  _scheduler->queue_task(tc, (Task) std::bind(&MQTTWorker::publish_task, this, topic, message), "queue_publish_task");
}

void MQTTWorker::queue_subscribe_task(TaskContext& tc, const QString& topic, const Path& node_path) {
  _scheduler->queue_task(tc, (Task) std::bind(&MQTTWorker::subscribe_task, this, topic, node_path), "queue_subscribe_task");
}

void MQTTWorker::queue_unsubscribe_task(TaskContext& tc, const QString& topic, const Path& node_path) {
  _scheduler->queue_task(tc, (Task) std::bind(&MQTTWorker::unsubscribe_task, this, topic, node_path), "queue_send_delete_request");
}

void MQTTWorker::queue_finished_task(TaskContext& tc, std::function<void()> done) {
  _scheduler->queue_task(tc, (Task)std::bind(&MQTTWorker::finished_compute_task, this, done), "queue_finished_compute");
}

// ------------------------------------------------------------------------------------------
// Client Management.
// ------------------------------------------------------------------------------------------

std::string MQTTWorker::get_key(const Config& config) {
  QString demark = "??";
  QString key = config.host_address.toString() + demark + QString::number(config.port) + demark + config.username + demark + config.password;
  return key.toStdString();
}

void MQTTWorker::register_group_lock(const Config& config, const Path& path) {
  std::string key = get_key(config);
  _group_locks[key].insert(path);
}

void MQTTWorker::unregister_group_lock(const Path& path) {
  for (auto &iter: _group_locks) {
    iter.second.erase(path);
  }
}

QMQTT::Client* MQTTWorker::get_client(const Config& config) {
  std::string key = get_key(config);
  if (_clients.count(key)) {
    return _clients.at(key);
  }
  QMQTT::Client* client = new_ff QMQTT::Client(config.host_address, config.port, NULL);
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

// This method has connects the mqtt client to the broker.
// However it also has an extra job which is to set the current mqtt client.
// Even if the client is already connected to the broker, it makes sure to set the current client.
void MQTTWorker::connect_task(const Config& config) {
  _current_client = get_client(config); //QHostAddress::LocalHost
  assert(_current_client);
  if (!config.username.isEmpty()) {
    _current_client->setUsername(config.username);
    _current_client->setPassword(config.password);
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
  bool first = (_subscribers.count(topic.toStdString()) == 0);
  _subscribers[topic.toStdString()].insert(node_path);
  _current_task_id = _scheduler->wait_for_response();
  if (first) {
    _current_client->subscribe(topic, 0);
  }
}

void MQTTWorker::unsubscribe_task(const QString& topic, const Path& node_path) {
  // Remove the path from the subscribers for this topic..
  _subscribers[topic.toStdString()].erase(node_path);
  // If there no more paths for this topic, then unsubscribe from the topic.
  if (_subscribers[topic.toStdString()].empty()) {
    _subscribers.erase(topic.toStdString());
    _current_task_id = _scheduler->wait_for_response();
    _current_client->unsubscribe(topic);
  } else {
    _scheduler->run_next_task();
  }
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

  // Get the sender.
  QObject* sender = QObject::sender();
  QMQTT::Client* client = dynamic_cast<QMQTT::Client*>(sender);

  // Get the key.
  Config config;
  config.host_address = client->host();
  config.port = client->port();
  config.username = client->username();
  config.password = client->password();
  std::string key = get_key(config);

  // Dirty all the group locks connected to this config.
  {
    for (auto& path: _group_locks.at(key)) {
      _manipulator->dirty_compute(path);
    }
  }

  QString error = "Disconnected from MQTT broker. Make sure the host address, username or password is correct on the group.";
  _scheduler->done_waiting_for_response(_current_task_id, error);
}

void MQTTWorker::on_published(const QMQTT::Message& message) {
  QString error;
  _scheduler->done_waiting_for_response(_current_task_id, error);
}

void MQTTWorker::on_subscribed(const QString& topic) {
  QString error;
  _scheduler->done_waiting_for_response(_current_task_id, error);
}

void MQTTWorker::on_unsubscribed(const QString& topic) {
  QString error;
  _scheduler->done_waiting_for_response(_current_task_id, error);
}

void MQTTWorker::on_received(const QMQTT::Message& message) {
  // Merge the payload of the message into the chain state.
  QString payload = QString::fromUtf8(message.payload());
  QString topic = message.topic();
  if (_subscribers.count(topic.toStdString())) {
    const std::unordered_set<Path, PathHasher>& paths = _subscribers.at(topic.toStdString());
    std::unordered_set<Path, PathHasher>::const_iterator iter;
    for (iter = paths.begin(); iter != paths.end(); ++iter) {
      const Path& node_path = *iter;
      _manipulator->set_mqtt_override(node_path, topic, payload);
    }
  }
}

void MQTTWorker::finished_compute_task(std::function<void()> done) {
  done();
  _scheduler->run_next_task();
}

}
