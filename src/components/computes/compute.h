#pragma once
#include <base/objectmodel/component.h>
#include <components/computes/computes_export.h>
#include <components/computes/paramtypes.h>
#include <components/computes/flux.h>

#include <entities/componentids.h>
#include <base/objectmodel/dep.h>
#include <string>

// QT
#include <QtCore/QObject>
#include <QtCore/QVariant>

namespace ngs {

#ifndef  ong_computes_EXPORTS
#define TEMPLATE_EXPORT extern
#else
#define TEMPLATE_EXPORT 
#endif

class InputCompute;
class BaseNodeGraphManipulator;

class COMPUTES_EXPORT Compute: public QObject, public Component {
  Q_OBJECT
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

  Q_INVOKABLE QVariant get_input(const QString& name) const;
  Q_INVOKABLE void set_input(const QString& name, const QVariant& value);

  // Our outputs.
  virtual const QVariantMap& get_outputs() const;
  virtual QVariant get_output(const std::string& name) const;

  // Our hints.
  virtual const QVariantMap& get_hints() const {return _hints;}

 protected:
  // Our state.
  virtual void initialize_wires();
  virtual bool update_state();
  virtual bool clean_finalize();

  // Our outputs. These are called during cleaning, so they don't dirty the instance's state.
  virtual void set_outputs(const QVariantMap& outputs);
  virtual void set_output(const std::string& name, const QVariant& value);

  // Plugs.
  Entity* create_input(const std::string& name, const QVariant& value, JSType type = JSType::kObject, bool exposed = true);
  Entity* create_output(const std::string& name, JSType type = JSType::kObject, bool exposed = true);
  Entity* create_namespace(const std::string& name);
  Entity* get_inputs_space();
  Entity* get_outputs_space();

  // Used by derived classes.
  static void add_hint(QVariantMap& map,
                       const std::string& name,
                       HintType hint_type,
                       const QVariant& value);

  void evaluate_script();

 protected:
  Dep<Inputs> _inputs;
  Dep<BaseNodeGraphManipulator> _ng_manipulator;

  // Map from output plug names to results.
  // QVariantMap is a typedef for QMap<QString, QVariant>.
  QVariantMap _outputs;

  // Hints are generally used for the hidden inputs (params) of a node.
  // They are used to display customized guis for the input.
  // Note this maps the input name to hints.
  // Hints are encoded by a key which is the string of the number representing the HintType.
  // The value is a QVariant holding an int representing an enum or another type.
  static const QVariantMap _hints;
};

}
