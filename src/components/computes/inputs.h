#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/flux.h>

class QJsonValue;
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

  // Get values from inputs.
  QJsonValue get_input_value(const std::string& input_name) const;
  QJsonObject get_input_values() const;

  friend class GroupNodeCompute;
};

}
