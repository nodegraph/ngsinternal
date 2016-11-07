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

class GUICOMPUTES_EXPORT BaseHTTPCompute: public Compute {
 public:
  COMPONENT_ID(Compute, InvalidComponent);
  BaseHTTPCompute(Entity* entity, ComponentDID did);
  virtual ~BaseHTTPCompute();

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

class GUICOMPUTES_EXPORT HTTPCompute: public BaseHTTPCompute {
 public:
  COMPONENT_ID(Compute, HTTPCompute);
  HTTPCompute(Entity* entity): BaseHTTPCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  // Hints.
  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

  // Extract values from the chain_state coming from the HTTP worker.
  virtual void on_get_outputs(const QJsonObject& chain_state);
 protected:
  virtual bool update_state();
};

}
