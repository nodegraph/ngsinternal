#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>

namespace ngs {

class COMPUTES_EXPORT LoopDataNodeCompute: public Compute {
 public:
  COMPONENT_ID(Compute, LoopDataNodeCompute);
  LoopDataNodeCompute(Entity* entity);
  virtual ~LoopDataNodeCompute();

  // Our topology.
  virtual void create_inputs_outputs(const EntityConfig& config);

  // Our hints.
  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

  // Our overrides.
  virtual void set_override(const QJsonValue& override);
  virtual const QJsonValue& get_override() const;
  virtual void clear_override();

 protected:
  // Our state.
  virtual bool update_state();

  // This is a runtime override on this node's output value.
  // This is not serialized.
  QJsonValue _override;
};

}
