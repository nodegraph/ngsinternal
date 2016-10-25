#include <base/objectmodel/entity.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/inputcompute.h>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

namespace ngs {

// Input Nodes don't have input plugs, but they are associated
// with an input plug on the surrounding group.
// The InputComputer mainly passes these calls onto its parent group node.

InputNodeCompute::InputNodeCompute(Entity* entity):
    Compute(entity, kDID()){
}

InputNodeCompute::~InputNodeCompute() {
}

void InputNodeCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("json", "{\"value\": 0}", false);
  create_output("out");
}

const QVariantMap InputNodeCompute::_hints = InputNodeCompute::init_hints();
QVariantMap InputNodeCompute::init_hints() {
  QVariantMap m;

  add_hint(m, "json", HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, "json", HintType::kDescription, "The object that will be output by this node, if the corresponding input plug on this group is not connected. This must be specified in JSON format and must represent an object.");

  return m;
}

bool InputNodeCompute::update_state() {
  Compute::update_state();

  // If we have a valid override, let's use that.
  if (_override.isValid() && (!_override.toMap().isEmpty())) {
    set_output("out", _override);
    return true;
  }

  // Otherwise use the json parameter for the output value.

  // Get the json string from the json parameter.
  const std::unordered_map<std::string, Dep<InputCompute> >& inputs = _inputs->get_all();
  assert(inputs.count("json"));

  QVariantMap upstream = inputs.at("json")->get_output("out").toMap();
  QString text("");

  if (upstream.count("value")) {
    text = upstream.find("value").value().toString();
  }

  // Create a variant map from the json string.'
  QJsonParseError err;
  QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(), &err);
  QVariantMap map;
  if (err.error != QJsonParseError::NoError) {
    map["value"] = (QString("parse error: ") + err.errorString());
  } else if (!doc.isObject()) {
    map["value"] = "parse error: the json string must represent an object.";
  } else {
    QJsonObject obj = doc.object();
    map = obj.toVariantMap();
  }
  set_output("out", map);
  return true;
}

void InputNodeCompute::set_override(const QVariant& override) {
  external();
  _override = override;
}

QVariant InputNodeCompute::get_override() const {
  external();
  return _override;
}

void InputNodeCompute::clear_override() {
  _override.clear();
}

}

