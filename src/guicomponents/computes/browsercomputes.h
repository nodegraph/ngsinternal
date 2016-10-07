#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/comms/message.h>
#include <components/computes/compute.h>
#include <base/objectmodel/dep.h>

namespace ngs {

class Entity;
class InputCompute;
class AppWorker;
class AppTaskQueue;
class AppTaskContext;

class GUICOMPUTES_EXPORT BrowserCompute: public Compute {
 public:
  COMPONENT_ID(Compute, InvalidComponent);
  BrowserCompute(Entity* entity, ComponentDID did);
  virtual ~BrowserCompute();

  virtual void create_inputs_outputs();

  virtual void on_get_outputs(const QVariantMap& outputs);

  virtual bool update_is_asynchronous() const {external(); return true;}
  bool is_processing() const {external(); return _processing;}

 protected:
  // Our state.
  virtual void pre_update_state(AppTaskContext& tc);
  virtual void post_update_state(AppTaskContext& tc);

  void dump_map(const QVariantMap& inputs) const;

  Dep<AppWorker> _app_worker;
  Dep<AppTaskQueue> _task_queue;

  bool _processing;
};

class GUICOMPUTES_EXPORT OpenBrowserCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, OpenBrowserCompute);
  OpenBrowserCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT CloseBrowserCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, CloseBrowserCompute);
  CloseBrowserCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT CheckBrowserIsOpenCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, CheckBrowserIsOpenCompute);
  CheckBrowserIsOpenCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT CheckBrowserSizeCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, CheckBrowserSizeCompute);
  CheckBrowserSizeCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT NavigateToCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, NavigateToCompute);
  NavigateToCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT NavigateRefreshCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, NavigateRefreshCompute);
  NavigateRefreshCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT SwitchToIFrameCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, SwitchToIFrameCompute);
  SwitchToIFrameCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT CreateSetFromValuesCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, CreateSetFromValuesCompute);
  CreateSetFromValuesCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT CreateSetFromTypeCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, CreateSetFromTypeCompute);
  CreateSetFromTypeCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT DeleteSetCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, DeleteSetCompute);
  DeleteSetCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT ShiftSetCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ShiftSetCompute);
  ShiftSetCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT MouseActionCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, MouseActionCompute);
  MouseActionCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT StartMouseHoverActionCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, StartMouseHoverActionCompute);
  StartMouseHoverActionCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT StopMouseHoverActionCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, StopMouseHoverActionCompute);
  StopMouseHoverActionCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT TextActionCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, TextActionCompute);
  TextActionCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT ElementActionCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ElementActionCompute);
  ElementActionCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT ExpandSetCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ExpandSetCompute);
  ExpandSetCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT MarkSetCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, MarkSetCompute);
  MarkSetCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT UnmarkSetCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, UnmarkSetCompute);
  UnmarkSetCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT MergeSetsCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, MergeSetsCompute);
  MergeSetsCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT ShrinkSetToSideCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ShrinkSetToSideCompute);
  ShrinkSetToSideCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();
 protected:
  virtual void update_state();
};

class GUICOMPUTES_EXPORT ShrinkAgainstMarkedCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ShrinkAgainstMarkedCompute);
  ShrinkAgainstMarkedCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();
 protected:
  virtual void update_state();
};

}
