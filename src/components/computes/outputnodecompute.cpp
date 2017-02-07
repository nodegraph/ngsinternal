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

  EntityConfig c = config;
  c.expose_plug = true;
  // We take the unconnected value from the config.

  create_input("in", c);
}

const QJsonObject OutputNodeCompute::_hints = OutputNodeCompute::init_hints();
QJsonObject OutputNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "in", GUITypes::HintKey::DescriptionHint, "The main object that flows through this node. This cannot be set manually.");

  return m;
}

bool OutputNodeCompute::update_state() {
  internal();
  Compute::update_state();
  set_output("out", _inputs->get_exposed().at("in")->get_output("out"));
  return true;
}

}
