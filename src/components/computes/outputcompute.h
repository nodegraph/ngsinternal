#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>

namespace ngs {

class Entity;

class COMPUTES_EXPORT OutputCompute: public Compute {
 public:
  COMPONENT_ID(Compute, OutputCompute);
  OutputCompute(Entity* entity);
  virtual ~OutputCompute();

  // We override to stop creating the inputs and outputs namespace.
  virtual void create_inputs_outputs() {external();}

  // Our data type.
  void set_param_type(JSType param_type);
  JSType get_param_type() const;

  // Exposure.
  void set_exposed(bool exposed);
  bool is_exposed() const;

  // Serialization.
  virtual void save(SimpleSaver& saver) const;
  virtual void load(SimpleLoader& loader);

 protected:

  // Our state.
  virtual void update_wires();
  virtual bool update_state();

 private:
  // Our fixed deps.
  Dep<Compute> _node_compute;

  // Our data type.
  JSType _param_type;

  // Exposure.
  bool _exposed;

  friend class InputCompute;
  friend class OutputShape;
};


}
