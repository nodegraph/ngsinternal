#include <base/objectmodel/deploader.h>
#include <guicomponents/computes/scriptgroupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/outputnodecompute.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>
#include <guicomponents/comms/commtypes.h>

#include <QtCore/QDebug>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlExpression>

namespace ngs {

ScriptGroupNodeCompute::ScriptGroupNodeCompute(Entity* entity):
    QObject(NULL),
    GroupNodeCompute(entity, kDID()),
    _fixed_inputs({"script"}) {
}

ScriptGroupNodeCompute::~ScriptGroupNodeCompute() {
}

void ScriptGroupNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  GroupNodeCompute::create_inputs_outputs(config);

  EntityConfig c = config;
  c.expose_plug = false;
  c.unconnected_value = "var input = get_input(\"input\");\n set_output(\"output\", input);\n";

  create_input("script", c);
}

const std::unordered_set<std::string>& ScriptGroupNodeCompute::get_fixed_inputs() const {
  return _fixed_inputs;
}

const QJsonObject ScriptGroupNodeCompute::_hints = ScriptGroupNodeCompute::init_hints();
QJsonObject ScriptGroupNodeCompute::init_hints() {
  QJsonObject m;
  //add_hint(m, "script", HintKey::kMultiLineHint, true);
  add_hint(m, "script", HintKey::kDescriptionHint, "The script which computes the output values of this group. All the input values are made available under their names. Use \"set_output_value(...)\" to set an output value.");
  return m;
}

QJsonValue ScriptGroupNodeCompute::get_input(const QString& name) {
  return _inputs->get_input_value(name.toStdString());
}

void ScriptGroupNodeCompute::set_output(const QString& name, const QJsonValue& value) {
  _outputs.insert(name, value);
}


bool ScriptGroupNodeCompute::evaluate_script() {
  internal();
  QQmlEngine engine;

  // Create a new context to run our javascript expression.
  QQmlContext eval_context(engine.rootContext());

  // Set ourself as the context object, so all our methods will be available to qml.
  eval_context.setContextObject(this);

  // Create the expression.
  QJsonValue script = _inputs->get_input_value("script");
  QQmlExpression expr(&eval_context, NULL, script.toString());

  // Run the expression. We only care about the side effects and not the return value.
  QVariant result = expr.evaluate();
  if (expr.hasError()) {
    qDebug() << "Error: expression has an error: " << expr.error().toString() << "\n";
    // Also show the error marker on the node.
    _manipulator->set_error_node(expr.error().toString());
    _manipulator->clear_ultimate_targets();
    return false;
  }
  return true;
}

bool ScriptGroupNodeCompute::update_state() {
  internal();
  Compute::update_state();

  // For each input on this group if there is an associated input node, we set data on the input node.
  for (auto &iter: _inputs->get_all()) {
    const Dep<InputCompute>& input = iter.second;
    const std::string& input_name = input->get_name();
    // Find the input node inside this group with the same name as the input.
    Entity* input_node = our_entity()->get_child(input_name);
    // Some of the inputs won't be input nodes inside the group, but actual params on the group.
    if (!input_node) {
      continue;
    }
    // Copy the input value to the input node, but only if they're different,
    // because this compute will get called multiple times as part of asynchronous updating
    // especially when the group contains asynchronous web action nodes.
    Dep<InputNodeCompute> input_node_compute = get_dep<InputNodeCompute>(input_node);
    if (input_node_compute) {
      if (input_node_compute->get_override() != input->get_output("out")) {
        input_node_compute->set_override(input->get_output("out"));
      }
      input_node_compute->clean_state();
    }
  }

  // Evaluate the script will set the outputs on this group.
  return evaluate_script();
}

}
