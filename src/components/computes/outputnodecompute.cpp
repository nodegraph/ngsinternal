#include <components/computes/inputcompute.h>
#include <components/computes/outputnodecompute.h>
#include <components/computes/flux.h>
#include <guicomponents/comms/commtypes.h>

namespace ngs {

// Output Nodes don't have output plugs, but they are associated
// with an output plug on the surrounding group.
// The OutputComputer mainly passes these calls onto its parent group node.

OutputNodeCompute::OutputNodeCompute(Entity* entity):
    Compute(entity, kDID()){
}

OutputNodeCompute::~OutputNodeCompute() {
}

void OutputNodeCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("in", QJsonValue());
}

const QJsonObject OutputNodeCompute::_hints = OutputNodeCompute::init_hints();
QJsonObject OutputNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "in", HintKey::kJSTypeHint, to_underlying(JSType::kObject));
  add_hint(m, "in", HintKey::kDescriptionHint, "The main object that flows through this node. This cannot be set manually.");

  return m;
}

bool OutputNodeCompute::update_state() {
  internal();
  Compute::update_state();
  set_output("out", _inputs->get_exposed().at("in")->get_output("out"));
  return true;
}

}
