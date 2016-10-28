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


 protected:

  // Our state.
  virtual void update_wires();
  virtual bool update_state();
  virtual void propagate_dirtiness(Component* dependency);
  virtual void set_self_dirty(bool dirty);

  // These are the names of any inputs (params) directly on the group node,
  // instead of the inputs/outputs created from input/output nodes inside the group.
  std::unordered_set<std::string> _on_group_inputs;

 private:
  Dep<BaseFactory> _factory;
};

}
