#pragma once
#include <base/objectmodel/component.h>
#include <components/computes/computes_export.h>
#include <components/computes/baseoutputs.h>
#include <entities/componentids.h>

#include <string>

namespace ngs {

//static void gather_plugs(Entity* self, bool gather_inputs,
//                   std::unordered_map<std::string, Entity*>& hidden,
//                   std::unordered_map<std::string, Entity*>& exposed,
//                   std::unordered_map<std::string, size_t>& exposed_ordering);

// Note this is class is very similar to Inputs and should
// be refactored later into something able to handle both.

class COMPUTES_EXPORT Outputs: public BaseOutputs {
 public:

  COMPONENT_ID(BaseOutputs, Outputs);
  Outputs(Entity* entity);
  virtual ~Outputs();

  // Our relative positioning.
  virtual size_t get_exposed_index(const std::string& output_name) const;
  virtual size_t get_num_exposed() const;
  virtual size_t get_num_hidden() const;
  virtual size_t get_total() const;

  virtual const std::unordered_map<std::string, Entity*>& get_hidden() const {external(); return _hidden;}
  virtual const std::unordered_map<std::string, Entity*>& get_exposed() const {external(); return _exposed;}

 protected:
  // Our state.
  virtual void update_wires();

  virtual void gather();

  bool _for_inputs;

  // Alphabetical ordering of exposed inputs.
  std::unordered_map<std::string, size_t> _exposed_ordering;
  std::unordered_map<std::string, Entity*> _hidden;
  std::unordered_map<std::string, Entity*> _exposed;
};

}
