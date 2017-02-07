#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>
#include <guicomponents/comms/guitypes.h>

#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/computes/mqttcomputes.h>
#include <guicomponents/computes/entermqttgroupcompute.h>

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

  EntityConfig c = config;
  c.expose_plug = true;
  c.unconnected_value = QJsonObject();

  create_input("in", c);
  create_output("out", c);
}

void BaseMQTTCompute::init_hints(QJsonObject& m) {
  add_hint(m, "in", GUITypes::HintKey::DescriptionHint, "The main object that flows through this node. This cannot be set manually.");
}

void BaseMQTTCompute::update_wires() {
  // This caches the dep, and will only dirty this component when it changes.
  _enter = find_enter_node();
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

Dep<EnterMQTTGroupCompute> BaseMQTTCompute::find_enter_node() {
  Entity* group = find_group_context();
  Entity* enter = group->get_child("enter");
  assert(enter);
  return get_dep<EnterMQTTGroupCompute>(enter);
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
  QJsonValue incoming = _inputs->get_input_value("in");
  set_output("out", incoming);
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

  QString topic = _inputs->get_input_value(Message::kTopic).toString();
  QString message = _inputs->get_input_value(Message::kMessage).toString();

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
  c.unconnected_value = "message";
  create_input(Message::kOutputPropertyName, c);
}

void MQTTSubscribeCompute::on_finished_task() {
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

  add_hint(m, Message::kTopic, GUITypes::HintKey::DescriptionHint, "The topic to subscribe to.");
  add_hint(m, Message::kOutputPropertyName, GUITypes::HintKey::DescriptionHint,
           "The name of the property to add to our output. The value of the property will be the latest message.");

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
  _topic = _inputs->get_input_value(Message::kTopic).toString();

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
