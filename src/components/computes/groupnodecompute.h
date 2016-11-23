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

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  // Clean the group interface.
  virtual bool clean_inputs();

  // Manage hints for the inputs on the group.
  virtual const QJsonObject& get_hints() const {return _node_hints;}
  virtual void add_param_hints(const std::string& name, const QJsonValue& param_hints);
  virtual void remove_param_hints(const std::string& name);

 protected:

  // Our state.
  virtual void update_wires();
  virtual bool update_state();
  virtual void propagate_dirtiness(Component* dependency);
  virtual void set_self_dirty(bool dirty);

 private:
  Dep<BaseFactory> _factory;
  QJsonObject _node_hints;
};

}
