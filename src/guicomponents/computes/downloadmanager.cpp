#include <guicomponents/computes/downloadmanager.h>
#include <base/objectmodel/deploader.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/computes/downloadvideoprocess.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <QtCore/QStandardPaths>
#include <QtCore/QDir>

namespace ngs {

DownloadManager::DownloadManager(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _manipulator(this),
      _last_msg_id(-1) {
  get_dep_loader()->register_fixed_dep(_manipulator, Path());
}

DownloadManager::~DownloadManager() {
  for (auto &p: _processes) {
    delete_ff(p.second);
  }
  _processes.clear();
}

void DownloadManager::download_on_the_side(const QString& url) {
  DownloadVideoProcess *p = new_ff DownloadVideoProcess();

  // Connect to signals.
  connect(p, SIGNAL(queued(long long, const QString&)), this, SLOT(on_queued_side_download(long long, const QString&)));
  connect(p, SIGNAL(started(long long, const QString&)), this, SLOT(on_started(long long, const QString&)));
  connect(p, SIGNAL(progress(long long, const  QString&)), this, SLOT(on_progress(long long, const  QString&)));
  connect(p, SIGNAL(finished(long long)), this, SLOT(on_finished(long long)));
  connect(p, SIGNAL(errored(long long, const QString&)), this, SLOT(on_errored(long long, const QString&)));

  // Setup the arguments.
  p->set_url(url);
  QDir dir(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + QDir::separator() + "smashbrowse");
  p->set_dir(dir.path());

  // Start the process.
  p->start();

  // Record the process.
  _processes.insert({p->get_pid(),p});
}

void DownloadManager::download(int msg_id, const QJsonObject& args) {
  _last_msg_id = msg_id;
  DownloadVideoProcess *p = new_ff DownloadVideoProcess();

  // Connect to signals.
  connect(p, SIGNAL(queued(long long, const QString&)), this, SLOT(on_queued(long long, const QString&)));
  connect(p, SIGNAL(started(long long, const QString&)), this, SLOT(on_started(long long, const QString&)));
  connect(p, SIGNAL(progress(long long, const  QString&)), this, SLOT(on_progress(long long, const  QString&)));
  connect(p, SIGNAL(finished(long long)), this, SLOT(on_finished(long long)));
  connect(p, SIGNAL(errored(long long, const QString&)), this, SLOT(on_errored(long long, const QString&)));

  // Setup the arguments.
  p->set_url(args[Message::kURL].toString());
  p->set_dir(args[Message::kDirectory].toString());
  p->set_max_width(args[Message::kMaxWidth].toInt());
  p->set_max_height(args[Message::kMaxHeight].toInt());
  p->set_max_filesize(args[Message::kMaxFilesize].toInt());

  // Start the process.
  p->start();

  // Record the process.
  _processes.insert({p->get_pid(),p});
}

void DownloadManager::stop(long long pid) {
  if (_processes.count(pid) == 0) {
    return;
  }
  destroy_process(pid);
}

void DownloadManager::destroy_process(long long pid) {
  DownloadVideoProcess *p = _processes[pid];
  _processes.erase(pid);
  // Let Qt destroy this when convenient.
  p->deleteLater();

  // The following delete causes a crash.
  //delete_ff(p);
}

void DownloadManager::on_queued_side_download(long long pid, const QString& url) {
  emit download_queued(pid, url);
}

void DownloadManager::on_queued(long long pid, const QString& url) {
  internal();

  // Once queued we consider the task to be done. We don't wait for it to finish.
  Message response(true, url);
  response.insert(Message::kID, _last_msg_id);
  _manipulator->receive_message(response.to_string());

  emit download_queued(pid, url);
}

void DownloadManager::on_started(long long pid, const QString& filename) {
  internal();
  emit download_started(pid, filename);
}

void DownloadManager::on_progress(long long pid, const  QString& progress) {
  emit download_progress(pid, progress);
}

void DownloadManager::on_errored(long long pid, const QString& error) {
  destroy_process(pid);
  emit download_errored(pid, error);
}

void DownloadManager::on_finished(long long pid) {
  destroy_process(pid);
  emit download_finished(pid);
}

}
