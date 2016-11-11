#include <guicomponents/computes/mqttgrouplock.h>
#include <base/objectmodel/dep.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/inputs.h>
#include <guicomponents/comms/mqttworker.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/message.h>

#include <QtNetwork/QHostAddress>
#include <QtCore/QString>

namespace ngs {

MQTTGroupLock::MQTTGroupLock(Entity* entity)
    : GroupLock(entity, kIID(), kDID()),
      _scheduler(this),
      _worker(this),
      _lock(true){
  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
  get_dep_loader()->register_fixed_dep(_worker, Path({}));
}

MQTTGroupLock::~MQTTGroupLock() {
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
  QHostAddress host_address; int port; QString username; QString password;
  get_inputs(host_address, port, username, password);
  if (!_lock) {
    if (_worker->is_connected(host_address, port, username, password)) {
      return true;
    }
    // Otherwise we start an asynchronous task.
    TaskContext tc(_scheduler);
    _worker->queue_connect_task(tc, host_address, port, username, password);
    return false;
  } else {
    if (!_worker->is_connected(host_address, port, username, password)) {
      return true;
    }
    // Otherwise we usually disconnect, but with mqtt groups we may have listeners attached,
    // so we don't disconnect.
    return true;
  }
}

void MQTTGroupLock::get_inputs(QHostAddress& host_address, int& port, QString& username, QString& password) const {
  external();
  QJsonObject inputs = _inputs->get_input_values();

  QString address = inputs.value(Message::kHostAddress).toString();
  host_address = QHostAddress(address);
  port = inputs.value(Message::kPort).toInt();
  username = inputs.value(Message::kUsername).toString();
  password = inputs.value(Message::kPassword).toString();
}


}
