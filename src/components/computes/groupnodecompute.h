#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>


namespace ngs {

class BaseFactory;
class InputShape;
class OutputShape;
class LinkShape;

class COMPUTES_EXPORT GroupNodeCompute: public Compute {
 public:

  COMPONENT_ID(Compute, GroupNodeCompute);

  GroupNodeCompute(Entity* entity);
  virtual ~GroupNodeCompute();

  // Our state.
  virtual void update_state();
  virtual HierarchyUpdate update_hierarchy();

//  // Info about our inputs. Note the output info can be found on OutputCompute.
//  virtual size_t get_exposed_input_index(const std::string& input_name) const;
//  virtual size_t get_num_exposed_inputs() const;
//  virtual size_t get_num_hidden_inputs() const;
//  virtual size_t get_num_inputs() const;

 private:
  Dep<BaseFactory> _factory;

};

}
