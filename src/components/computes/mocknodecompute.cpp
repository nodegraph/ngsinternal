#include <components/computes/inputcompute.h>
#include <components/computes/mocknodecompute.h>

namespace ngs {

MockNodeCompute::MockNodeCompute(Entity* entity):
    Compute(entity, kDID()),
    _counter(0) {
}

MockNodeCompute::~MockNodeCompute() {
}

void MockNodeCompute::create_inputs_outputs() {
  Compute::create_inputs_outputs();
  create_input("a");
  create_input("b");
  create_output("c");
  create_output("d");
}

void MockNodeCompute::update_state() {
  Compute::update_state();
  ++_counter;
  set_result("c", _counter);
  set_result("d", _counter+1);
}

}

