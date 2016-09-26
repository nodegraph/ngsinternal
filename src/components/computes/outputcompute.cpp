#include <components/computes/outputcompute.h>
#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
#include <entities/entityids.h>

namespace ngs {

OutputCompute::OutputCompute(Entity* entity)
    : Compute(entity, kDID()),
      _node_compute(this),
      _exposed(true),
      _exposed_index(-1),
      _num_exposed(-1),
      _num_outputs(-1){
  get_dep_loader()->register_fixed_dep(_node_compute, "../..");
}

OutputCompute::~OutputCompute() {
}

void OutputCompute::update_wires() {
  update_index();
}

void OutputCompute::update_state() {
  // Using our name we query our nodes compute results.
  const std::string& our_name = our_entity()->get_name();
  set_result("out", _node_compute->get_result(our_name));
}

void OutputCompute::set_param_type(ParamType param_type) {
  external();
  _param_type = param_type;
}

ParamType OutputCompute::get_param_type() const {
  external();
  return _param_type;
}

void OutputCompute::set_exposed(bool exposed) {
  external();
  _exposed = exposed;
}

bool OutputCompute::is_exposed() const {
  external();
  return _exposed;
}

size_t OutputCompute::get_exposed_output_index() const {
  external();
  return _exposed_index;
}

size_t OutputCompute::get_num_exposed_outputs() const {
  external();
  return _num_exposed;
}

size_t OutputCompute::get_num_hidden_outputs() const {
  external();
  return get_num_outputs() - get_num_exposed_outputs();
}

size_t OutputCompute::get_num_outputs() const {
  external();
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

void OutputCompute::save(SimpleSaver& saver) const {
  external();
  Compute::save(saver);

  // Serialize the param type.
  size_t type = _param_type;
  saver.save(type);

  // Serialize the exposed value.
  saver.save(_exposed);
}

void OutputCompute::load(SimpleLoader& loader) {
  external();
  Compute::load(loader);

  // Load the param type.
  size_t type;
  loader.load(type);
  _param_type = static_cast<ParamType>(type);

  // Load the exposed value.
  loader.load(_exposed);
}

}
