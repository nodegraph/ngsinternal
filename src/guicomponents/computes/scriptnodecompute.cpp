#include <base/objectmodel/entity.h>
#include <base/utils/path.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <components/computes/inputcompute.h>
#include <guicomponents/computes/scriptnodecompute.h>
#include <guicomponents/comms/message.h>

//#include <QtCore/QVariant>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExpression>
#include <QtQml/QQmlContext>

//#include <QtCore/QByteArray>
//#include <QtCore/QDataStream>
//
//#include <QtQml/QQmlEngine>
//#include <QtQml/QQmlContext>
//#include <QtQml/QQmlExpression>

//#include <QtCore/QJsonObject>

// Example of calling a qml method.
//    QMetaObject::invokeMethod(g_node_pool_api, "create_js_node",
//            Q_RETURN_ARG(QVariant, return_value),
//            Q_ARG(QVariant, _flattened_path),
//            Q_ARG(QVariant, compute_type));


namespace ngs {

ScriptNodeCompute::ScriptNodeCompute(Entity* entity):
  Compute(entity, kDID()) {
  // In derived classes the input and output param names should be set here.
}

ScriptNodeCompute::~ScriptNodeCompute() {
}

void ScriptNodeCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("in", QJsonObject());
  create_input("script", "", false);
  create_output("out");
}

const QJsonObject ScriptNodeCompute::_hints = ScriptNodeCompute::init_hints();
QJsonObject ScriptNodeCompute::init_hints() {
  QJsonObject m;
  add_hint(m, "in", HintType::kJSType, to_underlying(JSType::kObject));
  add_hint(m, "in", HintType::kDescription, "The main object that flows through this node. This cannot be set manually.");

  add_hint(m, "script", HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, "script", HintType::kDescription, "The script which performs the work for this node.");
  return m;
}

bool ScriptNodeCompute::update_state() {
  internal();
  Compute::update_state();

  // Accumulate all of our inputs into a map.
  QJsonObject inputs_obj;
  for (auto &in: _inputs->get_all()) {
    Dep<InputCompute> op = in.second;
    const std::string& input_name = in.first;
    QJsonValue out = op->get_output("out");
    inputs_obj.insert(input_name.c_str(), out);
  }

  {
    QQmlEngine engine;
    // Create a new context to run our javascript expression.
    QQmlContext eval_context(engine.rootContext());
    // Add the above inputs into the context.
    // The script will reference the inputs as inputs.in etc.
    eval_context.setContextProperty("inputs", inputs_obj);
    // Create the expression.
    QQmlExpression expr(&eval_context, NULL, _script);
    // Replace the results.
    QVariant var = expr.evaluate();
    _outputs = var.toJsonObject();
  }

  // Make sure we have enough result data for each of our outputs.
  Entity* outputs = get_entity(Path({".","outputs"}));
  for (auto &out: outputs->get_children()) {
    const std::string& output_name = out.first;
    if (!_outputs.contains(output_name.c_str())) {
      std::cerr << "Error: ScripNodeCompute did not produce enough data for its outputs: " << output_name << "\n";
    }
  }

  return true;
}

}
