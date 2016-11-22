#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/inputs.h>
#include <base/objectmodel/componentids.h>
#include <guicomponents/comms/hintkeys.h>

#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>

#include <string>

#include <QtCore/QJsonObject>

class QString;
class QJsonValue;
class QJsonObject;

namespace ngs {

class InputCompute;
class BaseNodeGraphManipulator;

class COMPUTES_EXPORT Compute: public Component {
 public:
  COMPONENT_ID(Compute, InvalidComponent);
  Compute(Entity* entity, ComponentDID derived_id);
  virtual ~Compute();

  virtual void create_inputs_outputs();
  virtual void set_self_dirty(bool dirty);

  virtual const Dep<Inputs>& get_inputs() {return _inputs;}

  // Inputs.
  virtual QJsonObject get_editable_inputs() const;
  virtual void set_editable_inputs(const QJsonObject& inputs);
  virtual QJsonObject get_input_exposure() const;
  virtual void set_input_exposure(const QJsonObject& settings);

  // Access outputs.
  virtual const QJsonObject& get_outputs() const;
  virtual QJsonValue get_output(const std::string& name) const;

  // Get our hints.
  virtual const QJsonObject& get_hints() const {return _hints;}

  bool eval_js_with_inputs(const QString& text, QJsonValue& result, QString& error) const;

  void on_error(const QString& error_message);


  virtual bool update_unlocked_group() {return true;}

 protected:
  // Our state.
  virtual void initialize_wires();
  virtual bool update_state();

  virtual bool clean_finalize();

  // Our outputs. These are called during cleaning, so they don't dirty the instance's state.
  virtual void set_outputs(const QJsonObject& outputs);
  virtual void set_output(const std::string& name, const QJsonValue& value);

  // Plugs.
  Entity* create_input(const std::string& name, const QJsonValue& value, bool exposed = true);
  Entity* create_output(const std::string& name, bool exposed = true);
  Entity* create_namespace(const std::string& name);
  Entity* get_inputs_space();
  Entity* get_outputs_space();

  // Used by derived classes.
  static void add_hint(QJsonObject& map, const std::string& name, HintKey hint_type, const QJsonValue& value);
  static void remove_hint(QJsonObject& node_hints, const std::string& name);

 protected:
  Dep<Inputs> _inputs;
  Dep<BaseNodeGraphManipulator> _manipulator;

  // Our outputs.
  QJsonObject _outputs;

  // Hints are generally used for all inputs on a node.
  // They are used to display customized guis for the input.
  // Note this maps the input name to hints.
  // The key is the string of the number representing the HintType.
  // The value is a QJsonValue holding an int representing an enum or another type.
  static const QJsonObject _hints;

};

}
