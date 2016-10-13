#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>


namespace ngs {

class BaseFactory;
class BaseNodeGraphManipulator;

class COMPUTES_EXPORT GroupNodeCompute: public Compute {
 public:
  COMPONENT_ID(Compute, GroupNodeCompute);
  GroupNodeCompute(Entity* entity, ComponentDID did = kDID());
  virtual ~GroupNodeCompute();

  virtual void create_inputs_outputs();

  // Dirty nodes inside web groups. This is called when the ultimate target
  // we want to clean is outside a web group. If we are inside a web group we
  // assume we are interactively building a web recording and we only want the
  // latest added node to execute.
  // This should be called from the app root.
  virtual void dirty_web_groups();
  virtual void dirty_web_computes();

  virtual void dirty_all_node_computes();
  virtual void dirty_input_node_computes();

 protected:

  // Our state.
  virtual void update_wires();
  virtual bool update_state();
  virtual void set_self_dirty(bool dirty);

 private:
  Dep<BaseFactory> _factory;
};

}
