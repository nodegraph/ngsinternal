#include <guicomponents/computes/entermqttgroupcompute.h>
#include <base/objectmodel/dep.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/inputs.h>
#include <guicomponents/comms/guitypes.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>


namespace ngs {

EnterMQTTGroupCompute::EnterMQTTGroupCompute(Entity* entity)
    : QObject(NULL),
      EnterGroupCompute(entity, kDID()),
      _scheduler(this) {
  get_dep_loader()->register_fixed_dep(_scheduler, Path());

  // MQTT Client.
  _client = new_ff QMQTT::Client();
  _client->setWillRetain(true);
  connect(_client, SIGNAL(connected()), this, SLOT(on_connected()));
  connect(_client, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
  connect(_client, SIGNAL(published(const QMQTT::Message&)), this, SLOT(on_published(const QMQTT::Message&)));
  connect(_client, SIGNAL(subscribed(const QString&)), this, SLOT(on_subscribed(const QString&)));
  connect(_client, SIGNAL(unsubscribed(const QString&)), this, SLOT(on_unsubscribed(const QString&)));
  connect(_client, SIGNAL(received(const QMQTT::Message&)), this, SLOT(on_received(const QMQTT::Message&)));

  // Timer setup.
  _timer.setSingleShot(true);
  _timer.setInterval(1000); // We allow 1 second to wait to connect.
  connect(&_timer,SIGNAL(timeout()),this,SLOT(on_time_out()));
}

EnterMQTTGroupCompute::~EnterMQTTGroupCompute() {
  _timer.stop();
  delete_ff(_client);
}

void EnterMQTTGroupCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  EnterGroupCompute::create_inputs_outputs(config);

  EntityConfig c = config;
  c.visible = false;
  c.expose_plug = false;
  c.unconnected_value = "";
  create_input(Message::kHostName, c);
  c.unconnected_value = "127.0.0.1";
  create_input(Message::kHostAddress, c);
  c.unconnected_value = 1883;
  create_input(Message::kPort, c);
  c.unconnected_value = "";
  create_input(Message::kUsername, c);
  create_input(Message::kPassword, c);
  create_input(Message::kClientID, c);
}

const QJsonObject EnterMQTTGroupCompute::_hints = EnterMQTTGroupCompute::init_hints();
QJsonObject EnterMQTTGroupCompute::init_hints() {
  QJsonObject m;

  add_hint(m, Message::kHostName, GUITypes::HintKey::DescriptionHint, "The host name of the MQTT broker. When set this takes precedence over host_address.");
  add_hint(m, Message::kHostAddress, GUITypes::HintKey::DescriptionHint, "The ip address of the MQTT broker.");
  add_hint(m, Message::kPort, GUITypes::HintKey::DescriptionHint, "The port number of the MQTT broker.");
  add_hint(m, Message::kUsername, GUITypes::HintKey::DescriptionHint, "The username to use if the MQTT broker uses authentication.");
  add_hint(m, Message::kPassword, GUITypes::HintKey::DescriptionHint, "The password to use if the MQTT broker uses authentication.");
  add_hint(m, Message::kClientID, GUITypes::HintKey::DescriptionHint, "The client id to use to identify your connection with the MQTT broker.");

  return m;
}

void EnterMQTTGroupCompute::on_finished_connect_task() {
  internal();
  clean_finalize();
}

bool EnterMQTTGroupCompute::update_state() {
  internal();
  EnterGroupCompute::update_state();

  // Otherwise queue up the connect task.
  TaskContext tc(_scheduler);
  queue_connect_task(tc);

  // Queue up to receive a callback when the above tasks are done.
  std::function<void()> done = std::bind(&EnterMQTTGroupCompute::on_finished_connect_task,this);
  queue_finished_task(tc, done);
  return false;
}

EnterMQTTGroupCompute::Config EnterMQTTGroupCompute::get_config() const {
  external();
  QJsonObject inputs = _inputs->get_input_values();

  EnterMQTTGroupCompute::Config config;
  QString address = inputs.value(Message::kHostAddress).toString();
  config.host_address = QHostAddress(address);
  config.host_name = inputs.value(Message::kHostName).toString();
  config.port = inputs.value(Message::kPort).toInt();
  config.username = inputs.value(Message::kUsername).toString();
  config.password = inputs.value(Message::kPassword).toString();
  config.client_id = inputs.value(Message::kClientID).toString();
  return config;
}

bool EnterMQTTGroupCompute::destroy_state() {
  // Note we can't launch any computes that require multiple cleaning passes.
  // We can only use cached values here to launch one task, and we can't wait for it.
  internal();
  return true;
}

bool EnterMQTTGroupCompute::is_connected() const{
  external();
  return _client->isConnectedToHost();
}

bool EnterMQTTGroupCompute::is_subscribed(const Compute* compute, const std::string& topic) const {
  external();
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


// ------------------------------------------------------------------------------------------
// Queue Tasks.
// ------------------------------------------------------------------------------------------

void EnterMQTTGroupCompute::queue_connect_task(TaskContext& tc) {
  _scheduler->queue_task(tc, (Task) std::bind(&EnterMQTTGroupCompute::connect_task, this), "queue_connect_task");
}

void EnterMQTTGroupCompute::queue_publish_task(TaskContext& tc, const QString& topic, const QString& message) {
  _scheduler->queue_task(tc, (Task) std::bind(&EnterMQTTGroupCompute::publish_task, this, topic, message), "queue_publish_task");
}

void EnterMQTTGroupCompute::queue_subscribe_task(TaskContext& tc, const QString& topic, const Path& node_path) {
  _scheduler->queue_task(tc, (Task) std::bind(&EnterMQTTGroupCompute::subscribe_task, this, topic, node_path), "queue_subscribe_task");
}

void EnterMQTTGroupCompute::queue_unsubscribe_task(TaskContext& tc, const QString& topic, const Path& node_path) {
  _scheduler->queue_task(tc, (Task) std::bind(&EnterMQTTGroupCompute::unsubscribe_task, this, topic, node_path), "queue_send_delete_request");
}

void EnterMQTTGroupCompute::queue_finished_task(TaskContext& tc, std::function<void()> done) {
  _scheduler->queue_task(tc, (Task)std::bind(&EnterMQTTGroupCompute::finished_compute_task, this, done), "queue_finished_compute");
}

// ------------------------------------------------------------------------------------------
// Slots.
// ------------------------------------------------------------------------------------------

void EnterMQTTGroupCompute::on_time_out() {
  internal();
  _timer.stop();
  QString error = "Unable to connect to MQTT broker. Make sure the host address, username or password is correct on the group.";
  _scheduler->done_waiting_for_response(_current_task_id, error);
}

void EnterMQTTGroupCompute::on_connected() {
  internal();
  _timer.stop();
  std::cerr << "MQTT on connected\n";
  QString error;
  _scheduler->done_waiting_for_response(_current_task_id, error);
}

void EnterMQTTGroupCompute::on_disconnected() {
  external(); // We mark ourself dirty as we're responsible for keeping the connection open with the mqtt broker.
  _timer.stop();
  QString error = "Disconnected from MQTT broker. Make sure the host address, username or password is correct on the group.";
  _scheduler->done_waiting_for_response(_current_task_id, error);
}

void EnterMQTTGroupCompute::on_published(const QMQTT::Message& message) {
  internal();
  QString error;
  _scheduler->done_waiting_for_response(_current_task_id, error);
}

void EnterMQTTGroupCompute::on_subscribed(const QString& topic) {
  internal();
  QString error;
  _scheduler->done_waiting_for_response(_current_task_id, error);
}

void EnterMQTTGroupCompute::on_unsubscribed(const QString& topic) {
  internal();
  QString error;
  _scheduler->done_waiting_for_response(_current_task_id, error);
}

void EnterMQTTGroupCompute::on_received(const QMQTT::Message& message) {
  internal();
  // Merge the payload of the message into the chain state.
  QString payload = QString::fromUtf8(message.payload());
  QString topic = message.topic();
  std::cerr << "received message from mqtt broker\n";
  std::cerr << "topic: " << topic.toStdString() << "\n";
  std::cerr << "payloc: " << payload.toStdString() << "\n";
  if (_subscribers.count(topic.toStdString())) {
    const std::unordered_set<Path, PathHasher>& paths = _subscribers.at(topic.toStdString());
    std::unordered_set<Path, PathHasher>::const_iterator iter;
    for (iter = paths.begin(); iter != paths.end(); ++iter) {
      const Path& node_path = *iter;
      _manipulator->set_mqtt_override(node_path, topic, payload);
    }
  }
}

// ------------------------------------------------------------------------------------------
// Tasks.
// ------------------------------------------------------------------------------------------

void EnterMQTTGroupCompute::finished_compute_task(std::function<void()> done) {
  internal();
  done();
  _scheduler->run_next_task();
}

void EnterMQTTGroupCompute::connect_task() {
  internal();
  EnterMQTTGroupCompute::Config config = get_config();
  _client->setHostName(config.host_name);
  _client->setHost(config.host_address);
  _client->setPort(config.port);
  _client->setUsername(config.username);
  _client->setPassword(config.password);
  _client->setClientId(config.client_id);

  if (!_client->isConnectedToHost()) {
    // If we're not already connected, we connect to the mqtt broker here.
    _current_task_id = _scheduler->wait_for_response();
    _client->connectToHost();
    // Start timer.
    _timer.start();
  } else {
    // Otherwise we can run the next task.
    _scheduler->run_next_task();
  }
}

void EnterMQTTGroupCompute::publish_task(const QString& topic, const QString& message) {
  internal();
  _current_task_id = _scheduler->wait_for_response();
  QMQTT::Message msg(_current_task_id, topic, message.toUtf8());
  _client->publish(msg);
}

void EnterMQTTGroupCompute::subscribe_task(const QString& topic, const Path& node_path) {
  internal();
  bool first = (_subscribers.count(topic.toStdString()) == 0);
  _subscribers[topic.toStdString()].insert(node_path);
  if (first) {
    _current_task_id = _scheduler->wait_for_response();
    _client->subscribe(topic, 0);
  } else {
    _scheduler->run_next_task();
  }
}

void EnterMQTTGroupCompute::unsubscribe_task(const QString& topic, const Path& node_path) {
  internal();
  // Remove the path from the subscribers for this topic..
  _subscribers[topic.toStdString()].erase(node_path);
  // If there no more paths for this topic, then unsubscribe from the topic.
  bool empty = _subscribers[topic.toStdString()].empty();
  if (empty) {
    std::cerr << "Actually unsubscribing now !!!!!!!!!!!!!!!\n";
    _subscribers.erase(topic.toStdString());
    _current_task_id = _scheduler->wait_for_response();
    _client->unsubscribe(topic);
  } else {
    std::cerr << "NNNNNNNNNNot unsubscribing as there are other node listening \n";
    _scheduler->run_next_task();
  }
}

}
