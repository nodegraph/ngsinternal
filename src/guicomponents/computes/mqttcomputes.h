#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/comms/message.h>
#include <components/computes/compute.h>
#include <base/objectmodel/dep.h>

namespace ngs {

class Entity;
class MQTTWorker;
class TaskScheduler;
class TaskContext;

class GUICOMPUTES_EXPORT BaseMQTTCompute: public Compute {
 public:
  COMPONENT_ID(Compute, InvalidComponent);
  BaseMQTTCompute(Entity* entity, ComponentDID did);
  virtual ~BaseMQTTCompute();

  virtual void create_inputs_outputs();
  virtual void on_finished_compute();

  static void init_hints(QJsonObject& m);

 protected:
  virtual void post_update_state(TaskContext& tc);

  Dep<MQTTWorker> _worker;
  Dep<TaskScheduler> _scheduler;
};

class GUICOMPUTES_EXPORT MQTTPublishCompute: public BaseMQTTCompute {
 public:
  COMPONENT_ID(Compute, MQTTPublishCompute);
  MQTTPublishCompute(Entity* entity): BaseMQTTCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  // Hints.
  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT MQTTSubscribeCompute: public BaseMQTTCompute {
 public:
  COMPONENT_ID(Compute, MQTTSubscribeCompute);
  MQTTSubscribeCompute(Entity* entity): BaseMQTTCompute(entity, kDID()){}
  virtual void create_inputs_outputs();
  virtual void on_finished_compute();

  // Hints.
  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

  // Our overrides.
  virtual void set_override(const QString& topic, const QString& payload);
  virtual const QJsonValue& get_override() const;
  virtual void clear_override();

 protected:
  virtual bool update_state();
  QJsonValue _override;
};

}
