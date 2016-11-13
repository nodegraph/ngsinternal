#include <guicomponents/computes/entermqttgroupcompute.h>
#include <base/objectmodel/dep.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/inputs.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/message.h>



namespace ngs {

EnterMQTTGroupCompute::EnterMQTTGroupCompute(Entity* entity)
    : EnterGroupCompute(entity, kDID()),
      _scheduler(this),
      _worker(this) {
  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
  get_dep_loader()->register_fixed_dep(_worker, Path({}));
}

EnterMQTTGroupCompute::~EnterMQTTGroupCompute() {
  _worker->unregister_group_lock(get_path());
}

void EnterMQTTGroupCompute::create_inputs_outputs() {
  external();
  EnterGroupCompute::create_inputs_outputs();
  create_input(Message::kHostAddress, "127.0.0.1", false);
  create_input(Message::kPort, 1883, false);
  create_input(Message::kUsername, "", false);
  create_input(Message::kPassword, "", false);
}

const QJsonObject EnterMQTTGroupCompute::_hints = EnterMQTTGroupCompute::init_hints();
QJsonObject EnterMQTTGroupCompute::init_hints() {
  QJsonObject m;

  add_hint(m, Message::kHostAddress, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kHostAddress, HintType::kDescription, "The ip address of the MQTT broker.");

  add_hint(m, Message::kPort, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kPort, HintType::kDescription, "The port number of the MQTT broker.");

  add_hint(m, Message::kUsername, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kUsername, HintType::kDescription, "The username to use if the MQTT broker uses authentication.");

  add_hint(m, Message::kPassword, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kPassword, HintType::kDescription, "The password to use if the MQTT broker uses authentication.");

  return m;
}

void EnterMQTTGroupCompute::on_finished_connect_task() {
  internal();
}

bool EnterMQTTGroupCompute::update_state() {
  internal();
  EnterGroupCompute::update_state();

  MQTTWorker::Config config = get_inputs();
  if (_worker->is_connected(config)) {
    return true;
  }

  // Register ourself for mqtt disconnects.
  _worker->register_group_lock(config, get_path());

  // Otherwise queue up the connect task.
  TaskContext tc(_scheduler);
  _worker->queue_connect_task(tc, config);

  // Queue up to receive a callback when the above tasks are done.
  std::function<void()> done = std::bind(&EnterMQTTGroupCompute::on_finished_connect_task,this);
  _worker->queue_finished_task(tc, done);
  return false;
}

MQTTWorker::Config EnterMQTTGroupCompute::get_inputs() const {
  external();
  QJsonObject inputs = _inputs->get_input_values();

  MQTTWorker::Config config;
  QString address = inputs.value(Message::kHostAddress).toString();
  config.host_address = QHostAddress(address);
  config.port = inputs.value(Message::kPort).toInt();
  config.username = inputs.value(Message::kUsername).toString();
  config.password = inputs.value(Message::kPassword).toString();
  return config;
}

}
