#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/comms/message.h>
#include <components/computes/compute.h>
#include <base/objectmodel/dep.h>

namespace ngs {

class Entity;
class HTTPWorker;
class TaskScheduler;
class TaskContext;

class GUICOMPUTES_EXPORT HTTPCompute: public Compute {
 public:
  COMPONENT_ID(Compute, InvalidComponent);
  HTTPCompute(Entity* entity, ComponentDID did);
  virtual ~HTTPCompute();

  virtual void create_inputs_outputs();

  virtual void on_get_outputs(const QJsonObject& chain_state);

  static void init_hints(QJsonObject& m);

 protected:
  // Our state.
  virtual void pre_update_state(TaskContext& tc);
  virtual void post_update_state(TaskContext& tc);

  void dump_map(const QJsonObject& inputs) const;

  Dep<HTTPWorker> _worker;
  Dep<TaskScheduler> _scheduler;
};

class GUICOMPUTES_EXPORT HTTPSendCompute: public HTTPCompute {
 public:
  COMPONENT_ID(Compute, HTTPSendCompute);
  HTTPSendCompute(Entity* entity): HTTPCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  // Hints.
  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT HTTPGetCompute: public HTTPCompute {
 public:
  COMPONENT_ID(Compute, HTTPGetCompute);
  HTTPGetCompute(Entity* entity): HTTPCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  // Hints.
  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

  // Extract values.
  virtual void on_get_outputs(const QJsonObject& chain_state);
 protected:
  virtual bool update_state();
};

}
