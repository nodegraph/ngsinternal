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
  create_input("a", QJsonValue());
  create_input("b", QJsonValue());
  create_output("c");
  create_output("d");
}

bool MockNodeCompute::update_state() {
  internal();
  Compute::update_state();

  ++_counter;

  QJsonValue c((double)_counter);
  set_output("c", c);

  QJsonValue d((double)_counter+1);
  set_output("d", d);
  return true;
}

}

