#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <components/computes/compute.h>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtNetwork/QHostAddress>
#include <QtCore/QString>

#include <qmqtt/qmqtt.h>

namespace ngs {

class TaskScheduler;
class Inputs;
class TaskContext;

class GUICOMPUTES_EXPORT MQTTHostCompute : public QObject, public Compute {
Q_OBJECT
 public:

  struct Config {
    QString host_name;
    QHostAddress host_address;
    quint16 port;
    QString username;
    QString password;
    QString client_id;
  };

  COMPONENT_ID(Compute, MQTTHostCompute);
  MQTTHostCompute(Entity* entity);
  virtual ~MQTTHostCompute();

  // Inputs and Outputs.
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  // Hints.
  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

  // Config.
  Config get_config() const;
  bool is_connected() const;
  bool is_subscribed(const Compute* compute, const std::string& topic) const;

  // Queue up tasks.
  void queue_connect_task(TaskContext& tc);
  void queue_publish_task(TaskContext& tc, const QString& topic, const QString& message);
  void queue_subscribe_task(TaskContext& tc, const QString& topic, const Path& node_path);
  void queue_unsubscribe_task(TaskContext& tc, const QString& topic, const Path& node_path);
  void queue_finished_task(TaskContext& tc, std::function<void()> done);

 protected:
  virtual bool update_state();
  virtual void on_finished_connect_task();
  virtual bool destroy_state();

  private slots:
  // Note our slots are all marked as internal(), as we don't want them to dirty this node.
  // It's actually performing some tasks for other nodes that get their results via callbacks.
  // And we're not actually getting dirtied in any way.
   void on_time_out();
   void on_connected();
   void on_disconnected();
   void on_published(const QMQTT::Message& message);
   void on_subscribed(const QString& topic);
   void on_unsubscribed(const QString& topic);
   void on_received(const QMQTT::Message& message);

 private:
  // Tasks.
  void connect_task();
  void publish_task(const QString& topic, const QString& message);
  void subscribe_task(const QString& topic, const Path& node_path);
  void unsubscribe_task(const QString& topic, const Path& node_path);

  // Finished Task.
  void finished_compute_task(std::function<void()> done);

  // Our dependencies.
  Dep<TaskScheduler> _scheduler;

  // Current task id for sanity checking.
  int _current_task_id;

  // Our MQTT client.
  QMQTT::Client* _client;

  // The current configuration of our mqtt client.
  Config _client_config;

  // List of computes listening to changes on certain topics.
  // The topic is the key.
  // The value is the path to the entity which has a MQTTSubscribeCompute.
  // Entities should remove their path when from this list when destroyed.
  std::unordered_map<std::string, std::unordered_set<Path, PathHasher> > _subscribers;

  // Timer to timeout the connection request.
  QTimer _timer;
};

}
