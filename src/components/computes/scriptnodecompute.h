#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>

namespace ngs {


class COMPUTES_EXPORT ScriptNodeCompute: public Compute  {
 public:

  COMPONENT_ID(Compute, ScriptNodeCompute);

  ScriptNodeCompute(Entity* entity);
  virtual ~ScriptNodeCompute();

  // Our state.
  virtual void update_state();

 private:
  QString _script;
};

}
