#include <base/objectmodel/entity.h>
#include <base/utils/path.h>

#include <components/computes/computeglobals.h>
#include <components/computes/scriptnodecompute.h>
#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <components/computes/inputcompute.h>

#include <QtCore/QByteArray>
#include <QtCore/QDataStream>

namespace ngs {

ScriptNodeCompute::ScriptNodeCompute(Entity* entity):
  Compute(entity, kDID()),
  _compute_type(""){
}

ScriptNodeCompute::~ScriptNodeCompute() {
}

void ScriptNodeCompute::initialize_deps() {
  update_input_names();
  update_output_names();
}

void ScriptNodeCompute::uninitialize_deps() {
  // Destroy our node on the QML/javascript side.
  QVariant return_value;
  QVariant compute_type("NoOp");

  // Invoke a method on the NodePoolAPI QML object.
  QMetaObject::invokeMethod(g_node_pool_api, "destroy_js_node",
          Q_RETURN_ARG(QVariant, return_value),
          Q_ARG(QVariant, _flattened_path));
  // Make sure the node was created on the QML/javascript side.
  check_variant_is_bool_and_true(return_value, "was not able to destroy a node on the qml side");
}

void ScriptNodeCompute::set_compute_type(const std::string& compute_type) {
  start_method();
  _compute_type = compute_type;
}

const std::string& ScriptNodeCompute::get_compute_type() const {
  return _compute_type;
}

// This should be called after the composing entity is fully built.
void ScriptNodeCompute::configure() {
  // Get the node path.
  Path path;
  //_node->get_path(path);
  _flattened_path = path.get_as_string().c_str();

  // Create our node on the QML/javascript side.
  {
    QVariant return_value;
    QVariant compute_type("NoOp");

    // Invoke a method on the NodePoolAPI QML object.
    QMetaObject::invokeMethod(g_node_pool_api, "create_js_node",
            Q_RETURN_ARG(QVariant, return_value),
            Q_ARG(QVariant, _flattened_path),
            Q_ARG(QVariant, compute_type));

    // Make sure the node was created on the QML/javascript side.
    check_variant_is_bool_and_true(return_value, "was not able to create a node on the qml side");
  }

  // Send over our param values.
  {
    QVariant return_value;
    QVariant compute_type("NoOp");

    // Invoke a method on the NodePoolAPI QML object.
    QMetaObject::invokeMethod(g_node_pool_api, "set_js_params",
            Q_RETURN_ARG(QVariant, return_value),
            Q_ARG(QVariant, _flattened_path),
            Q_ARG(QVariant, _params));

    // Make sure the node was created on the QML/javascript side.
    check_variant_is_bool_and_true(return_value, "was not able to set the params on a node on the qml side");
  }

}

//void ScriptNodeCompute::save(SimpleSaver& saver) const {
//  // We write our params into a qt byte array first.
//  QByteArray buffer;
//  QDataStream stream(&buffer, QIODevice::WriteOnly);
//  stream << _params;
//
//  // Then we save it using the simple saver.
//  // Note for QByteArrays that are not wrapping a const piece of memory, it always tags on a NULL byte at the end.
//  // So the real size of the QByteArray is buffer.size()+1.
//  int num_bytes = buffer.size();
//  saver.save(num_bytes);
//  saver.save_raw(buffer.data(),num_bytes);
//}

//void ScriptNodeCompute::load(SimpleLoader& loader) {
//  int num_bytes;
//  loader.load(num_bytes);
//
//  QByteArray buffer;
//  buffer.resize(num_bytes);
//  loader.load_raw(buffer.data(),num_bytes);
//
//  QDataStream stream(&buffer, QIODevice::ReadOnly);
//  _params.clear();
//  stream >> _params;
//}

// Desired input and output plug configuration.
void ScriptNodeCompute::update_input_names() {
  // Invoke a method on the NodePoolAPI QML object.
  QVariant return_value;
  QMetaObject::invokeMethod(g_node_pool_api, "get_js_input_plug_names",
          Q_RETURN_ARG(QVariant, return_value),
          Q_ARG(QVariant, _flattened_path));

  QVariantList list = return_value.toList();
  check_variant_is_list(return_value,"was expecting to receive the input plug names as a list");

  // Extract the names from the variant list.
  _input_names.clear();
  for (QVariantList::iterator iter = list.begin(); iter != list.end(); ++iter) {
    _input_names.push_back(iter->toString().toStdString());
  }
}

void ScriptNodeCompute::update_output_names(){
  // Invoke a method on the NodePoolAPI QML object.
  QVariant return_value;
  QMetaObject::invokeMethod(g_node_pool_api, "get_js_output_plug_names",
          Q_RETURN_ARG(QVariant, return_value),
          Q_ARG(QVariant, _flattened_path));

  QVariantList list = return_value.toList();
  check_variant_is_list(return_value,"was expecting to receive the output plug names as a list");

  // Extract the names from the variant list.
  _output_names.clear();
  for (QVariantList::iterator iter = list.begin(); iter != list.end(); ++iter) {
    _output_names.push_back(iter->toString().toStdString());
  }
}

void ScriptNodeCompute::update_state() {
  static const QVariantMap empty;

  // Note we expect all our derived classes to compute all
  // output plugs at once.

  // Send the input data to our javascript node.
  for (auto &in: _inputs) {
    Dep<InputCompute> op = in.second;
    const std::string& input_name = in.first;
    const QVariant& input_value = op->get_result("out");
    QVariant return_value;

    // Send over one input value to the NodePoolAPI QML object.
    QMetaObject::invokeMethod(g_node_pool_api, "set_js_input_data",
            Q_RETURN_ARG(QVariant, return_value),
            Q_ARG(QVariant, _flattened_path),
            Q_ARG(QVariant, input_name.c_str()),
            Q_ARG(QVariant, input_value));

    // Check the return value.
    check_variant_is_bool_and_true(return_value, "was not able to set and input on a node on the qml side");
  }

  // Call on our javascript node to perform it compute.
  {
    QVariant return_value;

    // Invoke a method on the NodePoolAPI QML object.
    QMetaObject::invokeMethod(g_node_pool_api, "perform_js_compute",
            Q_RETURN_ARG(QVariant, return_value),
            Q_ARG(QVariant, _flattened_path));

    // Check the return value.
    check_variant_is_bool_and_true(return_value, "was not able to perform a compute on a node on the qml side");
  }

  // Gather the output data from the javascript side
  // and update our outputs.
  Entity* outputs = get_entity("./outputs");
  for (auto &out: outputs->get_children()) {
    const std::string& output_name = out.first;
    QVariantMap return_value;

    // Invoke a method on the NodePoolAPI QML object.
    QMetaObject::invokeMethod(g_node_pool_api, "get_js_output_data",
                              Q_RETURN_ARG(QVariantMap, return_value),
                              Q_ARG(QVariant, _flattened_path),
                              Q_ARG(QVariant, output_name.c_str()));

    // Check the return value has something.
    check_variant_is_map(return_value, "was expecting to receive output plug data as a map");

    // Store the output data in our map.
    set_result(output_name, return_value);
  }
}

}
