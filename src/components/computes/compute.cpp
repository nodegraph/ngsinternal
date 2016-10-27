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
#include <QtQml/QJSValueIterator>

#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

#include <sstream>

namespace ngs {

struct InputComputeComparator {
  bool operator()(const Dep<InputCompute>& left, const Dep<InputCompute>& right) const {
    return left->get_name() < right->get_name();
  }
};

const QJSValue Compute::_hints;

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

QJSValue Compute::get_editable_inputs() const {
  external();
  QJSValue values;
  for (auto iter: _inputs->get_all()) {
    if (!iter.second->is_connected()) {
      values.setProperty(QString::fromStdString(iter.first), iter.second->get_unconnected_value());
    }
  }
  return values;
}

void Compute::set_editable_inputs(const QJSValue& values) {
  const std::unordered_map<std::string, Dep<InputCompute> >& inputs = _inputs->get_all();
  QJSValueIterator values_iter(values);
  while (values_iter.hasNext()) {
    values_iter.next();
    QString name = values_iter.name();
    if (inputs.count(name.toStdString())) {
          const Dep<InputCompute> *c = get_input(name.toStdString());
          if (c) {
            (*c)->set_unconnected_value(values_iter.value());
          }
        }
  }
}

const Dep<InputCompute>* Compute::get_input(const std::string& name) const {
  const std::unordered_map<std::string, Dep<InputCompute> >& inputs = _inputs->get_all();
  if (!inputs.count(name)) {
    return NULL;
  }
  return &inputs.at(name);
}

QJSValue Compute::get_input_value(const std::string& input_name, const std::string& output_port_name) const {
  const Dep<InputCompute>* input = get_input(input_name);
  if (!input) {
    return QJSValue();
  }
  return (*input)->get_output(output_port_name);
}

QJSValue Compute::get_input_values(const std::string& output_port_name) const {
  const std::unordered_map<std::string, Dep<InputCompute> >& inputs = _inputs->get_all();
  QJSValue result;
  for (auto &iter: inputs) {
    result.setProperty(QString::fromStdString(iter.first), iter.second->get_output(output_port_name));
  }
  return result;
}

const QJSValue& Compute::get_outputs() const {
  external();
  return _outputs;
}

void Compute::set_outputs(const QJSValue& outputs) {
  internal();
  _outputs = outputs;
}

QJSValue Compute::get_output(const std::string& name) const{
  external();
  if (!_outputs.hasOwnProperty(name.c_str())) {
    return QJSValue();
  }
  return _outputs.property(name.c_str());
}

void Compute::set_output(const std::string& name, const QJSValue& value) {
  internal();
  _outputs.setProperty(name.c_str(), value);
}

QJSValue Compute::get_input_exposure() const {
  external();
  QJSValue results;
  for (auto iter: _inputs->get_all()) {
    if (!iter.second) {
      continue;
    }
    results.setProperty(iter.first.c_str(), iter.second->is_exposed());
  }
  return results;
}

void Compute::set_input_exposure(const QJSValue& settings) {
  const std::unordered_map<std::string, Dep<InputCompute> >& inputs = _inputs->get_all();
  QJSValueIterator settings_iter(settings);
  while (settings_iter.hasNext()) {
    settings_iter.next();
    std::string name = settings_iter.name().toStdString();
    if (inputs.count(name)) {
      const Dep<InputCompute> *c = get_input(name);
      (*c)->set_exposed(settings_iter.value().toBool());
    }
  }
}



QJSValue Compute::deep_merge(const QJSValue& target, const QJSValue& source) {
  if (target.isObject()) {
    if (source.isObject()) {
      QJSValue object = target;
      QJSValueIterator source_iter(source);
      while (source_iter.hasNext()) {
        source_iter.next();
        QString name = source_iter.name();
        if (target.hasProperty(name)) {
          object.setProperty(name, deep_merge(target.property(name), source_iter.value()));
        } else {
          object.setProperty(name, source_iter.value());
        }
      }
      return object;
    } else {
      // Otherwise source data doesn't make it into the target.
      return target;
    }
  } else if (target.isArray()) {
    if (source.isArray()) {
      // The first element of the target list acts as a prototype if present.
      QJSValue proto;
      QJSValueIterator target_iter(target);
      if (target_iter.hasNext()) {
        target_iter.next();
        proto = target_iter.value();
      }
      QJSValue result;
      QJSValueIterator source_iter(source);
      while(source_iter.hasNext()) {
        source_iter.next();
        result.setProperty(source_iter.name(), deep_merge(proto, source_iter.value()));
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
  } else {
    if (target.isBool()) {
      return source.toBool();
    } else if (target.isNumber()) {
      return target.toNumber();
    } else if (target.isString()) {
      return target.toString();
    }
  }
  return target;
}

Entity* Compute::create_input(const std::string& name, const QJSValue& value, bool exposed) {
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

void Compute::add_hint(QJSValue& map,
                       const std::string& name,
                       HintType hint_type,
                       const QJSValue& value) {
  QJSValue hints = map.property(name.c_str());
  hints.setProperty(QString::number(to_underlying(hint_type)), value);
  map.setProperty(name.c_str(),  hints);
}

bool Compute::evaluate_expression_js(const QString& text, QJSValue& result, QString& error) const {
  internal();
  QJSEngine engine;

  // Add the input values into the context.
  for (auto &iter: _inputs->get_all()) {
    const Dep<InputCompute>& input = iter.second;
    QJSValue value = input->get_output("out");
    const std::string& input_name = input->get_name();
    engine.globalObject().setProperty(QString::fromStdString(input_name), value);
  }

  // Evaluate the expression.
  result = engine.evaluate(text, "expression", 0);
  if (result.isError()) {
    // Update the error string.
    std::stringstream ss;
    ss << "Uncaught exception at line: " << result.property("lineNumber").toInt() << "\n";
    ss << "name: " << result.property("name").toString().toStdString() << "\n";
    ss << "message: " << result.property("message").toString().toStdString() << "\n";
    ss << "stack: " << result.property("stack").toString().toStdString() << "\n";
    error = ss.str().c_str();
    return false;
  }
  std::cerr << "expression result is: " << result.toString().toStdString() << "\n";
  return true;
}

void Compute::on_error() {
  _ng_manipulator->set_error_node();
  _ng_manipulator->clear_ultimate_target();
}

}
