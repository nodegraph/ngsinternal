#include <guicomponents/computes/enterbrowsergroupcompute.h>
#include <base/objectmodel/dep.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/inputs.h>
#include <guicomponents/comms/browserworker.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/message.h>

#include <QtNetwork/QHostAddress>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QMap>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>


namespace ngs {

EnterBrowserGroupCompute::EnterBrowserGroupCompute(Entity* entity)
    : EnterGroupCompute(entity, kDID()),
      _scheduler(this),
      _worker(this),
      _lock(true),
      _up_to_date(true){
  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
  get_dep_loader()->register_fixed_dep(_worker, Path({}));
}

EnterBrowserGroupCompute::~EnterBrowserGroupCompute() {
}

bool EnterBrowserGroupCompute::get_lock_setting() const{
  external();
  return _lock;
}

void EnterBrowserGroupCompute::set_lock_setting(bool lock) {
  external();
  _lock = lock;
}

bool EnterBrowserGroupCompute::update_state() {
  internal();
  if (!_lock) {
    TaskContext tc(_scheduler);
    _worker->queue_open_browser(tc);
    std::function<void(const QJsonObject&)> callback = std::bind(&EnterBrowserGroupCompute::receive_chain_state, this, std::placeholders::_1);
    _worker->queue_receive_chain_state(tc, callback);
    return false;
  } else {
    TaskContext tc(_scheduler);
    _worker->queue_close_browser(tc);
    std::function<void(const QJsonObject&)> callback = std::bind(&EnterBrowserGroupCompute::receive_chain_state, this, std::placeholders::_1);
    _worker->queue_receive_chain_state(tc, callback);
    return false;
  }
}

void EnterBrowserGroupCompute::receive_chain_state(const QJsonObject& chain_state) {
  _up_to_date = chain_state.value("value").toBool();
}


}
