#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/computes/grouplock.h>

class QHostAddress;
class QString;

namespace ngs {

class TaskScheduler;
class BrowserWorker;
class Inputs;

struct InputValues {
  QString apiKey, authDomain, databaseURL, storageBucket, email, password;
};

class GUICOMPUTES_EXPORT FirebaseGroupLock: public GroupLock {
 public:
  COMPONENT_ID(GroupLock, FirebaseGroupLock);
  FirebaseGroupLock(Entity* entity);
  virtual ~FirebaseGroupLock();

  virtual bool get_lock_setting() const;
  virtual void set_lock_setting(bool lock);

 protected:
  virtual bool update_state();

 private:
  void get_inputs(InputValues& values) const;

  Dep<TaskScheduler> _scheduler;
  Dep<BrowserWorker> _worker;

  bool _lock;

};

}
