#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
#include <base/utils/path.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <components/computes/inputcompute.h>
#include <components/computes/scriptloopcontext.h>
#include <guicomponents/comms/guitypes.h>
#include <guicomponents/computes/scriptnodecompute.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/quick/nodegraphmanipulator.h>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExpression>
#include <QtQml/QQmlContext>

namespace ngs {

ScriptNodeCompute::ScriptNodeCompute(Entity* entity):
  QObject(),
  Compute(entity, kDID()),
  _context(this) {
  // The context is set when update_wires is called.
  // In derived classes the input and output param names should be set here.
  get_dep_loader()->register_fixed_dep(_context, Path({".."}));
}

ScriptNodeCompute::~ScriptNodeCompute() {
}

void ScriptNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  create_main_input(config);
  create_main_output(config);

  {
    EntityConfig c = config;
    c.expose_plug = false;
    QString example = "// Context usage example:\n"
                      "// In loops the context can be used to count iterations as follows.\n"
                      "\n"
                      "if (context.count != undefined) {\n"
                      "  context.count += 1;\n"
                      "} else {\n"
                      "  context.count = 1;\n"
                      "}\n"
                      "\n"
                      "// Output usage example:\n"
                      "// Here we copy the value property from the input to the output.\n"
                      "\n"
                      "output.value = input.value;\n";
    c.unconnected_value = example;
    create_input("script", c);
  }
}

const QJsonObject ScriptNodeCompute::_hints = ScriptNodeCompute::init_hints();
QJsonObject ScriptNodeCompute::init_hints() {
  QJsonObject m;
  add_main_input_hint(m);
  add_hint(m, "script", GUITypes::HintKey::MultiLineHint, true);
  add_hint(m, "script", GUITypes::HintKey::DescriptionHint, "Script to compute the output value.\nPredeclared variables are. \ncontext: a modifiable javascript object which holds state across loop iterations. \ninput: the unmodifiable input object.\noutput: the modifiable output object.");
  return m;
}

// Not all group nodes have a script loop context, so this method
// search for the closest one surrounding us.
Dep<ScriptLoopContext> ScriptNodeCompute::get_closest_context() {
  Entity* group = our_entity()->get_parent();
  while(group) {
    if (group->has_comp_with_did(ComponentIID::kIScriptLoopContext, ComponentDID::kScriptLoopContext)) {
      return get_dep<ScriptLoopContext>(group);
    }
    group = group->get_parent();
  }
  return Dep<ScriptLoopContext>(NULL);
}

QJsonObject ScriptNodeCompute::get_context() {
  if (_context) {
    return _context->get_context();
  }
  // Return an empty object if we don't have a group with a context surrounding us.
  return QJsonObject();
}

void ScriptNodeCompute::set_context(const QJsonObject& context) {
  if (_context) {
    _context->set_context(context);
  }
  // Does nothing if we don't have a group with a context surrounding us.
}

QJsonObject ScriptNodeCompute::get_input() {
  return _inputs->get_main_input_object();
}

void ScriptNodeCompute::set_output(const QJsonObject& value) {
  set_main_output(value);
}

void ScriptNodeCompute::update_wires() {
  std::cerr << "script group node is updating wires\n";
  _context = get_closest_context();
}

bool ScriptNodeCompute::update_state() {
  internal();
  Compute::update_state();

  QQmlEngine engine;
  // Create a new context to run our javascript expression.
  QQmlContext eval_context(engine.rootContext());
  // Expose our set_output method to the script.
  eval_context.setContextObject(this);
  // Note we add other properties into the context as follows.
  //eval_context.setContextProperty("test", 111);

  // Create the expression.
  QString body = _inputs->get_input_string("script");
  // Script prefix.
  QString prefix = "var context = get_context();\n"
      "var input = get_input();\n"
      "var output = input;\n";
  // Script suffix.
  QString suffix = "set_context(context);\n"
      "set_output(output);\n";
  // The full script.
  QString script = prefix + "\n" + body + "\n" + suffix; // The extra \n is for safety, as the user may omit them.

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
