#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>

#include <guicomponents/comms/mqttworker.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/computes/mqttcomputes.h>

#include <functional>

namespace ngs {

//--------------------------------------------------------------------------------

BaseMQTTCompute::BaseMQTTCompute(Entity* entity, ComponentDID did)
    : Compute(entity, did),
      _worker(this),
      _scheduler(this) {
  get_dep_loader()->register_fixed_dep(_worker, Path({}));
  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
}

BaseMQTTCompute::~BaseMQTTCompute() {
}

void BaseMQTTCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("in", QJsonObject());
  create_output("out");
}

void BaseMQTTCompute::init_hints(QJsonObject& m) {
  add_hint(m, "in", HintType::kJSType, to_underlying(JSType::kObject));
  add_hint(m, "in", HintType::kDescription, "The main object that flows through this node. This cannot be set manually.");
}

void BaseMQTTCompute::post_update_state(TaskContext& tc) {
  internal();
  _worker->queue_finished_compute(tc, this);
}

void BaseMQTTCompute::on_finished_compute() {
  internal();
  clean_finalize();

  // This copies the incoming data, to our output.
  // Derived classes will in add in extra data, extracted from the web.
  QJsonValue incoming = _inputs->get_input_value("in");
  set_output("out", incoming);
}

//--------------------------------------------------------------------------------

void MQTTPublishCompute::create_inputs_outputs() {
  external();
  BaseMQTTCompute::create_inputs_outputs();
  create_input(Message::kTopic, "topic", false);
  create_input(Message::kMessage, "some message", false);
}

const QJsonObject MQTTPublishCompute::_hints = MQTTPublishCompute::init_hints();
QJsonObject MQTTPublishCompute::init_hints() {
  QJsonObject m;
  BaseMQTTCompute::init_hints(m);

  add_hint(m, Message::kTopic, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kTopic, HintType::kDescription, "The topic to publish the message to.");

  add_hint(m, Message::kMessage, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kMessage, HintType::kDescription, "The message to publish.");

  return m;
}

bool MQTTPublishCompute::update_state() {
  internal();
  BaseMQTTCompute::update_state();

  QString topic = _inputs->get_input_value(Message::kTopic).toString();
  QString message = _inputs->get_input_value(Message::kMessage).toString();

  TaskContext tc(_scheduler);
  _worker->queue_publish_task(tc, topic, message);
  post_update_state(tc);
  return false;
}

//--------------------------------------------------------------------------------

void MQTTSubscribeCompute::create_inputs_outputs() {
  external();
  BaseMQTTCompute::create_inputs_outputs();
  create_input(Message::kTopic, "topic", false);
  create_input(Message::kOutputPropertyName, "message", false);
}

void MQTTSubscribeCompute::on_finished_compute() {
  internal();
  clean_finalize();

  // This copies the incoming data, to our output.
  // Derived classes will in add in extra data, extracted from the web.
  QJsonValue in = _inputs->get_input_value("in");
  if (in.isObject()) {
    QJsonObject obj = in.toObject();
    if (_override.isNull() || _override.isUndefined()) {
      // Do nothing.
    } else if (_override.isString()) {
      QString prop_name = _inputs->get_input_value(Message::kOutputPropertyName).toString();
      obj.insert(prop_name, _override.toString());
    } else {
      std::cerr << "Error: the MQTTSubscribe's override has an improper type. It should be a string.";
    }
    set_output("out", obj);
  }
}

const QJsonObject MQTTSubscribeCompute::_hints = MQTTSubscribeCompute::init_hints();
QJsonObject MQTTSubscribeCompute::init_hints() {
  QJsonObject m;
  BaseMQTTCompute::init_hints(m);

  add_hint(m, Message::kTopic, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kTopic, HintType::kDescription, "The topic to subscribe to.");

  add_hint(m, Message::kOutputPropertyName, HintType::kJSType, to_underlying(JSType::kString));
  add_hint(m, Message::kOutputPropertyName, HintType::kDescription,
           "The name of the property in the output which will get the message subscribed to.");

  return m;
}

void MQTTSubscribeCompute::set_override(const QString& topic, const QString& payload) {
  external();
  QString our_topic = _inputs->get_input_value(Message::kTopic).toString();
  if (topic != our_topic) {
    return;
  }
  _override = payload;
}

const QJsonValue& MQTTSubscribeCompute::get_override() const {
  external();
  return _override;
}

void MQTTSubscribeCompute::clear_override() {
  internal();
  _override = QJsonValue();
}

bool MQTTSubscribeCompute::update_state() {
  internal();
  BaseMQTTCompute::update_state();
  QString topic = _inputs->get_input_value(Message::kTopic).toString();
  QString message = _inputs->get_input_value(Message::kMessage).toString();

  TaskContext tc(_scheduler);
  if (!_worker->is_subscribed(this, topic.toStdString())) {
    _worker->queue_subscribe_task(tc, topic, our_entity()->get_path());
  }
  post_update_state(tc);
  return true;
}

//--------------------------------------------------------------------------------

}
