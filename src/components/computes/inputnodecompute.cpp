#include <base/objectmodel/entity.h>
#include <components/computes/inputnodecompute.h>

namespace ngs {

// Input Nodes don't have input plugs, but they are associated
// with an input plug on the surrounding group.
// The InputComputer mainly passes these calls onto its parent group node.

InputNodeCompute::InputNodeCompute(Entity* entity):
    Compute(entity, kDID()){
  _output_names.push_back("out");
}

InputNodeCompute::~InputNodeCompute() {
}

void InputNodeCompute::set_value(const QVariant& value) {
  set_result("out", value);
}

}

