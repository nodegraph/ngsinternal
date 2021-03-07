#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <components/computes/compute.h>
#include <base/objectmodel/dep.h>

#include <QtCore/QObject>

class QJsonValue;
class QQmlContext;

namespace ngs {

class ScriptLoopContext;

class GUICOMPUTES_EXPORT BaseScriptNodeCompute: public QObject, public Compute  {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, InvalidComponent);
  BaseScriptNodeCompute(Entity* entity, ComponentDID did = kDID());
  virtual ~BaseScriptNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

  // Script API.
  // Note that we need to return the json objects by value. Otherwise the js/qml side will get undefined as the value.
  Q_INVOKABLE QJsonObject get_context();
  Q_INVOKABLE void set_context(const QJsonObject& context);
  Q_INVOKABLE virtual QJsonObject get_main_input();
  Q_INVOKABLE virtual void set_main_output(const QJsonObject& value);
  Q_INVOKABLE virtual void post(int post_type, const QString& title, const QJsonValue& value);

protected:
  Dep<ScriptLoopContext> _context;
  Dep<ScriptLoopContext> get_closest_context();

  // Our state.
  virtual void update_wires();
  virtual bool update_state();

  virtual void expose_to_eval_context(QQmlContext& context);

  QString _script_body;
};

class GUICOMPUTES_EXPORT ScriptNodeCompute: public BaseScriptNodeCompute {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, ScriptNodeCompute);
  ScriptNodeCompute(Entity* entity);
  virtual ~ScriptNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

protected:
  // Our state.
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT ExtractPropertyNodeCompute: public BaseScriptNodeCompute {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, ExtractPropertyNodeCompute);
  ExtractPropertyNodeCompute(Entity* entity);
  virtual ~ExtractPropertyNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

protected:
  // Our state.
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT EmbedPropertyNodeCompute: public BaseScriptNodeCompute {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, EmbedPropertyNodeCompute);
  EmbedPropertyNodeCompute(Entity* entity);
  virtual ~EmbedPropertyNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

protected:
  // Our state.
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT ErasePropertyNodeCompute: public BaseScriptNodeCompute {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, ErasePropertyNodeCompute);
  ErasePropertyNodeCompute(Entity* entity);
  virtual ~ErasePropertyNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

protected:
  // Our state.
  virtual bool update_state();
};

}
