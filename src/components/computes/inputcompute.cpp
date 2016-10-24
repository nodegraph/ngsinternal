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

  QVariantMap output;

  // Merge in information from the default unconnected value.
  if (_unconnected_value.isValid()) {
    if (variant_is_map(_unconnected_value)) {
      // If the unconnected value is a map, merge the values in.
      QVariantMap map = _unconnected_value.toMap();
      for (QVariantMap::const_iterator iter = map.constBegin(); iter != map.constEnd(); ++iter) {
        output.insert(iter.key(), iter.value());
      }
    } else {
      output["value"] = _unconnected_value;
    }
  }

  // Merge in information from the upstream output.
  if (_upstream) {
    QVariant upstream_out = _upstream->get_output("out");
    assert(variant_is_map(upstream_out));

    QVariantMap map = upstream_out.toMap();
    for (QVariantMap::const_iterator iter = map.constBegin(); iter != map.constEnd(); ++iter) {
      output.insert(iter.key(), iter.value());
    }
  }

  // Cache the result in our outputs.
  set_output("out", output);
  return true;

//  // Note! The InputCompute is the only compute which can output something
//  // other than a javascript object type. It can output other types like
//  // arrays, strings and numbers.
//  if (_upstream) {
//    QVariant up = _upstream->get_output("out");
//    // Only js object are allowed to pass through links in the node graph.
//    assert(variant_is_map(up));
//    QVariantMap map = up.toMap();
//    // If there is only entry in the map, then we use that as our output.
//    if (map.size() == 1) {
//      set_output("out", map.begin().value());
//    } else if ((map.size() > 1) && (map.count("value"))){
//      // Otherwise if there is an entry named "value" we that.
//      set_output("out", map["value"]);
//    } else {
//      // Otherwise we use our unconnected value.
//      set_output("out", _unconnected_value);
//    }
//  } else {
//    set_output("out", _unconnected_value);
//  }
//  return true;
}

void InputCompute::set_unconnected_value(const QVariant& value) {
  external();
  _unconnected_value = value;
}

QVariant InputCompute::get_unconnected_value() const {
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

Dep<OutputCompute> InputCompute::get_output_compute() const {
  external();
  return _upstream;
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

  // Serialize the param value.
  QByteArray data;
  QDataStream ds(&data,QIODevice::WriteOnly);
  ds << _unconnected_value;
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
  QDataStream ds(&data,QIODevice::ReadOnly);
  ds >> _unconnected_value;
}

}
