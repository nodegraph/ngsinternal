#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>


namespace ngs {

class Entity;
class InputCompute;

class COMPUTES_EXPORT DotNodeCompute: public Compute {
 public:
  COMPONENT_ID(Compute, DotNodeCompute);
  DotNodeCompute(Entity* entity);
  virtual ~DotNodeCompute();

  // Input and Outputs.
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  // Our hints.
  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

protected:
  // Our state.
  virtual bool update_state();
};


}
