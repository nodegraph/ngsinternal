#include <guicomponents/comms/basegrouptraits.h>
#include <base/objectmodel/deploader.h>
#include <guicomponents/computes/scriptgroupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/outputnodecompute.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <QtCore/QDebug>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlExpression>

namespace ngs {

ScriptGroupNodeCompute::ScriptGroupNodeCompute(Entity* entity):
    QObject(NULL),
    GroupNodeCompute(entity, kDID()),
    _group_traits(this) {
  get_dep_loader()->register_fixed_dep(_group_traits, Path({"."}));

  _on_group_inputs.insert("script");
}

ScriptGroupNodeCompute::~ScriptGroupNodeCompute() {
}

void ScriptGroupNodeCompute::create_inputs_outputs() {
  external();
  GroupNodeCompute::create_inputs_outputs();
  create_input("script", "var delta = 99;\nset_output_value(\"output\", input.value + delta);\n", false);
}

const QJsonObject ScriptGroupNodeCompute::_hints = ScriptGroupNodeCompute::init_hints();
QJsonObject ScriptGroupNodeCompute::init_hints() {
  QJsonObject m;
  add_hint(m, "script", HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, "script", HintType::kMultiLineEdit, true);
  add_hint(m, "script", HintType::kDescription, "The script which computes the output values of this group. All the input values are made available under their names. Use \"set_output_value(...)\" to set an output value.");
  return m;
}

void ScriptGroupNodeCompute::set_output_value(const QString& name, const QJsonValue& value) {
  _outputs.insert(name, value);
}


bool ScriptGroupNodeCompute::evaluate_script() {
  internal();
  QQmlEngine engine;

  // Create a new context to run our javascript expression.
  QQmlContext eval_context(engine.rootContext());

  // Set ourself as the context object, so all our methods will be available to qml.
  eval_context.setContextObject(this);

  // Add the input values from the input nodes into the context.
  // Note that we use input values from the input nodes instead of on the group because
  // the default values are set properly on the input nodes for proper merging.
  // The inputs on the group have their default values set to null, so that all value
  // types will pass through.
  for (auto &iter: _inputs->get_all()) {
    const std::string& input_name = iter.first;
    // Try to find the input node inside this group with the same name as the input.
    Entity* input_node = our_entity()->get_child(input_name);
    // Not all the inputs on a group are associated with input nodes inside the group.
    // Some are just params directly on the group.
    if (input_node) {
      Dep<InputNodeCompute> input_node_compute = get_dep<InputNodeCompute>(input_node);
      QJsonValue value = input_node_compute->get_output("out");
      eval_context.setContextProperty(input_name.c_str(), value);
    } else {
      QJsonValue value = _inputs->get_input_value(input_name);
      eval_context.setContextProperty(input_name.c_str(), value);
    }
  }

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
