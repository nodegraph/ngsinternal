#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>

namespace ngs {

class COMPUTES_EXPORT EraseDataNodeCompute: public Compute {
 public:
  COMPONENT_ID(Compute, EraseDataNodeCompute);
  EraseDataNodeCompute(Entity* entity);
  virtual ~EraseDataNodeCompute();

  // Our topology.
  virtual void create_inputs_outputs(const EntityConfig& config);

  // Our hints.
  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

 protected:
  // Our state.
  virtual bool update_state();
};

}
