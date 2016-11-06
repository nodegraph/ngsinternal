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

  Dep<HTTPWorker> _http_worker;
  Dep<TaskScheduler> _task_scheduler;
};

//class GUICOMPUTES_EXPORT HTTPSendCompute: public HTTPCompute {
// public:
//  COMPONENT_ID(Compute, OpenBrowserCompute);
//  OpenBrowserCompute(Entity* entity): BrowserCompute(entity, kDID()){}
// protected:
//  virtual bool update_state();
//};
//
//class GUICOMPUTES_EXPORT HTTPReceiveCompute: public HTTPCompute {
// public:
//  COMPONENT_ID(Compute, OpenBrowserCompute);
//  OpenBrowserCompute(Entity* entity): BrowserCompute(entity, kDID()){}
// protected:
//  virtual bool update_state();
//};

}
