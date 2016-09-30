#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/comms/message.h>
#include <components/computes/compute.h>
#include <base/objectmodel/dep.h>

namespace ngs {

class Entity;
class InputCompute;
class AppWorker;

class GUICOMPUTES_EXPORT BrowserCompute: public Compute {
 public:
  COMPONENT_ID(Compute, InvalidComponent);
  BrowserCompute(Entity* entity, size_t did);
  virtual ~BrowserCompute();

  virtual void create_inputs_outputs();

  virtual void on_finished_sequence(const QVariantMap& outputs);

  QVariantMap get_map(const std::string& input_name) const;
  QString get_string(const std::string& input_name) const;
  QStringList get_string_list(const std::string& input_name) const;

  int get_int(const std::string& input_name) const;
  float get_float(const std::string& input_name) const;

  WrapType get_wrap_type(const std::string& input_name) const;
  ActionType get_action_type(const std::string& input_name) const;
  Direction get_direction(const std::string& input_name) const;

 protected:
  // Our state.
  virtual void update_state();

  Dep<AppWorker> _app_worker;
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

class GUICOMPUTES_EXPORT ClickActionCompute: public BrowserCompute {
 public:
  COMPONENT_ID(Compute, ClickActionCompute);
  ClickActionCompute(Entity* entity): BrowserCompute(entity, kDID()){}
  virtual void create_inputs_outputs();
 protected:
  virtual void update_state();
};

}
