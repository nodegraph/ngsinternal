#pragma once
#include <base/objectmodel/component.h>
#include <components/computes/computes_export.h>
#include <components/computes/paramtypes.h>
#include <components/computes/flux.h>

#include <entities/componentids.h>
#include <base/objectmodel/dep.h>
#include <string>

// QT
#include <QtQml/QJSValue>

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

  static QJSValue deep_merge(const QJSValue& target, const QJSValue& source);

  COMPONENT_ID(Compute, InvalidComponent);
  Compute(Entity* entity, ComponentDID derived_id);
  virtual ~Compute();

  virtual void create_inputs_outputs();
  virtual void set_self_dirty(bool dirty);


  virtual void update_input_flux();

  // Get and set editable inputs. (These are the unconnected inputs.)
  QJSValue get_editable_inputs() const;
  void set_editable_inputs(const QJSValue& inputs);

  // Access inputs.
  const Dep<InputCompute>* get_input(const std::string& name) const;
  QJSValue get_input_value(const std::string& input_name, const std::string& output_port_name="out") const;
  QJSValue get_input_values(const std::string& output_port_name="out") const;

  // Access outputs.
  virtual const QJSValue& get_outputs() const;
  virtual QJSValue get_output(const std::string& name) const;

  // Get our hints.
  virtual const QJSValue& get_hints() const {return _hints;}

  // Get and Set our exposed settings.
  virtual QJSValue get_input_exposure() const;
  virtual void set_input_exposure(const QJSValue& settings);

  bool evaluate_expression_js(const QString& text, QJSValue& result, QString& error) const;
  bool evaluate_expression_qml(const QString& text, QJSValue& result, QString& error) const;

  void on_error();

 protected:
  // Our state.
  virtual void initialize_wires();
  virtual bool update_state();
  virtual bool clean_finalize();

  // Our outputs. These are called during cleaning, so they don't dirty the instance's state.
  virtual void set_outputs(const QJSValue& outputs);
  virtual void set_output(const std::string& name, const QJSValue& value);

  // Plugs.
  Entity* create_input(const std::string& name, const QJSValue& value, bool exposed = true);
  Entity* create_output(const std::string& name, bool exposed = true);
  Entity* create_namespace(const std::string& name);
  Entity* get_inputs_space();
  Entity* get_outputs_space();

  // Used by derived classes.
  static void add_hint(QJSValue& map,
                       const std::string& name,
                       HintType hint_type,
                       const QJSValue& value);

 protected:
  Dep<Inputs> _inputs;
  Dep<BaseNodeGraphManipulator> _ng_manipulator;

  // Our outputs.
  QJSValue _outputs;

  // Hints are generally used for all inputs on a node.
  // They are used to display customized guis for the input.
  // Note this maps the input name to hints.
  // Hints are encoded by a key which is the string of the number representing the HintType.
  // The value is a QJSValue holding an int representing an enum or another type.
  static const QJSValue _hints;
};

}
