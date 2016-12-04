#include <base/objectmodel/entity.h>
#include <components/computes/datanodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/jsonutils.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>
#include <guicomponents/comms/commtypes.h>

namespace ngs {

DataNodeCompute::DataNodeCompute(Entity* entity)
    : Compute(entity, kDID()) {
}

DataNodeCompute::~DataNodeCompute() {
}

void DataNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    // We take the unconnected value from the config.
    create_input("data", c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = true;
    c.unconnected_value = QJsonObject();
    create_output("out", c);
  }
}

const QJsonObject DataNodeCompute::_hints = DataNodeCompute::init_hints();
QJsonObject DataNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "data", HintKey::kDescriptionHint, "The data value to output from this node.");

  return m;
}

bool DataNodeCompute::update_state() {
  Compute::update_state();
  QJsonValue value = _inputs->get_input_value("data");
  set_output("out", value);
  return true;
}

}

