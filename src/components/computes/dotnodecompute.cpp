#include <components/computes/dotnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <guicomponents/comms/commtypes.h>

#include <base/objectmodel/deploader.h>

namespace ngs {

DotNodeCompute::DotNodeCompute(Entity* entity)
    : Compute(entity, kDID()) {
}

DotNodeCompute::~DotNodeCompute() {
}

void DotNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);

  EntityConfig c = config;
  c.expose_plug = true;
  c.unconnected_value = QJsonObject();

  // The type of the input and output is set to be an object.
  // This allows us to connect our input and output to any plug types,
  // as the input computes will convert values automatically.
  create_input("in", c);
  create_output("out", c);
}

const QJsonObject DotNodeCompute::_hints = DotNodeCompute::init_hints();
QJsonObject DotNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "in", HintKey::kDescriptionHint, "The main object that flows through this node. This cannot be set manually.");

  return m;
}

bool DotNodeCompute::update_state() {
  internal();
  Compute::update_state();
  set_output("out", _inputs->get_input_value("in"));
  return true;
}

}
