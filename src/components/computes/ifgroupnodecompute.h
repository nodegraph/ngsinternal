#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/groupnodecompute.h>

namespace ngs {

class COMPUTES_EXPORT IfGroupNodeCompute: public GroupNodeCompute {
 public:
  COMPONENT_ID(Compute, IfGroupNodeCompute);
  IfGroupNodeCompute(Entity* entity, ComponentDID did = kDID());
  virtual ~IfGroupNodeCompute();
 protected:
  // Our state.
  virtual bool update_state();
};

}
