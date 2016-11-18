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

void DataNodeCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("value", QJsonObject(), false);
  create_output("out");
}

const QJsonObject DataNodeCompute::_hints = DataNodeCompute::init_hints();
QJsonObject DataNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "value", HintKey::kJSTypeHint, to_underlying(JSType::kObject));
  add_hint(m, "value", HintKey::kElementResizableHint, true);
  add_hint(m, "value", HintKey::kDescriptionHint, "The value which will be output by this node.");

  return m;
}

bool DataNodeCompute::update_state() {
  Compute::update_state();
  QJsonValue value = _inputs->get_input_value("value");
  set_output("out", value);
  return true;
}

}

