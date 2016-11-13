#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>
#include <guicomponents/computes/nodejsworker.h>

#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/computes/firebasecomputes.h>
#include <guicomponents/computes/enterfirebasegroupcompute.h>

#include <functional>

namespace ngs {

// -------------------------------------------------------------------------------------------------------------------------

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

void FirebaseCompute::prepend_tasks(TaskContext& tc) {
  internal();
  QJsonObject inputs = _inputs->get_input_values();

  // Add our path.
  QString node_path = get_path_as_string().c_str();
  inputs.insert(Message::kNodePath, node_path);

  // Queue up a merge chain state task.
  _worker->queue_merge_chain_state(tc, inputs);
}

void FirebaseCompute::append_tasks(TaskContext& tc) {
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

void FirebaseWriteDataCompute::create_inputs_outputs() {
  external();
  FirebaseCompute::create_inputs_outputs();
  create_input(Message::kDataPath, "some/path", false);
  create_input(Message::kValue, "hello there", false);
}

const QJsonObject FirebaseWriteDataCompute::_hints = FirebaseWriteDataCompute::init_hints();
QJsonObject FirebaseWriteDataCompute::init_hints() {
  QJsonObject m;
  FirebaseCompute::init_hints(m);

  add_hint(m, Message::kDataPath, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kDataPath, HintType::kDescription, "Firebase data path.");

  add_hint(m, Message::kValue, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kValue, HintType::kDescription, "The data to write at the path.");

  return m;
}

bool FirebaseWriteDataCompute::update_state() {
  internal();
  FirebaseCompute::update_state();

  TaskContext tc(_scheduler);
  FirebaseCompute::prepend_tasks(tc);
  _worker->queue_firebase_write_data(tc);
  FirebaseCompute::append_tasks(tc);
  return false;
}

// -------------------------------------------------------------------------------------------------------------------------

void FirebaseReadDataCompute::create_inputs_outputs() {
  external();
  FirebaseCompute::create_inputs_outputs();
  create_input(Message::kDataPath, "some/path", false);
  create_input(Message::kDataName, "data", false);
  create_input(Message::kListenForChanges, true, false);
}

const QJsonObject FirebaseReadDataCompute::_hints = FirebaseReadDataCompute::init_hints();
QJsonObject FirebaseReadDataCompute::init_hints() {
  QJsonObject m;
  FirebaseCompute::init_hints(m);

  add_hint(m, Message::kDataPath, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kDataPath, HintType::kDescription, "Firebase data path.");

  add_hint(m, Message::kDataName, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kDataName, HintType::kDescription, "The name given to the data read from Firebase. This will be merged into the data flowing through \"in\".");

  add_hint(m, Message::kListenForChanges, HintType::kJSType, to_underlying(JSType::kBoolean));
  add_hint(m, Message::kListenForChanges, HintType::kDescription, "When enabled, this node will be updated when there are any changes to the value.");

  return m;
}

void FirebaseReadDataCompute::set_override(const QString& data_path, const QJsonValue& value) {
  external();
  if (_data_path != data_path) {
    return;
  }
  _dummy_override = value;
}

const QJsonValue& FirebaseReadDataCompute::get_override() const {
  external();
  return _dummy_override;
}

void FirebaseReadDataCompute::clear_override() {
  internal();
  _dummy_override = QJsonValue();
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

  // Cache the data path value.
  _data_path = _inputs->get_input_value(Message::kDataPath).toString();

  // Perform our compute.
  TaskContext tc(_scheduler);
  FirebaseCompute::prepend_tasks(tc);
  _worker->queue_firebase_subscribe(tc);
  _worker->queue_firebase_read_data(tc);
  FirebaseCompute::append_tasks(tc);
  return false;
}

bool FirebaseReadDataCompute::destroy_state() {
  internal();
  // Note we can't launch any computes that require multiple cleaning passes.
  // We can only use cached values here to launch one task, and we can't wait for it.

  QJsonObject inputs;

  // Add our path.
  QString node_path = get_path_as_string().c_str();
  inputs.insert(Message::kNodePath, node_path);

  // Add our data path.
  inputs.insert(Message::kDataPath, _data_path);

  // Queue up a merge chain state task.
  TaskContext tc(_scheduler);
  _worker->queue_merge_chain_state(tc, inputs);
  _worker->queue_firebase_unsubscribe(tc);
  return true;
}

// -------------------------------------------------------------------------------------------------------------------------

}
