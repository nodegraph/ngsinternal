#pragma once
#include <base/objectmodel/component.h>
#include <components/computes/computes_export.h>
#include <components/computes/baseinputs.h>
#include <entities/componentids.h>

#include <string>

namespace ngs {

class COMPUTES_EXPORT Inputs: public BaseInputs {
 public:

  COMPONENT_ID(BaseInputs, Inputs);
  Inputs(Entity* entity);
  virtual ~Inputs();

  // Info about our inputs. Note the output info can be found on OutputCompute.
  virtual size_t get_exposed_index(const std::string& input_name) const;
  virtual size_t get_num_exposed() const;
  virtual size_t get_num_hidden() const;
  virtual size_t get_total() const;

  virtual const std::unordered_map<std::string, Entity*>& get_hidden() const {external(); return _hidden;}
  virtual const std::unordered_map<std::string, Entity*>& get_exposed() const {external(); return _exposed;}

 protected:
  // Our state.
  virtual void update_wires();

 protected:
  void gather_inputs();

  // Alphabetical ordering of exposed inputs.
  std::unordered_map<std::string, size_t> _exposed_ordering;
  std::unordered_map<std::string, Entity*> _hidden;
  std::unordered_map<std::string, Entity*> _exposed;

};

}
