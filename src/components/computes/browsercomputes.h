#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>


namespace ngs {

class Entity;
class InputCompute;

//--------------------------------------------------------------------------------

class COMPUTES_EXPORT OpenBrowserCompute: public Compute {
 public:
  COMPONENT_ID(Compute, OpenBrowserCompute);
  OpenBrowserCompute(Entity* entity);
  virtual ~OpenBrowserCompute();
  // Our state.
  virtual void update_state();
};

//--------------------------------------------------------------------------------

class COMPUTES_EXPORT CloseBrowserCompute: public Compute {
 public:
  COMPONENT_ID(Compute, CloseBrowserCompute);
  CloseBrowserCompute(Entity* entity);
  virtual ~CloseBrowserCompute();
  // Our state.
  virtual void update_state();
};

//--------------------------------------------------------------------------------

class COMPUTES_EXPORT CreateSetFromValuesCompute: public Compute {
 public:
  COMPONENT_ID(Compute, CreateSetFromValuesCompute);
  CreateSetFromValuesCompute(Entity* entity);
  virtual ~CreateSetFromValuesCompute();
  // Our state.
  virtual void update_state();
};

class COMPUTES_EXPORT CreateSetFromTypeCompute: public Compute {
 public:
  COMPONENT_ID(Compute, CreateSetFromTypeCompute);
  CreateSetFromTypeCompute(Entity* entity);
  virtual ~CreateSetFromTypeCompute();
  // Our state.
  virtual void update_state();
};

}
