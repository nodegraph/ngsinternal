#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/groupnodecompute.h>

namespace ngs {

class COMPUTES_EXPORT ForEachGroupNodeCompute: public GroupNodeCompute {
 public:
  COMPONENT_ID(Compute, ForEachGroupNodeCompute);
  ForEachGroupNodeCompute(Entity* entity, ComponentDID did = kDID());
  virtual ~ForEachGroupNodeCompute();

 protected:
  // Our state.
  virtual bool update_state();
  virtual void set_self_dirty(bool dirty);

  void reset_accumulate_data_nodes();

  bool _restart_loop;
};

}
