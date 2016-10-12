#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>


namespace ngs {

class Entity;
class InputCompute;

class COMPUTES_EXPORT DotNodeCompute: public Compute {
 public:
  COMPONENT_ID(Compute, DotNodeCompute);
  DotNodeCompute(Entity* entity);
  virtual ~DotNodeCompute();

  virtual void create_inputs_outputs();

protected:
  // Our state.
  virtual bool update_state();
};


}
