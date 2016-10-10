#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/flux.h>
#include <entities/entityids.h>
#include <entities/entityinstancer.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

namespace ngs {

const char* InputTraits::folder_name = "inputs";
const char* OutputTraits::folder_name = "outputs";


template<class Traits>
Flux<Traits>::Flux(Entity* entity)
    : Component(entity, kIID(), kDID()),
      _ng_manipulator(this) {
  get_dep_loader()->register_fixed_dep(_ng_manipulator, Path({}));
}

template<class Traits>
Flux<Traits>::~Flux() {
}

template<class Traits>
void Flux<Traits>::update_wires() {
  internal();
  if (!wires_are_up_to_date()) {
    gather();
    if (Traits::FluxDID == ComponentDID::kOutputs) {
      _ng_manipulator->set_output_topology(our_entity(), _exposed_ordering);
    } else if (Traits::FluxDID == ComponentDID::kInputs) {
      _ng_manipulator->set_input_topology(our_entity(), _exposed_ordering);
    }
  }
}

template<class Traits>
bool Flux<Traits>::wires_are_up_to_date() {
  Entity* space = has_entity(Path({".",Traits::folder_name}));
  if (!space) {
    return true;
  }

  size_t num_io = 0;
  if (space) {
    const Entity::NameToChildMap& children = space->get_children();
    for (auto iter: children) {
      Entity* child = iter.second;

      // If we don't have an appropriate entity, skip it.
      if (child->get_did() != Traits::IOComputeEntityDID) {
        continue;
      }

      // We have an input/output.
      const std::string& name = child->get_name();
      num_io += 1;

      // Check if this input/output is accounted for.
      if (_all.count(name)) {
        continue;
      } else {
        return false;
      }
    }
  }

  // Check the number of inputs/outputs.
  if (_all.size() != num_io) {
    return false;
  }

  // Make sure all the exposed are actually exposed.
  for (auto iter: _exposed) {
    if (!iter.second->is_exposed()) {
      return false;
    }
  }

  // Make sure all the hidden are actually hidden.
  for (auto iter: _hidden) {
    if (iter.second->is_exposed()) {
      return false;
    }
  }

  return true;
}

template<class Traits>
void Flux<Traits>::gather() {
  internal();
  _hidden.clear();
  _exposed.clear();

  std::vector<std::string> exposed_names;

  Entity* space = has_entity(Path({".",Traits::folder_name}));
  if (space) {
    const Entity::NameToChildMap& children = space->get_children();
    for (auto iter: children) {
      Entity* child = iter.second;
      if (child->get_did() != Traits::IOComputeEntityDID) {
        continue;
      }
      Dep<Traits::IOCompute> dep = get_dep<Traits::IOCompute>(child);
      if (dep) {
        if (dep->is_exposed()) {
          _exposed.insert({child->get_name(),dep});
          exposed_names.push_back(child->get_name());
        } else {
          _hidden.insert({child->get_name(),dep});
        }
      }
    }
  }

  // Sort the exposed inputs/outputs alphabetically.
  std::sort(exposed_names.begin(), exposed_names.end());
  for (size_t i=0; i<exposed_names.size(); ++i) {
    _exposed_ordering[exposed_names[i]] = i;
  }

  // Cache a merged map of the exposed and hidden.
  _all = _exposed;
  _all.insert(_hidden.begin(), _hidden.end());
}

template<class Traits>
size_t Flux<Traits>::get_exposed_index(const std::string& name) const {
  external();
  if (_exposed_ordering.count(name)) {
    return _exposed_ordering.at(name);
  }
  return -1;
}

template<class Traits>
size_t Flux<Traits>::get_num_exposed() const {
  external();
  return _exposed.size();
}

template<class Traits>
size_t Flux<Traits>::get_num_hidden() const {
  external();
  return _hidden.size();
}

template<class Traits>
size_t Flux<Traits>::get_total() const {
  external();
  return _exposed.size() + _hidden.size();
}

template<class Traits>
const std::unordered_map<std::string, Dep<typename Traits::IOCompute> >& Flux<Traits>::get_hidden() const {
  external();
  return _hidden;
}

template<class Traits>
const std::unordered_map<std::string, Dep<typename Traits::IOCompute> >& Flux<Traits>::get_exposed() const {
  external();
  return _exposed;
}

template<class Traits>
const std::unordered_map<std::string, Dep<typename Traits::IOCompute> >& Flux<Traits>::get_all() const {
  external();
  return _all;
}

template class Flux<InputTraits>;
template class Flux<OutputTraits>;

}
