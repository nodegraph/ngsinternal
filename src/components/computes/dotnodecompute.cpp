#include <components/computes/dotnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>

namespace ngs {

DotNodeCompute::DotNodeCompute(Entity* entity)
    : Compute(entity, kDID()) {
}

DotNodeCompute::~DotNodeCompute() {
}

void DotNodeCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("in");
  create_output("out");
}

void DotNodeCompute::update_state() {
  internal();
  copy_outputs("out", _inputs.at("in"), "out");
}

}
