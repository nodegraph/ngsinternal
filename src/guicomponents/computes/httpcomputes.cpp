#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <components/computes/jsonutils.h>

#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>
#include <guicomponents/comms/guitypes.h>

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
  get_dep_loader()->register_fixed_dep(_worker, Path());
  get_dep_loader()->register_fixed_dep(_scheduler, Path());
}

BaseHTTPCompute::~BaseHTTPCompute() {
}

void BaseHTTPCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  create_main_input(config);
  create_main_output(config);
}

void BaseHTTPCompute::init_hints(QJsonObject& m) {
  add_main_input_hint(m);
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
  QJsonObject incoming = _inputs->get_main_input_object();
  set_main_output(incoming);
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
}

const QJsonObject HTTPCompute::_hints = HTTPCompute::init_hints();
QJsonObject HTTPCompute::init_hints() {
  QJsonObject m;
  BaseHTTPCompute::init_hints(m);

  add_hint(m, Message::kURL, GUITypes::HintKey::DescriptionHint,
           "The URL to send the HTTP request to.");

  add_hint(m, Message::kPayload, GUITypes::HintKey::DescriptionHint,
           "The value to send with the HTTP request. This should be an expression in javascript.");

  add_hint(m, Message::kHTTPRequestMethod, GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::HTTPSendType));
  add_hint(m, Message::kHTTPRequestMethod, GUITypes::HintKey::DescriptionHint,
           "The value to send with the HTTP request. This should be an expression in javascript.");

  return m;
}

void HTTPCompute::on_get_outputs(const QJsonObject& chain_state) {
  internal();
  clean_finalize();

  // This copies the incoming data, to our output.
  // Derived classes will in add in extra data, extracted from the web.
  QJsonObject obj = _inputs->get_main_input_object();
  obj.insert(Message::kValue, chain_state.value(Message::kValue));
  set_main_output(obj);
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
