#include <components/computes/outputcompute.h>
#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/upperhierarchychange.h>
#include <entities/entityids.h>

namespace ngs {

OutputCompute::OutputCompute(Entity* entity)
    : Compute(entity, kDID()),
      _upper_change(this),
      _node_compute(this),
      _exposed(true),
      _exposed_index(-1),
      _num_exposed(-1),
      _num_outputs(-1){
  get_dep_loader()->register_fixed_dep(_upper_change, "../..");
  get_dep_loader()->register_fixed_dep(_node_compute, "../..");
}

OutputCompute::~OutputCompute() {
}

void OutputCompute::update_state() {
  if (dep_is_dirty(_upper_change)) {
    update_index();

    // Using our name we query our nodes compute results.
    const std::string& our_name = our_entity()->get_name();
    set_result("out", _node_compute->get_result(our_name));
  }
}

void OutputCompute::set_exposed(bool exposed) {
  start_method();
  _exposed = exposed;
}

bool OutputCompute::is_exposed() const {
  start_method();
  return _exposed;
}

size_t OutputCompute::get_exposed_output_index() const {
  start_method();
  return _exposed_index;
}

size_t OutputCompute::get_num_exposed_outputs() const {
  start_method();
  return _num_exposed;
}

size_t OutputCompute::get_num_hidden_outputs() const {
  start_method();
  return get_num_outputs() - get_num_exposed_outputs();
}

size_t OutputCompute::get_num_outputs() const {
  start_method();
  return _num_outputs;
}

void OutputCompute::update_index() {
  // Note this is slightly inefficient because all output computes will perform
  // this same computation individually.

  // Get the outputs namespacing entity.
  Entity* outputs_space = our_entity()->get_parent();

  // Loop through all its children.
  const Entity::NameToChildMap& children = outputs_space->get_children();
  // Gather exposed output names.
  std::vector<std::string> exposed_names;
  // Gather the total number of outputs.
  _num_outputs = 0;
  // Loop.
  for (auto iter: children) {
    // Record all exposed output computes names.
    if (iter.second->get_did() == kOutputEntity) {
      ++_num_outputs;

      // If the entity is us, just use ourself.
      // Otherwise we'll get a dep cycle.
      if (iter.second->get_name() == get_name()) {
        if (is_exposed()) {
          exposed_names.push_back(get_name());
        }
        continue;
      }

      // Otherwise if the entity is not us.
      Dep<OutputCompute> other = get_dep<OutputCompute>(iter.second);
      if (other->is_exposed()) {
        exposed_names.push_back(other->get_name());
      }
    }
  }

  // Sort the names.
  std::sort(exposed_names.begin(), exposed_names.end());

  // Update our relative positioning.
  _num_exposed = exposed_names.size();
  std::vector<std::string>::const_iterator iter = std::find(exposed_names.begin(),exposed_names.end(), get_name());
  if (iter != exposed_names.end()) {
    _exposed_index = iter - exposed_names.begin();
  } else {
    _exposed_index = -1;
  }
}

}
