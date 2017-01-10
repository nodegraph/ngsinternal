#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <components/computes/compute.h>
#include <base/objectmodel/dep.h>

#include <QtCore/QObject>

class QJsonValue;

namespace ngs {

class GUICOMPUTES_EXPORT ScriptNodeCompute: public QObject, public Compute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, ScriptNodeCompute);
  ScriptNodeCompute(Entity* entity);
  virtual ~ScriptNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

  Q_INVOKABLE QJsonValue get_input();
  Q_INVOKABLE void set_output_value(const QJsonValue& value);

protected:
  // Our state.
  virtual bool update_state();
};

}
