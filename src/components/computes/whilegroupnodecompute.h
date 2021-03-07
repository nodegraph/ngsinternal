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
  virtual void reset_loop_context();

  // Our state.
  virtual bool update_state();
  virtual void init_dirty_state();

  size_t _infinite_counter;

  bool _do_next;
};

}
