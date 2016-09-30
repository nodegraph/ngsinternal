#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>

namespace ngs {

class InputCompute;

class COMPUTES_EXPORT MockNodeCompute: public Compute {
 public:
  COMPONENT_ID(Compute, MockNodeCompute);
  MockNodeCompute(Entity* entity);
  virtual ~MockNodeCompute();

  virtual void create_inputs_outputs();

 protected:

  // Our state.
  virtual void update_state();

 private:
  unsigned int _counter;
};

}
