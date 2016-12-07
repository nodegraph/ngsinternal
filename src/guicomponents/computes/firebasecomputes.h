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

  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  virtual void receive_chain_state(const QJsonObject& chain_state);

  static void init_hints(QJsonObject& m);

 protected:
  // Our state.
  virtual void prepend_tasks(TaskContext& tc);
  virtual void append_tasks(TaskContext& tc);

  void dump_map(const QJsonObject& inputs) const;

  Dep<NodeJSWorker> _worker;
  Dep<TaskScheduler> _scheduler;
};

class GUICOMPUTES_EXPORT FirebaseWriteDataCompute: public FirebaseCompute {
 public:
  COMPONENT_ID(Compute, FirebaseWriteDataCompute);
  FirebaseWriteDataCompute(Entity* entity): FirebaseCompute(entity, kDID()){}
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

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
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

  // Our overrides.
  virtual void set_override(const QString& data_path, const QJsonValue& value);
  virtual const QJsonValue& get_override() const;
  virtual void clear_override();

 protected:
  virtual void receive_chain_state(const QJsonObject& chain_state);
  virtual bool update_state();
  virtual bool destroy_state();

 private:
  // This override value is not really used, because when we clean/update_state the current value
  // is always retrieved directly. However when the firebase database changes our set_override()
  // method gets called and sets this value.
  QJsonValue _dummy_override;

  // Cached value of our data_path.
  QString _data_path;
};

}
