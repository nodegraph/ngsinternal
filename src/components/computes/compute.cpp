#include <components/computes/compute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/inputs.h>
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

void Compute::initialize_wires() {
  Component::initialize_wires();
  if ((get_did() != ComponentDID::kInputCompute) && (get_did() != ComponentDID::kOutputCompute)) {
    _ng_manipulator = get_dep<BaseNodeGraphManipulator>(get_app_root());
  }
}

void Compute::update_wires() {
  internal();
  // The inputs component will only be present on the entity containing a node based compute.
  if (_inputs) {
    const std::unordered_map<std::string, Entity*>& exposed = _inputs->get_exposed();
    const std::unordered_map<std::string, Entity*>& hidden = _inputs->get_hidden();
    for (auto iter: exposed) {
      Dep<InputCompute> input = get_dep<InputCompute>(iter.second);
      _named_inputs.insert({input->get_name(), input});
    }
    for (auto iter: hidden) {
      Dep<InputCompute> input = get_dep<InputCompute>(iter.second);
      _named_inputs.insert({input->get_name(), input});
    }
  }
}

void Compute::update_state() {
  internal();
  // Notify the gui side that a computation is now processing on the compute side.
  if (_ng_manipulator) {
    _ng_manipulator->set_compute_node(our_entity());
  }
}

bool Compute::clean_finalize() {
  internal();
  Component::clean_finalize();
  // Notify the gui side that a computation is now processing on the compute side.
  if (_ng_manipulator) {
    _ng_manipulator->clear_compute_node();
  }
  return true;
}

QVariantMap Compute::get_inputs() const {
  external();
  QVariantMap map;
  for (auto iter: _named_inputs) {
    map[QString::fromStdString(iter.first)] = iter.second->get_output("out");
  }
  return map;
}

void Compute::set_params(const QVariantMap& params) {
  QVariantMap::const_iterator iter;
  for (iter = params.begin(); iter != params.end(); ++iter) {
    if (_named_inputs.count(iter.key().toStdString())) {
      _named_inputs.at(iter.key().toStdString())->set_value(iter.value());
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

void Compute::copy_outputs(const std::string& output_name, Dep<InputCompute>& other_compute, const std::string& other_output_name) {
  set_output(output_name, other_compute->get_output(other_output_name));
}

void Compute::copy_outputs(const std::string& output_name, Dep<Compute>& other_compute, const std::string& other_output_name) {
  set_output(output_name, other_compute->get_output(other_output_name));
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

}
