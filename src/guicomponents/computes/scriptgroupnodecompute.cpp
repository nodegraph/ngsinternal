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

const QJSValue ScriptGroupNodeCompute::_hints = ScriptGroupNodeCompute::init_hints();
QJSValue ScriptGroupNodeCompute::init_hints() {
  QJSValue m;
  add_hint(m, "script", HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, "script", HintType::kMultiLineEdit, true);
  add_hint(m, "script", HintType::kDescription, "The script which computes the output values of this group. All the input values are made available under their names. Use \"set_output_value(...)\" to set an output value.");
  return m;
}

void ScriptGroupNodeCompute::set_output_value(const QString& name, const QJSValue& value) {
  // Depending which variable is sent back from the js side, the QVariant may be a QJSValue type.
  // In this case we need to convert to a pure QVariant qt type, becuase a QJSValue will reference
  // values from the js engine context/environment.
//  QVariant non_js_value;
//  if (value.userType() == qMetaTypeId<QJSValue>()) {
//    non_js_value = qvariant_cast<QJSValue>(value).toVariant();
//  } else {
//    non_js_value = value;
//  }
//
//  // We wrap the value inside a map, if it's not already inside a map.
//  if (Compute::variant_is_map(non_js_value)) {
//    _outputs[name] = non_js_value;
//  } else {
//    QVariantMap map;
//    map["value"] = non_js_value;
//    _outputs[name] = map;
//  }

  _outputs.setProperty(name, value);
}


bool ScriptGroupNodeCompute::evaluate_script() {
  internal();
  QQmlEngine engine;

  // Create a new context to run our javascript expression.
  QQmlContext eval_context(engine.rootContext());

  // Set ourself as the context object, so all our methods will be available to qml.
  eval_context.setContextObject(this);

  // Add the input values into the context.
  for (auto &iter: _inputs->get_all()) {
    const Dep<InputCompute>& input = iter.second;
    QJSValue out = input->get_output("out");
    const std::string& input_name = input->get_name();
    eval_context.setContextProperty(input_name.c_str(), out.toVariant());
  }

  // Create the expression.
  QJSValue script = get_input_value("script");
  QQmlExpression expr(&eval_context, NULL, script.toString());

  // Wipe out our current outputs.
  _outputs = QJSValue();

  // Run the expression. We only care about the side effects and not the return value.
  QVariant result = expr.evaluate();
  if (expr.hasError()) {
    qDebug() << "Error: expression has an error: " << expr.error().toString() << "\n";
    // Also show the error marker on the node.
    _ng_manipulator->set_error_node();
    _ng_manipulator->clear_ultimate_target();
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
      if (!input_node_compute->get_override().strictlyEquals(input->get_output("out"))) {
        input_node_compute->set_override(input->get_output("out"));
      }
    }
  }

  // Evaluate the script will set the outputs on this group.
  return evaluate_script();
}

}
