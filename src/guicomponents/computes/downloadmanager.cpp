#include <guicomponents/computes/downloadmanager.h>
#include <base/objectmodel/deploader.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/computes/downloadvideoprocess.h>
#include <guicomponents/comms/licensechecker.h>
#include <guicomponents/comms/filemodel.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <QtCore/QStandardPaths>
#include <QtCore/QDir>

namespace ngs {


const int DownloadManager::kCheckInterval = 1000;

DownloadManager::DownloadManager(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _manipulator(this),
      _license_checker(this),
      _file_model(this),
      _last_msg_id(-1) {
  get_dep_loader()->register_fixed_dep(_manipulator, Path());
  get_dep_loader()->register_fixed_dep(_license_checker, Path());
  get_dep_loader()->register_fixed_dep(_file_model, Path());

  // Setup the poll timer.
  _check_timer.setSingleShot(false);
  _check_timer.setInterval(kCheckInterval);
  connect(&_check_timer,SIGNAL(timeout()),this,SLOT(on_check()));
  _check_timer.start();
}

DownloadManager::~DownloadManager() {
  for (auto &p: _processes) {
    delete_ff(p.second);
  }
  _processes.clear();
}

void DownloadManager::on_check() {
  // Determine the maximum number of running processes allowed.
  int max_running = 2;
  if (_license_checker->get_edition() == "pro") {
    // Get the number of concurrent downloads from the settings.
    max_running = _file_model->get_max_concurrent_downloads();
  }
  // If the number of running processes is less than the max, then start another.
  if (get_num_running() < max_running) {
    for (auto &i: _processes) {
      if (!i.second->is_running()) {
        i.second->start();
        break;
      }
    }
  }
}

int DownloadManager::get_num_running() const {
  int count = 0;
  for (auto &i: _processes) {
    if (i.second->is_running()) {
      count += 1;
    }
  }
  return count;
}

void DownloadManager::download_on_the_side(const QString& url) {
  DownloadVideoProcess *p = new_ff DownloadVideoProcess();

  // Connect to signals.
  connect(p, SIGNAL(started(long long, const QString&)), this, SLOT(on_started(long long, const QString&)));
  connect(p, SIGNAL(progress(long long, const  QString&)), this, SLOT(on_progress(long long, const  QString&)));
  connect(p, SIGNAL(finished(long long)), this, SLOT(on_finished(long long)));
  connect(p, SIGNAL(errored(long long, const QString&)), this, SLOT(on_errored(long long, const QString&)));

  // Setup the arguments.
  p->set_url(url);
  QDir dir(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + QDir::separator() + "smashbrowse");
  p->set_dir(dir.path());

  // Queue the process.
  _processes.insert({p->get_id(),p});
  // Emit queued signal.
  emit download_queued(p->get_id(), url);
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

  // Queue the process.
  _processes.insert({p->get_id(),p});

  // Emit queued signal.
  emit download_queued(p->get_id(), args[Message::kURL].toString());
}

void DownloadManager::stop(long long id) {
  if (_processes.count(id) == 0) {
    return;
  }
  destroy_process(id);
}

void DownloadManager::destroy_process(long long id) {
  DownloadVideoProcess *p = _processes[id];
  _processes.erase(id);
  // Let Qt destroy this when convenient.
  p->deleteLater();

  // The following delete causes a crash.
  //delete_ff(p);
}

void DownloadManager::on_queued_side_download(long long id, const QString& url) {
  emit download_queued(id, url);
}

void DownloadManager::on_queued(long long id, const QString& url) {
  internal();

  // Once queued we consider the task to be done. We don't wait for it to finish.
  Message response(true, url);
  response.insert(Message::kID, _last_msg_id);
  _manipulator->receive_message(response.to_string());

  emit download_queued(id, url);
}

void DownloadManager::on_started(long long id, const QString& filename) {
  internal();
  emit download_started(id, filename);
}

void DownloadManager::on_progress(long long id, const  QString& progress) {
  emit download_progress(id, progress);
}

void DownloadManager::on_errored(long long id, const QString& error) {
  destroy_process(id);
  emit download_errored(id, error);
}

void DownloadManager::on_finished(long long id) {
  destroy_process(id);
  emit download_finished(id);
}

}
