#include <components/computes/inputcompute.h>
#include <components/computes/mocknodecompute.h>

namespace ngs {

MockNodeCompute::MockNodeCompute(Entity* entity):
    Compute(entity, kDID()),
    _counter(0) {
}

MockNodeCompute::~MockNodeCompute() {
}

void MockNodeCompute::update_state() {
  Compute::update_state();
  ++_counter;
  set_result("c", _counter);
  set_result("d", _counter+1);
}

}

