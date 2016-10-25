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
    Compute(entity, kDID()){
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
  QVariantMap output;
  bool error_occurred = false;

  // If we're connected start with that value.
  const Dep<InputCompute> &c = _inputs->get_all().at("in");
  if (c->is_connected()) {
    Compute::merge_maps(output, c->get_output("out").toMap());
  }

  // Merge in the json data value.

  // Get the json text from the json parameter.
  QString text("");
  {
    const std::unordered_map<std::string, Dep<InputCompute> >& inputs = _inputs->get_all();
    assert(inputs.count("json"));
    QVariantMap json_param = inputs.at("json")->get_output("out").toMap();
    if (json_param.count("value")) {
      text = json_param.find("value").value().toString();
    }
  }

  // Create a variant map from the json string.'
  QJsonParseError err;
  QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(), &err);
  if (err.error != QJsonParseError::NoError) {
    output["error"] = (err.errorString());
    error_occurred = true;
  } else if (!doc.isObject()) {
    output["error"] = "the json string must represent an object.";
    error_occurred = true;
  } else {
    QJsonObject obj = doc.object();
    Compute::merge_maps(output,obj.toVariantMap());
  }

  // Finally if we have a valid override, let's merge that in.
  if (_override.isValid() && (!_override.toMap().isEmpty())) {
    Compute::merge_maps(output, _override.toMap());
  }

  set_output("out", output);

  if (error_occurred) {
    _ng_manipulator->set_error_node();
    _ng_manipulator->clear_ultimate_target();
    return false;
  }
  return true;
}

void DataNodeCompute::set_override(const QVariant& override) {
  external();
  _override = override;
}

QVariant DataNodeCompute::get_override() const {
  external();
  return _override;
}

void DataNodeCompute::clear_override() {
  _override.clear();
}

}

