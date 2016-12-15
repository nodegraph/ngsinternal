#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/comms/message.h>
#include <components/computes/compute.h>
#include <base/objectmodel/dep.h>

namespace ngs {

class Entity;
class NodeJSWorker;
class TaskScheduler;
class TaskContext;
class EnterBrowserGroupCompute;

class GUICOMPUTES_EXPORT BrowserCompute: public Compute {
 public:
  COMPONENT_ID(Compute, InvalidComponent);
  BrowserCompute(Entity* entity, ComponentDID did);
  virtual ~BrowserCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  virtual void receive_chain_state(const QJsonObject& chain_state);

  static void init_hints(QJsonObject& m);
  virtual void update_wires();

 protected:
  // Our state.
  virtual void pre_update_state(TaskContext& tc);
  virtual void post_update_state(TaskContext& tc);

  void dump_map(const QJsonObject& inputs) const;

  Entity* find_group_context() const;
  Dep<EnterBrowserGroupCompute> find_enter_node();

  Dep<NodeJSWorker> _worker;
  Dep<TaskScheduler> _scheduler;
  Dep<EnterBrowserGroupCompute> _enter;
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
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();

};

class GUICOMPUTES_EXPORT NavigateBackCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, NavigateBackCompute);
  NavigateBackCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT NavigateForwardCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, NavigateForwardCompute);
  NavigateForwardCompute(Entity* entity): BrowserCompute(entity, kDID()){}
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

class GUICOMPUTES_EXPORT FindElementByValuesCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, FindElementByValuesCompute);
  FindElementByValuesCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT FindElementByTypeCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, FindElementByTypeCompute);
  FindElementByTypeCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT ShiftElementByTypeCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ShiftElementByTypeCompute);
  ShiftElementByTypeCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT ShiftElementByValuesCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ShiftElementByValuesCompute);
  ShiftElementByValuesCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

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
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

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
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

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
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

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
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

  virtual void receive_chain_state(const QJsonObject& chain_state);

 protected:
  virtual bool update_state();
};

}
