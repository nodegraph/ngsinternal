#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>

#include <guicomponents/computes/httpworker.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/computes/httpcomputes.h>

#include <functional>

namespace ngs {

//--------------------------------------------------------------------------------

BaseHTTPCompute::BaseHTTPCompute(Entity* entity, ComponentDID did)
    : Compute(entity, did),
      _worker(this),
      _scheduler(this) {
  get_dep_loader()->register_fixed_dep(_worker, Path({}));
  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
}

BaseHTTPCompute::~BaseHTTPCompute() {
}

void BaseHTTPCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("in", QJsonObject());
  create_output("out");
}

void BaseHTTPCompute::init_hints(QJsonObject& m) {
  add_hint(m, "in", HintType::kJSType, to_underlying(JSType::kObject));
  add_hint(m, "in", HintType::kDescription, "The main object that flows through this node. This cannot be set manually.");
}

void BaseHTTPCompute::dump_map(const QJsonObject& inputs) const {
  for(QJsonObject::const_iterator iter = inputs.begin(); iter != inputs.end(); ++iter) {
    std::cerr << iter.key().toStdString() << " : " << iter.value().toString().toStdString() << "\n";
  }
}

void BaseHTTPCompute::pre_update_state(TaskContext& tc) {
  internal();
  QJsonObject inputs = _inputs->get_input_values();
  _worker->queue_merge_chain_state(tc, inputs);
}

void BaseHTTPCompute::post_update_state(TaskContext& tc) {
  internal();
  std::function<void(const QJsonObject&)> on_get_outputs_bound = std::bind(&BaseHTTPCompute::on_get_outputs,this,std::placeholders::_1);
  _worker->queue_get_outputs(tc, on_get_outputs_bound);
}

void BaseHTTPCompute::on_get_outputs(const QJsonObject& chain_state) {
  internal();
  clean_finalize();

  // This copies the incoming data, to our output.
  // Derived classes will in add in extra data, extracted from the web.
  QJsonValue incoming = _inputs->get_input_value("in");
  set_output("out", incoming);
}

//--------------------------------------------------------------------------------

void HTTPCompute::create_inputs_outputs() {
  external();
  BaseHTTPCompute::create_inputs_outputs();
  create_input(Message::kURL, "http://httpbin.org/get", false);
  create_input(Message::kPayload, "{\"alpha\": 1, \"beta\": 2}", false);
  create_input(Message::kHTTPRequestMethod, 0, false);
  create_input(Message::kOutputPropertyName, "reply", false);
}

const QJsonObject HTTPCompute::_hints = HTTPCompute::init_hints();
QJsonObject HTTPCompute::init_hints() {
  QJsonObject m;
  BaseHTTPCompute::init_hints(m);

  add_hint(m, Message::kURL, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kURL, HintType::kDescription,
           "The URL to send the HTTP request to.");

  add_hint(m, Message::kPayload, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kPayload, HintType::kDescription,
           "The value to send with the HTTP request. This should be an expression in javascript.");

  add_hint(m, Message::kHTTPRequestMethod, HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, Message::kHTTPRequestMethod, HintType::kEnum, to_underlying(EnumHint::kHTTPSendType));
  add_hint(m, Message::kHTTPRequestMethod, HintType::kDescription,
           "The value to send with the HTTP request. This should be an expression in javascript.");

  add_hint(m, Message::kOutputPropertyName, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kOutputPropertyName, HintType::kDescription,
           "The name of the property in the output which will get the HTTP reply.");

  return m;
}

void HTTPCompute::on_get_outputs(const QJsonObject& chain_state) {
  internal();
  clean_finalize();

  // This copies the incoming data, to our output.
  // Derived classes will in add in extra data, extracted from the web.
  QJsonValue incoming = _inputs->get_input_value("in");
  QString prop_name = _inputs->get_input_value(Message::kOutputPropertyName).toString();
  if (incoming.isObject()) {
    QJsonObject obj = incoming.toObject();
    obj.insert(prop_name, chain_state.value("value"));
    set_output("out", obj);
  } else {
    set_output("out", incoming);
  }
}

bool HTTPCompute::update_state() {
  internal();
  BaseHTTPCompute::update_state();

  TaskContext tc(_scheduler);
  BaseHTTPCompute::pre_update_state(tc);
  _worker->queue_http_request(tc);
  BaseHTTPCompute::post_update_state(tc);
  return false;
}

//--------------------------------------------------------------------------------

}
