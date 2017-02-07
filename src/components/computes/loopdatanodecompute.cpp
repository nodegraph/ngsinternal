#include <base/objectmodel/entity.h>
#include <components/computes/inputcompute.h>
#include <components/computes/jsonutils.h>
#include <components/computes/loopdatanodecompute.h>
#include <guicomponents/comms/guitypes.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

namespace ngs {

LoopDataNodeCompute::LoopDataNodeCompute(Entity* entity)
    : Compute(entity, kDID()) {
}

LoopDataNodeCompute::~LoopDataNodeCompute() {
}

void LoopDataNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = true;
    c.unconnected_value = QJsonObject();
    create_output("out", c);
  }
}

const QJsonObject LoopDataNodeCompute::_hints = LoopDataNodeCompute::init_hints();
QJsonObject LoopDataNodeCompute::init_hints() {
  QJsonObject m;
  return m;
}

bool LoopDataNodeCompute::update_state() {
  Compute::update_state();
  set_output("out", _override);
  return true;
}

void LoopDataNodeCompute::set_override(const QJsonValue& override) {
  external();
  _override = override;
}

const QJsonValue& LoopDataNodeCompute::get_override() const {
  external();
  return _override;
}

void LoopDataNodeCompute::clear_override() {
  internal();
  _override = QJsonValue();
}

}

