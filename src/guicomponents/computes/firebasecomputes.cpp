#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>
#include <guicomponents/computes/nodejsworker.h>

#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/computes/firebasecomputes.h>
#include <guicomponents/computes/enterfirebasegroupcompute.h>
#include <guicomponents/comms/commtypes.h>

#include <functional>

namespace ngs {

// -------------------------------------------------------------------------------------------------------------------------

FirebaseCompute::FirebaseCompute(Entity* entity, ComponentDID did)
    : Compute(entity, did),
      _worker(this),
      _scheduler(this),
      _enter(this) {
  get_dep_loader()->register_fixed_dep(_worker, Path());
  get_dep_loader()->register_fixed_dep(_scheduler, Path());
}

FirebaseCompute::~FirebaseCompute() {
}

void FirebaseCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);

  EntityConfig c = config;
  c.expose_plug = true;
  c.unconnected_value = QJsonObject();

  create_input("in", c);
  create_output("out", c);
}

void FirebaseCompute::init_hints(QJsonObject& m) {
  add_hint(m, "in", HintKey::kDescriptionHint, "The main object that flows through this node. This cannot be set manually.");
}

void FirebaseCompute::dump_map(const QJsonObject& inputs) const {
  for(QJsonObject::const_iterator iter = inputs.begin(); iter != inputs.end(); ++iter) {
    std::cerr << iter.key().toStdString() << " : " << iter.value().toString().toStdString() << "\n";
  }
}

void FirebaseCompute::update_wires() {
  // This caches the dep, and will only dirty this component when it changes.
  _enter = find_enter_node();
}

Entity* FirebaseCompute::find_group_context() const {
  Entity* e = our_entity();
  while(e) {
    if (e->get_did() == EntityDID::kFirebaseGroupNodeEntity) {
      return e;
    }
    e = e->get_parent();
  }
  assert(false);
  return NULL;
}

Dep<EnterFirebaseGroupCompute> FirebaseCompute::find_enter_node() {
  Entity* group = find_group_context();
  Entity* enter = group->get_child("enter");
  assert(enter);
  return get_dep<EnterFirebaseGroupCompute>(enter);
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

void FirebaseWriteDataCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  FirebaseCompute::create_inputs_outputs(config);

  EntityConfig c = config;
  c.expose_plug = false;
  c.unconnected_value = "some/path";

  create_input(Message::kDataPath, c);

  c.unconnected_value = "hello there";
  create_input(Message::kValue, c);
}

const QJsonObject FirebaseWriteDataCompute::_hints = FirebaseWriteDataCompute::init_hints();
QJsonObject FirebaseWriteDataCompute::init_hints() {
  QJsonObject m;
  FirebaseCompute::init_hints(m);

  add_hint(m, Message::kDataPath, HintKey::kDescriptionHint, "Firebase data path.");
  add_hint(m, Message::kValue, HintKey::kDescriptionHint, "The data to write at the path.");

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

void FirebaseReadDataCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  FirebaseCompute::create_inputs_outputs(config);

  EntityConfig c = config;
  c.expose_plug = false;
  c.unconnected_value = "some/path";

  create_input(Message::kDataPath, c);

  c.unconnected_value = "data";
  create_input(Message::kOutputPropertyName, c);

  c.unconnected_value = true;
  create_input(Message::kListenForChanges, c);
}

const QJsonObject FirebaseReadDataCompute::_hints = FirebaseReadDataCompute::init_hints();
QJsonObject FirebaseReadDataCompute::init_hints() {
  QJsonObject m;
  FirebaseCompute::init_hints(m);

  add_hint(m, Message::kDataPath, HintKey::kDescriptionHint, "Firebase data path.");
  add_hint(m, Message::kOutputPropertyName, HintKey::kDescriptionHint, "The name of the property to add to our output. The value of the property will be the latest value from Firebase.");
  add_hint(m, Message::kListenForChanges, HintKey::kDescriptionHint, "When enabled, this node will be updated when there are any changes to the value.");

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
  QString prop_name = _inputs->get_input_value(Message::kOutputPropertyName).toString();
  if (incoming.isObject()) {
    QJsonObject obj = incoming.toObject();
    obj.insert(prop_name, chain_state.value("value"));
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
