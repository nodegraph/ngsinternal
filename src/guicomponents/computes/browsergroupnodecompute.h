#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <components/computes/groupnodecompute.h>
#include <entities/componentids.h>

namespace ngs {

class BaseGroupTraits;

class GUICOMPUTES_EXPORT BrowserGroupNodeCompute: public GroupNodeCompute {
 public:
  COMPONENT_ID(Compute, BrowserGroupNodeCompute);
  BrowserGroupNodeCompute(Entity* entity);
  virtual ~BrowserGroupNodeCompute();

 protected:

  // Our state.
  virtual bool update_state();

 private:
  Dep<BaseGroupTraits> _group_traits;
};

}
