#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>


namespace ngs {

class COMPUTES_EXPORT DataNodeCompute: public Compute {
 public:
  COMPONENT_ID(Compute, DataNodeCompute);
  DataNodeCompute(Entity* entity);
  virtual ~DataNodeCompute();

  // Our topology.
  virtual void create_inputs_outputs();

  // Our hints.
  static QVariantMap init_hints();
  static const QVariantMap _hints;
  virtual const QVariantMap& get_hints() const {return _hints;}

  // Set and get the "json" parameter programmatically.
  virtual void set_override(const QVariant& override);
  virtual QVariant get_override() const;
  virtual void clear_override();

 protected:
  // Our state.
  virtual bool update_state();

  // This is a runtime override on this node's output value.
  // This is not serialized.
  // The "json" parameter holds the initial output value for this node.
  QVariant _override;
};

}
