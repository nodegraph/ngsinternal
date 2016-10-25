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

class COMPUTES_EXPORT Compute: public Component {
 public:

  static const QVariant _empty_variant;
  static bool variant_is_bool(const QVariant& value);
  static bool variant_is_list(const QVariant& value);
  static bool variant_is_map(const QVariant& value);

  COMPONENT_ID(Compute, InvalidComponent);
  Compute(Entity* entity, ComponentDID derived_id);
  virtual ~Compute();

  virtual void create_inputs_outputs();
  virtual void set_self_dirty(bool dirty);


  virtual void update_input_flux();

  // Get and set editable inputs. (These are the unconnected inputs.)
  QVariantMap get_editable_inputs() const;
  void set_editable_inputs(const QVariantMap& inputs);

  // Get our current input values.
  QVariantMap get_input_values() const;

  // Get our outputs.
  virtual const QVariantMap& get_outputs() const;
  virtual QVariant get_output(const std::string& name) const;

  // Get our hints.
  virtual const QVariantMap& get_hints() const {return _hints;}

  // Get and Set our exposed settings.
  virtual QVariantMap get_input_exposure() const;
  virtual void set_input_exposure(const QVariantMap& settings);

 protected:
  // Our state.
  virtual void initialize_wires();
  virtual bool update_state();
  virtual bool clean_finalize();

  // Our outputs. These are called during cleaning, so they don't dirty the instance's state.
  virtual void set_outputs(const QVariantMap& outputs);
  virtual void set_output(const std::string& name, const QVariant& value);

  // Plugs.
  Entity* create_input(const std::string& name, const QVariant& value, bool exposed = true);
  Entity* create_output(const std::string& name, bool exposed = true);
  Entity* create_namespace(const std::string& name);
  Entity* get_inputs_space();
  Entity* get_outputs_space();

  // Used by derived classes.
  static void add_hint(QVariantMap& map,
                       const std::string& name,
                       HintType hint_type,
                       const QVariant& value);

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
