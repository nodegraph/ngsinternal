#include <entities/entityids.h>

#include <base/objectmodel/deploader.h>

#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <entities/componentids.h>

#include <components/compshapes/linkshape.h>

namespace ngs {

InputCompute::InputCompute(Entity* entity)
    : QObject(),
      Compute(entity, kDID()),
      _output(this) {
	get_dep_loader()->register_dynamic_dep(_output);
}

InputCompute::~InputCompute() {
}

void InputCompute::update_state() {
  Compute::update_state();
  if (_output) {
    set_result("out", _output->get_result("out"));
  } else {
    set_result("out", _param_data);
  }
}

void InputCompute::set_value(QVariant& var) {
  start_method();
  _param_data = var;
}

bool InputCompute::can_link_output_compute(const Dep<OutputCompute>& output) const {
  assert(output);
  if (dep_creates_cycle(output)) {
    return false;
  }
  return true;
}

bool InputCompute::link_output_compute(Dep<OutputCompute>& output) {
  start_method();
  assert(output);

  // Make sure to be disconnected before connecting.
  if (!_output && output) {
    unlink_output_compute();
  }
  // Now connect.
  _output = output;
  if (_output) {
    return true;
  }
  // Linking was unsuccessful.
  return false;
}

Dep<OutputCompute> InputCompute::get_output_compute() const {
  start_method();
  return _output;
}

void InputCompute::unlink_output_compute() {
  start_method();
  if (_output) {
    _output.reset();
  }
}

}
