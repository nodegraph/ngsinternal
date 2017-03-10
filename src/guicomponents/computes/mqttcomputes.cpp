#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <components/computes/jsonutils.h>

#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>
#include <guicomponents/comms/guitypes.h>

#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/computes/mqttcomputes.h>
#include <guicomponents/computes/mqtthostcompute.h>
#include <functional>

namespace ngs {

//--------------------------------------------------------------------------------

BaseMQTTCompute::BaseMQTTCompute(Entity* entity, ComponentDID did)
    : Compute(entity, did),
      _scheduler(this),
      _enter(this) {
  get_dep_loader()->register_fixed_dep(_scheduler, Path());
}

BaseMQTTCompute::~BaseMQTTCompute() {
}

void BaseMQTTCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  create_main_input(config);
  create_main_output(config);
}

void BaseMQTTCompute::init_hints(QJsonObject& m) {
  add_main_input_hint(m);
}

void BaseMQTTCompute::update_wires() {
  // This caches the dep, and will only dirty this component when it changes.
  _enter = find_host_node();
}

Entity* BaseMQTTCompute::find_group_context() const {
  Entity* e = our_entity();
  while(e) {
    if (e->get_did() == EntityDID::kMQTTGroupNodeEntity) {
      return e;
    }
    e = e->get_parent();
  }
  assert(false);
  return NULL;
}

Dep<MQTTHostCompute> BaseMQTTCompute::find_host_node() {
  Entity* group = find_group_context();
  Entity* host = group->get_child("host");
  assert(host);
  return get_dep<MQTTHostCompute>(host);
}

void BaseMQTTCompute::append_callback_tasks(TaskContext& tc) {
  internal();
  std::function<void()> done = std::bind(&BaseMQTTCompute::on_finished_task, this);
  _enter->queue_finished_task(tc, done);
}

void BaseMQTTCompute::on_finished_task() {
  internal();
  clean_finalize();

  // This copies the incoming data, to our output.
  // Derived classes will in add in extra data, extracted from the web.
  QJsonObject incoming = _inputs->get_main_input_object();
  set_main_output(incoming);
}

//--------------------------------------------------------------------------------

void MQTTPublishCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseMQTTCompute::create_inputs_outputs(config);

  EntityConfig c = config;
  c.expose_plug = false;
  c.unconnected_value = "topic";
  create_input(Message::kTopic, c);
  c.unconnected_value = "some message";
  create_input(Message::kMessage, c);
}

const QJsonObject MQTTPublishCompute::_hints = MQTTPublishCompute::init_hints();
QJsonObject MQTTPublishCompute::init_hints() {
  QJsonObject m;
  BaseMQTTCompute::init_hints(m);

  add_hint(m, Message::kTopic, GUITypes::HintKey::DescriptionHint, "The topic to publish the message to.");
  add_hint(m, Message::kMessage, GUITypes::HintKey::DescriptionHint, "The message to publish.");

  return m;
}

bool MQTTPublishCompute::update_state() {
  internal();
  BaseMQTTCompute::update_state();

  QString topic = _inputs->get_input_string(Message::kTopic);
  QString message = _inputs->get_input_string(Message::kMessage);

  TaskContext tc(_scheduler);
  _enter->queue_publish_task(tc, topic, message);
  append_callback_tasks(tc);
  return false;
}

//--------------------------------------------------------------------------------

void MQTTSubscribeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseMQTTCompute::create_inputs_outputs(config);

  EntityConfig c = config;
  c.expose_plug = false;
  c.unconnected_value = "topic";
  create_input(Message::kTopic, c);
}

void MQTTSubscribeCompute::on_finished_task() {
  internal();
  clean_finalize();

  // This copies the incoming data, to our output.
  // Derived classes will in add in extra data, extracted from the web.
  QJsonObject obj = _inputs->get_main_input_object();
  if (_override.isNull() || _override.isUndefined()) {
    // Do nothing.
  } else if (_override.isString()) {
    obj.insert(Message::kValue, _override.toString());
  } else {
    std::cerr << "Error: the MQTTSubscribe's override has an improper type. It should be a string.";
  }
  set_main_output(obj);
}

const QJsonObject MQTTSubscribeCompute::_hints = MQTTSubscribeCompute::init_hints();
QJsonObject MQTTSubscribeCompute::init_hints() {
  QJsonObject m;
  BaseMQTTCompute::init_hints(m);

  add_hint(m, Message::kTopic, GUITypes::HintKey::DescriptionHint, "The topic to subscribe to.");

  return m;
}

void MQTTSubscribeCompute::set_override(const QString& topic, const QString& payload) {
  external();
  if (_topic != topic) {
    return;
  }
  _override = payload;
}

const QJsonValue& MQTTSubscribeCompute::get_override() const {
  external();
  return _override;
}

void MQTTSubscribeCompute::clear_override() {
  external();
  _override = QJsonValue();
}

bool MQTTSubscribeCompute::update_state() {
  internal();
  BaseMQTTCompute::update_state();
  _topic = _inputs->get_input_string(Message::kTopic);

  TaskContext tc(_scheduler);
  if (!_enter->is_subscribed(this, _topic.toStdString())) {
    _enter->queue_subscribe_task(tc, _topic, get_path());
  }
  // Let on_finished_task() handle setting our final output value.
  append_callback_tasks(tc);
  return true;
}

bool MQTTSubscribeCompute::destroy_state() {
  // Note we can't launch any computes that require multiple cleaning passes.
  // We can only use cached values here to launch one task, and we can't wait for it.
  internal();
  TaskContext tc(_scheduler);
  // Note the context is usually not null when we're working within our group,
  // however during destruction of our surrounding node
  // the context may be null as the deletion order of nodes within in a group is random.
  // The context is just another node in our group.
  if (_enter) {
    _enter->queue_unsubscribe_task(tc, _topic, get_path());
  }
  return true;
}

//--------------------------------------------------------------------------------

}
