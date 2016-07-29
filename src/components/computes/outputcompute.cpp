#include <components/computes/outputcompute.h>
#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>


namespace ngs {

OutputCompute::OutputCompute(Entity* entity)
    : Compute(entity, kDID()),
      _node_compute(this) {
  get_dep_loader()->register_fixed_dep(_node_compute, "../..");
}

OutputCompute::~OutputCompute() {
}

void OutputCompute::update_state() {
  // Using our name we query our nodes compute results.
  const std::string& our_name = our_entity()->get_name();
  set_result("out", _node_compute->get_result(our_name));
}

}
