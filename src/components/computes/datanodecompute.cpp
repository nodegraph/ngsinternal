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
    c.expose_plug = true;
    c.unconnected_value = QJsonObject();
    create_input("in", c);
  }

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

  add_hint(m, "in", HintKey::kDescriptionHint, "The target value into which the data value will be merged.");

  // testing
  //add_hint(m, "data", HintKey::kElementJSTypeHint, to_underlying(JSType::kString));

  add_hint(m, "data", HintKey::kElementResizableHint, true);
  add_hint(m, "data", HintKey::kDescriptionHint, "The source value which will be merged into the target(in) value.");

  return m;
}

bool DataNodeCompute::update_state() {
  Compute::update_state();
  QJsonValue incoming = _inputs->get_input_value("in");
  QJsonValue value = _inputs->get_input_value("data");

  QJsonValue output = JSONUtils::deep_merge(incoming, value);
  set_output("out", output);
  return true;
}

}

