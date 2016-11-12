#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/computes/entergroupcompute.h>

#include <QtCore/QString>


class QJsonObject;

namespace ngs {

class TaskScheduler;
class BrowserWorker;


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

  virtual bool get_lock_setting() const;
  virtual void set_lock_setting(bool lock);

  // Override.
  virtual void set_self_dirty(bool dirty);

 protected:
  virtual bool update_state();

 private:
  InputValues get_inputs() const;
  void queue_unlock();
  void receive_chain_state(const QJsonObject& chain_state);

  Dep<TaskScheduler> _scheduler;
  Dep<BrowserWorker> _worker;

  bool _lock; // state desired
  bool _current; // current state

};

}
