#include <base/objectmodel/entity.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/inputcompute.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>
#include <guicomponents/comms/commtypes.h>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

namespace ngs {

// Input Nodes don't have input plugs, but they are associated
// with an input plug on the surrounding group.
// The InputComputer mainly passes these calls onto its parent group node.

InputNodeCompute::InputNodeCompute(Entity* entity):
    Compute(entity, kDID()),
    _use_override(false) {
}

InputNodeCompute::~InputNodeCompute() {
}

void InputNodeCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("default_value", "{\n\t\"value\": 0\n}", false);
  create_input("type", (int)JSType::kNumber, false);
  create_output("out");
}

const QJsonObject InputNodeCompute::_hints = InputNodeCompute::init_hints();
QJsonObject InputNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "default_value", HintKey::kJSTypeHint, to_underlying(JSType::kString));
  add_hint(m, "default_value", HintKey::kMultiLineHint, true);
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

  // Start with our data value.
  QString text = _inputs->get_input_value("default_value").toString();
  QJsonValue expr_result;
  QString error;
  if (!eval_js_with_inputs(text, expr_result, error)) {
    set_output("out", expr_result); // result will contain info about the error as properties
    on_error(error);
    return false;
  } else {
    output = deep_merge(output, expr_result);
  }

  // If there is an override then merge that in.
  if (_use_override) {
    output = deep_merge(output, _override);
  }

  set_output("out", output);
  return true;
}

void InputNodeCompute::set_override(const QJsonValue& override) {
  external();
  if (override.isString()) {
    QJsonValue expr_result;
    QString error;
    if (!eval_js_with_inputs(override.toString(), expr_result, error)) {
      _override = override;
    } else {
      _override = expr_result;
    }
  }
  _use_override = true;
}

const QJsonValue& InputNodeCompute::get_override() const {
  external();
  return _override;
}

void InputNodeCompute::clear_override() {
  internal();
  _override = QJsonValue();
  _use_override = false;
}

}

