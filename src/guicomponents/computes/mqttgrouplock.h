#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/computes/grouplock.h>
#include <guicomponents/comms/mqttworker.h>

#include <QtNetwork/QHostAddress>
#include <QtCore/QString>

namespace ngs {

class TaskScheduler;
class MQTTWorker;
class Inputs;

class GUICOMPUTES_EXPORT MQTTGroupLock: public GroupLock {
 public:
  COMPONENT_ID(Compute, MQTTGroupLock);
  MQTTGroupLock(Entity* entity);
  virtual ~MQTTGroupLock();

  // Inputs and Outputs.
  virtual void create_inputs_outputs();

  // Hints.
  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

  // Settings.
  virtual bool get_lock_setting() const;
  virtual void set_lock_setting(bool lock);

 protected:
  virtual bool update_state();

 private:
  struct InputValues {
    QHostAddress host_address;
    int port;
    QString username;
    QString password;
  };

  MQTTWorker::Config MQTTGroupLock::get_inputs() const;

  Dep<TaskScheduler> _scheduler;
  Dep<MQTTWorker> _worker;

  bool _lock;
};

}
