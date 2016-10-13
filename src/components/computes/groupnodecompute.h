#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>


namespace ngs {

class BaseFactory;
class BaseNodeGraphManipulator;
class InputNodeCompute;
class OutputNodeCompute;

class COMPUTES_EXPORT GroupNodeCompute: public Compute {
 public:
  COMPONENT_ID(Compute, GroupNodeCompute);
  GroupNodeCompute(Entity* entity, ComponentDID did = kDID());
  virtual ~GroupNodeCompute();

  virtual void create_inputs_outputs();

 protected:

  // Our state.
  virtual void update_wires();
  virtual bool update_state();
  virtual void set_self_dirty(bool dirty);
  virtual void check_self_dirty();
  virtual void finalize_state();

 private:
  void dirty_internal_input_node_computes();
  void dirty_all_internal_node_computes();
  void dirty_all_internal_node_computes2();
  //void gather_node_computes();
  //bool update_state2();

  Dep<BaseFactory> _factory;

//  // References to the computes of all of our internal nodes.
//  std::unordered_map<std::string, Dep<InputNodeCompute> > _input_node_computes;
//  std::unordered_map<std::string, Dep<OutputNodeCompute> > _output_node_computes;
//  std::unordered_map<std::string, Dep<Compute> > _other_node_computes;

  // Count the number of update_state() calls since the last time we were clean.
  int _num_passes;
};

}
