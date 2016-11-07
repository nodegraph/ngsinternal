#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>

#include <guicomponents/comms/httpworker.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/computes/httpcomputes.h>

#include <functional>

namespace ngs {

//--------------------------------------------------------------------------------

HTTPCompute::HTTPCompute(Entity* entity, ComponentDID did)
    : Compute(entity, did),
      _worker(this),
      _scheduler(this) {
  get_dep_loader()->register_fixed_dep(_worker, Path({}));
  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
}

HTTPCompute::~HTTPCompute() {
}

void HTTPCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("in", QJsonObject());
  create_output("out");
}

void HTTPCompute::init_hints(QJsonObject& m) {
  add_hint(m, "in", HintType::kJSType, to_underlying(JSType::kObject));
  add_hint(m, "in", HintType::kDescription, "The main object that flows through this node. This cannot be set manually.");
}

void HTTPCompute::dump_map(const QJsonObject& inputs) const {
  for(QJsonObject::const_iterator iter = inputs.begin(); iter != inputs.end(); ++iter) {
    std::cerr << iter.key().toStdString() << " : " << iter.value().toString().toStdString() << "\n";
  }
}

void HTTPCompute::pre_update_state(TaskContext& tc) {
  internal();
  QJsonObject inputs = _inputs->get_input_values();
  _worker->queue_merge_chain_state(tc, inputs);
}

void HTTPCompute::post_update_state(TaskContext& tc) {
  internal();
  std::function<void(const QJsonObject&)> on_get_outputs_bound = std::bind(&HTTPCompute::on_get_outputs,this,std::placeholders::_1);
  _worker->queue_get_outputs(tc, on_get_outputs_bound);
}

void HTTPCompute::on_get_outputs(const QJsonObject& chain_state) {
  internal();
  clean_finalize();

  // This copies the incoming data, to our output.
  // Derived classes will in add in extra data, extracted from the web.
  QJsonValue incoming = _inputs->get_input_value("in");
  set_output("out", incoming);
}

//--------------------------------------------------------------------------------

void HTTPSendCompute::create_inputs_outputs() {
  external();
  HTTPCompute::create_inputs_outputs();
  create_input(Message::kURL, "", false);
  create_input(Message::kValue, "", false);
  create_input(Message::kHTTPRequestType, "", false);
}

const QJsonObject HTTPSendCompute::_hints = HTTPSendCompute::init_hints();
QJsonObject HTTPSendCompute::init_hints() {
  QJsonObject m;
  HTTPCompute::init_hints(m);

  add_hint(m, Message::kURL, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kURL, HintType::kDescription,
           "The URL to send the HTTP request to.");

  add_hint(m, Message::kValue, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kValue, HintType::kDescription,
           "The value to send with the HTTP request. This should be an expression in javascript.");

  add_hint(m, Message::kHTTPRequestType, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kHTTPRequestType, HintType::kEnum, to_underlying(EnumHint::kHTTPSentType));
  add_hint(m, Message::kHTTPRequestType, HintType::kDescription,
           "The value to send with the HTTP request. This should be an expression in javascript.");

  return m;
}

bool HTTPSendCompute::update_state() {
  internal();
  HTTPCompute::update_state();

  TaskContext tc(_scheduler);
  HTTPCompute::pre_update_state(tc);
  _worker->queue_http_send(tc);
  HTTPCompute::post_update_state(tc);
  return false;
}

//--------------------------------------------------------------------------------

void HTTPGetCompute::create_inputs_outputs() {
  external();
  HTTPCompute::create_inputs_outputs();
  create_input(Message::kURL, "", false);
  create_input(Message::kDataName, "data", false);
}

const QJsonObject HTTPGetCompute::_hints = HTTPGetCompute::init_hints();
QJsonObject HTTPGetCompute::init_hints() {
  QJsonObject m;
  HTTPCompute::init_hints(m);

  add_hint(m, Message::kURL, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kURL, HintType::kDescription,
           "The URL to send the HTTP request to.");

  add_hint(m, Message::kDataName, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kDataName, HintType::kDescription,
           "The return value from the GET request will be placed in property under this name.");

  return m;
}

void HTTPGetCompute::on_get_outputs(const QJsonObject& chain_state) {
  internal();
  clean_finalize();

  // This copies the incoming data, to our output.
  // Derived classes will in add in extra data, extracted from the web.
  QJsonValue incoming = _inputs->get_input_value("in");
  QString data_name = _inputs->get_input_value(Message::kDataName).toString();
  if (incoming.isObject()) {
    QJsonObject obj = incoming.toObject();
    obj.insert(data_name, chain_state.value("value"));
    set_output("out", obj);
  } else {
    set_output("out", incoming);
  }
}

bool HTTPGetCompute::update_state() {
  internal();
  HTTPCompute::update_state();

  TaskContext tc(_scheduler);
  HTTPCompute::pre_update_state(tc);
  _worker->queue_http_send(tc);
  HTTPCompute::post_update_state(tc);
  return false;
}

//--------------------------------------------------------------------------------

}
