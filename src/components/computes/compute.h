#pragma once
#include <base/objectmodel/component.h>
#include <components/computes/computes_export.h>
#include <components/computes/paramtypes.h>

#include <entities/componentids.h>
#include <base/objectmodel/dep.h>
#include <string>

// QT
#include <QtCore/QVariant>

namespace ngs {

#ifndef  ong_computes_EXPORTS
#define TEMPLATE_EXPORT extern
#else
#define TEMPLATE_EXPORT 
#endif

class BaseFactory;
class InputCompute;
class OutputCompute;

class COMPUTES_EXPORT Compute: public Component {
 public:

  static const QVariant _empty_variant;
  static bool check_variant_is_bool_and_true(const QVariant& value, const std::string& message);
  static bool check_variant_is_list(const QVariant& value, const std::string& message);
  static bool check_variant_is_map(const QVariant& value, const std::string& message);

  COMPONENT_ID(Compute, InvalidComponent);
  Compute(Entity* entity, ComponentDID derived_id);
  virtual ~Compute();

  virtual void create_inputs_outputs();

  // Info about our inputs. Note the output info can be found on OutputCompute.
  virtual size_t get_exposed_input_index(const std::string& input_name) const;
  virtual size_t get_num_exposed_inputs() const;
  virtual size_t get_num_hidden_inputs() const;
  virtual size_t get_num_inputs() const;

  // Our inputs.
  QVariantMap get_inputs() const;
  void set_params(const QVariantMap& inputs);

  virtual const QVariantMap& get_outputs() const;
  virtual QVariant get_output(const std::string& name) const;

 protected:
  // Our state.
  virtual void update_wires();

  // Our outputs. These are called during cleaning, so they don't dirty the instance's state.
  virtual void set_outputs(const QVariantMap& outputs);
  virtual void set_output(const std::string& name, const QVariant& value);

  virtual void copy_outputs(const std::string& output_name, Dep<InputCompute>& other_compute, const std::string& other_output_name);
  virtual void copy_outputs(const std::string& output_name, Dep<Compute>& other_compute, const std::string& other_output_name);

  // Plugs.
  Entity* create_input(const std::string& name, ParamType type = ParamType::kQVariantMap, bool exposed = true);
  Entity* create_output(const std::string& name, ParamType type = ParamType::kQVariantMap, bool exposed = true);
  Entity* create_namespace(const std::string& name);
  Entity* get_inputs_space();
  Entity* get_outputs_space();

 protected:
  void gather_inputs();

  // Our dynamic deps. These are gathered and not saved.
  // Map from input plug names to their internal output plugs.
  std::unordered_map<std::string, Dep<InputCompute> > _inputs;
  std::vector<Dep<InputCompute> > _exposed_inputs; // These are in alphabetical order.

  // Map from output plug names to results.
  // QVariantMap is a typedef for QMap<QString, QVariant>.
  QVariantMap _outputs;
};

}
