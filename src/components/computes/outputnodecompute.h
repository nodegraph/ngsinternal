#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>

namespace ngs {

class COMPUTES_EXPORT OutputNodeCompute: public Compute {
 public:

  COMPONENT_ID(Compute, OutputNodeCompute);

  OutputNodeCompute(Entity* entity);
  virtual ~OutputNodeCompute();

  virtual void create_inputs_outputs();

 protected:
  // Our state.
  virtual void update_state();

};

}
