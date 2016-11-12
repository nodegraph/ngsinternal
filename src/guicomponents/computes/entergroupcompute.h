#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <components/computes/compute.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>

namespace ngs {

class Inputs;

class GUICOMPUTES_EXPORT EnterGroupCompute: public Compute  {
 public:
  COMPONENT_ID(Compute, EnterGroupCompute);
  EnterGroupCompute(Entity* entity, ComponentDID did = kDID());
  virtual ~EnterGroupCompute();

  virtual bool get_lock_setting() const {return false;}
  virtual void set_lock_setting(bool lock) {}

 protected:

  // Note: Since we're grabbing all the inputs on the group,
  // if anything changes on the group we'll become dirty.
  // However most of our derive group lock components will cache whether
  // they are unlocked because it may take time to quere over the socket
  // whether they are properly signed in to something like firebase.
  // Because of this cache, in order to clean itself again it basically
  // does nothing.
  Dep<Inputs> _inputs;
};

class GUICOMPUTES_EXPORT ExitGroupCompute: public Compute  {
 public:
  COMPONENT_ID(Compute, ExitGroupCompute);
  ExitGroupCompute(Entity* entity, ComponentDID did = kDID());
  virtual ~ExitGroupCompute();
};

}
