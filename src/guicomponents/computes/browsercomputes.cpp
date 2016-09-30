#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>
#include <guicomponents/comms/appworker.h>
#include <guicomponents/computes/browsercomputes.h>

namespace ngs {

//--------------------------------------------------------------------------------

BrowserCompute::BrowserCompute(Entity* entity, size_t did)
    : Compute(entity, did),
      _app_worker(this) {
  get_dep_loader()->register_fixed_dep(_app_worker, Path({}));
}

BrowserCompute::~BrowserCompute() {
}

void BrowserCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("in");
  create_output("out");
}

void BrowserCompute::update_state() {
  internal();
  // Pass the inputs through.
  const QVariant &value = _inputs.at("in")->get_output("out");
  set_output("out", value);

  // Derived classes can do other work here or override this entirely.
}

void BrowserCompute::on_finished_sequence(const QVariantMap& outputs) {
  set_outputs(outputs);
}

QVariantMap BrowserCompute::get_map(const std::string& input_name) const {
  external();
  return _inputs.at(input_name)->get_output("out").toMap();
}

QString BrowserCompute::get_string(const std::string& input_name) const {
  external();
  return _inputs.at(input_name)->get_output("out").toString();
}

QStringList BrowserCompute::get_string_list(const std::string& input_name) const {
  external();
  return _inputs.at(input_name)->get_output("out").toStringList();
}

int BrowserCompute::get_int(const std::string& input_name) const {
  external();
  return _inputs.at(input_name)->get_output("out").toInt();
}

float BrowserCompute::get_float(const std::string& input_name) const {
  external();
  return _inputs.at(input_name)->get_output("out").toFloat();
}

WrapType BrowserCompute::get_wrap_type(const std::string& input_name) const {
  external();
  return static_cast<WrapType>(_inputs.at(input_name)->get_output("out").toInt());
}

ActionType BrowserCompute::get_action_type(const std::string& input_name) const {
  external();
  return static_cast<ActionType>(_inputs.at(input_name)->get_output("out").toInt());
}

Direction BrowserCompute::get_direction(const std::string& input_name) const {
  external();
  return static_cast<Direction>(_inputs.at(input_name)->get_output("out").toInt());
}

//--------------------------------------------------------------------------------

void OpenBrowserCompute::update_state() {
  internal();
  BrowserCompute::update_state();
  _app_worker->open_browser();
}

void CloseBrowserCompute::update_state() {
  internal();
  BrowserCompute::update_state();
  _app_worker->close_browser();
}

void CreateSetFromValuesCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input("type", ParamType::kWrapType, false);
  create_input("values", ParamType::kQStringList, false);
}

void CreateSetFromValuesCompute::update_state() {
  internal();
  // Make sure our input deps are hashed.
  BrowserCompute::update_state();

  QVariant wrap_type = _inputs.at("type")->get_output("out");
  QVariant match_values = _inputs.at("values")->get_output("out");

  QVariantMap args;
  args[Message::kWrapType] = wrap_type;
  args[Message::kMatchValues] = match_values;
  Message msg(RequestType::kCreateSetFromMatchValues, args);
  _app_worker->send_msg(msg);

  // Pass the inputs through.
  const QVariant &value = _inputs.at("in")->get_output("out");
  set_output("out", value);
}

void CreateSetFromTypeCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input("type", ParamType::kWrapType, false);
}

void CreateSetFromTypeCompute::update_state() {
  internal();
  BrowserCompute::update_state();

  QVariant wrap_type = _inputs.at("type")->get_output("out");

  QVariantMap args;
  args[Message::kWrapType] = wrap_type;
  Message msg(RequestType::kCreateSetFromWrapType, args);
  _app_worker->send_msg(msg);

  // Pass the inputs through.
  const QVariant &value = _inputs.at("in")->get_output("out");
  set_output("out", value);
}

void ClickActionCompute::create_inputs_outputs() {
  external();
  BrowserCompute::create_inputs_outputs();
  create_input(Message::kSetIndex, ParamType::kInt, false);
  create_input(Message::kOverlayIndex, ParamType::kInt, false);
  create_input(Message::kPosition, ParamType::kQVariantMap, false);
}

void ClickActionCompute::update_state() {
  internal();
  BrowserCompute::update_state();
  QVariantMap inputs = get_inputs();
  _app_worker->queue_chain_state_merge(inputs);
  _app_worker->queue_click();
  _app_worker->queue_finished();

  // Pass the inputs through.
  const QVariant &value = _inputs.at("in")->get_output("out");
  set_output("out", value);
}

}
