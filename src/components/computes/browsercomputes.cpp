#include <components/computes/browsercomputes.h>
#include <components/computes/computeglobals.h>
#include <components/computes/appcommunication.h>

#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>

namespace ngs {

OpenBrowserCompute::OpenBrowserCompute(Entity* entity)
    : Compute(entity, kDID()) {
}

OpenBrowserCompute::~OpenBrowserCompute() {
}

void OpenBrowserCompute::update_state() {

  assert(false);

  Compute::update_state();

  g_app_comm->handle_request_from_app("{ \"request\" : \"open_browser\" }");

  // Pass the inputs through.
  const QVariant &value = _inputs.at("in")->get_result("out");
  set_result("out", value);
}

//--------------------------------------------------------------------------------

CloseBrowserCompute::CloseBrowserCompute(Entity* entity)
    : Compute(entity, kDID()) {
}

CloseBrowserCompute::~CloseBrowserCompute() {
}

void CloseBrowserCompute::update_state() {
  Compute::update_state();

  g_app_comm->handle_request_from_app("{ \"request\" : \"close_browser\" }");

  // Pass the inputs through, but wipe out the browser cookies.
  // Todo: wipe out the browser cookies.
  const QVariant &value = _inputs.at("in")->get_result("out");
  set_result("out", value);
}

}
