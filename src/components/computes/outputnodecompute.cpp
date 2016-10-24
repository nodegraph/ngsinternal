#include <components/computes/inputcompute.h>
#include <components/computes/outputnodecompute.h>
#include <components/computes/flux.h>


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
  create_input("in", QVariantMap());
}

const QVariantMap OutputNodeCompute::_hints = OutputNodeCompute::init_hints();
QVariantMap OutputNodeCompute::init_hints() {
  QVariantMap m;

  add_hint(m, "in", HintType::kJSType, to_underlying(JSType::kObject));
  add_hint(m, "in", HintType::kDescription, "The main object that flows through this node. This cannot be set manually.");

  return m;
}

bool OutputNodeCompute::update_state() {
  internal();
  Compute::update_state();
  set_output("out", _inputs->get_exposed().at("in")->get_output("out"));
  return true;
}

}
