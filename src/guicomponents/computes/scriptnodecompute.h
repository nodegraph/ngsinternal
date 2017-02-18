#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <components/computes/compute.h>
#include <base/objectmodel/dep.h>

#include <QtCore/QObject>

class QJsonValue;

namespace ngs {

class ScriptLoopContext;

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

  // Script API.
  // Note that we need to return the json objects by value. Otherwise the js/qml side will get undefined as the value.
  Q_INVOKABLE QJsonObject get_context();
  Q_INVOKABLE void set_context(const QJsonObject& context);
  Q_INVOKABLE QJsonObject get_input();
  Q_INVOKABLE void set_output(const QJsonObject& value);

protected:
  Dep<ScriptLoopContext> _context;

  Dep<ScriptLoopContext> get_closest_context();

  // Our state.
  virtual void update_wires();
  virtual bool update_state();
};

}
