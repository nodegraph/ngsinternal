#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>

#include <unordered_set>

namespace ngs {

class BaseFactory;
class BaseNodeGraphManipulator;

class COMPUTES_EXPORT GroupNodeCompute: public Compute {
 public:
  COMPONENT_ID(Compute, GroupNodeCompute);

  class WireUpdater: public Component {
   public:
    WireUpdater(GroupNodeCompute* target);
    void update_wires();
    GroupNodeCompute* _target;
  };


  GroupNodeCompute(Entity* entity, ComponentDID did = kDID());
  virtual ~GroupNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());
  virtual const std::unordered_set<std::string>& get_fixed_inputs() const;

  // Clean the group interface.
  virtual bool clean_inputs();

  // Manage hints for the inputs on the group.
  virtual const QJsonObject& get_hints() const {return _node_hints;}
  virtual void add_param_hints(const std::string& name, const QJsonValue& param_hints);
  virtual void remove_param_hints(const std::string& name);

  // Internal nodes.
  virtual void dirty_all_nodes_in_group();
  virtual void dirty_all_nodes_in_group_recursively();



 protected:

  // Our state.
  virtual void update_wires();
  virtual bool update_state();
  virtual void propagate_dirtiness(Component* dependency);
  virtual void set_self_dirty(bool dirty);

  // Copying values between inside and outside the group.
  void copy_inputs_to_input_nodes();
  void copy_output_nodes_to_outputs();
  void reset_accumulate_data_nodes();


  Dep<BaseFactory> _factory;
  QJsonObject _node_hints;
  WireUpdater* _wire_updater;
};

}
