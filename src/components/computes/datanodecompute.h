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
  static QJSValue init_hints();
  static const QJSValue _hints;
  virtual const QJSValue& get_hints() const {return _hints;}

  // Our overrides.
  virtual void set_override(const QJSValue& override);
  virtual const QJSValue& get_override() const;
  virtual void clear_override();

 protected:
  // Our state.
  virtual bool update_state();


  // This is a runtime override on this node's output value.
  // This is not serialized.
  // The "default_value" parameter holds the initial/default output value for this node.
  QJSValue _override;
  bool _use_override;
};

}
