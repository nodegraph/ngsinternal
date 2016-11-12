#include <guicomponents/computes/enterfirebasegroupcompute.h>
#include <base/objectmodel/dep.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/inputs.h>
#include <guicomponents/comms/browserworker.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/message.h>

#include <QtNetwork/QHostAddress>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QMap>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>


namespace ngs {

EnterFirebaseGroupCompute::EnterFirebaseGroupCompute(Entity* entity)
    : EnterGroupCompute(entity, kDID()),
      _scheduler(this),
      _worker(this),
      _lock(true),
      _current(true){
  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
  get_dep_loader()->register_fixed_dep(_worker, Path({}));
}

EnterFirebaseGroupCompute::~EnterFirebaseGroupCompute() {
}

bool EnterFirebaseGroupCompute::get_lock_setting() const{
  external();
  return _lock;
}

void EnterFirebaseGroupCompute::set_lock_setting(bool lock) {
  external();
  _lock = lock;
}

void EnterFirebaseGroupCompute::set_self_dirty(bool dirty) {
  EnterGroupCompute::set_self_dirty(dirty);
  // Whenever we become dirty, we dirty ourself.
  if (dirty) {
    _lock = true;
  }
}

bool EnterFirebaseGroupCompute::update_state() {
  internal();
  if (!_lock && _current) {
    queue_unlock();
    return false;
  } else if (_lock && !_current){
    // Otherwise we usually disconnect, but with mqtt groups we may have listeners attached,
    // so we don't disconnect.
    return true;
  }
  // Otherwise our _lock and _current states already match up.
  return true;
}



EnterFirebaseGroupCompute::InputValues EnterFirebaseGroupCompute::get_inputs() const {
  external();
  QJsonObject inputs = _inputs->get_input_values();

  InputValues values;
  values.api_key = inputs.value(Message::kApiKey).toString();
  values.auth_domain = inputs.value(Message::kAuthDomain).toString();
  values.database_url = inputs.value(Message::kDatabaseURL).toString();
  values.storage_bucket = inputs.value(Message::kStorageBucket).toString();

  values.email = inputs.value(Message::kEmail).toString();
  values.password = inputs.value(Message::kPassword).toString();
  return values;
}

void EnterFirebaseGroupCompute::queue_unlock() {
  InputValues values = get_inputs();
  TaskContext tc(_scheduler);
  // Initialize the firebase wrapper.
  {
    QJsonObject args;
    args.insert(Message::kApiKey, values.api_key);
    args.insert(Message::kAuthDomain, values.auth_domain);
    args.insert(Message::kDatabaseURL, values.database_url);
    args.insert(Message::kStorageBucket, values.storage_bucket);
    args.insert(Message::kEmail, values.email);
    args.insert(Message::kPassword, values.password);
    _worker->queue_merge_chain_state(tc, args);
    _worker->queue_firebase_init(tc);
  }
  // Sign into a firebase account.
  {
    QJsonObject args;
    args.insert(Message::kEmail, values.email);
    args.insert(Message::kPassword, values.password);
    _worker->queue_merge_chain_state(tc, args);
    _worker->queue_firebase_sign_in(tc);
  }
  // Grab result.
  {
    std::function<void(const QJsonObject&)> callback = std::bind(&EnterFirebaseGroupCompute::receive_chain_state, this, std::placeholders::_1);
    _worker->queue_receive_chain_state(tc, callback);
  }
}

void EnterFirebaseGroupCompute::receive_chain_state(const QJsonObject& chain_state) {
  _current = chain_state.value("value").toBool();
  std::cerr << "FirebaseGroup is now with locked state: " << _current << "\n";
}


}
