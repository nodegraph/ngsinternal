#include <components/computes/computeglobals.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>
#include <guicomponents/comms/appcomm.h>
#include <guicomponents/computes/browsercomputes.h>

namespace ngs {

//--------------------------------------------------------------------------------

OpenBrowserCompute::OpenBrowserCompute(Entity* entity)
    : Compute(entity, kDID()) {
}

OpenBrowserCompute::~OpenBrowserCompute() {
}

void OpenBrowserCompute::update_state() {
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

//--------------------------------------------------------------------------------

CreateSetFromValuesCompute::CreateSetFromValuesCompute(Entity* entity)
    : Compute(entity, kDID()) {
}

CreateSetFromValuesCompute::~CreateSetFromValuesCompute() {
}

void CreateSetFromValuesCompute::update_state() {
  // Make sure our input deps are hashed.
  Compute::update_state();

  QVariant type = _inputs.at("type")->get_result("out");
  QVariant values = _inputs.at("values")->get_result("out");

  std::string request =
      "{\
         \"request\": \"create_set_from_match_values\", \
         \"wrap_type\": \"" + type.toString().toStdString() + "\", \
         \"match_values\": \"" + values.toString().toStdString() + "\" \
       }";
  g_app_comm->handle_request_from_app(request.c_str());

  // Pass the inputs through, but wipe out the browser cookies.
  // Todo: wipe out the browser cookies.
  const QVariant &value = _inputs.at("in")->get_result("out");
  set_result("out", value);
}

//--------------------------------------------------------------------------------

CreateSetFromTypeCompute::CreateSetFromTypeCompute(Entity* entity)
    : Compute(entity, kDID()) {
}

CreateSetFromTypeCompute::~CreateSetFromTypeCompute() {
}

void CreateSetFromTypeCompute::update_state() {
  Compute::update_state();

  g_app_comm->handle_request_from_app("{ \"request\" : \"close_browser\" }");

  // Pass the inputs through, but wipe out the browser cookies.
  // Todo: wipe out the browser cookies.
  const QVariant &value = _inputs.at("in")->get_result("out");
  set_result("out", value);
}

}
