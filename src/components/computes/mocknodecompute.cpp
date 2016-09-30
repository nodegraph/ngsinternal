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
  external();
  Compute::create_inputs_outputs();
  create_input("a");
  create_input("b");
  create_output("c");
  create_output("d");
}

void MockNodeCompute::update_state() {
  internal();
  ++_counter;
  set_output("c", _counter);
  set_output("d", _counter+1);
}

}

