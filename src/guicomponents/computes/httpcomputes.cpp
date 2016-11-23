#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>

#include <guicomponents/computes/httpworker.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/commtypes.h>
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

void BaseHTTPCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);

  EntityConfig c = config;
  c.expose_plug = true;
  c.unconnected_value = QJsonObject();

  create_input("in", c);
  create_output("out", c);
}

void BaseHTTPCompute::init_hints(QJsonObject& m) {
  add_hint(m, "in", HintKey::kJSTypeHint, to_underlying(JSType::kObject));
  add_hint(m, "in", HintKey::kDescriptionHint, "The main object that flows through this node. This cannot be set manually.");
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

void HTTPCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseHTTPCompute::create_inputs_outputs(config);

  EntityConfig c = config;
  c.expose_plug = false;
  c.unconnected_value = "http://httpbin.org/get";
  create_input(Message::kURL, c);
  c.unconnected_value = "{\"alpha\": 1, \"beta\": 2}";
  create_input(Message::kPayload, c);
  c.unconnected_value = 0;
  create_input(Message::kHTTPRequestMethod, c);
  c.unconnected_value = "reply";
  create_input(Message::kOutputPropertyName, c);
}

const QJsonObject HTTPCompute::_hints = HTTPCompute::init_hints();
QJsonObject HTTPCompute::init_hints() {
  QJsonObject m;
  BaseHTTPCompute::init_hints(m);

  add_hint(m, Message::kURL, HintKey::kJSTypeHint, to_underlying(JSType::kString));
  add_hint(m, Message::kURL, HintKey::kDescriptionHint,
           "The URL to send the HTTP request to.");

  add_hint(m, Message::kPayload, HintKey::kJSTypeHint, to_underlying(JSType::kString));
  add_hint(m, Message::kPayload, HintKey::kDescriptionHint,
           "The value to send with the HTTP request. This should be an expression in javascript.");

  add_hint(m, Message::kHTTPRequestMethod, HintKey::kJSTypeHint, to_underlying(JSType::kNumber));
  add_hint(m, Message::kHTTPRequestMethod, HintKey::kEnumHint, to_underlying(EnumHintValue::kHTTPSendType));
  add_hint(m, Message::kHTTPRequestMethod, HintKey::kDescriptionHint,
           "The value to send with the HTTP request. This should be an expression in javascript.");

  add_hint(m, Message::kOutputPropertyName, HintKey::kJSTypeHint, to_underlying(JSType::kString));
  add_hint(m, Message::kOutputPropertyName, HintKey::kDescriptionHint,
           "The name of the property to add to our output. The value of the property will be the HTTP reply.");

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
