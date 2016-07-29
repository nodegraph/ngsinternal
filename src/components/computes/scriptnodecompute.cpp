#include <base/objectmodel/entity.h>
#include <base/utils/path.h>

#include <components/computes/computeglobals.h>
#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <components/computes/inputcompute.h>
#include <components/computes/scriptnodecompute.h>
#include <gui/quick/nodegraphquickitemglobals.h>

#include <QtCore/QByteArray>
#include <QtCore/QDataStream>

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlExpression>

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

void ScriptNodeCompute::update_state() {
  // Make sure we have references to our input computes.
  Compute::update_state();

  // Accumulate all of our inputs into a map.
  QVariantMap inputs_obj;
  for (auto &in: _inputs) {
    Dep<InputCompute> op = in.second;
    const std::string& input_name = in.first;
    const QVariant& input_value = op->get_result("out");
    inputs_obj.insert(input_name.c_str(), input_value);
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
    _results = var.toMap();
  }

  // Make sure we have enough result data for each of our outputs.
  Entity* outputs = get_entity("./outputs");
  for (auto &out: outputs->get_children()) {
    const std::string& output_name = out.first;
    if (!_results.count(output_name.c_str())) {
      std::cerr << "Error: ScripNodeCompute did not produce enough data for its outputs: " << output_name << "\n";
    }
  }
}

}
