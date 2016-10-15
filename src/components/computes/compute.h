#pragma once
#include <base/objectmodel/component.h>
#include <components/computes/computes_export.h>
#include <components/computes/paramtypes.h>
#include <components/computes/flux.h>

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

class InputCompute;
class BaseNodeGraphManipulator;

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
  virtual void set_self_dirty(bool dirty);

  // Our inputs.
  QVariantMap get_inputs() const;
  QVariantMap get_hidden_inputs() const; // These are the params of the node.
  QVariantMap get_exposed_inputs() const; // These are the linkable inputs of the node.
  void set_params(const QVariantMap& inputs);

  virtual const QVariantMap& get_outputs() const;
  virtual QVariant get_output(const std::string& name) const;

 protected:
  // Our state.
  virtual void initialize_wires();
  virtual bool update_state();
  virtual bool clean_finalize();

  // Our outputs. These are called during cleaning, so they don't dirty the instance's state.
  virtual void set_outputs(const QVariantMap& outputs);
  virtual void set_output(const std::string& name, const QVariant& value);

  // Plugs.
  Entity* create_input(const std::string& name, ParamType type = ParamType::kMap, bool exposed = true);
  Entity* create_output(const std::string& name, ParamType type = ParamType::kMap, bool exposed = true);
  Entity* create_namespace(const std::string& name);
  Entity* get_inputs_space();
  Entity* get_outputs_space();

  void add_hint(const std::string& name, HintType hint_type, const QVariant& value);
  const QVariantMap& get_hints() const;

 protected:
  Dep<Inputs> _inputs;
  Dep<BaseNodeGraphManipulator> _ng_manipulator;

  // Map from output plug names to results.
  // QVariantMap is a typedef for QMap<QString, QVariant>.
  QVariantMap _outputs;

  // Hints are generally used for the hidden inputs (params) of a node.
  // They are used to display customized guis for the input.
  QVariantMap _hints;
};

}
