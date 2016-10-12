#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <components/computes/compute.h>

namespace ngs {


class GUICOMPUTES_EXPORT ScriptNodeCompute: public Compute  {
 public:
  COMPONENT_ID(Compute, ScriptNodeCompute);
  ScriptNodeCompute(Entity* entity);
  virtual ~ScriptNodeCompute();

  virtual void create_inputs_outputs();

protected:
  // Our state.
  virtual bool update_state();

 private:
  QString _script;
};

}
