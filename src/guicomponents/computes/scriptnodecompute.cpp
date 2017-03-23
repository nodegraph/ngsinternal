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

BaseScriptNodeCompute::BaseScriptNodeCompute(Entity* entity, ComponentDID did):
  QObject(),
  Compute(entity, did),
  _context(this) {
  // The context is set when update_wires is called.
  // In derived classes the input and output param names should be set here.
  get_dep_loader()->register_fixed_dep(_context, Path({".."}));
}

BaseScriptNodeCompute::~BaseScriptNodeCompute() {
}

void BaseScriptNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  create_main_input(config);
  create_main_output(config);
}

const QJsonObject BaseScriptNodeCompute::_hints = BaseScriptNodeCompute::init_hints();
QJsonObject BaseScriptNodeCompute::init_hints() {
  QJsonObject m;
  add_main_input_hint(m);
  return m;
}

// Not all group nodes have a script loop context, so this method
// search for the closest one surrounding us.
Dep<ScriptLoopContext> BaseScriptNodeCompute::get_closest_context() {
  Entity* group = our_entity()->get_parent();
  while(group) {
    if (group->has_comp_with_did(ComponentIID::kIScriptLoopContext, ComponentDID::kScriptLoopContext)) {
      return get_dep<ScriptLoopContext>(group);
    }
    group = group->get_parent();
  }
  return Dep<ScriptLoopContext>(NULL);
}

QJsonObject BaseScriptNodeCompute::get_context() {
  if (_context) {
    return _context->get_context();
  }
  // Return an empty object if we don't have a group with a context surrounding us.
  return QJsonObject();
}

void BaseScriptNodeCompute::set_context(const QJsonObject& context) {
  if (_context) {
    _context->set_context(context);
  }
  // Does nothing if we don't have a group with a context surrounding us.
}

QJsonObject BaseScriptNodeCompute::get_main_input() {
  return _inputs->get_main_input_object();
}

void BaseScriptNodeCompute::set_main_output(const QJsonObject& value) {
  Compute::set_main_output(value);
}

void BaseScriptNodeCompute::post(int post_type, const QString& title, const QJsonValue& value) {
  // Wrap the value in an object.
  QJsonObject obj;
  obj.insert(Message::kValue, value);
  // Post.
  _manipulator->send_post_value_signal(post_type, title, obj);
}

void BaseScriptNodeCompute::expose_to_eval_context(QQmlContext& eval_context) {
  QJsonObject inputs = _inputs->get_input_values();
  QJsonObject::iterator iter;
  for (iter = inputs.begin(); iter != inputs.end(); iter++) {
    if (iter.key() == Compute::kMainInputName) {
      continue;
    }
    eval_context.setContextProperty(iter.key(), inputs.value(iter.key()));
  }

}

void BaseScriptNodeCompute::update_wires() {
  std::cerr << "script group node is updating wires\n";
  _context = get_closest_context();
}

bool BaseScriptNodeCompute::update_state() {
  internal();
  Compute::update_state();

  QQmlEngine engine;
  // Create a new context to run our javascript expression.
  QQmlContext eval_context(engine.rootContext());
  // Expose our set_output method to the script.
  eval_context.setContextObject(this);
  // Add other properties into the context.
  expose_to_eval_context(eval_context);

  // Create the expression.

  // Script prefix.
  // Note: Ideally we could have used "in" and "out" as the variable names.
  // but the evaluator doesn't seem to like it. It seems it may already be defined in context.
  QString prefix = "var context = get_context();\n"
      "var input = get_main_input();\n"
      "var output = JSON.parse(JSON.stringify(input))\n";
  // Script suffix.
  QString suffix = "set_context(context);\n"
      "set_main_output(output);\n";
  // The full script.
  QString script = prefix + "\n" + _script_body + "\n" + suffix; // The extra \n is for safety, as the user may omit them.

  std::cerr << "evaluating script: " << script.toStdString() << "\n";
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
  std::cerr << "Finished evaluating expression\n";
  return true;
}

// ---------------------------------------------------------------------------------------

ScriptNodeCompute::ScriptNodeCompute(Entity* entity):
    BaseScriptNodeCompute(entity, kDID()) {
}

ScriptNodeCompute::~ScriptNodeCompute() {
}

void ScriptNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseScriptNodeCompute::create_inputs_outputs(config);

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

bool ScriptNodeCompute::update_state() {
  internal();
  _script_body = _inputs->get_input_string("script");
  return BaseScriptNodeCompute::update_state();
}

// ---------------------------------------------------------------------------------------

ExtractPropertyNodeCompute::ExtractPropertyNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
}

ExtractPropertyNodeCompute::~ExtractPropertyNodeCompute() {
}

void ExtractPropertyNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseScriptNodeCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "input.value";
    create_input(Message::kInputExpression, c);
  }
}

const QJsonObject ExtractPropertyNodeCompute::_hints = ExtractPropertyNodeCompute::init_hints();
QJsonObject ExtractPropertyNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();
  add_hint(m, Message::kInputExpression, GUITypes::HintKey::DescriptionHint, "The expression on our main input from which we copy to the main output's value property.");
  return m;
}

bool ExtractPropertyNodeCompute::update_state() {
  internal();
  QString input_expr = _inputs->get_input_value(Message::kInputExpression).toString();
  _script_body = "output.value = " + input_expr + "\n";
  return BaseScriptNodeCompute::update_state();
}

// ---------------------------------------------------------------------------------------

EmbedPropertyNodeCompute::EmbedPropertyNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
}

EmbedPropertyNodeCompute::~EmbedPropertyNodeCompute() {
}

void EmbedPropertyNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseScriptNodeCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "output.value";
    create_input(Message::kOutputExpression, c);
  }
}

const QJsonObject EmbedPropertyNodeCompute::_hints = EmbedPropertyNodeCompute::init_hints();
QJsonObject EmbedPropertyNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();
  add_hint(m, Message::kOutputExpression, GUITypes::HintKey::DescriptionHint, "The expression on our main output to which the main input's value property will be copied.");
  return m;
}

bool EmbedPropertyNodeCompute::update_state() {
  internal();
  QString output_expr = _inputs->get_input_value(Message::kOutputExpression).toString();
  _script_body = output_expr + " = input.value\n";
  return BaseScriptNodeCompute::update_state();
}

// ---------------------------------------------------------------------------------------

ErasePropertyNodeCompute::ErasePropertyNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
}

ErasePropertyNodeCompute::~ErasePropertyNodeCompute() {
}

void ErasePropertyNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseScriptNodeCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "output.value";
    create_input(Message::kOutputExpression, c);
  }
}

const QJsonObject ErasePropertyNodeCompute::_hints = ErasePropertyNodeCompute::init_hints();
QJsonObject ErasePropertyNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();
  add_hint(m, Message::kOutputExpression, GUITypes::HintKey::DescriptionHint, "The expression on our main output referring to the property we want to have erased.");
  return m;
}

bool ErasePropertyNodeCompute::update_state() {
  internal();
  QString output_expr = _inputs->get_input_value(Message::kOutputExpression).toString();
  _script_body = "delete " + output_expr + "\n";
  return BaseScriptNodeCompute::update_state();
}

}
