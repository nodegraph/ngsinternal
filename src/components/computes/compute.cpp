#include <components/computes/compute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <base/objectmodel/lowerhierarchychange.h>
#include <base/objectmodel/deploader.h>
#include <entities/entityids.h>
#include <entities/entityinstancer.h>

namespace ngs {

struct InputComputeComparator {
  bool operator()(const Dep<InputCompute>& left, const Dep<InputCompute>& right) const {
    return left->get_name() < right->get_name();
  }
};

const QVariant Compute::_empty_variant;

Compute::Compute(Entity* entity, size_t derived_id)
    : Component(entity, kIID(), derived_id),
      _lower_change(this) {
  get_dep_loader()->register_fixed_dep(_lower_change, ".");
}

Compute::~Compute() {
}

void Compute::create_inputs_outputs() {
  create_namespace("inputs");
  create_namespace("outputs");
}

void Compute::gather_wires() {
  gather_inputs();
}

void Compute::gather_inputs() {
  // Connect to our inputs.
  _inputs.clear();
  Entity* inputs_space = has_entity("./inputs");
  if (inputs_space) {
    const Entity::NameToChildMap& children = inputs_space->get_children();
    for (auto iter: children) {
      if (iter.second->get_did() != kInputEntity) {
        continue;
      }
      Dep<InputCompute> dep = get_dep<InputCompute>(iter.second);
      if (dep) {
        _inputs.insert({iter.first, dep});
      }
    }
  }

  // Get all the exposed inputs.
  _exposed_inputs.clear();
  for (auto iter: _inputs) {
    if (iter.second->is_exposed()) {
      _exposed_inputs.push_back(iter.second);
    }
  }

  // Sort the inputs alphabetically.
  std::sort(_exposed_inputs.begin(), _exposed_inputs.end(), InputComputeComparator());
}

size_t Compute::get_exposed_input_index(const std::string& input_name) const {
  external();
  for (size_t i=0; i<_exposed_inputs.size(); ++i) {
    if (_exposed_inputs[i]->get_name() == input_name) {
      return i;
    }
  }
  return -1;
}

size_t Compute::get_num_exposed_inputs() const {
  external();
  return _exposed_inputs.size();
}

size_t Compute::get_num_hidden_inputs() const {
  external();
  return get_num_inputs() - get_num_exposed_inputs();
}

size_t Compute::get_num_inputs() const {
  external();
  return _inputs.size();
}

const QVariantMap& Compute::get_results() const {
  external();
  return _results;
}

QVariant Compute::get_result(const std::string& name) const{
  external();
  if (!_results.count(name.c_str())) {
    return _empty_variant;
  }
  return _results[name.c_str()];
}

void Compute::set_result(const std::string& name, const QVariant& value) {
  external();
  _results.insert(name.c_str(), value);
}

bool Compute::check_variant_is_bool_and_true(const QVariant& value, const std::string& message) {
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
  // For some reason the value.userType() call returns 1119 which doesn't match
  // QMetaType::QVariantList or QMetaType::QStringList. So we use canConvert instead.
  if (value.canConvert(QMetaType::QVariantList)) {
    return true;
  }
  std::cerr << "Error: " << message << "\n";
  return false;
}

bool Compute::check_variant_is_map(const QVariant& value, const std::string& message) {
  if (value.userType() == QMetaType::QVariantMap) {
    return true;
  }
  std::cerr << "Error: " << message << "\n";
  return false;
}

Entity* Compute::create_input(const std::string& name, ParamType type, bool exposed) {
  Dep<BaseFactory> factory = get_dep<BaseFactory>("");
  Entity* inputs_space = get_inputs_space();
  InputEntity* input = static_cast<InputEntity*>(factory->instance_entity(inputs_space, name, kInputEntity));
  input->create_internals();
  input->set_param_type(type);
  input->set_exposed(exposed);
  return input;
}

Entity* Compute::create_output(const std::string& name, ParamType type, bool exposed) {
  Dep<BaseFactory> factory = get_dep<BaseFactory>("");
  Entity* outputs_space = get_outputs_space();
  OutputEntity* output = static_cast<OutputEntity*>(factory->instance_entity(outputs_space, name, kOutputEntity));
  output->create_internals();
  output->set_param_type(type);
  output->set_exposed(exposed);
  return output;
}

Entity* Compute::create_namespace(const std::string& name) {
  Dep<BaseFactory> factory = get_dep<BaseFactory>("");
  Entity* space = static_cast<BaseNamespaceEntity*>(factory->instance_entity(our_entity(), name, kBaseNamespaceEntity));
  space->create_internals();
  return space;
}

Entity* Compute::get_inputs_space() {
  return our_entity()->get_child("inputs");
}

Entity* Compute::get_outputs_space() {
  return our_entity()->get_child("outputs");
}

}
