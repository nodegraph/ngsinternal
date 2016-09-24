#include <entities/entityids.h>

#include <base/objectmodel/deploader.h>

#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <entities/componentids.h>

#include <components/compshapes/linkshape.h>

#include <QtCore/QIODevice>
#include <QtCore/QDataStream>

namespace ngs {

InputCompute::InputCompute(Entity* entity)
    : QObject(),
      Compute(entity, kDID()),
      _param_type(ParamType::kQVariantMap),
      _output(this),
      _exposed(true) {
	get_dep_loader()->register_dynamic_dep(_output);
}

InputCompute::~InputCompute() {
}

void InputCompute::update_state() {
  Compute::update_state();
  if (_output) {
    set_result("out", _output->get_result("out"));
  } else {
    set_result("out", _param_value);
  }
}

void InputCompute::set_value(const QVariant& value) {
  start_method();
  _param_value = value;
}

void InputCompute::set_param_type(ParamType param_type) {
  start_method();
  _param_type = param_type;
}

ParamType InputCompute::get_param_type() const {
  start_method();
  return _param_type;
}

void InputCompute::set_exposed(bool exposed) {
  start_method();
  _exposed = exposed;
}

bool InputCompute::is_exposed() const {
  start_method();
  return _exposed;
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

void InputCompute::save(SimpleSaver& saver) const {
  start_method();
  Compute::save(saver);

  // Serialize the param type.
  size_t type = _param_type;
  saver.save(type);

  // Serialize the exposed value.
  saver.save(_exposed);

  // Serialize the param value.
  QByteArray data;
  QDataStream ds(&data,QIODevice::WriteOnly);
  ds << _param_value;
  int num_bytes = data.size();
  saver.save(num_bytes);
  saver.save_raw(data.data(), num_bytes);
}

void InputCompute::load(SimpleLoader& loader) {
  start_method();
  Compute::load(loader);

  // Load the param type.
  size_t type;
  loader.load(type);
  _param_type = static_cast<ParamType>(type);

  // Load the exposed value.
  loader.load(_exposed);

  // Load the num bytes of the param value.
  int num_bytes;
  loader.load(num_bytes);

  // Load the param value.
  QByteArray data;
  data.resize(num_bytes);
  loader.load_raw(data.data(), num_bytes);
  QDataStream ds(&data,QIODevice::ReadOnly);
  ds >> _param_value;
}

}
