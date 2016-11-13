#include <guicomponents/computes/enterbrowsergroupcompute.h>
#include <base/objectmodel/dep.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/inputs.h>
#include <guicomponents/computes/nodejsworker.h>
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


// ----------------------------------------------------------------------------------------
// Enter.
// ----------------------------------------------------------------------------------------

EnterBrowserGroupCompute::EnterBrowserGroupCompute(Entity* entity)
    : EnterGroupCompute(entity, kDID()),
      _scheduler(this),
      _worker(this) {
  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
  get_dep_loader()->register_fixed_dep(_worker, Path({}));
}

EnterBrowserGroupCompute::~EnterBrowserGroupCompute() {
}

bool EnterBrowserGroupCompute::update_state() {
  internal();
  EnterGroupCompute::update_state();

  TaskContext tc(_scheduler);
  _worker->queue_open_browser(tc);
  std::function<void(const QJsonObject&)> callback = std::bind(&EnterBrowserGroupCompute::receive_chain_state, this, std::placeholders::_1);
  _worker->queue_receive_chain_state(tc, callback);
  return false;
}

void EnterBrowserGroupCompute::receive_chain_state(const QJsonObject& chain_state) {
  internal();
  // If the browser successfully, then we call clean_finalize directly here.
  // Note that our update_state() always returns false, so the app will continuously try to clean it until it returns true.
  // The other way to stop it is to call clean_finalize(), which marks ourself as clean, so the next cleaning wave will skip us.
  bool opened = chain_state.value("value").toBool();
  if (opened) {
    clean_finalize();
  }
}

// ----------------------------------------------------------------------------------------
// Exit.
// ----------------------------------------------------------------------------------------


ExitBrowserGroupCompute::ExitBrowserGroupCompute(Entity* entity)
    : ExitGroupCompute(entity, kDID()),
      _scheduler(this),
      _worker(this) {
  get_dep_loader()->register_fixed_dep(_scheduler, Path({}));
  get_dep_loader()->register_fixed_dep(_worker, Path({}));
}

ExitBrowserGroupCompute::~ExitBrowserGroupCompute() {
}

bool ExitBrowserGroupCompute::update_state() {
  TaskContext tc(_scheduler);
  _worker->queue_close_browser(tc);
  std::function<void(const QJsonObject&)> callback = std::bind(&ExitBrowserGroupCompute::receive_chain_state, this, std::placeholders::_1);
  _worker->queue_receive_chain_state(tc, callback);
  return false;
}

void ExitBrowserGroupCompute::receive_chain_state(const QJsonObject& chain_state) {
  bool closed = chain_state.value("value").toBool();
  if (closed) {
    clean_finalize();
  }
}

}
