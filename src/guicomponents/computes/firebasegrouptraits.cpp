//#include <guicomponents/computes/firebasegrouptraits.h>
//#include <base/objectmodel/deploader.h>
//#include <guicomponents/comms/message.h>
//#include <guicomponents/quick/basenodegraphmanipulator.h>
//#include <components/computes/inputs.h>
//#include <guicomponents/comms/browserworker.h>
//
//namespace ngs {
//
//FirebaseGroupTraits::FirebaseGroupTraits(Entity* entity)
//    : BaseGroupTraits(entity, kDID()),
//      _worker(this),
//      _inputs(this){
//  get_dep_loader()->register_fixed_dep(_worker, Path({}));
//  get_dep_loader()->register_fixed_dep(_inputs, Path({"."}));
//}
//
//FirebaseGroupTraits::~FirebaseGroupTraits() {
//}
//
//void FirebaseGroupTraits::on_enter() {
//  _inputs->propagate_cleanliness();
//  QJsonObject inputs = _inputs->get_input_values();
//
//  QString apiKey = inputs.value(Message::kApiKey).toString();
//  QString authDomain = inputs.value(Message::kAuthDomain).toString();
//  QString databaseURL = inputs.value(Message::kDatabaseURL).toString();
//  QString storageBucket = inputs.value(Message::kStorageBucket).toString();
//
//  QString email = inputs.value(Message::kEmail).toString();
//  QString password = inputs.value(Message::kPassword).toString();
//
//  _worker->dive_into_firebase_group(get_name(), apiKey, authDomain, databaseURL, storageBucket, email, password);
//}
//
//void FirebaseGroupTraits::on_clean() {
//  _inputs->propagate_cleanliness();
//  QJsonObject inputs = _inputs->get_input_values();
//
//  QString apiKey = inputs.value(Message::kApiKey).toString();
//  QString authDomain = inputs.value(Message::kAuthDomain).toString();
//  QString databaseURL = inputs.value(Message::kDatabaseURL).toString();
//  QString storageBucket = inputs.value(Message::kStorageBucket).toString();
//
//  QString email = inputs.value(Message::kEmail).toString();
//  QString password = inputs.value(Message::kPassword).toString();
//
//  _worker->clean_firebase_group(get_name(), apiKey, authDomain, databaseURL, storageBucket, email, password);
//}
//
//void FirebaseGroupTraits::on_exit() {
//  // We don't sign out because there may be nodes in the group that are
//  // listening to paths/locations in the firebase database.
//  // The nodes will get dirtied when firebase calls us back on changes.
//}
//
////bool GroupTraits::node_type_is_permitted(EntityDID did) const {
////}
////
////const std::unordered_set<EntityDID>& GroupTraits::get_permitted_node_types() {
////}
//
//}
