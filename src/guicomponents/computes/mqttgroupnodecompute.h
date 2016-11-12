//#pragma once
//#include <guicomponents/computes/guicomputes_export.h>
//#include <components/computes/groupnodecompute.h>
//#include <entities/componentids.h>
//#include <guicomponents/comms/message.h>
//#include <guicomponents/comms/mqttworker.h>
//
//// QT
//#include <QtCore/QObject>
//#include <QtCore/QVariant>
//#include <QtNetwork/QHostAddress>
//
//namespace ngs {
//
//class BaseGroupTraits;
//class TaskScheduler;
//
//class GUICOMPUTES_EXPORT MQTTGroupNodeCompute: public GroupNodeCompute {
// public:
//  COMPONENT_ID(Compute, MQTTGroupNodeCompute);
//  MQTTGroupNodeCompute(Entity* entity);
//  virtual ~MQTTGroupNodeCompute();
//
//  MQTTWorker::Config get_inputs() const;
//
//  // Inputs and Outputs.
//  virtual void create_inputs_outputs();
//
//  // Hints.
//  static QJsonObject init_hints();
//  static const QJsonObject _hints;
//  virtual const QJsonObject& get_hints() const {return _hints;}
//
//  // Group Unlocking.
//  virtual bool group_is_unlocked() const;
//  virtual void unlock_group(); // Starts an synchronous task.
//  virtual void unlock_group_and_dive(); // Starts an synchronous task.
//  virtual void lock_group(); // Starts an synchronous task.
//  virtual void lock_group_and_surface(); // Starts an synchronous task.
//
// protected:
//  // Group Unlocking.
//  virtual bool update_unlocked_group();
//
//  // Our state.
//  virtual bool update_state();
//
// private:
//  Dep<TaskScheduler> _scheduler;
//  Dep<MQTTWorker> _worker;
//  bool _unlocked;
//};
//
//}
