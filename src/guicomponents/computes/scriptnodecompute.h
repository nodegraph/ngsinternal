#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <components/computes/compute.h>
#include <base/objectmodel/dep.h>

namespace ngs {

class GUICOMPUTES_EXPORT ScriptNodeCompute: public Compute  {
 public:
  COMPONENT_ID(Compute, ScriptNodeCompute);
  ScriptNodeCompute(Entity* entity);
  virtual ~ScriptNodeCompute();

  virtual void create_inputs_outputs();

  static QJSValue init_hints();
  static const QJSValue _hints;
  virtual const QJSValue& get_hints() const {return _hints;}

protected:
  // Our state.
  virtual bool update_state();

 private:
  QString _script;
};

}
