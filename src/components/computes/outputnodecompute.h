#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>

namespace ngs {

class COMPUTES_EXPORT OutputNodeCompute: public Compute {
 public:

  COMPONENT_ID(Compute, OutputNodeCompute);

  OutputNodeCompute(Entity* entity);
  virtual ~OutputNodeCompute();

  // Our state.
  virtual void update_state();

};

}
