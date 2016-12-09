#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/computes/entergroupcompute.h>

#include <QtCore/QString>


class QJsonObject;

namespace ngs {

class TaskScheduler;
class NodeJSWorker;


class GUICOMPUTES_EXPORT EnterBrowserGroupCompute: public EnterGroupCompute {
 public:

  struct InputValues {
    QString api_key;
    QString auth_domain;
    QString database_url;
    QString storage_bucket;
    QString email;
    QString password;
  };


  COMPONENT_ID(Compute, EnterBrowserGroupCompute);
  EnterBrowserGroupCompute(Entity* entity);
  virtual ~EnterBrowserGroupCompute();

  virtual bool update_state();

 private:
  InputValues get_inputs() const;
  void queue_unlock();
  void receive_chain_state(const QJsonObject& chain_state);

  Dep<TaskScheduler> _scheduler;
  Dep<NodeJSWorker> _worker;
};

class GUICOMPUTES_EXPORT ExitBrowserGroupCompute: public ExitGroupCompute {
 public:
  COMPONENT_ID(Compute, ExitBrowserGroupCompute);
  ExitBrowserGroupCompute(Entity* entity);
  virtual ~ExitBrowserGroupCompute();

  virtual bool update_state();

 private:
  void queue_unlock();
  void receive_chain_state(const QJsonObject& chain_state);

  Dep<TaskScheduler> _scheduler;
  Dep<NodeJSWorker> _worker;
};

}
