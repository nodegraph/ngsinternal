#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>
#include <components/computes/paramtypes.h>

#include <QtQml/QJSValue>

namespace ngs {

class OutputCompute;

class COMPUTES_EXPORT InputCompute: public Compute  {
 public:
  COMPONENT_ID(Compute, InputCompute);
  InputCompute(Entity* entity);
  virtual ~InputCompute();

  // We override to stop creating the inputs and outputs namespace.
  virtual void create_inputs_outputs() {}

  // The unconnected value.
  void set_unconnected_value(const QJSValue& value);
  const QJSValue& get_unconnected_value() const;

  // Exposure.
  void set_exposed(bool exposed);
  bool is_exposed() const;

  // Our dynamic output compute dependency.
  bool can_link_output_compute(const Dep<OutputCompute>& output) const;
  bool link_output_compute(Dep<OutputCompute>& output);
  void unlink_output_compute();
  const Dep<OutputCompute>& get_output_compute() const;
  bool is_connected() const;

  // Serialization.
  virtual void save(SimpleSaver& saver) const;
  virtual void load(SimpleLoader& loader);

 protected:
  // Our state.
  virtual bool update_state();

 private:

  // Our dynamic deps.
  Dep<OutputCompute> _upstream;

  // Our data.
  // Usually stores the value directly.
  // If it has a string starting with "data." then it's
  // an expression which pulls a value from the data tree flowing through the graph.
  QJSValue _unconnected_value;

  // Whether we are exposed in the node graph as a plug on a node.
  bool _exposed;
};


}
