#include <guicomponents/computes/firebasegrouplock.h>
#include <base/objectmodel/dep.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/inputs.h>
#include <guicomponents/comms/mqttworker.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/message.h>

#include <QtNetwork/QHostAddress>
#include <QtCore/QString>

namespace ngs {

FirebaseGroupLock::FirebaseGroupLock(Entity* entity)
    : GroupLock(entity, kIID(), kDID()),
      _scheduler(this),
      _worker(this),
      _lock(true){
  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
  get_dep_loader()->register_fixed_dep(_worker, Path({}));
}

FirebaseGroupLock::~FirebaseGroupLock() {
}

bool FirebaseGroupLock::get_lock_setting() const{
  external();
  return _lock;
}

void FirebaseGroupLock::set_lock_setting(bool lock) {
  external();
  _lock = lock;
}

bool FirebaseGroupLock::update_state() {
//  internal();
//  InputValues values;
//  get_inputs(values);
//  if (!_lock) {
//    if (_worker->is_(host_address, port, username, password)) {
//      return true;
//    }
//    // Otherwise we start an asynchronous task.
//    TaskContext tc(_scheduler);
//    _worker->queue_connect_task(tc, host_address, port, username, password);
//    return false;
//  } else {
//    if (!_worker->is_connected(host_address, port, username, password)) {
//      return true;
//    }
//    // Otherwise we usually disconnect, but with mqtt groups we may have listeners attached,
//    // so we don't disconnect.
//    return true;
//  }
}

void FirebaseGroupLock::get_inputs(InputValues& values) const {
  external();
  QJsonObject inputs = _inputs->get_input_values();

  values.apiKey = inputs.value(Message::kApiKey).toString();
  values.authDomain = inputs.value(Message::kAuthDomain).toString();
  values.databaseURL = inputs.value(Message::kDatabaseURL).toString();
  values.storageBucket = inputs.value(Message::kStorageBucket).toString();

  values.email = inputs.value(Message::kEmail).toString();
  values.password = inputs.value(Message::kPassword).toString();
}





}
