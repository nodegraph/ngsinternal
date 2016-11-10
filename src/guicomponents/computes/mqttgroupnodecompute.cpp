#include <guicomponents/comms/basegrouptraits.h>
#include <base/objectmodel/deploader.h>
#include <guicomponents/computes/mqttgroupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/outputnodecompute.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <QtCore/QDebug>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlExpression>

namespace ngs {

MQTTGroupNodeCompute::MQTTGroupNodeCompute(Entity* entity):
    GroupNodeCompute(entity, kDID()),
    _group_traits(this),
    _unlocked(false){
  get_dep_loader()->register_fixed_dep(_group_traits, Path({"."}));

  _on_group_inputs.insert(Message::kHostAddress);
  _on_group_inputs.insert(Message::kPort);
  _on_group_inputs.insert(Message::kUsername);
  _on_group_inputs.insert(Message::kPassword);
}

MQTTGroupNodeCompute::~MQTTGroupNodeCompute() {
}

void MQTTGroupNodeCompute::create_inputs_outputs() {
  external();
  GroupNodeCompute::create_inputs_outputs();
  create_input(Message::kHostAddress, "127.0.0.1", false);
  create_input(Message::kPort, 1883, false);
  create_input(Message::kUsername, "", false);
  create_input(Message::kPassword, "", false);
}

const QJsonObject MQTTGroupNodeCompute::_hints = MQTTGroupNodeCompute::init_hints();
QJsonObject MQTTGroupNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, Message::kHostAddress, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kHostAddress, HintType::kDescription, "The ip address of the MQTT broker.");

  add_hint(m, Message::kPort, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kPort, HintType::kDescription, "The port number of the MQTT broker.");

  add_hint(m, Message::kUsername, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kUsername, HintType::kDescription, "The username to use if the MQTT broker uses authentication.");

  add_hint(m, Message::kPassword, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kPassword, HintType::kDescription, "The password to use if the MQTT broker uses authentication.");

  return m;
}

void MQTTGroupNodeCompute::set_self_dirty(bool dirty) {
  Compute::set_self_dirty(dirty);
  if (dirty) {
    _unlocked = false;
  }
}

bool MQTTGroupNodeCompute::update_state() {
  if (!_unlocked) {
    Compute::update_state(); // Need to call this to set the processing marker so that we can catch processing errors.
    _group_traits->on_clean();
    return false;
  }
  return update_unlocked_group();
}

bool MQTTGroupNodeCompute:: update_unlocked_group() {
  _unlocked = true;
  // Now call our base compute's update state.
  bool done = GroupNodeCompute::update_state();
  if (done) {
    _group_traits->on_exit();
    //clean_finalize();
  }
  return done;
}

}
