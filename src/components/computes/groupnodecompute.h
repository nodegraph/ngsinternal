#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>


namespace ngs {

class BaseFactory;
class InputShape;
class OutputShape;
class LinkShape;

class COMPUTES_EXPORT GroupNodeCompute: public Compute {
 public:
  COMPONENT_ID(Compute, GroupNodeCompute);
  GroupNodeCompute(Entity* entity);
  virtual ~GroupNodeCompute();

  virtual void create_inputs_outputs();

 protected:

  // Our state.
  virtual void update_wires();
  virtual void update_state();

 private:
  Dep<BaseFactory> _factory;
};

}
