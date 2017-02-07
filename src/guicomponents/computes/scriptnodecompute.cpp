#include <base/objectmodel/entity.h>
#include <base/utils/path.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <components/computes/inputcompute.h>
#include <guicomponents/comms/guitypes.h>
#include <guicomponents/computes/scriptnodecompute.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/quick/nodegraphmanipulator.h>

//#include <QtCore/QVariant>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExpression>
#include <QtQml/QQmlContext>

namespace ngs {

ScriptNodeCompute::ScriptNodeCompute(Entity* entity):
  QObject(),
  Compute(entity, kDID()) {
  // In derived classes the input and output param names should be set here.
}

ScriptNodeCompute::~ScriptNodeCompute() {
}

void ScriptNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);

  {
    EntityConfig c = config;
    c.expose_plug = true;
    c.unconnected_value = QJsonObject();
    create_input("in", c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "var value = get_input().value;\nset_output(value);";
    create_input("script", c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = true;
    create_output("out", c);
  }
}

const QJsonObject ScriptNodeCompute::_hints = ScriptNodeCompute::init_hints();
QJsonObject ScriptNodeCompute::init_hints() {
  QJsonObject m;
  add_hint(m, "in", GUITypes::HintKey::DescriptionHint, "The main object that flows through this node. This cannot be set manually.");

  add_hint(m, "script", GUITypes::HintKey::MultiLineHint, true);
  add_hint(m, "script", GUITypes::HintKey::DescriptionHint, "Script to compute output value.\nget_input(): retrieves the input.\nset_output_value(value): sets the output value property.");
  return m;
}

QJsonValue ScriptNodeCompute::get_input() {
  return _inputs->get_input_value("in");
}

void ScriptNodeCompute::set_output_value(const QJsonValue& value) {
  QJsonObject obj = _inputs->get_input_value("in").toObject();
  obj.insert("value", value);
  _outputs.insert("out", obj);
}

bool ScriptNodeCompute::update_state() {
  internal();
  Compute::update_state();

  QQmlEngine engine;
  // Create a new context to run our javascript expression.
  QQmlContext eval_context(engine.rootContext());
  // Expose our set_output method to the script.
  eval_context.setContextObject(this);
  // Add our single input into the context.
  //eval_context.setContextProperty("from", 111);
  //eval_context.setContextProperty("in", _inputs->get_input_value("in").toVariant());

  // Create the expression.
  QString script = _inputs->get_input_value("script").toString();
  QQmlExpression expr(&eval_context, NULL, script);
  // Evaluate the expression.
  bool value_is_undefined = false;
  QVariant result = expr.evaluate(&value_is_undefined);
  // Check for errors.
  if (expr.hasError()) {
    std::cerr << "Error: expression has an error: " << expr.error().toString().toStdString() << "\n";
    // Also show the error marker on the node.
    _manipulator->set_error_node(expr.error().toString());
    _manipulator->clear_ultimate_targets();
    return false;
  }
  return true;
}

}
