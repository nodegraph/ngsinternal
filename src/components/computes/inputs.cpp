#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/inputs.h>
#include <entities/entityids.h>
#include <entities/entityinstancer.h>

namespace ngs {

Inputs::Inputs(Entity* entity)
    : BaseInputs(entity, kDID()) {
}

Inputs::~Inputs() {
}

void Inputs::update_wires() {
  internal();
  gather_inputs();
}

void Inputs::gather_inputs() {
  internal();
  _hidden.clear();
  _exposed.clear();

  std::vector<std::string> exposed_names;

  Entity* inputs_space = has_entity(Path({".","inputs"}));
  if (inputs_space) {
    const Entity::NameToChildMap& children = inputs_space->get_children();
    for (auto iter: children) {
      Entity* child = iter.second;
      if (child->get_did() != EntityDID::kInputEntity) {
        continue;
      }
      Dep<InputCompute> dep = get_dep<InputCompute>(child);
      if (dep) {
        if (dep->is_exposed()) {
          _exposed.insert({child->get_name(),child});
          exposed_names.push_back(child->get_name());
        } else {
          _hidden.insert({child->get_name(),child});
        }
      }
    }
  }

  // Sort the exposed inputs alphabetically.
  std::sort(exposed_names.begin(), exposed_names.end());
  for (size_t i=0; i<exposed_names.size(); ++i) {
    _exposed_ordering[exposed_names[i]] = i;
  }
}

size_t Inputs::get_exposed_index(const std::string& input_name) const {
  external();
  if (_exposed_ordering.count(input_name)) {
    return _exposed_ordering.at(input_name);
  }
  return -1;
}

size_t Inputs::get_num_exposed() const {
  external();
  return _exposed.size();
}

size_t Inputs::get_num_hidden() const {
  external();
  return _hidden.size();
}

size_t Inputs::get_total() const {
  external();
  return _exposed.size() + _hidden.size();
}


}
