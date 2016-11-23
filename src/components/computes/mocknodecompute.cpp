#include <components/computes/inputcompute.h>
#include <components/computes/mocknodecompute.h>

namespace ngs {

MockNodeCompute::MockNodeCompute(Entity* entity):
    Compute(entity, kDID()),
    _counter(0) {
}

MockNodeCompute::~MockNodeCompute() {
}

void MockNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = true;
    c.unconnected_value = QJsonValue(0);
    create_input("a", c);
    create_input("b", c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = true;
    create_output("c", c);
    create_output("d", c);
  }
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

