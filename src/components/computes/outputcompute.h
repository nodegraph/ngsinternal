#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>

namespace ngs {

class UpperHierarchyChange;
class Entity;

class COMPUTES_EXPORT OutputCompute: public Compute {
 public:

  COMPONENT_ID(Compute, OutputCompute);

  OutputCompute(Entity* entity);
  virtual ~OutputCompute();

  // We override to stop creating the inputs and outputs namespace.
  virtual void create_inputs_outputs() {}

  // Our state.
  virtual void update_state();

  // Our data type.
  void set_param_type(ParamType param_type);
  ParamType get_param_type() const;

  // Exposure.
  void set_exposed(bool exposed);
  bool is_exposed() const;

  // Our relative positioning.
  virtual size_t get_exposed_output_index() const;
  virtual size_t get_num_exposed_outputs() const;
  virtual size_t get_num_hidden_outputs() const;
  virtual size_t get_num_outputs() const;

  // Serialization.
  virtual void save(SimpleSaver& saver) const;
  virtual void load(SimpleLoader& loader);

 private:
  // Update our relative positioning against other OutputComputes.
  void update_index();

  // Our fixed deps.
  Dep<UpperHierarchyChange> _upper_change;
  Dep<Compute> _node_compute;

  // Our data type.
  ParamType _param_type;

  // Exposure.
  bool _exposed;

  // Gathered info about our relative positioning.
  size_t _exposed_index; // Our position within the exposed outputs.
  size_t _num_exposed; // Total number of exposed outputs.
  size_t _num_outputs; // Total number of outputs.

  friend class InputCompute;
  friend class OutputShape;
};


}
