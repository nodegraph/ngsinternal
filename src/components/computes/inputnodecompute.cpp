#include <base/objectmodel/entity.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/jsonutils.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>
#include <guicomponents/comms/commtypes.h>

namespace ngs {

// Input Nodes don't have input plugs, but they are associated
// with an input plug on the surrounding group.
// The InputComputer mainly passes these calls onto its parent group node.

InputNodeCompute::InputNodeCompute(Entity* entity):
    Compute(entity, kDID()) {
}

InputNodeCompute::~InputNodeCompute() {
}

void InputNodeCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  QJsonObject data;
  data.insert("value", 123);
  create_input("default_value", data, false);
  create_input("description", "", false);
  create_output("out");
}

const QJsonObject InputNodeCompute::_hints = InputNodeCompute::init_hints();
QJsonObject InputNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "default_value", HintKey::kJSTypeHint, to_underlying(JSType::kObject));
  add_hint(m, "default_value", HintKey::kElementResizableHint, true);
  add_hint(m, "default_value", HintKey::kDescriptionHint, "The value to be output by this node when its associated input plug on the group is not connected.");

  add_hint(m, "description", HintKey::kJSTypeHint, to_underlying(JSType::kString));
  add_hint(m, "description", HintKey::kDescriptionHint, "A short description of this input value. It will be displayed when editing the surrounding group.");

  return m;
}

bool InputNodeCompute::update_state() {
  Compute::update_state();

  // This will hold our final output.
  QJsonValue output;

  // Start with our default value.
  QJsonValue default_value = _inputs->get_input_value("default_value");
  output = JSONUtils::deep_merge(output, default_value);

  // If there is an override then merge that in.
  if (!_override.isNull() && !_override.isUndefined()) {
    output = JSONUtils::deep_merge(output, _override);
  }

  set_output("out", output);
  return true;
}

void InputNodeCompute::set_default_value(const QJsonValue& value) {
  Dep<InputCompute> default_value = get_dep<InputCompute>(our_entity()->get_entity(Path({".","inputs","default_value"})));
  default_value->set_unconnected_value(value);
}

void InputNodeCompute::set_override(const QJsonValue& override) {
  external();
  _override = override;
}

const QJsonValue& InputNodeCompute::get_override() const {
  external();
  return _override;
}

void InputNodeCompute::clear_override() {
  internal();
  _override = QJsonValue();
}

}

