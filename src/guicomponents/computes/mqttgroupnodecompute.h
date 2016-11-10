#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <components/computes/groupnodecompute.h>
#include <entities/componentids.h>
#include <guicomponents/comms/message.h>

// QT
#include <QtCore/QObject>
#include <QtCore/QVariant>

namespace ngs {

class BaseGroupTraits;

class GUICOMPUTES_EXPORT MQTTGroupNodeCompute: public GroupNodeCompute {
 public:
  COMPONENT_ID(Compute, MQTTGroupNodeCompute);
  MQTTGroupNodeCompute(Entity* entity);
  virtual ~MQTTGroupNodeCompute();

  virtual void set_self_dirty(bool dirty);

  // Inputs and Outputs.
  virtual void create_inputs_outputs();

  // Hints.
  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

  virtual bool update_unlocked_group();

 protected:
  // Our state.
  virtual bool update_state();

 private:
  Dep<BaseGroupTraits> _group_traits;
  bool _unlocked;
};

}
