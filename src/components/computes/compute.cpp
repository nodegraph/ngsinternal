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
#include <QtCore/QTimer>

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
      _manipulator(this) {
  // Note this only exists for node computes and not for plug computes.
  get_dep_loader()->register_fixed_dep(_inputs, Path({"."}));
  // We only grab the manipulator for non input/output computes, to avoid cycles.
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
  if (_manipulator) {
      _manipulator->update_clean_marker(our_entity(), !is_state_dirty());
  }
}

void Compute::initialize_wires() {
  Component::initialize_wires();
  if ((get_did() != ComponentDID::kInputCompute) && (get_did() != ComponentDID::kOutputCompute)) {
    _manipulator = get_dep<BaseNodeGraphManipulator>(get_app_root());
  }
}

bool Compute::update_state() {
  internal();
  // Notify the gui side that a computation is now processing on the compute side.
  if (_manipulator) {
    _manipulator->set_processing_node(our_entity());
  }

  return true;
}

bool Compute::clean_finalize() {
  internal();
  Component::clean_finalize();
  // Notify the gui side that a computation is now processing on the compute side.
  if (_manipulator) {
    _manipulator->clear_processing_node();
  }
  return true;
}

void Compute::update_input_flux() {
  //_inputs->clean_wires();
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

QString Compute::value_to_json(QJsonValue value) {
  QJsonDocument doc;
  if (value.isArray()) {
    doc.setArray(value.toArray());
  } else if (value.isObject()) {
    doc.setObject(value.toObject());
  }
  return doc.toJson();
}

bool Compute::eval_json(const QString& json, QJsonValue& result, QString& error) {
  error.clear();

  QJsonParseError parse_error;
  QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &parse_error);

  if (parse_error.error == QJsonParseError::NoError) {
    result = QJsonValue(); // undefined.
    error = parse_error.errorString();
    return false;
  }

  if (doc.isObject()) {
    result = doc.object();
    return true;
  } else if (doc.isArray()) {
    result = doc.array();
    return true;
  }

  return false;
}

void Compute::shallow_object_merge(QJsonObject& target, const QJsonObject& source) {
  for (QJsonObject::const_iterator iter = source.constBegin(); iter != source.constEnd(); ++iter) {
    target.insert(iter.key(), iter.value());
  }
}

void Compute::prep_source_for_merge(const QJsonValue& target, QJsonValue& source) {
  if (source.isString()) {
    QJsonValue value = eval_js2(source.toString());
    if (target.isObject()) {
      if (value.isObject()) {
        // Conver the source from a string to an object.
        source = value;
        return;
      }
    } else if (target.isArray()) {
      if (value.isArray()) {
        // Convert the source from a string to an array.
        source = value;
        return;
      }
    }
  } else if (source.isObject()) {
    if (target.isString()) {
      // Convert the source from an object to a string.
      QString json = value_to_json(source);
      source = QJsonValue(json);
    }
  } else if (source.isArray()) {
    if (target.isString()) {
      // Conver the source from an array to a string.
      QString json = value_to_json(source);
      source = QJsonValue(json);
    }
  }
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

bool Compute::eval_js_with_inputs(const QString& text, QJsonValue& result, QString& error) const {
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

  return eval_js_in_context(engine, text, result,error);
}

bool Compute::eval_js(const QString& expr, QJsonValue& result, QString& error) {
  is_static();
  QJSEngine engine;
  return eval_js_in_context(engine, expr, result, error);
}

QJsonValue Compute::eval_js2(const QString& expr) {
  is_static();
  QJSEngine engine;
  QJsonValue result;
  QString error;
  eval_js_in_context(engine, expr, result, error);
  return result;
}

bool Compute::eval_js_in_context(QJSEngine& engine, const QString& expr, QJsonValue& result, QString& error) {
  is_static();

  // Evaluate the expression.
  QJSValue jsresult;
  jsresult = engine.evaluate(expr, "expression", 0);
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

void Compute::on_error(const QString& error_message) {
  _manipulator->set_error_node(error_message);
  _manipulator->clear_ultimate_targets();
}

}
