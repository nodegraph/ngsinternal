#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/computes/entergroupcompute.h>
#include <guicomponents/computes/mqttworker.h>

#include <QtNetwork/QHostAddress>
#include <QtCore/QString>

namespace ngs {

class TaskScheduler;
class MQTTWorker;
class Inputs;

class GUICOMPUTES_EXPORT EnterMQTTGroupCompute: public EnterGroupCompute {
 public:
  COMPONENT_ID(Compute, EnterMQTTGroupCompute);
  EnterMQTTGroupCompute(Entity* entity);
  virtual ~EnterMQTTGroupCompute();

  // Inputs and Outputs.
  virtual void create_inputs_outputs();

  // Hints.
  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

 protected:
  virtual bool update_state();
  virtual void on_finished_connect_task();

 private:
  struct InputValues {
    QHostAddress host_address;
    int port;
    QString username;
    QString password;
  };

  MQTTWorker::Config EnterMQTTGroupCompute::get_inputs() const;

  Dep<TaskScheduler> _scheduler;
  Dep<MQTTWorker> _worker;
};

}
