#include <guicomponents/computes/enterfirebasegroupcompute.h>
#include <base/objectmodel/dep.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/inputs.h>
#include <guicomponents/comms/guitypes.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/computes/taskqueuer.h>

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
  get_dep_loader()->register_fixed_dep(_scheduler, Path());
  get_dep_loader()->register_fixed_dep(_worker, Path());
}

EnterFirebaseGroupCompute::~EnterFirebaseGroupCompute() {
}

void EnterFirebaseGroupCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  EnterGroupCompute::create_inputs_outputs(config);

  EntityConfig c = config;
  c.visible = false;
  c.expose_plug = false;
  c.unconnected_value = "AIzaSyCXGNlyRf5uk8Xk1bvKXUcA53TC6Lc3I-A";
  create_input(Message::kApiKey, c);
  c.unconnected_value = "test-project-91c10.firebaseapp.com";
  create_input(Message::kAuthDomain, c);
  c.unconnected_value = "https://test-project-91c10.firebaseio.com/";
  create_input(Message::kDatabaseURL, c);
  c.unconnected_value = "gs://test-project-91c10.appspot.com";
  create_input(Message::kStorageBucket, c);
  c.unconnected_value = "your_email@some_place.com";
  create_input(Message::kEmail, c);
  c.unconnected_value = "your_password";
  create_input(Message::kPassword, c);
}

const QJsonObject EnterFirebaseGroupCompute::_hints = EnterFirebaseGroupCompute::init_hints();
QJsonObject EnterFirebaseGroupCompute::init_hints() {
  QJsonObject m;

  add_hint(m, Message::kApiKey, GUITypes::HintKey::DescriptionHint, "The Firebase api key.");
  add_hint(m, Message::kAuthDomain, GUITypes::HintKey::DescriptionHint, "The Firebase authentication domain.");
  add_hint(m, Message::kDatabaseURL, GUITypes::HintKey::DescriptionHint, "The Firebase realtime database url.");
  add_hint(m, Message::kStorageBucket, GUITypes::HintKey::DescriptionHint, "The Firebase storage bucket url.");
  add_hint(m, Message::kEmail, GUITypes::HintKey::DescriptionHint, "The email to sign in to Firebase. Please enable email/password authentication on your database.");
  add_hint(m, Message::kPassword, GUITypes::HintKey::DescriptionHint, "The password to sign in to Firebase. Please enable email/password authentication on your database.");

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
