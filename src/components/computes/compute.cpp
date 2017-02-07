#include <components/computes/compute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/jsonutils.h>

#include <base/objectmodel/deploader.h>

#include <base/objectmodel/entityids.h>
#include <entities/entityinstancer.h>

#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlExpression>
#include <QtCore/QJsonValue>

#include <sstream>


namespace ngs {

struct InputComputeComparator {
  bool operator()(const Dep<InputCompute>& left, const Dep<InputCompute>& right) const {
    return left->get_name() < right->get_name();
  }
};

const QJsonObject Compute::_hints;

Compute::Compute(Entity* entity, ComponentDID derived_id)
    : Component(entity, kIID(), derived_id),
      _inputs(this),
      _manipulator(this) {
  // Note this only exists for node computes and not for plug computes.
  get_dep_loader()->register_fixed_dep(_inputs, Path({"."}));
  // We only grab the manipulator for non input/output computes, to avoid cycles.
}

Compute::~Compute() {
}

void Compute::create_inputs_outputs(const EntityConfig& config) {
  external();
  create_namespace("inputs");
  create_namespace("outputs");
}

void Compute::set_self_dirty(bool dirty) {
  Component::set_self_dirty(dirty);
  if (_manipulator) {
      std::cerr << to_underlying(get_did()) << " Compute setting marker to: " << !is_state_dirty() << "\n";
      _manipulator->update_clean_marker(our_entity(), !is_state_dirty());
      if (dirty) {
        std::cerr << to_underlying(get_did()) << " Compute is bubbling dirtiness\n";
        _manipulator->bubble_group_dirtiness(our_entity());
      }
  }
}

void Compute::initialize_wires() {
  Component::initialize_wires();
  if ((get_did() != ComponentDID::kInputCompute) && (get_did() != ComponentDID::kOutputCompute)) {
    _manipulator = get_dep<BaseNodeGraphManipulator>(get_app_root());
  }
}

bool Compute::update_state() {
  internal();
  // Notify the gui side that a computation is now processing on the compute side.
  if (_manipulator) {
    _manipulator->set_processing_node(our_entity());
  }

  return true;
}

bool Compute::clean_finalize() {
  internal();
  Component::clean_finalize();
  // Notify the gui side that a computation is now processing on the compute side.
  if (_manipulator) {
    _manipulator->clear_processing_node();
  }
  return true;
}

QJsonObject Compute::get_editable_inputs() const {
  return _inputs->get_editable_inputs();
}

void Compute::set_editable_inputs(const QJsonObject& inputs) {
  _inputs->set_editable_inputs(inputs);
}

QJsonObject Compute::get_input_exposure() const {
  return _inputs->get_exposure();
}

void Compute::set_input_exposure(const QJsonObject& settings) {
  _inputs->set_exposure(settings);
}

const QJsonObject& Compute::get_outputs() const {
  external();
  return _outputs;
}

void Compute::set_outputs(const QJsonObject& outputs) {
  internal();
  _outputs = outputs;
}

QJsonValue Compute::get_output(const std::string& name) const{
  external();
  if (!_outputs.contains(name.c_str())) {
    return QJsonValue();
  }
  return _outputs.value(name.c_str());
}

void Compute::set_output(const std::string& name, const QJsonValue& value) {
  internal();
  _outputs.insert(name.c_str(), value);
}

Entity* Compute::create_input(const std::string& name, const EntityConfig& config) {
  external();

  // Get the inputs namespace.
  Dep<BaseFactory> factory = get_dep<BaseFactory>(Path());
  Entity* inputs_space = get_inputs_space();

  // Make sure the name doesn't exist already.
  assert(!inputs_space->get_child(name));

  // Create the input.
  InputEntity* input = static_cast<InputEntity*>(factory->instance_entity(inputs_space, EntityDID::kInputEntity, name));
  input->create_internals(config);
  return input;
}

Entity* Compute::create_output(const std::string& name, const EntityConfig& config) {
  external();

  // Get the outputs namespace.
  Dep<BaseFactory> factory = get_dep<BaseFactory>(Path());
  Entity* outputs_space = get_outputs_space();

  // Make sure the name doesn't exist already.
  assert(!outputs_space->get_child(name));

  // Create the output.
  OutputEntity* output = static_cast<OutputEntity*>(factory->instance_entity(outputs_space, EntityDID::kOutputEntity, name));
  output->create_internals(config);
  return output;
}

Entity* Compute::create_namespace(const std::string& name) {
  external();
  Dep<BaseFactory> factory = get_dep<BaseFactory>(Path());
  Entity* space = static_cast<BaseNamespaceEntity*>(factory->instance_entity(our_entity(), EntityDID::kBaseNamespaceEntity, name));
  space->create_internals();
  return space;
}

Entity* Compute::get_inputs_space() {
  external();
  return our_entity()->get_child("inputs");
}

Entity* Compute::get_outputs_space() {
  external();
  return our_entity()->get_child("outputs");
}

void Compute::add_hint(QJsonObject& node_hints, const std::string& name, GUITypes::HintKey hint_type, const QJsonValue& value) {
  QJsonObject param_hints = node_hints.value(name.c_str()).toObject();
  param_hints.insert(QString::number(to_underlying(hint_type)), value);
  node_hints.insert(name.c_str(),  param_hints);
}

void Compute::remove_hint(QJsonObject& node_hints, const std::string& name) {
  node_hints.remove(name.c_str());
}

void Compute::on_error(const QString& error_message) {
  _manipulator->set_error_node(error_message);
  _manipulator->clear_ultimate_targets();
}

bool Compute::eval_js_with_inputs(const QString& text, QJsonValue& result, QString& error) const {
  internal();
  QJSEngine engine;

  // Add the input values into the context.
  for (auto &iter: _inputs->get_all()) {
    const Dep<InputCompute>& input = iter.second;
    QJsonValue value = input->get_output("out");
    const std::string& input_name = input->get_name();
    QJSValue jsvalue = engine.toScriptValue(value);
    engine.globalObject().setProperty(QString::fromStdString(input_name), jsvalue);
  }

  return JSONUtils::eval_js_in_context(engine, text, result,error);
}

}
