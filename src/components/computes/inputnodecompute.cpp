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
  create_input("default_value", QJsonObject(), false); //"{\n\t\"value\": 0\n}"
  create_input("type", (int)JSType::kNumber, false);
  create_output("out");
}

const QJsonObject InputNodeCompute::_hints = InputNodeCompute::init_hints();
QJsonObject InputNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "default_value", HintKey::kJSTypeHint, to_underlying(JSType::kObject));
  add_hint(m, "default_value", HintKey::kElementResizableHint, true);
  add_hint(m, "default_value", HintKey::kDescriptionHint, "The value to be output by this node when its associated input plug on the group is not connected.");

  add_hint(m, "type", HintKey::kJSTypeHint, to_underlying(JSType::kNumber));
  add_hint(m, "type", HintKey::kEnumHint, to_underlying(EnumHintValue::kJSType));
  add_hint(m, "type", HintKey::kDescriptionHint, "The type of the value to be output by this node.");

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

void InputNodeCompute::set_override(const QJsonValue& override) {
  external();
//  if (override.isString()) {
//    QJsonValue expr_result;
//    QString error;
//    if (!eval_js_with_inputs(override.toString(), expr_result, error)) {
//      _override = override;
//    } else {
//      _override = expr_result;
//    }
//  }
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

