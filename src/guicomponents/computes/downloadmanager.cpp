#include <guicomponents/computes/downloadmanager.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/appconfig.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/computes/downloadvideoprocess.h>
#include <guicomponents/comms/licensechecker.h>
#include <guicomponents/comms/filemodel.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

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
  _poll_timer.setSingleShot(false);
  _poll_timer.setInterval(kCheckInterval);
  connect(&_poll_timer,SIGNAL(timeout()),this,SLOT(on_poll()));
  _poll_timer.start();
}

DownloadManager::~DownloadManager() {
  for (auto &p: _processes) {
    delete_ff(p.second);
  }
  _processes.clear();
}

void DownloadManager::on_poll() {
  // If the number of running processes is less than the max, then start another.
  int max = _file_model->get_max_concurrent_downloads();
  if (get_num_running() < max) {
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
  connect(p, SIGNAL(started(const QString&)), this, SLOT(on_started(const QString&)));
  connect(p, SIGNAL(progress(const  QString&)), this, SLOT(on_progress(const  QString&)));
  connect(p, SIGNAL(finished()), this, SLOT(on_finished()));
  connect(p, SIGNAL(errored(const QString&)), this, SLOT(on_errored(const QString&)));

  // Setup the arguments.
  p->set_url(url);
  QString dir = _file_model->get_default_download_dir();
  p->set_dir(dir);

  // Queue the process.
  _processes.insert({p->get_id(),p});
  // Emit queued signal.
  emit download_queued(p->get_id(), url, dir);
}

void DownloadManager::download(int msg_id, const QJsonObject& args) {
  _last_msg_id = msg_id;
  DownloadVideoProcess *p = new_ff DownloadVideoProcess();

  // Connect to signals.
  connect(p, SIGNAL(started(const QString&)), this, SLOT(on_started(const QString&)));
  connect(p, SIGNAL(progress(const  QString&)), this, SLOT(on_progress(const  QString&)));
  connect(p, SIGNAL(finished()), this, SLOT(on_finished()));
  connect(p, SIGNAL(errored(const QString&)), this, SLOT(on_errored(const QString&)));

  // Setup the arguments.
  QString path = args[Message::kDirectory].toString();

  // Combine the directory parameter with the default donwload directory.
  QDir dir(path);
  if (dir.isRelative()) {
    path = _file_model->get_default_download_dir() + QDir::separator() + dir.path();
  } else {
    path = dir.path();
  }

  // Setup the download process.
  p->set_dir(path);
  p->set_url(args[Message::kURL].toString());
  p->set_max_width(args[Message::kMaxWidth].toInt());
  p->set_max_height(args[Message::kMaxHeight].toInt());
  p->set_max_filesize(args[Message::kMaxFilesize].toInt());

  // Queue the process.
  _processes.insert({p->get_id(),p});

  // Emit queued signal.
  emit download_queued(p->get_id(), args[Message::kURL].toString(), path);
}

void DownloadManager::stop(long long id) {
  if (_processes.count(id) == 0) {
    return;
  }
  destroy_process(id);
}

long long DownloadManager::get_sender_id() {
  QObject* obj = sender();
  DownloadVideoProcess *p = static_cast<DownloadVideoProcess*>(obj);
  return p->get_id();
}

void DownloadManager::destroy_process(long long id) {
  DownloadVideoProcess *p = _processes[id];
  _processes.erase(id);
  // Let Qt destroy this when convenient.
  p->deleteLater();

  // The following delete causes a crash.
  //delete_ff(p);
}

void DownloadManager::on_started(const QString& filename) {
  internal();
  long long id = get_sender_id();
  emit download_started(id, filename);
}

void DownloadManager::on_progress(const  QString& progress) {
  internal();
  long long id = get_sender_id();
  emit download_progress(id, progress);
}

void DownloadManager::on_errored(const QString& error) {
  internal();
  long long id = get_sender_id();
  destroy_process(id);
  emit download_errored(id, error);
}

void DownloadManager::on_finished() {
  internal();
  long long id = get_sender_id();
  destroy_process(id);
  emit download_finished(id);
}

void DownloadManager::reveal_file(const QString& dir, const QString &filename) {
  DownloadManager::reveal_file_on_platform(dir, filename);
}

QString DownloadManager::find_best_matching_file(const QString& dir, const QString& filename) {
  // Find the base filename, which is the part up to the first period.
  QFileInfo info(filename);
  QString base_name = info.baseName();

  // Create our name filters.
  QStringList name_filters;
  name_filters.append(base_name+".*");

  // Grab the files matching the name filters.
  QDir d(dir);
  QFileInfoList files = d.entryInfoList(name_filters, QDir::Files);

  // Find the file with the largest byte size.
  qint64 largest = 0;
  QString largest_file = "";
  for (QFileInfo &info : files) {
    info.size();
    if (info.size() > largest) {
      largest = info.size();
      largest_file = info.fileName();
    }
  }
  return largest_file;
}

void DownloadManager::reveal_file_on_platform(const QString& dir, const QString &similar_filename) {
  if (dir.isEmpty()) {
    return;
  }
  QString best_filename = find_best_matching_file(dir, similar_filename);

#if (ARCH == ARCH_WINDOWS)
  QString cmd = "explorer.exe";
  QStringList args;
  if (!best_filename.isEmpty()) {
    args.append("/select,");
  }
  args.append(QDir::toNativeSeparators(dir) + QDir::separator() + best_filename);
  QProcess::startDetached(cmd, args);
#elif (ARCH == ARCH_MACOS)
  {
    QStringList args;
    args.append("-e");
    QString cmd = "tell application \"Finder\" to reveal POSIX file \"" + dir + "/" + best_filename + "\"";
    args.append(cmd);
    QProcess::execute("/usr/bin/osascript", args);
  }
  {
    QStringList args;
    args.append("-e");
    QString cmd = "tell application \"Finder\" to activate";
    args.append(cmd);
    QProcess::execute("/usr/bin/osascript", args);
  }
#endif

}

}