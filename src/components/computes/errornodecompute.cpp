#include <components/computes/errornodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <base/objectmodel/deploader.h>
#include <guicomponents/comms/guitypes.h>
#include <guicomponents/quick/nodegraphmanipulator.h>

namespace ngs {

ErrorNodeCompute::ErrorNodeCompute(Entity* entity)
    : Compute(entity, kDID()) {
}

ErrorNodeCompute::~ErrorNodeCompute() {
}

void ErrorNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  create_main_input(config);
  create_main_output(config);
  // The type of the input and output is set to be an object.
  // This allows us to connect our input and output to any plug types,
  // as the input computes will convert values automatically.
}

const QJsonObject ErrorNodeCompute::_hints = ErrorNodeCompute::init_hints();
QJsonObject ErrorNodeCompute::init_hints() {
  QJsonObject m;
  add_main_input_hint(m);
  return m;
}

bool ErrorNodeCompute::update_state() {
  internal();
  Compute::update_state();
  set_main_output(_inputs->get_main_input_object());

  // Also show the error marker on the node.
  std::string error = "An error node at the following location has been reached: " + our_entity()->get_path().get_as_string();
  _manipulator->set_error_node(error.c_str());
  _manipulator->clear_ultimate_targets();
  return false;
}

}
