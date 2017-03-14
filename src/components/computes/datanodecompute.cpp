#include <base/objectmodel/entity.h>
#include <components/computes/datanodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/jsonutils.h>
#include <guicomponents/comms/guitypes.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

namespace ngs {

DataNodeCompute::DataNodeCompute(Entity* entity, ComponentDID did)
    : Compute(entity, did) {
}

DataNodeCompute::~DataNodeCompute() {
}

void DataNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  create_main_output(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    // We take the unconnected value from the config.
    create_input("data", c);
  }
}

const QJsonObject DataNodeCompute::_hints = DataNodeCompute::init_hints();
QJsonObject DataNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "data", GUITypes::HintKey::DescriptionHint, "The data value to output from this node.");

  return m;
}

bool DataNodeCompute::update_state() {
  Compute::update_state();
  QJsonValue value = _inputs->get_input_value("data");
  set_main_output(value);
  return true;
}

// ---------------------------------------------------------------------------------------

PasswordDataNodeCompute::PasswordDataNodeCompute(Entity* entity)
    : DataNodeCompute(entity, kDID()) {
}

PasswordDataNodeCompute::~PasswordDataNodeCompute() {
}

const QJsonObject PasswordDataNodeCompute::_hints = PasswordDataNodeCompute::init_hints();
QJsonObject PasswordDataNodeCompute::init_hints() {
  QJsonObject m = DataNodeCompute::init_hints();
  add_hint(m, "data", GUITypes::HintKey::PasswordHint, true);
  return m;
}

}

