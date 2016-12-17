#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/groupnodecompute.h>

namespace ngs {

class COMPUTES_EXPORT WhileGroupNodeCompute: public GroupNodeCompute {
 public:
  COMPONENT_ID(Compute, WhileGroupNodeCompute);
  WhileGroupNodeCompute(Entity* entity, ComponentDID did = kDID());
  virtual ~WhileGroupNodeCompute();

 protected:
  // Our state.
  virtual bool update_state();
  virtual void set_self_dirty(bool dirty);

  bool _restart_loop;
};

}
