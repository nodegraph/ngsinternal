#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/outputs.h>
#include <entities/entityids.h>
#include <entities/entityinstancer.h>

namespace ngs {

Outputs::Outputs(Entity* entity)
    : BaseOutputs(entity, kDID()) {
}

Outputs::~Outputs() {
}

void Outputs::update_wires() {
  internal();
  gather();
}

void Outputs::gather() {
  internal();
  _hidden.clear();
  _exposed.clear();

  std::vector<std::string> exposed_names;

  Entity* output_space = has_entity(Path({".","outputs"}));
  if (output_space) {
    const Entity::NameToChildMap& children = output_space->get_children();
    for (auto iter: children) {
      Entity* child = iter.second;
      if (child->get_did() != EntityDID::kOutputEntity) {
        continue;
      }
      Dep<OutputCompute> dep = get_dep<OutputCompute>(child);
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

size_t Outputs::get_exposed_index(const std::string& output_name) const {
  external();
  if (_exposed_ordering.count(output_name)) {
    return _exposed_ordering.at(output_name);
  }
  return -1;
}

size_t Outputs::get_num_exposed() const {
  external();
  return _exposed.size();
}

size_t Outputs::get_num_hidden() const {
  external();
  return _hidden.size();
}

size_t Outputs::get_total() const {
  external();
  return _exposed.size() + _hidden.size();
}


}
