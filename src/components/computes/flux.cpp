#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/flux.h>
#include <base/objectmodel/entityids.h>
#include <entities/entityinstancer.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

namespace ngs {

const char* InputTraits::folder_name = kInputsFolderName;
const char* OutputTraits::folder_name = kOutputsFolderName;


template<class Traits>
Flux<Traits>::Flux(Entity* entity)
    : Component(entity, kIID(), kDID()),
      _manipulator(this),
      _null(this) {
  get_dep_loader()->register_fixed_dep(_manipulator, Path());
}

template<class Traits>
Flux<Traits>::~Flux() {
}

template<class Traits>
void Flux<Traits>::update_wires() {
  internal();

  std::unordered_map<std::string, size_t> next_exposed_ordering;
  std::unordered_map<std::string, Dep<typename Traits::IOCompute> > next_hidden;
  std::unordered_map<std::string, Dep<typename Traits::IOCompute> > next_exposed;
  std::unordered_map<std::string, Dep<typename Traits::IOCompute> > next_all;
  gather(next_exposed_ordering, next_hidden, next_exposed, next_all);

  // If we're already up to date, then return right away.
  if (next_exposed_ordering == _exposed_ordering &&
      next_hidden == _hidden &&
      next_exposed == _exposed &&
      next_all == _all) {
    return;
  }

  // Update our values.
  _exposed_ordering = next_exposed_ordering;
  _hidden = next_hidden;
  _exposed = next_exposed;
  _all = next_all;

  // Update the topologies.
  if (Traits::FluxDID == ComponentDID::kOutputs) {
    _manipulator->set_output_topology(our_entity(), _exposed_ordering);
  } else if (Traits::FluxDID == ComponentDID::kInputs) {
    _manipulator->set_input_topology(our_entity(), _exposed_ordering);
  }
}

template<class Traits>
void Flux<Traits>::gather(std::unordered_map<std::string, size_t>& exposed_ordering,
                          std::unordered_map<std::string, Dep<typename Traits::IOCompute> >& hidden,
                          std::unordered_map<std::string, Dep<typename Traits::IOCompute> >& exposed,
                          std::unordered_map<std::string, Dep<typename Traits::IOCompute> >& all) {
  internal();

  exposed_ordering.clear();
  hidden.clear();
  exposed.clear();
  exposed_ordering.clear();

  std::vector<std::string> exposed_names;

  // Loop through all the inputs or outputs.
  Entity* space = has_entity(Path({".",Traits::folder_name}));
  if (space) {
    const Entity::NameToChildMap& children = space->get_children();
    for (auto iter: children) {
      Entity* child = iter.second;
      // If the child is not an input or output, then skip it.
      if (child->get_did() != Traits::IOComputeEntityDID) {
        continue;
      }
      // Grab a dep on the input or output.
      Dep<typename Traits::IOCompute> dep = get_dep<typename Traits::IOCompute>(child);
      if (dep) {
        if (dep->is_exposed()) {
          // If the input or output is exposed, record it in the exposed set.
          exposed.insert({child->get_name(),dep});
          exposed_names.push_back(child->get_name());
        } else {
          // If the input or output is not exposed, record it in the hidden set.
          hidden.insert({child->get_name(),dep});
        }
      }
    }
  }

  // Sort the exposed inputs/outputs alphabetically.
  std::sort(exposed_names.begin(), exposed_names.end());
  for (size_t i=0; i<exposed_names.size(); ++i) {
    exposed_ordering[exposed_names[i]] = i;
  }

  // Cache a merged map of the exposed and hidden.
  all = exposed;
  all.insert(hidden.begin(), hidden.end());
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

template<class Traits>
bool Flux<Traits>::has(const std::string& name) const {
  external();
  if (_all.count(name)) {
    return true;
  }
  return false;
}

template<class Traits>
const Dep<typename Traits::IOCompute>& Flux<Traits>::get(const std::string& name) const {
  external();
  if (_all.count(name)) {
    return _all.at(name);
  }
  return _null;
}




// -------------------------------------------------------------------------------------------------------------------
// Explicit Template Initialization.
// -------------------------------------------------------------------------------------------------------------------

template class Flux<InputTraits>;
template class Flux<OutputTraits>;

}
