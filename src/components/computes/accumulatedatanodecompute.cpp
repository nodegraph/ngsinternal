#include <base/objectmodel/entity.h>
#include <components/computes/accumulatedatanodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/datanodecompute.h>
#include <components/computes/jsonutils.h>
#include <guicomponents/comms/guitypes.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

namespace ngs {

AccumulateDataNodeCompute::AccumulateDataNodeCompute(Entity* entity):
    Compute(entity, kDID()) {
}

AccumulateDataNodeCompute::~AccumulateDataNodeCompute() {
}

void AccumulateDataNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = true;
    c.unconnected_value = QJsonObject();
    create_input("in", c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = true;
    create_output("out", c);
  }
}

const QJsonObject AccumulateDataNodeCompute::_hints = AccumulateDataNodeCompute::init_hints();
QJsonObject AccumulateDataNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "in", GUITypes::HintKey::DescriptionHint, "This value will be merged into its value from a previous pass through this node.");
  return m;
}

bool AccumulateDataNodeCompute::update_state() {
  Compute::update_state();

  QJsonValue input = _inputs->get_input_value("in");
  _override = JSONUtils::deep_merge(_override, input);
  set_output("out", _override);

  return true;
}

void AccumulateDataNodeCompute::set_override(const QJsonValue& override) {
  external();
  _override = override;
}

const QJsonValue& AccumulateDataNodeCompute::get_override() const {
  external();
  return _override;
}

void AccumulateDataNodeCompute::clear_override() {
  internal();
  _override = QJsonValue();
}

}

