#include <components/computes/inputcompute.h>
#include <components/computes/outputnodecompute.h>
#include <components/computes/flux.h>
#include <guicomponents/comms/guitypes.h>

namespace ngs {

// Output Nodes don't have output plugs, but they are associated
// with an output plug on the surrounding group.
// The OutputComputer mainly passes these calls onto its parent group node.

OutputNodeCompute::OutputNodeCompute(Entity* entity):
    Compute(entity, kDID()){
}

OutputNodeCompute::~OutputNodeCompute() {
}

void OutputNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  create_main_input(config);
}

const QJsonObject OutputNodeCompute::_hints = OutputNodeCompute::init_hints();
QJsonObject OutputNodeCompute::init_hints() {
  QJsonObject m;
  add_main_input_hint(m);
  return m;
}

bool OutputNodeCompute::update_state() {
  internal();
  Compute::update_state();
  set_main_output(_inputs->get_main_input_object());
  return true;
}

}
