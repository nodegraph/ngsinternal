#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>

#include <QtCore/QJsonValue>
#include <QtCore/QJsonObject>

namespace ngs {

class COMPUTES_EXPORT MergeNodeCompute: public Compute {
 public:
  COMPONENT_ID(Compute, MergeNodeCompute);
  MergeNodeCompute(Entity* entity);
  virtual ~MergeNodeCompute();

  // Our topology.
  virtual void create_inputs_outputs();

  // Our hints.
  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

 protected:
  // Our state.
  virtual bool update_state();

};

}
