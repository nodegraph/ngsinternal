#include <entities/entityids.h>

#include <base/objectmodel/deploader.h>

#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <entities/componentids.h>

#include <components/compshapes/linkshape.h>

#include <QtCore/QVariant>
#include <QtCore/QIODevice>
#include <QtCore/QDataStream>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

namespace ngs {

InputCompute::InputCompute(Entity* entity)
    : Compute(entity, kDID()),
      _upstream(this),
      _exposed(true) {
	get_dep_loader()->register_dynamic_dep(_upstream);
}

InputCompute::~InputCompute() {
}

bool InputCompute::update_state() {
  internal();
  Compute::update_state();

  // This will hold our final output value.
  QJsonValue output;

  // Start with our unconnected value.
  output = _unconnected_value;

  // Merge in information from the upstream output.
  if (_upstream) {
    QJsonValue out = _upstream->get_output("out");

    // The input compute is special in that we allow data types from upstream to change.
    // This is because input computes are the only computes that actually get linked.
    // The type changes that we allow are from js/json strings to objects and arrays and vice versa.
    Compute::prep_source_for_merge(output, out);
    output = deep_merge(output, out);
  }

  // Cache the result in our outputs.
  set_output("out", output);
  return true;
}

void InputCompute::set_unconnected_value(const QJsonValue& value) {
  external();
  _unconnected_value = value;
}

const QJsonValue& InputCompute::get_unconnected_value() const {
  external();
  return _unconnected_value;
}

void InputCompute::set_exposed(bool exposed) {
  external();
  _exposed = exposed;
}

bool InputCompute::is_exposed() const {
  external();
  return _exposed;
}

bool InputCompute::can_link_output_compute(const Dep<OutputCompute>& output) const {
  external();
  assert(output);
  if (dep_creates_cycle(output)) {
    return false;
  }
  return true;
}

bool InputCompute::link_output_compute(Dep<OutputCompute>& output) {
  external();
  assert(output);

  // Make sure to be disconnected before connecting.
  if (!_upstream && output) {
    unlink_output_compute();
  }
  // Now connect.
  _upstream = output;
  if (_upstream) {
    return true;
  }
  // Linking was unsuccessful.
  return false;
}

const Dep<OutputCompute>& InputCompute::get_output_compute() const {
  external();
  return _upstream;
}

bool InputCompute::is_connected() const {
  if (_upstream) {
    return true;
  }
  return false;
}

void InputCompute::unlink_output_compute() {
  external();
  if (_upstream) {
    _upstream.reset();
  }
}

void InputCompute::save(SimpleSaver& saver) const {
  external();
  Compute::save(saver);

  // Serialize the exposed value.
  saver.save(_exposed);

  // Serialize the value.
  // We create a dummy array to shove the value into, as QJsonDocument won't take a non-array or non-object.
  // However the newer json specs allow storing singular values, this feature may be coming soon.
  QJsonArray arr;
  arr.push_back(_unconnected_value);
  QJsonDocument doc;
  doc.setArray(arr);
  QByteArray data = doc.toJson();
  int num_bytes = data.size();
  saver.save(num_bytes);
  saver.save_raw(data.data(), num_bytes);
}

void InputCompute::load(SimpleLoader& loader) {
  external();
  Compute::load(loader);

  // Load the exposed value.
  loader.load(_exposed);

  // Load the num bytes of the param value.
  int num_bytes;
  loader.load(num_bytes);

  // Load the param value.
  QByteArray data;
  data.resize(num_bytes);
  loader.load_raw(data.data(), num_bytes);
  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(data, &error);
  assert(error.error == QJsonParseError::NoError);
  QJsonArray arr = doc.array();
  _unconnected_value = arr.at(0);
}

}
