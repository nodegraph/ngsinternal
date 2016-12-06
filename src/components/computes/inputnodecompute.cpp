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

void InputNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    // We take the unconnected value from the config.
    create_input("default_value", c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "";
    create_input("description", c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = true;
    create_output("out", c);
  }
}

const QJsonObject InputNodeCompute::_hints = InputNodeCompute::init_hints();
QJsonObject InputNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "default_value", HintKey::kDescriptionHint, "The value to be output by this node when its associated input plug on the group is not connected.");
  add_hint(m, "description", HintKey::kDescriptionHint, "A short description of this input value. It will be displayed when editing the surrounding group.");

  return m;
}

bool InputNodeCompute::update_state() {
  Compute::update_state();

  // Start with our default value.
  QJsonValue output = _inputs->get_input_value("default_value");

  // If there is an override then merge that in.
  if (!_override.isNull() && !_override.isUndefined()) {
    output = JSONUtils::deep_merge(output, _override);
  }

  set_output("out", output);
  return true;
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

