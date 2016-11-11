#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/computes/grouplock.h>

class QHostAddress;
class QString;

namespace ngs {

class TaskScheduler;
class MQTTWorker;
class Inputs;

class GUICOMPUTES_EXPORT MQTTGroupLock: public GroupLock {
 public:
  COMPONENT_ID(GroupLock, MQTTGroupLock);
  MQTTGroupLock(Entity* entity);
  virtual ~MQTTGroupLock();

  virtual bool get_lock_setting() const;
  virtual void set_lock_setting(bool lock);

 protected:
  virtual bool update_state();

 private:
  void get_inputs(QHostAddress& host_address, int& port, QString& username, QString& password) const;

  Dep<TaskScheduler> _scheduler;
  Dep<MQTTWorker> _worker;

  bool _lock;

};

}
