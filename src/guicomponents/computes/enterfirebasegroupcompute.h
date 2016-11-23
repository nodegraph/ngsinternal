#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/computes/entergroupcompute.h>

#include <QtCore/QString>


class QJsonObject;

namespace ngs {

class TaskScheduler;
class NodeJSWorker;


class GUICOMPUTES_EXPORT EnterFirebaseGroupCompute: public EnterGroupCompute {
 public:

  struct InputValues {
    QString api_key;
    QString auth_domain;
    QString database_url;
    QString storage_bucket;
    QString email;
    QString password;
  };


  COMPONENT_ID(Compute, EnterFirebaseGroupCompute);
  EnterFirebaseGroupCompute(Entity* entity);
  virtual ~EnterFirebaseGroupCompute();

  // Inputs and Outputs.
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  // Hints.
  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const { return _hints; }

 protected:
  virtual bool update_state();
  virtual bool destroy_state();

 private:
  InputValues get_inputs() const;
  void queue_sign_in();
  void queue_destroy();
  void receive_chain_state(const QJsonObject& chain_state);

  Dep<TaskScheduler> _scheduler;
  Dep<NodeJSWorker> _worker;
};

}
