#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/comms/message.h>
#include <components/computes/compute.h>
#include <base/objectmodel/dep.h>

namespace ngs {

class Entity;
class TaskScheduler;
class TaskContext;
class MQTTHostCompute;

class GUICOMPUTES_EXPORT BaseMQTTCompute: public Compute {
 public:
  COMPONENT_ID(Compute, InvalidComponent);
  BaseMQTTCompute(Entity* entity, ComponentDID did);
  virtual ~BaseMQTTCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());
  virtual void on_finished_task();

  static void init_hints(QJsonObject& m);
  virtual void update_wires();

 protected:
  virtual void append_callback_tasks(TaskContext& tc);


  Entity* find_group_context() const;
  Dep<MQTTHostCompute> find_host_node();

  Dep<TaskScheduler> _scheduler;
  Dep<MQTTHostCompute> _enter; // This is not serialized and is gathered at runtime.
};

class GUICOMPUTES_EXPORT MQTTPublishCompute: public BaseMQTTCompute {
 public:
  COMPONENT_ID(Compute, MQTTPublishCompute);
  MQTTPublishCompute(Entity* entity): BaseMQTTCompute(entity, kDID()){}
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

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
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());
  virtual void on_finished_task();

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
  virtual bool destroy_state();
  QJsonValue _override;

  // Cached value of our topic. Use especially for destruction.
  QString _topic;
};

}
