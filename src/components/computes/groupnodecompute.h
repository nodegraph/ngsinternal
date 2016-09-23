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

  // Our state.
  virtual void update_state();
  virtual HierarchyUpdate update_hierarchy();

 private:
  Dep<BaseFactory> _factory;
  Dep<LowerHierarchyChange> _lower_change;
};

}
