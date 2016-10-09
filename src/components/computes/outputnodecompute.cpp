#include <components/computes/inputcompute.h>
#include <components/computes/outputnodecompute.h>


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
  create_input("in");
}

void OutputNodeCompute::update_state() {
  internal();
  Compute::update_state();

  if (_named_inputs.count("in")) {
    copy_outputs("out", _named_inputs.at("in"), "out");
  }
}

}
