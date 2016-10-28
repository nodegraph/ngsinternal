#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/comms/message.h>
#include <components/computes/compute.h>
#include <base/objectmodel/dep.h>

namespace ngs {

class Entity;
class WebWorker;
class TaskScheduler;
class TaskContext;

class GUICOMPUTES_EXPORT BrowserCompute: public Compute {
 public:
  COMPONENT_ID(Compute, InvalidComponent);
  BrowserCompute(Entity* entity, ComponentDID did);
  virtual ~BrowserCompute();

  virtual void create_inputs_outputs();

  virtual void on_get_outputs(const QJsonObject& chain_state);

  static void init_hints(QJsonObject& m);

 protected:
  // Our state.
  virtual void pre_update_state(TaskContext& tc);
  virtual void post_update_state(TaskContext& tc);

  void dump_map(const QJsonObject& inputs) const;

  Dep<WebWorker> _web_worker;
  Dep<TaskScheduler> _task_scheduler;
};

class GUICOMPUTES_EXPORT OpenBrowserCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, OpenBrowserCompute);
  OpenBrowserCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT CloseBrowserCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, CloseBrowserCompute);
  CloseBrowserCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT IsBrowserOpenCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, IsBrowserOpenCompute);
  IsBrowserOpenCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT ResizeBrowserCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ResizeBrowserCompute);
  ResizeBrowserCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT NavigateToCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, NavigateToCompute);
  NavigateToCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();

};

class GUICOMPUTES_EXPORT NavigateRefreshCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, NavigateRefreshCompute);
  NavigateRefreshCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT SwitchToIFrameCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, SwitchToIFrameCompute);
  SwitchToIFrameCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT CreateSetFromValuesCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, CreateSetFromValuesCompute);
  CreateSetFromValuesCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT CreateSetFromTypeCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, CreateSetFromTypeCompute);
  CreateSetFromTypeCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT DeleteSetCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, DeleteSetCompute);
  DeleteSetCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT ShiftSetCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ShiftSetCompute);
  ShiftSetCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT MouseActionCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, MouseActionCompute);
  MouseActionCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT StartMouseHoverActionCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, StartMouseHoverActionCompute);
  StartMouseHoverActionCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT StopMouseHoverActionCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, StopMouseHoverActionCompute);
  StopMouseHoverActionCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT TextActionCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, TextActionCompute);
  TextActionCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT ElementActionCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ElementActionCompute);
  ElementActionCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

  virtual void on_get_outputs(const QJsonObject& chain_state);

 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT ExpandSetCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ExpandSetCompute);
  ExpandSetCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT MarkSetCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, MarkSetCompute);
  MarkSetCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT UnmarkSetCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, UnmarkSetCompute);
  UnmarkSetCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT MergeSetsCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, MergeSetsCompute);
  MergeSetsCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT ShrinkSetToSideCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ShrinkSetToSideCompute);
  ShrinkSetToSideCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT ShrinkAgainstMarkedCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ShrinkAgainstMarkedCompute);
  ShrinkAgainstMarkedCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

}
