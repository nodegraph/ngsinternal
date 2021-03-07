#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/flux.h>

class QString;
class QJsonValue;
class QJsonArray;
class QJsonObject;

namespace ngs {

//--------------------------------------------------------------------------
// The Inputs class adds additional input specific methods onto the base Flux.
//--------------------------------------------------------------------------

class COMPUTES_EXPORT Inputs: public Flux<InputTraits> {
 public:
  Inputs(Entity* entity);
  virtual ~Inputs();

  // Query an inputs state.
  virtual bool is_connected(const std::string& name) const;

  // Get exposure settings.
  virtual QJsonObject get_exposure() const;
  virtual void set_exposure(const QJsonObject& settings);

  // Get editable values. Editable values come from inputs that are not connected.
  virtual QJsonObject get_editable_inputs() const;
  virtual void set_editable_inputs(const QJsonObject& inputs);

  // Get value from inputs.
  QJsonValue get_input_value(const std::string& input_name) const;
  QJsonValue get_unset_value(const std::string& input_name) const;

  // Get value from input, but also do a conversion to a certain json type.
  double get_input_double(const std::string& input_name) const;
  bool get_input_boolean(const std::string& input_name) const;
  QString get_input_string(const std::string& input_name) const;
  QJsonArray get_input_array(const std::string& input_name) const;
  QJsonObject get_input_object(const std::string& input_name) const;

  // Get the main input as an object.
  QJsonObject get_main_input_object() const;

  // Get all input values.
  QJsonObject get_input_values() const;

  friend class GroupNodeCompute;
};

}
