#include <components/computes/outputcompute.h>
#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
#include <entities/entityids.h>

namespace ngs {

OutputCompute::OutputCompute(Entity* entity)
    : Compute(entity, kDID()),
      _node_compute(this),
      _exposed(true) {
  get_dep_loader()->register_fixed_dep(_node_compute, Path({"..",".."}));
}

OutputCompute::~OutputCompute() {
}

void OutputCompute::update_wires() {
  internal();
}

bool OutputCompute::update_state() {
  internal();
  Compute::update_state();

  // Using our name we query our nodes compute results.
  const std::string& our_name = our_entity()->get_name();
  set_output("out", _node_compute->get_output("our_name"));
  return true;
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
