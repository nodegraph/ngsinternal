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
  create_input("a", QVariantMap());
  create_input("b", QVariantMap());
  create_output("c");
  create_output("d");
}

bool MockNodeCompute::update_state() {
  internal();
  Compute::update_state();

  ++_counter;
  set_output("c", _counter);
  set_output("d", _counter+1);
  return true;
}

}

