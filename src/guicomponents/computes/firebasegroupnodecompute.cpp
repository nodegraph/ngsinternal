#include <guicomponents/comms/basegrouptraits.h>
#include <base/objectmodel/deploader.h>
#include <guicomponents/computes/firebasegroupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/outputnodecompute.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <QtCore/QDebug>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlExpression>

namespace ngs {

FirebaseGroupNodeCompute::FirebaseGroupNodeCompute(Entity* entity):
    GroupNodeCompute(entity, kDID()),
    _group_traits(this) {
  get_dep_loader()->register_fixed_dep(_group_traits, Path({"."}));

  _on_group_inputs.insert(Message::kApiKey);
  _on_group_inputs.insert(Message::kAuthDomain);
  _on_group_inputs.insert(Message::kDatabaseURL);
  _on_group_inputs.insert(Message::kStorageBucket);
  _on_group_inputs.insert(Message::kEmail);
  _on_group_inputs.insert(Message::kPassword);
}

FirebaseGroupNodeCompute::~FirebaseGroupNodeCompute() {
}

void FirebaseGroupNodeCompute::create_inputs_outputs() {
  external();
  GroupNodeCompute::create_inputs_outputs();
  create_input(Message::kApiKey, "AIzaSyCXGNlyRf5uk8Xk1bvKXUcA53TC6Lc3I-A", false);
  create_input(Message::kAuthDomain, "test-project-91c10.firebaseapp.com", false);
  create_input(Message::kDatabaseURL, "https://test-project-91c10.firebaseio.com/", false);
  create_input(Message::kStorageBucket, "gs://test-project-91c10.appspot.com", false);
  create_input(Message::kEmail, "your_email@some_place.com", false);
  create_input(Message::kPassword, "your_password", false);
}

const QJsonObject FirebaseGroupNodeCompute::_hints = FirebaseGroupNodeCompute::init_hints();
QJsonObject FirebaseGroupNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, Message::kApiKey, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kApiKey, HintType::kDescription, "The Firebase api key.");

  add_hint(m, Message::kAuthDomain, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kAuthDomain, HintType::kDescription, "The Firebase authentication domain.");

  add_hint(m, Message::kDatabaseURL, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kDatabaseURL, HintType::kDescription, "The Firebase realtime database url.");

  add_hint(m, Message::kStorageBucket, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kStorageBucket, HintType::kDescription, "The Firebase storage bucket url.");

  add_hint(m, Message::kEmail, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kEmail, HintType::kDescription, "The email to sign in to Firebase. You must enable email/password authentication on your database.");

  add_hint(m, Message::kPassword, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kPassword, HintType::kDescription, "The password to sign in to Firebase. You must enable email/password authentication on your database.");

  return m;
}

bool FirebaseGroupNodeCompute::update_state() {
  // Need to call this to set the processing so that we can catch processing errors.
  Compute::update_state();
  _group_traits->on_clean();
  return false;
}

bool FirebaseGroupNodeCompute:: update_state2() {
  // Now call our base compute's update state.
  bool done = GroupNodeCompute::update_state();
  if (done) {
    _group_traits->on_exit();
    clean_finalize();
  }
  return done;
}

}
