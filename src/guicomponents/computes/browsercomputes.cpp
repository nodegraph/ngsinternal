#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>
#include <guicomponents/comms/appworker.h>
#include <guicomponents/computes/browsercomputes.h>

namespace ngs {

//--------------------------------------------------------------------------------

OpenBrowserCompute::OpenBrowserCompute(Entity* entity)
    : Compute(entity, kDID()),
      _app_worker(this) {
  get_dep_loader()->register_fixed_dep(_app_worker, "");
}

OpenBrowserCompute::~OpenBrowserCompute() {
}

void OpenBrowserCompute::update_state() {
  Compute::update_state();

  _app_worker->open_browser();

  // Pass the inputs through.
  const QVariant &value = _inputs.at("in")->get_result("out");
  set_result("out", value);
}

//--------------------------------------------------------------------------------

CloseBrowserCompute::CloseBrowserCompute(Entity* entity)
    : Compute(entity, kDID()),
      _app_worker(this) {
  get_dep_loader()->register_fixed_dep(_app_worker, "");
}

CloseBrowserCompute::~CloseBrowserCompute() {
}

void CloseBrowserCompute::update_state() {
  Compute::update_state();

  _app_worker->close_browser();

  // Pass the inputs through.
  const QVariant &value = _inputs.at("in")->get_result("out");
  set_result("out", value);
}

//--------------------------------------------------------------------------------

CreateSetFromValuesCompute::CreateSetFromValuesCompute(Entity* entity)
    : Compute(entity, kDID()),
      _app_worker(this) {
  get_dep_loader()->register_fixed_dep(_app_worker, "");
}

CreateSetFromValuesCompute::~CreateSetFromValuesCompute() {
}

void CreateSetFromValuesCompute::update_state() {
  // Make sure our input deps are hashed.
  Compute::update_state();

  QVariant wrap_type = _inputs.at("type")->get_result("out");
  QVariant match_values = _inputs.at("values")->get_result("out");

  QVariantMap args;
  args[Message::kWrapType] = wrap_type;
  args[Message::kMatchValues] = match_values;
  Message msg(RequestType::kCreateSetFromMatchValues, args);
  _app_worker->send_msg(msg);

  // Pass the inputs through.
  const QVariant &value = _inputs.at("in")->get_result("out");
  set_result("out", value);
}

//--------------------------------------------------------------------------------

CreateSetFromTypeCompute::CreateSetFromTypeCompute(Entity* entity)
    : Compute(entity, kDID()),
      _app_worker(this) {
  get_dep_loader()->register_fixed_dep(_app_worker, "");
}

CreateSetFromTypeCompute::~CreateSetFromTypeCompute() {
}

void CreateSetFromTypeCompute::update_state() {
  Compute::update_state();

//  _app_worker->handle_request_from_app("{ \"request\" : \"close_browser\" }");
//
//  // Pass the inputs through, but wipe out the browser cookies.
//  // Todo: wipe out the browser cookies.
//  const QVariant &value = _inputs.at("in")->get_result("out");
//  set_result("out", value);
}

}
