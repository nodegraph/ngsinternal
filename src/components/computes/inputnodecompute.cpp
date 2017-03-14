#include <base/objectmodel/entity.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/jsonutils.h>
#include <guicomponents/comms/guitypes.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

namespace ngs {

// Input Nodes don't have input plugs, but they are associated
// with an input plug on the surrounding group.
// The InputComputer mainly passes these calls onto its parent group node.

InputNodeCompute::InputNodeCompute(Entity* entity, ComponentDID did):
    Compute(entity, did) {
}

InputNodeCompute::~InputNodeCompute() {
}

void InputNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  create_main_output(config);
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
}

const QJsonObject InputNodeCompute::_hints = InputNodeCompute::init_hints();
QJsonObject InputNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "default_value", GUITypes::HintKey::DescriptionHint, "The value to use when its input plug on the group is not connected.");
  add_hint(m, "description", GUITypes::HintKey::DescriptionHint, "A short description of this input value. It will be displayed when editing the surrounding group.");

  return m;
}

bool InputNodeCompute::update_state() {
  Compute::update_state();

  // Start with our default value.
  QJsonValue output = _inputs->get_input_value("default_value");
  // If we are not inside the root group, but some other child group,
  // then the group may set an override from the corresponding group's input param.
  if (!our_entity()->get_parent()->is_root_group()) {
    // If there is an override then merge that in.
    if (!_override.isNull() && !_override.isUndefined()) {
      output = JSONUtils::deep_merge(output, _override);
    }
  }

  set_main_output(output);
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

// --------------------------------------------------------------------------------------


PasswordInputNodeCompute::PasswordInputNodeCompute(Entity* entity):
    InputNodeCompute(entity, kDID()) {
}

PasswordInputNodeCompute::~PasswordInputNodeCompute() {
}

const QJsonObject PasswordInputNodeCompute::_hints = PasswordInputNodeCompute::init_hints();
QJsonObject PasswordInputNodeCompute::init_hints() {
  QJsonObject m = InputNodeCompute::init_hints();
  add_hint(m, "default_value", GUITypes::HintKey::PasswordHint, true);
  return m;
}

}

