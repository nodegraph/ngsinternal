#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>
#include <base/utils/path.h>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QMap>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include <qmqtt/qmqtt.h>

#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>

#include <unordered_map>

namespace ngs {

class Message;
class TaskScheduler;
class TaskContext;
class Compute;
class MQTTSubscribeCompute;
class BaseFactory;
class BaseNodeGraphManipulator;
class BaseMQTTCompute;

// This class implements the computes of the MQTT related nodes.
class COMMS_EXPORT MQTTWorker : public QObject, public Component {
Q_OBJECT
 public:
  struct Config {
    QHostAddress host_address;
    quint16 port;
    QString username;
    QString password;
  };



  COMPONENT_ID(MQTTWorker, MQTTWorker)
  MQTTWorker(Entity* parent);
  virtual ~MQTTWorker();

  bool is_connected(const Config& config);
  bool is_subscribed(const Compute* compute, const std::string& topic) const;

  // Register the group lock so that it can get dirtied if the connection is disconnected.
  void register_group_lock(const Config& config, const Path& path);
  void unregister_group_lock(const Path& path);

  // Helpers which call multiple queue methods internally.
  void dive_into_lockable_group(const std::string& child_group_name, const Config& config);
//  void clean_lockable_group(const std::string& child_group_name, const QHostAddress& host_address, const quint16 port,
//                            const QString& username, const QString& password);

  // Queue task to perform at a later time.
  void queue_connect_task(TaskContext& tc, const Config& config);
  void queue_publish_task(TaskContext& tc, const QString& topic, const QString& message);
  void queue_subscribe_task(TaskContext& tc, const QString& topic, const Path& node_path);
  void queue_unsubscribe_task(TaskContext& tc, const QString& topic);

  // Group Logic.
  void queue_dive_into_lockable_group(TaskContext& tc, const std::string& child_group_name);
  void queue_surface_from_lockable_group(TaskContext& tc);

//  void queue_clean_lockable_group(TaskContext& tc, const std::string& child_group_name);

  // Compute Logic.
  void queue_finished_compute(TaskContext& tc, BaseMQTTCompute* compute);

 private slots:
  void on_time_out();
  void on_connected();
  void on_disconnected();
  void on_published(const QMQTT::Message& message);
  void on_subscribed(const QString& topic);
  void on_unsubscribed(const QString& topic);
  void on_received(const QMQTT::Message& message);

 private:

  // Client Management.
  std::string get_key(const Config& config);

  QMQTT::Client* get_client(const Config& config);

  // Task which perform the work directly.
  void connect_task(const Config& config);
  void publish_task(const QString& topic, const QString& message);
  void subscribe_task(const QString& topic, const Path& node_path);
  void unsubscribe_task(const QString& topic);

  // Group Logic.
  void dive_into_lockable_group_task(const std::string& child_group_name);
  void surface_from_lockable_group_task();
//  void clean_lockable_group_task(const std::string& child_group_name);

  // Compute Logic.
  void finished_compute_task(BaseMQTTCompute* compute);

  // Our fixed dependencies.
  Dep<TaskScheduler> _scheduler;
  Dep<BaseNodeGraphManipulator> _manipulator;

  // List of computes listening to changes on certain topics.
  // The topic is the key.
  // The value is the path to the entity which has a MQTTSubscribeCompute.
  // Entities should remove their path when from this list when destroyed.
  std::unordered_map<std::string, std::unordered_set<Path, PathHasher> > _subscribers;

  // The current client, we're working with.
  QMQTT::Client* _current_client;
  int _current_task_id;

  // Our mqtt client objects. There is one client per mqtt group node.
  // The key can be obtain by calling get_key(...).
  std::unordered_map<std::string, QMQTT::Client*> _clients;

  // Our invisible mqtt group lock nodes.
  // The key can be obtain by calling get_key(...).
  std::unordered_map<std::string, std::unordered_set<Path, PathHasher> > _group_locks;

  // Timer to timeout the connection request.
  QTimer _timer;
};

}
