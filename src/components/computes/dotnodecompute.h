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

  // Our state.
  virtual void update_state();

};


}
