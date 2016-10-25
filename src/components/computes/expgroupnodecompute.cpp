#include <guicomponents/comms/basegrouptraits.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/expgroupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/outputnodecompute.h>

#include <QtCore/QDebug>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlExpression>

namespace ngs {

ExpGroupNodeCompute::ExpGroupNodeCompute(Entity* entity):
    QObject(NULL),
    GroupNodeCompute(entity, kDID()),
    _group_traits(this) {
  get_dep_loader()->register_fixed_dep(_group_traits, Path({"."}));
}

ExpGroupNodeCompute::~ExpGroupNodeCompute() {
}

void ExpGroupNodeCompute::create_inputs_outputs() {
  external();
  GroupNodeCompute::create_inputs_outputs();
  create_input("script", "", false);
}

const QVariantMap ExpGroupNodeCompute::_hints = ExpGroupNodeCompute::init_hints();
QVariantMap ExpGroupNodeCompute::init_hints() {
  QVariantMap m;
  add_hint(m, "script", HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, "script", HintType::kDescription, "The script which calculates the output values of this group.");
  return m;
}

//bool ExpGroupNodeCompute::update_state() {
//  _group_traits->on_enter();
//  bool done = GroupNodeCompute::update_state();
//  if (done) {
//    _group_traits->on_exit();
//  }
//  return done;
//}

void ExpGroupNodeCompute::set_output_value(const QString& name, const QVariant& value) {

}


void ExpGroupNodeCompute::evaluate_script() {
  internal();
  QQmlEngine engine;
  // Create a new context to run our javascript expression.
  QQmlContext eval_context(engine.rootContext());
  // Set ourself as the context object, so all our methods will be available to qml.
  eval_context.setContextObject(this);
  // Create the expression.
  const Dep<InputCompute>& input = _inputs->get_all().at("script");
  QVariantMap map = input->get_output("out").toMap();
  QQmlExpression expr(&eval_context, NULL, map["value"].toString());
  // Run the expression. We only care about the side effects and not the return value.
  QVariant result = expr.evaluate();
  if (expr.hasError()) {
    qDebug() << "Error: expression has an error: " << expr.error().toString() << "\n";
  }
}

bool ExpGroupNodeCompute::update_state() {
  internal();
  Compute::update_state();

  // For each input on this group if there is an associated input node, we set data on the input node.
  for (auto &iter: _inputs->get_all()) {
    const Dep<InputCompute>& input = iter.second;
    const std::string& input_name = input->get_name();
    // Find the input node inside this group with the same name as the input.
    Entity* input_node = our_entity()->get_child(input_name);
    // Make sure we have an input node.
    if (!input_node) {
      assert(false);
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
    }
  }

  // Find all of our output entities.
  // For each one if there is an associated output node, we clean it and cache the result.
  Entity* outputs = get_entity(Path({".","outputs"}));
  for (auto &iter: outputs->get_children()) {
    Entity* output_entity = iter.second;
    const std::string& output_name = output_entity->get_name();
    // Find an output node in this group with the same name as the output.
    Entity* output_node = our_entity()->get_child(output_name);
    // Make sure we have an output node.
    if (!output_node) {
      assert(false);
      continue;
    }
    // If the output node compute is dirty, then clean it.
    Dep<OutputNodeCompute> output_node_compute = get_dep<OutputNodeCompute>(output_node);
    if (output_node_compute->is_state_dirty()) {
      if (!output_node_compute->propagate_cleanliness()) {
        return false;
      }
    }
  }

  // If we get here then all of our internal computes have finished.
  for (auto &iter: outputs->get_children()) {
    Entity* output_entity = iter.second;
    const std::string& output_name = output_entity->get_name();
    Entity* output_node = our_entity()->get_child(output_name);
    Dep<OutputNodeCompute> output_node_compute = get_dep<OutputNodeCompute>(output_node);
    set_output(output_name, output_node_compute->get_output("out"));
  }

  return true;
}

}
