#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <components/computes/groupnodecompute.h>
#include <entities/componentids.h>

namespace ngs {

class BaseGroupTraits;

class GUICOMPUTES_EXPORT WebGroupNodeCompute: public GroupNodeCompute {
 public:
  COMPONENT_ID(Compute, WebGroupNodeCompute);
  WebGroupNodeCompute(Entity* entity);
  virtual ~WebGroupNodeCompute();


  virtual void dirty_web_computes();

 protected:

  // Our state.
  virtual bool update_state();

 private:
  Dep<BaseGroupTraits> _group_traits;
};

}
