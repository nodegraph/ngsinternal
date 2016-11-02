#include <base/objectmodel/entity.h>
#include <components/computes/mergenodecompute.h>
#include <components/computes/inputcompute.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

namespace ngs {

// Input Nodes don't have input plugs, but they are associated
// with an input plug on the surrounding group.
// The InputComputer mainly passes these calls onto its parent group node.

MergeNodeCompute::MergeNodeCompute(Entity* entity):
    Compute(entity, kDID()),
    _use_override(false) {
}

MergeNodeCompute::~MergeNodeCompute() {
}

void MergeNodeCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("in", QJsonValue());
  create_input("default_value", "3 * 9 + 3", false);
  create_output("out");
}

const QJsonObject MergeNodeCompute::_hints = MergeNodeCompute::init_hints();
QJsonObject MergeNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "in", HintType::kJSType, to_underlying(JSType::kObject));
  add_hint(m, "in", HintType::kDescription, "The main object that flows through this node. This cannot be set manually.");


  add_hint(m, "default_value", HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, "default_value", HintType::kMultiLineEdit, true);
  add_hint(m, "default_value", HintType::kDescription, "The object that will be output by this node, if the corresponding input plug on this group is not connected. This must be a proper javascript expression.");

  return m;
}

bool MergeNodeCompute::update_state() {
  Compute::update_state();

  // This will hold our final output.
  QJsonValue output;

  // Start with our data value.
  QString text = _inputs->get_input_value("default_value").toString();
  QJsonValue expr_result;
  QString error;
  if (!evaluate_expression_js(text, expr_result, error)) {
    set_output("out", expr_result); // result contains info about the error as properties.
    on_error();
    return false;
  } else {
    output = deep_merge(output, expr_result);
  }

  // If our input is connected, we merge that value in.
  const Dep<InputCompute>& c = _inputs->get("in");
  if (c->is_connected()) {
    output = deep_merge(output, c->get_output("out"));
  }

  // If there is an override then merge that in.
  if (_use_override) {
    output = deep_merge(output, _override);
  }

  set_output("out", output);
  return true;
}

void MergeNodeCompute::set_override(const QJsonValue& override) {
  external();
  _override = override;
  _use_override = true;
}

const QJsonValue& MergeNodeCompute::get_override() const {
  external();
  return _override;
}

void MergeNodeCompute::clear_override() {
  _override = QJsonValue();
  _use_override = false;
}

}

