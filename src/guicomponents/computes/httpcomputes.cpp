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
      _http_worker(this),
      _task_scheduler(this) {
  get_dep_loader()->register_fixed_dep(_http_worker, Path({}));
  get_dep_loader()->register_fixed_dep(_task_scheduler, Path({}));
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
  _http_worker->queue_merge_chain_state(tc, inputs);
}

void HTTPCompute::post_update_state(TaskContext& tc) {
  internal();
  std::function<void(const QJsonObject&)> on_get_outputs_bound = std::bind(&HTTPCompute::on_get_outputs,this,std::placeholders::_1);
  _http_worker->queue_get_outputs(tc, on_get_outputs_bound);
}

void HTTPCompute::on_get_outputs(const QJsonObject& chain_state) {
  internal();
  clean_finalize();

  // This copies the incoming data, to our output.
  // Derived classes will in add in extra data, extracted from the web.
  QJsonValue incoming = _inputs->get_input_value("in");
  set_output("out", incoming);
}

}
