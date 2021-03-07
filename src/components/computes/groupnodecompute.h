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
    void revert_params_to_defaults();
    GroupNodeCompute* _target;
  };


  GroupNodeCompute(Entity* entity, ComponentDID did = kDID());
  virtual ~GroupNodeCompute();

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());
  virtual const std::unordered_set<std::string>& get_fixed_inputs() const;

  // Clean the group interface.
  virtual bool clean_inputs();
  virtual bool dirty_input_nodes();

  // Manage hints for the inputs on the group.
  virtual const QJsonObject& get_hints() const {return _node_hints;}
  virtual void add_param_hints(const std::string& name, const QJsonValue& param_hints);
  virtual void remove_param_hints(const std::string& name);
  virtual void revert_params_to_defaults();

  // Internal nodes.
  virtual void dirty_all_nodes_in_group();
  virtual void dirty_all_nodes_in_group_recursively();
  virtual void reset_dirty_state_on_all_nodes_in_group();
  virtual void reset_dirty_state_on_output_nodes();

 protected:

  // Our state.
  virtual void update_wires();
  virtual bool update_state();
  virtual void init_dirty_state();

  // Copying values between inside and outside the group.
  void copy_inputs_to_input_nodes();
  void copy_output_nodes_to_outputs();


  Dep<BaseFactory> _factory;
  QJsonObject _node_hints;
  WireUpdater* _wire_updater;
};

}
