#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/computes/grouplock.h>

#include <QtCore/QString>


class QJsonObject;

namespace ngs {

class TaskScheduler;
class BrowserWorker;


class GUICOMPUTES_EXPORT BrowserGroupLock: public GroupLock {
 public:

  struct InputValues {
    QString api_key;
    QString auth_domain;
    QString database_url;
    QString storage_bucket;
    QString email;
    QString password;
  };


  COMPONENT_ID(Compute, BrowserGroupLock);
  BrowserGroupLock(Entity* entity);
  virtual ~BrowserGroupLock();

  virtual bool get_lock_setting() const;
  virtual void set_lock_setting(bool lock);

 protected:
  virtual bool update_state();

 private:
  InputValues get_inputs() const;
  void queue_unlock();
  void receive_chain_state(const QJsonObject& chain_state);

  Dep<TaskScheduler> _scheduler;
  Dep<BrowserWorker> _worker;

  bool _lock; // state desired
  bool _up_to_date; // current state

};

}
