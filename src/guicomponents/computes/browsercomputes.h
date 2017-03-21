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
class OpenBrowserCompute;
class InputNodeCompute;
class MessageSender;
class DownloadManager;
class FileModel;

class GUICOMPUTES_EXPORT BrowserCompute: public Compute {
 public:
  COMPONENT_ID(Compute, InvalidComponent);
  BrowserCompute(Entity* entity, ComponentDID did);
  virtual ~BrowserCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  virtual void on_results(const std::deque<QJsonObject>& last_results);
  virtual void on_finished(const std::deque<QJsonObject>& last_results);

  static void init_hints(QJsonObject& m);
  virtual void update_wires();

  QJsonObject get_params() const;

 protected:
  // Our state.
  virtual void queue_start_update(TaskContext& tc);
  virtual void queue_on_results(TaskContext& tc);
  virtual void queue_on_finished(TaskContext& tc);
  virtual void queue_end_update(TaskContext& tc);

  void dump_map(const QJsonObject& inputs) const;

  Entity* find_group_context() const;
  void find_dep_nodes();

  Dep<TaskQueuer> _queuer;
  Dep<TaskScheduler> _scheduler;
  Dep<MessageSender> _msg_sender;
  Dep<InputNodeCompute> _browser_width;
  Dep<InputNodeCompute> _browser_height;
};

class GUICOMPUTES_EXPORT OpenBrowserCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, OpenBrowserCompute);
  OpenBrowserCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual bool update_state(); // made public
  void wait_for_chrome_connection_task();
};

class GUICOMPUTES_EXPORT CloseBrowserCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, CloseBrowserCompute);
  CloseBrowserCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual bool update_state(); // made public
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
  virtual void on_response(const QJsonObject& chain_state);

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT GetTitleCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, GetTitleCompute);
  GetTitleCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  //virtual void receive_chain_state(const QJsonObject& chain_state);
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT CloseTabCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, CloseTabCompute);
  CloseTabCompute(Entity* entity): BrowserCompute(entity, kDID()){}
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
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
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

class GUICOMPUTES_EXPORT DownloadVideosCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, DownloadVideosCompute);
  DownloadVideosCompute(Entity* entity);
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();

  Dep<DownloadManager> _download_manager;
};

class GUICOMPUTES_EXPORT DownloadImagesCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, DownloadImagesCompute);
  DownloadImagesCompute(Entity* entity);
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
  Dep<FileModel> _file_model;
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

class GUICOMPUTES_EXPORT GetURLCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, GetURLCompute);
  GetURLCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  //virtual void receive_chain_state(const QJsonObject& chain_state);
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT GetAllElementsCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, GetAllElementsCompute);
  GetAllElementsCompute(Entity* entity): BrowserCompute(entity, kDID()) {}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT HighlightElementsCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, HighlightElementsCompute);
  HighlightElementsCompute(Entity* entity): BrowserCompute(entity, kDID()){}
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
  virtual void queue_perform_hover(TaskContext& tc);
  virtual void queue_perform_action(TaskContext& tc);
  virtual void queue_end_update(TaskContext& tc);
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
  virtual void queue_end_update(TaskContext& tc);
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
 protected:
  virtual bool update_state();
  virtual void queue_end_update(TaskContext& tc);
};

class GUICOMPUTES_EXPORT ElementScrollCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ElementScrollCompute);
  ElementScrollCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
  virtual void queue_end_update(TaskContext& tc);
};

class GUICOMPUTES_EXPORT ElementScrollIntoViewCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ElementScrollIntoViewCompute);
  ElementScrollIntoViewCompute(Entity* entity): BrowserCompute(entity, kDID()){}
 protected:
  virtual bool update_state();
  virtual void queue_end_update(TaskContext& tc);
};

}
