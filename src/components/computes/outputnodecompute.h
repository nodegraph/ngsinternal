#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>

namespace ngs {

class COMPUTES_EXPORT OutputNodeCompute: public Compute {
 public:
  COMPONENT_ID(Compute, OutputNodeCompute);
  OutputNodeCompute(Entity* entity);
  virtual ~OutputNodeCompute();

  // Our topology.
  virtual void create_inputs_outputs();

  // Our hints.
  static QVariantMap init_hints();
  static const QVariantMap _hints;
  virtual const QVariantMap& get_hints() const {return _hints;}

 protected:
  // Our state.
  virtual bool update_state();

};

}
