#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/comms/message.h>
#include <components/computes/compute.h>
#include <base/objectmodel/dep.h>

namespace ngs {

class Entity;
class TaskQueuer;
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
  virtual void handle_response(TaskContext& tc);
  virtual void post_update_state(TaskContext& tc);

  void dump_map(const QJsonObject& inputs) const;

  Entity* find_group_context() const;
  Dep<EnterBrowserGroupCompute> find_enter_node();

  Dep<TaskQueuer> _worker;
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

class GUICOMPUTES_EXPORT ReleaseBrowserCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ReleaseBrowserCompute);
  ReleaseBrowserCompute(Entity* entity): BrowserCompute(entity, kDID()){}
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
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT GetBrowserSizeCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, GetBrowserSizeCompute);
  GetBrowserSizeCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());
  virtual void receive_chain_state(const QJsonObject& chain_state);

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT GetActiveTabTitleCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, GetActiveTabTitleCompute);
  GetActiveTabTitleCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  //virtual void receive_chain_state(const QJsonObject& chain_state);
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT DestroyCurrentTabCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, DestroyCurrentTabCompute);
  DestroyCurrentTabCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT OpenTabCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, OpenTabCompute);
  OpenTabCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT AcceptSaveDialogCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, AcceptSaveDialogCompute);
  AcceptSaveDialogCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT DownloadVideoCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, DownloadVideoCompute);
  DownloadVideoCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
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

class GUICOMPUTES_EXPORT GetCurrentURLCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, GetCurrentURLCompute);
  GetCurrentURLCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  //virtual void receive_chain_state(const QJsonObject& chain_state);
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT FindElementByPositionCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, FindElementByPositionCompute);
  FindElementByPositionCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
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

class GUICOMPUTES_EXPORT ShiftElementByTypeAlongRowsCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ShiftElementByTypeAlongRowsCompute);
  ShiftElementByTypeAlongRowsCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT ShiftElementByValuesAlongRowsCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ShiftElementByValuesAlongRowsCompute);
  ShiftElementByValuesAlongRowsCompute(Entity* entity): BrowserCompute(entity, kDID()){}
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
  virtual void post_update_state(TaskContext& tc);
};

class GUICOMPUTES_EXPORT TextActionCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, TextActionCompute);
  TextActionCompute(Entity* entity, ComponentDID did = kDID()): BrowserCompute(entity, did){}
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
  virtual void post_update_state(TaskContext& tc);
};

// This class is exactly like TextActionCompute except that
// it adds a hint to the text param indicate that it's a password
// and that it should be hidden.
class GUICOMPUTES_EXPORT PasswordActionCompute: public TextActionCompute {
 public:
  COMPONENT_ID(Compute, PasswordActionCompute);
  PasswordActionCompute(Entity* entity): TextActionCompute(entity, PasswordActionCompute::kDID()){}
  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
};

class GUICOMPUTES_EXPORT ElementActionCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ElementActionCompute);
  ElementActionCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

  //virtual void receive_chain_state(const QJsonObject& chain_state);

 protected:
  virtual bool update_state();
  virtual void post_update_state(TaskContext& tc);
};

}
