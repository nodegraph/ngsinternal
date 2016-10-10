#pragma once
#include <base/objectmodel/component.h>
#include <components/computes/computes_export.h>
#include <entities/componentids.h>
#include <entities/entityids.h>

#include <string>

namespace ngs {

class InputCompute;
class OutputCompute;
class BaseNodeGraphManipulator;

struct InputTraits {
  typedef InputCompute IOCompute;
  static const EntityDID IOComputeEntityDID = EntityDID::kInputEntity;
  static const ComponentIID FluxIID = ComponentIID::kIInputs;
  static const ComponentDID FluxDID = ComponentDID::kInputs;
  static const char* folder_name;
};

struct OutputTraits {
  typedef OutputCompute IOCompute;
  static const EntityDID IOComputeEntityDID = EntityDID::kOutputEntity;
  static const ComponentIID FluxIID = ComponentIID::kIOutputs;
  static const ComponentDID FluxDID = ComponentDID::kOutputs;
  static const char* folder_name;
};

template <class Traits>
class COMPUTES_EXPORT Flux: public Component {
 public:
  static ComponentIID kIID() {
      return Traits::FluxIID;
  }
  static ComponentDID kDID() {
    return Traits::FluxDID;
  }

  Flux(Entity* entity);
  virtual ~Flux();

  // Our relative positioning.
  virtual size_t get_exposed_index(const std::string& name) const;
  virtual size_t get_num_exposed() const;
  virtual size_t get_num_hidden() const;
  virtual size_t get_total() const;

  virtual const std::unordered_map<std::string, Dep<typename Traits::IOCompute> >& get_hidden() const;
  virtual const std::unordered_map<std::string, Dep<typename Traits::IOCompute> >& get_exposed() const;
  virtual const std::unordered_map<std::string, Dep<typename Traits::IOCompute> >& get_all() const;

 protected:
  // Our state.
  virtual void update_wires();

 private:
  virtual bool wires_are_up_to_date();
  virtual void gather();

  Dep<BaseNodeGraphManipulator> _ng_manipulator;

  // Alphabetical ordering of exposed inputs/outputs.
  std::unordered_map<std::string, size_t> _exposed_ordering;
  std::unordered_map<std::string, Dep<typename Traits::IOCompute> > _hidden;
  std::unordered_map<std::string, Dep<typename Traits::IOCompute> > _exposed;
  std::unordered_map<std::string, Dep<typename Traits::IOCompute> > _all;
};

typedef Flux<InputTraits> Inputs;
typedef Flux<OutputTraits> Outputs;

}
