#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>

#include <QtCore/QJsonValue>
#include <QtCore/QJsonObject>

namespace ngs {

class COMPUTES_EXPORT DataNodeCompute: public Compute {
 public:
  COMPONENT_ID(Compute, DataNodeCompute);
  DataNodeCompute(Entity* entity);
  virtual ~DataNodeCompute();

  // Our topology.
  virtual void create_inputs_outputs();

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
  // The "value" parameter holds the initial/default output value for this node.
  QJsonValue _override;
  bool _use_override;
};

}
