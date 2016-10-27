#include <base/objectmodel/entity.h>
#include <components/computes/datanodecompute.h>
#include <components/computes/inputcompute.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

namespace ngs {

// Input Nodes don't have input plugs, but they are associated
// with an input plug on the surrounding group.
// The InputComputer mainly passes these calls onto its parent group node.

DataNodeCompute::DataNodeCompute(Entity* entity):
    Compute(entity, kDID()),
    _use_override(false) {
}

DataNodeCompute::~DataNodeCompute() {
}

void DataNodeCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("in", QJSValue());
  create_input("default_value", "3 * 9 + 3", false);
  create_output("out");
}

const QJSValue DataNodeCompute::_hints = DataNodeCompute::init_hints();
QJSValue DataNodeCompute::init_hints() {
  QJSValue m;

  add_hint(m, "in", HintType::kJSType, to_underlying(JSType::kObject));
  add_hint(m, "in", HintType::kDescription, "The main object that flows through this node. This cannot be set manually.");


  add_hint(m, "default_value", HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, "default_value", HintType::kMultiLineEdit, true);
  add_hint(m, "default_value", HintType::kDescription, "The object that will be output by this node, if the corresponding input plug on this group is not connected. This must be a proper javascript expression.");

  return m;
}

bool DataNodeCompute::update_state() {
  Compute::update_state();

  // If _use_override is enabled, we use the _override value
  // exclusively even if its empty.
  if (_use_override) {
    set_output("out", _override);
    return true;
  }

  // This will hold our final output.
  QJSValue output;

  // Start with our data value.
  QString text = get_input_value("default_value").toString();
  std::cerr << "expression is: " << text.toStdString() << "\n";
  QJSValue result;
  QString error;
  if (!evaluate_expression_js(text, result, error)) {
    set_output("out", result); // result contains info about the error as properties.
    on_error();
    return false;
  } else {
    output = deep_merge(output, result);
  }

  // If our input is connected, we merge that value in.
  const Dep<InputCompute>* c = get_input("in");
  if ((*c)->is_connected()) {
    output = deep_merge(output, (*c)->get_output("out"));
  }

  set_output("out", output);
  return true;
}

void DataNodeCompute::set_override(const QJSValue& override) {
  external();
  _override = override;
  _use_override = true;
}

const QJSValue& DataNodeCompute::get_override() const {
  external();
  return _override;
}

void DataNodeCompute::clear_override() {
  _override = QJSValue();
  _use_override = false;
}

}

