#include <guicomponents/computes/mqttgrouplock.h>
#include <base/objectmodel/dep.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/inputs.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/message.h>



namespace ngs {

MQTTGroupLock::MQTTGroupLock(Entity* entity)
    : GroupLock(entity, kDID()),
      _scheduler(this),
      _worker(this),
      _lock(false){
  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
  get_dep_loader()->register_fixed_dep(_worker, Path({}));
}

MQTTGroupLock::~MQTTGroupLock() {
  _worker->unregister_group_lock(our_entity()->get_path());
}

void MQTTGroupLock::create_inputs_outputs() {
  external();
  GroupLock::create_inputs_outputs();
  create_input(Message::kHostAddress, "127.0.0.1", false);
  create_input(Message::kPort, 1883, false);
  create_input(Message::kUsername, "", false);
  create_input(Message::kPassword, "", false);
}

const QJsonObject MQTTGroupLock::_hints = MQTTGroupLock::init_hints();
QJsonObject MQTTGroupLock::init_hints() {
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

bool MQTTGroupLock::get_lock_setting() const{
  external();
  return _lock;
}

void MQTTGroupLock::set_lock_setting(bool lock) {
  external();
  _lock = lock;
}

bool MQTTGroupLock::update_state() {
  internal();
  GroupLock::update_state();

  std::cerr << "MQTTGroupLock::update_state \n";

  MQTTWorker::Config config = get_inputs();
  if (!_lock) {
    if (_worker->is_connected(config)) {
      std::cerr << "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC connected to mqtt broker !!!!!!!!!!!\n";
      return true;
    }

    // Register ourself for mqtt disconnects.
    _worker->register_group_lock(config, our_entity()->get_path());

    // Otherwise we start an asynchronous task.
    TaskContext tc(_scheduler);
    _worker->queue_connect_task(tc, config);
    return false;
  } else {
    if (!_worker->is_connected(config)) {
      return true;
    }
    // Otherwise we usually disconnect, but with mqtt groups we may have listeners attached,
    // so we don't disconnect.
    return true;
  }
}

MQTTWorker::Config MQTTGroupLock::get_inputs() const {
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
