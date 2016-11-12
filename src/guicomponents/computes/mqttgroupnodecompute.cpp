//#include <guicomponents/computes/mqttgroupnodecompute.h>
//#include <base/objectmodel/deploader.h>
//
//#include <guicomponents/comms/taskscheduler.h>
//
//
//namespace ngs {
//
//MQTTGroupNodeCompute::MQTTGroupNodeCompute(Entity* entity):
//    GroupNodeCompute(entity, kDID()),
//    _scheduler(this),
//    _worker(this),
//    _unlocked(false){
//  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
//  get_dep_loader()->register_fixed_dep(_worker, Path({}));
//
//  _on_group_inputs.insert(Message::kHostAddress);
//  _on_group_inputs.insert(Message::kPort);
//  _on_group_inputs.insert(Message::kUsername);
//  _on_group_inputs.insert(Message::kPassword);
//}
//
//MQTTGroupNodeCompute::~MQTTGroupNodeCompute() {
//}
//
//MQTTWorker::Config MQTTGroupNodeCompute::get_inputs() const {
//  QJsonObject inputs = _inputs->get_input_values();
//
//  MQTTWorker::Config config;
//  QString address = inputs.value(Message::kHostAddress).toString();
//  config.host_address = QHostAddress(address);
//  config.port = inputs.value(Message::kPort).toInt();
//  config.username = inputs.value(Message::kUsername).toString();
//  config.password = inputs.value(Message::kPassword).toString();
//  return config;
//}
//
//void MQTTGroupNodeCompute::create_inputs_outputs() {
//  external();
//  GroupNodeCompute::create_inputs_outputs();
//  create_input(Message::kHostAddress, "127.0.0.1", false);
//  create_input(Message::kPort, 1883, false);
//  create_input(Message::kUsername, "", false);
//  create_input(Message::kPassword, "", false);
//}
//
//const QJsonObject MQTTGroupNodeCompute::_hints = MQTTGroupNodeCompute::init_hints();
//QJsonObject MQTTGroupNodeCompute::init_hints() {
//  QJsonObject m;
//
//  add_hint(m, Message::kHostAddress, HintType::kJSType, to_underlying(JSType::kString));
//  add_hint(m, Message::kHostAddress, HintType::kDescription, "The ip address of the MQTT broker.");
//
//  add_hint(m, Message::kPort, HintType::kJSType, to_underlying(JSType::kNumber));
//  add_hint(m, Message::kPort, HintType::kDescription, "The port number of the MQTT broker.");
//
//  add_hint(m, Message::kUsername, HintType::kJSType, to_underlying(JSType::kString));
//  add_hint(m, Message::kUsername, HintType::kDescription, "The username to use if the MQTT broker uses authentication.");
//
//  add_hint(m, Message::kPassword, HintType::kJSType, to_underlying(JSType::kString));
//  add_hint(m, Message::kPassword, HintType::kDescription, "The password to use if the MQTT broker uses authentication.");
//
//  return m;
//}
//
//bool MQTTGroupNodeCompute::update_state() {
//  if (!_unlocked) {
//    Compute::update_state(); // Need to call this to set the processing marker so that we can catch processing errors.
//    unlock_group();
//    return false;
//  }
//  return update_unlocked_group();
//}
//
//bool MQTTGroupNodeCompute::group_is_unlocked() const {
//  MQTTWorker::Config config = get_inputs();
//  return _worker->is_connected(config);
//}
//
//void MQTTGroupNodeCompute::unlock_group() {
//  MQTTWorker::Config config = get_inputs();
//  TaskContext tc(_scheduler);
//  _worker->queue_connect_task(tc, config);
//}
//
//void MQTTGroupNodeCompute::unlock_group_and_dive() {
//  MQTTWorker::Config config = get_inputs();
//  TaskContext tc(_scheduler);
//  _worker->queue_connect_task(tc, config);
//  _worker->queue_dive_into_lockable_group(tc, get_name());
//}
//
//void MQTTGroupNodeCompute::lock_group() {
//}
//
//void MQTTGroupNodeCompute::lock_group_and_surface() {
//  TaskContext tc(_scheduler);
//  _worker->queue_surface_from_lockable_group(tc);
//}
//
//bool MQTTGroupNodeCompute::update_unlocked_group() {
//  // Now call our base compute's update state.
//  return GroupNodeCompute::update_state();
//}
//
//}
