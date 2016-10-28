#include <components/computes/inputs.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <QtCore/QJsonValue>
#include <QtCore/QJsonObject>


namespace ngs {

// -------------------------------------------------------------------------------------------------------------------
// Inputs.
// -------------------------------------------------------------------------------------------------------------------

Inputs::Inputs(Entity* entity):
    Flux<InputTraits>(entity){
}

Inputs::~Inputs() {
}

bool Inputs::is_connected(const std::string& name) const {
  external();
  const Dep<InputCompute>& dep = get(name);
  if (!dep) {
    return false;
  }
  return dep->is_connected();
}

QJsonObject Inputs::get_exposure() const {
  external();
  QJsonObject values;
  for (auto iter: _all) {
    values.insert(iter.first.c_str(), iter.second->is_exposed());
  }
  return values;
}

void Inputs::set_exposure(const QJsonObject& settings) {
  for (QJsonObject::const_iterator iter = settings.constBegin(); iter != settings.constEnd(); ++iter) {
    std::string name = iter.key().toStdString();
    const Dep<InputCompute> & dep = get(name);
    if (dep) {
      dep->set_exposed(iter.value().toBool());
    }
  }
}

QJsonObject Inputs::get_editable_inputs() const {
  external();
  QJsonObject values;
  for (auto iter: _all) {
    if (!iter.second->is_connected()) {
      values.insert(iter.first.c_str(), iter.second->get_unconnected_value());
    }
  }
  return values;
}

void Inputs::set_editable_inputs(const QJsonObject& values) {
  for (QJsonObject::const_iterator iter = values.constBegin(); iter != values.constEnd(); ++iter) {
    std::string name = iter.key().toStdString();
    const Dep<InputCompute> & dep = get(name);
    if (dep) {
        dep->set_unconnected_value(iter.value());
    }
  }
}

QJsonValue Inputs::get_input_value(const std::string& input_name, const std::string& port_name) const {
  const Dep<InputCompute>& input = get(input_name);
  if (!input) {
    return QJsonValue();
  }
  return input->get_output(port_name);
}

QJsonObject Inputs::get_input_values(const std::string& port_name) const {
  QJsonObject result;
  for (auto &iter: _all) {
    result.insert(iter.first.c_str(), iter.second->get_output(port_name));
  }
  return result;
}



}