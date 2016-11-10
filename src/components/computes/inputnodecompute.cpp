#include <base/objectmodel/entity.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/inputcompute.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

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
  create_output("out");
}

const QJsonObject InputNodeCompute::_hints = InputNodeCompute::init_hints();
QJsonObject InputNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "default_value", HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, "default_value", HintType::kMultiLineEdit, true);
  add_hint(m, "default_value", HintType::kDescription, "The object that will be output by this node, if the corresponding input plug on this group is not connected. This must be specified in JSON format and must represent an object.");

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
  _override = override;
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

