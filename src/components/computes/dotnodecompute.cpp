#include <components/computes/dotnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <base/objectmodel/deploader.h>
#include <guicomponents/comms/guitypes.h>

namespace ngs {

DotNodeCompute::DotNodeCompute(Entity* entity)
    : Compute(entity, kDID()) {
}

DotNodeCompute::~DotNodeCompute() {
}

void DotNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  create_main_input(config);
  create_main_output(config);
  // The type of the input and output is set to be an object.
  // This allows us to connect our input and output to any plug types,
  // as the input computes will convert values automatically.
}

const QJsonObject DotNodeCompute::_hints = DotNodeCompute::init_hints();
QJsonObject DotNodeCompute::init_hints() {
  QJsonObject m;
  add_main_input_hint(m);
  return m;
}

bool DotNodeCompute::update_state() {
  internal();
  Compute::update_state();
  set_main_output(_inputs->get_main_input_object());
  return true;
}

}
