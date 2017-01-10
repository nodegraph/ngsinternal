#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <components/computes/compute.h>

#include <QtCore/QTimer>
#include <QtCore/QObject>

namespace ngs {

class Entity;
class InputCompute;

class GUICOMPUTES_EXPORT WaitNodeCompute: public QObject, public Compute {
  Q_OBJECT
 public:
  COMPONENT_ID(Compute, WaitNodeCompute);
  WaitNodeCompute(Entity* entity);
  virtual ~WaitNodeCompute();

  // Input and Outputs.
  virtual void create_inputs_outputs(const EntityConfig& config = EntityConfig());

  // Our hints.
  static QJsonObject init_hints();
  static const QJsonObject _hints;
  virtual const QJsonObject& get_hints() const {return _hints;}

  private slots:
   void on_done_wait();

protected:
  // Our state.
  virtual bool update_state();
  virtual void set_self_dirty(bool dirty);

  bool _restart_timer;
  QTimer _wait_timer;
};


}
