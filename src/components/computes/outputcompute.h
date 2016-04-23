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

  // Our state.
  virtual void update_state();

 private:
  // Our fixed deps.
  Dep<Compute> _node_compute;

  friend class InputCompute;
  friend class OutputShape;
};


}
