//#include <guicomponents/computes/mqttgrouptraits.h>
//#include <base/objectmodel/deploader.h>
//#include <guicomponents/comms/message.h>
//#include <guicomponents/quick/basenodegraphmanipulator.h>
//#include <components/computes/inputs.h>
//#include <guicomponents/comms/mqttworker.h>
//
//namespace ngs {
//
//MQTTGroupTraits::MQTTGroupTraits(Entity* entity)
//    : BaseGroupTraits(entity, kDID()),
//      _worker(this),
//      _inputs(this){
//  get_dep_loader()->register_fixed_dep(_worker, Path({}));
//  get_dep_loader()->register_fixed_dep(_inputs, Path({"."}));
//}
//
//MQTTGroupTraits::~MQTTGroupTraits() {
//}
//
//void MQTTGroupTraits::on_enter() {
//  _inputs->propagate_cleanliness();
//  QJsonObject inputs = _inputs->get_input_values();
//
//  QString address = inputs.value(Message::kHostAddress).toString();
//  QHostAddress host_address(address);
//  int port = inputs.value(Message::kPort).toInt();
//  QString username = inputs.value(Message::kUsername).toString();
//  QString password = inputs.value(Message::kPassword).toString();
//
//  _worker->dive_into_lockable_group(get_name(), host_address, port, username, password);
//}
//
//void MQTTGroupTraits::on_clean() {
//  _inputs->propagate_cleanliness();
//  QJsonObject inputs = _inputs->get_input_values();
//
//  QString address = inputs.value(Message::kHostAddress).toString();
//  QHostAddress host_address(address);
//  int port = inputs.value(Message::kPort).toInt();
//  QString username = inputs.value(Message::kUsername).toString();
//  QString password = inputs.value(Message::kPassword).toString();
//
//  _worker->clean_lockable_group(get_name(), host_address, port, username, password);
//}
//
//void MQTTGroupTraits::on_exit() {
//}
//
//}