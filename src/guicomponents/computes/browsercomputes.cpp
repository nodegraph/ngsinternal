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
  Compute::create_inputs_outputs();
  create_input("in");
  create_output("out");
}

void BrowserCompute::update_state() {
  // Pass the inputs through.
  const QVariant &value = _inputs.at("in")->get_result("out");
  set_result("out", value);

  // Derived classes can do other work here or override this entirely.
}

QVariantMap BrowserCompute::get_map(const std::string& input_name) {
  return _inputs.at(input_name)->get_result("out").toMap();
}

QString BrowserCompute::get_string(const std::string& input_name) {
  return _inputs.at(input_name)->get_result("out").toString();
}

QStringList BrowserCompute::get_string_list(const std::string& input_name) {
  return _inputs.at(input_name)->get_result("out").toStringList();
}

int BrowserCompute::get_int(const std::string& input_name) {
  return _inputs.at(input_name)->get_result("out").toInt();
}

float BrowserCompute::get_float(const std::string& input_name) {
  return _inputs.at(input_name)->get_result("out").toFloat();
}

WrapType BrowserCompute::get_wrap_type(const std::string& input_name) {
  return static_cast<WrapType>(_inputs.at(input_name)->get_result("out").toInt());
}

ActionType BrowserCompute::get_action_type(const std::string& input_name) {
  return static_cast<ActionType>(_inputs.at(input_name)->get_result("out").toInt());
}

Direction BrowserCompute::get_direction(const std::string& input_name) {
  return static_cast<Direction>(_inputs.at(input_name)->get_result("out").toInt());
}


//--------------------------------------------------------------------------------

void OpenBrowserCompute::update_state() {
  BrowserCompute::update_state();
  _app_worker->open_browser();
}

void CloseBrowserCompute::update_state() {
  BrowserCompute::update_state();
  _app_worker->close_browser();
}

void CreateSetFromValuesCompute::create_inputs_outputs() {
  BrowserCompute::create_inputs_outputs();
  create_input("type", ParamType::kWrapType, false);
  create_input("values", ParamType::kQStringList, false);
}

void CreateSetFromValuesCompute::update_state() {
  // Make sure our input deps are hashed.
  BrowserCompute::update_state();

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

void CreateSetFromTypeCompute::create_inputs_outputs() {
  BrowserCompute::create_inputs_outputs();
  create_input("type", ParamType::kWrapType, false);
}

void CreateSetFromTypeCompute::update_state() {
  BrowserCompute::update_state();

  QVariant wrap_type = _inputs.at("type")->get_result("out");

  QVariantMap args;
  args[Message::kWrapType] = wrap_type;
  Message msg(RequestType::kCreateSetFromWrapType, args);
  _app_worker->send_msg(msg);

  // Pass the inputs through.
  const QVariant &value = _inputs.at("in")->get_result("out");
  set_result("out", value);
}

void MouseActionCompute::create_inputs_outputs() {
  BrowserCompute::create_inputs_outputs();
  create_input("set_index", ParamType::kInt, false);
  create_input("overlay_index", ParamType::kInt, false);
  create_input("action_type", ParamType::kActionType, false);
  create_input("x", ParamType::kFloat, false);
  create_input("y", ParamType::kFloat, false);
}

void MouseActionCompute::update_state() {
  BrowserCompute::update_state();

  int set_index = _inputs.at("set_index")->get_result("out").toInt();
  int overlay_index = _inputs.at("overlay_index")->get_result("out").toInt();
  int action_type = _inputs.at("action_type")->get_result("out").toInt();
  float rel_x = _inputs.at("x")->get_result("out").toFloat();
  float rel_y = _inputs.at("y")->get_result("out").toFloat();

  if (action_type == ActionType::kSendClick) {
    _app_worker->_click(set_index, overlay_index, rel_x, rel_y);
  } else if (action_type == ActionType::kMouseOver) {

  } else if (action_type == ActionType::kStartMouseHover) {

  } else {
    std::cerr << "Error unknown mouse action type encountered.\n";
  }

  // Pass the inputs through.
  const QVariant &value = _inputs.at("in")->get_result("out");
  set_result("out", value);
}

}
