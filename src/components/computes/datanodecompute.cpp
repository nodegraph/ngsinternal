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
  create_input("in", QVariantMap());
  create_input("json", "{\n\t\"value\": 0\n}", false);
  create_output("out");
}

const QVariantMap DataNodeCompute::_hints = DataNodeCompute::init_hints();
QVariantMap DataNodeCompute::init_hints() {
  QVariantMap m;

  add_hint(m, "in", HintType::kJSType, to_underlying(JSType::kObject));
  add_hint(m, "in", HintType::kDescription, "The main object that flows through this node. This cannot be set manually.");


  add_hint(m, "json", HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, "json", HintType::kMultiLineEdit, true);
  add_hint(m, "json", HintType::kDescription, "The object that will be output by this node, if the corresponding input plug on this group is not connected. This must be specified in JSON format and must represent an object.");

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

  QVariantMap output;
  bool error_occurred = false;

  // If we're connected start with that value.
  const Dep<InputCompute> &c = _inputs->get_all().at("in");
  if (c->is_connected()) {
    Compute::merge_maps(output, c->get_output("out"));
  }

  // Merge in the json data value.

  // Get the json text from the json parameter.
  QString text = get_input_value_element("json").toString();

  // Evaluate the json string.
  QJsonParseError err;
  QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(), &err);
  if (err.error != QJsonParseError::NoError) {
    output["error"] = err.errorString();
    error_occurred = true;
  } else if (!doc.isObject()) {
    output["error"] = "the json string must represent an object.";
    error_occurred = true;
  } else {
    QJsonObject obj = doc.object();
    Compute::merge_maps(output,obj.toVariantMap());
  }

  set_output("out", output);

  if (error_occurred) {
    _ng_manipulator->set_error_node();
    _ng_manipulator->clear_ultimate_target();
    return false;
  }
  return true;
}

void DataNodeCompute::set_override(const QVariantMap& override) {
  external();
  _override = override;
  _use_override = true;
}

QVariantMap DataNodeCompute::get_override() const {
  external();
  return _override;
}

void DataNodeCompute::clear_override() {
  _override.clear();
  _use_override = false;
}

}

