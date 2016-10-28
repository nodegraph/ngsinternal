#include <components/computes/compute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <base/objectmodel/deploader.h>
#include <entities/entityids.h>
#include <entities/entityinstancer.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlExpression>

#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>

#include <QtCore/QJsonValue>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include <sstream>

namespace ngs {

struct InputComputeComparator {
  bool operator()(const Dep<InputCompute>& left, const Dep<InputCompute>& right) const {
    return left->get_name() < right->get_name();
  }
};

const QJsonObject Compute::_hints;

Compute::Compute(Entity* entity, ComponentDID derived_id)
    : Component(entity, kIID(), derived_id),
      _inputs(this),
      _ng_manipulator(this) {
  // Note this only exists for node computes and not for plug computes.
  get_dep_loader()->register_fixed_dep(_inputs, Path({"."}));
  // We only grab the _ng_manipulator for non input/output computes, to avoid cycles.
}

Compute::~Compute() {
}

void Compute::create_inputs_outputs() {
  external();
  create_namespace("inputs");
  create_namespace("outputs");
}

void Compute::set_self_dirty(bool dirty) {
  Component::set_self_dirty(dirty);
  if (_ng_manipulator) {
      _ng_manipulator->update_clean_marker(our_entity(), !is_state_dirty());
  }
}

void Compute::initialize_wires() {
  Component::initialize_wires();
  if ((get_did() != ComponentDID::kInputCompute) && (get_did() != ComponentDID::kOutputCompute)) {
    _ng_manipulator = get_dep<BaseNodeGraphManipulator>(get_app_root());
  }
}

bool Compute::update_state() {
  internal();
  // Notify the gui side that a computation is now processing on the compute side.
  if (_ng_manipulator) {
    _ng_manipulator->set_processing_node(our_entity());
  }

  return true;
}

bool Compute::clean_finalize() {
  internal();
  Component::clean_finalize();
  // Notify the gui side that a computation is now processing on the compute side.
  if (_ng_manipulator) {
    _ng_manipulator->clear_processing_node();
  }
  return true;
}

void Compute::update_input_flux() {
  _inputs->clean_wires();
}

QJsonObject Compute::get_editable_inputs() const {
  return _inputs->get_editable_inputs();
}

void Compute::set_editable_inputs(const QJsonObject& inputs) {
  _inputs->set_editable_inputs(inputs);
}

QJsonObject Compute::get_input_exposure() const {
  return _inputs->get_exposure();
}

void Compute::set_input_exposure(const QJsonObject& settings) {
  _inputs->set_exposure(settings);
}

const QJsonObject& Compute::get_outputs() const {
  external();
  return _outputs;
}

void Compute::set_outputs(const QJsonObject& outputs) {
  internal();
  _outputs = outputs;
}

QJsonValue Compute::get_output(const std::string& name) const{
  external();
  if (!_outputs.contains(name.c_str())) {
    return QJsonValue();
  }
  return _outputs.value(name.c_str());
}

void Compute::set_output(const std::string& name, const QJsonValue& value) {
  internal();
  _outputs.insert(name.c_str(), value);
}

QJsonValue Compute::deep_merge(const QJsonValue& target, const QJsonValue& source) {
  if (source.isNull()) {
    return target;
  } else if (source.isUndefined()) {
    return target;
  }else if (target.isObject()) {
    if (source.isObject()) {
      QJsonObject tobj = target.toObject();
      QJsonObject sobj = source.toObject();
      for (QJsonObject::const_iterator siter = sobj.constBegin(); siter != sobj.constEnd(); ++siter) {
        QString name = siter.key();
        if (tobj.contains(name)) {
          tobj.insert(name, deep_merge(tobj[name], siter.value()));
        } else {
          tobj.insert(name, siter.value());
        }
      }
      return tobj;
    } else {
      // Otherwise source data doesn't make it into the target.
      return target;
    }
  } else if (target.isArray()) {
    if (source.isArray()) {
      QJsonArray tarr = target.toArray();
      QJsonArray sarr = source.toArray();
      // The first element of the target list acts as a prototype if present.
      QJsonValue proto;
      if (!tarr.empty()) {
        proto = tarr.at(0);
      }
      QJsonArray result;
      for (int i = 0; i < sarr.size(); ++i) {
        result[i] = deep_merge(proto, sarr.at(i));
      }
      return result;
    } else {
      // Otherwise source data doesn't make it into the target.
      return target;
    }
  } else if (target.isNull()) {
    return source;
  } else if (target.isUndefined()) {
    return source;
  } else if (target.isBool()) {
    if (source.isBool()) {
      return source;
    } else {
      return target;
    }
  } else if (target.isDouble()) {
    if (source.isDouble()) {
      return source;
    } else {
      return target;
    }
  } else if (target.isString()) {
    if (source.isString()) {
      return source;
    } else {
      return target;
    }
  }
  return target;
}

Entity* Compute::create_input(const std::string& name, const QJsonValue& value, bool exposed) {
  external();
  Dep<BaseFactory> factory = get_dep<BaseFactory>(Path({}));
  Entity* inputs_space = get_inputs_space();
  InputEntity* input = static_cast<InputEntity*>(factory->instance_entity(inputs_space, name, EntityDID::kInputEntity));
  input->create_internals();
  input->set_exposed(exposed);
  input->set_unconnected_value(value);
  return input;
}

Entity* Compute::create_output(const std::string& name, bool exposed) {
  external();
  Dep<BaseFactory> factory = get_dep<BaseFactory>(Path({}));
  Entity* outputs_space = get_outputs_space();
  OutputEntity* output = static_cast<OutputEntity*>(factory->instance_entity(outputs_space, name, EntityDID::kOutputEntity));
  output->create_internals();
  output->set_exposed(exposed);
  return output;
}

Entity* Compute::create_namespace(const std::string& name) {
  external();
  Dep<BaseFactory> factory = get_dep<BaseFactory>(Path({}));
  Entity* space = static_cast<BaseNamespaceEntity*>(factory->instance_entity(our_entity(), name, EntityDID::kBaseNamespaceEntity));
  space->create_internals();
  return space;
}

Entity* Compute::get_inputs_space() {
  external();
  return our_entity()->get_child("inputs");
}

Entity* Compute::get_outputs_space() {
  external();
  return our_entity()->get_child("outputs");
}

void Compute::add_hint(QJsonObject& map,
                       const std::string& name,
                       HintType hint_type,
                       const QJsonValue& value) {
  QJsonObject hints = map.value(name.c_str()).toObject();
  hints.insert(QString::number(to_underlying(hint_type)), value);
  map.insert(name.c_str(),  hints);
}

bool Compute::evaluate_expression_js(const QString& text, QJsonValue& result, QString& error) const {
  internal();
  QJSEngine engine;

  // Add the input values into the context.
  for (auto &iter: _inputs->get_all()) {
    const Dep<InputCompute>& input = iter.second;
    QJsonValue value = input->get_output("out");
    const std::string& input_name = input->get_name();
    QJSValue jsvalue = engine.toScriptValue(value);
    engine.globalObject().setProperty(QString::fromStdString(input_name), jsvalue);
  }

  // Evaluate the expression.
  QJSValue jsresult;
  jsresult = engine.evaluate(text, "expression", 0);
  if (jsresult.isError()) {
    // Update the error string.
    std::stringstream ss;
    ss << "Uncaught exception at line: " << jsresult.property("lineNumber").toInt() << "\n";
    ss << "name: " << jsresult.property("name").toString().toStdString() << "\n";
    ss << "message: " << jsresult.property("message").toString().toStdString() << "\n";
    ss << "stack: " << jsresult.property("stack").toString().toStdString() << "\n";
    error = ss.str().c_str();
    std::cerr << ss.str() << "\n";
    return false;
  }

  result = engine.fromScriptValue<QJsonValue>(jsresult);
  std::cerr << "js result is: " << jsresult.toVariant().toString().toStdString() << "\n";
  std::cerr << "expression result is: " << result.toString().toStdString() << "\n";
  return true;
}

void Compute::on_error() {
  _ng_manipulator->set_error_node();
  _ng_manipulator->clear_ultimate_target();
}

}
