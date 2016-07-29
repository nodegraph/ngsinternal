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

void DotNodeCompute::update_state() {
  Compute::update_state();
  const QVariant &value = _inputs.at("in")->get_result("out");
  set_result("out", value);
}

}
