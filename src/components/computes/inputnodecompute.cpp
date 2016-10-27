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

const QJSValue InputNodeCompute::_hints = InputNodeCompute::init_hints();
QJSValue InputNodeCompute::init_hints() {
  QJSValue m;

  add_hint(m, "default_value", HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, "default_value", HintType::kMultiLineEdit, true);
  add_hint(m, "default_value", HintType::kDescription, "The object that will be output by this node, if the corresponding input plug on this group is not connected. This must be specified in JSON format and must represent an object.");

  return m;
}

bool InputNodeCompute::update_state() {
  Compute::update_state();

  // The override takes the highest priority.
  if (_use_override) {
    set_output("out", _override);
    return true;
  }

  // This will hold our final output.
  QJSValue output;

  // Start with our data value.
  QString text = get_input_value("default_value").toString();
  QJSValue result;
  QString error;
  if (!evaluate_expression_js(text, result, error)) {
    QJSValue map;
    set_output("out", result); // result will contain info about the error as properties
    on_error();
    return false;
  } else {
    output = deep_merge(output, result);
  }

  set_output("out", output);
  return true;
}

void InputNodeCompute::set_override(const QJSValue& override) {
  external();
  _override = override;
  _use_override = true;
}

const QJSValue& InputNodeCompute::get_override() const {
  external();
  return _override;
}

void InputNodeCompute::clear_override() {
  _override = QJSValue();
  _use_override = false;
}

}

