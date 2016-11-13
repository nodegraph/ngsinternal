#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <guicomponents/comms/message.h>
#include <components/computes/compute.h>
#include <base/objectmodel/dep.h>

namespace ngs {

class Entity;
class NodeJSWorker;
class TaskScheduler;
class TaskContext;
class EnterFirebaseGroupCompute;

class GUICOMPUTES_EXPORT FirebaseCompute: public Compute {
 public:
  COMPONENT_ID(Compute, InvalidComponent);
  FirebaseCompute(Entity* entity, ComponentDID did);
  virtual ~FirebaseCompute();

  virtual void create_inputs_outputs();

  virtual void receive_chain_state(const QJsonObject& chain_state);

  static void init_hints(QJsonObject& m);

 protected:
  // Our state.
  virtual void pre_update_state(TaskContext& tc);
  virtual void post_update_state(TaskContext& tc);

  void dump_map(const QJsonObject& inputs) const;

  Dep<NodeJSWorker> _worker;
  Dep<TaskScheduler> _scheduler;
  // This enter dep makes sure the group context node computes before us.
  Dep<EnterFirebaseGroupCompute> _enter;
};

class GUICOMPUTES_EXPORT FirebaseSignInCompute: public FirebaseCompute {
 public:
  COMPONENT_ID(Compute, FirebaseSignInCompute);
  FirebaseSignInCompute(Entity* entity): FirebaseCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT FirebaseSignOutCompute: public FirebaseCompute {
 public:
  COMPONENT_ID(Compute, FirebaseSignOutCompute);
  FirebaseSignOutCompute(Entity* entity): FirebaseCompute(entity, kDID()){}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT FirebaseWriteDataCompute: public FirebaseCompute {
 public:
  COMPONENT_ID(Compute, FirebaseWriteDataCompute);
  FirebaseWriteDataCompute(Entity* entity): FirebaseCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT FirebaseReadDataCompute: public FirebaseCompute {
 public:
  COMPONENT_ID(Compute, FirebaseReadDataCompute);
  FirebaseReadDataCompute(Entity* entity): FirebaseCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

  virtual void receive_chain_state(const QJsonObject& chain_state);
 protected:
  virtual bool update_state();
};

class GUICOMPUTES_EXPORT FirebaseListenToChangesCompute: public FirebaseCompute {
 public:
  COMPONENT_ID(Compute, FirebaseListenToChangesCompute);
  FirebaseListenToChangesCompute(Entity* entity): FirebaseCompute(entity, kDID()){}
  virtual void create_inputs_outputs();

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}
 protected:
  virtual bool update_state();
};

}
