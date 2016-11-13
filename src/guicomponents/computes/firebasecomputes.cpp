#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>
#include <guicomponents/comms/nodejsworker.h>
#include <guicomponents/comms/commutils.h>

#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/computes/firebasecomputes.h>
#include <guicomponents/computes/enterfirebasegroupcompute.h>

#include <functional>

namespace ngs {

//--------------------------------------------------------------------------------

FirebaseCompute::FirebaseCompute(Entity* entity, ComponentDID did)
    : Compute(entity, did),
      _worker(this),
      _scheduler(this),
      _enter(this) {
  get_dep_loader()->register_fixed_dep(_worker, Path({}));
  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
  get_dep_loader()->register_fixed_dep(_enter, Path({"..","group_context"}));
}

FirebaseCompute::~FirebaseCompute() {
}

void FirebaseCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("in", QJsonObject());
  create_output("out");
}

void FirebaseCompute::init_hints(QJsonObject& m) {
  add_hint(m, "in", HintType::kJSType, to_underlying(JSType::kObject));
  add_hint(m, "in", HintType::kDescription, "The main object that flows through this node. This cannot be set manually.");
}

void FirebaseCompute::dump_map(const QJsonObject& inputs) const {
  for(QJsonObject::const_iterator iter = inputs.begin(); iter != inputs.end(); ++iter) {
    std::cerr << iter.key().toStdString() << " : " << iter.value().toString().toStdString() << "\n";
  }
}

void FirebaseCompute::pre_update_state(TaskContext& tc) {
  internal();
  QJsonObject inputs = _inputs->get_input_values();
  _worker->queue_merge_chain_state(tc, inputs);
  // Make sure nothing is loading right now.
  // Note in general a page may start loading content at random times.
  // For examples ads may rotate and flip content.
  _worker->queue_wait_until_loaded(tc);
}

void FirebaseCompute::post_update_state(TaskContext& tc) {
  internal();
  std::function<void(const QJsonObject&)> callback = std::bind(&FirebaseCompute::receive_chain_state,this,std::placeholders::_1);
  _worker->queue_receive_chain_state(tc, callback);
}

void FirebaseCompute::receive_chain_state(const QJsonObject& chain_state) {
  internal();
  clean_finalize();

  // This copies the incoming data, to our output.
  // Derived classes will in add in extra data, extracted from the web.
  QJsonValue incoming = _inputs->get_input_value("in");
  set_output("out", incoming);
}

// -------------------------------------------------------------------------------------------------------------------------
// Firebase
// -------------------------------------------------------------------------------------------------------------------------

void FirebaseSignInCompute::create_inputs_outputs() {
  external();
  FirebaseCompute::create_inputs_outputs();
  create_input(Message::kEmail, "user@user.com", false);
  create_input(Message::kPassword, "password", false);
}

const QJsonObject FirebaseSignInCompute::_hints = FirebaseSignInCompute::init_hints();
QJsonObject FirebaseSignInCompute::init_hints() {
  QJsonObject m;
  FirebaseCompute::init_hints(m);

  add_hint(m, Message::kEmail, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kEmail, HintType::kDescription, "Email address for firebase authentication.");

  add_hint(m, Message::kPassword, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kPassword, HintType::kDescription, "Password for firebase authentication.");

  return m;
}

bool FirebaseSignInCompute::update_state() {
  internal();
  FirebaseCompute::update_state();

  TaskContext tc(_scheduler);
  //FirebaseCompute::pre_update_state(tc);
  QJsonObject inputs = _inputs->get_input_values();
  _worker->queue_merge_chain_state(tc, inputs);

  _worker->queue_firebase_sign_in(tc);
  FirebaseCompute::post_update_state(tc);
  return false;
}

bool FirebaseSignOutCompute::update_state() {
  internal();
  FirebaseCompute::update_state();

  TaskContext tc(_scheduler);
  //FirebaseCompute::pre_update_state(tc);
  QJsonObject inputs = _inputs->get_input_values();
  _worker->queue_merge_chain_state(tc, inputs);

  _worker->queue_firebase_sign_out(tc);
  FirebaseCompute::post_update_state(tc);
  return false;
}

void FirebaseWriteDataCompute::create_inputs_outputs() {
  external();
  FirebaseCompute::create_inputs_outputs();
  create_input(Message::kPath, "some/path", false);
  create_input(Message::kValue, "hello there", false);
}

const QJsonObject FirebaseWriteDataCompute::_hints = FirebaseWriteDataCompute::init_hints();
QJsonObject FirebaseWriteDataCompute::init_hints() {
  QJsonObject m;
  FirebaseCompute::init_hints(m);

  add_hint(m, Message::kPath, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kPath, HintType::kDescription, "Firebase data path.");

  add_hint(m, Message::kValue, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kValue, HintType::kDescription, "The data to write at the path.");

  return m;
}

bool FirebaseWriteDataCompute::update_state() {
  internal();
  FirebaseCompute::update_state();

  TaskContext tc(_scheduler);
  //FirebaseCompute::pre_update_state(tc);
  QJsonObject inputs = _inputs->get_input_values();
  _worker->queue_merge_chain_state(tc, inputs);

  _worker->queue_firebase_write_data(tc);
  FirebaseCompute::post_update_state(tc);
  return false;
}

void FirebaseReadDataCompute::create_inputs_outputs() {
  external();
  FirebaseCompute::create_inputs_outputs();
  create_input(Message::kPath, "some/path", false);
  create_input(Message::kDataName, "data", false);
}

const QJsonObject FirebaseReadDataCompute::_hints = FirebaseReadDataCompute::init_hints();
QJsonObject FirebaseReadDataCompute::init_hints() {
  QJsonObject m;
  FirebaseCompute::init_hints(m);

  add_hint(m, Message::kPath, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kPath, HintType::kDescription, "Firebase data path.");

  add_hint(m, Message::kDataName, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kDataName, HintType::kDescription, "The name given to the data read from Firebase. This will be merged into the data flowing through \"in\".");

  return m;
}

void FirebaseReadDataCompute::receive_chain_state(const QJsonObject& chain_state) {
  internal();
  clean_finalize();

  // This copies the incoming data, to our output.
  // Derived classes will in add in extra data, extracted from the web.
  QJsonValue incoming = _inputs->get_input_value("in");
  QString data_name = _inputs->get_input_value(Message::kDataName).toString();
  if (incoming.isObject()) {
    QJsonObject obj = incoming.toObject();
    obj.insert(data_name, chain_state.value("value"));
    set_output("out", obj);
  } else {
    set_output("out", incoming);
  }
}

bool FirebaseReadDataCompute::update_state() {
  internal();
  FirebaseCompute::update_state();

  TaskContext tc(_scheduler);
  //FirebaseCompute::pre_update_state(tc);
  QJsonObject inputs = _inputs->get_input_values();
  _worker->queue_merge_chain_state(tc, inputs);

  _worker->queue_firebase_read_data(tc);
  FirebaseCompute::post_update_state(tc);
  return false;
}

void FirebaseListenToChangesCompute::create_inputs_outputs() {
  external();
  FirebaseCompute::create_inputs_outputs();
  create_input(Message::kPath, "some/path", false);
}

const QJsonObject FirebaseListenToChangesCompute::_hints = FirebaseListenToChangesCompute::init_hints();
QJsonObject FirebaseListenToChangesCompute::init_hints() {
  QJsonObject m;
  FirebaseCompute::init_hints(m);

  add_hint(m, Message::kPath, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kPath, HintType::kDescription, "Firebase data path.");

  return m;
}

bool FirebaseListenToChangesCompute::update_state() {
  internal();
  FirebaseCompute::update_state();

  TaskContext tc(_scheduler);
  //FirebaseCompute::pre_update_state(tc);
  QJsonObject inputs = _inputs->get_input_values();
  //Add our path.
  QStringList list = path_to_string_list(our_entity()->get_path());
  QJsonArray arr = QJsonArray::fromStringList(list);
  inputs.insert("node_path", arr);
  _worker->queue_merge_chain_state(tc, inputs);

  _worker->queue_firebase_listen_to_changes(tc);
  FirebaseCompute::post_update_state(tc);
  return false;
}

}
