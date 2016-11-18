#include <base/objectmodel/entity.h>
#include <components/computes/datanodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/jsonutils.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>
#include <guicomponents/comms/commtypes.h>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

namespace ngs {

// Input Nodes don't have input plugs, but they are associated
// with an input plug on the surrounding group.
// The InputComputer mainly passes these calls onto its parent group node.

DataNodeCompute::DataNodeCompute(Entity* entity)
    : Compute(entity, kDID())  // , _use_override(false)
{
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

//void DataNodeCompute::set_override(const QJsonValue& override) {
//  external();
//  _override = override;
//  _use_override = true;
//}

//const QJsonValue& DataNodeCompute::get_override() const {
//  external();
//  return _override;
//}
//
//void DataNodeCompute::clear_override() {
//  internal();
//  _override = QJsonValue();
//  _use_override = false;
//}

}

