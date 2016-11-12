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

  // Clean the group interface.
  virtual bool clean_inputs();

  // Inputs.
  virtual QJsonObject get_editable_inputs() const;

  // Group locking.
  virtual bool group_is_unlocked() const {return false;}
  virtual void unlock_group(){} // Starts an synchronous task.
  virtual void unlock_group_and_dive(){} // Starts an synchronous task.
  virtual void lock_group(){} // Starts an synchronous task.
  virtual void lock_group_and_surface(){} // Starts an synchronous task.

 protected:

  // Our state.
  virtual void update_wires();
  virtual bool update_state();
  virtual void propagate_dirtiness(Component* dependency);
  virtual void set_self_dirty(bool dirty);

 private:
  Dep<BaseFactory> _factory;
};

}
