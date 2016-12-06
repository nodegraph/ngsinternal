#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>


namespace ngs {

class COMPUTES_EXPORT AccumulateDataNodeCompute: public Compute {
 public:
  COMPONENT_ID(Compute, AccumulateDataNodeCompute);
  AccumulateDataNodeCompute(Entity* entity);
  virtual ~AccumulateDataNodeCompute();

  // Our topology.
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

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

  QJsonValue _override;
};

}
