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
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig()) {external();}

  // Exposure.
  void set_exposed(bool exposed);
  bool is_exposed() const;

  // Serialization.
  virtual void save(SimpleSaver& saver) const;
  virtual void load(SimpleLoader& loader);

 protected:

  // Our state.
  virtual bool update_state();

 private:
  // Our fixed deps.
  Dep<Compute> _node_compute;

  // Exposure.
  bool _exposed;

  friend class InputCompute;
  friend class OutputShape;
};


}
