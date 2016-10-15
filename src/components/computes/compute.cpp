#include <components/computes/compute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <base/objectmodel/deploader.h>
#include <entities/entityids.h>
#include <entities/entityinstancer.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

namespace ngs {

struct InputComputeComparator {
  bool operator()(const Dep<InputCompute>& left, const Dep<InputCompute>& right) const {
    return left->get_name() < right->get_name();
  }
};

const QVariant Compute::_empty_variant;

Compute::Compute(Entity* entity, ComponentDID derived_id)
    : Component(entity, kIID(), derived_id),
      _inputs(this),
      _ng_manipulator(this) {
  // Note this only exists for node computes and not for plug computes.
  get_dep_loader()->register_fixed_dep(_inputs, Path({"."}));
  // We only grab the _ng_manipulator for non input/output computes, to avoid cycles.
}

Compute::~Compute() {
}

void Compute::create_inputs_outputs() {
  external();
  create_namespace("inputs");
  create_namespace("outputs");
}

void Compute::set_self_dirty(bool dirty) {
  Component::set_self_dirty(dirty);
  if (_ng_manipulator) {
      _ng_manipulator->update_clean_marker(our_entity(), !is_state_dirty());
  }
}

void Compute::initialize_wires() {
  Component::initialize_wires();
  if ((get_did() != ComponentDID::kInputCompute) && (get_did() != ComponentDID::kOutputCompute)) {
    _ng_manipulator = get_dep<BaseNodeGraphManipulator>(get_app_root());
  }
}

bool Compute::update_state() {
  internal();
  // Notify the gui side that a computation is now processing on the compute side.
  if (_ng_manipulator) {
    _ng_manipulator->set_processing_node(our_entity());
  }
  return true;
}

bool Compute::clean_finalize() {
  internal();
  Component::clean_finalize();
  // Notify the gui side that a computation is now processing on the compute side.
  if (_ng_manipulator) {
    _ng_manipulator->clear_processing_node();
  }
  return true;
}

QVariantMap Compute::get_inputs() const {
  external();
  QVariantMap map;
  for (auto iter: _inputs->get_exposed()) {
    map[QString::fromStdString(iter.first)] = iter.second->get_output("out");
  }
  for (auto iter: _inputs->get_hidden()) {
    map[QString::fromStdString(iter.first)] = iter.second->get_output("out");
  }
  return map;
}

QVariantMap Compute::get_hidden_inputs() const {
  QVariantMap map;
  for (auto iter: _inputs->get_hidden()) {
    iter.second->clean_state();
    map[QString::fromStdString(iter.first)] = iter.second->get_output("out");
  }
  return map;
}

QVariantMap Compute::get_exposed_inputs() const {
  QVariantMap map;
  for (auto iter: _inputs->get_exposed()) {
    map[QString::fromStdString(iter.first)] = iter.second->get_output("out");
  }
  return map;
}

void Compute::set_params(const QVariantMap& params) {
  QVariantMap::const_iterator iter;
  for (iter = params.begin(); iter != params.end(); ++iter) {
    if (_inputs->get_exposed().count(iter.key().toStdString())) {
      _inputs->get_exposed().at(iter.key().toStdString())->set_param_value(iter.value());
    } else if (_inputs->get_hidden().count(iter.key().toStdString())) {
      _inputs->get_hidden().at(iter.key().toStdString())->set_param_value(iter.value());
    }
  }
}

const QVariantMap& Compute::get_outputs() const {
  external();
  return _outputs;
}

void Compute::set_outputs(const QVariantMap& outputs) {
  internal();
  _outputs = outputs;
}

QVariant Compute::get_output(const std::string& name) const{
  external();
  if (!_outputs.count(name.c_str())) {
    return _empty_variant;
  }
  return _outputs[name.c_str()];
}

void Compute::set_output(const std::string& name, const QVariant& value) {
  internal();
  _outputs.insert(name.c_str(), value);
}

bool Compute::check_variant_is_bool_and_true(const QVariant& value, const std::string& message) {
  is_static();
  if (value.userType() != QMetaType::Bool) {
    std::cerr << "Error: " << "variant is not a bool: " << message << "\n";
    return false;
  }
  if (!value.toBool()) {
    std::cerr << "Error: " << message << "\n";
    return false;
  }
  return true;
}

bool Compute::check_variant_is_list(const QVariant& value, const std::string& message) {
  is_static();
  // For some reason the value.userType() call returns 1119 which doesn't match
  // QMetaType::QVariantList or QMetaType::QStringList. So we use canConvert instead.
  if (value.canConvert(QMetaType::QVariantList)) {
    return true;
  }
  std::cerr << "Error: " << message << "\n";
  return false;
}

bool Compute::check_variant_is_map(const QVariant& value, const std::string& message) {
  is_static();
  if (value.userType() == QMetaType::QVariantMap) {
    return true;
  }
  std::cerr << "Error: " << message << "\n";
  return false;
}

Entity* Compute::create_input(const std::string& name, ParamType type, bool exposed) {
  external();
  Dep<BaseFactory> factory = get_dep<BaseFactory>(Path({}));
  Entity* inputs_space = get_inputs_space();
  InputEntity* input = static_cast<InputEntity*>(factory->instance_entity(inputs_space, name, EntityDID::kInputEntity));
  input->create_internals();
  input->set_param_type(type);
  input->set_exposed(exposed);
  return input;
}

Entity* Compute::create_output(const std::string& name, ParamType type, bool exposed) {
  external();
  Dep<BaseFactory> factory = get_dep<BaseFactory>(Path({}));
  Entity* outputs_space = get_outputs_space();
  OutputEntity* output = static_cast<OutputEntity*>(factory->instance_entity(outputs_space, name, EntityDID::kOutputEntity));
  output->create_internals();
  output->set_param_type(type);
  output->set_exposed(exposed);
  return output;
}

Entity* Compute::create_namespace(const std::string& name) {
  external();
  Dep<BaseFactory> factory = get_dep<BaseFactory>(Path({}));
  Entity* space = static_cast<BaseNamespaceEntity*>(factory->instance_entity(our_entity(), name, EntityDID::kBaseNamespaceEntity));
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

void Compute::add_hint(const std::string& name, HintType hint_type, const QVariant& value) {
  _hints[name.c_str()].toMap()[QString::number(to_underlying(hint_type))] = value;
}

const QVariantMap& Compute::get_hints() const {
  return _hints;
}

}
