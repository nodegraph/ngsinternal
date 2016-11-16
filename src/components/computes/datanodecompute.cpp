#include <base/objectmodel/entity.h>
#include <components/computes/datanodecompute.h>
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

DataNodeCompute::DataNodeCompute(Entity* entity):
    Compute(entity, kDID()),
    _use_override(false) {
}

DataNodeCompute::~DataNodeCompute() {
}

void DataNodeCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("value", "{\"alpha\": 1, \"beta\": 2, \"gamma\": 3}", false);
  create_output("out");
}

const QJsonObject DataNodeCompute::_hints = DataNodeCompute::init_hints();
QJsonObject DataNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "value", HintKey::kJSTypeHint, to_underlying(JSType::kString));
  add_hint(m, "value", HintKey::kMultiLineHint, true);
  add_hint(m, "value", HintKey::kDescriptionHint, "The object that will be output by this node, if the corresponding input plug on this group is not connected. This must be a proper javascript expression.");

  return m;
}

bool DataNodeCompute::update_state() {
  Compute::update_state();

  // This will hold our final output.
  QJsonValue output;

  // Start with our data value.
  QString text = _inputs->get_input_value("value").toString();
  QJsonValue expr_result;
  QString error;
  if (!eval_js_with_inputs(text, expr_result, error)) {
    set_output("out", expr_result); // result contains info about the error as properties.
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

void DataNodeCompute::set_override(const QJsonValue& override) {
  external();
  _override = override;
  _use_override = true;
}

const QJsonValue& DataNodeCompute::get_override() const {
  external();
  return _override;
}

void DataNodeCompute::clear_override() {
  internal();
  _override = QJsonValue();
  _use_override = false;
}

}

