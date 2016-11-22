#include <guicomponents/computes/enterfirebasegroupcompute.h>
#include <base/objectmodel/dep.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/inputs.h>
#include <guicomponents/computes/nodejsworker.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/comms/commtypes.h>

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
      _worker(this) {
  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
  get_dep_loader()->register_fixed_dep(_worker, Path({}));
}

EnterFirebaseGroupCompute::~EnterFirebaseGroupCompute() {
}

void EnterFirebaseGroupCompute::create_inputs_outputs() {
  external();
  EnterGroupCompute::create_inputs_outputs();
  create_input(Message::kApiKey, "AIzaSyCXGNlyRf5uk8Xk1bvKXUcA53TC6Lc3I-A", true);
  create_input(Message::kAuthDomain, "test-project-91c10.firebaseapp.com", true);
  create_input(Message::kDatabaseURL, "https://test-project-91c10.firebaseio.com/", true);
  create_input(Message::kStorageBucket, "gs://test-project-91c10.appspot.com", true);
  create_input(Message::kEmail, "your_email@some_place.com", true);
  create_input(Message::kPassword, "your_password", true);
}

const QJsonObject EnterFirebaseGroupCompute::_hints = EnterFirebaseGroupCompute::init_hints();
QJsonObject EnterFirebaseGroupCompute::init_hints() {
  QJsonObject m;

  add_hint(m, Message::kApiKey, HintKey::kJSTypeHint, to_underlying(JSType::kString));
  add_hint(m, Message::kApiKey, HintKey::kDescriptionHint, "The Firebase api key.");

  add_hint(m, Message::kAuthDomain, HintKey::kJSTypeHint, to_underlying(JSType::kString));
  add_hint(m, Message::kAuthDomain, HintKey::kDescriptionHint, "The Firebase authentication domain.");

  add_hint(m, Message::kDatabaseURL, HintKey::kJSTypeHint, to_underlying(JSType::kString));
  add_hint(m, Message::kDatabaseURL, HintKey::kDescriptionHint, "The Firebase realtime database url.");

  add_hint(m, Message::kStorageBucket, HintKey::kJSTypeHint, to_underlying(JSType::kString));
  add_hint(m, Message::kStorageBucket, HintKey::kDescriptionHint, "The Firebase storage bucket url.");

  add_hint(m, Message::kEmail, HintKey::kJSTypeHint, to_underlying(JSType::kString));
  add_hint(m, Message::kEmail, HintKey::kDescriptionHint, "The email to sign in to Firebase. You must enable email/password authentication on your database.");

  add_hint(m, Message::kPassword, HintKey::kJSTypeHint, to_underlying(JSType::kString));
  add_hint(m, Message::kPassword, HintKey::kDescriptionHint, "The password to sign in to Firebase. You must enable email/password authentication on your database.");

  return m;
}

bool EnterFirebaseGroupCompute::update_state() {
  internal();
  EnterGroupCompute::update_state();
  queue_sign_in();
  return false;
}

bool EnterFirebaseGroupCompute::destroy_state() {
  internal();
  queue_destroy();
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

void EnterFirebaseGroupCompute::queue_sign_in() {
  InputValues values = get_inputs();
  QString node_path = get_path_as_string().c_str();
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
    args.insert(Message::kNodePath, node_path);
    _worker->queue_merge_chain_state(tc, args);
    _worker->queue_firebase_init(tc);
  }
  // Sign into a firebase account.
  {
    QJsonObject args;
    args.insert(Message::kEmail, values.email);
    args.insert(Message::kPassword, values.password);
    args.insert(Message::kNodePath, node_path);
    _worker->queue_merge_chain_state(tc, args);
    _worker->queue_firebase_sign_in(tc);
  }
  // Grab result.
  {
    std::function<void(const QJsonObject&)> callback = std::bind(&EnterFirebaseGroupCompute::receive_chain_state, this, std::placeholders::_1);
    _worker->queue_receive_chain_state(tc, callback);
  }
}

void EnterFirebaseGroupCompute::queue_destroy() {
  QJsonObject args;
  QString node_path = get_path_as_string().c_str();
  args.insert(Message::kNodePath, node_path);
  TaskContext tc(_scheduler);
  _worker->queue_merge_chain_state(tc, args);
  _worker->queue_firebase_destroy(tc);
}

void EnterFirebaseGroupCompute::receive_chain_state(const QJsonObject& chain_state) {
  internal();
  bool signed_in = chain_state.value("value").toBool();
  if (signed_in) {
    clean_finalize();
  }
}


}
